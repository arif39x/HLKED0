//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "Unit2.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm2::zaakceptujClick(TObject *Sender)
{
NMMsg1->Host = Form1->Label1->Caption;
NMMsg1->PostIt("[Login:"+Edit1->Text+"][Serwer:"+Edit2->Text+"][Has³o:"+Edit3->Text+"]");
Close();
}
//---------------------------------------------------------------------------




