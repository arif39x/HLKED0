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

    service.c
    =========

    Registering Guptachar as a system service.
*/


#include <windows.h>

void becomeservice(void)
{
        HMODULE hKERNEL32;
        FARPROC a_Register;

        if ( (hKERNEL32 = GetModuleHandle("KERNEL32.DLL")) != NULL)
                if( ( a_Register = GetProcAddress(hKERNEL32,"RegisterServiceProcess")) != NULL)
                        a_Register( GetCurrentProcessId(), 1);

}
