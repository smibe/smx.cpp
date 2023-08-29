// FcuGenKey.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FcuGenKey.h"
#include "FcuGenKeyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyApp

BEGIN_MESSAGE_MAP(CFcuGenKeyApp, CWinApp)
	//{{AFX_MSG_MAP(CFcuGenKeyApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyApp construction

CFcuGenKeyApp::CFcuGenKeyApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFcuGenKeyApp object

CFcuGenKeyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyApp initialization

BOOL CFcuGenKeyApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CFcuGenKeyDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
