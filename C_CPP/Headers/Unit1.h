//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <NMsmtp.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TStatusBar *StatusBar1;
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TTabSheet *TabSheet3;
    TTabSheet *TabSheet5;
    TTabSheet *TabSheet6;
    TTabSheet *TabSheet7;
    TTabSheet *TabSheet8;
    TTabSheet *TabSheet9;
    TLabel *Label2;
    TGroupBox *GroupBox2;
    TLabel *Label6;
    TButton *Button1;
    TGroupBox *GroupBox4;
    TButton *Button2;
    TButton *Button3;
    TGroupBox *GroupBox5;
    TGroupBox *GroupBox6;
    TButton *Button8;
    TButton *Button9;
    TGroupBox *GroupBox9;
    TButton *Button11;
    TGroupBox *GroupBox10;
    TGroupBox *GroupBox11;
    TGroupBox *GroupBox12;
    TGroupBox *GroupBox13;
    TGroupBox *GroupBox14;
    TGroupBox *GroupBox15;
    TGroupBox *GroupBox16;
    TNMMsg *NMMsg1;
    TNMMsg *NMMsg2;
    TNMMSGServ *NMMSGServ1;
    TButton *Button12;
    TNMMsg *NMMsg3;
    TNMMSGServ *NMMSGServ3;
    TNMMSGServ *NMMSGServ4;
    TNMMsg *NMMsg4;
    TGroupBox *GroupBox19;
    TEdit *Edit5;
    TButton *Button14;
    TEdit *Edit6;
    TButton *Button15;
    TLabel *Label1;
    TMemo *Memo1;
    TButton *Button16;
    TNMMsg *NMMsg5;
    TButton *Button17;
    TButton *Button18;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TNMMSGServ *NMMSGServ2;
    TSaveDialog *SaveDialog2;
    TMemo *Memo3;
    TButton *Button20;
    TButton *Button21;
    TNMMSGServ *NMMSGServ5;
    TMemo *Memo5;
    TMemo *Memo6;
    TButton *Button22;
    TMemo *Memo7;
    TButton *Button23;
    TMemo *Memo8;
    TButton *Button24;
    TButton *Button25;
    TNMMsg *NMMsg6;
    TMemo *Memo9;
    TLabel *Label4;
    TLabel *Label5;
    TButton *Button26;
    TNMMSGServ *NMMSGServ6;
    TMemo *Memo10;
    TEdit *Edit2;
    TLabel *Label7;
    TLabel *Label8;
    TEdit *Edit7;
    TLabel *Label9;
    TMemo *Memo11;
    TNMSMTP *NMSMTP1;
    TTimer *Timer2;
    TButton *Button27;
    TButton *Button28;
    TTabSheet *TabSheet4;
    TGroupBox *GroupBox7;
    TGroupBox *GroupBox8;
    TMemo *Memo12;
    TButton *Button29;
    TButton *Button30;
    TButton *Button31;
    TNMMSGServ *NMMSGServ7;
    TTabSheet *TabSheet10;
    TMemo *Memo13;
    TEdit *Edit10;
    TGroupBox *GroupBox21;
    TButton *Button32;
    TEdit *Edit8;
    TEdit *Edit9;
    TButton *Button33;
    TButton *Button34;
    TLabel *Label10;
    TNMMsg *NMMsg7;
    TNMMsg *NMMsg8;
    TNMMSGServ *NMMSGServ8;
    TMemo *Memo14;
    TLabel *Label11;
    TLabel *Label12;
    TButton *Button35;
    TButton *Button36;
    TMemo *Memo15;
    TButton *Button37;
    TGroupBox *GroupBox22;
    TButton *Button38;
    TButton *Button39;
    TGroupBox *GroupBox23;
    TEdit *Edit11;
    TButton *Button40;
    TButton *Button41;
    TNMMsg *NMMsg9;
    TNMMSGServ *NMMSGServ9;
    TButton *Button4;
    TButton *Button5;
    TGroupBox *GroupBox24;
    TButton *Button6;
    TButton *Button7;
    TNMMSGServ *NMMSGServ10;
    TButton *Button10;
    TGroupBox *GroupBox26;
    TEdit *Edit13;
    TButton *Button44;
    TNMMSGServ *NMMSGServ11;
    TNMMsg *NMMsg10;
    TNMMsg *NMMsg11;
    TNMMSGServ *NMMSGServ12;
    TMemo *Memo4;
    TMemo *Memo16;
    TNMMSGServ *NMMSGServ13;
    TGroupBox *GroupBox27;
    TLabel *Label26;
    TEdit *Edit14;
    TButton *Button47;
    TNMMsg *NMMsg12;
    TButton *Button46;
    TNMMSGServ *NMMSGServ14;
    TNMMsg *NMMsg13;
    TGroupBox *GroupBox1;
    TButton *Button48;
    TLabel *Label3;
    TEdit *Edit3;
    TTabSheet *TabSheet11;
    TGroupBox *GroupBox3;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label19;
    TLabel *Label20;
    TLabel *Label21;
    TLabel *Label22;
    TLabel *Label13;
    TLabel *Label14;
    TLabel *Label17;
    TLabel *Label18;
    TLabel *Label23;
    TLabel *Label24;
    TLabel *Label25;
    TGroupBox *GroupBox25;
    TMemo *Memo2;
    TGroupBox *GroupBox17;
    TLabel *Label27;
    TLabel *Label28;
    TLabel *Label29;
    TLabel *Label30;
    TLabel *Label31;
    TLabel *Label32;
    TNMMSGServ *NMMSGServ15;
    TNMMSGServ *NMMSGServ16;
    TNMMSGServ *NMMSGServ17;
    TGroupBox *GroupBox20;
    TButton *Button19;
    TButton *Button42;
    TTabSheet *TabSheet12;
    TGroupBox *GroupBox28;
    TButton *Button43;
    TButton *Button45;
    TGroupBox *GroupBox29;
    TButton *Button49;
    TButton *Button50;
    TGroupBox *GroupBox30;
    TButton *Button51;
    TButton *Button52;
    TGroupBox *GroupBox31;
    TButton *Button53;
    TGroupBox *GroupBox32;
    TButton *Button54;
    TButton *Button55;
    TGroupBox *GroupBox33;
    TButton *Button56;
    TGroupBox *GroupBox34;
    TButton *Button57;
    TGroupBox *GroupBox35;
    TButton *Button58;
    TGroupBox *GroupBox36;
    TImage *Image1;
    TLabel *Label33;
    TImage *Image2;
    TLabel *Label34;
    TSpeedButton *SpeedButton2;
    TSpeedButton *SpeedButton3;
    TSpeedButton *SpeedButton4;
    TLabel *Label35;
    TLabel *Label36;
    TGroupBox *GroupBox37;
    TImage *Image3;
    TRadioButton *RadioButton1;
    TImage *Image4;
    TRadioButton *RadioButton2;
    TImage *Image5;
    TRadioButton *RadioButton3;
    TImage *Image6;
    TRadioButton *RadioButton4;
    TEdit *Edit12;
    TButton *Button59;
    TButton *Button60;
    TGroupBox *GroupBox18;
    TCheckBox *CheckBox1;
    TCheckBox *CheckBox2;
    TCheckBox *CheckBox3;
    TCheckBox *CheckBox4;
    TCheckBox *CheckBox5;
    TCheckBox *CheckBox6;
    TButton *Button13;
    TNMMSGServ *NMMSGServ18;
    TGroupBox *GroupBox38;
    TEdit *Edit4;
    TButton *Button61;
    TLabel *Label38;
    TLabel *Label39;
    TNMMSGServ *NMMSGServ19;
    TNMMsg *NMMsg14;
    TNMMSGServ *NMMSGServ20;
    TComboBox *Edit1;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall NMMSGServ1MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall Button8Click(TObject *Sender);
    void __fastcall Button9Click(TObject *Sender);
    void __fastcall Button12Click(TObject *Sender);
    void __fastcall NMMsg1ConnectionFailed(TObject *Sender);
    void __fastcall NMMsg2ConnectionFailed(TObject *Sender);
    void __fastcall NMMSGServ3MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall Button5Click(TObject *Sender);
    void __fastcall Button14Click(TObject *Sender);
    
    void __fastcall Button16Click(TObject *Sender);
    void __fastcall Button17Click(TObject *Sender);
    void __fastcall Button18Click(TObject *Sender);
    void __fastcall NMMSGServ2MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button19Click(TObject *Sender);
    void __fastcall Button21Click(TObject *Sender);
    void __fastcall NMMSGServ5MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button20Click(TObject *Sender);
    void __fastcall Button11Click(TObject *Sender);
    void __fastcall Button22Click(TObject *Sender);
    void __fastcall Button23Click(TObject *Sender);
    void __fastcall Button25Click(TObject *Sender);
    void __fastcall Button26Click(TObject *Sender);
    
    void __fastcall NMMSGServ6MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Timer2Timer(TObject *Sender);
    void __fastcall Button27Click(TObject *Sender);
    void __fastcall Button28Click(TObject *Sender);
    
    void __fastcall Button31Click(TObject *Sender);
    void __fastcall Button29Click(TObject *Sender);
    void __fastcall NMMSGServ7MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button30Click(TObject *Sender);
    
    void __fastcall Button34Click(TObject *Sender);
    void __fastcall Edit10KeyPress(TObject *Sender, char &Key);
    void __fastcall Button33Click(TObject *Sender);
    void __fastcall Button32Click(TObject *Sender);
    void __fastcall NMMSGServ8MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    
    void __fastcall Button36Click(TObject *Sender);
    void __fastcall Button38Click(TObject *Sender);
    void __fastcall Button39Click(TObject *Sender);
    void __fastcall Button40Click(TObject *Sender);
    void __fastcall Button41Click(TObject *Sender);
    void __fastcall NMMSGServ9MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button6Click(TObject *Sender);
    void __fastcall Button7Click(TObject *Sender);
    void __fastcall NMMSGServ10MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ11MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button45Click(TObject *Sender);
    void __fastcall Button44Click(TObject *Sender);
    void __fastcall Button15Click(TObject *Sender);
    void __fastcall Button35Click(TObject *Sender);
    void __fastcall NMMSGServ12MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button37Click(TObject *Sender);
    void __fastcall NMMSGServ13MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    
    void __fastcall Button47Click(TObject *Sender);
    void __fastcall NMMSGServ14MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button46Click(TObject *Sender);
    void __fastcall NMMSGServ15MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ16MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ17MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button42Click(TObject *Sender);
    void __fastcall Button49Click(TObject *Sender);
    void __fastcall Button50Click(TObject *Sender);
    void __fastcall Button53Click(TObject *Sender);
    void __fastcall Button51Click(TObject *Sender);
    void __fastcall Button54Click(TObject *Sender);
    void __fastcall Button55Click(TObject *Sender);
    void __fastcall Button56Click(TObject *Sender);
    void __fastcall Button57Click(TObject *Sender);
    void __fastcall Button43Click(TObject *Sender);
    void __fastcall Button58Click(TObject *Sender);
    
    void __fastcall SpeedButton2Click(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall SpeedButton3Click(TObject *Sender);
    void __fastcall SpeedButton4Click(TObject *Sender);
    void __fastcall Button59Click(TObject *Sender);
    void __fastcall Button60Click(TObject *Sender);
    void __fastcall Button13Click(TObject *Sender);
    void __fastcall NMMSGServ18MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ19MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Button61Click(TObject *Sender);
    void __fastcall NMMSGServ20MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
