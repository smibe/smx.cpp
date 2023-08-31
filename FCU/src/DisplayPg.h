/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DisplayPg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DisplayPg.h,v 1.3 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_DISPLAYPG_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_
#define AFX_DISPLAYPG_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_

#include "SMXPropertyPg.h"
#include "FcuOptions.h"

class CFilCmpPg : public CSMXPropertyPg
{
public:
	 ~CFilCmpPg() {}
	CFilCmpPg();

	//{{AFX_VIRTUAL(CFilCmpPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CFilCmpPg)
  afx_msg void OnFoldLines();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  int  m_nNotFolded;
protected:
  //{{AFX_DATA(CFilCmpPg)
	enum { IDD = IDD_FILCMP_OPTIONS };
	//}}AFX_DATA

};

class CEditorPg : public CSMXPropertyPg
{
public:
	 ~CEditorPg() {}
	CEditorPg();

	//{{AFX_VIRTUAL(CEditorPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CEditorPg)
	afx_msg void OnBtnFontChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  int      m_nTabWidth;
  SECEdit *m_pEdit;
protected:
  //{{AFX_DATA(CEditorPg)
	enum { IDD = IDD_EDITOR_OPTIONS };
	CEdit	m_ebFontCtrl;
	//}}AFX_DATA

};

class CFilDiffPrintPg : public CSMXPropertyPg
{
public:
	 ~CFilDiffPrintPg() {}
	CFilDiffPrintPg();

	//{{AFX_VIRTUAL(CEditorPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CEditorPg)
	afx_msg void OnBtnFontChange();
	afx_msg void OnBtnLeftColorChange();
	afx_msg void OnBtnRightColorChange();
	afx_msg void OnPrintWithColors();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  EView m_eView;

protected:
  CFont m_Font;
  COLORREF m_crLeft;
  COLORREF m_crRight;

  //{{AFX_DATA(CEditorPg)
	enum { IDD = IDD_FILDIFF_PRINT_OPTIONS};
	CEdit	m_ebFontCtrl;
	CEdit	m_ebLeftColor;
	CEdit	m_ebRightColor;
	//}}AFX_DATA

};

class CDirCmpPg : public CSMXPropertyPg
{
public:
	 ~CDirCmpPg() {}
   CDirCmpPg() {}

	//{{AFX_VIRTUAL(CDirCmpPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CDirCmpPg)
	afx_msg void OnCbDate();
	afx_msg void OnCbTime();
	afx_msg void OnCbFileNotifications();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

protected:
  //{{AFX_DATA(CDirCmpPg)
	enum { IDD = IDD_DIRCMP_OPTIONS };
	//}}AFX_DATA
};

class CCmdLinePg : public CSMXPropertyPg
{
public:
	 ~CCmdLinePg() {}
   CCmdLinePg() {}

	//{{AFX_VIRTUAL(CCmdLinePg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CCmdLinePg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

protected:
  //{{AFX_DATA(CCmdLinePg)
	enum { IDD = IDD_CMDLINE_OPTIONS };
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_DISPLAYPG_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_
