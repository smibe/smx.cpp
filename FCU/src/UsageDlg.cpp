/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: UsageDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/UsageDlg.cpp,v 1.1 2002/09/20 09:34:48 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "UsageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CALLBACK EditStreamCallback(
  DWORD dwCookie, // application-defined value
  LPBYTE pbBuff,  // pointer to a buffer
  LONG cb,        // number of bytes to read or write
  LONG *pcb       // pointer to number of bytes transferred
);

/////////////////////////////////////////////////////////////////////////////
// CUsageDlg dialog


CUsageDlg::CUsageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUsageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUsageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUsageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUsageDlg)
	DDX_Control(pDX, IDC_reUsage, m_reUsage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUsageDlg, CDialog)
	//{{AFX_MSG_MAP(CUsageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUsageDlg message handlers

BOOL CUsageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (!m_strError.IsEmpty())
    GetDlgItem(IDC_stError)->SetWindowText(m_strError);

  HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_rtfUsage), "RTF");
  if (hRes)
  {
    m_reUsage.SetBackgroundColor(FALSE, RGB(255, 255, 221));
    HGLOBAL hGlob = LoadResource(AfxGetInstanceHandle(), hRes);
    LPCTSTR pszUsage = (LPCTSTR) LockResource(hGlob);
    m_reUsage.SetWindowText(pszUsage);
    UnlockResource(hGlob);
    
  }
  else
  {
    m_reUsage.ShowWindow(SW_HIDE);
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
