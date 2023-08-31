/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffEditCtrl.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffEditCtrl.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "FcuOptions.h"
#include "state.h"
#include "popup.h"
#include "DiffEditCtrl.h"
#include "FilDiffView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


UINT PASCAL GetMouseScrollLines()
{
	static UINT uCachedScrollLines = -1;

  if (uCachedScrollLines != (UINT) -1)
    return uCachedScrollLines;

  // couldn't use the window -- try system settings
	uCachedScrollLines = 3; // reasonable default
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uCachedScrollLines, 0);

	return uCachedScrollLines;
}


COLORREF CDiffEditCtrl::m_crRight = INT_MAX;
COLORREF CDiffEditCtrl::m_crLeft  = INT_MAX;
COLORREF CDiffEditCtrl::m_crFolded  = INT_MAX;

class CFilDiffView;

IMPLEMENT_DYNAMIC(CDiffEditCtrl, SECEditCtrl);

BEGIN_MESSAGE_MAP(CDiffEditCtrl, SECEditCtrl)
	//{{AFX_MSG_MAP(CDiffEditCtrl)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
  ON_COMMAND(ID_FOLD, OnFold)
  ON_UPDATE_COMMAND_UI(ID_FOLD, OnUpdateFold)
  ON_COMMAND(ID_UNFOLD, OnUnFold)
  ON_UPDATE_COMMAND_UI(ID_UNFOLD, OnUpdateUnFold)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDiffEditCtrl::CDiffEditCtrl()
{
	m_pLastMarked	  = NULL;
	m_pFirstMarked	= NULL;
  m_bReadOnly     = FALSE;

  if (m_crRight == INT_MAX)
  {
    //Uninitialized
    m_crRight = GetOptions().GetEditColorRight();
    m_crLeft  = GetOptions().GetEditColorLeft();
    m_crFolded  = GetOptions().GetEditColorFolded();
  }

  m_nLDiffLineNo = -1;
  m_nRDiffLineNo = -1;
}

CDiffEditCtrl::~CDiffEditCtrl()
{
}


void CDiffEditCtrl::DrawMarks(CPaintDC * pDC, RECT * pRect, PLINEDESC pLine)
{
	// Call the base class to draw the marks in the guttr
	SECEditCtrl::DrawMarks(pDC, pRect, pLine);

  pRect->right -= m_iLineNumSpace;
  if (!pLine)
    return;

	ASSERT(GetParent()->GetRuntimeClass() == RUNTIME_CLASS(CFilDiffView));
	CFilDiffView * pWnd= (CFilDiffView *) GetParent();

	if(m_pLastMarked && m_pFirstMarked) 
	{
		if(m_pFirstMarked->iLineNo <= pLine->iLineNo
			&& m_pLastMarked->iLineNo >= pLine->iLineNo)
		{
			//we have a marked sections, draw corresponding 
			//background
			//Fill in a blue rect
			CBrush Brush(RGB(0, 0, 200));
			pDC->FillRect(pRect, &Brush);
		}
	} 
  else if(m_pLastMarked && m_pLastMarked->iLineNo == pLine->iLineNo) 
  {
		CRect rect;
		rect.top = pRect->top-2;
		rect.bottom = pRect->top+2;
		rect.right = pRect->right;
		rect.left = pRect->left;
		CBrush Brush(RGB(0, 0, 200));
		pDC->FillRect(rect, &Brush);

		pDC->SetBkColor(GetEdit()->m_crBackground);
		//return;

	} 
  else if(m_pFirstMarked && m_pFirstMarked->iLineNo == pLine->iLineNo-1) 
  {
		CRect rect;
		rect.top = pRect->top-2;
		rect.bottom = pRect->top+2;
		rect.right = pRect->right;
		rect.left = pRect->left;
		CBrush Brush1(RGB(0, 0, 200));
		pDC->FillRect(rect, &Brush1);

		pDC->SetBkColor(GetEdit()->m_crBackground);
		//return;
	} 
	else if(m_pFirstMarked && m_pFirstMarked->iLineNo == pLine->iLineNo && m_pFirstMarked->pNext == NULL) 
	{
		//We have to mark after the last line
		CRect rect;
		rect.top = pRect->bottom-2;
		rect.bottom = pRect->bottom+2;
		rect.right = pRect->right;
		rect.left = pRect->left;
		FillTransparentRect(pDC, rect, RGB(0, 0, 200));

		pDC->SetBkColor(GetEdit()->m_crBackground);
		//return;
	}

	CRect rect;	
	GetClientRect(rect);
	rect.top = pRect->top -2;
	rect.bottom = pRect->top + 4;
	rect.left = pRect->right + 1;

	switch(pLine->state)
	{
		case STATE_DELLEFTAFTER:
			rect.top = pRect->bottom -2;
			rect.bottom = pRect->bottom + 4;
			//Fall through
		case STATE_DELLEFTBEFORE:
			FillTransparentRect(pDC, rect, m_crLeft);
			break;

		case STATE_DELRIGHTAFTER:
			rect.top = pRect->bottom -2;
			rect.bottom = pRect->bottom + 4;
			//Fall through
		case STATE_DELRIGHTBEFORE:
			FillTransparentRect(pDC, rect, m_crRight);
			break;
	}

	pDC->SetBkColor(GetBackgroundColor(pLine));
}

COLORREF CDiffEditCtrl::GetBackgroundColor(PLINEDESC pLine)
{
	switch(pLine->state)
	{
		case STATE_LEFTONLY:
		case STATE_MOVEDLEFT:
			return(m_crLeft);

		case STATE_RIGHTONLY:
		case STATE_MOVEDRIGHT:
			return(m_crRight);

		case STATE_FOLDED:
      return m_crFolded;
		case STATE_SAME:
		default:
			return(GetEdit()->m_crBackground);

	}
}

COLORREF CDiffEditCtrl::GetGutterColor(COLORREF color)
{
	// Since we reset the line color on the previous line, let's
	// reset it for this one.
	CDC* pDC = GetDC();
	if(pDC)
	{
		pDC->SetBkColor(GetEdit()->m_crBackground);
		ReleaseDC(pDC);
	}

	// Do the default processing for the gutter
	return SECEditCtrl::GetGutterColor(color);
}

int CDiffEditCtrl::JoinLines(BOOL bUpdate, BOOL bSaveUndoRecord)
{
	GetParent()->SendMessage(WMU_JOINLINES, GetDlgCtrlID(), (LPARAM) m_pCurLine);
	return(SECEditCore<CWnd>::JoinLines(bUpdate, bSaveUndoRecord));
}

void CDiffEditCtrl::OnUpdatePopup(CMenu *pMenu)
{
	ASSERT(GetParent()->GetRuntimeClass() == RUNTIME_CLASS(CFilDiffView));
	CFilDiffView * pWnd= (CFilDiffView *) GetParent();

  if (pMenu)
    pWnd->UpdatePopupMenu(this, pMenu);
}


void CDiffEditCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	PopupMenu(IDR_DIFFEDIT_POPUP,TRUE).TrackMenu(this,point, (UpdateMenuFkt)&CDiffEditCtrl::OnUpdatePopup);
}

void CDiffEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	ASSERT(GetParent()->GetRuntimeClass() == RUNTIME_CLASS(CFilDiffView));
	CFilDiffView * pWnd= (CFilDiffView *) GetParent();

	switch(nChar) 
	{
		case VK_ESCAPE:
			pWnd->SendMessage(WM_COMMAND, ID_FILE_CLOSE);
			break;

		case VK_BACK:
			if(GetOption(OCF_CmdMode))
				return;
			else
				SECEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
			break;


		default:
			SECEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	}

}

void CDiffEditCtrl::MarkLine(PLINEDESC start, PLINEDESC end, bool bCenterLine)
{
	m_pFirstMarked = start;
	m_pLastMarked = end;

	if (bCenterLine)
  {
    if(start)
		  MyCenterLine(start);
	  else if (end)
		  MyCenterLine(end);
  }

	m_bUpdate = TRUE;
	UpdateViews(this);
}


void CDiffEditCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	SECEditCtrl::OnSetFocus(pOldWnd);
	
	ASSERT(GetParent()->GetRuntimeClass() == RUNTIME_CLASS(CFilDiffView));
	CFilDiffView * pWnd= (CFilDiffView *) GetParent();
	pWnd->SetLastActiveWnd(this);
}

BOOL CDiffEditCtrl::CutMarkedLines(PLINEDESC start, PLINEDESC end)
{
	BOOL bReadonly = GetEdit()->GetReadOnly();

	if(bReadonly)
		GetEdit()->SetReadOnly(FALSE);

	GetEdit()->CutLines(start, end);
	GetEdit()->SetReadOnly(bReadonly);
	CheckLines(GetEdit()->m_pLine);
	return TRUE;
}


BOOL CDiffEditCtrl::InsertAllocLines(PLINEDESC& pBef, PLINEDESC& pAfter, 
									 PLINEDESC start, PLINEDESC end)
{
	BOOL bResult = FALSE;
	BOOL bReadonly = GetEdit()->GetReadOnly();

	if(bReadonly)
		GetEdit()->SetReadOnly(FALSE);

	
	bResult = GetEdit()->InsertAllocLines(pBef, pAfter, start, end);
	GetEdit()->SetReadOnly(bReadonly);

	return bResult;

}

void CDiffEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd *pWnd = GetParent();

  if(GetOption(OCF_CmdMode)) 
  {
		switch(nChar) 
    {
		  case 'n':
			  pWnd->SendMessage(WM_COMMAND, ID_VIEW_NEXTDIFF);
			  break;
		  case 'p':
			  pWnd->SendMessage(WM_COMMAND, ID_VIEW_PREVDIFF);
			  break;
		  case 'r':
			  pWnd->SendMessage(WM_COMMAND, ID_EDIT_INSERTRIGHT);
			  break;
		  case 'l':
			  pWnd->SendMessage(WM_COMMAND, ID_EDIT_INSERTLEFT);
			  break;
		  case '+':
			  pWnd->SendMessage(WM_COMMAND, ID_EDIT_INSERTBOTH);
			  break;
		  case '-':
			  pWnd->SendMessage(WM_COMMAND, ID_EDIT_DELETEDIFF);
			  break;
		  case 'R':
			  pWnd->SendMessage(WM_COMMAND, ID_EDIT_RESTORERIGHT);
			  break;
		  case 'L':
			  pWnd->SendMessage(WM_COMMAND, ID_EDIT_RESTORELEFT);
			  break;
		  default:
			  ;
		  
    }
	} 
  else 
		SECEditCtrl::OnChar(nChar, nRepCnt, nFlags);

}

void CDiffEditCtrl::MyCenterLine(PLINEDESC pLine)
{
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);
	
	if (!AfxIsValidAddress(pLine, sizeof(LINEDESC)))
	{
		ASSERT(false);
		return;
	}

	if (pLine && pLine->pPrev  && !AfxIsValidAddress(pLine->pPrev, sizeof(LINEDESC)))
	{
		ASSERT(false);
		return;
	}

	m_pCurLine = pLine;
	m_iLineNo = pEdit->GetLineNo(m_pCurLine);

  // center this line
	int iNumLines = m_rect.bottom / m_iLineHeight;
	int iTargetLine = iNumLines * 2 / 5;

	m_pTopLine = m_pCurLine;
	m_iFirstLineNo = m_iLineNo;
	while (m_pTopLine->pPrev && (m_iLineNo-m_iFirstLineNo) < iTargetLine)
		{
		m_pTopLine = m_pTopLine->pPrev;
		m_iFirstLineNo--;
		}
	Invalidate();
  m_bSendVScrollChanged = false;
	MakeCursorVisible();
  m_bSendVScrollChanged = true;

}

int CDiffEditCtrl::Cut(BOOL bUpdate, BOOL bSaveUndoRecord)
{
	GetParent()->SendMessage(WMU_DELSECTMARK, GetDlgCtrlID(), NULL);

  //First unfold all lines in the selection
  PLINEDESC pLine  = m_BlockStart.pLine;
  PLINEDESC pEnd   = m_BlockEnd.pLine;
  PLINEDESC pNext  = NULL;
  BOOL      bDone  = false;

  while (pLine)
  {
    pNext = pLine->pNext;

    if (pLine == pEnd)
      bDone = true;

    if (pLine->state == STATE_FOLDED)
    {
      GetEdit()->UnFoldLines(pLine);
    }
    
    if (bDone)
      break;

    pLine = pNext;
  }

  int nResult = SECEditCtrl::Cut(bUpdate, bSaveUndoRecord);
	GetParent()->SendMessage(WMU_DELCOMPLETE, GetDlgCtrlID(), NULL);
	return nResult;

}

int CDiffEditCtrl::SplitLine(BOOL bUpdate, BOOL bSaveUndoRecord)
{
	int       nResult; 
	PLINEDESC pLine = m_pCurLine;

	nResult = SECEditCtrl::SplitLine(bUpdate, bSaveUndoRecord);
	GetParent()->SendMessage(WMU_SPLITLINE, GetDlgCtrlID(), (LPARAM) pLine);
	return nResult;
}

void CDiffEditCtrl::RefreshMark(PLINEDESC pLine)
{
	CRect rcMark;
	
	ASSERT(m_pTopLine && pLine);
	rcMark.top = (m_pTopLine->iLineNo - pLine->iLineNo) * m_iLineHeight;
	rcMark.left = rcMark.top + m_iLineHeight;	
	rcMark.left  = 0;
	rcMark.right = m_iMarkSpace;
	
	m_bUpdate = TRUE;
	UpdateViews(this);
}

void 	CDiffEditCtrl::FillTransparentRect(CDC *pDC, CRect& rect, COLORREF crColor)
{
  CDC dcMem;
  CBitmap bmp;

  dcMem.CreateCompatibleDC(pDC);
  bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
  CBitmap *pPrev = dcMem.SelectObject(&bmp);

  CRect rcCopy = rect;
  CPoint pt(rcCopy.left, rcCopy.top);
  rcCopy -= pt;

  dcMem.FillSolidRect(rcCopy, crColor);
  dcMem.BitBlt(0, 0, rcCopy.Width(), rcCopy.Height(), 
					           pDC, rect.left, rect.top, SRCAND);

  dcMem.SelectObject(pPrev);
  bmp.DeleteObject();

  pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}

BOOL CDiffEditCtrl::OnMouseWheel(UINT fFlags, short zDelta, CPoint point) 
{
  BOOL bResult = SECEditCtrl::OnMouseWheel(fFlags, zDelta, point);

  if (bResult)
    //return TRUE;

  UNUSED_ALWAYS(point);
	UNUSED_ALWAYS(fFlags);

	// if we have a vertical scroll bar, the wheel scrolls that
	// if we have _only_ a horizontal scroll bar, the wheel scrolls that
	// otherwise, don't do any work at all
	DWORD dwStyle = GetStyle();
	CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
	BOOL bHasVertBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_VSCROLL);

  if (!bHasVertBar)
    return FALSE;

	int nToScroll;
	nToScroll = zDelta / WHEEL_DELTA;
  WPARAM upDown = zDelta > 0 ? SB_LINEUP : SB_LINEDOWN;

  PostMessage(WM_VSCROLL, upDown,    0 );

  return TRUE;
}

// MCN - set the top line
void CDiffEditCtrl::SyncLine(PLINEDESC pTop) 
{
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);
	
	if (!AfxIsValidAddress(pTop, sizeof(LINEDESC)))
	{
		ASSERT(false);
		return;
	}

	m_pTopLine = pTop;
	m_iFirstLineNo = pEdit->GetLineNo(m_pTopLine);

  m_bSendVScrollChanged = false;
  MySetScrollPos(SB_VERT, m_iFirstLineNo - 1, TRUE);
  m_bSendVScrollChanged = true;
	
	m_bUpdate = TRUE;
	UpdateViews(this);
}

void CDiffEditCtrl::MySetCaretPos()
{
	SECEditCtrl::MySetCaretPos();

	SECEdit *pEdit = GetEdit();
	ASSERT(pEdit);

	if (m_bReadOnly)
		pEdit->SetReadOnly(TRUE);
	else if (m_pCurLine->state == STATE_FOLDED || HasBlockFoldedSelection())
		pEdit->SetReadOnly(TRUE);
	else
		pEdit->SetReadOnly(FALSE);
}

void CDiffEditCtrl::OnFold()
{
  TEXTPOS Start;
  TEXTPOS End;
  PLINEDESC pFoldedLine = NULL;
  BOOL bColumnar;

  GetSelection(Start, End, &bColumnar);

  if (End.pLine && End.iOffset == 0)
    End.pLine = End.pLine->pPrev;

  pFoldedLine = GetEdit()->FoldLines(Start.pLine, End.pLine, NULL);

  pFoldedLine->state = STATE_FOLDED;

  CountLines();
	m_pCurLine = pFoldedLine;
	m_iLineNo = GetEdit()->GetLineNo(m_pCurLine);
  UpdateViews(this);
	MakeCursorVisible();
}

void CDiffEditCtrl::OnUpdateFold(CCmdUI *pCmdUI)
{
  TEXTPOS Start;
  TEXTPOS End;
  BOOL bColumnar;
  if (GetSelection(Start, End, &bColumnar) && !bColumnar && Start.pLine != End.pLine)
  {
    //Check that all marked lines have the state STATE_SAME or STATE_FOLDED.
    PLINEDESC pLine = Start.pLine;
    while (pLine && pLine != End.pLine->pNext)
    {
      if (pLine->state != STATE_SAME && pLine->state != STATE_FOLDED)
      {
        pCmdUI->Enable(FALSE);
        return;
      }
      pLine = pLine->pNext;
    }
    pCmdUI->Enable(TRUE);
    return;
  }
  pCmdUI->Enable(FALSE);
}

void CDiffEditCtrl::OnUnFold()
{
  TEXTPOS Start;
  TEXTPOS End;
  BOOL bColumnar;
  BOOL bSelection = GetSelection(Start, End, &bColumnar);
  if ((!bSelection || (!bColumnar && Start.pLine == End.pLine)) 
                       && Start.pLine && Start.pLine->state == STATE_FOLDED)
  {
    PLINEDESC pFoldedLines = GetEdit()->UnFoldLines(Start.pLine, true);
    if(!pFoldedLines )
    {
      ASSERT(false);
      return;
    }
    if (m_pTopLine == Start.pLine)
      m_pTopLine = pFoldedLines;

	  m_pCurLine = pFoldedLines;

    CountLines();
    
	  m_iLineNo = GetEdit()->GetLineNo(m_pCurLine);
    ClearSelection(FALSE);
    UpdateViews(this);
    MakeCursorVisible();
  }
}

void CDiffEditCtrl::OnUpdateUnFold(CCmdUI *pCmdUI)
{
  TEXTPOS Start;
  TEXTPOS End;
  BOOL bColumnar;
  BOOL bSelection = GetSelection(Start, End, &bColumnar);
  if ((!bSelection || (!bColumnar && Start.pLine == End.pLine)) 
                       && Start.pLine && Start.pLine->state == STATE_FOLDED)
  {
    pCmdUI->Enable(TRUE);
    return;
  }
  pCmdUI->Enable(FALSE);
}

PLINEDESC CDiffEditCtrl::CreateFoldedLine(int iStart, int iEnd)
{
   CString str;

   str.Format("+---- %d Lines folded ------------", iEnd - iStart + 1);

   PLINEDESC pFoldedLine = GetEdit()->AllocLine(str);
   pFoldedLine->state = STATE_FOLDED;
   return pFoldedLine;
}

int CDiffEditCtrl::InitView()
{
  int nRes    = SECEditCtrl::InitView();
  m_bReadOnly = GetEdit()->GetReadOnly() != FALSE;

  return nRes;
}

bool CDiffEditCtrl::HasBlockFoldedSelection()
{
  TEXTPOS Start;
  TEXTPOS End;
  BOOL bColumnar;
  if (!m_bSelecting)
    return false;

  if (GetSelection(Start, End, &bColumnar) && bColumnar)
  {
    PLINEDESC pLine = Start.pLine;
    while (pLine)
    {
      if (pLine->state = STATE_FOLDED)
        return true;

      if (pLine == End.pLine)
        break;
    }
  }
  
  return false;
}


/*
void CDiffEditCtrl::OnUnFold()
{
  TEXTPOS Start;
  TEXTPOS End;
  BOOL bColumnar;
  BOOL bModified = GetEdit()->GetModified();
  BOOL bSelection = GetSelection(Start, End, &bColumnar);
  if ((!bSelection || (!bColumnar && Start.pLine == End.pLine)) 
                       && Start.pLine && Start.pLine->state == STATE_FOLDED)
  {
    PLINEDESC pFoldedLines = NULL;
    if (m_FoldedMap.Lookup(Start.pLine, (void *&)pFoldedLines))
    {
      PLINEDESC pBef        = NULL;
      PLINEDESC pAfter      = NULL;

      pAfter = Start.pLine->pPrev;
      pBef   = End.pLine->pNext;

      if (GetEdit()->DetachLines(Start.pLine, Start.pLine) != NULL)
      {

        GetEdit()->InsertLines(pBef, pAfter, pFoldedLines);

        GetEdit()->FreeLines(Start.pLine);
        m_FoldedMap.RemoveKey(Start.pLine);
      }
      else
      {
        ASSERT(FALSE);
      }

    	CountLines();
	    m_pCurLine = pFoldedLines;
	    m_iLineNo = GetEdit()->GetLineNo(m_pCurLine);
      UpdateViews(this);
    	MakeCursorVisible();
    }
    else
    {
      ASSERT(FALSE);
    }
  }
  //Don't change the modified flag.
  GetEdit()->SetModified(bModified);
}

void CDiffEditCtrl::OnUnFold()
{
  TEXTPOS Start;
  TEXTPOS End;
  BOOL bColumnar;
  BOOL bModified = GetEdit()->GetModified();
  BOOL bSelection = GetSelection(Start, End, &bColumnar);
  if ((!bSelection || (!bColumnar && Start.pLine == End.pLine)) 
                       && Start.pLine && Start.pLine->state == STATE_FOLDED)
  {
    PLINEDESC pFoldedLines = NULL;
    if (m_FoldedMap.Lookup(Start.pLine, (void *&)pFoldedLines))
    {
      PLINEDESC pBef        = NULL;
      PLINEDESC pAfter      = NULL;

      pAfter = Start.pLine->pPrev;
      pBef   = End.pLine->pNext;

      if (GetEdit()->DetachLines(Start.pLine, Start.pLine) != NULL)
      {

        GetEdit()->InsertLines(pBef, pAfter, pFoldedLines);

        GetEdit()->FreeLines(Start.pLine);
        m_FoldedMap.RemoveKey(Start.pLine);
      }
      else
      {
        ASSERT(FALSE);
      }

    	CountLines();
	    m_pCurLine = pFoldedLines;
	    m_iLineNo = GetEdit()->GetLineNo(m_pCurLine);
      UpdateViews(this);
    	MakeCursorVisible();
    }
    else
    {
      ASSERT(FALSE);
    }
  }
  //Don't change the modified flag.
  GetEdit()->SetModified(bModified);
}


*/

void CDiffEditCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SECEditCtrl::OnLButtonDblClk(nFlags, point);

  OnUnFold();
}

void CDiffEditCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (HIWORD(GetKeyState(VK_CONTROL)))
  {
    GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_VIEW_NEXT_DIFF_FROM_CURSOR, 1), 0);
  }
	
	SECEditCtrl::OnLButtonUp(nFlags, point);
}

int CDiffEditCtrl::GetDisplayLineNo(PLINEDESC pLine, CDC *pDC)
{
	CFilDiffView * pWnd= (CFilDiffView *) GetParent();
	return pWnd->GetDisplayLineNo(this, pLine, pDC);
}

BOOL CDiffEditCtrl::PaintTextOut( SECEdit* pEdit, 
					              	        PLINEDESC lpLine, 	// pointer to the current line
							                    LPSTR str, 			// the full text line, tabs expanded
							                    int nLength,			// the full length of the text
							                    int nIndex, 			// first character to draw
							                    int nMaxCount, 			// max characters to draw
							                    BOOL bChroma, 			// do it in color
							                    CDC* dc, 				// printer's dc
							                    int x, 					// textout coordinates
							                    int y)
{
  if (lpLine->iLineNo != m_nLDiffLineNo && lpLine->iLineNo != m_nRDiffLineNo)
  {
    return SECEditCtrl::PaintTextOut(pEdit, lpLine, str, nLength, nIndex, nMaxCount, bChroma, dc, x, y);
  }

  CUIntArray *pDiffSect = &m_aRDiffLineSect;
  COLORREF crDiffColor = m_crRight;

  if (lpLine->iLineNo == m_nLDiffLineNo)
  {
    pDiffSect = &m_aLDiffLineSect;
    crDiffColor = m_crLeft;
  }

  int nIdx = nIndex;
  CString strText;
  LPTSTR pszPaint = NULL;
  int nCount = 0;
  for (int i = 0; i < pDiffSect->GetSize(); i++)
  {
    if(nIdx >= nLength)
      break;
    
    if (i % 2 == 0)
    {
      dc->SetBkColor(::GetSysColor(COLOR_WINDOW));
    }
    else
    {
      dc->SetBkColor(crDiffColor);
    }

    nCount = pDiffSect->GetAt(i) - nIdx;
    if (nCount > 0)
    {
      pszPaint = strText.GetBuffer(nCount + 1);
      _tcsncpy(pszPaint, &str[nIdx], nCount);
      pszPaint[nCount] = 0;
      strText.ReleaseBuffer();
      dc->TextOut(x, y, strText);
      x += GetWidth(dc, strText, nCount);
    }
    nIdx = pDiffSect->GetAt(i);
  }

  dc->SetBkColor(::GetSysColor(COLOR_WINDOW));

  nCount = nLength - nIdx;
  if (nCount > 0)
  {
    pszPaint = strText.GetBuffer(nCount + 1);
    _tcsncpy(pszPaint, &str[nIdx], nCount);
    pszPaint[nCount] = 0;
    strText.ReleaseBuffer();
    dc->TextOut(x, y, strText);
  }
  return TRUE;
}
