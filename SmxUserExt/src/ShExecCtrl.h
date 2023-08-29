/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: ShExecCtrl.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/ShExecCtrl.h,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#if !defined(AFX_SHEXECCTRL_H__4271AF41_D1EB_11D5_B2D4_000000000000__INCLUDED_)
#define AFX_SHEXECCTRL_H__4271AF41_D1EB_11D5_B2D4_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShExecCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShExecCtrl window

class CShExecCtrl : public CButton
{
// Construction
public:
	CShExecCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShExecCtrl)
	public:
  virtual void DrawItem(LPDRAWITEMSTRUCT);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShExecCtrl();
  void SetCommand(LPCTSTR pszCommand) { m_strCommand = pszCommand; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CShExecCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClicked();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
  LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
  HCURSOR GetHandCursor();

  DECLARE_MESSAGE_MAP()

public:
  static HCURSOR sm_hHandCursor;
  static HCURSOR sm_hArrowCursor;

protected:
  bool    m_bSelected;
  bool    m_bLeaveTracked;
  CString m_strCommand;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEXECCTRL_H__4271AF41_D1EB_11D5_B2D4_000000000000__INCLUDED_)
