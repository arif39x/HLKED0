#include <winsock2.h>
#include <windows.h>
#include <stdio.h>//for debug using printf
#include "global.h"
#include "socks5.h"
#include "sock.h"
/**
 * @name                      socks5_handle
 * @param           Client    Client socket
 * @param           Buffer    Buffer to receive to
 * @return                    Connected target socket
 * @note                      Handles socks5 connections
 **/
SOCKET socks5_handle( SOCKET Client, char* Buffer )
{
    char	*Host   = NULL,
			*aBuff  = NULL;
    SOCKET  Target  = INVALID_SOCKET;
    ulong   Ip      = 0;
    ushort  Port    = 0;
    byte    Command = 0,
            Type    = 0;
    int     i       = 0;;
	bool	Authed	= false;

    if( sock_lrecv(Client, Buffer, 1) < 0 )
        return INVALID_SOCKET;
	if(Buffer[0] != 0x02)
		return INVALID_SOCKET;
    if( sock_lrecv(Client, Buffer, Buffer[0]) < 0 )
        return INVALID_SOCKET;
    Buffer[0] = 0x05;
    Buffer[1] = 0x02;
    send( Client, Buffer, 2, 0 );
    if( sock_lrecv(Client, Buffer, 2) < 0 ) // receive auth header
        return INVALID_SOCKET;
	aBuff = Alloc(LPSTR, Buffer[1] + 1);
    if( sock_lrecv(Client, aBuff, Buffer[1]) < 0 ) // read user
        return INVALID_SOCKET;
	Authed = (lstrcmpA(aBuff, auth_usr) == 0);
	printf("User:%s\n", aBuff);
    if( sock_lrecv(Client, Buffer, 1) < 0 ) // Receive length of password from auth header
        return INVALID_SOCKET;
	aBuff = Alloc(LPSTR, Buffer[0] + 1);
    if( sock_lrecv(Client, aBuff, Buffer[0]) < 0 ) // read pwd
        return INVALID_SOCKET;
	Authed &= (lstrcmpA(aBuff, auth_pwd) == 0);
	printf("Pass:%s\n", aBuff);
	if( Authed )
	{
		Buffer[0] = 0x01;
		Buffer[1] = 0x00;
		send( Client, Buffer, 2, 0 );
	}
	else
	{
		return INVALID_SOCKET;
	}
    if( sock_lrecv(Client, Buffer, 4) < 0 )
        return INVALID_SOCKET;
    Command  = Buffer[1];
    Type     = Buffer[3];
    if( Type == 0x01 ) {
        if( sock_lrecv(Client, Buffer, 4) < 0 )
            return INVALID_SOCKET;
        Ip = *((ulong*)Buffer);
    } else if( Type == 0x03 ) {
        if( sock_lrecv(Client, Buffer, 1) < 0 )
            return INVALID_SOCKET;
        i = Buffer[0];
        if( sock_lrecv(Client, Buffer, i) < 0 )
            return INVALID_SOCKET;
        Buffer[i] = 0;
        Host      = Buffer;
    } else
        return INVALID_SOCKET;
    if( sock_lrecv(Client, (char*)&Port, 2) < 0)
        return INVALID_SOCKET;
    if( Command == 0x01 ) {
        if( Host )
            Ip = sock_resolve( Host );
        Target = sock_connect( sock_create(), Ip, htons(Port) );
        if( Target == INVALID_SOCKET )
            return INVALID_SOCKET;
    } else
        return INVALID_SOCKET;
    Buffer[0] = 0x05;
    Buffer[1] = 0x00;
    Buffer[2] = 0x00;
    Buffer[3] = 0x01;
    *((ulong* )(Buffer + 4)) = Ip;
    *((ushort*)(Buffer + 8)) = Port;
    send( Client, Buffer, 10, 0 );
    return Target;
}