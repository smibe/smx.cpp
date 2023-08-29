/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: PasswdDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/PasswdDlg.cpp,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "passwddlg.h"
#include "Crypt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetPasswdKey(LPCTSTR pszRegName);


CPasswdDlg::CPasswdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswdDlg::IDD, pParent)
{
  m_bRegister = false;
	m_nTry = 0;
  //{{AFX_DATA_INIT(CPasswdDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPasswdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswdDlg)
	DDX_Control(pDX, IDC_btnWWW, m_WWW);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswdDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswdDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswdDlg message handlers

void CPasswdDlg::OnOK() 
{
  CTime  Time;
  
  GetDlgItem(IDC_ebKey)->GetWindowText(m_strPasswd);
  GetDlgItem(IDC_ebUserName)->GetWindowText(m_strUserName);

  m_strCode = GetPasswdKey(m_strUserName);

  if (m_strUserName.IsEmpty() || !GetTimeFromPasswd(Time, m_strPasswd, m_strCode))
  {
    if (m_nTry < 3 || m_bRegister)
    {
      AfxMessageBox("Invalid password or user name!");
      GetDlgItem(IDC_ebPasswd)->SetFocus();
      m_nTry++;
      return;
    }
    
    AfxMessageBox("Sorry, you have entered 3 times an invalid password! FCU will terminate.");
    EndDialog(IDCANCEL);
    return;
  }

  EndDialog(IDOK);
}

bool CPasswdDlg::GetTimeFromPasswd(CTime& Time, LPCTSTR pszPasswd, LPCTSTR pszCode)
{
  CCrypt Crypt;
  CString strPasswd = pszPasswd;

  Crypt.DecryptDigits(strPasswd, pszCode);

  if (strPasswd.IsEmpty() || !GetTimeFromString(Time, strPasswd))
    return false;

  return true;
}

bool CPasswdDlg::GetTimeFromString(CTime& Time, LPCTSTR pszPasswd)
{
  CString str = pszPasswd;
  if (str.GetLength() != 6)
    return false;

  int nDays  = atoi(str.Mid(0, 2));
  int nMonth = atoi(str.Mid(2, 2));
  int nYear  = atoi(str.Mid(4, 2)) + 1966;

  if (nDays <= 0 || nDays > 31 || nMonth <= 0 || nMonth > 12 || nYear < 1966 || nYear > 2038)
  {
    return false;
  }

  CTime TempTime(nYear, nMonth, nDays, 0, 0, 0);
  if (TempTime > CTime::GetCurrentTime())
    return false;

  Time = TempTime;
  return true;
}

BOOL CPasswdDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_bRegister)
  {
    CRect rcLine;
    GetDlgItem(IDC_stThanks)->SetWindowText("Thanks for using FCU!");

    GetDlgItem(IDC_stLine1)->GetWindowRect(rcLine);
    GetDlgItem(IDC_stLine1)->SetWindowText("For extending your evaluation password or regis-");
    GetDlgItem(IDC_stLine2)->SetWindowText("ter FCU please contact:");
    m_WWW.SetCommand("http://fcu.smibe.com/Register.htm");
    
    //GetDlgItem(IDC_stLine1)->ShowWindow(SW_HIDE);
    //GetDlgItem(IDC_stLine2)->ShowWindow(SW_HIDE);
    
    GetDlgItem(IDC_stLine3)->ShowWindow(SW_HIDE);
    
    //GetDlgItem(IDC_btnWWW)->ShowWindow(SW_HIDE);

    CSize pt(0, - rcLine.Height() * 1);
    MoveWindow(GetDlgItem(IDC_stPasswd), pt);
    MoveWindow(GetDlgItem(IDC_stUserName), pt);
    MoveWindow(GetDlgItem(IDC_ebUserName), pt);
    MoveWindow(GetDlgItem(IDC_stKey), pt);
    MoveWindow(GetDlgItem(IDC_ebKey), pt);
    MoveWindow(GetDlgItem(IDOK), pt);
    MoveWindow(GetDlgItem(IDCANCEL), pt);
    MoveWindow(this, pt);
  }

  GetDlgItem(IDC_ebUserName)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswdDlg::MoveWindow(CWnd *pWnd, CSize& pt)
{
  CRect rcWnd;

  pWnd->GetWindowRect(rcWnd);
  if (pWnd != this)
  {
    rcWnd += pt;
    ScreenToClient(rcWnd);
  }
  else
  {
    rcWnd.bottom += pt.cy;
  }

  pWnd->MoveWindow(rcWnd);
}
