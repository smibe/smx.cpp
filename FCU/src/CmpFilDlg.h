/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CmpFilDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/CmpFilDlg.h,v 1.1 2002/09/20 09:34:46 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#ifndef AFX_CMPFILDLG_H__C5764FA2_21FC_11D2_8C5F_9482AD000000__INCLUDED_
#define AFX_CMPFILDLG_H__C5764FA2_21FC_11D2_8C5F_9482AD000000__INCLUDED_

class CFileDropTarget;

class CCmpFilDlg : public CDialog
{
public:
	CCmpFilDlg(CWnd* pParent = NULL);
	~CCmpFilDlg();

	void Init();

  //-------------- Virtual functions ----------------------------
	//{{AFX_VIRTUAL(CCmpFilDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

  //Drag and drop function
  DROPEFFECT DragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
  void       DragLeave();
  DROPEFFECT DragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
  BOOL       Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

protected:
  bool UpdateRegistryEntries();
  void GetFilter(CString& str, LPCTSTR pszKey);
  void StoreLastFilter(CFileDialog& dlg, CString& strFilter, LPCTSTR pszKey);

  //------------- Generated message map functions -------------
	//{{AFX_MSG(CCmpFilDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtFil1();
	afx_msg void OnBtFil2();
	//}}AFX_MSG

public:
  //-------------- Dialog data ---------------------
	//{{AFX_DATA(CCmpFilDlg)
	enum { IDD = IDD_CMP_FIL };
	CString	m_strLeftFileName;
	CString	m_strRightFileName;
	//}}AFX_DATA

protected:
  CFileDropTarget     *m_pFileDropTarget;
  UINT                 m_nNofPathNames;

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CMPFILDLG_H__C5764FA2_21FC_11D2_8C5F_9482AD000000__INCLUDED_
