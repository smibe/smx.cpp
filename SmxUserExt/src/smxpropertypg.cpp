// SMXSheet.cpp: implementation of the CSMXSheet class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "SMXRes.h"
#include "SMXSheet.h"
#include "SMXPropertyPg.h"
#include "HtmlHelp.h"

CSMXPropertyPg::CSMXPropertyPg()
{
  m_nID     = 0;
  m_pSheet  = NULL;
}

CSMXPropertyPg::~CSMXPropertyPg()
{
}

void CSMXPropertyPg::DoDataExchange(CDataExchange* pDX)
{
	CSmxDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMXPropertyPg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSMXPropertyPg, CSmxDialog)
	//{{AFX_MSG_MAP(CSMXPropertyPg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSMXPropertyPg::OnInitDialog() 
{
	CSmxDialog::OnInitDialog();

  if(GetNextDlgTabItem(NULL))
    GetNextDlgTabItem(NULL)->SetFocus();

  return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSMXPropertyPg::Create(UINT nDlgID, CSMXSheet *pSheet)
{
  m_nID    = nDlgID;
  m_pSheet = pSheet;
  return CSmxDialog::Create(nDlgID, pSheet);
}

void CSMXPropertyPg::OnOK()
{
  m_pSheet->OnOK();
}

void CSMXPropertyPg::OnCancel()
{
  m_pSheet->OnCancel();
}


BOOL CSMXPropertyPg::PreCreateWindow(CREATESTRUCT& cs) 
{
  cs.style &= (WS_CAPTION | WS_VISIBLE | WS_POPUP);
  cs.style |= WS_CHILD;
	
	return CSmxDialog::PreCreateWindow(cs);
}


BOOL CSMXPropertyPg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_SYSCHAR)
  {
    if (GetKeyState(VK_MENU))
    {
      if (HandleHotKey(pMsg->wParam))
        return TRUE;
    }
  }
  else if (pMsg->message == WM_KEYDOWN)
  {
    if (pMsg->wParam == VK_TAB)
    {
      CWnd *pWnd = NULL;
      if (HIBYTE(GetKeyState(VK_SHIFT)))
      {
        pWnd = GetNextDlgTabItem(NULL);
        if (GetFocus() == pWnd)
        {
          m_pSheet->SetFocus();
          return TRUE;
        }
      }
      else
      {
        pWnd = GetNextDlgTabItem(NULL);
        pWnd = GetNextDlgTabItem(pWnd, TRUE);
        if (GetFocus() == pWnd)
        {
          m_pSheet->SetFocus();
          return TRUE;
        }
      }
    }
  }

	
	return CSmxDialog::PreTranslateMessage(pMsg);
}

void CSMXPropertyPg::SetNextDlgTabItem(bool bPrevious)
{
  CWnd *pWnd = GetNextDlgTabItem(NULL);

  if (bPrevious)
    pWnd = GetNextDlgTabItem(pWnd, TRUE);
  pWnd->SetFocus();
}

BOOL CSMXPropertyPg::HandleHotKey(UINT nKey)
{
  CWnd *pWnd = GetWindow(GW_CHILD);
  CString str;
  int nIdx = 0;
  
  if (IsChild(GetFocus()))
  {
    //Let the default handle it
    return FALSE;
  }

  while (pWnd)
  {
    pWnd->GetWindowText(str);
    nIdx = str.Find('&');
    if (nIdx != -1 && nIdx + 1 < str.GetLength() )
    {
      TCHAR szKey[2];
      szKey[0] = nKey;
      szKey[1] = NULL;

      if (str.Mid(nIdx +1, 1).CompareNoCase(szKey) == NULL)
      {
        TCHAR szClass[32];
        if (GetFocus() != pWnd)
          pWnd->SetFocus();

        GetClassName(pWnd->m_hWnd, szClass, 32);
        szClass[31] = NULL;

        if (stricmp(szClass, "Button") == 0)
        {
          //It's a button, 
          ((CButton *) pWnd)->SetCheck(!((CButton *) pWnd)->GetCheck());
        }

        return TRUE;
      }
    }
    pWnd = pWnd->GetWindow(GW_HWNDNEXT);
  }
  return FALSE;
}

