// SmxLib.h : main header file for the SMXLIB DLL
//

#if !defined(AFX_SMXLIB_H__4154E4D9_4A86_4883_BB3D_465B9D0ED678__INCLUDED_)
#define AFX_SMXLIB_H__4154E4D9_4A86_4883_BB3D_465B9D0ED678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSmxLibApp
// See SmxLib.cpp for the implementation of this class
//

class CSmxLibApp : public CWinApp
{
public:
	CSmxLibApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmxLibApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSmxLibApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMXLIB_H__4154E4D9_4A86_4883_BB3D_465B9D0ED678__INCLUDED_)
