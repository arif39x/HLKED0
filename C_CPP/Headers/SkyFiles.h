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
// SkyFiles.h: Schnittstelle für die Klasse CSkyFiles.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKYFILES_H__959CC523_FDBA_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYFILES_H__959CC523_FDBA_11D3_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "SkyFiles.h"

class CSkyFiles : public CObject 
{
public:
	int m_iActiveColumn;
	int m_iFileCounter;
	//vector<CString> m_sFileName;
	CStringArray m_sFileName;
	CStringArray m_sTypeName;
	void AddFile(CString sFileName);
	void AddType(CString sTypeName);
	CSkyFiles();
	virtual ~CSkyFiles();
};

#endif // !defined(AFX_SKYFILES_H__959CC523_FDBA_11D3_A059_00C026A21151__INCLUDED_)
