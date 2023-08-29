// SMXSheet.cpp: implementation of the CSMXSheet class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "SMXRes.h"
#include "SMXSheet.h"
#include "SMXPropertyPg.h"

UINT aButtons[] = { IDOK, IDCANCEL, ID_HELP, 0 };

CSMXSheet::CSMXSheet(CWnd* pParent /*=NULL*/)
: CDialog(CSMXSheet::IDD, pParent)
{
  m_pTreeDescr = NULL;
  m_pCreateFkt = NULL;
  m_pWnd       = NULL;
  m_pPage      = NULL;
  m_nDlgID     = 0;
  m_rcPage.SetRectEmpty();
}

CSMXSheet::~CSMXSheet()
{
  UINT            nDlgID;
  CSMXPropertyPg *pPage = NULL;
  for (POSITION pos = m_PageMap.GetStartPosition(); pos; )
  {
    m_PageMap.GetNextAssoc(pos, nDlgID, pPage);
    delete pPage;
  }
}

void CSMXSheet::SetTreeDescription(SMXTreeDescription *pTreeDescr)
{
  m_pTreeDescr = pTreeDescr;
}

void CSMXSheet::SetCreateFunction(CWnd *pWnd, SMXPropertyPageFkt pCreateFkt)
{
  ASSERT(pWnd && pCreateFkt);
  m_pCreateFkt = pCreateFkt;
  m_pWnd       = pWnd;
}

void CSMXSheet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMXSheet)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSMXSheet, CDialog)
	//{{AFX_MSG_MAP(CSMXSheet)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define MAX_TREE_INDENTATION 10

BOOL CSMXSheet::OnInitDialog() 
{
	CDialog::OnInitDialog();
  HTREEITEM hInitial = 0;

  if (m_pTreeDescr == NULL || m_pCreateFkt == NULL)
  {
    ASSERT(false);
    //ASSERT -> you must set the tree description before anything else!!
    return TRUE;
  }

  SMXTreeDescription *pTreeDescript = m_pTreeDescr;
  HTREEITEM           aItemStack[MAX_TREE_INDENTATION];

  UINT       nStackIdx = 1;
  HTREEITEM  hItem = NULL;

  aItemStack[0] = TVI_ROOT;

  while (pTreeDescript->nIndent > 0)
  {
    if (pTreeDescript->nIndent > nStackIdx + 1 || pTreeDescript->nIndent > MAX_TREE_INDENTATION)
    {
      ASSERT(false);
      //ASSERT -> you should not skip indentation levels in the tree description
      pTreeDescript++;
      continue;
    }

    hItem  = m_Tree.InsertItem(pTreeDescript->pszItem, aItemStack[pTreeDescript->nIndent - 1]);
    m_Tree.SetItemData(hItem, (DWORD) pTreeDescript);
    aItemStack[pTreeDescript->nIndent] = hItem;
    nStackIdx = pTreeDescript->nIndent;

    if (m_nDlgID && m_nDlgID == pTreeDescript->nDlgID)
      hInitial = hItem;

    pTreeDescript++;
  }

  CRect rcClient;
  CRect rcTree;
  CRect rcButton;
  bool  bButton = FALSE;

  GetClientRect(rcClient);
  m_Tree.GetWindowRect(rcTree);
  ScreenToClient(rcTree);

  if (GetDlgItem(ID_HELP))
  {
    bButton = true;
    GetDlgItem(ID_HELP)->GetWindowRect(rcButton);
    ScreenToClient(rcButton);
  }

  int nDist = max (5, rcTree.left/2);

  
  if (!m_rcPage.IsRectEmpty())
  {
    //The user has set his dialog size
    if (m_bDialogUnits)
    {
      MapDialogRect(m_rcPage);
      m_bDialogUnits = false;
    }
   
    CRect rcWnd;
    GetClientRect(rcWnd);
    CSize szDiff;
    szDiff.cx = rcWnd.Width() - m_rcPage.Width() - nDist - 2 * rcTree.left - rcTree.Width();
    szDiff.cy = rcWnd.Height() - m_rcPage.Height() - nDist - 2 * rcTree.top - rcButton.Height();
    GetWindowRect(rcWnd);
    rcWnd.bottom -= szDiff.cy;
    rcWnd.right -= szDiff.cx;
    MoveWindow(rcWnd);
    
    //Move the tree control
    rcTree.bottom -= szDiff.cy;
    m_Tree.MoveWindow(rcTree);

    //Move the buttons
    for (int i = 0; aButtons[i] != 0; i++)
    {
      if (GetDlgItem(aButtons[i]))
      {
        GetDlgItem(aButtons[i])->GetWindowRect(rcButton);
        ScreenToClient(rcButton);
        rcButton -= szDiff;
        GetDlgItem(aButtons[i])->MoveWindow(rcButton);
      }
    }
  }

  m_rcPage.top = rcTree.top;

  m_rcPage.left = rcTree.right + nDist;
  if (bButton)
  {
    m_rcPage.bottom = rcButton.top - nDist;
    m_rcPage.right  = rcButton.right;
  }
  else
  {
    m_rcPage.bottom = rcTree.bottom;
    m_rcPage.right = rcClient.right - rcTree.left;
  }

  if (hInitial)
    m_Tree.Select(hInitial, TVGN_CARET);

  return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSMXSheet::OnOK()
{
  UINT            nDlgID;
  CSMXPropertyPg *pPage = NULL;
  for (POSITION pos = m_PageMap.GetStartPosition(); pos; )
  {
    m_PageMap.GetNextAssoc(pos, nDlgID, pPage);
    pPage->UpdateData(TRUE);
    pPage->EndDialog(IDOK);
  }

  HTREEITEM hItem = m_Tree.GetSelectedItem();
  if (hItem)
  {
    SMXTreeDescription *pTreeDescript = (SMXTreeDescription *) m_Tree.GetItemData(hItem);
    if (pTreeDescript)
      m_nDlgID = pTreeDescript->nDlgID;
  }

  EndDialog(IDOK);
}

void CSMXSheet::OnCancel()
{
  UINT            nDlgID;
  CSMXPropertyPg *pPage = NULL;
  for (POSITION pos = m_PageMap.GetStartPosition(); pos; )
  {
    m_PageMap.GetNextAssoc(pos, nDlgID, pPage);
    pPage->EndDialog(IDCANCEL);
  }
  
  HTREEITEM hItem = m_Tree.GetSelectedItem();
  if (hItem)
  {
    SMXTreeDescription *pTreeDescript = (SMXTreeDescription *) m_Tree.GetItemData(hItem);
    if (pTreeDescript)
      m_nDlgID = pTreeDescript->nDlgID;
  }

    EndDialog(IDCANCEL);
}


void CSMXSheet::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
  HTREEITEM hOld = pNMTreeView->itemOld.hItem;
  HTREEITEM hNew = pNMTreeView->itemNew.hItem;

  DisplayPage(hNew);   
	*pResult = 0;
}

void CSMXSheet::DisplayPage(HTREEITEM hItem)
{
  SMXTreeDescription *pTreeDescript = (SMXTreeDescription *) m_Tree.GetItemData(hItem);

  if (pTreeDescript && (m_pPage == NULL || pTreeDescript->nDlgID != m_pPage->GetID()))
  {
    CSMXPropertyPg *pNew = NULL;
    if (!m_PageMap.Lookup(pTreeDescript->nDlgID, pNew))
    {
      pNew = (m_pWnd->*m_pCreateFkt)(pTreeDescript->nDlgID);
      if (!pNew->Create(pTreeDescript->nDlgID, this))
      {
        ASSERT(false);
        return;
      }

      pNew->MoveWindow(m_rcPage);
      m_PageMap.SetAt(pTreeDescript->nDlgID, pNew);
      m_Tree.SetFocus();
    }

    if (m_pPage)
      m_pPage->ShowWindow(SW_HIDE);

    pNew->ShowWindow(SW_SHOW);
    m_pPage = pNew;
  }
}

BOOL CSMXSheet::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
  {
    TCHAR c = pMsg->wParam;
    if ('a' <= c && c <= 'z' && GetKeyState(VK_MENU))
    {
      if(m_pPage->PreTranslateMessage(pMsg))
        return TRUE;
    }
    if (pMsg->message == WM_KEYDOWN)
    {
      if (c == VK_TAB)
      {
        m_pPage->SetNextDlgTabItem(HIBYTE(GetKeyState(VK_SHIFT)) != NULL);
        return TRUE;
      }
    }
  }
  
  
	return CDialog::PreTranslateMessage(pMsg);
}


void CSMXSheet::SetSheetSize(UINT nWidth, UINT nHeight, bool bDialogUnits)
{
  m_rcPage.top = 0;
  m_rcPage.left = 0;
  m_rcPage.bottom = nHeight;
  m_rcPage.right = nWidth;
  m_bDialogUnits = bDialogUnits;
}

