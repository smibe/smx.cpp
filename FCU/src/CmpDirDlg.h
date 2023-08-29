/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CmpDirDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/CmpDirDlg.h,v 1.2 2002/11/22 17:27:12 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_CMPDIRDLG_H__D2BCD8C2_EEFA_11D1_9E37_204C4F4F5020__INCLUDED_
#define AFX_CMPDIRDLG_H__D2BCD8C2_EEFA_11D1_9E37_204C4F4F5020__INCLUDED_

class CFileDropTarget;

class CCmpDirDlg : public CDialog
{
public:
	CCmpDirDlg(CWnd* pParent = NULL);
	virtual ~CCmpDirDlg();

  //---------------- Overrides -----------------
	//{{AFX_VIRTUAL(CCmpDirDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

  //Drag and drop function
  DROPEFFECT DragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
  void       DragLeave();
  DROPEFFECT DragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
  BOOL       Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

protected:
  bool UpdateRegistryEntries();

  
  //--------------Generated message map functions ------------------
  //{{AFX_MSG(CCmpDirDlg)
	afx_msg void OnBtDir1();
	afx_msg void OnBtDir2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  //{{AFX_DATA(CCmpDirDlg)
	enum { IDD = IDD_CMP_DIR };
	CString	m_strDir1;
	CString	m_strDir2;
	CString	m_strFileMask;
	CString	m_strExcludeMask;
	//}}AFX_DATA

  CFileDropTarget     *m_pFileDropTarget;
  UINT                 m_nNofPathNames;

protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CMPDIRDLG_H__D2BCD8C2_EEFA_11D1_9E37_204C4F4F5020__INCLUDED_
