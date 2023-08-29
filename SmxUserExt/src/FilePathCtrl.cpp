// FilePathCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FilePathCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilePathCtrl

CFilePathCtrl::CFilePathCtrl()
{
}

CFilePathCtrl::~CFilePathCtrl()
{
}


BEGIN_MESSAGE_MAP(CFilePathCtrl, CStatic)
	//{{AFX_MSG_MAP(CFilePathCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilePathCtrl message handlers

void CFilePathCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcWnd;
  GetClientRect(rcWnd);
  CString str;
  GetWindowText(str);

  UINT nFormat = DT_SINGLELINE | DT_PATH_ELLIPSIS | DT_BOTTOM;
  dc.SelectObject(GetFont());
  dc.FillSolidRect(rcWnd, GetSysColor(COLOR_BTNFACE));
  rcWnd.left += 2;
  dc.DrawText(str, str.GetLength(), rcWnd, nFormat);
}
