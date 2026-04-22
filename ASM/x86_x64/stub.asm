_TEXT        SEGMENT
             ASSUME  CS:_TEXT,DS:_TEXT,SS:_STACK
StartX:
             push  CS
             pop   DS
             mov   AH,9
             mov   DX, OFFSET Msg
             int   21h
             mov   AX,4C01h
             int   21h

Msg          DB    'This program cannot be run in DOS mode.',13,10,'$'

_TEXT        ENDS

_STACK       SEGMENT 'STACK'
             DB   64 DUP(0)
_STACK       ENDS

             END   StartX
