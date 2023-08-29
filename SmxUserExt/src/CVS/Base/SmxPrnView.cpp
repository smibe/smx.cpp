/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SmxPrnView.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SmxPrnView.cpp,v 1.2 2002/11/22 17:23:17 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "StdAfx.h"
//#include "PrnStatusDlg.h"

#ifndef CScrollView
#include "SmxPrnView.h"
#endif

#include "Util.h"
#include "FormEdit.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CSmxPrnView, CScrollView)

const TCHAR CSmxPrnView::sm_szBold[]      = _T("bold");     // Bold value
const TCHAR CSmxPrnView::sm_szSub[]       = _T("sub");      // Subscript value
const TCHAR CSmxPrnView::sm_szSymb[]      = _T("symb");     // Change to symbol font
const TCHAR CSmxPrnView::sm_szUnicode[]   = _T("unicode");  // Unicode symbols
const TCHAR CSmxPrnView::sm_szSymbol[]    = _T("Symbol");   // The symbol font face

BEGIN_MESSAGE_MAP(CSmxPrnView, CScrollView)
	//{{AFX_MSG_MAP(CBookJournalView)
	//ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//***************************************************************************
// Function         CSmxPrnView::CSmxPrnView
// @mfunc           Constructor.
//***************************************************************************
#ifdef CScrollView
CSmxPrnView::CSmxPrnView(UINT nID)
: CFormView(nID)
#else
CSmxPrnView::CSmxPrnView()
#endif
{
  m_pDC = NULL;
  m_nCurPage  = 1;
  m_nNofPages = 1;

  m_nX = 0;
  m_nY = 0;

  m_nMaxX = 0;
  m_nMaxY = 0;

  m_nRectTopMargin     = 0;
  m_nRectBottomMargin  = 0;
  m_nRectLeftMargin    = 0;
  m_nRectRightMargin   = 0;

  m_nPaperLeftMargin = 10;
  m_nPaperRightMargin = 10;
  m_nPaperTopMargin = 5;
  m_nPaperBottomMargin = 5;

  m_nHeaderHeight = 3;
  m_nFooterHeight = 3;

  m_nClientLeftMargin = 2;
  m_nClientRightMargin = 2;
  m_nClientTopMargin = 2;
  m_nClientBottomMargin = 2;

  ::ZeroMemory(&m_LogFont, sizeof(m_LogFont));
  ::ZeroMemory(&m_LogBrush, sizeof(m_LogBrush));
  ::ZeroMemory(&m_LogPen, sizeof(m_LogPen));

  // The map mode should be set in the constructor otherwise the conversion functions
  // would not work correctly.
  m_nMapMode           = MM_HIMETRIC;

  m_bCalcNofPages = false;
}

//***************************************************************************
// Function         CSmxPrnView::~CSmxPrnView
// @mfunc           Destructor.
//***************************************************************************
CSmxPrnView::~CSmxPrnView()
{

}

//***************************************************************************
// Function         CSmxPrnView::GetDefaultFont
// @mfunc           Retrieve the default font.
// @parm            OUT: Logfont structure retrieving the font.
//***************************************************************************
void CSmxPrnView::GetDefaultFont(LOGFONT *pLogFont)
{
  if (GetFont())
    GetFont()->GetLogFont(pLogFont);
  else if (AfxGetMainWnd() && AfxGetMainWnd()->GetFont())
    AfxGetMainWnd()->GetFont()->GetLogFont(pLogFont);
}

//***************************************************************************
// Function         CSmxPrnView::GetDefaultPen
// @mfunc           Retrieve the default pen.
// @parm            OUT: The default pen settings.
//***************************************************************************
void CSmxPrnView::GetDefaultPen(LOGPEN *pLogPen)
{
  CPen::FromHandle((HPEN ) GetStockObject(BLACK_PEN))->GetLogPen(pLogPen);
}

//***************************************************************************
// Function         CSmxPrnView::GetDefaultBrush
// @mfunc           Retrieve the default brush.
// @parm            OUT: The default brush settings.
//***************************************************************************
void CSmxPrnView::GetDefaultBrush(LOGBRUSH *pLogBrush)
{
  CBrush::FromHandle((HBRUSH ) GetStockObject(BLACK_BRUSH))->GetLogBrush(pLogBrush);
}


//***************************************************************************
// Function         CSmxPrnView::GetPageCount
// @mfunc           Overloaded member function which returns the number of pages.
// @rdesc           Default: returns 1.
//***************************************************************************
int CSmxPrnView::GetPageCount() const
{
  return 1;
}

//***************************************************************************
// Function         CSmxPrnView::OnPrint
// @mfunc           Overloaded member function which actually does the printing.
// @parm            Device context.
// @parm            Print info.
//***************************************************************************
void CSmxPrnView::OnPrint (CDC* pDC, CPrintInfo* pInfo)
{
  m_nCurPage = pInfo->m_nCurPage;
  CScrollView::OnPrint(pDC, pInfo);
}

//***************************************************************************
// Function         CSmxPrnView::OnBeginPrinting
// @mfunc           Virtual function called for one time initializations.
// @parm            Device context.
// @parm            Print info.
//***************************************************************************
void CSmxPrnView::OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo)
{
  CScrollView::OnBeginPrinting(pDC, pInfo);

  CDC *pPrevDC = m_pDC;
  m_pDC = pDC;
  m_rcClient = pInfo->m_rectDraw;

  m_nRectTopMargin     = PointsToLP(3);
  m_nRectBottomMargin  = PointsToLP(3);

  m_nRectTopMargin     = PointsToLP(2);
  m_nRectBottomMargin  = PointsToLP(2);
  m_nRectLeftMargin    = GetClientLeftMargin();
  m_nRectRightMargin   = GetClientRightMargin();

  m_nX = GetLeftPrintPos();
  m_nY = GetTopPrintPos();

  m_pDC = pPrevDC;
}


//***************************************************************************
// Function         CSmxPrnView::SetFont
// @mfunc           Set a new font.
// @parm            LOGFONT structure of the new font.
//***************************************************************************
void CSmxPrnView::SetFont(const LOGFONT *pLogFont)
{
  if (memcmp(&m_LogFont, pLogFont, sizeof(LOGFONT)) != 0)
  {
    memcpy(&m_LogFont, pLogFont, sizeof(LOGFONT));
    m_nUpdateFlags |= UPD_FONT;
  }
}

//***************************************************************************
// Function         CSmxPrnView::GetLogFont
// @mfunc           Retrieve the current log font.
// @parm            OUT: pointer to the log font which will be filled.
//***************************************************************************
void CSmxPrnView::GetLogFont(LOGFONT *pLogFont)
{
  memcpy(pLogFont, &m_LogFont, sizeof(LOGFONT));
}

//***************************************************************************
// Function         CSmxPrnView::SetFontSize
// @mfunc           Set a new font size. The font size is given in points!
// @parm            The size (in points) of the new font.
//***************************************************************************
void CSmxPrnView::SetFontSize(int nSize)
{
  int nPrevHeight = m_LogFont.lfHeight;
  m_LogFont.lfHeight = - PointsToLP(nSize);
  
  if (m_LogFont.lfHeight != nPrevHeight)
    m_nUpdateFlags |= UPD_FONT;
}

//***************************************************************************
// Function         CSmxPrnView::SetFontWeight
// @mfunc           Set a new font weight.
// @parm            The new font weight.
//***************************************************************************
void CSmxPrnView::SetFontWeight(int nWeight)
{
  if (m_LogFont.lfWeight != nWeight)
  {
    m_LogFont.lfWeight = nWeight;
    m_nUpdateFlags |= UPD_FONT;
  }
}

//***************************************************************************
// Function         CSmxPrnView::SetFontName
// @mfunc           Select a new font name.
// @parm            The new font name.
//***************************************************************************
void CSmxPrnView::SetFontName(LPCTSTR pszFontName)
{
  _tcsncpy(m_LogFont.lfFaceName, pszFontName, LF_FACESIZE);
  m_nUpdateFlags |= UPD_FONT;
}

//***************************************************************************
// Function         CSmxPrnView::SetPen
// @mfunc           Set the specified pen.
// @parm            LOGPEN structure of the new pen.
//***************************************************************************
void CSmxPrnView::SetPen(const PLOGPEN pLogPen)
{
  if (memcmp(&m_LogPen, pLogPen, sizeof(LOGPEN)) != 0)
  {
    memcpy(&m_LogPen, pLogPen, sizeof(LOGPEN));
    m_nUpdateFlags |= UPD_PEN;
  }
}

//***************************************************************************
// Function         CSmxPrnView::GetLogPen
// @mfunc           Retrieve the LOGPEN structure of the current pen.
// @parm            OUT: the LOGPEN of the current pen.
//***************************************************************************
void CSmxPrnView::GetLogPen(PLOGPEN pLogPen)
{
  memcpy(pLogPen, &m_LogPen, sizeof(LOGPEN));
}

//***************************************************************************
// Function         CSmxPrnView::SetPenStyle
// @mfunc           Set a new pen style.
// @parm            The new style of the pen.
//***************************************************************************
void CSmxPrnView::SetPenStyle(UINT nStyle)
{
  if (nStyle != m_LogPen.lopnStyle)
  {
    m_LogPen.lopnStyle = nStyle;
    m_nUpdateFlags |= UPD_PEN;
  }
}

//***************************************************************************
// Function         CSmxPrnView::SetPenWidth
// @mfunc           Set a new pen width. The pen width is given in points.
// @parm            The new pen width in points.
//***************************************************************************
void CSmxPrnView::SetPenWidth(double dbWidth)
{
  m_LogPen.lopnWidth.x = PointsToLP(dbWidth);
  m_nUpdateFlags |= UPD_PEN;
}

//***************************************************************************
// Function         CSmxPrnView::SetBrush
// @mfunc           Set a new brush.
// @parm            
// @rdesc           None
// @comm Exception  
//***************************************************************************
void CSmxPrnView::SetBrush(const PLOGBRUSH pLogBrush)
{
  if (memcmp(&m_LogBrush, pLogBrush, sizeof(LOGBRUSH)) != 0)
  {
    memcpy(&m_LogBrush, pLogBrush, sizeof(LOGBRUSH));
    m_nUpdateFlags |= UPD_BRUSH;
  }
}

//***************************************************************************
// Function         CSmxPrnView::SetBrushColor
// @mfunc           Set a new brush color.
// @parm            The new brush color.
//***************************************************************************
void CSmxPrnView::SetBrushColor(COLORREF crBrush)
{
  if (m_LogBrush.lbColor != crBrush)
  {
    m_LogBrush.lbColor = crBrush;
    m_nUpdateFlags |= UPD_BRUSH;
  }
}

//***************************************************************************
// Function         CSmxPrnView::UpdateBrush
// @mfunc           Update the device context brush.
//***************************************************************************
void CSmxPrnView::UpdateBrush()
{
  if (m_nUpdateFlags & UPD_BRUSH)
  {
    if (m_Brush.GetSafeHandle() != NULL)
      m_Brush.DeleteObject();

    m_Brush.CreateBrushIndirect(&m_LogBrush);

    m_pDC->SelectObject(&m_Brush);
  }
}
 
//***************************************************************************
// Function         CSmxPrnView::UpdateFont
// @mfunc           Update the device context font.
//***************************************************************************
void CSmxPrnView::UpdateFont()
{
  if (m_nUpdateFlags & UPD_FONT)
  {
    if (m_Font.GetSafeHandle() != NULL)
      m_Font.DeleteObject();

    VERIFY(m_Font.CreateFontIndirect(&m_LogFont));

    m_pDC->SelectObject(&m_Font);
  }
}

//***************************************************************************
// Function         CSmxPrnView::UpdatePen
// @mfunc           Update the device context pen.
//***************************************************************************
void CSmxPrnView::UpdatePen()
{
  if (m_nUpdateFlags & UPD_PEN)
  {
    if (m_Pen.GetSafeHandle() != NULL)
      m_Pen.DeleteObject();

    m_Pen.CreatePenIndirect(&m_LogPen);

    m_pDC->SelectObject(&m_Pen);
  }
}

//***************************************************************************
// Function         CSmxPrnView::PrintText
// @mfunc           Print a text. The text will be printed with the currnt font.
// @parm            The text which should be printed.
// @parm            The text format. (for available formats check DrawText() ).
// @parm            Set to true if the text uses special formatting with formatting
//                  tags like <sub ...> for subscript etc.
//***************************************************************************
void CSmxPrnView::PrintText(LPCTSTR pszText, UINT nFormat, EPrnFlags ePrnFlags)
{
  LOGFONT  LogFont;

  if (pszText == NULL)
    return;

  if (ePrnFlags & PRN_MARKED)
  {
    memcpy(&LogFont, &m_LogFont,  sizeof(LOGFONT));
    SetFontWeight(FW_BOLD);
  }
  
  if (ePrnFlags & PRN_FORMATTED)
    PrintFormattedText(pszText, nFormat);
  else
    PrintSimpleText(pszText, nFormat);

  if (ePrnFlags & PRN_MARKED)
  {
    SetFont(&LogFont);
  }

  if (m_nY > m_nMaxY)
    m_nMaxY = m_nY;

  if (m_nX > m_nMaxX)
    m_nMaxX = m_nX;
}

//***************************************************************************
// Function         CSmxPrnView::PrintSimpleText
// @mfunc           Print a text which does not have formatting tags.
// @parm            The text to be printes.
// @parm            The format of the text.
//***************************************************************************
void CSmxPrnView::PrintSimpleText(LPCTSTR pszText, UINT nFormat)
{
  UpdateFont();

  if (pszText == 0 || strlen(pszText) == 0)
    return;

  //TRACE1("%d\n", m_nX);

  CRect rcText;

  CalcTextRect(rcText, pszText, nFormat);

  if (rcText.right > GetRightPrintPos() - GetRightMargin())
  {
    //TRACE("Clipping at right margin!\n");
    rcText.right = GetRightPrintPos();
  }

    
  if (rcText.bottom > GetBottomPrintPos() && m_rcPrint != m_rcClient)
  {
    //TRACE("Clipping at bottom margin!\n");
    rcText.bottom = GetBottomPrintPos();
  }

  //Draw the text
  LPtoDC(rcText);
  m_pDC->DrawText(pszText, -1, rcText, nFormat);
  DCtoLP(rcText);

  //Move the positions
  m_nX = rcText.right;

  if (!(nFormat & DT_SINGLELINE))
    m_nY = rcText.bottom;
}

void CSmxPrnView::CalcTextRect(CRect&rcText, LPCTSTR pszText, UINT nFormat)
{
  if (m_nX < GetLeftPrintPos() + GetLeftMargin())
    m_nX = GetLeftPrintPos() + GetLeftMargin();

  if (m_nY < GetTopPrintPos() + GetTopMargin())
    m_nY = GetTopPrintPos() + GetTopMargin();

  rcText = CRect(m_nX, m_nY, m_nX, m_nY);
  if ((nFormat & DT_SINGLELINE) == 0)
  {
    ASSERT(m_nX == GetLeftPrintPos() + GetLeftMargin());
    //Multiline text, need to make sure that we start from the beginning

    rcText.right = GetRightPrintPos() - GetRightMargin();
  }

  //Calculate the rectangle for drawing the text
  UINT nCalcFormat = nFormat | DT_CALCRECT;
  nCalcFormat &= ~DT_VCENTER;
  LPtoDC(rcText);
  m_pDC->DrawText(pszText, -1, rcText, nCalcFormat);
  DCtoLP(rcText);

  if (nFormat & DT_RIGHT)
  {
    //The text should be right alligned in the printing rectangle, move the rectangle.
    int dx = GetRightPrintPos() - GetRightMargin() - rcText.right;
    rcText.right = GetRightPrintPos() - GetRightMargin();
    rcText.left += dx;
  }

  if (nFormat & DT_CENTER)
  {
    //The text should be centered in the printing rectangle, move the rectangle accordingly.
    int dx = ((GetRightPrintPos() - GetRightMargin() - GetLeftPrintPos() - GetLeftMargin())  - rcText.Width()) / 2;
    int nWidth = rcText.Width();
    rcText.left  = GetLeftPrintPos() + GetLeftMargin() + dx;
    rcText.right = rcText.left + nWidth;
  }

  if (nFormat & DT_VCENTER)
  {
    //The text should be centered vertically in the printing rectangle, move the rectangle accordingly.
    int dy = ((GetBottomPrintPos() - GetBottomMargin() - GetTopPrintPos() - GetTopMargin())  - rcText.Height()) / 2;
    int nHeight = rcText.Height();
    rcText.top    = GetTopPrintPos() + GetTopMargin() + dy;
    rcText.bottom = rcText.top + nHeight;
  }

}

//***************************************************************************
// Function         CSmxPrnView::PrintEOL
// @mfunc           Move to the beginning of the next line.
//***************************************************************************
void CSmxPrnView::PrintEOL()
{
  m_nX = GetLeftPrintPos();
  m_nY += GetCurrentLineHeight();

  if (m_nY > m_nMaxY)
    m_nMaxY = m_nY;
}

//***************************************************************************
// Function         CSmxPrnView::PrintLineTo
// @mfunc           Print a line from the current to the given position.
// @parm            The x coordinate.
// @parm            The y coordinate.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CSmxPrnView::PrintLineTo(int x, int y)
{
  UpdatePen();
  m_pDC->MoveTo(m_nX, LPtoDC_y(m_nY));
  return m_pDC->LineTo(x, LPtoDC_y(y)) != NULL;
}

//***************************************************************************
// Function         CSmxPrnView::PrintLine
// @mfunc           Print a line.
// @parm            x coordinate of the first point.
// @parm            y coordinate of the first point.
// @parm            x coordinate of the second point.
// @parm            y coordinate of the second point.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CSmxPrnView::PrintLine(int xFrom, int yFrom, int xTo, int yTo)
{
  UpdatePen();
  m_pDC->MoveTo(xFrom, LPtoDC_y(yFrom));
  return m_pDC->LineTo(xTo, LPtoDC_y(yTo)) != NULL;
}

//***************************************************************************
// Function         CSmxPrnView::PrintVLine
// @mfunc           Print a vertical line at the current x position from top 
//                  to the bottom of the printing rectangle.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CSmxPrnView::PrintVLine()
{
  UpdatePen();
  m_pDC->MoveTo(m_nX, LPtoDC_y(GetTopPrintPos()));
  return m_pDC->LineTo(m_nX, LPtoDC_y(GetBottomPrintPos())) != NULL;
}

//***************************************************************************
// Function         CSmxPrnView::PrintHLine
// @mfunc           Print a horizontal line from the left to the right border 
//                  of the printing rectangle. If there is something written 
//                  in this row the current positon is moved downwards with 
//                  the height of one row.
//                  The current position will be moved downwards with the size
//                  of the current pen. 
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CSmxPrnView::PrintHLine()
{
  UpdatePen();
  bool bResult = true;

  if (m_nX > GetLeftPrintPos() + GetLeftMargin())
  {
    //We have written something in this row, move the point in the next row
    m_nY += GetCurrentLineHeight();
    m_nX = GetLeftPrintPos();
  }

  m_pDC->MoveTo(GetLeftPrintPos(), LPtoDC_y(m_nY));
  bResult = m_pDC->LineTo(GetRightPrintPos(), LPtoDC_y(m_nY)) != NULL;

  //Move the point vertically
  m_nY += m_LogPen.lopnWidth.x;

  if (m_nY > m_nMaxY)
    m_nMaxY = m_nY;

  return bResult;
}

//***************************************************************************
// Function         CSmxPrnView::FramePrintRect
// @mfunc           Frame the printing rectangle with the current pen.
// @parm            Flags indicating which lines should be drawn (FRM_LEFT,.., FRM_BOTTOM).
//***************************************************************************
void CSmxPrnView::FramePrintRect(UINT nFrameFlags)
{
  UpdatePen();
  CRect rcRect = m_rcPrint;
  FrameRect(rcRect, nFrameFlags);
}

//***************************************************************************
// Function         CSmxPrnView::FrameRect
// @mfunc           Frame the specified rectangle with the current pen.
// @parm            The rectangle to be framed.
// @parm            Flags indicating which lines should be drawn (FRM_LEFT,.., FRM_BOTTOM).
//***************************************************************************
void CSmxPrnView::FrameRect(LPCRECT pRect, UINT nFrameFlags)
{
  UpdatePen();
  CRect rcRect = pRect;

  if (nFrameFlags & FRM_TOP)
    PrintLine(rcRect.left, rcRect.top, rcRect.right, rcRect.top);

  if (nFrameFlags & FRM_BOTTOM)
    PrintLine(rcRect.left, rcRect.bottom, rcRect.right, rcRect.bottom);

  if (nFrameFlags & FRM_LEFT)
    PrintLine(rcRect.left, rcRect.top, rcRect.left, rcRect.bottom);

  if (nFrameFlags & FRM_RIGHT)
    PrintLine(rcRect.right, rcRect.top, rcRect.right, rcRect.bottom);
}

//***************************************************************************
// Function         CSmxPrnView::FillSolidRect
// @mfunc           Fill the specified rectangle withe the given color.
// @parm            The rectangle to be filled.
// @parm            The color with which the rectangle should be filled.
//***************************************************************************
void CSmxPrnView::FillSolidRect(LPCRECT pRect, COLORREF crFill)
{
  CRect rcRect = pRect;
  LPtoDC(rcRect);
  m_pDC->FillSolidRect(rcRect, crFill);
}

//***************************************************************************
// Function         CSmxPrnView::MoveTo
// @mfunc           Move the current position to the specified coordinates.
// @parm            The new x coordinate.
// @parm            The new y coordinate.
// @rdesc           The previous position.
//***************************************************************************
CPoint CSmxPrnView::MoveTo(int x, int y)
{
  m_nX = x;
  m_nY = y;
  return m_pDC->MoveTo(x, LPtoDC_y(y));
}

//***************************************************************************
// Function         CSmxPrnView::MoveTo
// @mfunc           Move the current position to the coordinates specified by 
//                  the given point.
// @parm            The new coordinates.
// @rdesc           The previous position.
//***************************************************************************
CPoint CSmxPrnView::MoveTo(CPoint Point)
{
  m_nX = Point.x;
  m_nY = Point.y;
  LPtoDC(Point);
  return m_pDC->MoveTo(Point);
}

//***************************************************************************
// Function         CSmxPrnView::GetCurrentPosition
// @mfunc           Retrieve the current position.
// @rdesc           The current position.
//***************************************************************************
CPoint CSmxPrnView::GetCurrentPosition()
{
  return CPoint(m_nX, m_nY);
}

//***************************************************************************
// Function         CSmxPrnView::MoveHSpace
// @mfunc           Insert a horizontal space.
// @parm            Value of the horizontal space.
//***************************************************************************
void CSmxPrnView::MoveHSpace(int dx)
{
  m_nX += dx;
}

//***************************************************************************
// Function         CSmxPrnView::MoveVSpace
// @mfunc           Insert a vertical space.
// @parm            The value of the vertical space.
//***************************************************************************
void   CSmxPrnView::MoveVSpace(int dy)
{
  m_nY += dy;
}

//***************************************************************************
// Function         CSmxPrnView::SetPrintRect
// @mfunc           Set a new printing rectangle. The current position will
//                  be moved to the TopLeft corner of the printing rectangle.
// @parm            The new printing rectangle or NULL if the current client
//                  rectangle should be used.
//***************************************************************************
void CSmxPrnView::SetPrintRect(LPCRECT pRect)
{
  if (pRect == NULL)
  {
    //Use the whole client rect for printing.
    m_rcPrint = m_rcClient;

    if (m_pDC && m_pDC->IsPrinting())
    {
      m_rcPrint.right  -= m_nPaperRightMargin;
      m_rcPrint.bottom -= m_nPaperBottomMargin;
    }
  }
  else
  {
    m_rcPrint = pRect;
    m_nX = m_rcPrint.left;
    m_nY = m_rcPrint.top;
  }
}

//***************************************************************************
// Function         CSmxPrnView::CalcPrintRectHeight
// @mfunc           Calculate the print height needed to print the given 
//                  number of rows. Values for top and bottom margins are added.
// @parm            The number of rows which will be printed.
// @rdesc           The height needed to print the given number of rows.
//***************************************************************************
int CSmxPrnView::CalcPrintRectHeight(UINT nRows)
{
  UpdateFont();
  return GetCurrentLineHeight() * nRows + GetRectTopMargin() + GetRectBottomMargin();
}

//***************************************************************************
// Function         CSmxPrnView::GetPrintRect
// @mfunc           Retriece the current print rectangle.
// @parm            OUT: the current print rectangle.
//***************************************************************************
void CSmxPrnView::GetPrintRect(CRect& rcRect)
{
  if (m_rcPrint.IsRectEmpty())
    rcRect =  m_rcClient;
  else
    rcRect = m_rcPrint;
}

//***************************************************************************
// Function         CSmxPrnView::GetLeftPrintPos
// @mfunc           Retrieve the left printing position.
// @rdesc           The left printing position.
//***************************************************************************
int CSmxPrnView::GetLeftPrintPos()
{
  return m_rcPrint.left;
}

//***************************************************************************
// Function         CSmxPrnView::GetRightPrintPos
// @mfunc           Retrieve the right print position.
// @rdesc           The rightmost print position.
//***************************************************************************
int CSmxPrnView::GetRightPrintPos()
{
  return m_rcPrint.right;
}

//***************************************************************************
// Function         CSmxPrnView::GetTopPrintPos
// @mfunc           Retrieve the topmost print positon.
// @rdesc           The topmost print positon.
//***************************************************************************
int CSmxPrnView::GetTopPrintPos()
{
  return m_rcPrint.top;
}

//***************************************************************************
// Function         CSmxPrnView::GetBottomPrintPos
// @mfunc           Retrieve the bottommost print position.
// @rdesc           The bottommost print position.
//***************************************************************************
int CSmxPrnView::GetBottomPrintPos()
{
  return m_rcPrint.bottom;
}

//***************************************************************************
// Function         CSmxPrnView::GetCurrentLineHeight
// @mfunc           Retrieve the current line height.
// @rdesc           The height of the current line.
//***************************************************************************
int CSmxPrnView::GetCurrentLineHeight()
{
  UpdateFont();
  TEXTMETRIC Metric;
  m_pDC->GetTextMetrics(&Metric);
  return Metric.tmHeight + Metric.tmExternalLeading;
}


//***************************************************************************
// Function         CSmxPrnView::GetLeftMargin
// @mfunc           Return the left margin, that is the margin from the 
//                  printing rectangle to the leftmost positon were text can
//                  be printed.
// @rdesc           The left margin.
//***************************************************************************
int CSmxPrnView::GetLeftMargin()
{
  if (IsClientRect(m_rcPrint))
    return GetClientLeftMargin();
  else
    return GetRectLeftMargin();
}

//***************************************************************************
// Function         CSmxPrnView::GetTopMargin
// @mfunc           Retrieve the top margin, that is the margin from the 
//                  printing rectangle to the topmost positon were text can
//                  be printed.
// @rdesc           The top margin.
//***************************************************************************
int CSmxPrnView::GetTopMargin()
{
  if (IsClientRect(m_rcPrint))
    return GetClientTopMargin();
  else
    return GetRectTopMargin();
}

//***************************************************************************
// Function         CSmxPrnView::GetRightMargin
// @mfunc           Retrieve the right margin for the current printing rectangle.
// @rdesc           The right margin.
//***************************************************************************
int CSmxPrnView::GetRightMargin()
{
  if (IsClientRect(m_rcPrint))
    return GetClientRightMargin();
  else
    return GetRectRightMargin();
}

//***************************************************************************
// Function         CSmxPrnView::GetBottomMargin
// @mfunc           Retrieve the bottm margin of the current printing rectangle.
// @rdesc           The bottom margin.
//***************************************************************************
int CSmxPrnView::GetBottomMargin()
{
  if (IsClientRect(m_rcPrint))
    return GetClientBottomMargin();
  else
    return GetRectBottomMargin();
}


//***************************************************************************
// Function         CSmxPrnView::IsClientRect
// @mfunc           Check if the specified rectangle is the clinet rect.
// @parm            Rect to be checked.
// @rdesc           true if successful, otherwise false.
// @comm Exception  
//***************************************************************************
bool CSmxPrnView::IsClientRect(const CRect& Rect)
{
  return  (Rect == m_rcClient) != FALSE;
}

//***************************************************************************
// Function         CSmxPrnView::PrintMetaFile
// @mfunc           Print a meta file.
// @parm            The resource ID of the binary data which represents an
//                  enhanced meta file.
// @parm            The rectangle were the meta file should be printed.
// @parm            true if the meta file should be enlarged to fit into the 
//                  specified rectangle, false if the meta file should be centered
//                  horizontally and vertically.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CSmxPrnView::PrintMetaFile(UINT nMetaFileID, LPCRECT pRect, bool bEnlarge)
{
  HRSRC    hFound = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nMetaFileID), _T("EMF"));
  DWORD    dwSize = SizeofResource(AfxGetResourceHandle(), hFound);
  HGLOBAL  hRes   = LoadResource(AfxGetResourceHandle(), hFound);
  BYTE    *pBuf   = (BYTE *) LockResource(hRes);

  HENHMETAFILE hMeta = SetEnhMetaFileBits(dwSize, pBuf);

  CRect rcMetaFile = pRect;

  if (!hMeta)
  {
    VERIFY(false);
    //VERIFY -> the given resource is not a valid Meta file!

    return false;
  }

  if (!bEnlarge)
  {
    //We center the meta file without resizing it.
    PENHMETAHEADER pMetaHeader = (PENHMETAHEADER) pBuf;
    CRect rcFrame(pMetaHeader->rclFrame.left,  pMetaHeader->rclFrame.top, 
                  pMetaHeader->rclFrame.right, pMetaHeader->rclFrame.bottom); 
    CSize szSize = rcFrame.Size();
    m_pDC->HIMETRICtoLP(&szSize);

    if (rcMetaFile.Width() > abs(szSize.cx))
    {
      //Center horizontally
      int nDiff = rcMetaFile.Width() - abs(szSize.cx);
      rcMetaFile.left += nDiff / 2;
      rcMetaFile.right = rcMetaFile.left + abs(szSize.cx);
    }

    if (rcMetaFile.Height() > abs(szSize.cy))
    {
      //Center horizontally
      int nDiff = rcMetaFile.Height() - abs(szSize.cy);
      rcMetaFile.top += nDiff / 2;
      rcMetaFile.bottom = rcMetaFile.top + abs(szSize.cy);
    }
  }

  DCtoLP(rcMetaFile);

  if (hMeta)
    m_pDC->PlayMetaFile(hMeta, rcMetaFile);

  UnlockResource(hRes);
  FreeResource(hRes); 

  return true;
}

//***************************************************************************
// Function         CSmxPrnView::PrintFormattedText
// @mfunc           Print text which uses formatted text tags.
// @parm            Text to be printed which can contain text tags. 
//                  <nl> Ex.: "d<sub V>" means that V is printed as subscript.
//                  For available text tags please check <em CSmxPrnView::EFormatFlags>.
// @parm            The text format (check DrawText for available values).
//***************************************************************************
void CSmxPrnView::PrintFormattedText(LPCTSTR pszText, UINT nFormat)
{
  CFormattedTextArray TextArray;
  int                 nBaseLine = 0;
  CRect               rcBounds(m_rcPrint.right, m_rcPrint.bottom, m_rcPrint.left, m_rcPrint.top);
  
  //Split the formatted text into tokens
  SplitFormattedText(TextArray, pszText);
  GetFormattedTextBounds(rcBounds, nBaseLine, TextArray);

  // Calculate the top left point of the bounding rectangle
  CPoint ptMove = GetBoundsTopLeft(rcBounds, nFormat);

  //Move all bounds of the text tokens
  MoveTextTokenBounds(TextArray, ptMove, nBaseLine);

  //Print the text
  PrintTextTokens(TextArray, nFormat);

  //Delete the text tokens
  for (int i = 0; i < TextArray.GetSize(); i++)
  {
    delete TextArray[i];
  }
  TextArray.RemoveAll();

}

//***************************************************************************
// Function         CSmxPrnView::SplitFormattedText
// @mfunc           Split the specified text into an array of text tags.
// @parm            OUT: Array of formatted text tags.
// @parm            The text which should be splitted.
//***************************************************************************
void CSmxPrnView::SplitFormattedText(CFormattedTextArray& TextArray, LPCTSTR pszText)
{
  const TCHAR *pChar  = pszText;
  const TCHAR *pStart = pszText;
  UINT nNofTextTokens = 0;
  CArray<DWORD, DWORD> FormatStack;
  DWORD dwTextFormatFlags = 0;
  CString strText = pszText;
  EFormatFlags eFormatFlags = fmt_None;

  while (*pChar)
  {
    if (*pChar == TCHAR('<') && !(pChar - pszText > 0 && pszText[(pChar - pszText)/sizeof(TCHAR) - 1] == TCHAR('\\')))
    {
      if (FormatStack.GetUpperBound() >= 0)
        dwTextFormatFlags = FormatStack[FormatStack.GetUpperBound()];
      else
        dwTextFormatFlags = NULL;

      //Extract the string
      CreateTextTag(TextArray, strText, pStart - pszText, pChar - pStart, dwTextFormatFlags);

      //This has to ba a format token, get it
      pChar = GetFormatToken(eFormatFlags, pChar);
      pStart = pChar;

      if (pChar == NULL)
        break;

      //Push it on the stack
      dwTextFormatFlags |= eFormatFlags;
      FormatStack.Add(dwTextFormatFlags);
      
      //dont't move the pChar forward, we would skip the next character which could be an '<' or '>'
      continue;
    }
    else if (*pChar == TCHAR('>') && pChar - pszText > 0 && pszText[(pChar - pszText)/sizeof(TCHAR) - 1] != TCHAR('\\'))
    {
      // We have got a string here, check if the format token stack is not empty
      if (FormatStack.GetUpperBound() >= 0)
      {
        dwTextFormatFlags = FormatStack[FormatStack.GetUpperBound()];

        //Extract the string
        CreateTextTag(TextArray, strText, pStart - pszText, pChar - pStart, dwTextFormatFlags);

        //Remove the last entry from the stack
        FormatStack.RemoveAt(FormatStack.GetUpperBound());

        pStart = pChar + 1;
      }
      else
      {
        ASSERT(false);
        //ASSERT -> '>' is a special character, you have to mask it with '\\'!
      }
    }

    pChar++;
  }

  //Extract the string
  CreateTextTag(TextArray, strText, pStart - pszText, pChar - pStart, 0);
}


//***************************************************************************
// Function         CSmxPrnView::GetFormatToken
// @mfunc           Retrieve the format token.
// @parm            OUT: the enum value of the format token.
// @parm            The string which has to begin with a format token.
// @rdesc           The positon of the first nonwhite character after the format 
//                  token.
//***************************************************************************
const TCHAR *CSmxPrnView::GetFormatToken(EFormatFlags& eFormatFlags, LPCTSTR pszChar)
{
  const TCHAR *pChar = pszChar + 1;
  
  if (_tcsncmp(pChar, sm_szBold, _tcslen(sm_szBold)) == 0)
  {
    eFormatFlags = fmt_Bold;
    pChar += _tcslen(sm_szBold);
  } 
  else if (_tcsncmp(pChar, sm_szSub, _tcslen(sm_szSub)) == 0)
  {
    eFormatFlags = fmt_Sub;
    pChar += _tcslen(sm_szSub);
  }
  else if (_tcsncmp(pChar, sm_szSymb, _tcslen(sm_szSymb)) == 0)
  {
    eFormatFlags = fmt_Symb;
    pChar += _tcslen(sm_szSymb);
  }
  else if (_tcsncmp(pChar, sm_szUnicode, _tcslen(sm_szUnicode)) == 0)
  {
    eFormatFlags = fmt_Unicode;
    pChar += _tcslen(sm_szUnicode);
  }

  //Skip the white characters
  while (*pChar != NULL && _istspace(*pChar))
    pChar++;

  return pChar;
}


//***************************************************************************
// Function         CSmxPrnView::CreateTextTag
// @mfunc           Create a text tag.
// @parm            OUT: array were the newly created tag will be appended,
// @parm            The input string.
// @parm            The start positon for the text tag.
// @parm            The number of characters from the starting positon.
// @parm            The text format flags.
//***************************************************************************
void CSmxPrnView::CreateTextTag(CFormattedTextArray& TextArray, const CString& strText, UINT nStart, UINT nCount, 
                             DWORD dwTextFormatFlags)
{
  LPWSTR pwszText = NULL;

  if (nCount <= 0)
    return;

  CFormattedTextTag *pTextTag = new CFormattedTextTag;
  pTextTag->m_strText = strText.Mid(nStart, nCount);

  if (dwTextFormatFlags & fmt_Bold || dwTextFormatFlags & fmt_Sub || dwTextFormatFlags & fmt_Symb)
  {
    //The font will change, create the logfont structure
    pTextTag->m_pLogFont = new LOGFONT;
    GetLogFont(pTextTag->m_pLogFont);
  }

  if (dwTextFormatFlags & fmt_Bold)
  {
    ASSERT(pTextTag->m_pLogFont);
    pTextTag->m_pLogFont->lfWeight = FW_BOLD;
  }

  if (dwTextFormatFlags & fmt_Sub)
  {
    ASSERT(pTextTag->m_pLogFont);
    pTextTag->m_pLogFont->lfHeight = pTextTag->m_pLogFont->lfHeight * 80 / 100;
  }

  if (dwTextFormatFlags & fmt_Symb)
  {
    ASSERT(pTextTag->m_pLogFont);
    _tcscpy(pTextTag->m_pLogFont->lfFaceName, sm_szSymbol);
  }

  if (dwTextFormatFlags & fmt_Unicode)
  {
    int nSize = sizeof(TCHAR) * pTextTag->m_strText.GetLength();
    pwszText  = new WCHAR[nSize / 2 + 1];
    memcpy((void *) pwszText, (void *) ((LPCTSTR) pTextTag->m_strText), nSize);
    pwszText[nSize/2] = 0;
  }

  //Set the font which is needed for calculating the text bounds
  LOGFONT LogFont;
  if (pTextTag->m_pLogFont)
  {
    GetLogFont(&LogFont);
    SetFont(pTextTag->m_pLogFont);
  }
  UpdateFont();
  
  //Remember the descent which will be used to allign the text at the base line
  TEXTMETRIC tm;
  m_pDC->GetTextMetrics(&tm);
  pTextTag->m_tmDescent = tm.tmDescent;

  pTextTag->m_dwFormatFlags = dwTextFormatFlags;

  //Calculate the bounding rectangle
  CRect rcText(m_nX, m_nY, m_nX, m_nY);
  LPtoDC(rcText);
  if (dwTextFormatFlags & fmt_Unicode)
  {
    ::DrawTextW(m_pDC->GetSafeHdc(), pwszText, wcslen(pwszText), rcText, DT_CALCRECT | DT_LEFT | DT_SINGLELINE);
  }
  else
  {
    m_pDC->DrawText(pTextTag->m_strText, rcText, DT_CALCRECT | DT_LEFT | DT_SINGLELINE);
  }
  
  DCtoLP(rcText);

  //Reset the text font
  if (pTextTag->m_pLogFont)
  {
    SetFont(&LogFont);
  }

  m_nX += rcText.Width();

  pTextTag->m_rcBounds = rcText;

  TextArray.Add(pTextTag);

  if (pwszText)
    delete [] pwszText;
}


//***************************************************************************
// Function         CSmxPrnView::GetFormattedTextBounds
// @mfunc           Retrieve the bounding rectangle for all formatted text tags 
//                  from the specified array.
// @parm            OUT: the bounding rectangle.
// @parm            OUT: the base line were the all printed text should be alligned vertically.
// @parm            The array with text tags.
//***************************************************************************
void CSmxPrnView::GetFormattedTextBounds(CRect& rcBounds, int& nBaseLine, const CFormattedTextArray& TextArray)
{
  LONG tmMaxDescent = 0;
  for (int i = 0; i < TextArray.GetSize(); i++)
  {
    rcBounds.left    = min(rcBounds.left, TextArray[i]->m_rcBounds.left);
    rcBounds.top     = min(rcBounds.top, TextArray[i]->m_rcBounds.top);
    rcBounds.right   = max(rcBounds.right, TextArray[i]->m_rcBounds.right);
    rcBounds.bottom  = max(rcBounds.bottom, TextArray[i]->m_rcBounds.bottom);
    tmMaxDescent     = max(tmMaxDescent, TextArray[i]->m_tmDescent);
  }
  nBaseLine = rcBounds.bottom - tmMaxDescent;
}

//***************************************************************************
// Function         CSmxPrnView::GetBoundsTopLeft
// @mfunc           Retrieve the top left point for alligning the specified 
//                  rectangle in the current print rectangle.
// @parm            The rectangle which should be alligned.
// @parm            Format flags (check DrawText) which are used.
// @rdesc           The TopLeft point were the specified rectangle should be 
//                  moved in order to be alligned as specified in the format 
//                  flags.
//***************************************************************************
CPoint CSmxPrnView::GetBoundsTopLeft(const CRect& rcBounds, UINT nFormat)
{
  CPoint ptMove(0, 0);
  if (nFormat & DT_RIGHT)
  {
    //The text should be right alligned in the printing rectangle, move the rectangle.
    ptMove.x = GetRightPrintPos() - GetRightMargin() - rcBounds.right;
  }
  else if (nFormat & DT_CENTER)
  {
    //The text should be centered in the printing rectangle, move the rectangle accordingly.
    ptMove.x = - (rcBounds.left - m_rcPrint.left - (m_rcPrint.Width() - rcBounds.Width()) / 2);
  }
  else
  {
    if (rcBounds.left < GetLeftPrintPos() + GetLeftMargin())
      ptMove.x = GetLeftPrintPos() + GetLeftMargin() - rcBounds.left;
  }

  if (nFormat & DT_VCENTER)
  {
    //The text should be centered vertically in the printing rectangle, move the rectangle accordingly.
    ptMove.y = GetTopPrintPos() + (GetBottomPrintPos() - GetTopPrintPos()  - rcBounds.Height()) / 2 - rcBounds.top;
    //ptMove.y = - (rcBounds.top - m_rcPrint.top - (m_rcPrint.Height() - rcBounds.Height()) / 2);
  }

  return ptMove;
}

//***************************************************************************
// Function         CSmxPrnView::MoveTextTokenBounds
// @mfunc           Move the test tokens.
// @parm            Array containing the text toke´ns.
// @parm            Point for moving the text token bounds.
// @parm            The base line for alligning the test vertically.
//***************************************************************************
void CSmxPrnView::MoveTextTokenBounds(CFormattedTextArray& TextArray, const CPoint& ptMove, int nBaseLineY)
{
  CPoint ptTemp;
  for (int i = 0; i < TextArray.GetSize(); i++)
  {
    ptTemp.x = ptMove.x;
    ptTemp.y = ptMove.y + nBaseLineY - TextArray[i]->m_rcBounds.bottom + TextArray[i]->m_tmDescent;
    
    TextArray[i]->m_rcBounds = TextArray[i]->m_rcBounds + ptTemp;
  }
}

//***************************************************************************
// Function         CSmxPrnView::PrintTextTokens
// @mfunc           Print the text tokens.
// @parm            Array with text tokens.
// @parm            The format of printing with DrawText.
//***************************************************************************
void CSmxPrnView::PrintTextTokens(const CFormattedTextArray& TextArray, UINT nFormat)
{
  LOGFONT LogFont;
  CRect   rcText;
  LPWSTR  pwszText = NULL;

  UNREFERENCED_PARAMETER(nFormat);                  //The format is actually handled before calling PrintTextTokens
  for (int i = 0; i < TextArray.GetSize(); i++)
  {
    if (TextArray[i]->m_pLogFont)
    {
      GetLogFont(&LogFont);
      SetFont(TextArray[i]->m_pLogFont);
    }
    UpdateFont();

    if (TextArray[i]->m_dwFormatFlags & fmt_Unicode)
    {
      int nSize = sizeof(TCHAR) * TextArray[i]->m_strText.GetLength();
      pwszText  = new WCHAR[nSize / 2 + 1];
      memcpy((void *) pwszText, (void *) ((LPCTSTR) TextArray[i]->m_strText), nSize);
      pwszText[nSize/2] = 0;
    }

    rcText = TextArray[i]->m_rcBounds;

    if (TextArray[i]->m_dwFormatFlags & fmt_Sub)
    {
      //Lower the rectangle with one point
      CPoint pt(0, PointsToLP(1));
      rcText += pt;
    }

    LPtoDC(rcText);

    if (TextArray[i]->m_dwFormatFlags & fmt_Unicode)
    {
      ::DrawTextW(m_pDC->GetSafeHdc(), pwszText, wcslen(pwszText), rcText, DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
    }
    else
    {
      m_pDC->DrawText(TextArray[i]->m_strText, rcText, DT_LEFT | DT_SINGLELINE | DT_BOTTOM);
    }

    if (TextArray[i]->m_pLogFont)
    {
      SetFont(&LogFont);
    }

    if (pwszText)
    {
      delete[] pwszText;
      pwszText = NULL;
    }
  }
}


//***************************************************************************
// Function         CSmxPrnView::SetTextColor
// @mfunc           Set the text color.
// @parm            The text color to be set.
// @rdesc           None
//***************************************************************************
COLORREF CSmxPrnView::SetTextColor(COLORREF crColor)
{
  return GetDC()->SetTextColor(crColor);
}

//***************************************************************************
// Function         CSmxPrnView::GetUnicodeSymbol
// @mfunc           Return a string containing the specified unicode symbol.
// @parm            The unicode symbol.
// @rdesc           String reference containing the specified unicode symbol.
//***************************************************************************
CString& CSmxPrnView::GetUnicodeSymbol(EUnicodeSymbol eSymbol)
{
#ifdef _UNICODE
  m_strSymbol = (TCHAR) eSymbol;
#else
  BYTE buf[sizeof(short) + 1];
  short nSymbol = (short) eSymbol;
  memcpy(buf, &nSymbol, sizeof(short));
  buf[sizeof(short)] = 0;
  m_strSymbol.Format("<%s ", sm_szUnicode);
  m_strSymbol += (const char *) buf;
  m_strSymbol += '>';
#endif
  return m_strSymbol;
}

//***************************************************************************
// Function         CSmxPrnView::GetDiameter
// @mfunc           Return a string containing the diameter symbol.
// @rdesc           A reference to a string containing the diameter symbol.
//***************************************************************************
CString& CSmxPrnView::GetDiameter()
{ 
  m_strSymbol = (TCHAR) sy_Diameter;
  return m_strSymbol;
}

void CSmxPrnView::OnPrepareDC( CDC* pDC, CPrintInfo* pInfo)
{
  CScrollView::OnPrepareDC(pDC, pInfo);

  if (UseForPrintOnly() && !pDC->IsPrinting())
  {
    return;
  }

  CDC *pPrevDC = m_pDC;
  m_pDC = pDC;

  CPoint ptOrg(-GetScrollPos(SB_HORZ), -GetScrollPos(SB_VERT));
	pDC->SetViewportOrg(ptOrg);

  
  m_nX = 0;
  m_nY = 0;
  pDC->SetMapMode(m_nMapMode);

  GetDefaultFont(&m_LogFont);
  GetDefaultPen(&m_LogPen);
  GetDefaultBrush(&m_LogBrush);
  
  if (!pDC->IsPrinting ())
  {
    CPoint ptOrg = pDC->GetViewportOrg();
    GetClientRect(m_rcPrint);
    GetClientRect(m_rcClient);
    m_rcPrint += ptOrg;
    m_rcClient += ptOrg;
    m_pDC->DPtoLP(m_rcPrint);
    m_pDC->DPtoLP(m_rcClient);
    DCtoLP(m_rcPrint);
    DCtoLP(m_rcClient);
  }
  else
  {
		m_rcClient.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		pDC->DPtoLP(&m_rcClient);
    DCtoLP(m_rcClient);

    CSize szOffset(pDC->GetDeviceCaps(PHYSICALOFFSETX), pDC->GetDeviceCaps(PHYSICALOFFSETY));
    pDC->DPtoLP(&szOffset);
  
    CPoint ptWindowOrg(0, 0);
    ptWindowOrg.x -= m_nPaperLeftMargin - szOffset.cx;
    ptWindowOrg.y -= m_nPaperTopMargin  - szOffset.cy;
    LPtoDC (ptWindowOrg);
    pDC->SetWindowOrg (ptWindowOrg);
  }

  SetPrintRect(NULL);
  m_pDC = pPrevDC;
}

void CSmxPrnView::OnInitialUpdate()
{
  CScrollView::OnInitialUpdate();
  EnableScrollBarCtrl(SB_VERT, TRUE);
  CDC dc;
  dc.CreateCompatibleDC(NULL);
  
  SetScrollSizes(MM_HIMETRIC, CSize(10000, 10000));

  CDC *pPrevDC = m_pDC;
  m_pDC = &dc;
  OnPrepareDC(&dc);
  m_bCalcNofPages = true;
  OnDraw(&dc);
  m_bCalcNofPages = false;
  
  if (m_nX > GetLeftPrintPos())
    PrintEOL();

  SetScrollSizes(m_nMapMode, CSize(m_nMaxX + m_rcClient.left, m_nMaxY + m_rcClient.top));
  m_pDC = pPrevDC;
}

void CSmxPrnView::PrintValue(double dbValue, UINT nFormat, WORD nPrecision, WORD nGrpNumber, EPrnFlags ePrnFormat)
{
  //CFormEdit FormEdit;
  //FormEdit.SetDeciNumber(nPrecision);
  //FormEdit.DoubleToString(strValue, dbValue);
  PrintText(Double2String(dbValue, nPrecision, nGrpNumber), nFormat, ePrnFormat);
}

void CSmxPrnView::PrintCurrentDate(UINT nFormat, EPrnFlags ePrnFlags)
{
  PrintText(CTime::GetCurrentTime().Format("%x"), nFormat, ePrnFlags);
}

BOOL CSmxPrnView::OnPreparePrinting(CPrintInfo* pInfo) 
{
  if (UseForPrintOnly() && CalculateNofPagesInOnDraw())
  {
    //Let him print itself to calculate the number of pages
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CDC *pPrevDC = m_pDC;
    m_pDC = &dc;
    dc.m_bPrinting = TRUE;
    OnPrepareDC(&dc);
    m_bCalcNofPages = true;
    OnDraw(&dc);
    m_bCalcNofPages = false;
    m_pDC = pPrevDC;
  
  }

  pInfo->SetMaxPage(m_nNofPages);
	return DoPreparePrinting(pInfo);
}

void CSmxPrnView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  int     nScrollMin;
  int     nScrollMax;
  int     nScrollInc = 0;
  int nScrollPos = GetScrollPos (SB_VERT);

  GetScrollRange (SB_VERT, &nScrollMin, &nScrollMax);

  switch(nSBCode)
  {
    case SB_BOTTOM:
      nScrollInc = nScrollMax - nScrollPos;
      break;
    case SB_ENDSCROLL:
      break;
    case SB_LINEDOWN:
      nScrollInc = 100;
      break;
    case SB_LINEUP:
      nScrollInc = -100;
      break;
    case SB_PAGEDOWN:
      nScrollInc = 1000;
      break;
    case SB_PAGEUP:
      nScrollInc = -1000;
      break;
    case SB_THUMBPOSITION:
      break;
    case SB_THUMBTRACK:
      break;
    case SB_TOP:
      break;
  }

  if (nScrollPos + nScrollInc > nScrollMax)
    nScrollInc = nScrollMax - nScrollPos;

  if (nScrollPos + nScrollInc < nScrollMin)
    nScrollInc = -(nScrollPos - nScrollMin);

  if (nScrollInc != 0)
  {
    /* do the scrolling */
    ScrollWindow (0, -nScrollInc, NULL, NULL);
    SetScrollPos (SB_VERT, nScrollPos + nScrollInc, TRUE);

    UpdateWindow ();
  }
}

void CSmxPrnView::DoScroll(int nScroll)
{
  return;
}


void CSmxPrnView::PrintTable(CSmxPrnTable& PrnTable)
{
  UINT nRow = 0;
  UINT nPage = 0;

  PrintTableHeader(PrnTable, nPage);
  while(PrintTableRow(PrnTable, nPage, nRow))
  {
    nRow++;
  }
  PrintTableFooter(PrnTable, nPage);
}

int CSmxPrnView::GetPaperHeight()
{
  CDC dc;
  static int nPaperHeight = 0;

  if (nPaperHeight == 0)
  {
    AfxGetApp()->CreatePrinterDC(dc);
    dc.SetMapMode(m_nMapMode);
    CSize Extent = dc.GetWindowExt();
    nPaperHeight =  Extent.cy;
  }
  return nPaperHeight;
}

void CSmxPrnView::OnDraw(CDC* pDC)
{
  if (UseForPrintOnly() && !pDC->IsPrinting())
  {
    CScrollView::OnDraw(pDC);
    return;
  }
  
  CDC *pPrevDC = m_pDC;
  m_pDC = pDC;
  Draw();
  m_pDC = pPrevDC;
}

//***************************************************************************
// Function         CSmxPrnView::LPtoDC
// @mfunc           Change the y direction for drawing operations.
// @parm            Point.
//***************************************************************************
void CSmxPrnView::LPtoDC(CPoint& Point)
{
  if (m_pDC->GetMapMode () != MM_TEXT)
  {
    Point.y = -Point.y;
  }
}

//***************************************************************************
// Function         CSmxPrnView::LPtoDC
// @mfunc           Chenge the y direction for drawing operations.
// @parm            Rectangle .
//***************************************************************************
void CSmxPrnView::LPtoDC(CRect& Rect)
{
  if (m_pDC->GetMapMode () != MM_TEXT)
  {
    Rect.top = -Rect.top;
    Rect.bottom = -Rect.bottom;
  }
}

//***************************************************************************
// Function         CSmxPrnView::LPtoDC_y
// @mfunc           Change the y direction for drawing operations.
// @parm            The y coordinate.
// @rdesc           The changed y value.
//***************************************************************************
int CSmxPrnView::LPtoDC_y(int nY)
{
  if (m_pDC->GetMapMode () != MM_TEXT)
  {
    return -nY;
  }
  return nY;
}

//***************************************************************************
// Function         CSmxPrnView::DCtoLP
// @mfunc           Change the y direction for drawing operations.
// @parm            Point.
//***************************************************************************
void CSmxPrnView::DCtoLP(CPoint& Point)
{
  if (m_pDC->GetMapMode () != MM_TEXT)
  {
    Point.y = -Point.y;
  }
}

//***************************************************************************
// Function         CSmxPrnView::DCtoLP
// @mfunc           Change the y direction for drawing operations.
// @parm            Rect.
//***************************************************************************
void CSmxPrnView::DCtoLP(CRect& Rect)
{
  if (m_pDC->GetMapMode () != MM_TEXT)
  {
    Rect.top = -Rect.top;
    Rect.bottom = -Rect.bottom;
  }
}

//***************************************************************************
// Function         CSmxPrnView::DCtoLP_y
// @mfunc           Change the y direction for drawing operations.
// @parm            The y value.
// @rdesc           THe changed y value.
//***************************************************************************
int CSmxPrnView::DCtoLP_y(int nY)
{
  if (m_pDC->GetMapMode () != MM_TEXT)
  {
    return -nY;
  }
  return nY;
}

//***************************************************************************
// Function         CSmxPrnView::LPtoHiMetric
// @mfunc           Convert logical points into himetric points.
// @parm            IN/OUT: Rectangle
//***************************************************************************
void CSmxPrnView::LPtoHiMetric(CRect *pRect)
{
  ASSERT(m_pDC->GetWindowOrg() == CPoint(0, 0));
  //GPMASSERT -> If the window origin is not at zero, we do not know if the 
  //             window origin should be changed in MM_HIMETRIC coordinates as well!!
  m_pDC->LPtoDP(pRect);
  int nMode = m_pDC->SetMapMode(MM_HIMETRIC);
  m_pDC->DPtoLP(pRect);

  m_pDC->SetMapMode(nMode);
}

//***************************************************************************
// Function         CSmxPrnView::TwipsToLP
// @mfunc           Convert twips into logical points.
// @parm            The value of twips.
// @rdesc           The value of logical points.
//***************************************************************************
int CSmxPrnView::TwipsToLP(int nTwips) const 
{
  if (m_pDC->GetMapMode() == MM_TWIPS)
    return nTwips;
  else
  {
    return PointsToLP(nTwips / 20.0);
  }
}

//***************************************************************************
// Function         CSmxPrnView::PointsToLP
// @mfunc           Convert a value of points (1/72 inch) into a value of logical points.
// @parm            The value of points to be converted.
// @rdesc           The convertes number of logical points.
//***************************************************************************
int CSmxPrnView::PointsToLP(double dbPoints) const
{
  CSize Size(0, (int) (dbPoints * m_pDC->GetDeviceCaps(LOGPIXELSY) / 72));
  m_pDC->DPtoLP(&Size);
  return Size.cy;
}


