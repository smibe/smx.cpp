/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: UserToolsDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/UserToolsDlg.h,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_USERTOOLSDLG_H__B04045C6_6D1A_11D5_B2C1_000000000000__INCLUDED_)
#define AFX_USERTOOLSDLG_H__B04045C6_6D1A_11D5_B2C1_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UserToolMgr.h"
#include "OXGridList.h"

/////////////////////////////////////////////////////////////////////////////
// CUserToolsDlg dialog

class CUserToolsDlg : public CDialog
{
// Construction
public:
	CUserToolsDlg(CWnd* pParent = NULL);   // standard constructor

  void SetUserToolMgr(CUserToolMgr *pMgr) { m_pMgr = pMgr; }
  void LoadUserTool(int nIdx);
  bool StoreUserTool(int nIdx);
  bool UpdateMenuText();
  bool CheckCommandString();

  //--------------- Dialog Data -----------------------
	//{{AFX_DATA(CUserToolsDlg)
	enum { IDD = IDD_USER_TOOLS };
	COXGridList	m_List;
	//}}AFX_DATA


  //----------------- Overrides ------------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserToolsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual void OnOK();
  virtual void OnCancel();
	//}}AFX_VIRTUAL

	//------------------ Generated message map functions ------------------
	//{{AFX_MSG(CUserToolsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNew();
	afx_msg void OnSelchangeUserTools();
	afx_msg void OnRemove();
	afx_msg void OnUpdate();
  afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusMenuString();
	afx_msg void OnKillfocusCommand();
	afx_msg void OnParameter();
	afx_msg void OnInitialDir();
	afx_msg void OnBatchFile();
	afx_msg void OnBatchFileName();
	afx_msg void OnCommand();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  CUserToolMgr *m_pMgr;
  int m_nCurrentIdx;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERTOOLSDLG_H__B04045C6_6D1A_11D5_B2C1_000000000000__INCLUDED_)
