.data
stm SYSTEMTIME<>
dateformat db "dd:MM",0
timeformat db "hh:mm",0
.code
cmptime proc
	IFDEF MOD_DDMM
cmp_again: 
invoke GetLocalTime, addr stm
invoke GetDateFormat, LOCALE_USER_DEFAULT, NULL,addr stm,addr dateformat, addr lpBuf1, sizeof lpBuf1
invoke lstrcmp, addr lpBuf1, addr actdate 
 .IF eax ==0
 jmp gotonext
 .ENDIF
invoke Sleep, 1000
jmp cmp_again
	ENDIF
	IFDEF MOD_HHMM
gotonext: 
invoke GetLocalTime, addr stm
invoke GetTimeFormat, LANG_ENGLISH, TIME_FORCE24HOURFORMAT,addr stm,addr timeformat, addr lpBuf1, sizeof lpBuf1
invoke lstrcmp, addr lpBuf1, addr acttime 
 .IF eax ==0
 jmp gototret
 .ENDIF
invoke Sleep, 1000
jmp gotonext
	ENDIF
gototret:
ret
cmptime endp
