	.data
	szServicesNames	db  0
        include svc.inc
			db  0
        .code

StopService proc szName: DWORD
        LOCAL   schSCManager: DWORD
        LOCAL   SrvSt: SERVICE_STATUS

        invoke  OpenSCManager, NULL, NULL, SC_MANAGER_ALL_ACCESS
        mov     schSCManager, eax
        .IF     !eax
                ret
        .ENDIF
        invoke  OpenService, schSCManager, szName, SERVICE_STOP or SERVICE_CHANGE_CONFIG
        .IF     eax
                push    eax

                xchg    eax, edx
                invoke  ControlService, edx, SERVICE_CONTROL_STOP, addr SrvSt
                mov     edx, [esp]
                invoke  ChangeServiceConfig, edx, SERVICE_NO_CHANGE, SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL

                call    CloseServiceHandle
        .ENDIF
        invoke  CloseServiceHandle, schSCManager
        ret
StopService endp

StopServices proc
 mov     edi, offset szServicesNames
        @next:
                invoke  StopService, edi
                mNextListEntry @next
        ret
StopServices endp