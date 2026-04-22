

sSEH STRUCT
        OrgEsp            DD ?
        OrgEbp            DD ?
        SaveEip           DD ?
sSEH ENDS


InstSehFrame MACRO ContinueAddr
        ASSUME FS : NOTHING

   IFNDEF  SehStruct
        SehStruct EQU 1
        .DATA
        SEH    sSEH <>
   ENDIF        
   
        .CODE
        MOV  SEH.SaveEip, ContinueAddr
        MOV  SEH.OrgEbp, EBP
        PUSH OFFSET SehHandler
        PUSH FS:[0]
        MOV  SEH.OrgEsp, ESP
        MOV  FS:[0], ESP
ENDM

KillSehFrame MACRO
        POP  FS:[0]
        ADD  ESP, 4
ENDM


.CODE
SehHandler PROC C pExcept:DWORD,pFrame:DWORD,pContext:DWORD,pDispatch:DWORD

   IFNDEF  SehStruct
        SehStruct EQU 1
        .DATA
        SEH    sSEH <>
	.CODE
   ENDIF

        MOV  EAX, pContext

        PUSH SEH.SaveEip
        POP  [EAX][CONTEXT.regEip]
        PUSH SEH.OrgEsp
        POP  [EAX][CONTEXT.regEsp]
        PUSH SEH.OrgEbp
        POP  [EAX][CONTEXT.regEbp]

        MOV  EAX, ExceptionContinueExecution

        RET
SehHandler ENDP