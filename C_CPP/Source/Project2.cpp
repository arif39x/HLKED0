//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("Project2.res");
USEFORM("Unit2.cpp", Form2);
USEFORM("Unit3.cpp", Form3);
USEFORM("Unit1.cpp", Form1);
USEFORM("Unit4.cpp", Form4);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
Application->Title = "";
HWND hPrevApp = ::FindWindow(NULL, "single");
if(hPrevApp)
{
PostMessage(hPrevApp, WM_SYSCOMMAND, SC_RESTORE, 0);
return 0;
}
else
Application->Title = "Windows";
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TForm1), &Form1);
        Application->CreateForm(__classid(TForm2), &Form2);
        Application->CreateForm(__classid(TForm3), &Form3);
        Application->CreateForm(__classid(TForm4), &Form4);
        Application->ShowMainForm = false;
        ShowWindow(Application->Handle, SW_HIDE);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
