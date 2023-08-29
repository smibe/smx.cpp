/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: MainFrm.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/MainFrm.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// 060698:GeH created.


#if !defined(AFX_MAINFRM_H__2867E826_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_)
#define AFX_MAINFRM_H__2867E826_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define WMU_CMPFILE (WM_USER + 100)

#define MainFrameParent CMDIFrameWnd

#include "ToolbarEx.h"

class COptionSheet;
class CSingleViewTemplate;
class CSMXPropertyPg;
class CFileDropTarget;

class CMainFrame : public MainFrameParent
{
	DECLARE_DYNAMIC(CMainFrame)

//@access Public member functions
public:
	CMainFrame();
	virtual ~CMainFrame();

  CToolBarEx *GetToolBar() { return &m_wndToolBar; }
  bool IsButtonInitialyHidden(UINT nCmdID);

  bool ExpandVar(const CString& strVar, CStringArray& strArray);
  bool CanExpandVar(const CString& strVar);

	void SetStatusText(LPCTSTR text, bool bResetOnChar = false);

  CAcceleratorManager& CMainFrame::GetAccelMgr();

  CSMXPropertyPg* CreatePropertyPage(UINT nDlgID);

  void AdjustInsertBitmap(CBitmap *pBmp, COLORREF crColor);
  void AdjustInsertColor(CToolBar *pToolBar);
  
//Drag and drop function
  DROPEFFECT DragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, CFileDropTarget* pDrop);
  void       DragLeave(CFileDropTarget* pDrop);
  DROPEFFECT DragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, CFileDropTarget* pDrop);
  BOOL       Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point, CFileDropTarget* pDrop);

  //------------- Overloaded functions ----------------------------
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, 		DWORD dwExStyle, CCreateContext* pContext = NULL);
  virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

//@access Protected member functions.
protected:
  void UpdateToolMenu();
  void UpdateAllAccelMgr();


  //{{AFX_MSG(CMainFrame)
  afx_msg void OnUserTool(UINT nCmdID);
  afx_msg void OnUpdateUserTool(CCmdUI *pCmdUI);
  afx_msg void OnViewOptions();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCmpDir();
	afx_msg void OnCustomizeToolBar();
	afx_msg void OnCustomizeKeyboard();
	afx_msg void OnCustomizeUserTools();
	afx_msg void OnClose();
	afx_msg void OnCmpFile();
	afx_msg void OnUpdateCmpFile(CCmdUI* pCmdUI);
	afx_msg void OnWindowsNext();
	afx_msg void OnUpdateWindowsNext(CCmdUI* pCmdUI);
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	afx_msg void OnUpdateCmdIndicator(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnWMUCmpFile(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSmxOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//@access Public attributes
public:
	static CMenu menu;

//@access Protected attributes
protected:
  CSingleViewTemplate *m_pCmpFilTemplate;
  CSingleViewTemplate *m_pCmpDirTemplate;
	CStatusBar           m_wndStatusBar;
	CToolBarEx           m_wndToolBar;
  UINT                 m_nReadyTimer;
  CFileDropTarget     *m_pFileDropTarget;
  UINT                 m_nShownOptionDlgID;
};


//Global function
CMainFrame *TheTop();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_MAINFRM_H__2867E826_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_)
