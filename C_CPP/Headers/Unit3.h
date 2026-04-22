//---------------------------------------------------------------------------
#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <NMMSG.hpp>
#include <Psock.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TMemo *Memo1;
    TGroupBox *GroupBox2;
    TEdit *Edit1;
    TNMMSGServ *NMMSGServ1;
    TNMMSGServ *NMMSGServ2;
    TNMMsg *NMMsg1;
    TLabel *Label1;
    void __fastcall NMMSGServ1MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall NMMSGServ2MSG(TComponent *Sender,
          const AnsiString sFrom, const AnsiString sMsg);
    void __fastcall Edit1KeyPress(TObject *Sender, char &Key);
    
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    
    
private:	// User declarations
public:		// User declarations
    __fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
