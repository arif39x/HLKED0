#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>

#include "PortScan.h"

/* Lithium Headers */
#include "shared.h"
#include "SrvPlugin.h"

/* define this for VERY verbose debug msgs from the scanning thread - this helped me solve the last bug which plauged me for over 2 hours! */
//#define _PORTSCAN_DEBUG_VERBOSE_

/* define this for debug msgs from plugin */
//#define _PORTSCAN_DEBUG_

/* define this for debug message boxes from plugin */
//#define _PORTSCAN_DEBUG_LOCAL_

/* This is where we keep track of our scans */
struct PortScan Scans[MAX_SIMULTANIOUS_PORTSCANS];

/* Here are all the messages we might want to send back..

  There all here for two reasons,
	1 - Ease of editing
	2 - Some are used more than once

  */
char c_Error_In_Args		[]=		"Error in arguments.";
char c_Error_No_Free_PS		[]=		"No free portscans.";
char c_Error_PS_NFound		[]=		"Scan No. %d not active.";
char c_Ouput_Info			[]=		"#%-2d:%s";
char c_Error_WSA_STARTUP	[]=		"Error with WSAStartup() - exiting thread.";
char c_StartingScan			[]=		"Starting scan.";
char c_Found_Open			[]=		"%15s:%-5d is open.";
char c_CurrentlyScanning	[]=		"Currently scanning %s:%d, %d seconds elapsed.";
char c_Error_Resuming		[]=		"Error resuming new thread %d.";
char c_Error_CNT			[]=		"Error Creating new thread.";
char c_No_Active_Portscans	[]=		"No active scans.";
char c_Tot_Portscans		[]=		"Total %d portscan(s) listed.";
char c_Paused_C				[]=		" PAUSED";
char c_Stopping				[]=		" STOPPING";
char c_Paused				[]=		"Paused.";
char c_Unpaused				[]=		"Unpaused.";
char c_Scan_List			[]=		"Scanning %s:%s.";
char c_Halted_At			[]=		"Halted at %s:%d, %d seconds elapsed.";
char c_Stop_Msg				[]=		"Stop msg sent.";
char c_Fin					[]=		"Finnished scanning, %d open ports found, %d seconds elapsed.";

/*  How we send messages back to the server */
void SendMsg(char *cMSG,FROM_ID ci,USHORT sId) {
	char *args[1];

	args[0] = (char*)malloc(strlen(cMSG) + 64);

	if (sId)
		sprintf(args[0],c_Ouput_Info,sId,cMSG);
	else
		strcpy(args[0],cMSG);

#ifdef _PORTSCAN_DEBUG_LOCAL_
	MessageBox(NULL,"Before",args[0],MB_OK);
#endif

	CliConsoleMsg(args[0],ci);

	free (args[0]);

	return;
}

/*
	Called when we dont need the portscan anymore.. only
	to be called by the portscan thread, or if it hasnt
	been created yet by StartPortScan()
*/
void FreePS(struct PortScan *scan) {

		free (scan->cIpRange );
		free (scan->cPorts );
		scan->cIpRange = scan->cPorts = NULL;
}

/* Called by the portscan thread only to get the next port in a port list */
int GetPort (char *cPortList,int iCount) {

	char buffer[64];
	USHORT n=0,len = strlen(cPortList),count = 0;
	char *split;

	do {

		memset(buffer,0,sizeof(buffer));

		while ( (cPortList[count] != ',' ) && ( count <= len ) ) {
			buffer[n++] = cPortList[count++];
		}

		count++;
		n=0;

		split = strchr(buffer,'-');

		if (split) {
			int f,s,l;
			*split = 0;

			f= atoi(buffer);
			s= atoi(split + 1);


			if (f>s) {
				l = f-s + 1;

				if (l > iCount) {
					return atoi(buffer) - iCount;
				} else {
					iCount -= l;
				}


			} else if (s>f) {
				l = s-f + 1;

				if (l > iCount) {
					return atoi(buffer) + iCount;
				} else {
					iCount -= l;
				}


			} else {

				if (!iCount)
					return atoi(buffer);
				iCount--;

			}


			*split = '-';


		} else {

			if (!iCount)
				return atoi(buffer);
			iCount--;
		}


	} while (count <= len);

	return 0;

}

/* This thread does the actual scanning */
DWORD WINAPI PortScanThread(LPVOID lpArgs)
{
	SOCKADDR_IN IA_Host;
	SOCKET s[MAX_SIMULTANIOUS_PORTSCAN_SOCKETS];
	char *split = NULL,buf[256];

	int sc,
		res;
	LONG lStartHost,
		lEndHost,
		scan_time;
	BOOL bCountingUp = TRUE;
	USHORT p=0,
		pcount=0,
		o_count=0,
		scan_wait = PORTSCAN_WAIT,
		max =MAX_SIMULTANIOUS_PORTSCAN_SOCKETS;

	DWORD wait,
		dwLen;

	fd_set rfds;
	TIMEVAL tm;
	WSADATA WSock_dat;
	LINGER s_linger;
	WORD WSock_ver=MAKEWORD(1,1);

	if (WSAStartup(WSock_ver,&WSock_dat)) {
		SendMsg(c_Error_WSA_STARTUP,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
		FreePS((struct PortScan*)lpArgs);

		return 0;
	}

	tm.tv_sec=0;
	tm.tv_usec=0;

	split = strchr(((struct PortScan*)lpArgs)->cIpRange,'-');

	if (split) *split = 0;

	/* lip = ntohl(inet_addr("126.255.224.1")); */
	lStartHost = ntohl(inet_addr(((struct PortScan*)lpArgs)->cIpRange ));

	if (split)
		lEndHost = ntohl(inet_addr(split + 1));
	else
		lEndHost = lStartHost;

	if (split)	{
		if (lStartHost > lEndHost) bCountingUp = FALSE;
	} else {
		bCountingUp = rand() % 2;
	}

	if (split) *split = '-';
	IA_Host.sin_family				= AF_INET;
	IA_Host.sin_addr.S_un.S_addr	= htonl(lStartHost);

	SendMsg(c_StartingScan,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);

	scan_time = GetTickCount();

	sc=0;
	while (sc < max){
		s[sc++] = INVALID_SOCKET;
	}

	while (!((struct PortScan*)lpArgs)->Stop) {

		wait = GetTickCount();

		sc = 0;
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("max = ",((struct PortScan*)lpArgs)->cId,max);
Sleep(100);
#endif
		while (sc < max) {

			while ( (p = GetPort(((struct PortScan*)lpArgs)->cPorts,pcount++)) && sc < max) {
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("p=",((struct PortScan*)lpArgs)->cId,p);
#endif
				IA_Host.sin_port				= htons(p);

				s[sc] = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

#ifdef _PORTSCAN_DEBUG_

			if (s[sc] == INVALID_SOCKET)
			{
				char buf[256];
				sprintf (buf,"socket() error:%d, socket: %d, port %d",WSAGetLastError(),sc,p);
				SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
				Sleep(10);
			}

#endif

				dwLen = TRUE; /* Must be non zero to set non blocking! */

				res	= ioctlsocket(s[sc],FIONBIO,&dwLen);

#ifdef _PORTSCAN_DEBUG_

			if (res)
			{
				char buf[256];
				sprintf (buf,"ioctlsocket() error:%d, socket: %d, port %d",WSAGetLastError(),sc,p);
				SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
				Sleep(10);
			}

#endif

				res = connect(s[sc],(const SOCKADDR*)&IA_Host,sizeof(IA_Host));

				if (res == SOCKET_ERROR && WSAGetLastError() == WSAENOBUFS ) {

						max = sc-1;
					//	scan_wait = max/MAX_SIMULTANIOUS_PORTSCAN_SOCKETS * PORTSCAN_WAIT;
						pcount -=2; /* two instead of one, because were not breaking, and the while loop increments pcount before it breaks! */

#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("max=sc= ",((struct PortScan*)lpArgs)->cId,max);
#endif
				}

#ifdef _PORTSCAN_DEBUG_

				if (res == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAENOBUFS) {
						char buf[256];
						sprintf (buf,"connect() failed WSAGetLastError: %d, socket %d, port %d",WSAGetLastError(),sc,p);
						SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
				}

#endif

				sc++;
			}

			if (sc >= max){
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("sc",((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
#endif
				break;
			}

			if (lStartHost == lEndHost){
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("lsh=leh",((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
#endif
				break;
			}
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("pcount=0",((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
#endif
			pcount=0;
			if(bCountingUp)
				lStartHost++;
			else
				lStartHost--;

			IA_Host.sin_addr .S_un .S_addr = htonl(lStartHost);

		}

		if (GetTickCount() - wait < scan_wait)
			Sleep(scan_wait - GetTickCount() + wait);

		sc = 0;
		while (sc < max) {

			if (s[sc] == INVALID_SOCKET){
				sc++;
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("inv=sock",((struct PortScan*)lpArgs)->cId,sc);
#endif
				continue;
			}

			FD_ZERO(&rfds);
			FD_SET(s[sc],&rfds);

			if((res=select(0,NULL,&rfds,NULL,&tm))>0)  //found one
			{
				IN_ADDR found;
				SOCKADDR sname;
				USHORT fp;

				int coolio = sizeof(sname);
				getpeername (s[sc],&sname,&coolio);
				memcpy(&found.S_un.S_addr,&sname.sa_data[2],4);
				memcpy(&fp,&sname.sa_data,2);

				sprintf(buf,c_Found_Open,inet_ntoa(found),ntohs(fp));
				o_count++;
				SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);

			}

#ifdef _PORTSCAN_DEBUG_
			if (res == SOCKET_ERROR)
			{
				char buf[256];
				sprintf (buf,"select() failed WSAGetLastError: %d, socket %d",WSAGetLastError(),sc);
				SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
				Sleep(10);
			}
#endif

			/* make sure were doing a hard close with no waiting - fairly important! */
			s_linger.l_onoff =1;
			s_linger.l_linger =0;

#ifdef _PORTSCAN_DEBUG_
			if (setsockopt(s[sc],SOL_SOCKET,SO_LINGER,(char*)&s_linger,sizeof(s_linger)))
			{
				char buf[256];
				sprintf (buf,"setsockopt() failed WSAGetLastError: %d, socket %d, port",WSAGetLastError(),sc,p);
				SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
				Sleep(10);
			}
#else

			setsockopt(s[sc],SOL_SOCKET,SO_LINGER,(char*)&s_linger,sizeof(s_linger));
#endif


			res = closesocket(s[sc]);
#ifdef _PORTSCAN_DEBUG_VERBOSE_
SendMsg("close=sock",((struct PortScan*)lpArgs)->cId,sc);
#endif

#ifdef _PORTSCAN_DEBUG_

				if (res == SOCKET_ERROR) {
						char buf[256];
						sprintf (buf,"closesocket() failed WSAGetLastError: %d, socket %d, port %d",WSAGetLastError(),sc,p);
						SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
				}

#endif


			s[sc] = INVALID_SOCKET;

			sc++;
		}

		if (lStartHost == lEndHost && !p)
			break;

		while (((struct PortScan*)lpArgs)->Pause) Sleep(50);

		if (((struct PortScan*)lpArgs)->Info) {
			IN_ADDR found;
			found.S_un .S_addr = htonl(lStartHost);
			sprintf(buf,c_CurrentlyScanning,inet_ntoa(found),p,(GetTickCount() - scan_time) / 1000);
			SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);

			((struct PortScan*)lpArgs)->Info = FALSE;
		}

	}

	if (((struct PortScan*)lpArgs)->Stop) {
		IN_ADDR found;
		found.S_un .S_addr = htonl(lStartHost);
		sprintf(buf,c_Halted_At,inet_ntoa(found),p, (GetTickCount() - scan_time) / 1000 );
		SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);

	}else{

		sprintf(buf,c_Fin,o_count,(GetTickCount() - scan_time) / 1000);
		SendMsg(buf,((struct PortScan*)lpArgs)->cId,((struct PortScan*)lpArgs)->sId);
	}

	sc = 0;
	while (sc < max) {
		closesocket(s[sc++]);
	}

	FreePS((struct PortScan*)lpArgs);

	return 0;
}

/* Client Interface to find the correct portscan and stop it */
void StopPortScan(PORTSCAN_ID dId, FROM_ID ci)
{
	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;
	char buffer[128];

	for (;i>0;i--) {
		if (Scans[i].sId == dId && Scans[i].cIpRange && Scans[i].cPorts) {

			Scans[i].cId = ci;
			Scans[i].Stop = TRUE;
			Scans[i].Pause = FALSE;
			SendMsg(c_Stop_Msg,ci,dId);
			return;
		}
	}

	sprintf(buffer,c_Error_PS_NFound,dId);
	SendMsg(buffer,ci,0);
	return;

}

/* Makes sure the list we are getting works so there is no trouble later */
BOOL ParsePortList(char *cPortList) {

	USHORT len = strlen(cPortList);
	UINT count=len,n=0;
	char buffer[8],*t,*v,*u;


	/* Check that the only characters were allowing are: 0 1 2 3 4 5 6 7 8 9 , - */
	while (count-- > 0){

		if (! isdigit(cPortList[count]) ) {
			if (cPortList[count] == ',' || cPortList[count] == '-') {
				continue;
			}
			else return FALSE;

			continue;
		}

	}

	//Now check if any ports are below 0 or above 65535
	count=0;

	do {
		memset(buffer,0,sizeof(buffer));

		while (cPortList[count] != ',' && cPortList[count] != '-' && n < sizeof(buffer) && cPortList[count]) {
			buffer[n++] = cPortList[count++];
		}

		n=0;

		if (atoi(buffer) > 0xffff || atoi(buffer) < 1) return FALSE;

	} while (count++ < len);

	count =0;

	// Check there is no 114-116-118 type thing (not sure how bullet proof this is)
	// Really checks that there is not more than one '-' before each ','
	while ( (t = strchr(cPortList + count,'-')) ) {

		v = strchr(t,',');
		u = strchr(t+1,'-');

		if (v && u && v > u)
			return FALSE;
		else if (!v && u)
			return FALSE;

		count = t - cPortList + 1 ;
	}

	return TRUE;

}

/* Makes sure the IPs we are getting are valid */
BOOL ParseIP(char *cIp) {

	char *split = strchr(cIp,'-');

	/* The minimum Ip length is "1.2.3.4" == 7 */
	if (strlen(cIp) < 7)
		return FALSE;

	if (split) {

		*split = 0;

		if (inet_addr(cIp) == INADDR_NONE || inet_addr(split + 1) == INADDR_NONE) {
			*split = '-';
			return FALSE;
		}

		*split = '-';

	} else {

		if (inet_addr(cIp) == INADDR_NONE) {
			return FALSE;
		}
	}

	return TRUE;


}

/* Called on pluin startup to set initial variables APPLICATION INTERFACE */
void InitScanList() {

	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;

	for (;i>0;) {
		Scans[i].sId =i;
		Scans[i].cIpRange = Scans[i].cPorts = NULL;
		Scans[i].cId = Scans[i].Pause = Scans[i].Stop = 0;
		i--;
	}

}

/* Gets the next free portscan */
struct PortScan *GetFreeScan(void) {

	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;

	for (;i>0;i--) {
		if (!Scans[i].cPorts && !Scans[i].cIpRange ) return &Scans[i];
	}

	return NULL;
}

/* Sets the PAUSE bool CLIENT INTERFACE */
void PausePortScan(PORTSCAN_ID dId, FROM_ID ci) {

	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;
	char buffer[256];

	for (;i>0;i--) {
		if (Scans[i].sId == dId && Scans[i].cIpRange && Scans[i].cPorts) {
			if (Scans[i].Pause) {

				Scans[i].Pause = FALSE;
				SendMsg(c_Unpaused,ci,dId);
				return;
			} else {
				Scans[i].Pause = TRUE;
				SendMsg(c_Paused,ci,dId);
				return;
			}
		}
	}

	sprintf(buffer,c_Error_PS_NFound,dId);
	SendMsg(buffer,ci,0);
	return;

}

/* Lists active portscans */
void ListPortScans(FROM_ID ci) {

	char buffer[256];
	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;
	USHORT count =0;

	for (;i>0;i--) {
		if (Scans[i].cIpRange && Scans[i].cPorts) {
			sprintf(buffer,c_Scan_List,Scans[i].cIpRange,Scans[i].cPorts);
			if (Scans[i].Pause) strcat(buffer,c_Paused_C);
			if (Scans[i].Stop) strcat(buffer,c_Stopping);

			SendMsg(buffer,ci,Scans[i].sId);
			count++;
		}
	}

	if (!count) {
		SendMsg(c_No_Active_Portscans,ci,0);
		return;
	}

	sprintf(buffer,c_Tot_Portscans,count);
	SendMsg(buffer,ci,0);
	return;
}

/* Creates the thread, memcpy's etc CLIENT INTERFACE */
void StartPortScan(char *cIp,char* cPort, FROM_ID ci) {

	struct PortScan *scan =GetFreeScan();
	DWORD tId;
	HANDLE hThread;

	if (!scan) {

		SendMsg(c_Error_No_Free_PS,ci,0);
		return;
	}

	if ( ! ParsePortList(cPort) ||
		 ! ParseIP(cIp) ) {

		SendMsg(c_Error_In_Args,ci,0);
		return;
	}

	scan->cPorts	=(char*)malloc(strlen(cPort) + 1);
	scan->cIpRange	=(char*)malloc(strlen(cIp) + 1);

	if (!scan->cPorts || !scan->cIpRange)
		return;

	scan->Pause	= scan->Stop = scan->Info = FALSE;

	strcpy(scan->cPorts,cPort);
	strcpy(scan->cIpRange,cIp);

	scan->cId = ci;

	if (! (hThread = CreateThread(NULL,0,PortScanThread,(LPVOID)scan,CREATE_SUSPENDED,&tId)) ) {
		SendMsg(c_Error_CNT,ci,0);
		FreePS(scan);

	} else {
#ifdef _PORTSCAN_DEBUG_LOCAL_
		MessageBox(NULL,"Resuming Thread",":P",MB_OK);
#endif
		if (ResumeThread(hThread) == 0xFFFFFFFF) {
			char buf[256];
			sprintf(buf,c_Error_Resuming,GetLastError());
			SendMsg(buf,ci,scan->sId);
			FreePS(scan);
		}
#ifdef _PORTSCAN_DEBUG_
		else
		{
			SendMsg("New thread created and resumend",ci,scan->sId);
		}
#endif
	}

}

/* Stops all portscans */
void StopAllPortscans() {

	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;

	for (;i>0;i--) {
		if (Scans[i].sId) {
			Scans[i].Pause = FALSE;
			Scans[i].Stop = TRUE;

		}
	}
}


/* Sends a "Error in arguments msg */
void ErrorInArgs(FROM_ID ci) {

	SendMsg(c_Error_In_Args,ci,0);

}

/* Sets the INFO bool so the portscan thread sends the info */
void PortScanInfo (PORTSCAN_ID dId,FROM_ID ci){

	USHORT i=MAX_SIMULTANIOUS_PORTSCANS;
	char buffer[256];

	for (;i>0;i--) {
		if (Scans[i].sId == dId && Scans[i].cIpRange && Scans[i].cPorts) {
			Scans[i].Info = TRUE;
			return;
		}
	}

	sprintf(buffer,c_Error_PS_NFound,dId);
	SendMsg(buffer,ci,0);
	return;
}
