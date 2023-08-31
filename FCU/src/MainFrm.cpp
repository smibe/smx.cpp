/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: MainFrm.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/MainFrm.cpp,v 1.3 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// MainFrm.cpp : Implementation of class CMainFrame
//
/////////////////////////////////////////////////////////////////////////////
// 060698:GeH created.

#include "stdafx.h"
#include "FCU.h"
#include "FCUOptions.h"
#include "SInstance.h"
#include "SingleViewTemplate.h"
#include "OXGridList.h"
#include "OXDirDlg.h"
#include "SMXRes.h"
#include "SMXSheet.h"
#include "ComparePg.h"
#include "ColorPg.h"
#include "FileDropTarget.h"

#include "MainFrm.h"
#include "diffDirDoc.h"
#include "diffDirView.h"
#include "FilDiffView.h"
#include "FilDiffFrm.h"
#include "FilDiffDoc.h"
#include "FilDiffParam.h"
#include "DisplayPg.h"
#include "AccelMapDlg.h"
#include "SmxUserToolsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CFCUApp theApp;

#define ID_ReadyTimer 2001

LPCTSTR szCode = _T("2541");

SMXTreeDescription TreeDescription[] = 
{
  1, "Compare Directories", IDD_DIRCMP_OPTIONS,
  2, "Printing",            IDD_DIFFDIR_PRINT_OPTIONS,
  1, "Compare Files",       IDD_FILCMP_OPTIONS,
  2, "Editor",              IDD_EDITOR_OPTIONS,
  2, "Printing",            IDD_FILDIFF_PRINT_OPTIONS,
  1, "Colors",              IDD_COLORS,
  1, "Comparison",          IDD_COMPARE,
  1, "Command line",        IDD_CMDLINE_OPTIONS,
  0, NULL, 0,
};


CMenu CMainFrame::menu;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, MainFrameParent)

BEGIN_MESSAGE_MAP(CMainFrame, MainFrameParent)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_CMP_DIR, OnCmpDir)
	ON_COMMAND(ID_TOOLS_CUSTOMIZE_TOOLBAR, OnCustomizeToolBar)
	ON_COMMAND(ID_TOOLS_CUSTOMIZE_KEYBOARD,OnCustomizeKeyboard)
	ON_COMMAND(ID_TOOLS_CUSTOMIZE_USERTOOLS, OnCustomizeUserTools)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CMP_FILE, OnCmpFile)
	ON_UPDATE_COMMAND_UI(ID_CMP_FILE, OnUpdateCmpFile)
	ON_COMMAND(ID_WINDOWS_NEXT, OnWindowsNext)
	ON_UPDATE_COMMAND_UI(ID_WINDOWS_NEXT, OnUpdateWindowsNext)
	ON_WM_ACTIVATEAPP()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DIFF, OnUpdateCmdIndicator)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINE, OnUpdateCmdIndicator)
	ON_WM_TIMER()
  ON_MESSAGE(WMU_CMPFILE, OnWMUCmpFile)
	ON_COMMAND(ID_SMX_OPTIONS, OnSmxOptions)
	//}}AFX_MSG_MAP
  //ON_NOTIFY_RANGE(TBN_QUERYINSERT, 1, INT_MAX, OnQueryInsert)
  //ON_NOTIFY_RANGE(TBN_QUERYDELETE, 1, INT_MAX, OnQueryDelete)
  //ON_NOTIFY_RANGE(TBN_GETBUTTONINFO, 1, INT_MAX, OnGetButtonInfo)
  ON_COMMAND_RANGE(ID_USER_TOOLS_FIRST, ID_USER_TOOLS_LAST, OnUserTool)
  ON_UPDATE_COMMAND_UI_RANGE(ID_USER_TOOLS_FIRST, ID_USER_TOOLS_LAST, OnUpdateUserTool)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_DIFF,
	ID_INDICATOR_LINE,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Message-Handler

CMainFrame::CMainFrame()
{
  m_nReadyTimer       = -1;
  m_nShownOptionDlgID = 0;
  m_pFileDropTarget   = NULL;
  m_pCmpDirTemplate   = NULL;
  m_pCmpFilTemplate   = NULL;
}

CMainFrame::~CMainFrame()
{
  delete m_pFileDropTarget;

  CFilDiffFrm::DeleteAccelMgr();
  CDiffDirFrame::DeleteAccelMgr();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // Fehler beim Erzeugen
	}

  
  m_pCmpFilTemplate = new CSingleViewTemplate(
				                         IDR_DIFF_FIL,
				                         RUNTIME_CLASS(CFilDiffDoc),
				                         RUNTIME_CLASS(CFilDiffFrm),
				                         RUNTIME_CLASS(CFilDiffView));

  CFilDiffFrm::GetAccelMgr()->Connect(m_pCmpFilTemplate, RUNTIME_CLASS(CFilDiffFrm), true);
  AfxGetApp()->AddDocTemplate(m_pCmpFilTemplate);

  m_pCmpDirTemplate = new CSingleViewTemplate(
					                       IDR_DIFF_DIR,
					                       RUNTIME_CLASS(CDiffDirDoc),
					                       RUNTIME_CLASS(CDiffDirFrame),
					                       RUNTIME_CLASS(CDiffDirView));

  CDiffDirFrame::GetAccelMgr()->Connect(m_pCmpDirTemplate, RUNTIME_CLASS(CDiffDirFrame), true);
  AfxGetApp()->AddDocTemplate(m_pCmpDirTemplate);

  //Enable Customization
	m_wndToolBar.SetToolBarInfoForCustomization();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // Fehler beim Erzeugen
	}

  UINT nIDX, nStyleX;
  int nWidth;
    
  m_wndStatusBar.GetPaneInfo(0, nIDX, nStyleX, nWidth);

	//No resize the status bar panes.
	UINT nStyle;
	UINT nID;
	int  cxWidth;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, 60);
	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle, 50);


	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
  if (!m_wndToolBar.RestoreState())
  {
    UINT nCmdID = (UINT) -1;

    for (int i=0;i<m_wndToolBar.GetToolBarCtrl().GetButtonCount();i++)
    {
      nCmdID = m_wndToolBar.GetItemID(i);
      if (IsButtonInitialyHidden(nCmdID))
      {
        m_wndToolBar.GetToolBarCtrl().DeleteButton(m_wndToolBar.GetToolBarCtrl().CommandToIndex(nCmdID));
        i--;
      }
      //m_wndToolBar.GetToolBarCtrl().HideButton(nCmdID, IsButtonInitialyHidden(nCmdID));
    }
    m_wndToolBar.SaveState();
  }


	WINDOWPLACEMENT *pWplmt;
	int len;
	AfxGetApp()->GetProfileBinary("Init", "Layout",(BYTE **) &pWplmt, 
		                          (UINT *) &len);
	if(len == sizeof(WINDOWPLACEMENT))
	{
		SetWindowPlacement(pWplmt);
		delete pWplmt;
	}

  TRY
  {
    LoadBarState(_T("Bar State"));
  }
  CATCH_ALL(e)
  {
    int xx = 1;
  }
  END_CATCH_ALL;
  
  AdjustInsertColor(&m_wndToolBar);

  UpdateToolMenu();

  GetUserToolMgr().SetCallbackFct(this, (EXPAND_VAR_FKT)&CMainFrame::ExpandVar, (CAN_EXPAND_VAR_FKT)&CMainFrame::CanExpandVar);

	//Initialize the drag and drop
  m_pFileDropTarget = new CFileDropTarget();
  m_pFileDropTarget->m_pDragEnter = (DragEnterFP) &CMainFrame::DragEnter;
  m_pFileDropTarget->m_pDragLeave = (DragLeaveFP) &CMainFrame::DragLeave;
  m_pFileDropTarget->m_pDragOver  = (DragOverFP)  &CMainFrame::DragOver;
  m_pFileDropTarget->m_pDrop      = (DropFP)      &CMainFrame::Drop;
  m_pFileDropTarget->m_pObject    = this;
  m_pFileDropTarget->Register(this);

  return 0;
}

void CMainFrame::UpdateToolMenu()
{
  CMenu *pMenu = GetMenu();
  CString str;
  bool    bFound = false;
  int nCount = pMenu->GetMenuItemCount();
  for (int i = 0; i < nCount; i++)
  {
    if (pMenu->GetMenuString(i, str, MF_BYPOSITION))
    {
      if (!str.CompareNoCase("Tools"))
      {
        pMenu = pMenu->GetSubMenu(i);
        bFound = pMenu != NULL;
        break;
      }
    }
  }

  if (bFound)
    GetUserToolMgr().UpdateUserToolMenu(pMenu);
}


bool CMainFrame::ExpandVar(const CString& strVar, CStringArray& strArray)
{
  bool bResult = false;
  if (GetActiveFrame())
  {
    CView *pView = GetActiveFrame()->GetActiveView();

    if (pView)
    {
      if (pView->IsKindOf(RUNTIME_CLASS(CDiffDirView)))
	    {
		    bResult = ((CDiffDirView *)pView)->ExpandVar(strVar, strArray);
	    }
	    else if (pView->IsKindOf(RUNTIME_CLASS(CFilDiffView)))
	    {
		    bResult = ((CFilDiffView *)pView)->ExpandVar(strVar, strArray);
	    }
    }
  }
  return bResult;
}

bool CMainFrame::CanExpandVar(const CString& strVar)
{
  bool bCanExpand = false;
  if (GetActiveFrame())
  {
    CView *pView = GetActiveFrame()->GetActiveView();

    if (pView)
    {
      if (pView->IsKindOf(RUNTIME_CLASS(CDiffDirView)))
	    {
		    bCanExpand = ((CDiffDirView *)pView)->CanExpandVar(strVar);
	    }
	    else if (pView->IsKindOf(RUNTIME_CLASS(CFilDiffView)))
	    {
		    bCanExpand = ((CFilDiffView *)pView)->CanExpandVar(strVar);
	    }
    }
  }

  return bCanExpand;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CMDIFrameWnd::PreCreateWindow(cs);
}

CMainFrame *TheTop()
{
	return((CMainFrame *) AfxGetApp()->m_pMainWnd);
}


void CMainFrame::OnCmpDir() 
{
  m_pCmpDirTemplate->OpenDocumentFile("DiffDir", TRUE);
  UpdateToolMenu();
}

void CMainFrame::OnViewOptions() 
{
  CSMXSheet dlg;
  GetOptions().ResetChanges();
  dlg.SetTreeDescription(TreeDescription);
  dlg.SetCreateFunction(this, (SMXPropertyPageFkt) &CMainFrame::CreatePropertyPage);
  dlg.SetSheetSize(194, 155, true);
  dlg.m_nDlgID = m_nShownOptionDlgID;

  dlg.DoModal();

  m_nShownOptionDlgID = dlg.m_nDlgID;

  CDiffDirView *pDirView = (CDiffDirView *) m_pCmpDirTemplate->GetView();
	if(pDirView != NULL) 
	{
		pDirView->Update();
	}

  CFilDiffView *pView = (CFilDiffView *) m_pCmpFilTemplate->GetView();
  if (pView)
  {
    pView->UpdateWndLayout(false);
    pView->UpdateEditControls();
  }
}

BOOL CMainFrame::DestroyWindow() 
{
	WINDOWPLACEMENT wplmt;
	GetWindowPlacement(&wplmt);
	AfxGetApp()->WriteProfileBinary("Init", "Layout", (BYTE *) &wplmt, sizeof(WINDOWPLACEMENT));
	
	return MainFrameParent::DestroyWindow();
}

BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						DWORD dwExStyle, CCreateContext* pContext) 
{
  UNREFERENCED_PARAMETER(nID);
  return MainFrameParent::Create(lpszClassName, lpszWindowName, dwStyle, 
									rect, pParentWnd, MAKEINTRESOURCE(IDR_MAINFRAME),
									dwExStyle, pContext);
	
}


void CMainFrame::OnClose() 
{
  SaveBarState(_T("Bar State"));
	
	MainFrameParent::OnClose();
}

void CMainFrame::OnCmpFile() 
{
  m_pCmpFilTemplate->OpenDocumentFile(NULL, TRUE);
  UpdateToolMenu();
}

void CMainFrame::OnUpdateCmpFile(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	//pCmdUI->Enable(m_pCmpDirTemplate->GetDocument() == NULL);
}

void CMainFrame::OnWindowsNext() 
{
	MDINext();
}

void CMainFrame::OnUpdateWindowsNext(CCmdUI* pCmdUI) 
{
	if(m_pCmpDirTemplate->GetDocument() != NULL &&
		m_pCmpFilTemplate->GetDocument()!=NULL)
	{
		pCmdUI->Enable (TRUE);
	} 
  else 
  {
		pCmdUI->Enable (FALSE);
	}
}

void CMainFrame::OnUpdateCmdIndicator(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CMainFrame::OnCustomizeToolBar()
{
  m_wndToolBar.GetToolBarCtrl().Customize();
}

void CMainFrame::OnCustomizeUserTools()
{
  CSmxUserToolsDlg dlg;
  dlg.SetUserToolMgr(&GetUserToolMgr());
  dlg.DoModal();
  
  UpdateToolMenu();
  UpdateAllAccelMgr();
}

void CMainFrame::OnCustomizeKeyboard()
{
	CAcceleratorManager tmpAccelManager[4];
	tmpAccelManager[0] = ::GetAccelMgr();
  tmpAccelManager[1] = *CDiffDirFrame::GetAccelMgr();
	tmpAccelManager[2] = *CFilDiffFrm::GetAccelMgr();

  CAcceleratorManager readonlyAccMgr[4];
  readonlyAccMgr[2].CreateDefaultTable(IDR_DIFF_EDIT);

  CAccelMapDlg dlg(tmpAccelManager, readonlyAccMgr, 3);
  
  CWnd *pWnd = GetActiveFrame();
  int nIdx = 0;
  if (pWnd)
  {
    if (pWnd->IsKindOf(RUNTIME_CLASS(CDiffDirFrame)))
	  {
      nIdx = 1;
	  }
	  else if (pWnd->IsKindOf(RUNTIME_CLASS(CFilDiffFrm)))
	  {
      nIdx = 2;
	  }
  }
  dlg.m_nInitialAccelMgrIdx = nIdx;

  if (dlg.DoModal() == IDOK) 
  {
    ::GetAccelMgr() = tmpAccelManager[0];
    *CDiffDirFrame::GetAccelMgr() = tmpAccelManager[1];
	  *CFilDiffFrm::GetAccelMgr() = tmpAccelManager[2];
    ::GetAccelMgr().UpdateWndTable();
    CDiffDirFrame::GetAccelMgr()->UpdateWndTable();
    CFilDiffFrm::GetAccelMgr()->UpdateWndTable();
	}
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
  if (nCode == CN_UPDATE_COMMAND_UI)
  {
    CCmdUI *pCmdUI = (CCmdUI *) pExtra;
    if (pCmdUI->m_pMenu)
    {
      CString str;
      CCmdAccelOb *pCmdAccelOb;
      pCmdUI->m_pMenu->GetMenuString(nID, str, MF_BYCOMMAND);
      if (GetAccelMgr().GetAccelTableMap().Lookup(nID, pCmdAccelOb) && pCmdAccelOb &&
          pCmdAccelOb->m_Accels.GetCount() > 0)
      {
        int nIdx = str.Find(TCHAR('\t'));
        if (nIdx != -1)
          str = str.Left(nIdx +1);
        else
          str += TCHAR('\t');


        CString strAccel;
        pCmdAccelOb->m_Accels.GetHead()->GetString(strAccel);
        str += strAccel;
        pCmdUI->SetText(str);
      }
    }
  }
  else
  {
    int XXX = 1;
  }

  return CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

bool CMainFrame::IsButtonInitialyHidden(UINT nCmdID)
{
  switch (nCmdID)
  {
    case ID_CMP_DIR:
    case ID_CMP_FILE:
    case ID_EDIT_CUT:
    case ID_EDIT_COPY:
    case ID_EDIT_PASTE:
    case -1:
      return false;
    default:
      ;
  }
  return true;
}

void CMainFrame::OnUserTool(UINT nCmdID)
{
  GetUserToolMgr().Execute(nCmdID);
}

void CMainFrame::OnUpdateUserTool(CCmdUI *pCmdUI)
{
  pCmdUI->Enable(GetUserToolMgr().CanExecute(pCmdUI->m_nID));
}

CAcceleratorManager& CMainFrame::GetAccelMgr()
{
  CWnd *pWnd = GetActiveFrame();
  if (pWnd)
  {
    if (pWnd->IsKindOf(RUNTIME_CLASS(CDiffDirFrame)))
	  {
      return *CDiffDirFrame::GetAccelMgr();
	  }
	  else if (pWnd->IsKindOf(RUNTIME_CLASS(CFilDiffFrm)))
	  {
      return *CFilDiffFrm::GetAccelMgr();
	  }
  }

  return ::GetAccelMgr();
}

void CMainFrame::UpdateAllAccelMgr()
{
  GetUserToolMgr().UpdateAccelMgr(&::GetAccelMgr());
  GetUserToolMgr().UpdateAccelMgr(CFilDiffFrm::GetAccelMgr());
  GetUserToolMgr().UpdateAccelMgr(CDiffDirFrame::GetAccelMgr());
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD hTask) 
{
#if (_MFC_VER >= 0x0700)
  MainFrameParent::OnActivateApp(bActive, (DWORD) hTask);
#else
  MainFrameParent::OnActivateApp(bActive, (HTASK) hTask);
#endif
	
  if (!bActive)
    return;

  CString str;
  theApp.m_instanceChecker.GetCommandLine(str);

  if (!str.IsEmpty())
  {
    CFCUCmdLineInfo& CmdLine = GetCmdLineInfo();
    CmdLine.Reset();
    CmdLine.ParseCmdLine(str);

    if(CmdLine.m_bCmpFile)
    {
		  CFilDiffParam *pParam = new CFilDiffParam(CmdLine.m_strLeftParam, CmdLine.m_strRightParam);
      pParam->SetFromCmdLine(true);
      PostMessage(WMU_CMPFILE, (WPARAM) pParam, (LPARAM) TRUE); 
    }

	  if(theApp.m_cmdInfo.m_bCmpDir)
		  PostMessage(WM_COMMAND, ID_CMP_DIR, NULL); 
  }
}

void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
  if (nID >= ID_USER_TOOLS_FIRST && nID <= ID_USER_TOOLS_LAST)
  {
    CSmxUserTool *pUserTool = GetUserToolMgr().GetUserTool(nID);

    if (pUserTool && !pUserTool->m_strDescription.IsEmpty())
    {
      rMessage = pUserTool->m_strDescription;
      return;
    }
  }

  MainFrameParent::GetMessageString(nID, rMessage);
}

void CMainFrame::SetStatusText(LPCTSTR text, bool bResetOnChar) 
{ 
  m_wndStatusBar.SetPaneText(0, text); 

  if (bResetOnChar)
  {
    m_nReadyTimer = SetTimer(ID_ReadyTimer, 2000, NULL);
  }
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	return MainFrameParent::PreTranslateMessage(pMsg);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nReadyTimer)
  {
    SetStatusText("Ready");
    KillTimer(nIDEvent);
    m_nReadyTimer = -1;
  }
	
	MainFrameParent::OnTimer(nIDEvent);
}

LRESULT CMainFrame::OnWMUCmpFile(WPARAM wParam, LPARAM lParam)
{
  if (wParam)
  {
    CFilDiffParam* pParam = (CFilDiffParam* )wParam;
    m_pCmpFilTemplate->OpenDocumentFile(*pParam, TRUE);
    UpdateToolMenu();

    if (lParam)
      delete pParam;
  }
  else
  {
    m_pCmpFilTemplate->OpenDocumentFile(NULL, TRUE);
    UpdateToolMenu();
  }
  return 1L;
}

void CMainFrame::OnSmxOptions()
{
  CSMXSheet dlg;
  dlg.SetTreeDescription(TreeDescription);
  dlg.SetCreateFunction(this, (SMXPropertyPageFkt) &CMainFrame::CreatePropertyPage);
  dlg.DoModal();
}

CSMXPropertyPg* CMainFrame::CreatePropertyPage(UINT nDlgID)
{
  CSMXPropertyPg *pPage = NULL;
  switch(nDlgID)
  {
    case IDD_COMPARE:
      pPage = new CComparePg;
      break;
    case IDD_COLORS:
      pPage = new CColorPg;
      break;
    case IDD_FILCMP_OPTIONS:
      pPage = new CFilCmpPg;
      break;
    case IDD_DIRCMP_OPTIONS:
      pPage = new CDirCmpPg;
      break;
    case IDD_EDITOR_OPTIONS:
      pPage = new CEditorPg;
      break;
    case IDD_CMDLINE_OPTIONS:
      pPage = new CCmdLinePg;
      break;
    case IDD_FILDIFF_PRINT_OPTIONS:
      pPage = new CFilDiffPrintPg;
      ((CFilDiffPrintPg *)pPage)->m_eView = VIEW_FilDiff;
      break;
    case IDD_DIFFDIR_PRINT_OPTIONS:
      pPage = new CFilDiffPrintPg;
      ((CFilDiffPrintPg *)pPage)->m_eView = VIEW_DiffDir;
      break;
    default:
      break;
  }

  return pPage;
}

void CMainFrame::AdjustInsertColor(CToolBar *pToolBar)
{
  CImageList *pImageList = pToolBar->GetToolBarCtrl().GetImageList();
  CBitmap bmp;
  bmp.LoadBitmap(IDB_INS_MASK);

  AdjustInsertBitmap(&bmp, GetOptions().GetEditColorLeft());

  IMAGEINFO info;
  pImageList->GetImageInfo(13, &info);
  pImageList->Replace(13, &bmp, CBitmap::FromHandle(info.hbmMask));

  AdjustInsertBitmap(&bmp, GetOptions().GetEditColorRight());
  pImageList->GetImageInfo(14, &info);
  pImageList->Replace(14, &bmp, CBitmap::FromHandle(info.hbmMask));

  m_wndToolBar.GetToolBarCtrl().SetImageList(pImageList);

  bmp.DeleteObject();
}

void CMainFrame::AdjustInsertBitmap(CBitmap *pBmp, COLORREF crColor)
{
  CDC dcSrc;
  CDC dcDest;
  dcSrc.CreateCompatibleDC(NULL);
  dcDest.CreateCompatibleDC(NULL);

  CBitmap bmpSrc;
  bmpSrc.LoadBitmap(IDB_INS_MASK);

  CBitmap *pPrevBmpSrc  = dcSrc.SelectObject(&bmpSrc);
  CBitmap *pPrevBmpDest = dcDest.SelectObject(pBmp);

  BITMAP bmp;
  pBmp->GetObject(sizeof(bmp), &bmp);
  dcDest.FillSolidRect(0, 0, bmp.bmWidth, bmp.bmHeight, crColor);
  
  COLORREF crPrevBkColor  = dcDest.SetBkColor(crColor);
  //COLORREF crPrevTxtColor = dcSrc.SetTextColor();
  dcDest.BitBlt(0, 0, bmp.bmWidth, bmp.bmHeight, &dcSrc, 0, 0, SRCAND);

  //cleanup
  //dcSrc.SetBkColor(crPrevBkColor);
  //dcSrc.SetTextColor(crPrevTxtColor);
  dcDest.SelectObject(pPrevBmpDest);
  dcSrc.SelectObject(pPrevBmpSrc);
  bmpSrc.DeleteObject();
}

DROPEFFECT CMainFrame::DragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, CFileDropTarget* pDrop)
{
  UINT nNofPathNames = 0;
  
  if (!IsWindowEnabled())
    return DROPEFFECT_NONE;

  nNofPathNames = pDrop->GetPathNames(pDataObject, NULL);

  if (nNofPathNames == 0)
    return DROPEFFECT_NONE;

  if (nNofPathNames > 2)
  {
    AfxMessageBox("Sorry, cant handle more than two dropped files/directories.");
    return DROPEFFECT_NONE;
  }

  return DROPEFFECT_LINK;
}

void CMainFrame::DragLeave(CFileDropTarget* pDrop)
{
}

DROPEFFECT CMainFrame::DragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, CFileDropTarget* pDrop)
{
  return DROPEFFECT_LINK;
}

BOOL CMainFrame::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point, CFileDropTarget* pDrop)
{
  CStringArray strArray;
  CString      str;
  UINT nNofPathNames = pDrop->GetPathNames(pDataObject, &strArray);
  
  GetCmdLineInfo().Reset();

  for (int i = 0; i < (int) nNofPathNames; i++)
  {
    GetCmdLineInfo().ParseParam(strArray[i], false, i != (int) (nNofPathNames - 1));
  }
	if(GetCmdLineInfo().m_bCmpFile)
  {
		CFilDiffParam *pParam = new CFilDiffParam(GetCmdLineInfo().m_strLeftParam, GetCmdLineInfo().m_strRightParam);
    pParam->SetFromCmdLine(true);
    PostMessage(WMU_CMPFILE, (WPARAM) pParam, (LPARAM) TRUE); 
  }

	if(GetCmdLineInfo().m_bCmpDir)
		PostMessage(WM_COMMAND, ID_CMP_DIR, NULL); 

  return TRUE;
}
