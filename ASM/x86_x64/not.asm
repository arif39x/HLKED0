.code
GetSysInfoOutput proc uses esi edi ebx
	invoke	ExportModule, MODULE_SYSINFO
	ret
GetSysInfoOutput endp
