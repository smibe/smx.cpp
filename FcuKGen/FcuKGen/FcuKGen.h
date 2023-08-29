// FCUKeyGen.h : main header file for the FCUKEYGEN DLL
//

#if !defined(AFX_FCUKEYGEN_H__BCA8CE7E_4AAD_4FBA_971A_AB6008BC3F5A__INCLUDED_)
#define AFX_FCUKEYGEN_H__BCA8CE7E_4AAD_4FBA_971A_AB6008BC3F5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#ifdef KEYGEN_API
#define DLLEXPORT __declspec( dllexport )
#else 
#define DLLEXPORT __declspec( dllimport )
#endif


class CFCUKeyGenApp : public CWinApp
{
public:
	CFCUKeyGenApp();

  //------------------ Overrides
	//{{AFX_VIRTUAL(CFCUKeyGenApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFCUKeyGenApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

};

class DLLEXPORT CFCUKeyGen
{
public:
  CFCUKeyGen();
  int GenKeyEx(char * args[], char *pszResult1, char *pszResult2, int nBufSize);

  LPCTSTR GetKey(LPCTSTR pszRegName);

protected:
  bool ParseInput(char *args[]);

protected:
  LPCTSTR m_pszRegName;
  LPCTSTR m_pszEmail;
  UINT m_nLanguage;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCUKEYGEN_H__BCA8CE7E_4AAD_4FBA_971A_AB6008BC3F5A__INCLUDED_)
