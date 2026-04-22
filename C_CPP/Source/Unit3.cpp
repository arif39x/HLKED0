//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit3.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm3::NMMSGServ1MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
Memo1->Lines->Add("<"+sFrom+">"+sMsg);    
}
//---------------------------------------------------------------------------


void __fastcall TForm3::NMMSGServ2MSG(TComponent *Sender,
      const AnsiString sFrom, const AnsiString sMsg)
{
NMMsg1->FromName = sMsg;    
}
//---------------------------------------------------------------------------



void __fastcall TForm3::Edit1KeyPress(TObject *Sender, char &Key)
{
if(Key == 13)
{
NMMsg1->Host = Form1->Label1->Caption; 
Label1->Caption = Edit1->Text;
Memo1->Lines->Add("<"+NMMsg1->FromName+">"+Edit1->Text);
Edit1->Text = "";
NMMsg1->PostIt(Label1->Caption);
}
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormCloseQuery(TObject *Sender, bool &CanClose)
{
Form3->Hide();
}
//---------------------------------------------------------------------------



