/*
    Guptachar 2.0
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

    main.cpp
    ========

    The main module of Guptachar 2.0
*/

#include <windows.h>
#include <winsock.h>
#include "gserver.h"

extern "C" {void becomeservice(void);}
unsigned int initopts(void);


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine, int nCmdShow)
{
        GuptServer s;
        MSG msg;
        WSADATA ws;
        unsigned int port;
        
	becomeservice();
        port = initopts();

	WSAStartup(0x101,&ws);

        s.setPort(port);
        s.run();

        while(GetMessage(&msg,NULL,0,0))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }

        s.stop();
        WSACleanup();
        ExitProcess(0);
}

