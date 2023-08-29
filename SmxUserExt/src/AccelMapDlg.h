/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelMapDlg.h,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelMapDlg.h,v 1.2 2003/02/21 08:56:52 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Thierry Maurel
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    tmaurel@caramail.com   (or tmaurel@hol.fr)
//
////////////////////////////////////////////////////////////////////////////////
// File    : AccelMapDlg.h
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : Edit dialog. This is just an encapsulation of the orders, all
//           of their implementation are in the CAccelDlgHelper object.
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ACCELMAPDLG_H__AA285EC4_2713_11D2_AD1D_F7716D5B9F0E__INCLUDED_)
#define AFX_ACCELMAPDLG_H__AA285EC4_2713_11D2_AD1D_F7716D5B9F0E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//


#include "AcceleratorManager.h"
#include "AccelListBox.h"
#include "KeyboardEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CAccelMapDlg dialog

class CAccelMapDlg : public CDialog
{
// Construction
public:
	CAccelMapDlg(CAcceleratorManager* pAccelManager = NULL,
               CAcceleratorManager* pReadonlyAccelManager = NULL,
               int nNofAccelManagers = 0,
				       CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAccelMapDlg)
	enum { IDD = IDD_EDIT_ACCELMAP };
	CStatic	m_AlreadyAssigned;
	CStatic	m_Description;
	CKeyboardEdit	m_Key;
	CAccelListBox	m_Currents;
	CListBox	m_Commands;
  CComboBox m_Category;
  CComboBox m_View;
	//}}AFX_DATA

  int m_nInitialAccelMgrIdx;
protected:
	CAcceleratorManager* m_pAccelManager;
	CAcceleratorManager* m_pReadonlyAccelManager;
  int m_nNofAccelManagers;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccelMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Called by the Dlg or Page, at each user action. 
	void Assign();
	void Remove();
	void Reset();
  void UpdateAffected();
  bool UpdateAffected(CAcceleratorManager *pAccelMgr);

	// Generated message map functions
	//{{AFX_MSG(CAccelMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAssign();
	afx_msg void OnRemove();
	afx_msg void OnReset();
	afx_msg void OnSelChangeCommands();
	afx_msg void OnChangeEditKey();
  afx_msg void OnSelChangeCategory() ;
  afx_msg void OnSelChangeView() ;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCELMAPDLG_H__AA285EC4_2713_11D2_AD1D_F7716D5B9F0E__INCLUDED_)

