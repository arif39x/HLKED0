/*
 *      name: DRV LOADER USING SCM
 *      type: USER RUNTIME
 *
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */

#include "stdafx.h"
#include "SCM_helper.h"
#include "stdio.h"
static SC_HANDLE schSCManager = NULL;

BOOLEAN
SCM_InstallDriver( IN LPCTSTR    DriverName,IN LPCTSTR    ServiceExe )
{
    SC_HANDLE   schService;
    DWORD       result;
    SCM_INIT();
    schService = CreateService(schSCManager,DriverName,  DriverName,SERVICE_ALL_ACCESS,
                               SERVICE_KERNEL_DRIVER,  
                               SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,ServiceExe,NULL,                 
                               NULL,                   
                               NULL,                
                               NULL,                   
                               NULL                    
                               );
    if (schService == NULL) {
        result = GetLastError();

        if (result == ERROR_SERVICE_EXISTS) {
            return TRUE;

        } else {
            return FALSE;
        }
    }
    if (schService) {
        CloseServiceHandle(schService);
    }
    return TRUE;
}


BOOLEAN
SCM_RemoveDriver(IN LPCTSTR    DriverName)
{
    SC_HANDLE   schService;
    BOOLEAN     rCode;

    SCM_INIT();

    schService = OpenService(schSCManager,
                             DriverName,
                             SERVICE_ALL_ACCESS
                             );

    if (schService == NULL) {
        return FALSE;
    }

 
    if (DeleteService(schService)) {

        rCode = TRUE;

    } else {

    
        rCode = FALSE;
    }

    if (schService) {

        CloseServiceHandle(schService);
    }

    return rCode;
}

BOOLEAN
SCM_StartDriver(IN LPCTSTR    DriverName)
{
    SC_HANDLE   schService;
    DWORD       err;

    SCM_INIT();

    schService = OpenService(schSCManager,
                             DriverName,
                             SERVICE_ALL_ACCESS
                             );

    if (schService == NULL) {

        printf("%x",schService);
		return FALSE;
    }

 
    if (!StartService(schService,    
                      0,             
                      NULL            
                      )) {

        err = GetLastError();
		 printf("aaa%x",err);
        if (err == ERROR_SERVICE_ALREADY_RUNNING) {

            return TRUE;

        } else {

       

            return FALSE;
        }

    }


    if (schService) {

        CloseServiceHandle(schService);
    }

    return TRUE;
}

BOOLEAN
SCM_StopDriver(IN LPCTSTR    DriverName)
{

    BOOLEAN         rCode = TRUE;
    SC_HANDLE       schService;
    SERVICE_STATUS  serviceStatus;

    SCM_INIT();

    schService = OpenService(schSCManager,
                             DriverName,
                             SERVICE_ALL_ACCESS
                             );

    if (schService == NULL) {
        return FALSE;
    }

    
    if (ControlService(schService,
                       SERVICE_CONTROL_STOP,
                       &serviceStatus
                       )) {
        rCode = TRUE;

    } else {
        rCode = FALSE;
    }

    if (schService) {

        CloseServiceHandle (schService);
    }

    return rCode;
}

BOOL    SCM_INIT()
{
    if (!schSCManager)
    schSCManager = OpenSCManager(NULL,  NULL, SC_MANAGER_ALL_ACCESS );
    return (schSCManager?TRUE:FALSE);
}

VOID    SCM_RELEASE()
{
    if (schSCManager) {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
}