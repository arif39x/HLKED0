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
// arena_winsock.cpp

#include "stdafx.h"
//---------------------------------------------------------------------------
#include "arena_winsock.h"
//---------------------------------------------------------------------------
int ws_init(int high_ver, int low_ver)
{
  WORD wVersionRequested=MAKEWORD(high_ver, low_ver);
  WSADATA wsaData;

  if (WSAStartup(wVersionRequested, &wsaData)!=0)
    return WSENODLL;

  if (wsaData.wVersion!=MAKEWORD(high_ver, low_ver))
    return WSEINVALIDVERSION;
  return 0;
}
//---------------------------------------------------------------------------
int ws_done(void)
{
  if (WSACleanup()==SOCKET_ERROR)
    return -1;
  return 0;
}
//---------------------------------------------------------------------------
int ws_lasterror(void)
{
  return WSAGetLastError();
}
//---------------------------------------------------------------------------
void ws_showerror(int err)
{
  char msg[256];
  switch (err)
  {
    case WSAEINTR: strcpy(msg, "WSAEINTR"); break;
    case WSAEBADF: strcpy(msg, "WSAEBADF"); break;
    case WSAEACCES: strcpy(msg, "WSAEACCES"); break;
    case WSAEFAULT: strcpy(msg, "WSAEFAULT"); break;
    case WSAEINVAL: strcpy(msg, "WSAEINVAL"); break;
    case WSAEMFILE: strcpy(msg, "WSAEMFILE"); break;

    case WSAEWOULDBLOCK: strcpy(msg, "WSAEWOULDBLOCK"); break;
    case WSAEINPROGRESS: strcpy(msg, "WSAEINPROGRESS"); break;
    case WSAEALREADY: strcpy(msg, "WSAEALREADY"); break;
    case WSAENOTSOCK: strcpy(msg, "WSAENOTSOCK"); break;
    case WSAEDESTADDRREQ: strcpy(msg, "WSAEDESTADDRREQ"); break;
    case WSAEMSGSIZE: strcpy(msg, "WSAEMSGSIZE"); break;
    case WSAEPROTOTYPE: strcpy(msg, "WSAEPROTOTYPE"); break;
    case WSAENOPROTOOPT: strcpy(msg, "WSAENOPROTOOPT"); break;
    case WSAEPROTONOSUPPORT: strcpy(msg, "WSAEPROTONOSUPPORT"); break;
    case WSAESOCKTNOSUPPORT: strcpy(msg, "WSAESOCKTNOSUPPORT"); break;
    case WSAEOPNOTSUPP: strcpy(msg, "WSAEOPNOTSUPP"); break;
    case WSAEPFNOSUPPORT: strcpy(msg, "WSAEPFNOSUPPORT"); break;
    case WSAEAFNOSUPPORT: strcpy(msg, "WSAEAFNOSUPPORT"); break;

    case WSAEADDRINUSE: strcpy(msg, "WSAEADDRINUSE"); break;
    case WSAEADDRNOTAVAIL: strcpy(msg, "WSAEADDRNOTAVAIL"); break;
    case WSAENETDOWN: strcpy(msg, "WSAENETDOWN"); break;
    case WSAENETUNREACH: strcpy(msg, "WSAENETUNREACH"); break;
    case WSAENETRESET: strcpy(msg, "WSAENETRESET"); break;
    case WSAECONNABORTED: strcpy(msg, "WSAECONNABORTED"); break;
    case WSAECONNRESET: strcpy(msg, "WSAECONNRESET"); break;
    case WSAENOBUFS: strcpy(msg, "WSAENOBUFS"); break;
    case WSAEISCONN: strcpy(msg, "WSAEISCONN"); break;
    case WSAENOTCONN: strcpy(msg, "WSAENOTCONN"); break;
    case WSAESHUTDOWN: strcpy(msg, "WSAESHUTDOWN"); break;
    case WSAETOOMANYREFS: strcpy(msg, "WSAETOOMANYREFS"); break;
    case WSAETIMEDOUT: strcpy(msg, "WSAETIMEDOUT"); break;
    case WSAECONNREFUSED: strcpy(msg, "WSAECONNREFUSED"); break;
    case WSAELOOP: strcpy(msg, "WSAELOOP"); break;
    case WSAENAMETOOLONG: strcpy(msg, "WSAENAMETOOLONG"); break;
    case WSAEHOSTDOWN: strcpy(msg, "WSAEHOSTDOWN"); break;
    case WSAEHOSTUNREACH: strcpy(msg, "WSAEHOSTUNREACH"); break;
    case WSAENOTEMPTY: strcpy(msg, "WSAENOTEMPTY"); break;
    case WSAEPROCLIM: strcpy(msg, "WSAEPROCLIM"); break;
    case WSAEUSERS: strcpy(msg, "WSAEUSERS"); break;
    case WSAEDQUOT: strcpy(msg, "WSAEDQUOT"); break;
    case WSAESTALE: strcpy(msg, "WSAESTALE"); break;
    case WSAEREMOTE: strcpy(msg, "WSAEREMOTE"); break;

    case WSASYSNOTREADY: strcpy(msg, "WSASYSNOTREADY"); break;
    case WSAVERNOTSUPPORTED: strcpy(msg, "WSAVERNOTSUPPORTED"); break;
    case WSANOTINITIALISED: strcpy(msg, "WSANOTINITIALISED"); break;
    case WSAEDISCON: strcpy(msg, "WSAEDISCON"); break;
    case WSAENOMORE: strcpy(msg, "WSAENOMORE"); break;
    case WSAECANCELLED: strcpy(msg, "WSAECANCELLED"); break;
    case WSAEINVALIDPROCTABLE: strcpy(msg, "WSAEINVALIDPROCTABLE"); break;
    case WSAEINVALIDPROVIDER: strcpy(msg, "WSAEINVALIDPROVIDER"); break;
    case WSAEPROVIDERFAILEDINIT: strcpy(msg, "WSAEPROVIDERFAILEDINIT"); break;
    case WSASYSCALLFAILURE: strcpy(msg, "WSASYSCALLFAILURE"); break;
    case WSASERVICE_NOT_FOUND: strcpy(msg, "WSASERVICE_NOT_FOUND"); break;
    case WSATYPE_NOT_FOUND: strcpy(msg, "WSATYPE_NOT_FOUND"); break;
    case WSA_E_NO_MORE: strcpy(msg, "WSA_E_NO_MORE"); break;
    case WSA_E_CANCELLED: strcpy(msg, "WSA_E_CANCELLED"); break;
    case WSAEREFUSED: strcpy(msg, "WSAEREFUSED"); break;

    case WSAHOST_NOT_FOUND: strcpy(msg, "WSAHOST_NOT_FOUND"); break;
    case WSATRY_AGAIN: strcpy(msg, "WSATRY_AGAIN"); break;
    case WSANO_RECOVERY: strcpy(msg, "WSANO_RECOVERY"); break;
    case WSANO_DATA: strcpy(msg, "WSANO_DATA"); break;

    default: strcpy(msg, "Unknown error!");
  }
  MessageBox(NULL, msg, "WSError", MB_OK);
}
//---------------------------------------------------------------------------
SOCKET ws_socket(short port)
{
  sockaddr_in name;
  SOCKET s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s==INVALID_SOCKET)
    return INVALID_SOCKET;
  memset(&name, 0, sizeof(sockaddr_in));
  name.sin_family=AF_INET;
  name.sin_addr.S_un.S_addr=INADDR_ANY; //inet_addr("x.y.z.t");
  name.sin_port=htons(port);

  if (bind(s, (SOCKADDR*) &name, sizeof(sockaddr_in))==SOCKET_ERROR)
    return INVALID_SOCKET;
  if (listen(s, SOMAXCONN)==SOCKET_ERROR)
    return INVALID_SOCKET;
  return s;
}
//---------------------------------------------------------------------------
SOCKET ws_accept(SOCKET s, char *remoteip)
{
  char t[20];
  sockaddr_in name;
  int namelen=sizeof(sockaddr_in);

  memset(&name, 0, sizeof(sockaddr_in));
  SOCKET client=accept(s, (SOCKADDR* FAR) &name, (int * FAR) &namelen);
  if (client==INVALID_SOCKET)
    return INVALID_SOCKET;
  strcpy(remoteip, itoa(name.sin_addr.S_un.S_un_b.s_b1, t, 10));
  strcat(remoteip, ".");
  strcat(remoteip, itoa(name.sin_addr.S_un.S_un_b.s_b2, t, 10));
  strcat(remoteip, ".");
  strcat(remoteip, itoa(name.sin_addr.S_un.S_un_b.s_b3, t, 10));
  strcat(remoteip, ".");
  strcat(remoteip, itoa(name.sin_addr.S_un.S_un_b.s_b4, t, 10));
  return client;
}
//---------------------------------------------------------------------------
SOCKET ws_connect(char *ip, short port)
{
  sockaddr_in name;
  int namelen=sizeof(sockaddr_in);
  SOCKET s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s==INVALID_SOCKET)
    return INVALID_SOCKET;
  memset(&name, 0, sizeof(sockaddr_in));
  name.sin_family=AF_INET;
  name.sin_addr.S_un.S_addr=INADDR_ANY; //inet_addr("x.y.z.t");

  if (bind(s, (SOCKADDR*) &name, sizeof(sockaddr_in))==SOCKET_ERROR)
    return INVALID_SOCKET;

  if (s==SOCKET_ERROR)
    return INVALID_SOCKET;

  memset(&name, 0, sizeof(sockaddr_in));
  name.sin_family=AF_INET;
  name.sin_addr.S_un.S_addr=inet_addr(ip);
  name.sin_port=htons(port);

  if (connect(s, (SOCKADDR* FAR) &name, namelen)==SOCKET_ERROR)
    return INVALID_SOCKET;
  return s;
}
//---------------------------------------------------------------------------
int ws_deletesocket(SOCKET s)
{
  if (closesocket(s)==SOCKET_ERROR)
    return -1;
  return 0;
}
//---------------------------------------------------------------------------
int ws_send(SOCKET s, char *msg)
{
  if (send(s, msg, strlen(msg), 0))
    return -1;
  return 0;
}
//---------------------------------------------------------------------------
int ws_recieve(SOCKET s, char *msg)
{
  int result=recv(s, msg, 255, 0);
  if ((result==0)||(result==SOCKET_ERROR))
  {
    closesocket(s);
    return -1;
  }
  msg[result]=0;
  return 0;
}
//---------------------------------------------------------------------------
int ws_sendblock(SOCKET s, char *msg, int count)
{
  if (send(s, msg, count, 0))
    return -1;
  return 0;
}
//---------------------------------------------------------------------------
int ws_recieveblock(SOCKET s, char *msg, int count)
{
  int result=recv(s, msg, count, 0);
  if ((result==0)||(result==SOCKET_ERROR))
  {
    closesocket(s);
    return -1;
  }
  return 0;
}
//---------------------------------------------------------------------------
int ws_localname(char *name)
{
  if (gethostname((char * FAR) name, 255)==SOCKET_ERROR)
    return -1;
  return 0;
}
//---------------------------------------------------------------------------
int ws_nametoip(char *name, char ip[20][16], int &count)
{
  char tname[256];
  strcpy(tname, name);
  ip[0][0]=0;
  hostent FAR *hostdetails ;
  hostdetails=gethostbyname(tname) ;
  if (hostdetails==NULL)
    return -1;
  int i=0;
  while (hostdetails->h_addr_list[i]!=NULL)
  {
    for (int j=0; j<hostdetails->h_length; j++)
    {
      strcat(ip[i], itoa((unsigned char)hostdetails->h_addr_list[i][j], tname, 10));
      strcat(ip[i], ".");
    }
    ip[i][strlen(ip[i])-1]=0;
    i++;
  }
  count=i;
  return 0;
}

