/*
 *      name: ZION User mode proxy
 *      type: USER RUNTIME
 *
 *      Org Author: Shikai Chen
 *		modified for test purpose: by Liang Chen
 *      Version : 0.2
 */


#include "stdafx.h"
#include "zion/zion_define.h"
#include "zion/proxy/zion_proxy_dll.h"


#include "proxy_internal.h"
#include "SCM_helper.h"
#include "stdio.h"

static BOOL _get_drv_path(LPWSTR strBuffer, int dwLength);
static void _close_dev_handle();

STD_EXPORT(BOOL) zion_isServerLoaded(void)
//Test whether the zion server device object handle exists
//if such handle exists, open it and return TRUE
//if such handle does not exist or cannot be opened, return FALSE


//NOTICE: 
// [1] Client should call this funcion BEFORE any other ZION OPERATIONS
// [2] This function will *automantically* open a Sever Devive Handle when
//     the Server has already been loaded.
//     Client should *explicit* invoke the zion_release to release such handle

{
//TODO: very *NAIVE* method! Change it!

    if (g_hZionServer_Dev) return TRUE;
        // The server handle has readly existed,
        // Must be loaded before

    if ((g_hZionServer_Dev = CreateFile(L"\\\\.\\"STR_OBJ_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
        )) == INVALID_HANDLE_VALUE) {
            g_hZionServer_Dev = NULL;
            return FALSE;
    }
    return TRUE;
}

STD_EXPORT(VOID) zion_proxy_release(void)
//Release the Proxy's resources
{
	/*
	added by Liang Chen for test.
	*/
	
	/*end add*/
    zion_disconnectServer(ZION_OPERATION_FORCE);
//	zion_unloadServer(0, 0);
    _close_dev_handle();
}

STD_EXPORT(ZIONRESULT) zion_isConnected(void)
//Indicate whether aconnection has been established
{
    return (g_bIsConnected && g_hZionServer_Dev);
}


STD_EXPORT(ZIONRESULT) zion_connectServer(LPCWSTR strHelloMsg, DWORD dwFlag)
//Try to establish a connection between the Server and current client
//Any client should make a connection to the server FIRST

//NOTICE: To connect to the server, one should first load it 

//TODO: The user authorization operation has not been implemented yet.
//      After implement this operation, add some wrap-code HERE.
//      Currently, this function always returns OK without *ANY* operation.
{
    if (zion_isConnected()) return ZION_OK;

    if (zion_isServerLoaded()){
        g_bIsConnected = TRUE;
        return ZION_OK;
    }
    return ZION_SERVER_NOTLOAD;
}


STD_EXPORT(ZIONRESULT) zion_disconnectServer(DWORD dwFlag)
//Disconnect the connection between the Server and current client

//TODO: The user authorization operation has not been implemented yet.
//      After implement this operation, add some wrap-code HERE.
//      Currently, this function always returns OK without *ANY* operation.
{
    if (g_bIsConnected){

        g_bIsConnected = FALSE;
    }
    return ZION_OK;
}


STD_EXPORT(ZIONRESULT) zion_loadServer(DWORD dwFlag)
//Load the Server device driver into the kernel
//If specify the ZION_OPERATION_FORCE flag, this function will first stop 
//currently running server, unload it, and then reload it again.

{
//TODO: SCM method should be deprecated
    ZIONRESULT ans = ZION_UNKNOWN;
    WCHAR   strPathBuffer[MAX_PATH];
    if (zion_isServerLoaded()){
        if (dwFlag == ZION_OPERATION_STD)
        {
			return ZION_OK;
            //No needs to reload
        }else //ZION_OPERATION_FORCE
        { 
            if (ZION_FAILED(ans=zion_unloadServer(L"FORCE_RELOAD",ZION_OPERATION_FORCE)))
            {
                //cannot unload
                return ans;
            }
        }
    }
    
    if (!_get_drv_path(strPathBuffer,MAX_PATH))
    //server driver path's string length too long...
    {
        return ZION_SERVER_LOADERROR;
    }
    
    ans = ZION_SERVER_LOADERROR;
    if (SCM_InstallDriver(g_strDrvFileName,strPathBuffer )) {
        if (SCM_StartDriver(g_strDrvFileName ))
        {

            if (zion_isServerLoaded()){
                 ans = ZION_OK;
            }
        }
    }
	
    return ans;
}

STD_EXPORT(ZIONRESULT) zion_unloadServer(LPCWSTR strReason, DWORD dwFlag)
{
//TODO: SCM method should be deprecated
//TODO:  strReason not implemented yet!

//TODO: maybe change to zion_serverGetClientsNum()
//TODO: notify the server!

    if (zion_isConnected()) return ZION_CANNOT_UNLOAD;
    _close_dev_handle();
    SCM_StopDriver(g_strDrvFileName);
    SCM_RemoveDriver(g_strDrvFileName);

    
    return ZION_OK;
}












///////////////////////////////////////////////
//HELPER FUNCTIONS

static BOOL _get_drv_path(LPWSTR strBuffer, int dwLength)
{
    int dwDesiredBufferSize = 0;
    if (!strBuffer) return FALSE;
    dwDesiredBufferSize = GetCurrentDirectory(dwLength,
		strBuffer
		);

    dwLength -= dwDesiredBufferSize;
    
    if (dwLength <= lstrlenW(g_strDrvFileName) + 5)
        return FALSE;
	lstrcatW(strBuffer, L"\\");
	lstrcatW(strBuffer,g_strDrvFileName);
	lstrcatW(strBuffer, L".sys");    
    return TRUE;
}

static void _close_dev_handle()
{
    CloseHandle(g_hZionServer_Dev);
    g_hZionServer_Dev = NULL;
}