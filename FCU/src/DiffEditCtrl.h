/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffEditCtrl.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffEditCtrl.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_DIFFEDITCTRL_H__659F32E9_05F6_11D2_9E5B_5620EF000000__INCLUDED_)
#define AFX_DIFFEDITCTRL_H__659F32E9_05F6_11D2_9E5B_5620EF000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FileDropTarget.h"

class CDiffEditCtrl : public SECEditCtrl
{
DECLARE_DYNAMIC(CDiffEditCtrl);
public:
	CDiffEditCtrl();
	virtual ~CDiffEditCtrl();

  //-----------  Overrides -----------------
	//{{AFX_VIRTUAL(CDiffEditCtrl)
	public:
	virtual void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_VIRTUAL

public:
	void     MyCenterLine(PLINEDESC pLine);
	BOOL     InsertAllocLines(PLINEDESC& pBef, PLINEDESC& pAfter, PLINEDESC start, PLINEDESC end);
	BOOL     CutMarkedLines(PLINEDESC start, PLINEDESC end);
	void     MarkLine(PLINEDESC start, PLINEDESC end, bool bCenterLine = true);
	COLORREF GetBackgroundColor(PLINEDESC pLine);

  virtual int      JoinLines(BOOL bUpdate, BOOL bSaveUndoRecord);
	virtual COLORREF GetGutterColor(COLORREF color);
	virtual void     DrawMarks(CPaintDC* pDC, RECT* pRect, PLINEDESC  pLine);
  virtual void     MySetCaretPos();
  virtual int      InitView();

  void    RefreshMark(PLINEDESC pLine);
	void 	  FillTransparentRect(CDC *pDC, CRect& rect, COLORREF crColor);
  void    OnUpdatePopup(CMenu *pMenu);

  virtual int Cut(BOOL bUpdate, BOOL bSaveUndoRecord);
  virtual int SplitLine(BOOL bUpdate, BOOL bSaveUndoRecord);
  virtual int GetDisplayLineNo(PLINEDESC pLine, CDC *pDC);
	virtual BOOL PaintTextOut( SECEdit* pEdit, 
					              	   PLINEDESC lpLine, 	// pointer to the current line
							               LPSTR str, 			// the full text line, tabs expanded
							               int nLength,			// the full length of the text
							               int nIndex, 			// first character to draw
							               int nCount, 			// max characters to draw
							               BOOL bChroma, 			// do it in color
							               CDC* dc, 				// printer's dc
							               int x, 					// textout coordinates
							               int y);					//		 "			 "
   	

	// MCN - set the top line
	void SyncLine(PLINEDESC pTop);

  PLINEDESC CreateFoldedLine(int iStart, int iEnd);
  bool      HasBlockFoldedSelection();
  bool      HasMarkedLines() { return m_pFirstMarked || m_pLastMarked; }

  //{{AFX_MSG(CDiffEditCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
  
  afx_msg void OnFold();
  afx_msg void OnUnFold();
  afx_msg void OnUpdateFold(CCmdUI *pCmdUI);
  afx_msg void OnUpdateUnFold(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
  int m_nLDiffLineNo;                //Line Number of the diff , -1 if no diff line
  CUIntArray m_aLDiffLineSect;       //Array containing the indexes for the line diff sections
  int m_nRDiffLineNo;                //Line Number of the diff , -1 if no diff line
  CUIntArray m_aRDiffLineSect;       //Array containing the indexes for the line diff sections

  static COLORREF m_crRight;
	static COLORREF m_crLeft;
  static COLORREF m_crFolded;

protected:
	PLINEDESC    m_pLastMarked;
	PLINEDESC    m_pFirstMarked;
  bool         m_bReadOnly;

  //CMapPtrToPtr m_FoldedMap;
};

#endif // !defined(AFX_DIFFEDITCTRL_H__659F32E9_05F6_11D2_9E5B_5620EF000000__INCLUDED_)
