// SMXComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SMXUtil.h"
#include "SMXComboBox.h"
#include "SMXRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMXComboBox

CSMXComboBox::CSMXComboBox()
{
  m_b3DFrame      = true;
  m_nShrinkHeight = 0;
  m_bClicked = false;
  m_bLBMoved = false;
  m_bReadOnly = false;
}

CSMXComboBox::~CSMXComboBox()
{
}


void CSMXComboBox::Initialize(CB_INIT *pCbInit)
{
  int nIdx = CB_ERR;
  for (int i = 0; i < INT_MAX; i++)
  {
    if (pCbInit[i].pszText == NULL)
      break;

    nIdx = AddString(pCbInit[i].pszText);
    if (nIdx != CB_ERR)
    {
      SetItemData(nIdx, pCbInit[i].nItemData);
    }
  }
}

int CSMXComboBox::GetCurSelItemData()
{
  int nIdx = GetCurSel();

  if (nIdx == CB_ERR)
    return CB_ERR;

  return(GetItemData(nIdx));
}

int CSMXComboBox::SetCurSelItemWithData(int nData)
{
  for(int nIdx = 0; nIdx < GetCount(); nIdx++)
  {
    if((int) GetItemData(nIdx) == nData)
    {
      SetCurSel(nIdx);
      return nIdx;
    }
  }

  return CB_ERR;
}

void CSMXComboBox::Set3DFrame(bool b3DFrame)
{
  if(!b3DFrame)
  {
    m_nShrinkHeight = 2 * GetSystemMetrics(SM_CYEDGE);
  }
  m_b3DFrame = b3DFrame;
}


BEGIN_MESSAGE_MAP(CSMXComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSMXComboBox)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_NCCALCSIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMXComboBox message handlers

void CSMXComboBox::OnPaint() 
{
  if(m_nShrinkHeight == 0)
  {
    CComboBox::OnPaint();
    return;
  }

  CRect btnRect = m_btnRect;
  
  CRect rcCombo;
  GetWindowRect(rcCombo);
  GetParent()->ScreenToClient(rcCombo);

  CPaintDC dc(this); // device context for painting
  
  //Fill the Face of the button
  // face color
  COLORREF rgbFace    = GetSysColor(COLOR_BTNFACE);
  COLORREF rgbShadow  = GetSysColor(COLOR_BTNSHADOW);
  COLORREF rgbHilite = GetSysColor(COLOR_BTNHIGHLIGHT);
  COLORREF rgbFrame   = GetSysColor(COLOR_WINDOWFRAME);

  CRect rect;

  GetClientRect(rect);
  rect.bottom -= m_nShrinkHeight;

  //HBRUSH hBrush = (HBRUSH) SendMessage(WM_CTLCOLOREDIT);
  dc.FillSolidRect((const RECT *) rect, GetSysColor(COLOR_WINDOW));
  dc.FillSolidRect(btnRect, rgbFace);

  if (m_edit.m_hWnd == NULL)
  {
    //Need to paint the text
    CRect rcText = rect;
    CString strText;
    GetWindowText(strText);
    COLORREF crBgColor = GetSysColor(COLOR_WINDOW);
    COLORREF crTxtColor = GetSysColor(COLOR_WINDOWTEXT);
    rcText.right = m_btnRect.left;
    rcText.left += 3;
    
    CFont *pPrevFont = dc.SelectObject(GetFont());
    COLORREF crPrevBg = dc.SetBkColor(crBgColor);
    COLORREF crPrevTxt = dc.SetTextColor(crTxtColor);
    dc.DrawText(strText, rcText, DT_VCENTER |DT_LEFT | DT_SINGLELINE);
    dc.SetBkColor(crPrevBg);
    dc.SetTextColor(crPrevTxt);
    dc.SelectObject(pPrevFont);
  }
  
  if(m_b3DFrame)
  {
    //Draw a 3D rect
    CRect rc3D = rect;
    dc.Draw3dRect(rc3D, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHILIGHT));
    rc3D.DeflateRect(1,1);
    dc.Draw3dRect(rc3D, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_3DLIGHT));
  }

  //if(m_bReadOnly)
    //return;

  if(m_bClicked)
    dc.Draw3dRect(btnRect, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
  else
    dc.Draw3dRect(btnRect, rgbFace, rgbFrame);

  //shrink the rect
  btnRect.top +=1;
  btnRect.left +=1;
  btnRect.right -= 1;
  btnRect.bottom -=1;

  if(m_bClicked)
    dc.Draw3dRect(btnRect, rgbShadow, rgbShadow);
  else
    dc.Draw3dRect(btnRect, rgbHilite, rgbShadow);

  //shrink the rect
  btnRect.top +=1;
  btnRect.left +=1;
  btnRect.right -= 1;
  btnRect.bottom -=1;

  //now draw the bitmap centered
  CDC memDC;
  if(memDC.CreateCompatibleDC(&dc))
  {
    CBitmap bmp;
    bmp.LoadBitmap(IDB_COMBO_BTN);

    BITMAP bm;
    VERIFY(bmp.GetObject(sizeof(bm), &bm) == sizeof(bm));

    int nLeft = max(0, (btnRect.Width() - bm.bmWidth)/2) + btnRect.left;
    int nTop  = max(0, (btnRect.Height() - bm.bmHeight)/2) + btnRect.top;

    if(m_bClicked)
    {
      // The button is currently pressed, so offset bitmap to give
      // impression of movement.
      nLeft++, nTop++;
    }

    CBitmap *pOldBmp = memDC.SelectObject(&bmp);

    dc.BitBlt(nLeft, nTop, min(bm.bmWidth, btnRect.Width()-1), 
              min(bm.bmHeight, btnRect.Height()-1), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldBmp);
  }
}

void CSMXComboBox::OnSize(UINT nType, int cx, int cy) 
{
  CRect rect;

  CComboBox::OnSize(nType, cx, cy);

  if (!m_b3DFrame)
  {
    //Need to calculate the button rect
    GetWindowRect(rect);
    GetParent()->ScreenToClient(rect);
    GetClientRect(rect);
    m_btnRect = rect;
    m_btnRect.left = m_btnRect.right - GetSystemMetrics(SM_CXVSCROLL);
    if (m_nShrinkHeight)
      m_btnRect.bottom -= m_nShrinkHeight;

  }
}

void CSMXComboBox::OnLButtonDown(UINT nFlags, CPoint pt) 
{
  if (GetStyle() & WS_DISABLED || m_bReadOnly)
    return;
  
  if(m_nShrinkHeight)
  {
    if(m_btnRect.PtInRect(pt))
    {
      m_bClicked = TRUE;
      InvalidateRect(m_btnRect);
    
    }
  }

  //XXXif(GetDroppedState() && !m_edit.m_bAllowInput)
    //SetState(st_focus);

  CComboBox::OnLButtonDown(nFlags, pt);
}

void CSMXComboBox::OnLButtonUp(UINT nFlags, CPoint pt) 
{
  if (GetStyle() & WS_DISABLED || m_bReadOnly)
    return;

  if(m_nShrinkHeight == 0 )
  {
    CComboBox::OnLButtonUp(nFlags, pt);
    return;
  }

  if(m_btnRect.PtInRect(pt))
  {
    CComboBox::OnLButtonUp(nFlags, pt);
    m_bClicked = FALSE;
    InvalidateRect(m_btnRect);
  }
}

void CSMXComboBox::OnMouseMove(UINT nFlags, CPoint pt) 
{
  if (GetStyle() & WS_DISABLED || m_bReadOnly)
    return;

  CComboBox::OnMouseMove(nFlags, pt);

  if(m_nShrinkHeight == 0 )
  {
    return;
  }
  
  bool bState = m_bClicked && m_btnRect.PtInRect(pt);
  if(bState != m_bClicked)
  {
    m_bClicked = bState;
    InvalidateRect(m_btnRect);
    CWnd *pWnd = GetFocus();
  }
}

int CSMXComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  CWnd *pWnd = GetDlgItem(IDC_COMBOEDIT);
  if(pWnd)
    m_edit.SubclassDlgItem(IDC_COMBOEDIT, this);

	return 0;
}

void CSMXComboBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// TODO: Add your message handler code here and/or call default
	
	CComboBox::OnNcCalcSize(bCalcValidRects, lpncsp);
  /*
  if (!m_b3DFrame && bCalcValidRects)
  {
    //We have to add the 3D border
    DWORD dwStyle = GetStyle();
    WORD  nBorderY = (dwStyle & WS_THICKFRAME) ? GetSystemMetrics(SM_CYSIZEFRAME) : GetSystemMetrics(SM_CYFIXEDFRAME);
    WORD  nBorderX = (dwStyle & WS_THICKFRAME) ? GetSystemMetrics(SM_CXSIZEFRAME) : GetSystemMetrics(SM_CXFIXEDFRAME);
    
    lpncsp->rgrc[0].top    -= nBorderY;
    lpncsp->rgrc[0].bottom += nBorderY;
    lpncsp->rgrc[0].left   -= nBorderX;
    lpncsp->rgrc[0].right  += nBorderX;
  }
  */
}

HBRUSH CSMXComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
  CRect rcComboBox;
	
  if(!::IsWindow(m_hWnd))
    return hbr;

  GetWindowRect(rcComboBox);

  if(nCtlColor == CTLCOLOR_LISTBOX && pWnd && pWnd != this)
  {
    CRect rcListBox;
    pWnd->GetWindowRect(rcListBox);

    if(m_nShrinkHeight)
    {
      rcComboBox.bottom = rcComboBox.bottom - m_nShrinkHeight;

      if (rcListBox.top > rcComboBox.bottom)
      {
        rcListBox.bottom = rcComboBox.bottom + rcListBox.Height();
        rcListBox.top = rcComboBox.bottom;
        pWnd->MoveWindow(rcListBox);
      }
    }
  }
	
	return hbr;
}
