/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffView.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffView.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "AfxTempl.h"
#include "AfxPriv.h"
#include "Fcu.h"
#include "Util.h"
#include "FilDiff_res.h"
#include "FCUOptions.h"
#include "state.h"
#include "list.h"
#include "FilDiffDoc.h"
#include "FilDiffFrm.h"
#include "FilDiffview.h"
#include "SaveCompDlg.h"
#include "VarMgr.h"
#include "Popup.h"
#include "SmxPrnTable.h"
#include "SmxRes.h"
#include "SmxPgSetupDlg.h"
#include "Util.h"
#include "PxshlApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DIM_MRG 3
#define DIM_SPLIT  2
#define ID_StatusTimer 2001

#define ThisDoc (GetFilDiffDoc())

void GetFileName(CString& strFilePath);
void GetFileDir(CString& strFilePath);

extern LPCTSTR szProperties;

//static member declarations
HCURSOR CFilDiffView::m_splitCurH=NULL;
HCURSOR CFilDiffView::m_splitCurV=NULL;

#define MYPERCENT 1000.0
#define DEF_HSPLIT 500
#define DEF_VSPLIT 350
namespace
{
  LPCTSTR szLayoutFil      = _T("LayoutFil");
  LPCTSTR szCMD            = _T("CMD");
  LPCTSTR szHSplit         = _T("HSplit");
  LPCTSTR szVSplit         = _T("VSplit");
  LPCTSTR szFilDiffPgSetup = _T("FilDiffPgSetup");
}

IMPLEMENT_DYNCREATE(CFilDiffView, CSmxPrnFormView)

CFilDiffView::CFilDiffView()
	: CSmxPrnFormView(CFilDiffView::IDD),
	m_bInitialized(FALSE)
{
	//{{AFX_DATA_INIT(CFilDiffView)
	//}}AFX_DATA_INIT

  m_split = 0;
	m_hTracker.m_nStyle |= CRectTracker::hatchInside | RectTracker_OnlyMoveVert;
	m_vTracker.m_nStyle |= CRectTracker::hatchInside | RectTracker_OnlyMoveHorz;
	m_lastActiveWnd = NULL;
  m_nStatusTimer  = -1;
  m_bVLayoutChanged = false;
  m_bHLayoutChanged = false;

  m_LeftFileWnd.SetAccelTableID(IDR_DIFF_EDIT);
  m_RightFileWnd.SetAccelTableID(IDR_DIFF_EDIT);
  m_MergeFileWnd.SetAccelTableID(IDR_DIFF_EDIT);
}

CFilDiffView::~CFilDiffView()
{
}

void CFilDiffView::DoDataExchange(CDataExchange* pDX)
{
	CSmxPrnFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilDiffView)
  DDX_Control(pDX, IDC_stRightFile, m_RightFileName);
  DDX_Control(pDX, IDC_stLeftFile, m_LeftFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFilDiffView, CSmxPrnFormView)
	//{{AFX_MSG_MAP(CFilDiffView)
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_WINDOWS_COMPFILE, OnWindowsCompfile)
	ON_UPDATE_COMMAND_UI(ID_WINDOWS_COMPFILE, OnUpdateWindowsCompfile)
	ON_COMMAND(ID_WINDOWS_LEFTFILE, OnWindowsLeftfile)
	ON_UPDATE_COMMAND_UI(ID_WINDOWS_LEFTFILE, OnUpdateWindowsLeftfile)
	ON_COMMAND(ID_WINDOWS_RIGHTFILE, OnWindowsRightfile)
	ON_UPDATE_COMMAND_UI(ID_WINDOWS_RIGHTFILE, OnUpdateWindowsRightfile)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTLEFT, OnUpdateEditInsertleft)
	ON_COMMAND(ID_EDIT_INSERTLEFT, OnEditInsertleft)
	ON_COMMAND(ID_EDIT_INSERTRIGHT, OnEditInsertright)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTRIGHT, OnUpdateEditInsertright)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINEDIFF, OnUpdateViewLineDiff)
	ON_COMMAND(ID_VIEW_LINEDIFF, OnViewLineDiff)
	ON_COMMAND(ID_VIEW_NEXTDIFF, OnViewNextdiff)
	ON_COMMAND(ID_VIEW_PREVDIFF, OnViewPrevdiff)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVECOMPOSED, OnUpdateFileSavecomposed)
	ON_COMMAND(ID_FILE_SAVECOMPOSED, OnFileSavecomposed)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdateCmdIndicatorCmd)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DIFF, OnUpdateCmdIndicatorDiff)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINE, OnUpdateCmdIndicatorLine)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_INSERTBOTH, OnEditInsertboth)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTBOTH, OnUpdateEditInsertboth)
	ON_COMMAND(ID_EDIT_DELETEDIFF, OnEditDeletediff)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXTDIFF, OnUpdateViewNextdiff)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREVDIFF, OnUpdateViewPrevdiff)
	ON_COMMAND(ID_EDIT_RESTORELEFT, OnEditRestoreLeft)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESTORELEFT, OnUpdateEditRestoreLeft)
	ON_COMMAND(ID_EDIT_RESTORERIGHT, OnEditRestoreRight)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESTORERIGHT, OnUpdateEditRestoreRight)
	ON_MESSAGE(WMU_DELSECTMARK, OnDelSectMark)
	ON_MESSAGE(WMU_SPLITLINE, OnSplitLines)
	ON_MESSAGE(WMU_JOINLINES, OnJoinLines)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
  ON_COMMAND(ID_VIEW_NEXT_DIFF_FROM_CURSOR, OnNextDiffFromCursor)
	ON_COMMAND(ID_FILE_XMLEXPORT, OnFileXmlExport)
	ON_COMMAND(ID_OPTIONS_FOLDLINES, OnOptionsFoldlines)
	ON_COMMAND(ID_OPTIONS_SHOWLINENO, OnOptionsShowlineno)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FOLDLINES, OnUpdateOptionsFoldlines)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWLINENO, OnUpdateOptionsShowlineno)
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSavecomposed)
	ON_COMMAND(ID_FILE_SAVE, OnFileSavecomposed)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETEDIFF, OnUpdateEditInsertboth)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_CONTROL(EN_VSCROLLCHANGED, IDC_ED_FLEFT, OnLScrollChanged)
	ON_CONTROL(EN_VSCROLLCHANGED, IDC_ED_FRIGHT, OnRScrollChanged)
	ON_CONTROL(EN_VSCROLLCHANGED, IDC_ED_FMERGE, OnMScrollChanged)
	ON_COMMAND(ID_OPTIONS_SYNCSCROLLING, OnOptionsSyncscrolling)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SYNCSCROLLING, OnUpdateOptionsSyncscrolling)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CSmxPrnFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CSmxPrnFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CSmxPrnFormView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
#ifdef _DEBUG
	ON_MESSAGE(WMU_DELCOMPLETE, OnDelComplete)
#endif
END_MESSAGE_MAP()


void CFilDiffView::OnSize(UINT nType, int cx, int cy) 
{
	CSmxPrnFormView::OnSize(nType, cx, cy);
	CRect rect; 
	GetClientRect(rect);

	if(m_bInitialized) 
	{
		UpdateWndLayout(FALSE);
	}
	
}

void CFilDiffView::SetHitCursor(CPoint point)
{ 
	CRect rect;
	int xl, xh, yl, yh;

  rect.SetRectEmpty();

  if (GetOption(OCF_ShowLeft))
    m_LeftFileWnd.GetWindowRect(rect);
  else
    m_RightFileWnd.GetWindowRect(rect);

	xl = DIM_MRG + DIM_SPLIT + rect.Width()-3;
	xh = xl + DIM_SPLIT+6;
	yl = DIM_MRG;
  yh = DIM_MRG + rect.Height();

	m_split = 0;

  if(GetOption(OCF_ShowLeft) && GetOption(OCF_ShowRight))
  {
    if(xl <= point.x && xh >= point.x && yl <= point.y && yh >= point.y)
	  {
		  if (m_splitCurV == 0) 
      {
			  m_splitCurV = AfxGetApp()->LoadCursor(AFX_IDC_HSPLITBAR);

        if (m_splitCurV == NULL)
				  m_splitCurV = AfxGetApp()->LoadCursor(IDC_VSIZE);
		  }			
		  ::SetCursor(m_splitCurV);
		  m_split = 1;
      return;
    }
  }

  if(GetOption(OCF_ShowComposed))
  {
		GetClientRect(rect);
		xl = DIM_MRG;
		xh = rect.right - 2* DIM_MRG;
		yl = yh-1;
		yh = yl + DIM_SPLIT + 4;
		if(xl <= point.x && xh >= point.x && yl <= point.y && yh >= point.y)
		{
			if (m_splitCurH == 0) 
      {
				m_splitCurH = AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR);
				if (m_splitCurH == NULL)
					m_splitCurH = AfxGetApp()->LoadCursor(IDC_VSIZE);
			}			
			::SetCursor(m_splitCurH);
			m_split = 2;
      return;
    }
	}
}


void CFilDiffView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(nFlags == 0 || nFlags == MK_LBUTTON) 
  {
		//Check if Point is on Horiz. or Vertical splitting line
		SetHitCursor(point);
	}

	CSmxPrnFormView::OnMouseMove(nFlags, point);
}

void CFilDiffView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int dx = 0; 
	int dy = 0;
	int x, y, w, h;

	SetHitCursor(point);

  DWORD dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
  dwStyle &= ~WS_CLIPCHILDREN;
  SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle);

  if (m_hTracker.HitTest(point) != CRectTracker::hitNothing) 
  {
		m_hTracker.TrackFromHitTest(CRectTracker::hitMiddle, this, point, FALSE);
		dy = m_hTracker.m_rect.top - m_hTracker.m_OrigRect.top;	
    if (dy)
      m_bVLayoutChanged = true;
	}
	else if (m_vTracker.HitTest(point) != CRectTracker::hitNothing) 
  {
		m_vTracker.TrackFromHitTest(CRectTracker::hitMiddle, this, point, FALSE);
		dx = m_vTracker.m_rect.left - m_vTracker.m_OrigRect.left;	
    if (dx)
      m_bHLayoutChanged = true;
	}

  dwStyle = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
  dwStyle |= WS_CLIPCHILDREN;
  SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle);

	CRect rect;
	m_LeftFileWnd.GetWindowRect(rect);
	ScreenToClient(rect);
	x = rect.left;
	y = rect.top;
	w = rect.Width() + dx;
	h = rect.Height() + dy;
	m_LeftFileWnd.MoveWindow(x, y, w, h);
	
	m_RightFileWnd.GetWindowRect(rect);
	ScreenToClient(rect);
	x = rect.left + dx;
	y = rect.top;
	w = rect.Width() - dx;
	h = rect.Height() + dy;
	m_RightFileWnd.MoveWindow(x, y, w, h);

	m_MergeFileWnd.GetWindowRect(rect);
	ScreenToClient(rect);
	x = rect.left;
	y = rect.top + dy;
	w = rect.Width();
	h = rect.Height() - dy;
	m_MergeFileWnd.MoveWindow(x, y, w, h);
	UpdateWndLayout(FALSE);
  StoreSplitRatio();
}

void CFilDiffView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	CFilDiffDoc *pDoc = (CFilDiffDoc *) GetDocument();

	ASSERT(pDoc);
  LPCTSTR pszEditorSection = GetOptions().sm_szEditSection;
	
	m_LeftFileWnd.Initialize(IDC_ED_FLEFT, this);
	m_MergeFileWnd.Initialize(IDC_ED_FMERGE, this);
	m_RightFileWnd.Initialize(IDC_ED_FRIGHT, this);
	pDoc->m_leftEdit.SetFileMappingKey(pszEditorSection);
	pDoc->m_leftEdit.InitFileTypeSettings();
  m_LeftFileWnd.AttachEdit(&pDoc->m_leftEdit);
	m_LeftFileWnd.GetEdit()->SetReadOnly(TRUE);
	m_LeftFileWnd.InitView();
	m_LeftFileWnd.Invalidate();

  pDoc->m_rightEdit.SetFileMappingKey(pszEditorSection);
	pDoc->m_rightEdit.InitFileTypeSettings();
	m_RightFileWnd.AttachEdit(&pDoc->m_rightEdit);
	m_RightFileWnd.GetEdit()->SetReadOnly(TRUE);
	m_RightFileWnd.InitView();
	m_RightFileWnd.Invalidate();

  pDoc->m_mergeEdit.SetFileMappingKey(pszEditorSection);
	pDoc->m_mergeEdit.InitFileTypeSettings();
	m_MergeFileWnd.AttachEdit(&pDoc->m_mergeEdit);
	m_MergeFileWnd.InitView();

  m_MergeFileWnd.Invalidate();

  if (GetOptions().GetOption(OCF_SyncScrolling))
  {
    m_LeftFileWnd.m_bSendVScrollChanged = true;
	  m_RightFileWnd.m_bSendVScrollChanged = true;
	  m_MergeFileWnd.m_bSendVScrollChanged = true;
  }

	m_curDiffSection = NULL;

	WINDOWPLACEMENT *pWplmt;
	int len;
	AfxGetApp()->GetProfileBinary(GetOptions().GetSection(), szLayoutFil,(BYTE **) &pWplmt, (UINT *) &len);

  if(len == sizeof(WINDOWPLACEMENT))
	{
		GetParent()->SetWindowPlacement(pWplmt);
		delete pWplmt;
	}
	
  m_LeftFileName.SetWindowText(((CFilDiffDoc *) GetDocument())->GetLeftFileName());
  m_RightFileName.SetWindowText(((CFilDiffDoc *) GetDocument())->GetRightFileName());
	UpdateWndLayout(TRUE);

	m_bInitialized = TRUE;
}

BOOL CFilDiffView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(::GetCursor() != m_splitCurV)
	{
		int kk = 0;
	}
	return CSmxPrnFormView::OnSetCursor(pWnd, nHitTest, message);
}

void CFilDiffView::UpdateWndLayout(bool bInitial)
{
	int leftW, rightW, compW, leftH, rightH, compH;
	CRect rect, leftRect, rightRect, compRect;

	GetClientRect(rect);
	int ww = rect.Width();
	int hh = rect.Height();

	m_RightFileWnd.GetWindowRect(rightRect);
	m_LeftFileWnd.GetWindowRect(leftRect);
	m_MergeFileWnd.GetWindowRect(compRect);
	ScreenToClient(rightRect);
	ScreenToClient(leftRect);
	ScreenToClient(compRect);
	
	leftW  = leftRect.Width();
	leftH  = leftRect.Height();
	rightW = rightRect.Width();
	rightH = rightRect.Height();
	compW  = compRect.Width();
	compH  = compRect.Height();

  int nHSplit = 0;
  int nVSplit = 0;
	if(!m_bVLayoutChanged && !m_bHLayoutChanged) 
  {
	  leftW = rightW = leftH = rightH = compW = compH = 0;

    nHSplit = AfxGetApp()->GetProfileInt(GetOptions().GetSection(), szHSplit, DEF_HSPLIT);
    
    if (nHSplit == 0)
      nHSplit = DEF_HSPLIT;
    
    nVSplit = AfxGetApp()->GetProfileInt(GetOptions().GetSection(), szVSplit, DEF_VSPLIT);

    if (nVSplit == 0)
      nVSplit = DEF_VSPLIT;
	}

	if(GetOption(OCF_ShowLeft) && GetOption(OCF_ShowRight))
	{
		//both file windows should be shown
		if(leftW == 0 || rightW == 0)
		{
			//split them equal
			leftW = (int) ((ww - 2 * DIM_MRG - DIM_SPLIT) * nHSplit / MYPERCENT + 0.5);
			rightW = (int) (ww - 2 * DIM_MRG - DIM_SPLIT - leftW);
		} 
    else 
    {
			//maybe resize?
			int diffW = ww - 2 * DIM_MRG - DIM_SPLIT - leftW - rightW;
			leftW += diffW / 2;
			rightW += (diffW - diffW / 2);
		}


	}
	else if (GetOption(OCF_ShowRight) || GetOption(OCF_ShowLeft))
	{
		if(GetOption(OCF_ShowRight))
			rightW = ww - 2 * DIM_MRG;
		else
			leftW = ww - 2 * DIM_MRG;
		
	}

	if((GetOption(OCF_ShowRight) || GetOption(OCF_ShowLeft)) 
		&& GetOption(OCF_ShowComposed))
	{
		//we have two vertical windows
		if((!leftH && !rightH) || !compH)
		{
			//split up 
			leftH = rightH = (int) ((hh -  2 * DIM_MRG - DIM_SPLIT) * nVSplit / MYPERCENT + 0.5);
			compH = hh - 2 * DIM_MRG - DIM_SPLIT - leftH;
		} 
    else 
    {
			//maybe resize ?
			int diffH = hh - 2*DIM_MRG - DIM_SPLIT -max(leftH, rightH) 
				        - compH;
			if(!leftH) {
				rightH += diffH/2;
				leftH = rightH;
			} else { 
				leftH += diffH/2;
				rightH = leftH;
			}
			compH += (diffH - diffH/2);
		}
		compW = ww - 2*DIM_MRG;

	} 
	else
	{
		//we have just one window
		if(GetOption(OCF_ShowRight))
			rightH = hh - 2 * DIM_MRG;

		if(GetOption(OCF_ShowLeft))
			leftH = hh - 2 * DIM_MRG;

		if(GetOption(OCF_ShowComposed)) 
    {
 			compH = hh - 2 * DIM_MRG;
			compW = ww - 2 * DIM_MRG;
		}
	}

	//hide the respective windows and set w = h = 0
	MyHideWindow(m_LeftFileWnd, GetOption(OCF_ShowLeft), leftW, leftH);
	MyHideWindow(m_RightFileWnd, GetOption(OCF_ShowRight), rightW, rightH);
	MyHideWindow(m_MergeFileWnd, GetOption(OCF_ShowComposed), compW, compH);

  int nFileNameH = 0;
  int nEditTop   = DIM_MRG;
  if (GetOptions().GetOption(OCF_ShowFileName))
  {
    CRect rcWnd;
    GetDlgItem(IDC_stLeftFile)->GetWindowRect(rcWnd);
    nFileNameH = rcWnd.Height();
    if (leftW && leftH)
    {
      GetDlgItem(IDC_stLeftFile)->MoveWindow(DIM_MRG + 1, DIM_MRG, leftW - 2, nFileNameH);
      GetDlgItem(IDC_stLeftFile)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_stLeftFile)->Invalidate();
    }
    else
    {
      GetDlgItem(IDC_stLeftFile)->ShowWindow(SW_HIDE);
    }

    if (rightW && rightH)
    {
      GetDlgItem(IDC_stRightFile)->MoveWindow(ww - DIM_MRG - rightW + 1, DIM_MRG, rightW - 2, nFileNameH);
      GetDlgItem(IDC_stRightFile)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_stRightFile)->Invalidate();
    }
    else
    {
      GetDlgItem(IDC_stRightFile)->ShowWindow(SW_HIDE);
    }
    nEditTop += 3 + nFileNameH;
    nFileNameH += DIM_MRG;
  }
  else
  {
    GetDlgItem(IDC_stLeftFile)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_stRightFile)->ShowWindow(SW_HIDE);
  }

  leftH = max(0, leftH - nFileNameH);
  rightH = max(0, rightH - nFileNameH);
	m_LeftFileWnd.MoveWindow(DIM_MRG, nEditTop, leftW, leftH);
	m_RightFileWnd.MoveWindow(ww - DIM_MRG - rightW, nEditTop, rightW, rightH);
	m_MergeFileWnd.MoveWindow(DIM_MRG, hh - DIM_MRG 
		                      - compH, compW, compH);

	//resize tracker
	if(GetOption(OCF_ShowRight) && GetOption(OCF_ShowLeft) )
	{
		//resize Tracker
		rect.left = DIM_MRG + leftW - 1;
		rect.right = rect.left + DIM_SPLIT+2;
		rect.top = DIM_MRG;
		rect.bottom = DIM_MRG + max(leftH, rightH);
	} 
  else 
  {
		//Clear tracker
		rect.left = rect.right = rect.top = rect.bottom = 0;
	}
	m_vTracker.m_rect = rect;

	if(GetOption(OCF_ShowLeft) || GetOption(OCF_ShowRight)
		&& GetOption(OCF_ShowComposed))
	{
		//resize Tracker
		rect.left = DIM_MRG;
		rect.right = rect.left + compW;
		rect.top = nEditTop + max(leftH, rightH) - 1;
		rect.bottom = rect.top + DIM_SPLIT + 2;
	} 
  else 
  {
		//Clear tracker
		rect.left = rect.right = rect.top = rect.bottom = 0;
	}
	m_hTracker.m_rect = rect;
}

void CFilDiffView::OnWindowsCompfile() 
{
	ToggleOption(OCF_ShowComposed);
  UpdateWndLayout(false);
	
}

void CFilDiffView::OnUpdateWindowsCompfile(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetOption(OCF_ShowComposed));
	
}

void CFilDiffView::OnWindowsLeftfile() 
{
  ToggleOption(OCF_ShowLeft);
	UpdateWndLayout(FALSE);
}

void CFilDiffView::OnUpdateWindowsLeftfile(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetOption(OCF_ShowLeft));
}

void CFilDiffView::OnWindowsRightfile() 
{
  ToggleOption(OCF_ShowRight);
	UpdateWndLayout(FALSE);
}

void CFilDiffView::OnUpdateWindowsRightfile(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetOption(OCF_ShowRight));
}

void CFilDiffView::MyHideWindow(CDiffEditCtrl & Wnd, bool bShow, int &nWidth, int &nHeight)
{
	if(bShow)
	{
		ASSERT(nWidth);
		ASSERT(nHeight);
		Wnd.ShowWindow(SW_SHOW);
	}
	else
	{
		nWidth  = nHeight = 0;
		Wnd.ShowWindow(SW_HIDE);
	}
}

void CFilDiffView::UpdateEditControls()
{
  bool bChanged = false;
  if (CDiffEditCtrl::m_crLeft != GetOptions().GetEditColorLeft() || 
      CDiffEditCtrl::m_crRight != GetOptions().GetEditColorRight() || 
      CDiffEditCtrl::m_crFolded != GetOptions().GetEditColorFolded())
  {
    CDiffEditCtrl::m_crLeft  = GetOptions().GetEditColorLeft();
    CDiffEditCtrl::m_crRight = GetOptions().GetEditColorRight();
    CDiffEditCtrl::m_crFolded = GetOptions().GetEditColorFolded();
    bChanged = true;
  }

	if (GetOptions().EditFontChanged())
  {
    GetOptions().StoreEditFont();
    m_RightFileWnd.SetFont(GetOptions().GetEditFont());
	  m_LeftFileWnd.SetFont(GetOptions().GetEditFont());
	  m_MergeFileWnd.SetFont(GetOptions().GetEditFont());

    bChanged = true;
  }

	if (GetOptions().EditTabSizeChanged() || GetOptions().HasChanged_Option(OCF_Editor))
  {
    GetOptions().StoreEditTabWidth();
    GetOptions().StoreEditOptions();
    m_RightFileWnd.GetEdit()->InitFileTypeSettings();
	  m_LeftFileWnd.GetEdit()->InitFileTypeSettings();
	  m_MergeFileWnd.GetEdit()->InitFileTypeSettings();

    bChanged = true;
  }

	if (GetOptions().HasChanged_Option(OCF_ShowLineNo))
  {
    GetOptions().StoreEditOptions();
    //Note that InitFileTypeSettings was executed at this point
    m_RightFileWnd.InitView();
	  m_LeftFileWnd.InitView();
	  m_MergeFileWnd.InitView();
    bChanged = true;
  }

  if (GetOptions().HasChanged_Option(OCF_FoldLines) || GetOptions().NofUnfoldedLinesChanged())
  {
    GetOptions().StoreNofUnfoldedLines();
    if ((GetPreviousOptions() && GetPreviousOptions()->GetNofUnfoldedLines() == 0) ||
        GetOptions().GetNofUnfoldedLines() == 0)
    {
      OnViewRefresh();
    }
    else
    {
      ((CFilDiffDoc *) GetDocument())->UpdateFolding();
    }

    bChanged = true;
  }

	bool bVSyncScrolling = GetOptions().GetOption(OCF_SyncScrolling);
  m_LeftFileWnd.m_bSendVScrollChanged =  bVSyncScrolling;
	m_RightFileWnd.m_bSendVScrollChanged = bVSyncScrolling;
	m_MergeFileWnd.m_bSendVScrollChanged = bVSyncScrolling;

  if (bChanged)
  {
    m_LeftFileWnd.Invalidate();
    m_RightFileWnd.Invalidate();
    m_MergeFileWnd.Invalidate();
  }
}


void CFilDiffView::OnUpdateEditInsertleft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ThisDoc->GetActualSect() != NULL);
}

void CFilDiffView::OnEditInsertleft() 
{
	CDiffSectElt *pSect = ThisDoc->GetActualSect();

	if(!pSect)
		return;

	if(!pSect->IsLEmpty() && pSect->IsCLEmpty())
		InsertLeftSect(pSect);

	
	if(!pSect->IsCREmpty())
		DeleteRightSect(pSect);

	MarkDiffSect(pSect);
}

void CFilDiffView::OnEditInsertright() 
{
	CDiffSectElt *pSect = ThisDoc->GetActualSect();

	if(!pSect)
		return;

	if(!pSect->IsREmpty() && pSect->IsCREmpty())
		InsertRightSect(pSect);

	
	if(!pSect->IsCLEmpty())
		DeleteLeftSect(pSect);

	MarkDiffSect(pSect);
}

void CFilDiffView::OnUpdateEditInsertright(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ThisDoc->GetActualSect() != NULL);
}

void CFilDiffView::OnViewNextdiff() 
{
	if (ThisDoc->GetDiffArray()->GetSize() == 0)
  {
		AfxMessageBox(IDS_ThereAreNoDifferencies, MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  CWnd *pWnd = GetFocus();
  CDiffEditCtrl *pEdit = NULL;

  if (pWnd == &m_LeftFileWnd || pWnd == &m_RightFileWnd || pWnd == &m_MergeFileWnd)
    pEdit = (CDiffEditCtrl*) pWnd;

  if (!ThisDoc->GetActualSect() || !pEdit || pEdit->HasMarkedLines())
  {

    if(!ThisDoc->NextDiff()) 
	  {
      if (!GetOption(OCF_Wrap))
      {
        AfxMessageBox(IDS_NoMoreDifferencies, MB_OK | MB_ICONEXCLAMATION);
        MarkDiffSect(NULL);
        return;
      }
		  
      SetStatusText(IDS_PassedEndOfFile, true);
      Sleep(300);
      ThisDoc->NextDiff();


    }
	}
	
	CDiffSectElt *pSect = ThisDoc->GetActualSect();
	ASSERT(pSect);
	MarkDiffSect(pSect);
}

void CFilDiffView::OnViewPrevdiff() 
{
	if (ThisDoc->GetDiffArray()->GetSize() == 0)
  {
		AfxMessageBox(IDS_ThereAreNoDifferencies, MB_OK | MB_ICONEXCLAMATION);
    return;
  }

	if(!ThisDoc->PrevDiff()) 
  {
    if (!GetOption(OCF_Wrap))
    {
      AfxMessageBox(IDS_TheFirstDifferenceReached, MB_OK | MB_ICONEXCLAMATION);
      MarkDiffSect(NULL);
      return;
    }
		
    ThisDoc->PrevDiff();
    SetStatusText(IDS_PassedBeginningOfFile, true);
	}
	
	CDiffSectElt *pSect = ThisDoc->GetActualSect();
	ASSERT(pSect);
	MarkDiffSect(pSect);
}

void CFilDiffView::MarkDiffSect(CDiffSectElt * pSect, bool bCenterLine)
{
	if(pSect == NULL) 
	{
		//unmark all windows
		m_RightFileWnd.MarkLine(0, 0, bCenterLine);
		m_LeftFileWnd.MarkLine (0, 0, bCenterLine);
		m_MergeFileWnd.MarkLine(0, 0, bCenterLine);
	} 
	else 
	{
		m_RightFileWnd.MarkLine(pSect->GetRStart(), pSect->GetREnd(), bCenterLine);
		m_LeftFileWnd.MarkLine (pSect->GetLStart(), pSect->GetLEnd(), bCenterLine);
		if(!pSect->GetCStart() && !pSect->GetCEnd())
		{
			PLINEDESC pAfter = ThisDoc->GetSectAfter(pSect);
			ASSERT(pAfter == NULL || AfxIsValidAddress(pAfter, sizeof(LINEDESC)));
			m_MergeFileWnd.MarkLine(pAfter, NULL, bCenterLine);
		}
		else
		{
			TRY
			{
				ASSERT(pSect->GetCStart() && pSect->GetCEnd());
				ASSERT(AfxIsValidAddress(pSect->GetCStart(), sizeof(LINEDESC)));
				ASSERT(AfxIsValidAddress(pSect->GetCEnd(), sizeof(LINEDESC)));

				m_MergeFileWnd.MarkLine(pSect->GetCStart(), pSect->GetCEnd(), bCenterLine);
			}
			CATCH_ALL(e)
			{
				PLINEDESC pAfter = ThisDoc->GetSectAfter(pSect);
				m_MergeFileWnd.MarkLine(pAfter, NULL, bCenterLine);
			}
			END_CATCH_ALL
		}
	}

  m_MergeFileWnd.m_nLDiffLineNo = -1;
  m_MergeFileWnd.m_nRDiffLineNo = -1;

}


void CFilDiffView::SetLastActiveWnd(CDiffEditCtrl * pWnd)
{
	m_lastActiveWnd = pWnd;
}

void CFilDiffView::OnUpdateFileSavecomposed(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	return;
}

void CFilDiffView::OnFileSavecomposed() 
{
	ThisDoc->SaveComposed();
}

void CFilDiffView::OnDestroy() 
{
	m_LeftFileWnd.DetachEdit();
	m_RightFileWnd.DetachEdit();
	m_MergeFileWnd.DetachEdit();
 
	
  WINDOWPLACEMENT wplmt;
	GetParent()->GetWindowPlacement(&wplmt);
	AfxGetApp()->WriteProfileBinary(GetOptions().GetSection(), szLayoutFil, (BYTE *) &wplmt, sizeof(WINDOWPLACEMENT));
	
  StoreSplitRatio();
  AfxGetApp()->WriteProfileBinary(GetOptions().GetSection(), szLayoutFil, (BYTE *) &wplmt, sizeof(WINDOWPLACEMENT));
	
	CSmxPrnFormView::OnDestroy();
}

void CFilDiffView::InsertLeftSect(CDiffSectElt * pSect)
{
	//Make sure that the left section isn't allready there
	if(!pSect->IsCLEmpty())
		return;

	PLINEDESC pStart, pEnd;
	PLINEDESC pBef = NULL;
	PLINEDESC pAfter = NULL;
	
	if(pSect->IsLEmpty())
		return;

	pStart = pSect->GetLStart();
	pEnd =   pSect->GetLEnd();

  pAfter = ThisDoc->GetSectAfter(pSect);
	if(!pAfter)
  {
	  pBef = ThisDoc->m_mergeEdit.m_pLine;
  }

	//Paste the string
	bool bReadonly = m_MergeFileWnd.GetEdit()->GetReadOnly() != NULL;
	m_MergeFileWnd.GetEdit()->SetReadOnly(FALSE);
	m_MergeFileWnd.InsertAllocLines(pBef, pAfter, pStart, pEnd);  
	
	m_MergeFileWnd.GetEdit()->SetReadOnly(bReadonly);

	//pBef, pAfter will point to the newly inserted lines
	pSect->SetCLLines(pBef, pAfter);
}

void CFilDiffView::InsertRightSect(CDiffSectElt * pSect)
{
	//Make sure that the right section isn't allready there
	if(!pSect->IsCREmpty())
		return;

	PLINEDESC pStart, pEnd;
	PLINEDESC pBef = NULL;
	PLINEDESC pAfter = NULL;

	if(pSect->IsREmpty())
		return;

	pStart = pSect->GetRStart();
	pEnd   = pSect->GetREnd();


  pAfter = ThisDoc->GetSectAfter(pSect);
	if(!pAfter)
  {
	  pBef = ThisDoc->m_mergeEdit.m_pLine;
  }

	//Paste the string
	bool bReadonly = m_MergeFileWnd.GetEdit()->GetReadOnly() != NULL;
	m_MergeFileWnd.GetEdit()->SetReadOnly(false);
	m_MergeFileWnd.InsertAllocLines(pBef, pAfter, pStart, pEnd);
	m_MergeFileWnd.GetEdit()->SetReadOnly(bReadonly);
	pSect->SetCRLines(pBef, pAfter);
}


bool CFilDiffView::InCmdMode()
{
	return(GetOptions().GetOption(OCF_CmdMode));
}

void CFilDiffView::OnUpdateCmdIndicatorCmd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetOptions().GetOption(OCF_CmdMode));
	pCmdUI->SetText(szCMD);
}

void CFilDiffView::OnUpdateCmdIndicatorDiff(CCmdUI* pCmdUI)
{
	CString strText;
	pCmdUI->Enable(TRUE);

	
  strText.Format(GetResourceString(IDS_Diffxx), ThisDoc->GetActDiff(), ThisDoc->GetDiffNo());
	pCmdUI->SetText(strText);
}

void CFilDiffView::OnUpdateCmdIndicatorLine(CCmdUI* pCmdUI)
{
	int nCurLine = -1;
	CString strText;

	if(GetFocus() == &m_LeftFileWnd)
	{
		if(m_LeftFileWnd.m_pCurLine)
			nCurLine = m_LeftFileWnd.m_pCurLine->iLineNo;
	}
	else if(GetFocus() == &m_RightFileWnd)
	{
		if(m_RightFileWnd.m_pCurLine)
			nCurLine = m_RightFileWnd.m_pCurLine->iLineNo;
	}
	else if(GetFocus() == &m_MergeFileWnd)
	{
		if(m_MergeFileWnd.m_pCurLine)
			nCurLine = m_MergeFileWnd.m_pCurLine->iLineNo;
	}

	if(nCurLine == -1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	pCmdUI->Enable(TRUE);

	strText.Format("Ln %d", nCurLine);
	pCmdUI->SetText(strText);
}

void CFilDiffView::OnEditInsertboth() 
{
	CDiffSectElt *pSect = ThisDoc->GetActualSect();

	if(!pSect) 
  {
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	InsertLeftSect(pSect);
	InsertRightSect(pSect);	
	MarkDiffSect(pSect);
}

void CFilDiffView::OnUpdateEditInsertboth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ThisDoc->GetActualSect() != NULL);
}

void CFilDiffView::OnEditDeletediff() 
{
	CDiffSectElt *pSect = ThisDoc->GetActualSect();

  if(!pSect) 
  {
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}

	if(!pSect->IsCLEmpty()) {
		DeleteLeftSect(pSect);
	}
	if(!pSect->IsCREmpty()) {
		DeleteRightSect(pSect);
	}
	MarkDiffSect(pSect);
	
}

void CFilDiffView::DeleteLeftSect(CDiffSectElt * pSect)
{
  if(!pSect)
		return;

	m_MergeFileWnd.CutMarkedLines(pSect->GetCLStart(),
				                  pSect->GetCLEnd());
	pSect->SetCLEmpty();
}

void CFilDiffView::DeleteRightSect(CDiffSectElt * pSect)
{
	if(!pSect)
		return;

	m_MergeFileWnd.CutMarkedLines(pSect->GetCRStart(),
				                  pSect->GetCREnd());
	pSect->SetCREmpty();

}

void CFilDiffView::OnUpdateViewNextdiff(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ThisDoc->GetDiffNo() > 0);
	
}

void CFilDiffView::OnUpdateViewPrevdiff(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ThisDoc->GetDiffNo() > 0);
	
}

void CFilDiffView::OnEditRestoreLeft() 
{
	CObArray      *pDiffArray  = ThisDoc->GetDiffArray();
	CDiffSectElt  *pSect       = NULL;

	for(int i=0; i<=pDiffArray->GetUpperBound(); i++)
	{
		pSect = (CDiffSectElt *) (*pDiffArray)[i];

	if(!pSect)
		continue;

	if(!pSect->IsLEmpty() && pSect->IsCLEmpty())
		InsertLeftSect(pSect);

	
	if(!pSect->IsCREmpty())
		DeleteRightSect(pSect);

	}
	
	MarkDiffSect(ThisDoc->GetActualSect());
	
}

void CFilDiffView::OnUpdateEditRestoreLeft(CCmdUI* pCmdUI) 
{
	BOOL bEnable = ThisDoc->GetDiffNo();
	pCmdUI->Enable(bEnable);
	
}

void CFilDiffView::OnEditRestoreRight() 
{
	CObArray      *pDiffArray = ThisDoc->GetDiffArray();
	CDiffSectElt  *pSect       = NULL;

	for(int i=0; i<=pDiffArray->GetUpperBound(); i++)
	{
		pSect = (CDiffSectElt *) (*pDiffArray)[i];

		if(!pSect)
			continue;

		if(!pSect->IsREmpty() && pSect->IsCREmpty())
			InsertRightSect(pSect);

	
		if(!pSect->IsCLEmpty())
			DeleteLeftSect(pSect);
	}
	
	MarkDiffSect(ThisDoc->GetActualSect());
}

void CFilDiffView::OnUpdateEditRestoreRight(CCmdUI* pCmdUI) 
{
	BOOL bEnable = ThisDoc->GetDiffNo();
	pCmdUI->Enable(bEnable);

}

#ifdef _DEBUG
//Just for test
LRESULT CFilDiffView::OnDelComplete(WPARAM nCtrlId, LPARAM lParam)
{
	ThisDoc->VerifyDiffArray();
	return 1L;
}
#endif

LRESULT CFilDiffView::OnDelSectMark(WPARAM nCtrlId, LPARAM lParam)
{
	TEXTPOS       Start;
	TEXTPOS       End;
	PLINEDESC     pDelStart;
	PLINEDESC     pDelEnd;

	// Deleting lines will:
	// 1: pLine.Text = pLine->pNext.Text
	// 2. delete pLine->pNext!!
	
	if(m_MergeFileWnd.GetSelection(Start, End))
	{
		//The user deletes selected lines, check if the sections have references
		if(Start.pLine == End.pLine)
			return 1L;
		
		pDelStart = Start.pLine->pNext ? Start.pLine->pNext : Start.pLine;
		ASSERT(End.pLine->pPrev);

		if(End.pLine->pPrev != Start.pLine)
		{
			pDelEnd = End.pLine->pPrev;
			ASSERT(pDelStart->iLineNo <= pDelEnd->iLineNo);
			ThisDoc->OnDeleteLine(pDelStart, pDelEnd);
		}
		ThisDoc->OnJoinLines(&Start, End.pLine);
			
		//m_MergeFileWnd.Get
	
		MarkDiffSect(ThisDoc->GetActualSect());
	}
	return 1L;
}

LRESULT CFilDiffView::OnJoinLines(WPARAM nCtrlId, LPARAM lParam)
{
	TEXTPOS       Start;

	ASSERT((PLINEDESC) lParam == m_MergeFileWnd.m_pCurLine);

	Start.pLine = m_MergeFileWnd.m_pCurLine;
	Start.iOffset = m_MergeFileWnd.GetEdit()->GetRealOffset(m_MergeFileWnd.m_pCurLine, m_MergeFileWnd.m_iColNo);

	if(Start.pLine->pNext)
		ThisDoc->OnJoinLines(&Start, Start.pLine->pNext);
	
	return 1L;
}


LRESULT CFilDiffView::OnSplitLines(WPARAM nCtrlId, LPARAM lParam)
{
	PLINEDESC  pLine = (PLINEDESC) lParam;

	if(pLine)
	{
		ThisDoc->OnSplitLines(pLine);
	}
	
	return 1L;
}

//Refresh the edit controls because the font/tab size was changed
void CFilDiffView::SetFont(LOGFONT& Logfont)
{
	m_RightFileWnd.SetFont(Logfont);
	m_LeftFileWnd.SetFont(Logfont);
	m_MergeFileWnd.SetFont(Logfont);
	m_RightFileWnd.Invalidate();
	m_LeftFileWnd.Invalidate();
	m_MergeFileWnd.Invalidate();
  
}

void CFilDiffView::RefreshEditControls()
{
  m_RightFileWnd.GetEdit()->InitFileTypeSettings();
	m_LeftFileWnd.GetEdit()->InitFileTypeSettings();
	m_MergeFileWnd.GetEdit()->InitFileTypeSettings();

	m_RightFileWnd.Invalidate();
	m_LeftFileWnd.Invalidate();
	m_MergeFileWnd.Invalidate();
}

bool CFilDiffView::ExpandVar(const CString& strVar, CStringArray& strArray)
{
  ECmdVar      eVar;
  if (!CVarMgr::Lookup(strVar, eVar))
  {
    ASSERT(false);
    return false;
  }
  
  return ExpandVar(eVar, strArray);
}

bool CFilDiffView::ExpandVar(ECmdVar eVar, CStringArray& strArray)
{
	CFilDiffDoc *doc = ThisDoc;

  switch (eVar)
  {
    case var_LFilePath:
    case var_LFileName:
    case var_LFileDir:
      strArray.SetSize(1);
      strArray[0] = doc->GetLeftFileName();

      if (eVar == var_LFileName)
        GetFileName(strArray[0]);
      if (eVar == var_LFileDir)
        GetFileDir(strArray[0]);

      return true;

    case var_RFilePath:
    case var_RFileName:
    case var_RFileDir:
      strArray.SetSize(1);
      strArray[0] = doc->GetRightFileName();

      if (eVar == var_RFileName)
        GetFileName(strArray[0]);
      if (eVar == var_RFileDir)
        GetFileDir(strArray[0]);
      return true;

    case var_ComposedFilePath:
    case var_ComposedFileName:
    case var_ComposedFileDir:
      strArray.SetSize(1);
      if(!GetMergeFileName(strArray[0]))
        return false;

      if (eVar == var_ComposedFileName)
        GetFileName(strArray[0]);
      if (eVar == var_ComposedFileDir)
        GetFileDir(strArray[0]);
      
      return true;

    case var_CurFilePath:
    case var_CurFileName:
    case var_CurFileDir:
      {
        ECmdVar eNewVar;
        if (GetFocus() == &m_MergeFileWnd)
          eNewVar = (ECmdVar) (var_ComposedFilePath + eVar - var_CurFilePath);
        else if (GetFocus() == &m_LeftFileWnd)
          eNewVar = (ECmdVar) (var_LFilePath + eVar - var_CurFilePath);
        else if (GetFocus() == &m_RightFileWnd)
          eNewVar = (ECmdVar) (var_RFilePath + eVar - var_CurFilePath);
        else
          return false;

        return ExpandVar(eNewVar, strArray);
      }
      break;

    case var_CurLine:
      {
        CWnd *pWnd = GetFocus();
        CDiffEditCtrl *pEdit = NULL;
        if (pWnd == &m_LeftFileWnd || pWnd == &m_RightFileWnd || pWnd == &m_MergeFileWnd)
        {
          int nCurLine = -1;
          pEdit = (CDiffEditCtrl *) pWnd;
          if(pEdit->m_pCurLine)
      			nCurLine = pEdit->m_pCurLine->iLineNo;
          if (nCurLine > 0)
          {
            strArray.SetSize(1);
            strArray[0].Format("%d", nCurLine);
            return true;
          }
        }
      }
      break;


  }

  return false;
}

bool CFilDiffView::GetMergeFileName(CString& strFileName)
{
  if (ThisDoc->GetMergeFileName().IsEmpty())
  {
    AfxMessageBox("The composed file is not saved yet. Please save the file before executing the command!");
    return false;
  }
  strFileName = ThisDoc->GetMergeFileName();
  return true;
}

bool CFilDiffView::CanExpandVar(const CString& strVar)
{
  ECmdVar      eVar;
  if (!CVarMgr::Lookup(strVar, eVar))
  {
    ASSERT(false);
    return false;
  }

  switch (eVar)
  {
    case var_LFilePath:
    case var_RFilePath:
    case var_CurFilePath:
    case var_ComposedFilePath:
    case var_LFileName:
    case var_RFileName:
    case var_CurFileName:
    case var_ComposedFileName:
    case var_LFileDir:
    case var_RFileDir:
    case var_CurFileDir:
    case var_ComposedFileDir:
    case var_LDir:
    case var_RDir:
      return true;
    case var_CurLine:
      {
        CWnd *pWnd = GetFocus();
        if (pWnd == &m_LeftFileWnd || pWnd == &m_RightFileWnd || pWnd == &m_MergeFileWnd)
          return true;
      }
      return false;
  }

  return false;
}


BOOL CFilDiffView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	CWnd *pWnd = GetFocus();

  if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CDiffEditCtrl)))
  {
    if (pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
      return TRUE;
  }
	
	return CSmxPrnFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CFilDiffView::SetStatusText(UINT nStrID, bool bSetTimer)
{
  AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, nStrID);
  if (bSetTimer)
  {
    m_nStatusTimer = SetTimer(ID_StatusTimer, 2000, NULL);
  }
}

void CFilDiffView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nStatusTimer)
  {
    AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
    KillTimer(nIDEvent);
    m_nStatusTimer = -1;
  }
	
	CView::OnTimer(nIDEvent);
}


void CFilDiffView::StoreSplitRatio()
{
  CRect rcClient;
  CRect rcLeft;
  CRect rcRight;
  CRect rcMerge;
  int nHSplit = 0;
  int nVSplit = 0;

  if (!m_bVLayoutChanged && !m_bHLayoutChanged)
    return;

  if (m_RightFileWnd.m_hWnd == NULL)
    return;

  GetClientRect(rcClient);
  m_LeftFileWnd.GetWindowRect(rcLeft);
  m_RightFileWnd.GetWindowRect(rcRight);
  m_MergeFileWnd.GetWindowRect(rcMerge);


  if (m_bHLayoutChanged && rcLeft.Width() > 0 && rcRight.Width())
  {
    nHSplit = (int) (rcLeft.Width() * MYPERCENT / (rcClient.Width() - 2 * DIM_MRG - DIM_SPLIT) + 0.5);

    if (nHSplit)
    {
      AfxGetApp()->WriteProfileInt(GetOptions().GetSection(), szHSplit, nHSplit);
      m_bHLayoutChanged = false;
    }
  }

  if (m_bVLayoutChanged && rcMerge.Height() > 0)
  {
    if (rcLeft.Height() > 0)
      nVSplit = (int) (rcLeft.Height() * MYPERCENT / (rcClient.Height() - 2 * DIM_MRG - DIM_SPLIT) + 0.5);
    else if (rcRight.Height() > 0)
      nVSplit = (int) (rcRight.Height() * MYPERCENT / (rcClient.Height() - 2 * DIM_MRG - DIM_SPLIT) + 0.5);

    if (nVSplit)
    {
      AfxGetApp()->WriteProfileInt(GetOptions().GetSection(), szVSplit, nVSplit);
      m_bVLayoutChanged = false;
    }
  }
}

void CFilDiffView::OnLScrollChanged()
{
	SyncLLine(m_LeftFileWnd.m_pTopLine);
}

void CFilDiffView::SyncLLine(PLINEDESC pLine)
{
  CRect rcMerge;
  CRect rcWnd;
  CFilDiffDoc *pDoc = (CFilDiffDoc *) GetDocument();
	ASSERT(pDoc);

	PLINEDESC pTopRight, pTopMerge, pDummy;
  int nLines = pLine->iLineNo - m_LeftFileWnd.m_pTopLine->iLineNo;

  m_MergeFileWnd.GetClientRect(rcMerge);
  m_LeftFileWnd.GetClientRect(rcWnd);

  // synchronize the right window
	ThisDoc->GetLinkedLines(pDummy, pTopRight, pTopMerge, m_LeftFileWnd.GetEdit(), pLine);
  //pTopRight = line_getlink(pLine);

  if (!pTopRight)
  {
    CDiffSectElt *pSect = GetFilDiffDoc()->GetNextDiffFromPos(m_LeftFileWnd.GetEdit(), pLine, false);
    if (pSect)
    {
      if (pSect->GetRStart() && pSect->GetREnd())
      {
        pTopRight = pSect->GetRStart();
        if (pSect->GetLStart())
          pTopRight = MoveDown(pTopRight, pLine->iLineNo - pSect->GetLStart()->iLineNo);
      }
      else if (pSect->GetRStart())
      {
        if (pSect->GetLStart())
          nLines = pSect->GetLStart()->iLineNo - m_LeftFileWnd.m_pTopLine->iLineNo;
        pTopRight = pSect->GetRStart();
      }
      else
      {
        if (pSect->GetLStart())
          nLines = pSect->GetLStart()->iLineNo - m_LeftFileWnd.m_pTopLine->iLineNo;
        pTopRight = pSect->GetREnd();
      }
    }
  }
  pTopRight = MoveUp(pTopRight, nLines);

	if (pTopRight)
		m_RightFileWnd.SyncLine(pTopRight);

	// synchronize the merge window
	//pTopMerge = pDoc->SyncMergeWithLeft(pLine);
  pTopMerge = MoveUp(pTopMerge, max(nLines - (rcWnd.Height() - rcMerge.Height()) / m_MergeFileWnd.m_iLineHeight, 0));

	if (pTopMerge)
  {
    m_MergeFileWnd.SyncLine(pTopMerge);
  }
}

void CFilDiffView::OnRScrollChanged()
{
	SyncRLine(m_RightFileWnd.m_pTopLine);
}

void CFilDiffView::SyncRLine(PLINEDESC pLine)
{
  CRect rcMerge;
  CRect rcWnd;
	CFilDiffDoc *pDoc = (CFilDiffDoc *) GetDocument();
	ASSERT(pDoc);

	PLINEDESC pTopLeft  = NULL;
  PLINEDESC pTopMerge = NULL;
  PLINEDESC pDummy    = NULL;

  int nLines = pLine->iLineNo - m_RightFileWnd.m_pTopLine->iLineNo;

  m_MergeFileWnd.GetClientRect(rcMerge);
  m_RightFileWnd.GetClientRect(rcWnd);

	ThisDoc->GetLinkedLines(pTopLeft, pDummy, pTopMerge, m_RightFileWnd.GetEdit(), pLine);

  // synchronize the left window
  //pTopLeft = line_getlink(pLine);
  if (!pTopLeft)
  {
    CDiffSectElt *pSect = GetFilDiffDoc()->GetNextDiffFromPos(m_RightFileWnd.GetEdit(), pLine, false);
    if (pSect)
    {
      if (pSect->GetLStart() && pSect->GetLEnd())
      {
        pTopLeft = pSect->GetLStart();
        if (pSect->GetRStart())
          pTopLeft = MoveDown(pTopLeft, pLine->iLineNo - pSect->GetRStart()->iLineNo);
      }
      else if (pSect->GetLStart())
      {
        //if (pSect->GetRStart())
          //nLines = pSect->GetRStart()->iLineNo - m_LeftFileWnd.m_pTopLine->iLineNo;
        pTopLeft = pSect->GetLStart();
      }
      else
      {
        //if (pSect->GetRStart())
          //nLines = pSect->GetRStart()->iLineNo - m_LeftFileWnd.m_pTopLine->iLineNo;
        pTopLeft = pSect->GetLEnd();
      }
    }
  }
  pTopLeft = MoveUp(pTopLeft, nLines);

  if (pTopLeft)
	  m_LeftFileWnd.SyncLine(pTopLeft);

  // synchronize the merge window
  //pTopMerge = pDoc->SyncMergeWithRight(m_RightFileWnd.m_pTopLine);
  if (nLines > rcMerge.Height() / m_MergeFileWnd.m_iLineHeight)
    pTopMerge = MoveUp(pTopMerge, max(nLines - (rcWnd.Height() - rcMerge.Height()) / m_MergeFileWnd.m_iLineHeight, 0));

  if (pTopMerge)
	  m_MergeFileWnd.SyncLine(pTopMerge);
}

void CFilDiffView::OnMScrollChanged()
{
	SyncMLine(m_MergeFileWnd.m_pTopLine);
}

void CFilDiffView::SyncMLine(PLINEDESC pLine)
{
  CRect rcMerge;
  CRect rcWnd;
	CFilDiffDoc *pDoc = (CFilDiffDoc *) GetDocument();
	ASSERT(pDoc);

	PLINEDESC pTopLeft  = NULL;
  PLINEDESC pTopRight = NULL;
  PLINEDESC pDummy    = NULL;
  int nLines = pLine->iLineNo - m_MergeFileWnd.m_pTopLine->iLineNo;

  m_MergeFileWnd.GetClientRect(rcMerge);
	ThisDoc->GetLinkedLines(pTopLeft, pTopRight, pDummy, m_MergeFileWnd.GetEdit(), pLine);

  // synchronize the left window
  //pTopLeft = pDoc->SyncLeftWithMerge(pLine);
  m_LeftFileWnd.GetClientRect(rcWnd);
  if (nLines > rcWnd.Height() / m_LeftFileWnd.m_iLineHeight)
    pTopLeft = MoveUp(pTopLeft, max(nLines - (rcMerge.Height() - rcWnd.Height()) / m_MergeFileWnd.m_iLineHeight, 0));

  if (pTopLeft)
	  m_LeftFileWnd.SyncLine(pTopLeft);


  // synchronize the right window
  m_RightFileWnd.GetClientRect(rcWnd);
  //pTopRight = pDoc->SyncRightWithMerge(pLine);
  if (nLines > rcWnd.Height() / m_RightFileWnd.m_iLineHeight)
    pTopRight = MoveUp(pTopRight, max(nLines - (rcMerge.Height() - rcWnd.Height()) / m_MergeFileWnd.m_iLineHeight, 0));

  if (pTopRight)
	  m_RightFileWnd.SyncLine(pTopRight);
}

void CFilDiffView::OnPageSetup()
{
	CSmxPgSetupDlg dlg(0, this);
  dlg.m_eHeader = SWC_hidden;
  dlg.m_eFooter = SWC_hidden;
  dlg.m_strRegKey = GetOptions().GetSectionPath(szFilDiffPgSetup);

  AfxGetApp()->DoPrintDialog(&dlg);
}

void CFilDiffView::SetPageOrientation(bool bLandscape)
{
  ((CFCUApp *) AfxGetApp())->SetPageOrientation(bLandscape);
}

LPCTSTR CFilDiffView::GetPageSetupSection()
{
  return GetOptions().GetSectionPath(szFilDiffPgSetup);  
}

void CFilDiffView::OnViewRefresh() 
{
  MarkDiffSect(NULL, false);

  ((CFilDiffDoc *) GetDocument())->Refresh();


  m_LeftFileWnd.GetEdit()->SetReadOnly(TRUE);
	m_LeftFileWnd.InitView();

	m_RightFileWnd.GetEdit()->SetReadOnly(TRUE);
	m_RightFileWnd.InitView();

	m_MergeFileWnd.InitView();

  MarkDiffSect(ThisDoc->GetActualSect());
	m_LeftFileWnd.Invalidate();
	m_RightFileWnd.Invalidate();
	m_MergeFileWnd.Invalidate();
  Invalidate();

}

void CFilDiffView::Draw()
{
  int nY = m_nY;
  int nX = m_nX;
  int nStartY = m_nY;
  
  LOGFONT logfont;
  GetOptions().GetPrnFont(VIEW_FilDiff, &logfont, m_pDC);
  CSmxPrnFormView::SetFont(&logfont);

  COLORREF crLeft = GetSysColor(COLOR_WINDOWTEXT);
  COLORREF crRight = GetSysColor(COLOR_WINDOWTEXT);

  if (GetOptions().GetOption(OCF_PrintWithColors))
  {
    crLeft  = GetOptions().GetLeftPrnColor(VIEW_FilDiff);
    crRight = GetOptions().GetRightPrnColor(VIEW_FilDiff);
  }

  CRect rcNumber;
  int nNofLines = m_MergeFileWnd.CountLines();
  int nDigits = 1;
  while (nNofLines >= 1)
  {
    nNofLines /= 10;
    nDigits++;
  }

  CalcTextRect(rcNumber, "0", DT_SINGLELINE); 
  int nNumberWidth = nDigits * rcNumber.Width();
  

  if (m_bCalcNofPages || m_nCurPage == 1)
  {
    PrintText("FCU File Compare  ");
    PrintCurrentDate();
    PrintEOL();
    PrintEOL();


    PrintText("Left file:");
    nY = m_nY;
    nX = m_nX;
    PrintEOL();
    PrintText("Right file:");
    nX = max(m_nX, nX);
    nX += 200;
    MoveTo(nX, nY);
    PrintText(ThisDoc->GetLeftFileName());
    PrintEOL();
    MoveTo(nX, m_nY);
    PrintText(ThisDoc->GetRightFileName());
    PrintEOL();
    PrintEOL();
  }

  int nLineHeight = CalcPrintRectHeight(1);

  PLINEDESC pLine = m_MergeFileWnd.GetEdit()->m_pLine;
  CObArray* pArray = ThisDoc->GetDiffArray();
  CDiffSectElt *pSect = NULL;
  int nSectIdx = 0;
 
  if (m_bCalcNofPages)
  {
    m_nNofPages = 1;
    m_aFirstLineInPage.RemoveAll();
    m_aFirstLineInPage.Add(pLine);
  }
  else
  {
    pLine = (PLINEDESC) m_aFirstLineInPage[m_nCurPage - 1];
  }

  nSectIdx = ThisDoc->GetNextDiffIdxFromPos(m_MergeFileWnd.GetEdit(), pLine);
  if (nSectIdx == -1)
    nSectIdx = pArray->GetUpperBound();

  if (pLine->state == STATE_LEFTONLY || pLine->state == STATE_MOVEDLEFT)
  {
    SetTextColor(crLeft);
  }
  else if (pLine->state == STATE_RIGHTONLY || pLine->state == STATE_MOVEDRIGHT)
  {
    SetTextColor(crRight);
  }

  while(pLine)
  {
    pSect = (CDiffSectElt *) pArray->GetAt(nSectIdx);

    ASSERT(pSect->GetCLEnd() || pSect->GetCREnd());
    if (!pSect)
    {
      ASSERT(false);
      continue;
    }

    if (pLine == pSect->GetCLStart())
    {
      SetTextColor(crLeft);
      PrintText(">>>>>>");
      PrintLine(m_nX + 200, m_nY + nLineHeight / 2, GetRightPrintPos(), m_nY + nLineHeight / 2);
      PrintEOL();
    }
    else if (pLine == pSect->GetCRStart())
    {
      SetTextColor(crRight);
      PrintText("<<<<<<");
      PrintLine(m_nX + 200, m_nY + nLineHeight / 2, GetRightPrintPos(), m_nY + nLineHeight / 2);
      PrintEOL();
    }
    
    MoveHSpace(300);
    nX = m_nX;
    PrintValue(GetDisplayLineNo(&m_MergeFileWnd, pLine, NULL), DT_LEFT | DT_SINGLELINE, 0, 0);
    MoveTo(nX + nNumberWidth, m_nY);
    
    TCHAR *pText     = NULL;
    int    iTxtSize = 0;
    if (memchr(pLine->pText,9,pLine->iTxtSize))
    {
      pText = m_MergeFileWnd.GetEdit()->ExpandTabs(pLine->pText, pLine->iTxtSize, 0, 1, &iTxtSize);
    }
    else
    {
      pText    = pLine->pText;
      iTxtSize = pLine->iTxtSize;
    }

    //remove the <CL><LF> characters
    TCHAR *pChar = pText;
    TCHAR *pLast = NULL;
    TCHAR  cSave;
    if (iTxtSize > 0)
      pChar = &pText[iTxtSize - 1];
    
    while (pChar != pText)
    {
      if (*pChar != 0x0a && *pChar != 0x0d)
        break;
      pLast = pChar;
      pChar--;
    }

    if (pChar && (*pChar == 0x0a || *pChar == 0x0d))
      pLast = pChar;

    if (pLast) 
    {
      cSave = *pLast;
      *pLast = NULL;
    }

    int nLeft = m_rcPrint.left;
    m_rcPrint.left = m_nX;
    
    //Print the text
    if (pText && *pText)
      PrintText(pText, DT_LEFT | DT_WORDBREAK);
    else
      PrintEOL();

    m_rcPrint.left = nLeft;
    m_nX = nLeft;

    //Restore <CL><LF>
    if (pLast)
      *pLast = cSave;

    if (pText != pLine->pText)
      delete [] pText;

    //PrintEOL();

    if (pLine == pSect->GetCLEnd())
    {
      PrintText(">>>>>>");
      PrintLine(m_nX + 200, m_nY + nLineHeight / 2, GetRightPrintPos(), m_nY + nLineHeight / 2);
      PrintEOL();
      SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
    }
    else if (pLine == pSect->GetCREnd())
    {
      PrintText("<<<<<<");
      PrintLine(m_nX + 200, m_nY + nLineHeight / 2, GetRightPrintPos(), m_nY + nLineHeight / 2);
      PrintEOL();
      SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
    }

    if (pSect->GetCEnd()->iLineNo <= pLine->iLineNo && nSectIdx +1 < pArray->GetSize())
    {
      nSectIdx++;
    }

    pLine = pLine->pNext;

    if (m_bCalcNofPages)
    {
      if (m_nY > int (m_rcClient.bottom - 4 * nLineHeight))
      {
        if (pLine)
        {
          m_nNofPages++;
          m_aFirstLineInPage.Add(pLine);
          m_nY = nStartY;
        }
      }
    }
    else
    {
      if(!m_bCalcNofPages && m_nCurPage != m_nNofPages && pLine == m_aFirstLineInPage[m_nCurPage])
        break;
    }
  }

  PrintEOL();
  CString str;
  COLORREF crText = SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
  str.Format(_T("Page %d/%d"), m_nCurPage, m_nNofPages);
  MoveTo(m_nX, GetBottomPrintPos() - nLineHeight);
  PrintText(str, DT_RIGHT);
  SetTextColor(crText);
}

void CFilDiffView::OnNextDiffFromCursor()
{
  CDiffEditCtrl *pEdit = (CDiffEditCtrl *) GetFocus();
  if (pEdit == &m_LeftFileWnd || pEdit == &m_RightFileWnd || pEdit == &m_MergeFileWnd)
  {
	  TEXTPOS       Start;
	  TEXTPOS       End;
	  if(pEdit->GetSelection(Start, End) && End.pLine)
	  {
      CDiffSectElt *pSect = GetFilDiffDoc()->GetNextDiffFromPos(pEdit->GetEdit(), End.pLine, true);

	    if(pSect)
      {
      	if (End.pLine->state == STATE_LEFTONLY || End.pLine->state == STATE_MOVEDLEFT ||
            End.pLine->state == STATE_RIGHTONLY || End.pLine->state == STATE_MOVEDRIGHT)
        {
          MarkDiffSect(pSect, false);
        }
        else
        {
          MarkDiffSect(NULL, false);
        }
        
        if (pEdit == &m_LeftFileWnd)
          SyncLLine(End.pLine);
        else if (pEdit == &m_RightFileWnd)
          SyncRLine(End.pLine);
        else
          SyncMLine(End.pLine);
      }
    }
  }
}

PLINEDESC CFilDiffView::MoveUp(PLINEDESC pLine, int nLines)
{
  if (!pLine)
    return NULL;

  for (int i = 0; i < nLines; i++)
  {
    if (pLine->pPrev)
      pLine = pLine->pPrev;
  }
  return pLine;
}

PLINEDESC CFilDiffView::MoveDown(PLINEDESC pLine, int nLines)
{
  if (!pLine)
    return NULL;

  for (int i = 0; i < nLines; i++)
  {
    if (pLine->pNext)
      pLine = pLine->pNext;
  }
  return pLine;
}

int CFilDiffView::GetDisplayLineNo(CDiffEditCtrl *pEdit, PLINEDESC pLine, CDC *pDC)
{
  if (pEdit != &m_MergeFileWnd || !GetFilDiffDoc()->GetMergeFileName().IsEmpty())
    return pLine->iDisplayLineNo;

  
  if (pLine->state == STATE_LEFTONLY || pLine->state == STATE_MOVEDLEFT)
  {
    if (pDC && !GetOptions().GetOption(OCF_InsLeftIdentSect))
    {
      pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
    }
    if (pLine->link && pLine->link != pLine)
      return pLine->link->iDisplayLineNo;
  }
  else if (pLine->state == STATE_RIGHTONLY || pLine->state == STATE_MOVEDRIGHT)
  {
    if (pDC && GetOptions().GetOption(OCF_InsLeftIdentSect))
    {
      pDC->SetTextColor(GetSysColor(COLOR_3DSHADOW));
    }

    if (pLine->link && pLine->link != pLine)
      return pLine->link->iDisplayLineNo;
  }
  else
  {
    if (GetOptions().GetOption(OCF_InsLeftIdentSect))
    {
      if (pLine->link && pLine->link != pLine)
        return pLine->link->iDisplayLineNo;
    }
    else
    {
      if (pLine->link && pLine->link != pLine && pLine->link->link && 
          pLine->link->link != pLine)
      {
        return pLine->link->link->iDisplayLineNo;
      }
    }
  }
  return -1;
}


void CFilDiffView::OnFileXmlExport() 
{
  GetFilDiffDoc()->XmlExport();
}

void CFilDiffView::OnUpdateViewLineDiff(CCmdUI* pCmdUI)
{
	CDiffSectElt *pSect = ThisDoc->GetActualSect();
  bool bEnable = false;
  if (pSect && pSect->GetCLStart() && pSect->GetCLStart() == pSect->GetCLEnd() && 
      pSect->GetCRStart() && pSect->GetCRStart() == pSect->GetCREnd())
  {
    bEnable = true;
  }

  pCmdUI->Enable(bEnable);

  if (!bEnable)
    return;

  pCmdUI->SetCheck(m_MergeFileWnd.m_nLDiffLineNo != -1);
}

void CFilDiffView::OnViewLineDiff()
{
  CDiffSectElt *pSect = ThisDoc->GetActualSect();

  if (m_MergeFileWnd.m_nLDiffLineNo == -1)
  {
    ThisDoc->CompareLines(pSect->GetCLStart(), pSect->GetCRStart(), &m_MergeFileWnd.m_aLDiffLineSect, 
                       &m_MergeFileWnd.m_aRDiffLineSect);

    m_MergeFileWnd.m_nLDiffLineNo = pSect->GetCLStart()->iLineNo;
    m_MergeFileWnd.m_nRDiffLineNo = pSect->GetCRStart()->iLineNo;

    PLINEDESC pLine = NULL;
    int nCol = 0;
    if (m_MergeFileWnd.m_aLDiffLineSect.GetSize() > 0)
    {
      pLine = pSect->GetCLStart();
      nCol  = m_MergeFileWnd.m_aLDiffLineSect.GetAt(0);
    }
    else if (m_MergeFileWnd.m_aRDiffLineSect.GetSize() > 0)
    {
      pLine = pSect->GetCRStart();
      nCol  = m_MergeFileWnd.m_aRDiffLineSect.GetAt(0);
    }

    if (pLine)
    {
      m_MergeFileWnd.m_pCurLine = pLine;
      m_MergeFileWnd.m_iColNo = nCol;
      m_MergeFileWnd.MakeCursorVisible();
    }
  }
  else
  {
    m_MergeFileWnd.m_nLDiffLineNo = -1;
    m_MergeFileWnd.m_nRDiffLineNo = -1;
  }
  
  m_MergeFileWnd.InvalidateLineRect(pSect->GetCLStart());
  m_MergeFileWnd.InvalidateLineRect(pSect->GetCRStart());
}

void CFilDiffView::UpdatePopupMenu(CDiffEditCtrl*pEdit, CMenu *pMenu)
{
  if (pMenu)
    GetUserToolMgr().UpdateUserToolMenu(pMenu);

  if (pEdit == &m_LeftFileWnd || pEdit == &m_RightFileWnd)
  {
    pMenu->AppendMenu(MF_SEPARATOR, 0);
    pMenu->AppendMenu(MF_STRING, ID_PROPERTIES, GetResourceString(IDS_Properties));
  }
}

void CFilDiffView::OnProperties()
{
  CStringArray strArray;
  if (ExpandVar(var_CurFilePath, strArray))
    ExecuteContextCommand(szProperties, strArray, GetSafeHwnd());
}
void CFilDiffView::OnOptionsFoldlines() 
{
	ToggleOption(OCF_FoldLines);
  GetOptions().StoreNofUnfoldedLines();
  OnViewRefresh();

  m_LeftFileWnd.Invalidate();
  m_RightFileWnd.Invalidate();
  m_MergeFileWnd.Invalidate();
}

void CFilDiffView::OnOptionsShowlineno() 
{
	ToggleOption(OCF_ShowLineNo);
  GetOptions().StoreEditOptions();

  m_RightFileWnd.GetEdit()->InitFileTypeSettings();
	m_LeftFileWnd.GetEdit()->InitFileTypeSettings();
	m_MergeFileWnd.GetEdit()->InitFileTypeSettings();

  m_RightFileWnd.InitView();
	m_LeftFileWnd.InitView();
	m_MergeFileWnd.InitView();

  m_LeftFileWnd.Invalidate();
  m_RightFileWnd.Invalidate();
  m_MergeFileWnd.Invalidate();
}

void CFilDiffView::OnUpdateOptionsFoldlines(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetOptions().GetOption(OCF_FoldLines));
	
}

void CFilDiffView::OnUpdateOptionsShowlineno(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetOptions().GetOption(OCF_ShowLineNo));
}

void CFilDiffView::OnOptionsSyncscrolling() 
{
	ToggleOption(OCF_SyncScrolling);

  bool bVSyncScrolling = GetOptions().GetOption(OCF_SyncScrolling);
  m_LeftFileWnd.m_bSendVScrollChanged =  bVSyncScrolling;
	m_RightFileWnd.m_bSendVScrollChanged = bVSyncScrolling;
	m_MergeFileWnd.m_bSendVScrollChanged = bVSyncScrolling;
}

void CFilDiffView::OnUpdateOptionsSyncscrolling(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GetOptions().GetOption(OCF_SyncScrolling));
}
