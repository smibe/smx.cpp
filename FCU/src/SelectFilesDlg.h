/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SelectFilesDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/SelectFilesDlg.h,v 1.1 2002/09/20 09:34:48 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#if !defined(AFX_SELECTFILESDLG_H__1EEDF357_6C6D_11D5_B2C0_000000000000__INCLUDED_)
#define AFX_SELECTFILESDLG_H__1EEDF357_6C6D_11D5_B2C0_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectFilesDlg.h : header file
//

class COXGridList;

class CSelectFilesDlg : public CDialog
{
// Construction
public:
	CSelectFilesDlg(CWnd* pParent = NULL);   // standard constructor

  void SetDiffList(COXGridList *pList) { m_pList = pList; }

  //---------------- Dialog Data ---------------------
	//{{AFX_DATA(CSelectFilesDlg)
	enum { IDD = IDD_SELECT_FILES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


  //---------------- Overrides ------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectFilesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSelectFilesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  COXGridList *m_pList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTFILESDLG_H__1EEDF357_6C6D_11D5_B2C0_000000000000__INCLUDED_)
