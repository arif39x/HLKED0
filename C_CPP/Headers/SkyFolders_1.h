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
// SkyFolders.h: Schnittstelle für die Klasse CSkyFolders.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKYFOLDERS_H__8AB9ADC3_FEB0_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYFOLDERS_H__8AB9ADC3_FEB0_11D3_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkyFolders : public CObject
{
public:
	bool m_bFolderStatus;
	CString m_sFolderPath;
	CString m_sFolderName;
	CSkyFolders();
	virtual ~CSkyFolders();

};

#endif // !defined(AFX_SKYFOLDERS_H__8AB9ADC3_FEB0_11D3_A059_00C026A21151__INCLUDED_)
