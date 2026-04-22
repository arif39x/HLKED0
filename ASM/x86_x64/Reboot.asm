.data 
szShut db "SeShutdownPrivilege",0 
.code 
Reboot proc
invoke GetCurrentProcess 
invoke OpenProcessToken,eax,TOKEN_ADJUST_PRIVILEGES+TOKEN_QUERY,ADDR hToken 
invoke LookupPrivilegeValue,NULL,ADDR szShut,addr tkp.Privileges[0].Luid 
mov tkp.PrivilegeCount,1 
mov tkp.Privileges[0].Attributes,SE_PRIVILEGE_ENABLED 
invoke AdjustTokenPrivileges,hToken,FALSE, ADDR tkp, 0, NULL, 0 
invoke ExitWindowsEx,EWX_REBOOT,NULL 
ret 
Reboot endp