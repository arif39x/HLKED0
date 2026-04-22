/*
    Helper console output func.
    BY CSK
    www.csksoft.net csk@live.com

*/
#include "clrcon.h"

static HANDLE std_out = INVALID_HANDLE_VALUE;
static WORD org_clr_info = NULL;


static inline void get_stdout()
{
    if (std_out == INVALID_HANDLE_VALUE)
    {
        std_out = GetStdHandle(STD_OUTPUT_HANDLE);
        if (std_out != INVALID_HANDLE_VALUE)
        {
          //  org_clr_info = 
            CONSOLE_SCREEN_BUFFER_INFO init_info;
            
            if   (GetConsoleScreenBufferInfo(std_out,  &init_info)) 
            {
                org_clr_info = init_info.wAttributes;
            }

        }
    }
}



namespace consolehelper{




BOOL clr_foregnd(WORD clr)
{
    CONSOLE_SCREEN_BUFFER_INFO init_info;
    WORD curr_clr_info;
    get_stdout();

    if   (!GetConsoleScreenBufferInfo(std_out,  &init_info)) return FALSE;
    curr_clr_info = init_info.wAttributes & (~(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED 
                                               ));
    curr_clr_info |= clr;
    return SetConsoleTextAttribute(std_out, curr_clr_info);

}

BOOL clr_bkgnd(WORD clr)
{
    get_stdout();
    CONSOLE_SCREEN_BUFFER_INFO init_info;
    WORD curr_clr_info;
    get_stdout();

    if   (!GetConsoleScreenBufferInfo(std_out,  &init_info)) return FALSE;
    curr_clr_info = init_info.wAttributes & (~( 
                                               BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED));
    if (clr & CON_CLR::RED)
    {
        curr_clr_info |= BACKGROUND_RED;
    }
    if (clr & CON_CLR::GREEN)
    {
        curr_clr_info |= BACKGROUND_GREEN;
    }
    if (clr & CON_CLR::BLUE)
    {
        curr_clr_info |= BACKGROUND_BLUE;
    }
    return SetConsoleTextAttribute(std_out, curr_clr_info);

}

BOOL clr_highlight(BOOL bHighlight, BOOL foregnd)
{
    get_stdout();
    CONSOLE_SCREEN_BUFFER_INFO init_info;
    WORD curr_clr_info;
    get_stdout();
    WORD selector = foregnd?FOREGROUND_INTENSITY:BACKGROUND_INTENSITY;
    if   (!GetConsoleScreenBufferInfo(std_out,  &init_info)) return FALSE;
    curr_clr_info = init_info.wAttributes & (~selector);
    
    curr_clr_info|= bHighlight?selector:0;
    return SetConsoleTextAttribute(std_out, curr_clr_info);


}

BOOL clr_underline(BOOL bUnderLine)
{
    get_stdout();
    CONSOLE_SCREEN_BUFFER_INFO init_info;
    WORD curr_clr_info;
    get_stdout();
   
    if   (!GetConsoleScreenBufferInfo(std_out,  &init_info)) return FALSE;
    curr_clr_info = init_info.wAttributes & (~COMMON_LVB_UNDERSCORE);
    
    curr_clr_info|= bUnderLine?COMMON_LVB_UNDERSCORE:0;
    return SetConsoleTextAttribute(std_out, curr_clr_info);


}

BOOL clr_reverse()
{
    get_stdout();
    CONSOLE_SCREEN_BUFFER_INFO init_info;
    WORD curr_clr_info = 0;
    get_stdout();
   
    if   (!GetConsoleScreenBufferInfo(std_out,  &init_info)) return FALSE;
    curr_clr_info = init_info.wAttributes & (~(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED  |
                                   BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | 
                                   FOREGROUND_INTENSITY | BACKGROUND_INTENSITY));
    if (init_info.wAttributes & BACKGROUND_RED)
    {
        curr_clr_info |= FOREGROUND_RED;
    }
    if (init_info.wAttributes & BACKGROUND_GREEN)
    {
        curr_clr_info |= FOREGROUND_GREEN;
    }
    if (init_info.wAttributes & BACKGROUND_BLUE)
    {
        curr_clr_info |= FOREGROUND_BLUE;
    }

    if (init_info.wAttributes &  FOREGROUND_RED)
    {
        curr_clr_info |= BACKGROUND_RED;
    }
    if (init_info.wAttributes &  FOREGROUND_GREEN)
    {
        curr_clr_info |= BACKGROUND_GREEN;
    }
    if (init_info.wAttributes &  FOREGROUND_BLUE)
    {
        curr_clr_info |= BACKGROUND_BLUE;
    }
    if (init_info.wAttributes &  FOREGROUND_INTENSITY)
    {
        curr_clr_info |= BACKGROUND_INTENSITY;
    }
    if (init_info.wAttributes &  BACKGROUND_INTENSITY)
    {
        curr_clr_info |= FOREGROUND_INTENSITY;
    }

    return SetConsoleTextAttribute(std_out, curr_clr_info);
}

VOID clr_restore()
{
    if (org_clr_info == NULL) return;
    get_stdout();
    SetConsoleTextAttribute(std_out,   org_clr_info);   

}

VOID con_cls()
{
    get_stdout();
    //from microsoft

    COORD coordScreen = { 0, 0 };    /* here's where we'll home the
                                        cursor */ 
    BOOL bSuccess;
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */ 
    DWORD dwConSize;                 /* number of character cells in
                                        the current buffer */ 

    /* get the number of character cells in the current buffer */ 

    bSuccess = GetConsoleScreenBufferInfo( std_out, &csbi );
   
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    /* fill the entire screen with blanks */ 

    bSuccess = FillConsoleOutputCharacter( std_out, (TCHAR) ' ',
       dwConSize, coordScreen, &cCharsWritten );
    

    /* get the current text attribute */ 

    bSuccess = GetConsoleScreenBufferInfo( std_out, &csbi );


    /* now set the buffer's attributes accordingly */ 

    bSuccess = FillConsoleOutputAttribute( std_out, csbi.wAttributes,
       dwConSize, coordScreen, &cCharsWritten );


    /* put the cursor at (0, 0) */ 

    bSuccess = SetConsoleCursorPosition( std_out, coordScreen );

    return;

}



}
