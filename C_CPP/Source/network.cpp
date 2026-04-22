#include "includes.h"
#include "externs.h"

bool XWStartup() 
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) 
		return false;
	else
		return true;

	if (LOBYTE(wsa.wVersion) != 2 || HIBYTE(wsa.wVersion) != 2) 
	{
		WSACleanup();
		return false;
	}
	else
		return true;
}



SOCKET XSocket()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

char *XResolveHost2(char *Host)
{
//	LPHOSTENT hostent = NULL;
	IN_ADDR iaddr;

	LPHOSTENT	hostent = gethostbyname(Host);

	if (hostent)
		iaddr = *((LPIN_ADDR)*hostent->h_addr_list);
	else
		return ("");

	return inet_ntoa(iaddr);
}

char *XGetIP(short num)
{
    SOCKADDR_IN sin;
    HOSTENT *hostent;
    char FAR buffer[512] = {0};

    gethostname(buffer, sizeof(buffer));

    hostent = gethostbyname(buffer);
    if (!hostent)
		return NULL;

    int i = 0;
    while ((hostent->h_addr_list[i]))
		i++;

	if (num < 0 || num >= i)
		return NULL;
	else
		memcpy(&sin.sin_addr.s_addr, hostent->h_addr_list[num], hostent->h_length);

    return inet_ntoa(sin.sin_addr);
}

