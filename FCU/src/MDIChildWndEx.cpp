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
#include "MDIChildWndEx.h"

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

IMPLEMENT_DYNCREATE(CMDIChildWndEx, CMDIChildWnd)

CMDIChildWndEx::CMDIChildWndEx()
{
}

CMDIChildWndEx::~CMDIChildWndEx()
{
}


BEGIN_MESSAGE_MAP(CMDIChildWndEx, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMDIChildWndEx)
  ON_WM_MDIACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMDIChildWndEx::OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd )
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

CToolBarEx *CMDIChildWndEx::GetToolBar()
{
  return ((CMainFrame *)AfxGetMainWnd())->GetToolBar();
}

LPCTSTR CMDIChildWndEx::GetToolbarRegKeyName()
{
  return szDefaultToolBar;
}

void CMDIChildWndEx::InitializeToolBar(bool bDefault)
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
      bHidden = CMDIChildWndEx::IsButtonInitialyHidden(nCmdID);
    else
      bHidden = IsButtonInitialyHidden(nCmdID);

    if (bHidden)
    {
      pToolBar->GetToolBarCtrl().DeleteButton(pToolBar->GetToolBarCtrl().CommandToIndex(nCmdID));
      i--;
    }
  }
}

bool CMDIChildWndEx::IsButtonInitialyHidden(UINT nCmdID)
{
  return ((CMainFrame *)AfxGetMainWnd())->IsButtonInitialyHidden(nCmdID);
}

void CMDIChildWndEx::ResizeToolBar()
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
