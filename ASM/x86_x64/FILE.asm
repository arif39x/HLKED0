; #########################################################################

        .code

; #########################################################################

Sender proc lpMailMessage: DWORD

        invoke  StreamToFile,offset szSFileName
        mov     eax, 1
        ret

Sender endp
