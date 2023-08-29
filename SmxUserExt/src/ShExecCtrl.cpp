/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: ShExecCtrl.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/ShExecCtrl.cpp,v 1.2 2002/11/22 17:23:58 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// ShExecCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ShExecCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShExecCtrl

static CFont FontUnderline;
HCURSOR CShExecCtrl::sm_hHandCursor = NULL;
HCURSOR CShExecCtrl::sm_hArrowCursor = NULL;

typedef BOOL  (WINAPI *TRACKMOUSEFKTPTR) (LPTRACKMOUSEEVENT);

class CGetFkt
{
public:
  CGetFkt()
  {
    m_hModule = NULL; m_pTrackMouse = NULL; m_bChecked = false;
  }

  ~CGetFkt() { if (m_hModule) ::FreeLibrary(m_hModule); }

  TRACKMOUSEFKTPTR GetTrackMouseFktPtr()
  {
    if (!m_bChecked)
    {
      m_hModule = LoadLibrary("User32.dll");
      if (m_hModule)
      {
        m_pTrackMouse = (TRACKMOUSEFKTPTR) ::GetProcAddress (m_hModule, "TrackMouseEvent");
        m_bChecked = true;
      }
    }
    return m_pTrackMouse;
  }

public:
  HMODULE          m_hModule;
  TRACKMOUSEFKTPTR m_pTrackMouse;
  bool             m_bChecked;
};

static CGetFkt s_GetFkt;

//BOOL TrackMouseEvent(LPTRACKMOUSEEVENT lpEventTrack);

CShExecCtrl::CShExecCtrl()
{
  if (sm_hArrowCursor == NULL)
    sm_hArrowCursor = ::GetCursor();

  if (sm_hHandCursor == NULL)
  {
    sm_hHandCursor = GetHandCursor();
  }


  m_bSelected = false;
  m_bLeaveTracked = false;
}

CShExecCtrl::~CShExecCtrl()
{
}


BEGIN_MESSAGE_MAP(CShExecCtrl, CButton)
	//{{AFX_MSG_MAP(CShExecCtrl)
	ON_WM_MOUSEMOVE()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
  ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShExecCtrl message handlers

void CShExecCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bLeaveTracked && s_GetFkt.GetTrackMouseFktPtr() != NULL)
  {
    TRACKMOUSEEVENT EventTrack;
    EventTrack.hwndTrack = GetSafeHwnd();
    EventTrack.dwFlags   = TME_LEAVE;
    EventTrack.dwHoverTime = HOVER_DEFAULT;
    EventTrack.cbSize      = sizeof(TRACKMOUSEEVENT);
    //VERIFY( TrackMouseEvent(&EventTrack) );
    VERIFY( (*(s_GetFkt.GetTrackMouseFktPtr()))(&EventTrack) );
    m_bLeaveTracked = true;
  }

  if (!m_bSelected)
  {
    m_bSelected = true;
    Invalidate();
    UpdateWindow();
  }
	
	CButton::OnMouseMove(nFlags, point);
}

void CShExecCtrl::DrawItem(LPDRAWITEMSTRUCT pDrawItem)
{
  CDC *pDC = CDC::FromHandle(pDrawItem->hDC);

  CString str;
  CFont *pPrevFont = NULL;
  GetWindowText(str);


  if (m_bSelected)
  {
    LOGFONT LogFont;
    if (FontUnderline.GetSafeHandle() == NULL)
    {
      if (GetFont())
      {
        GetFont()->GetLogFont(&LogFont);
        LogFont.lfUnderline = TRUE;
        FontUnderline.CreateFontIndirect(&LogFont);
      }
    }

    if (FontUnderline.GetSafeHandle() != NULL)
    {
      pPrevFont = pDC->SelectObject(&FontUnderline);
    }

    pDC->SetTextColor(RGB(0, 0, 255));
  }
  else
    pDC->SetTextColor(RGB(0, 0, 0));


  pDC->DrawText(str, &pDrawItem->rcItem, DT_LEFT | DT_SINGLELINE);

  if (pPrevFont)
    pDC->SelectObject(pPrevFont);

}


LRESULT CShExecCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
  if (s_GetFkt.GetTrackMouseFktPtr() != NULL)
  {
    TRACKMOUSEEVENT EventTrack;
    EventTrack.hwndTrack = GetSafeHwnd();
    EventTrack.dwFlags   = TME_CANCEL;
    EventTrack.dwHoverTime = 0;
    EventTrack.cbSize      = sizeof(TRACKMOUSEEVENT);
    //VERIFY( TrackMouseEvent(&EventTrack) );
    VERIFY( (*(s_GetFkt.GetTrackMouseFktPtr()))(&EventTrack) );
    m_bLeaveTracked = false;
  }
  
  if (m_bSelected)
  {
    m_bSelected = false;
    Invalidate();
    UpdateWindow();
  }

  return 1L;
}

void CShExecCtrl::OnClicked() 
{
  if (m_strCommand.IsEmpty())
    GetWindowText(m_strCommand);

  if (m_strCommand[0] == TCHAR('&'))
    m_strCommand = m_strCommand.Mid(1);

  HINSTANCE hInst = ShellExecute(NULL, "open", m_strCommand, NULL,  NULL, SW_SHOW); 
}

HCURSOR CShExecCtrl::GetHandCursor()
{
  HCURSOR hHandCursor = NULL;
#ifndef _WIN32_WCE
        // Get the windows directory
		CString strWndDir;
		GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
		strWndDir.ReleaseBuffer();

		strWndDir += _T("\\winhlp32.exe");
		// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
		HMODULE hModule = LoadLibrary(strWndDir);
		if( hModule )
		{
			HCURSOR hTempCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
			if( hTempCursor )
			{
				hHandCursor = CopyCursor(hTempCursor);
			}
		}
		FreeLibrary(hModule);
#endif

  if (hHandCursor == NULL)
    hHandCursor = ::GetCursor();

  return hHandCursor;
}


/*
http://activex.microsoft.com/controls/vc/mfc42.cab
*/

BOOL CShExecCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bSelected && sm_hHandCursor)
  {
    ::SetCursor(sm_hHandCursor);
    return true;
  }

	
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}
