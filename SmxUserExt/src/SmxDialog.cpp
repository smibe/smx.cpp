// SmxDialog.cpp: implementation of the CSmxDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "HtmlHelp.h"
#include "SmxDialog.h"
#include "Util.h"


int CSmxDialog::sm_nCursorHeight = 0;

CSmxDialog::CSmxDialog()
: m_ptCursor(0, 0)
{
  m_dwTimer = 0;
  m_nToolTipRequest = 0;
  m_nToolTipCtrlID  = 0;
}

CSmxDialog::CSmxDialog(UINT nID, CWnd* pParent) 
: CDialog(nID, pParent)
{
  m_dwTimer = 0;
  m_nToolTipRequest = 0;

}

CSmxDialog::~CSmxDialog()
{

}

BEGIN_MESSAGE_MAP(CSmxDialog, CDialog)
	//{{AFX_MSG_MAP(CSmxDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
  ON_NOTIFY_EX( TTN_NEEDTEXT, 0, ShowTooltip )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSmxDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

  EnableToolTips();

  m_dwTimer = SetTimer(1, 200, NULL);
  
  return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSmxDialog::OnTimer(UINT nIDEvent) 
{
  UINT nCtrlID = 0;
  static bool bIn = false;                  //avoid reentry
  
  if (m_nToolTipRequest == 0)
    return;

  CSmxLock Semafore(bIn);

  if (Semafore.IsLocked())
    return;
  
  Semafore.Lock();

  CPoint ptCursor;
  CPoint ptScreenCursor;
  if (GetCursorPos(&ptCursor))
  {
    //Get the control ID
    ptScreenCursor = ptCursor; 
    ScreenToClient(&ptScreenCursor);
    CWnd *pWnd = ChildWindowFromPoint(ptScreenCursor, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

    if (pWnd && IsChild(pWnd))
      nCtrlID = pWnd->GetDlgCtrlID();
  }
    
  if (m_nToolTipRequest > 3)
  {
    //This meens we are showing the tooltip, check if we are still on the same control
    if (nCtrlID == NULL || nCtrlID != m_nToolTipCtrlID)
    {
      //Remove the tool tip
      SmxHtmlHelp(GetSafeHwnd(), NULL, HH_DISPLAY_TEXT_POPUP, (DWORD) NULL, false);
      m_nToolTipCtrlID  = 0;
      m_nToolTipRequest = 0;
    }
    return;
  }
      
  if (ptCursor == m_ptCursor)
  {
    m_nToolTipRequest++;
  }

  if (m_nToolTipRequest > 3)
  {
    CString str;
    if (str.LoadString(nCtrlID))
    {
      HH_POPUP hhPopup;
      int nDiff = 10;

      //Calculate the height of the cursor
      HCURSOR hCursor = GetCursor();
      if (hCursor)
      {
        ICONINFO IconInfo;
        if (GetIconInfo(hCursor, &IconInfo))
        {
          BITMAP BitMap;
          CBitmap *pBmp = CBitmap::FromHandle(IconInfo.hbmMask);
          if (pBmp->GetBitmap(&BitMap))
          {
            if (BitMap.bmPlanes <= 1)
              nDiff = BitMap.bmHeight / 2;
            else
              nDiff = BitMap.bmHeight;
          }
          nDiff -= IconInfo.yHotspot;
        }
      }
      
      hhPopup.cbStruct = sizeof(HH_POPUP);
      hhPopup.hinst    = AfxGetInstanceHandle();
      hhPopup.idString = 0;
      hhPopup.pszText = str;
      hhPopup.pt.x = m_ptCursor.x;
      hhPopup.pt.y = m_ptCursor.y + nDiff;
      hhPopup.clrForeground = -1;
      hhPopup.clrBackground = -1;
      SetRect(&hhPopup.rcMargins, -1, -1, -1, -1);
      hhPopup.pszFont = NULL;
  
      m_nToolTipCtrlID = nCtrlID;

      SmxHtmlHelp(GetSafeHwnd(), NULL, HH_DISPLAY_TEXT_POPUP, (DWORD) &hhPopup, false);
    }

  }

	CDialog::OnTimer(nIDEvent);
}

void CSmxDialog::OnDestroy() 
{
  CDialog::OnDestroy();
	
  if (m_dwTimer)
  {
    KillTimer(m_dwTimer);
    m_dwTimer = NULL;
  }
}

BOOL CSmxDialog::ShowTooltip(UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
  CPoint ptCursor;
  if (GetCursorPos(&ptCursor))
  {
    if (ptCursor != m_ptCursor)
    {
      m_ptCursor = ptCursor;
      m_nToolTipRequest = 1;
    }
  }

  return FALSE;
}