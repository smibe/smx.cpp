/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: MDIChildWndEx.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/MDIChildWndEx.h,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#if !defined(AFX_MDICHILDWNDEX_H__FB0B3006_66D2_11D5_B2BC_000000000000__INCLUDED_)
#define AFX_MDICHILDWNDEX_H__FB0B3006_66D2_11D5_B2BC_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CToolBarEx;

class CFcuMDIChildWndEx : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFcuMDIChildWndEx)
protected:
	CFcuMDIChildWndEx();           // protected constructor used by dynamic creation

public:
	//{{AFX_VIRTUAL(CMDIChildWndEx)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFcuMDIChildWndEx();

  virtual LPCTSTR GetToolbarRegKeyName();
  virtual void InitializeToolBar(bool bDefault);
  virtual bool IsButtonInitialyHidden(UINT nCmdID);
          void ResizeToolBar();

  CToolBarEx           *GetToolBar();
	//{{AFX_MSG(CMDIChildWndEx)
  afx_msg void OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDICHILDWNDEX_H__FB0B3006_66D2_11D5_B2BC_000000000000__INCLUDED_)
