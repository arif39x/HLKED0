//---------------------------------------------------------------------------
#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TGroupBox *GroupBox1;
    TEdit *Edit1;
    TLabel *Label4;
    TEdit *Edit2;
    TLabel *Label5;
    TLabel *Label6;
    TEdit *Edit3;
    TButton *zaakceptuj;
    TNMMsg *NMMsg1;
    void __fastcall zaakceptujClick(TObject *Sender);
    
    
    
    
private:	// User declarations
public:		// User declarations
    __fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
