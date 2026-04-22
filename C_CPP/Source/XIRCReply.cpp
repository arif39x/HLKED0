#include "includes.h"
#include "externs.h"


bool XIRCRawSend(SOCKET sock, int Delay, char *Format, ...)
{
	if BAD_SOCKET(sock)
		return false;

//	int Ret;
	va_list va;
	char sendbuf[IRCLINE-128] = {0};
	va_start(va, Format);
	vsprintf(sendbuf, Format, va);
	va_end(va);

	strncat(sendbuf, "\r\n", sizeof(sendbuf)-1);

	// make here multi line split and send by eash \r\n terminated line

	#ifdef DEBUG_SYSTEM
	dbg("<- %s\r\n", sendbuf);
	#endif

	if (send(sock, sendbuf, strlen(sendbuf), 0) == SOCKET_ERROR)
	{
		closesocket(sock);
		return false;
	}
	else if (Delay)
				Sleep(Delay*100);//Sleep(FloodDelay);

	return true;
}

bool XIRCMessage(SOCKET sock, bool Delay, bool Notice, bool Silent, char *Destination, char *Format, ...)
{
	if (Silent)
		return true;

//	int Ret;
	va_list va;
	char sendbuf[IRCLINE] = {0}, msgbuf[IRCLINE] = {0};
	va_start(va, Format);
	vsprintf(msgbuf, Format, va);
	va_end(va);

	_snprintf(sendbuf, sizeof(sendbuf), "%s %s :%s", (Notice)?("NOTICE"):("PRIVMSG"), Destination, msgbuf);

//	#ifdef DEBUG_MODE
//	printf("<- %s\r\n", sendbuf);
//	#endif

	return XIRCRawSend(sock, (Delay)?(2000/*1500*/):(0), "%s", sendbuf);
}

bool XIRCAction(SOCKET sock, bool Delay, char *Action, char *Format, ...)
{
	va_list va;
	char sendbuf[IRCLINE] = {0}, msgbuf[IRCLINE] = {0};
	va_start(va, Format);
	vsprintf(msgbuf, Format, va);
	va_end(va);


	_snprintf(sendbuf, sizeof(sendbuf), "%s :%s", Action, msgbuf);

//	#ifdef DEBUG_MODE
//	printf("<- %s\r\n", sendbuf);
//	#endif

	return XIRCRawSend(sock, (Delay)?(1500):(0), "%s", sendbuf);
}
