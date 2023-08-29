/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CopyFilesDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/CopyFilesDlg.h,v 1.1 2002/09/20 09:34:46 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#if !defined(AFX_COPYFILESDLG_H__1EEDF356_6C6D_11D5_B2C0_000000000000__INCLUDED_)
#define AFX_COPYFILESDLG_H__1EEDF356_6C6D_11D5_B2C0_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CopyFilesDlg.h : header file
//

class COXGridList;
class CDiffDirDoc;


class CCopyFilesDlg : public CDialog
{
public:
	CCopyFilesDlg(CWnd* pParent = NULL);   // standard constructor

  void SetDiffList(COXGridList *pList) { m_pList = pList; }
  void SetDiffDoc(CDiffDirDoc *pDoc)   { m_pDoc = pDoc; }

  void DDX_Button(CDataExchange* pDX, DWORD& dwFlags, UINT nID, DWORD dwFlagMask);
  void UpdateControls();
  void UpdateStaticText();
  void UpdateFileList();
  void UpdateSelectedDirCtrl();
  bool GetCheck(UINT nID);
  //void SetCheck(UINT nID) {};

	//{{AFX_DATA(CCopyFilesDlg)
	enum { IDD = IDD_COPY_FILES };
	COXGridList	m_List;
	//}}AFX_DATA


	//--------------------- ClassWizard generated virtual function overrides -----------
	//{{AFX_VIRTUAL(CCopyFilesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	//-------------------  Generated message map functions ------------------------
	//{{AFX_MSG(CCopyFilesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnrdFromLeftDir();
	afx_msg void OnrdFromRightDir();
	afx_msg void OnShowFileList();
	afx_msg void OnCkFiles();
	afx_msg void OnToLeftDir();
	afx_msg void OnToRightDir();
	afx_msg void OnToSelectedDir();
	afx_msg void OnEditchangeToSelectedDir();
	afx_msg void OnSelchangeToSelectedDir();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillfocuscbToSelectedDir();
	afx_msg void OnSelectToDir();
	virtual void OnOK();
	afx_msg void OnPreserveSubdir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
  CStringArray  m_strFrom;
  CStringArray  m_strTo;
  bool         *m_pSelected;

protected:
  CDiffDirDoc *m_pDoc;
  COXGridList *m_pList;
  static DWORD        m_dwFlags;
  static CStringArray m_strDirs;
  bool m_bShowFileList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COPYFILESDLG_H__1EEDF356_6C6D_11D5_B2C0_000000000000__INCLUDED_)
