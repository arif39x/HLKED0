
//---------------------------------------------------------------------------
#ifndef arena_winsockH
#define arena_winsockH
//---------------------------------------------------------------------------
#include <winsock2.h>
#include <stdlib.h>
//---------------------------------------------------------------------------
#define WSENODLL -1
#define WSEINVALIDVERSION -2
//---------------------------------------------------------------------------
int ws_init(int highver, int lowver);
int ws_done(void);
int ws_lasterror(void);
void ws_showerror(int err);
SOCKET ws_socket(short port);
SOCKET ws_accept(SOCKET s, char *remoteip);
SOCKET ws_connect(char *ip, short port);
int ws_deletesocket(SOCKET s);
int ws_send(SOCKET s, char *msg);
int ws_recieve(SOCKET s, char *msg);
int ws_sendblock(SOCKET s, char *msg, int count);
int ws_recieveblock(SOCKET s, char *msg, int count);
int ws_localname(char *name);
int ws_nametoip(char *name, char ip[20][16], int &count);
//---------------------------------------------------------------------------
#endif
