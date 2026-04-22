#pragma once
#include "info.h"
int Rprt(int Port,int Private, int UpNP, info_t* Info);
BOOL doRprt(LPCWSTR szReportUrl);
DWORD WINAPI GetCommand(char szData);