/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: WaitDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/WaitDlg.cpp,v 1.2 2002/11/22 17:21:56 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// WaitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMXRes.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog


CWaitDlg::CWaitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWaitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_nTimerID = 2000;
}


void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaitDlg)
	DDX_Control(pDX, IDC_SMX_btnWWW, m_WWW);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialog)
	//{{AFX_MSG_MAP(CWaitDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg message handlers

BOOL CWaitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont *pFont = GetDlgItem(IDC_stExpired)->GetFont();
  LOGFONT LogFont;
  pFont->GetLogFont(&LogFont);

  LogFont.lfHeight = LogFont.lfHeight * 3 / 2;
  m_LargeFont.CreateFontIndirect(&LogFont);
  GetDlgItem(IDC_stExpired)->SetFont(&m_LargeFont);

  if (!m_strText.IsEmpty())
    GetDlgItem(IDC_stExpired)->SetWindowText(m_strText);

  GetDlgItem(IDOK)->EnableWindow(FALSE);
  m_nTimerID = SetTimer(m_nTimerID, 100, NULL);

  m_Progress.SetRange(0, 100);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaitDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimerID)
  {
    int nPos = m_Progress.GetPos();
    if (nPos == 100)
    {
      GetDlgItem(IDOK)->EnableWindow(TRUE);
      KillTimer(m_nTimerID);
      return;
    }

    nPos++;
    m_Progress.SetPos(nPos);
  }
	
	CDialog::OnTimer(nIDEvent);
}

void CWaitDlg::OnCancel()
{
  return;
}
