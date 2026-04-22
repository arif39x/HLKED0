/*
Copyright (C) 2000-2001 Marius David.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// decode.cpp

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>


#define DEF_OPENFILE "encode_in.txt"    // encoded input data
#define DEF_SAVEFILE "decode_out.txt"   // decoded output data

//---------------------------------------------------------------------------
char chValidChar[100]   = "<>(): -=,+/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char chEncodedChar[100] = "8,+uvw(opq56RScdefg)DIJKV> WXrsx/0MNhi:jTU9ABC=3OPQF<-4YZabklmnt12LE7GHyz";
//---------------------------------------------------------------------------
void decode(char *chText)
{
	unsigned int ctlen=strlen(chValidChar);
	for (unsigned int i=0; i<strlen(chText); i++)
		for (unsigned int j=0; j<ctlen; j++)
			if (chText[i]==chEncodedChar[j])
			{
				chText[i]=chValidChar[(ctlen-i%ctlen+j)%ctlen];
				break;
			}
};
//---------------------------------------------------------------------------
int savefile(char *name, char *buffer)
{
	FILE *f;
	f=fopen(name, "wb");
	if (f==NULL)
		return -1;
	fputs(buffer, f);
	fclose(f);
	return 0;
}
//---------------------------------------------------------------------------
int openfile(char *fname, char *buffer)
{
	FILE *f;
	char name[500];
	f=fopen(fname, "rb");
	if (f==NULL)
		return -1;
	while (! feof(f))
	{
		name[0]=0;
		fgets(name, 5000, f);
		strcat(buffer, name);
	}
	fclose(f);
	return 0;
}
//---------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	char buffer[5000]="";

	openfile(DEF_OPENFILE, buffer);
	decode(buffer);
	savefile(DEF_SAVEFILE, buffer);
	return 0;
}
//---------------------------------------------------------------------------

