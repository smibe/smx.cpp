/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: PasswdDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/PasswdDlg.h,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#if !defined(AFX_PASSWDDLG_H__B5FED846_B3E8_11D5_B2D2_000000000000__INCLUDED_)
#define AFX_PASSWDDLG_H__B5FED846_B3E8_11D5_B2D2_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// passwddlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswdDlg dialog
#include "SHExecCtrl.h"

class CPasswdDlg : public CDialog
{
// Construction
public:
	CPasswdDlg(CWnd* pParent = NULL);   // standard constructor

  static bool GetTimeFromString(CTime& Time, LPCTSTR pszPasswd);
  static bool GetTimeFromPasswd(CTime& Time, LPCTSTR pszPasswd, LPCTSTR pszCode);
  void SetCode(LPCTSTR pszCode) { m_strCode = pszCode; }
  void GetPasswd(CString& strPasswd) { strPasswd = m_strPasswd; }
  void MoveWindow(CWnd *pWnd, CSize& pt);

  void GetCode(CString& strCode) { strCode = m_strCode; }

  void GetUserName(CString& strUserName) { strUserName = m_strUserName; }
  void SetUserName(CString& strUserName) { m_strUserName = strUserName; }

  //{{AFX_DATA(CPasswdDlg)
	enum { IDD = IDD_PASSWD };
	CShExecCtrl	m_WWW;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:
	//{{AFX_VIRTUAL(CPasswdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPasswdDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  bool m_bRegister;

protected:
  int m_nTry;
  CString m_strCode;
  CString m_strPasswd;
  CString m_strUserName;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWDDLG_H__B5FED846_B3E8_11D5_B2D2_000000000000__INCLUDED_)
