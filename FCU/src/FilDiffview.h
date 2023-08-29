/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffview.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffview.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_FILDIFFVIEW_H__8786B282_0502_11D2_9E5A_3068BF000000__INCLUDED_
#define AFX_FILDIFFVIEW_H__8786B282_0502_11D2_9E5A_3068BF000000__INCLUDED_

// FilDiffview.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Formularansicht FilDiffView 

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "OXRectTracker.h"
#include "DiffEditCtrl.h"
#include "resource.h"
#include "list.h"
#include "section.h"
#include "DiffSectElt.h"
#include "SmxPrnFormView.h"
#include "FilePathCtrl.h"

#define WMU_DELSECTMARK (WM_USER +1)
#define WMU_JOINLINES   (WM_USER +2)
#define WMU_SPLITLINE   (WM_USER +3)
#define WMU_PASTE			  (WM_USER +4)
#define WMU_DELCOMPLETE	(WM_USER +5)

enum ECmdVar;

class CSaveCompDlg;
class CFilDiffDoc;

class CFilDiffView : public CSmxPrnFormView
{
  friend class CDiffEditCtrl;
	DECLARE_DYNCREATE(CFilDiffView)
//@public member functions
public:

  CFilDiffDoc *GetFilDiffDoc() { return (CFilDiffDoc *) GetDocument(); }

	void DeleteRightSect(CDiffSectElt * pSect);
	void DeleteLeftSect(CDiffSectElt * pSect);
	bool InCmdMode();
	void SetLastActiveWnd(CDiffEditCtrl *pWnd);
	void SetHitCursor(CPoint point);
  void RefreshEditControls();
  void SetFont(LOGFONT& Logfont);

  virtual bool UseForPrintOnly() { return TRUE; }
  virtual bool CalculateNofPagesInOnDraw() { return TRUE; }
	virtual void Draw();

  bool ExpandVar(const CString& strVar, CStringArray& strArray);
  bool ExpandVar(ECmdVar eVar, CStringArray& strArray);
  bool CanExpandVar(const CString& strVar);
  bool GetMergeFileName(CString& strFileName);

	void UpdateWndLayout(bool initial);
  void UpdateEditControls();

  void UpdatePopupMenu(CDiffEditCtrl*pEdit, CMenu *pMenu);

  void SyncLLine(PLINEDESC pLine);
  void SyncRLine(PLINEDESC pLine);
  void SyncMLine(PLINEDESC pLine);
  
  PLINEDESC MoveUp(PLINEDESC pLine, int nLines);
  PLINEDESC MoveDown(PLINEDESC pLine, int nLines);

	//{{AFX_VIRTUAL(CFilDiffView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

//@access Protected member functions
protected:
	CFilDiffView();

	void InsertLeftSect(CDiffSectElt *pSect);
	void InsertRightSect(CDiffSectElt *pSect);
	void MarkDiffSect(CDiffSectElt *pSect, bool bCenterLine = true);
	void MyHideWindow(CDiffEditCtrl& wnd, bool bShow, int& nWidth, int& nHeight);
  void SetStatusText(UINT nStrID, bool bSetTimer);
	virtual ~CFilDiffView();

  void StoreSplitRatio();
  int  GetDisplayLineNo(CDiffEditCtrl *pEdit, PLINEDESC pLine, CDC *pDC);

	//----------- Message Map functions ---------------
	//{{AFX_MSG(CFilDiffView)
  afx_msg void OnProperties();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowsCompfile();
	afx_msg void OnUpdateWindowsCompfile(CCmdUI* pCmdUI);
	afx_msg void OnWindowsLeftfile();
	afx_msg void OnUpdateWindowsLeftfile(CCmdUI* pCmdUI);
	afx_msg void OnWindowsRightfile();
	afx_msg void OnUpdateWindowsRightfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditInsertleft(CCmdUI* pCmdUI);
	afx_msg void OnEditInsertleft();
	afx_msg void OnEditInsertright();
	afx_msg void OnUpdateEditInsertright(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewLineDiff(CCmdUI* pCmdUI);
  afx_msg void OnViewLineDiff();
	afx_msg void OnViewNextdiff();
	afx_msg void OnViewPrevdiff();
	afx_msg void OnUpdateFileSavecomposed(CCmdUI* pCmdUI);
	afx_msg void OnFileSavecomposed();
	afx_msg void OnUpdateCmdIndicatorCmd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdIndicatorDiff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCmdIndicatorLine(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnEditInsertboth();
	afx_msg void OnUpdateEditInsertboth(CCmdUI* pCmdUI);
	afx_msg void OnEditDeletediff();
	afx_msg void OnUpdateViewNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnEditRestoreLeft();
	afx_msg void OnUpdateEditRestoreLeft(CCmdUI* pCmdUI);
	afx_msg void OnEditRestoreRight();
	afx_msg void OnUpdateEditRestoreRight(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDelSectMark(WPARAM nCtrlId, LPARAM lParam);
	afx_msg LRESULT OnSplitLines(WPARAM nCtrlId, LPARAM lParam);
	afx_msg LRESULT OnJoinLines(WPARAM nCtrlId, LPARAM lParam);
	afx_msg void OnViewSyncronize();
	afx_msg void OnViewRefresh();
	afx_msg void OnPageSetup();
  afx_msg void OnNextDiffFromCursor();
	afx_msg void OnFileXmlExport();
	afx_msg void OnOptionsFoldlines();
	afx_msg void OnOptionsShowlineno();
	afx_msg void OnUpdateOptionsFoldlines(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowlineno(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSyncscrolling();
	afx_msg void OnUpdateOptionsSyncscrolling(CCmdUI* pCmdUI);
	//}}AFX_MSG
  afx_msg void OnLScrollChanged();
  afx_msg void OnRScrollChanged();
  afx_msg void OnMScrollChanged();

#ifdef _DEBUG
	afx_msg LRESULT OnDelComplete(WPARAM nCtrlId, LPARAM lParam);
#endif
	DECLARE_MESSAGE_MAP()

  virtual void    SetPageOrientation(bool bLandscape);
  virtual LPCTSTR GetPageSetupSection();

//@access Protected attributes.
protected:
	static CMultiDocTemplate *pDocTemplate;
	static HCURSOR            m_splitCurH;
	static HCURSOR            m_splitCurV;

  UINT           m_nStatusTimer;
  bool           m_bInitialized;
  bool           m_bVLayoutChanged;
  bool           m_bHLayoutChanged;
	CDiffEditCtrl* m_lastActiveWnd;
	int            m_split;
	COXRectTracker m_hTracker;
	COXRectTracker m_vTracker;
	SECTION        m_curDiffSection;
  CPtrArray      m_aFirstLineInPage;

  //{{AFX_DATA(CFilDiffView)
	enum { IDD = IDD_FILDIFFVIEW };
	CDiffEditCtrl	m_LeftFileWnd;
	CDiffEditCtrl	m_MergeFileWnd;
	CDiffEditCtrl	m_RightFileWnd;
  CFilePathCtrl m_RightFileName;
  CFilePathCtrl m_LeftFileName;
	//}}AFX_DATA
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_FILDIFFVIEW_H__8786B282_0502_11D2_9E5A_3068BF000000__INCLUDED_
