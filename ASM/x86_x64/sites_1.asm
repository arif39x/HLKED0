IFDEF MOD_TZ
.data
szRegDom	db  "Software\Microsoft\Windows\CurrentVersion\Internet Settings\ZoneMap\Domains\%s",0
szFMT1		db  "*",0
szFMT2		db  "http",0
sz1		db   0
include sites.inc
		db   0
lpBuf12         dd  0
szfe 		equ 2

.code
createzonemap proc site: DWORD
LOCAL	TwoPar		:DWORD
LOCAL	RegHandle	:DWORD
	invoke wsprintf,addr lpBuf12,addr szRegDom,site
	invoke	RegCreateKey, HKEY_CURRENT_USER, addr lpBuf12, addr RegHandle
	mov eax, szfe
	mov TwoPar, eax
	mov eax, [TwoPar]
	invoke RegSetValueEx, RegHandle,addr szFMT1,NULL,REG_DWORD, addr TwoPar, 4
	invoke RegSetValueEx, RegHandle,addr szFMT2,NULL,REG_DWORD, addr TwoPar, 4
	invoke RegCloseKey, RegHandle
        ret
createzonemap endp

createzonemaps proc
mov     edi,  offset sz1
  @next:
invoke createzonemap, edi
    mNextListEntry @next
ret
createzonemaps endp
ENDIF