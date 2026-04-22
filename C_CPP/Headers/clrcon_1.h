/*
    Helper console output func.
    BY CSK
    www.csksoft.net csk@live.com

*/
#include <tchar.h>
#include <windows.h>
#pragma once


namespace consolehelper{



struct CON_CLR
{
    static const WORD RED  =  FOREGROUND_RED;
    static const WORD GREEN = FOREGROUND_GREEN;
    static const WORD BLUE = FOREGROUND_BLUE;

};


BOOL clr_foregnd(WORD clr);
BOOL clr_bkgnd(WORD clr);

BOOL clr_underline(BOOL bUnderLine=TRUE);
BOOL clr_highlight(BOOL bHighlight=TRUE, BOOL foregnd = TRUE);
BOOL clr_reverse();
VOID clr_restore();




VOID con_cls();



}
