
#include <windows.h>

#ifndef _WININETREPLACEMENT_H
#define _WININETREPLACEMENT_H


typedef LPVOID HINTERNET;
typedef WORD INTERNET_PORT;

HINTERNET WINAPI MyHttpOpenRequest(HINTERNET hConnect, LPCTSTR lpszVerb, LPCTSTR lpszObjectName, LPCTSTR lpszVersion, LPCTSTR lpszReferer, LPCTSTR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
HINTERNET WINAPI MyInternetConnect(HINTERNET hInternet, LPCTSTR lpszServerName, INTERNET_PORT nServerPort, LPCTSTR lpszUsername, LPCTSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
HINTERNET WINAPI MyFtpOpenFile(HINTERNET hConnect, LPCTSTR lpszFileName, DWORD dwAccess, DWORD dwFlags, DWORD_PTR dwContext);

typedef HINTERNET (WINAPI *MYHTTPOPENREQUEST)(HINTERNET hConnect, LPCTSTR lpszVerb, LPCTSTR lpszObjectName, LPCTSTR lpszVersion, LPCTSTR lpszReferer, LPCTSTR *lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET (WINAPI *MYINTERNETCONNECT)(HINTERNET hInternet, LPCTSTR lpszServerName, INTERNET_PORT nServerPort, LPCTSTR lpszUsername, LPCTSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET (WINAPI *MYFTPOPENFILE)(HINTERNET hConnect, LPCTSTR lpszFileName, DWORD dwAccess, DWORD dwFlags, DWORD_PTR dwContext);

#endif