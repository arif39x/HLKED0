REG_NONE                EQU   0
REG_SZ                  EQU   1
REG_EXPAND_SZ           EQU   2
REG_BINARY              EQU   3
REG_DWORD               EQU   4
REG_DWORD_LITTLE_ENDIAN EQU   4
REG_DWORD_BIG_ENDIAN    EQU   5
REG_LINK                EQU   6
REG_MULTI_SZ            EQU   7
REG_RESOURCE_LIST       EQU   8
REG_FULL_RESOURCE_DESCRIPTOR    EQU   9
REG_RESOURCE_REQUIREMENTS_LIST  EQU  10


KEY_ALL_ACCESS          EQU   0F003Fh
KEY_QUERY_VALUE         EQU   1
KEY_SET_VALUE           EQU   2
KEY_CREATE_SUB_KEY      EQU   4
KEY_ENUMERATE_SUB_KEYS  EQU   8
KEY_NOTIFY              EQU   10h
KEY_CREATE_LINK         EQU   20h

REG_OPTION_RESERVED     EQU   0
REG_OPTION_NON_VOLATILE EQU   0
REG_OPTION_VOLATILE     EQU   1
REG_OPTION_CREATE_LINK  EQU   2
REG_OPTION_BACKUP_RESTORE  EQU   4


EVENT_ALL_ACCESS        EQU   1F0003h


OBJ_INHERIT             EQU    2
OBJ_PERMANENT           EQU   10h
OBJ_EXCLUSIVE           EQU   20h
OBJ_CASE_INSENSITIVE    EQU   40h
OBJ_OPENIF              EQU   80h
OBJ_VALID_ATTRIBUTES    EQU  0F2h


LARGE_INTEGER            STRUC
      liLowPart          DD    ?
      liHighPart         DD    ?
LARGE_INTEGER            ENDS


UNICODE_STRING           STRUC
      usLength           DW    ?
      usMaximumLength    DW    ?
      usBuffer           DD    ?        ; VOID*
UNICODE_STRING           ENDS


ANSI_STRING              STRUC
      asLength           DW    ?
      asMaximumLength    DW    ?
      asBuffer           DD    ?        ; VOID*
ANSI_STRING              ENDS


CLIENT_ID                STRUC
      ciProcess          DD    ?        ; HANDLE? ID?
      ciThread           DD    ?        ; HANDLE? ID?
CLIENT_ID                ENDS


PROCESS_PARAMETERS       STRUC
      ppAllocationSize   DD    ?
      ppActualSize       DD    ?
      ppFlags            DD    ?
      ppUnknown1         DD    ?
      ppInputHandle      DD    ?              ; HANDLE
      ppOutputHandle     DD    ?              ; HANDLE
      ppErrorHandle      DD    ?              ; HANDLE
      ppCurrentDirectory DB    SIZE UNICODE_STRING DUP (?)
      ppCurrentDir       DD    ?              ; HANDLE
      ppSearchPaths      DB    SIZE UNICODE_STRING DUP(?)
      ppApplicationName  DB    SIZE UNICODE_STRING DUP(?)
      ppCommandLine      DB    SIZE UNICODE_STRING DUP(?)
      ppEnvironmentBlock DD    ?              ; VOID*
      ppUnknown2         DD    9 DUP(?)
      ppUnknown3         DB    SIZE UNICODE_STRING DUP(?)
      ppUnknown4         DB    SIZE UNICODE_STRING DUP(?)
      ppUnknown5         DB    SIZE UNICODE_STRING DUP(?)
      ppUnknown6         DB    SIZE UNICODE_STRING DUP(?)
PROCESS_PARAMETERS       ENDS


PROCESS_ENVIRONMENT_BLOCK  STRUC
      pebAllocationSize    DD    ?
      pebUnknown1          DD    ?
      pebProcessInstance   DD    ?    ; HINSTANCE
      pebDllList           DD    ?    ; VOID*
      pebProcessParameters DD    ?    ; PROCESS_PARAMETERS*
      pebUnknown2          DD    ? 
      pebDefaultHeap       DD    ?    ; HANDLE
PROCESS_ENVIRONMENT_BLOCK  ENDS


SECTION_IMAGE_INFORMATION  STRUC
      siiEntryPoint        DD    ?
      siiUnknown1          DD    ?
      siiReservedStackSize DD    ?
      siiCommittedStackSize     DD    ?
      siiSubsystem              DD    ?
      siiSubsystemVersionMinor  DD    ?
      siiSubsystemVersionMajor  DD    ?
      siiUnknown2          DD    ?
      siiCharacteristics   DD    ?
      siiMachine           DD    ?
      siiUnknown3          DD    ?
      siiUnknown4          DD    ?
      siiUnknown5          DD    ?
SECTION_IMAGE_INFORMATION  ENDS


RTL_PROCESS_INFORMATION   STRUC
      rpiSize             DD    ?
      rpiProcessHandle    DD    ?          ; HANDLE
      rpiThreadHandle     DD    ?          ; HANDLE
      rpiClientId         DB    SIZE CLIENT_ID DUP(?)
      rpiSectionImageInfo DB    SIZE SECTION_IMAGE_INFORMATION DUP(?)
RTL_PROCESS_INFORMATION   ENDS


OBJECT_ATTRIBUTES         STRUC
      oaLength            DD    ?
      oaRootDirectory     DD    ?          ; HANDLE
      oaObjectName        DD    ?          ; UNICODE_STRING*
      oaAttributes        DD    ?
      oaSecurityDescriptor DD   ?          ; VOID*
      oaSecurityQoS       DD    ?          ; VOID*
OBJECT_ATTRIBUTES         ENDS
