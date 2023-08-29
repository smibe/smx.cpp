/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: UsageDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/UsageDlg.h,v 1.1 2002/09/20 09:34:48 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#if !defined(AFX_USAGEDLG_H__ED637ED5_10B6_11D4_ABDC_93DD47FB313D__INCLUDED_)
#define AFX_USAGEDLG_H__ED637ED5_10B6_11D4_ABDC_93DD47FB313D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UsageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUsageDlg dialog

class CUsageDlg : public CDialog
{
// Construction
public:
	CUsageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUsageDlg)
	enum { IDD = IDD_USAGE };
	CRichEditCtrl	m_reUsage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUsageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUsageDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    CString m_strError;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USAGEDLG_H__ED637ED5_10B6_11D4_ABDC_93DD47FB313D__INCLUDED_)
