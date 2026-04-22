	lpRecvBufLen    equ 100h        ; Size of lpBuf1 (in bytes)

        ;=============
        ; Data
        ;=============

        .data
                szMailData      db "DATA",13,10,0
                szMailHelo      db "EHLO localhost",13,10,0
                szMailEnd       db 13,10,".",13,10,0
            
        .code

; #########################################################################

Recv_DataLF proc uses edi
        lea     edi, lpBuf1
        mov     ecx, lpRecvBufLen
@loop:
        jecxz   @err ; Check for buffer overlow

        push    ecx
	push    ecx
        invoke  recv, esi, edi, ecx, 0
        pop     ecx
pop     ecx

        ; Error occured
        cmp     eax, SOCKET_ERROR
        je      @err

        add     edi, eax ; Move the poiter
        sub     ecx, eax ; Decrease remaining buffer size


        ; Fix edi-1 problems when connection was closed from the first
        ; call of recv procedure (eg. wasn't received any data)
        cmp     ecx, lpRecvBufLen
        jz      @test

        ; Note: recv always receives at least 1 byte if no error occures
        ; or connection hasn't been dropped
        cmp     byte ptr[edi-1], 10
        je      @ok

@test:
        ; If connection was closed do not try to receive more data
        test    eax, eax        
        jz      @err

        jmp     @loop
@err:
        xor     eax, eax
        jmp     @end
@ok:    
        ; Check the received status
        cmp     byte ptr [lpBuf1], '2'; 2** - OK
        jz      @F
        cmp     dword ptr [lpBuf1], ' 453' ; Accept 354: Start mail input;
        jnz     @err
@@:
        inc     eax ; Result = True
        inc     edi
        mov     byte ptr[edi], 0 ; Write the null terminator
	
@end:

        ret        
Recv_DataLF endp

; #########################################################################

Send_String proc lpStr: DWORD
        ; Get length of the buffer to send to
        invoke  lstrlen, lpStr
        ; In blocking sockets send() blocks until all data hasn't been sent
        ; or error happened (in this case SOCKET_ERROR is returned)
        invoke  send, esi, lpStr, eax, 0
        ret
Send_String endp

; #########################################################################

Sender proc uses edi esi lpMailMessage: DWORD
        LOCAL   sockAddr: sockaddr_in

        xor     edi, edi

        invoke  socket, AF_INET, SOCK_STREAM, IPPROTO_TCP
        cmp     eax, INVALID_SOCKET
        je      @end
        mov     esi, eax
	
        ; Fill in the sockaddr_in structure
        lea     edx, sockAddr
        mov     ecx, [MailPortNumber]
        xchg    cl, ch  ; convert to network byte order
    
        mov     [edx][sockaddr_in.sin_family], AF_INET  
        mov     [edx][sockaddr_in.sin_port], cx
        mov     [edx][sockaddr_in.sin_addr.S_un.S_addr], szMailHost

        ; Connect to server
        invoke  connect, esi, addr sockAddr, sizeof sockAddr
        test    eax, eax
        jnz     @cleanup

        ; Receive server's greet message
        invoke  Recv_DataLF
        test    eax, eax
        jz      @cleanup

        ; MAIL FROM:
        invoke  Send_String, offset szMailHelo
        inc     eax
        jz      @cleanup

        ; Receive response
        invoke  Recv_DataLF
        test    eax, eax
        jz      @cleanup

        ; MAIL FROM:
        invoke  Send_String, offset szMailFrom
        inc     eax
        jz      @cleanup

        ; Receive response
        invoke  Recv_DataLF
        test    eax, eax
        jz      @cleanup

        ; RCPT TO:
        invoke  Send_String, offset szMailRecepient
        inc     eax
        jz      @cleanup

        ; Receive response
        invoke  Recv_DataLF
        test    eax, eax
        jz      @cleanup

        ; Data
        invoke  Send_String, addr szMailData
        inc     eax
        jz      @cleanup

        ; Receive response (code 354: Start mail input)
        invoke  Recv_DataLF
        test    eax, eax
        jz      @cleanup

        ; Format headers (subject only)
        invoke  gethostname, offset lpBuf3, MAXGETHOSTSTRUCT
        invoke  wsprintf, offset lpBuf4, offset szMailHeaders, offset lpBuf3
        invoke  Send_String, offset lpBuf4
        test    eax, eax
        jz      @cleanup

        ; Message
        invoke  Send_String, lpMailMessage
        inc     eax
        jz      @cleanup

        ; .
        invoke  Send_String, addr szMailEnd
        inc     eax
        jz      @cleanup

        ; Wait untill final message received
        invoke  Recv_DataLF
        test    eax, eax
        jz      @cleanup

        or      edi, 1

@cleanup:
        invoke  closesocket, esi

@end:
	mov     eax, edi

        ret
Sender endp
