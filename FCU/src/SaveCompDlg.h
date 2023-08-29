/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SaveCompDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/SaveCompDlg.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#ifndef AFX_SAVECOMPDLG_H__4EFC8BF2_1FB6_11D2_8C5A_8ECB63000000__INCLUDED_
#define AFX_SAVECOMPDLG_H__4EFC8BF2_1FB6_11D2_8C5A_8ECB63000000__INCLUDED_


class CSaveCompDlg : public CDialog
{
public:
	CSaveCompDlg(CWnd* pParent = NULL);
	~CSaveCompDlg();

  CString GetFileName();
	void    SetFileNames(CString relName, CString leftDir, CString rightDir);
	
  enum selType 
  { 
		sel_right = 0,
		sel_left  = 1, 
		sel_merge = 2,
		sel_save  = 3
	};


	//{{AFX_VIRTUAL(CSaveCompDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
  virtual void OnOK();
	//}}AFX_VIRTUAL

protected:
	void UpdateFileName();
	void EnableDirSet(BOOL enable);

	//{{AFX_MSG(CSaveCompDlg)
	afx_msg void OnRRight();
	afx_msg void OnRLeft();
	afx_msg void OnRSaveAs();
	afx_msg void OnRMerge();
	virtual BOOL OnInitDialog();
	afx_msg void OnBSelDir();
	afx_msg void OnEditchangeCbDir();
	afx_msg void OnBSaveAs();
	afx_msg void OnCkSubdir();
	afx_msg void OnSelchangeCbDir();
	afx_msg void OnKillfocusCbDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  //{{AFX_DATA(CSaveCompDlg)
	enum { IDD = IDD_SAVE_COMP };
	CString	m_strDir;
	BOOL	  m_bSubdir;
	CString	m_strFileName;
	int     m_nSelSave;
	//}}AFX_DATA

protected:
	CString m_strLeftFileName;
	CString m_strRightFileName;
	CString m_strRelDir;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SAVECOMPDLG_H__4EFC8BF2_1FB6_11D2_8C5A_8ECB63000000__INCLUDED_
