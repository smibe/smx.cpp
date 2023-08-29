/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SelectVarDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SelectVarDlg.h,v 1.1 2002/09/20 10:42:59 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_SELECTVARDLG_H__6D251AE8_71E9_11D5_B2C4_000000000000__INCLUDED_)
#define AFX_SELECTVARDLG_H__6D251AE8_71E9_11D5_B2C4_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// selectvardlg.h : header file
//

#include "OXGridList.h"
enum ECmdVar;

class CSelectVarDlg : public CDialog
{
// Construction
public:
	CSelectVarDlg(CWnd* pParent = NULL);   // standard constructor

  void SetTopLeft(int top, int left) { m_top = top; m_left = left; }

// Dialog Data
	//{{AFX_DATA(CSelectVarDlg)
	enum { IDD = IDD_SELECT_VAR };
	COXGridList	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectVarDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectVarDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  ECmdVar m_eCmdVar;
  DWORD   m_dwFlags;

protected:
  int     m_top;
  int     m_left;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTVARDLG_H__6D251AE8_71E9_11D5_B2C4_000000000000__INCLUDED_)
