/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: MDIChildWndEx.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/MDIChildWndEx.cpp,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "MainFrm.h"
#include "ToolBarEx.h"
#include "FcuMDIChildWndEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  LPCTSTR szDefaultToolBar = _T("ToolBarEx");
}

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWndEx

IMPLEMENT_DYNCREATE(CFcuMDIChildWndEx, CMDIChildWnd)

CFcuMDIChildWndEx::CFcuMDIChildWndEx()
{
}

CFcuMDIChildWndEx::~CFcuMDIChildWndEx()
{
}


BEGIN_MESSAGE_MAP(CFcuMDIChildWndEx, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMDIChildWndEx)
  ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFcuMDIChildWndEx::OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd )
{
  CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

  if (bActivate)
  {
    //Activate the tool bar for this MDI window
    if (!GetToolBar()->RestoreState(GetToolbarRegKeyName()))
    {
      InitializeToolBar(false);
    }
    ResizeToolBar();
  }
  else if (!bActivate && pActivateWnd == NULL && AfxGetMainWnd()->IsWindowVisible())
  {
    //The last MDI child window was closed, activate the default tool bar
    if (!GetToolBar()->RestoreState(szDefaultToolBar))
    {
      InitializeToolBar(true);
    }
    ResizeToolBar();
  }
}

CToolBarEx *CFcuMDIChildWndEx::GetToolBar()
{
  return ((CMainFrame *)AfxGetMainWnd())->GetToolBar();
}

LPCTSTR CFcuMDIChildWndEx::GetToolbarRegKeyName()
{
  return szDefaultToolBar;
}

void CFcuMDIChildWndEx::InitializeToolBar(bool bDefault)
{
  UINT nCmdID = -1;
  CToolBarEx  *pToolBar = GetToolBar();

  pToolBar->LoadToolBar(IDR_MAINFRAME);
	pToolBar->SetToolBarInfoForCustomization();
  for (int i=0;i<pToolBar->GetToolBarCtrl().GetButtonCount();i++)
  {
    nCmdID = pToolBar->GetItemID(i);
    bool bHidden = false;
    if (bDefault)
      bHidden = CFcuMDIChildWndEx::IsButtonInitialyHidden(nCmdID);
    else
      bHidden = IsButtonInitialyHidden(nCmdID);

    if (bHidden)
    {
      pToolBar->GetToolBarCtrl().DeleteButton(pToolBar->GetToolBarCtrl().CommandToIndex(nCmdID));
      i--;
    }
  }
}

bool CFcuMDIChildWndEx::IsButtonInitialyHidden(UINT nCmdID)
{
  return ((CMainFrame *)AfxGetMainWnd())->IsButtonInitialyHidden(nCmdID);
}

void CFcuMDIChildWndEx::ResizeToolBar()
{
  CToolBarEx  *pToolBar = GetToolBar();

  //Don't know how to resize the tool bar, simulate a click on the topleft edge!!
  CRect rcToolBar;
  pToolBar->GetWindowRect(rcToolBar);
  pToolBar->ScreenToClient(rcToolBar);
  CPoint pt = rcToolBar.TopLeft();
  pToolBar->PostMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
  pToolBar->PostMessage(WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
}
