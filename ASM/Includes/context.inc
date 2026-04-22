_SIZE_OF_80387_REGISTERS                EQU             80t
CONTEXT_i386            EQU             000010000h
CONTEXT_i486            EQU             000010000h
CONTEXT_CONTROL         EQU             000010001h
CONTEXT_INTEGER         EQU             000010002h
CONTEXT_SEGMENTS                EQU             000010004h
CONTEXT_FLOATING_POINT          EQU             000010008h
CONTEXT_DEBUG_REGISTERS         EQU             000010010h
CONTEXT_FULL            EQU             000010007h
_FLOATING_SAVE_AREA             STRUCT 4t
ControlWord             DWORD           ?
StatusWord              DWORD           ?
TagWord         DWORD           ?
ErrorOffset             DWORD           ?
ErrorSelector           DWORD           ?
DataOffset              DWORD           ?
DataSelector            DWORD           ?
RegisterArea            BYTE            80t DUP (?)
Cr0NpxState             DWORD           ?
_FLOATING_SAVE_AREA             ENDS

FLOATING_SAVE_AREA              TYPEDEF         _FLOATING_SAVE_AREA

PFLOATING_SAVE_AREA             TYPEDEF         PTR FLOATING_SAVE_AREA

_CONTEXT                STRUCT 4t
ContextFlags            DWORD           ?
_Dr0            DWORD           ?
_Dr1            DWORD           ?
_Dr2            DWORD           ?
_Dr3            DWORD           ?
_Dr6            DWORD           ?
_Dr7            DWORD           ?
FloatSave               FLOATING_SAVE_AREA              <>
SegGs           DWORD           ?
SegFs           DWORD           ?
SegEs           DWORD           ?
SegDs           DWORD           ?
_Edi            DWORD           ?
_Esi            DWORD           ?
_Ebx            DWORD           ?
_Edx            DWORD           ?
_Ecx            DWORD           ?
_Eax            DWORD           ?
_Ebp            DWORD           ?
Eip             DWORD           ?
SegCs           DWORD           ?
EFlags          DWORD           ?
_Esp            DWORD           ?
SegSs           DWORD           ?
_CONTEXT                ENDS

CONTEXT         TYPEDEF         _CONTEXT
