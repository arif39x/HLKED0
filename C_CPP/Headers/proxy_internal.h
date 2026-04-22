/*
 *      name: ZION Proxy interal state
 *      type: USER RUNTIME
 *
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */

#pragma once
#ifndef _UNICODE
#error Please compile this project in UNICODE MODE!
#endif

HANDLE      g_hZionServer_Dev       = NULL;

BOOL        g_bIsConnected          = FALSE;

LPCWSTR     g_strDrvFileName        = L"zion_server";