/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SmxPrnTable.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SmxPrnTable.cpp,v 1.2 2002/11/22 17:23:17 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "StdAfx.h"
#include "SmxPrnView.h"
#include "SmxPrnTable.h"

#ifndef CScrollView
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif

//***************************************************************************
// Function         CSmxPrnTable::CSmxPrnTable
// @mfunc           Constructor.
// @parm            The owning print  module.
//***************************************************************************
CSmxPrnTable::CSmxPrnTable(CSmxPrnView *pModule) 
{ 
  m_pModule            = pModule; 
  m_pColumns           = 0;
  m_nNofColumns        = 0;
  m_bMarginsSet        = false;
  m_nLeftColMargin     = m_pModule->PointsToLP(3);
  m_nRightColMargin    = m_pModule->PointsToLP(3);
  m_nDefaultRowHeight  = m_pModule->CalcPrintRectHeight(1);
  m_nDefaultTxtFormat  = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
  m_nTableBottomMargin = 0;
  m_nTableTopMargin    = 0;
  m_pModule->GetPrintRect(m_rcTable);
  m_rcTable.top    = m_pModule->m_nY;
  m_rcTable.bottom = m_rcTable.top + m_nDefaultRowHeight;
}

//***************************************************************************
// Function         CSmxPrnTable::~CSmxPrnTable
// @mfunc           Destructor.
//***************************************************************************
CSmxPrnTable::~CSmxPrnTable() 
{
  if (m_pColumns)
    delete m_pColumns;

  if (m_bMarginsSet)
  {
    ASSERT(false);
    //ASSERT -> you should call ResetPrintRect() after using the print cell (SetPrintCell() was called
    //             without a corresponding ResetPrintRect()!! )
    ResetPrintRect();
  }
}

//***************************************************************************
// Function         CSmxPrnTable::SetRowHeight
// @mfunc           Set the row height.
// @parm            The row height to be set.
// @rdesc           Reference to the table.
//***************************************************************************
CSmxPrnTable& CSmxPrnTable::SetRowHeight(UINT nHeight, UINT nRow)
{
  if (m_aRowHeight.GetSize() <= (int) nRow)
  {
    int nPrevSize = m_aRowHeight.GetSize();
    m_aRowHeight.SetSize(nRow + 1);
    for (int i = nPrevSize; i < m_aRowHeight.GetSize(); i++)
    {
      m_aRowHeight[i] = m_nDefaultRowHeight;
    }
  }

  if (nHeight > m_aRowHeight[nRow])
    m_aRowHeight[nRow] = nHeight;
  return *this;
}

CSmxPrnTable& CSmxPrnTable::SetDefaultRowHeight(UINT nHeight)
{
  m_nDefaultRowHeight = nHeight;
  return *this;
}

//***************************************************************************
// Function         CSmxPrnTable::SetColumns
// @mfunc           Set the number ans width of columns.
// @parm            Number of columns in the table.
// @parm            Number of values specifying column width. This number can 
//                  be smaller than the number of columns in which case the remaining
//                  width is split up equally for the remaining columns.
// @parm            Array with values for the column width.
// @rdesc           Reference to the table.
//***************************************************************************
CSmxPrnTable& CSmxPrnTable::SetColumns(UINT nNofColumns, UINT nNofValues, UINT *pValues)
{
  ASSERT(nNofColumns >= nNofValues);

  if (m_nNofColumns < nNofColumns)
  {
    if (m_pColumns)
      delete [] m_pColumns;
    m_pColumns = new UINT[nNofColumns];
  }

  UINT nSumWidth = 0;
  for (UINT i = 0; i < nNofValues; i++)
  {
    m_pColumns[i] = pValues[i];
    nSumWidth += pValues[i];
  }

  ASSERT(nSumWidth <= (UINT) m_rcTable.Width());
  //ASSERT -> The sum of the column width is greater than the table width??

  if (nSumWidth < (UINT) m_rcTable.Width())
  {
    if (nNofColumns > nNofValues)
    {
      // The user did not specify all column width, set the remaining column
      // width by splitting the remaining width into equal parts.
      UINT nWidth = (m_rcTable.Width() - nSumWidth) / (nNofColumns - nNofValues);
      for (i = nNofValues; i < nNofColumns - 1; i++)
      {
        m_pColumns[i] = nWidth;
      }
      
      //Set the last column width to the remaining width
      m_pColumns[nNofColumns - 1] = m_rcTable.Width() - nSumWidth - nWidth * (nNofColumns - nNofValues - 1);

    }
  }

  m_nNofColumns = nNofColumns;

  return *this;
}

//***************************************************************************
// Function         CSmxPrnTable::SetLeftColMargin
// @mfunc           Set the left column margin.
// @parm            The left column margin.
// @rdesc           Reference to the table.
//***************************************************************************
CSmxPrnTable& CSmxPrnTable::SetLeftColMargin(UINT nLeftColMargin)
{
  m_nLeftColMargin = nLeftColMargin;
  return *this;
}

//***************************************************************************
// Function         CSmxPrnTable::SetRightColMargin
// @mfunc           Set the right column margin.
// @parm            The right colulmn margin.
// @rdesc           Reference to the table.
//***************************************************************************
CSmxPrnTable& CSmxPrnTable::SetRightColMargin(UINT nRightColMargin)
{
  m_nRightColMargin = nRightColMargin;
  return *this;
}

//***************************************************************************
// Function         CSmxPrnTable::SetNofRows
// @mfunc           Set the number of rows.
// @parm            The number of rows for the table.
// @rdesc           Reference to the table.
//***************************************************************************
CSmxPrnTable& CSmxPrnTable::SetNofRows(UINT nNofRows)
{
  m_nNofRows       = nNofRows;
  m_bNofRowsSet    = true;
  m_rcTable.bottom = GetRowBottom(nNofRows - 1) + m_nTableBottomMargin;
  return *this;
}


//***************************************************************************
// Function         CSmxPrnTable::PrintFrame
// @mfunc           Print a table frame with the current pen.
// @parm            Frame flags.
//***************************************************************************
void CSmxPrnTable::PrintFrame(UINT nFlags)
{
  m_pModule->SetPrintRect(m_rcTable);
  m_pModule->FramePrintRect(nFlags);
  LOGPEN LogPen;
  m_pModule->GetLogPen(&LogPen);

  if (nFlags & FRM_BOTTOM)
    m_rcTable.bottom += LogPen.lopnWidth.x;
}

//***************************************************************************
// Function         CSmxPrnTable::PrintHLines
// @mfunc           Print horizontal lines between all table rows.
//***************************************************************************
void CSmxPrnTable::PrintHLines()
{
  for (UINT i = 0; i < m_nNofRows - 1; i++)
  {
    PrintHLine(i);
  }
}

//***************************************************************************
// Function         CSmxPrnTable::PrintVLines
// @mfunc           Print vertical lines between all table columns.
//***************************************************************************
void CSmxPrnTable::PrintVLines()
{
  for (UINT i = 0; i < m_nNofColumns - 1; i++)
  {
    PrintVLine(i);
  }
}

//***************************************************************************
// Function         CSmxPrnTable::PrintHLine
// @mfunc           Print a horizontal line at the bottom of the specified row.
// @parm            The row number.
//***************************************************************************
void CSmxPrnTable::PrintHLine(UINT nRow)
{
  int nY = GetRowBottom(nRow);

  if (m_pModule->m_LogPen.lopnWidth.x == 0)
    m_pModule->SetPenWidth(1);
  m_pModule->UpdatePen();

  if (nRow == m_nNofRows - 1)
  {
    nY += m_nTableBottomMargin;
    m_rcTable.bottom += m_pModule->m_LogPen.lopnWidth.x;
  }
  else
  {
    //Enlarge the row height with the size of the line
    int nRowHeight = m_nDefaultRowHeight;
    if ((int) nRow < m_aRowHeight.GetSize())
      nRowHeight = m_aRowHeight[nRow];

    nRowHeight += m_pModule->m_LogPen.lopnWidth.x;
    SetRowHeight(nRowHeight, nRow);
  }

  m_pModule->MoveTo(m_rcTable.left, nY);
  m_pModule->m_pDC->LineTo(m_rcTable.right, m_pModule->LPtoDC_y(nY));
}

//***************************************************************************
// Function         CSmxPrnTable::PrintHLine
// @mfunc           Print a horizontal line at the bottom of the specified row 
//                  between the specified columns.
// @parm            The row number.
// @parm            Starting column number.
// @parm            Ending number of the 
//***************************************************************************
void CSmxPrnTable::PrintHLine(UINT nRow, UINT nColFrom, UINT nColTo)
{
  int nY = GetRowBottom(nRow);
  m_pModule->UpdatePen();

  if (nRow == m_nNofRows - 1)
  {
    nY += m_nTableBottomMargin;
    m_rcTable.bottom += m_pModule->m_LogPen.lopnWidth.x;
  }
  else
    nY -= m_pModule->m_LogPen.lopnWidth.y / 2;

  m_pModule->MoveTo(GetColLeft(nColFrom), nY);
  m_pModule->m_pDC->LineTo(GetColRight(nColTo), m_pModule->LPtoDC_y(nY));
}

//***************************************************************************
// Function         CSmxPrnTable::PrintVLine
// @mfunc           Print a vertical line at the right of the given column.
// @parm            Column number.
//***************************************************************************
void CSmxPrnTable::PrintVLine(UINT nCol)
{
  int nX = GetColRight(nCol) - m_pModule->m_LogPen.lopnWidth.x / 2;
  m_pModule->UpdatePen();
  m_pModule->MoveTo(nX, m_rcTable.top);
  m_pModule->GetDC()->LineTo(nX, m_pModule->LPtoDC_y(m_rcTable.bottom));
}

//***************************************************************************
// Function         CSmxPrnTable::PrintVLine
// @mfunc           Print a vertical line at the right of the specified column 
//                  between the given rows.
// @parm            The column number.
// @parm            The starting row.
// @parm            The ending row.
//***************************************************************************
void CSmxPrnTable::PrintVLine(UINT nCol, UINT nRowFrom, UINT nRowTo)
{
  int nX = GetColRight(nCol) - m_pModule->m_LogPen.lopnWidth.x / 2;
  m_pModule->UpdatePen();
  m_pModule->MoveTo(nX, GetRowTop(nRowFrom));
  m_pModule->m_pDC->LineTo(nX, m_pModule->LPtoDC_y(GetRowBottom(nRowTo)));
}

//***************************************************************************
// Function         CSmxPrnTable::GetColLeft
// @mfunc           Retrieve the left border position for the specified column.
// @parm            The column number.
// @rdesc           The left border position for the specified column
//***************************************************************************
int CSmxPrnTable::GetColLeft(UINT nCol)
{
  ASSERT(nCol < m_nNofColumns);

  int nX = m_rcTable.left;
  
  for (UINT i = 0; i < nCol; i++)
  {
    nX += m_pColumns[i];
  }
  return nX;
}

//***************************************************************************
// Function         CSmxPrnTable::GetColRight
// @mfunc           Retrieve the right border position for the specified column.
// @parm            The column number.
// @rdesc           The righr border position.
// @comm Exception  
//***************************************************************************
int CSmxPrnTable::GetColRight(UINT nCol)
{
  ASSERT(nCol < m_nNofColumns);

  int nX = m_rcTable.left;
  
  for (UINT i = 0; i <= nCol; i++)
  {
    nX += m_pColumns[i];
  }
  return nX;
}

//***************************************************************************
// Function         CSmxPrnTable::GetRowTop
// @mfunc           Retrieve the top border position for the given row.
// @parm            The row number.
// @rdesc           The top border position.
//***************************************************************************
int CSmxPrnTable::GetRowTop(UINT nRow)
{
  CheckRow(nRow);

  return m_nTableTopMargin + m_rcTable.top + CalcTableRowHeight(0, nRow - 1);
}

int CSmxPrnTable::CalcTableRowHeight(int nStartRow, int nEndRow)
{
  int nHeight = 0;
  for (int i = nStartRow; i <= nEndRow; i++)
  {
    if (i < m_aRowHeight.GetSize())
      nHeight += m_aRowHeight[i];
    else
      nHeight += m_nDefaultRowHeight;
  }
  return nHeight;
}

//***************************************************************************
// Function         CSmxPrnTable::GetRowBottom
// @mfunc           Retrieve the bottom border position for the specified row.
// @parm            The row number.
// @rdesc           The bottom border positon.
//***************************************************************************
int CSmxPrnTable::GetRowBottom(UINT nRow)
{
  CheckRow(nRow);
  UINT nBottomMargin = 0;

  return m_nTableTopMargin + nBottomMargin +m_rcTable.top + CalcTableRowHeight(0, nRow);
}

//***************************************************************************
// Function         CSmxPrnTable::GetCellRect
// @mfunc           Retrieve the rectangle for the given cell.
// @parm            OUT: the cells rectangle.
// @parm            The row number of the cell.
// @parm            The column number of the cell.
//***************************************************************************
void CSmxPrnTable::GetCellRect(CRect& rcCell, UINT nRow, UINT nCol)
{
  ASSERT(nCol < m_nNofColumns);
  CheckRow(nRow);

  rcCell.top    = GetRowTop(nRow);
  rcCell.bottom = GetRowBottom(nRow);
  rcCell.left   = GetColLeft(nCol);
  rcCell.right  = GetColRight(nCol);

}

//***************************************************************************
// Function         CSmxPrnTable::CheckRow
// @mfunc           Check if the given row number is a valid row number.
// @parm            The row number which should be checked.
//***************************************************************************
void CSmxPrnTable::CheckRow(UINT nRow)
{
  if (!m_bNofRowsSet)
  {
    if (nRow >= m_nNofRows)
      m_nNofRows = nRow;
  }
  else
  {
    ASSERT(nRow < m_nNofRows);
  }
}

//Print a text over more than one column
void CSmxPrnTable::PrintText(UINT nRow, UINT nColStart, UINT nColEnd, LPCTSTR pszText, 
                             UINT nFormat, CSmxPrnView::EPrnFlags ePrnFlags)
{
  SetPrintCell(nRow, nColStart, nColEnd);
  nFormat |=  m_nDefaultTxtFormat;

  if (pszText == NULL)
    return;

  if ((ePrnFlags & CSmxPrnView::PRN_FORMATTED) == 0)
  {
    //Print the text
    CRect rcText(m_pModule->m_nX, m_pModule->m_nY, m_pModule->m_nX, m_pModule->m_nY);
    m_pModule->CalcTextRect(rcText, pszText, nFormat);
    if (rcText.right > m_pModule->GetRightPrintPos() - m_pModule->GetRightMargin())
    {
      CRect rcTextTemp;
      //We need multiple lines here, calculate the rectangle for multi line
      nFormat |= DT_WORDBREAK;
      nFormat &= ~DT_SINGLELINE;
      m_pModule->CalcTextRect(rcTextTemp, pszText, nFormat);

      //Now enlarge the print rect
      CRect rcPrint;
      m_pModule->GetPrintRect(rcPrint);
      rcPrint.bottom += rcTextTemp.Height() - rcText.Height();
      m_pModule->SetPrintRect(rcPrint);

      //Remember the row height
      SetRowHeight(rcPrint.Height(), nRow);
    }
  }

  m_pModule->PrintText(pszText, nFormat, ePrnFlags);

  ResetPrintRect();
}

//***************************************************************************
// Function         CSmxPrnTable::PrintText
// @mfunc           Print a text in the specified cell.
// @parm            The row number of the cell.
// @parm            The column number of the cell.
// @parm            The text which should be printed.
// @parm            The draw text format.
// @parm            Print text flags.
//***************************************************************************
void CSmxPrnTable::PrintText(UINT nRow, UINT nCol, LPCTSTR pszText, UINT nFormat, CSmxPrnView::EPrnFlags ePrnFlags)
{
  PrintText(nRow, nCol, nCol, pszText, nFormat, ePrnFlags);
}


//***************************************************************************
// Function         CSmxPrnTable::SetPrintCell
// @mfunc           Set the rectangle of the specified cell as the printing rectangle.
// @parm            The cells row number.
// @parm            The cells column number.
//***************************************************************************
void CSmxPrnTable::SetPrintCell(UINT nRow, UINT nColStart, UINT nColEnd)
{
  if (!m_bMarginsSet)
  {
    m_rcPrevMargins.left   = m_pModule->GetRectLeftMargin();
    m_rcPrevMargins.right  = m_pModule->GetRectRightMargin();
    m_rcPrevMargins.top    = m_pModule->GetRectTopMargin();
    m_rcPrevMargins.bottom = m_pModule->GetRectBottomMargin();
    m_rcPrevPrintRect      = m_pModule->m_rcPrint;

    m_pModule->SetRectLeftMargin(m_nLeftColMargin);
    m_pModule->SetRectRightMargin(m_nLeftColMargin);
 
    //Will be centered vertically anyhow!
    m_pModule->SetRectTopMargin(0);
    m_pModule->SetRectBottomMargin(0);

    m_bMarginsSet = true;
  }

  CRect rcCell;
  GetCellRect(rcCell, nRow, nColStart);
  if (nColEnd && nColEnd > nColStart)
  {
    CRect rcTemp;
    GetCellRect(rcTemp, nRow, nColEnd);
    rcCell.right = rcTemp.right;
  }

  if ((int) nRow < m_aRowHeight.GetSize() && m_aRowHeight[nRow] > m_nDefaultRowHeight)
  {
    //Modify the print rect, otherwise the text would be centered vertically
    rcCell.bottom -= m_aRowHeight[nRow] - m_nDefaultRowHeight;
  }

  m_pModule->SetPrintRect(rcCell);

}

//***************************************************************************
// Function         CSmxPrnTable::ResetPrintRect
// @mfunc           Reset the margins and printing rectangle.
//***************************************************************************
void CSmxPrnTable::ResetPrintRect()
{
  if (m_bMarginsSet)
  {
    m_pModule->SetRectLeftMargin(m_rcPrevMargins.left);
    m_pModule->SetRectRightMargin(m_rcPrevMargins.right);
    m_pModule->SetRectTopMargin(m_rcPrevMargins.top);
    m_pModule->SetRectBottomMargin(m_rcPrevMargins.bottom);
    m_bMarginsSet = false;
  }
  else
  {
    ASSERT(false);
  }

  m_pModule->SetPrintRect(NULL);

}

