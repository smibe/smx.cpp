// FcuGenKey.h : main header file for the FCUGENKEY application
//

#if !defined(AFX_FCUGENKEY_H__49CC1702_48F3_472F_BF6B_880CD10ADDB8__INCLUDED_)
#define AFX_FCUGENKEY_H__49CC1702_48F3_472F_BF6B_880CD10ADDB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyApp:
// See FcuGenKey.cpp for the implementation of this class
//

class CFcuGenKeyApp : public CWinApp
{
public:
	CFcuGenKeyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFcuGenKeyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFcuGenKeyApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCUGENKEY_H__49CC1702_48F3_472F_BF6B_880CD10ADDB8__INCLUDED_)
