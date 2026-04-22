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
/**********************************************************************************//
// SkyFiles.cpp: Implementierung der Klasse CSkyFiles.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkyFiles.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CSkyFiles::CSkyFiles()
{
	m_iFileCounter = -1;	
	m_iActiveColumn = 0;
}

CSkyFiles::~CSkyFiles()
{

}

void CSkyFiles::AddFile(CString sFileName)
{
	m_iFileCounter++;
	m_sFileName.Add(sFileName);
	///Type setzen
	CString sType;
	int typeBreak = sFileName.Find(".");
	if (typeBreak != -1)
	{
		int strLeng = sFileName.GetLength();
		sType = sFileName.Right(strLeng - typeBreak - 1);
		sType.MakeUpper();
		AddType(sType);
	}
	else AddType("");
}

void CSkyFiles::AddType(CString sTypeName)
{
	m_sTypeName.Add(sTypeName);
}