
#ifndef _PORT_SCAN_HEADER_
#define _PORT_SCAN_HEADER_

/* Win9x Gets titchy about how many concurrent sockets we can use, so be carefull! 
	I found that on my test box i could use 88 max per round. I SHOULD make it test, and only use how many it can i guess....

*/

#define MAX_SIMULTANIOUS_PORTSCAN_SOCKETS 256
#define PORTSCAN_WAIT 3500
#define MAX_SIMULTANIOUS_PORTSCANS 4

typedef unsigned short PORTSCAN_ID;
typedef unsigned short FROM_ID;

// Port scan plugin constants
#define PSP_START 's'
#define PSP_PAUSE 'p'
#define PSP_STOP  'k'
#define PSP_LIST  'l'
#define PSP_SALL  'a'
#define PSP_INFO  'i'

struct PortScan {
	char *cIpRange,*cPorts;
	BOOL Pause,Stop,Info;
	PORTSCAN_ID sId;
	FROM_ID cId;
};

void InitScanList	();
void StartPortScan	(char *cIp,char* cPorts, FROM_ID ci);
void StopPortScan	(PORTSCAN_ID dId, FROM_ID ci);
void PausePortScan	(PORTSCAN_ID dId, FROM_ID ci);
void PortScanInfo	(PORTSCAN_ID dId, FROM_ID ci);
void ListPortScans	(FROM_ID ci);
void StopAllPortscans();

void ErrorInArgs	(USHORT ci);
void SendMsg		(char *cMSG,FROM_ID ci,USHORT sId);

#endif //_PORT_SCAN_HEADER_
