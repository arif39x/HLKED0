#include "includes.h"
#include "externs.h"

int XAddThread(DWORD ThreadID, bool Protected, SOCKET sock, char *Name, ...)
{
	for (int t = 1; t < MAXTHREADS; t++)
	{
		if (!threads[t].Name[0])
		{
		//	printf("%d\r\n", t);
			memset(&threads[t], 0, sizeof(THREAD));

			char ThreadName[sizeof(threads[t].Name)-1] = {0};

			va_list va;
			va_start(va, Name);
			_vsnprintf(ThreadName, sizeof(ThreadName), Name, va);
			va_end(va);

			strncpy(threads[t].Name, ThreadName, sizeof(threads[t].Name)-1);

			threads[t].ID = 0;
			threads[t].Threadnum = t;
			threads[t].ThreadID = ThreadID;

//			threads[t].KillClientOnClose = FALSE;

			threads[t].Protected = Protected;
			threads[t].ThreadSocket = sock;
			memset(&threads[t].ThreadSin, 0, sizeof(SOCKADDR_IN));
			threads[t].ThreadHandle = 0;

		//	XAddSystemLog("New Thread, ID: %d, IP: %s", threads[t].Threadnum, threads[t].Name);
			break;
		}
		
	}

	return t;
}

bool XKillThread(DWORD ThreadID, bool force, bool kill)
{
	if (!threads[ThreadID].Protected || force)
	{
		if (ThreadID >= 1 && ThreadID < MAXTHREADS)
		{
			if (threads[ThreadID].Name[0])
			{
			//	XAddSystemLog("Quit Thread, ID: %d, IP: %s", threads[ThreadID].ThreadID, threads[ThreadID].Name);

				threads[ThreadID].Threadnum = 0;
				threads[ThreadID].ThreadID = 0;

//				if (threads[ThreadID].KillClientOnClose)
//					XKillClient(threads[ThreadID].ClientID, NULL, NULL, FALSE);


//				threads[ThreadID].ClientID = 0;
				threads[ThreadID].ID = 0;

//				XKillProcessByPid(NULL, threads[ThreadID].pid);
//				threads[ThreadID].pid = 0;

				threads[ThreadID].Protected = FALSE;
				closesocket(threads[ThreadID].ThreadSocket);
				threads[ThreadID].ThreadSocket = 0;

//				closesocket(threads[ThreadID].DataSocket);
//				threads[ThreadID].DataSocket = 0;

				closesocket(threads[ThreadID].ThreadSpecialSocket);
				threads[ThreadID].ThreadSpecialSocket = 0;

//				closesocket(threads[ThreadID].TempSocket);
//				threads[ThreadID].TempSocket = 0;

			//	XHeapFree(threads[threadnum].Name);

				threads[ThreadID].Name[0] = 0;
				
	//			threads[threadnum].MainStruct = 0;
				if (kill && threads[ThreadID].ThreadHandle)
					TerminateThread(threads[ThreadID].ThreadHandle, 0);

				if (threads[ThreadID].ThreadHandle)
					CloseHandle(threads[ThreadID].ThreadHandle);

				threads[ThreadID].ThreadHandle = 0;

			//	if (threads[ThreadID].cs)
				DeleteCriticalSection(&threads[ThreadID].ThreadCriticalSection);

				memset(&threads[ThreadID], 0, sizeof(THREAD));

				return true;
			}
		}
	}
	return false;
}

bool XListThreads(SESSION* Session, char *Filter, bool All)
{
	dbg("ALL:%d", All);

	for (int t = 1; t < MAXTHREADS; t++)
	{
	//	printf("%s, %x\r\n", threads[t].Name, threads[t].Name);

		if ((!threads[t].ParentThreadID || All) && threads[t].Name[0])
		{
			if (Filter && strcmp(Filter, ""))
				if (!StrStrI(threads[t].Name, Filter))
					continue;

			if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%d. %s", t, threads[t].Name))
				return false;
		}
	//	else
	//	{
	//		memset(&threads[t], 0, sizeof(THREAD));
	//		free(&threads[t]);
	//	}
	}
//	XHeapFree(&threads);
//	memset(&threads, 0, sizeof(threads));
//	free(&threads);

	return true;
}


DWORD XKillAllThreads()
{
	DWORD numthreads = 0;

	for (int t = 1; t < MAXTHREADS; t++)
		if (threads[t].Name[0])
			if (XKillThread(threads[t].Threadnum, TRUE, TRUE))
				numthreads++;

	return numthreads;
}

bool XThreadTitle(DWORD ThreadID, char *Title, ...)
{
	if (Title)
	{
		char ThreadTitle[sizeof(threads[ThreadID].Name)-1] = {0};
		//char ThreadTitle[1024] = {0};

		va_list va;
		va_start(va, Title);
		_vsnprintf(ThreadTitle, sizeof(ThreadTitle), Title, va);
		va_end(va);

		if (ThreadID >= 0 && ThreadID < MAXTHREADS)
		{
			if (threads[ThreadID].Name)
			{
				strncpy(threads[ThreadID].Name, ThreadTitle, sizeof(threads[ThreadID].Name)-1);
				return true;
			}
		}
	}
	return false;
}

DWORD XFindThreadByID(DWORD ThreadID)
{
	DWORD numthreads = 0;

	for (int t = 1; t < MAXTHREADS; t++)
		if (threads[t].ThreadID == ThreadID)
			numthreads++;

	return numthreads;
}

int XFindThreadNumberByID(DWORD ThreadID)
{
	for (int t = 1; t < MAXTHREADS; t++)
		if (threads[t].ThreadID == ThreadID)
			return t;

	return -1;
}

