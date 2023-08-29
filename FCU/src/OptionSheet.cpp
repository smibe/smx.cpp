/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: OptionSheet.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/OptionSheet.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "OptionSheet.h"
#include "DisplayPg.h"
#include "ComparePg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(COptionSheet, CPropertySheet)

COptionSheet::COptionSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_psh.dwFlags |= PSH_USEPAGELANG;
	//AddPage(&m_displayPg);
	//AddPage(&m_colorPg);
	//AddPage(&m_comparePg);

}

COptionSheet::COptionSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_psh.dwFlags |= PSH_USEPAGELANG;
	//AddPage(&m_displayPg);
	//AddPage(&m_colorPg);
	//AddPage(&m_comparePg);
}

COptionSheet::~COptionSheet()
{
}


BEGIN_MESSAGE_MAP(COptionSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COptionSheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionSheet message handlers

BOOL COptionSheet::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bResult = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	if(GetDlgItem(ID_HELP))
		GetDlgItem(ID_HELP)->SetWindowText("Help");

	if(GetDlgItem(IDCANCEL))
		GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
	
	return bResult;
}

int COptionSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	/**
	CWnd *pWndCtrl = GetWindow(GW_CHILD);;
	
	do
	{
		CString strWndText;
		pWndCtrl->GetWindowText(strWndText);
		TCHAR buf[10];
		GetClassName(GetSafeHwnd(), buf, 6);
		if(!_tcsncmp(buf, _T("Button"), 6) && strWndText == "Hilfe")
		{
			pWndCtrl->SetWindowText("Help");
			int n = pWndCtrl->GetDlgCtrlID();
		}

		if(!_tcsncmp(buf, _T("Button"), 6) && strWndText == "Abbrechen")
		{
			pWndCtrl->SetWindowText("Cancel");
			int n = pWndCtrl->GetDlgCtrlID();
		}

	} while(pWndCtrl = pWndCtrl->GetNextWindow());
	***/

	return 0;
}
