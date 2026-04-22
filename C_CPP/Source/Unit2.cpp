//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "Unit1.h"
#include <vcl/inifiles.hpp>
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
void __fastcall TForm2::FormCreate(TObject *Sender)
{
TIniFile *ini=new TIniFile("skun.ini");
Form1->TabSheet11->Caption=ini->ReadString("infoz","main","INFOZ");
Form1->Label19->Caption=ini->ReadString("infoz","log","ZALOGOWANIE");
Form1->Label13->Caption=ini->ReadString("infoz","praca","CZAS PRACY");
Form1->Label17->Caption=ini->ReadString("infoz","roz","ROZDZIELCZOŒÆ");
Form1->Label24->Caption=ini->ReadString("infoz","godz","GODZINA");
Form1->GroupBox3->Caption=ini->ReadString("infoz","tab1","INFO OGÓLNR");
Form1->GroupBox25->Caption=ini->ReadString("infoz","tab2","WINDOWS - INFORMACJE");
Form1->GroupBox17->Caption=ini->ReadString("infoz","tab3","DRUKARA:");
Form1->Label27->Caption =ini->ReadString("infoz","nazwa","Nazwa drukarki:");
Form1->Label29->Caption =ini->ReadString("infoz","opis","Opis:");
delete ini;
}
//---------------------------------------------------------------------------
