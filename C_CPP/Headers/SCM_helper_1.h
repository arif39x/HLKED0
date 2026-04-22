/*
 *      name: DRV LOADER USING SCM
 *      type: USER RUNTIME
 *
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */

#pragma once


BOOL    SCM_INIT();
VOID    SCM_RELEASE();

BOOLEAN
SCM_InstallDriver(
    IN LPCTSTR    DriverName,
    IN LPCTSTR    ServiceExe
    );


BOOLEAN
SCM_RemoveDriver(
    IN LPCTSTR    DriverName
    );

BOOLEAN
SCM_StartDriver(
    IN LPCTSTR    DriverName
    );

BOOLEAN
SCM_StopDriver(
    IN LPCTSTR    DriverName
    );