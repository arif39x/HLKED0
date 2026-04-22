/*
    Guptachar IRCBOT Module
    Copyright (C) 2002 Ayan Chakrabarti

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    irc.cpp 
    ======= 

    Implementation of the IRC Bot module
*/


#include <winsock.h>
#include <string.h>
#include <stdio.h>

char buf[1024];
int  bpos;

struct _ircopts
{
        char nick[1024];
        char host[1024];
        unsigned int port;

        char ifuseproxy;

        char proxy[1024];
        unsigned int pport;
} ircopts;


void setircopts(char * nick, char * host, unsigned int port, char ifuseproxy, char * proxy, unsigned int pport)
{
        strcpy(ircopts.nick,nick);
        strcpy(ircopts.host,host);
        ircopts.port = port;

        ircopts.ifuseproxy = ifuseproxy;

        if(ifuseproxy)
        {
                strcpy(ircopts.proxy,proxy);
                ircopts.pport = pport;
        }
}


int docheck(char * str)
{
        int i,j;
        for(i = 0;i < bpos;i++)
                if(buf[i] == '\r' || buf[i] == '\n')
                        break;
        if(i == bpos)
                return 0;

        for(j = 0;j < i;j++)
                str[j] = buf[j];

        str[j] = '\0';

        for(;(buf[i] == '\r' || buf[i] == '\n') && i < bpos;i++);

        for(j = 0;i < bpos;i++,j++)
                buf[j] = buf[i];
        bpos = j;
        return 1;
}


int readline(SOCKET s,char * str)
{
        int n;

        while(1)
        {
                if(docheck(str))
                        return 1;
                if(bpos >= 1023)
                        bpos = 0;
                n = recv(s,&buf[bpos],1023-bpos,0);
                if(n <= 0)
                        return 0;
                bpos += n;
                
        }
}

void makenick(char * user, unsigned int * np, char * nick)
{
        unsigned int i,j;

        for(i = 0;user[i] != '\0';i++)
                nick[i] = user[i];
        (*np)++;

        j = *np;
        while(j != 0)
        {
                nick[i++] = '0' + j % 10;
                j /= 10;
        }
        nick[i] = '\0';
}

void getip(char * ip)
{
	char name[257];
	struct hostent far * he;
        unsigned char a,b,c,d;

        gethostname(name,256);
        he = gethostbyname(name);

        a = he->h_addr[0];
        b = he->h_addr[1];
        c = he->h_addr[2];
        d = he->h_addr[3];


        sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
}


void doirc(char * server,unsigned int sport,char * user, char ifuseproxy, char * proxy, unsigned int pport)
{
        SOCKET s;
        char nick[512];
        struct hostent * hs;
        struct sockaddr_in sa;
        char outb[1024],buf2[1024];
        unsigned int np = 0,i;
        char * host;
        unsigned int port;
        char ip[256];

        strcpy(nick,user);
        bpos = 0;

        if(ifuseproxy)
        {
                host = proxy;
                port = pport;
        }
        else
        {
                host = server;
                port = sport;
        }

        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        if((sa.sin_addr.s_addr = inet_addr(host)) == -1)
        {
                if((hs = gethostbyname(host)) == NULL)
                        return;
                memcpy(&sa.sin_addr,hs->h_addr,sizeof(struct in_addr));
        }

        s = socket(AF_INET,SOCK_STREAM,0);
        if(s == INVALID_SOCKET)
                return;

        if(connect(s,(struct sockaddr *)&sa,sizeof(sa)) < 0)
                return;

        if(ifuseproxy)
                sprintf(outb,"CONNECT %s:%d HTTP/1.0\r\n\r\n",server,sport);
        else
                outb[0] = '\0';

        getip(ip);
        strcat(outb,"NICK ");
        strcat(outb,nick);
        strcat(outb,"\r\nUSER ");
        strcat(outb,nick);
        strcat(outb," 0 * :");
        strcat(outb,ip);
        strcat(outb,"\r\n");
        send(s,outb,strlen(outb),0);

        while(1)
        {
                if(!readline(s,buf2))
                        break;

                if(strncmpi(buf2,"PING ",5) == 0)
                {
                        buf2[1] = 'O';
                        strcpy(outb,buf2);
                        strcat(outb,"\r\n");
                        send(s,outb,strlen(outb),0);
                }
                else
                {
                        for(i = 0;buf2[i] != ' ' && buf2[i] != '\0';i++);
                        for(;buf2[i] == ' ';i++);
                        if(strncmp(&buf2[i],"433",3) == 0)
                        {
                                makenick(user,&np,nick);
                                strcpy(outb,"NICK ");
                                strcat(outb,nick);
                                strcat(outb,"\r\n");
                                send(s,outb,strlen(outb),0);
                        }
                        
                }
        }
        closesocket(s);
        
}

int isonline()
{
	char name[257];
	struct hostent far * he;

        gethostname(name,256);
        he = gethostbyname(name);
        if(he == NULL)
                return 0;

        if(he->h_addr[0] == 127)
		return 0;
	else
		return 1;
}


long WINAPI ircbot(LPVOID t)
{
        while(1)
        {
              if(isonline())
                      doirc(ircopts.host, ircopts.port, ircopts.nick,
                            ircopts.ifuseproxy, ircopts.proxy,
                            ircopts.pport);
        }
        return 3;
}


void startbot(void)
{
        HANDLE h;

        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) ircbot,(LPVOID) 0,0,(LPDWORD) &h);
        if(h)
                CloseHandle(h);
}
