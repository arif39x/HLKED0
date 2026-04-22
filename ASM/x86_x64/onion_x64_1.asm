.CODE

__adjustRSP PROC
POP RAX
ADD RSP, [RSP]
JMP RAX
__adjustRSP ENDP

__moveRAX PROC
POP RCX
MOV RAX, [RSP]
JMP RCX
__moveRAX ENDP

KiFastSystemCall PROC
ADD RSP, 16
LEA RDX, [ESP + 16]
INT 46
RETN
KiFastSystemCall ENDP

END