/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffDirView.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffDirView.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_DIFFDIRVIEW_H__97553F04_E978_11D1_9E33_204C4F4F5020__INCLUDED_
#define AFX_DIFFDIRVIEW_H__97553F04_E978_11D1_9E33_204C4F4F5020__INCLUDED_

#include "OXGridList.h"
#include "savecompdlg.h"
#include "FcuMdiChildWndEx.h"
#include "SmxPrnFormView.h"

class CDiffDirDoc;
class CAcceleratorManager;
class CDiffRec;

// DiffDirView.h : Header-Datei
//

enum ECmdVar;



class CDiffDirView : public CSmxPrnFormView
{
DECLARE_DYNCREATE(CDiffDirView)
friend class CDiffDirFrame;

// Konstruktion
public:
	CDiffDirView(CWnd* pParent = NULL);   // Standardkonstruktor
  virtual ~CDiffDirView();

  virtual bool UseForPrintOnly() { return TRUE; }
  virtual bool CalculateNofPagesInOnDraw() { return TRUE; }
	virtual void Draw();

  bool ExpandVar(const CString& strVar, CStringArray& strArray);
  bool ExpandVar(ECmdVar eVar, CStringArray& strArray, bool *pAllExistent = NULL);
  bool CanExpandVar(const CString& strVar);
	bool IsModified();

  void Update();
  bool IsColHidden(int nCol);
  void StoreColWidth();
  bool IsVisibleVScroll();
  void ExecuteShellCmd(LPCTSTR pszCmd, bool bRight, bool bRelative);
  void OnUpdatePopup(CMenu *pMenu);

  void UpdateRecord(int nIdx);
  void InsertRec(CDiffRec *pRec, int nRow, int nIdx);
  void UpdateRec(CDiffRec *pRec, int nRow, bool bUpdateNow = true);

	//{{AFX_VIRTUAL(CDiffDirView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
protected:

  CDiffDirDoc * GetDiffDirDoc();
  void ResizeControls();

	//-----------Generated message map functions ----------
	//{{AFX_MSG(CDiffDirView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnDblclkDiffList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnReturnDiffList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnToolsCopyfiles();
  afx_msg void OnToolsSelectFiles();
  afx_msg void OnTrackHeader(NMHDR* pNMHDR, LRESULT* pResult) ;
	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnProperties_L();
  afx_msg void OnProperties_R();
  afx_msg void OnUpdateCommand_L(CCmdUI *pCmdUI);
  afx_msg void OnUpdateCommand_R(CCmdUI *pCmdUI);
  afx_msg void OnUpdateOneDir_L(CCmdUI *pCmdUI);
  afx_msg void OnUpdateOneDir_R(CCmdUI *pCmdUI);
  afx_msg void OnCopy_L();
  afx_msg void OnCopy_R();
  afx_msg void OnDelete_L();
  afx_msg void OnDelete_R();
  afx_msg void OnOpen_L();
  afx_msg void OnOpen_R();
	afx_msg void OnViewRefresh();
	afx_msg void OnPageSetup();
	afx_msg void OnEditComment();
	afx_msg void OnViewCompareagain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  
  virtual void    SetPageOrientation(bool bLandscape);
  virtual LPCTSTR GetPageSetupSection();


protected:
  UINT *m_pColSizes;

  //------------- Dialog data -------------
	//{{AFX_DATA(CDiffDirView)
	enum { IDD = IDR_DIFF_DIR };
	COXGridList m_diffList;
	//}}AFX_DATA


  BOOL m_bInitialized;
  CUIntArray m_aFirstRowsInPage;
};


class CDiffDirFrame : public CFcuMDIChildWndEx
{
	DECLARE_DYNCREATE(CDiffDirFrame)
protected:
	CDiffDirFrame();           // Dynamische Erstellung verwendet geschützten Konstruktor

public:
  static CAcceleratorManager *GetAccelMgr();
  static void DeleteAccelMgr();

  //{{AFX_VIRTUAL(CDiffDirFrame)
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CDiffDirFrame();

  LPCTSTR GetToolbarRegKeyName() { return _T("CmpDirToolBar"); }
  virtual bool IsButtonInitialyHidden(UINT nCmdID);

  //{{AFX_MSG(CDiffDirFrame)
	afx_msg void OnClose();
  afx_msg void OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  static CAcceleratorManager* m_pAccelMgr;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_DIFFDIRVIEW_H__97553F04_E978_11D1_9E33_204C4F4F5020__INCLUDED_
