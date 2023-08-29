/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SmxPrnTable.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SmxPrnTable.h,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_PRNTABLE_H__130B0186_B25C_11D5_B7CC_0050BA1FAC52__INCLUDED_)
#define AFX_PRNTABLE_H__130B0186_B25C_11D5_B7CC_0050BA1FAC52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
// @class CSmxPrnTable |
// 
// Class used for printing tables.
//
//***************************************************************************
class CSmxPrnTable
{
public:
  CSmxPrnTable(CSmxPrnView *pModule);                                             //@cmember Constructor.
  ~CSmxPrnTable();                                                             //@cmember Destructor.

  //------------------ Attributes ------------------------
  CSmxPrnTable& SetRowHeight(UINT nHeight, UINT nRow);                         //@cmember Set the row height for the specified row table.
  CSmxPrnTable& SetDefaultRowHeight(UINT nHeight);                  //@cmember Set the default row height.
  CSmxPrnTable& SetColumns(UINT nNofColumns, UINT nNovValues, UINT *pValues);  //@cmember Set the number and width of columns.
  CSmxPrnTable& SetLeftColMargin(UINT nLeftColMargin);                         //@cmember Set the left column margin.
  CSmxPrnTable& SetRightColMargin(UINT nRightColMargin);                       //@cmember Set the right column margin.
  CSmxPrnTable& SetNofRows(UINT nNofRows);                                     //@cmember Set the number of rows.
  CSmxPrnTable& SetDefaultTxtFormat(UINT nFormat)             { m_nDefaultTxtFormat = nFormat; return *this; }
  CSmxPrnTable& SetTableTopMargin(UINT nTableTopMargin)       { m_nTableTopMargin = nTableTopMargin; return *this; }
  CSmxPrnTable& SetTableBottomMargin(UINT nTableBottomMargin) { m_nTableBottomMargin = nTableBottomMargin; return *this; }
  void       GetTableRect(CRect &rcTable)                  { rcTable = m_rcTable; }
  int        GetTableBottom()                              { return m_rcTable.bottom; }
  CPoint     GetTableBottomLeft()                          { return CPoint(m_rcTable.left, m_rcTable.bottom); }
  UINT       GetNofRows()                                  { return m_nNofRows; }
  UINT       GetNofColumns()                               { return m_nNofColumns; }
  UINT       GetDefaultRowHeight() {return m_nDefaultRowHeight; }

  //------------------ Line and frame functions ------------
  void PrintFrame(UINT nFlags);                                //@cmember Print a frame around the table.
  void PrintHLines();                                          //@cmember Print horizontal lines between rows.
  void PrintVLines();                                          //@cmember Print vertical lines between columns.
  void PrintHLine(UINT nRow);                                  //@cmember Print a horizonztal line at the bottom of the specified row.
  void PrintHLine(UINT nRow, UINT nColFrom, UINT nColTo);      //@cmember Print a horizontal line.
  void PrintVLine(UINT nCol);                                  //@cmember Print a vertical line at the right of the specified column.
  void PrintVLine(UINT nCol, UINT nRowFrom, UINT nRowTo);      //@cmember Print a vertical line.

  //------------------ Text functions --------------------
  //@cmember Print a text in the specified cell.
  void PrintText(UINT nRow, UINT nCol, LPCTSTR pszText, UINT nFlags = 0, CSmxPrnView::EPrnFlags ePrnFlags = CSmxPrnView::PRN_NONE);
  //@cmember Print a text spanning more columns
  void PrintText(UINT nRow, UINT nColStart, UINT nColEnd, LPCTSTR pszText, UINT nFormat = 0, CSmxPrnView::EPrnFlags ePrnFlags = CSmxPrnView::PRN_NONE);

  //----------------- Printing values ------------------------------
  //@cmember Print a value in the specified cell.
  void PrintValue(UINT nRow, UINT nCol, double dbValue, UINT nFormat, WORD nPrecision, WORD nGrpSymbol = -1, 
                  CSmxPrnView::EPrnFlags ePrnFlags = CSmxPrnView::PRN_NONE)
  {
    SetPrintCell(nRow, nCol);

    //Print the text
    m_pModule->PrintValue(dbValue, nFormat | m_nDefaultTxtFormat, nPrecision, nGrpSymbol, ePrnFlags);

    //Restore the values
    ResetPrintRect();
  }

  //------------------ Misc functions --------------------
  int  GetColLeft(UINT nCol);                            //@cmember Get the left border position for the specified column.
  int  GetColRight(UINT nCol);                           //@cmember Get the right border position for the specified column.
  int  GetRowTop(UINT nCol);                             //@cmember Get the top border position for the specified row.
  int  GetRowBottom(UINT nCol);                          //@cmember Get the bottom border position for the specified row.
  void GetCellRect(CRect& rcCell, UINT nRow, UINT nCol); //@cmember Retrieve the cell rectangle.
  void CheckRow(UINT nRow);                              //@cmember Check if the specified row is a valid row.
  void ResetPrintRect();                                 //@cmember Reset the printing rectangle.
  void SetPrintCell(UINT nRow, UINT nColStart, UINT nColEnd = 0); //@cmember Set the rectangle of the specified cell a the printing rectangle.
  int  CalcTableRowHeight(int nStartRow, int nEndRow);   //@cmember Calculate the height of the specified rows (including the end row!)
protected:
  CSmxPrnView *m_pModule;               //@cmember The printing module.
  UINT     *m_pColumns;              //@cmember Width of the columns.
  UINT      m_nNofColumns;           //@cmember Number of columns.
  UINT      m_nLeftColMargin;        //@cmember Left column margin (Margin from the left cell  border to the last printable text position)
  UINT      m_nRightColMargin;       //@cmember Right column margin (Margin from the last printable text position to the right cell border.)
  UINT      m_nTableBottomMargin;    //@cmember Table bottom margin.
  UINT      m_nTableTopMargin;       //@cmember Table top  margin.
  UINT      m_nDefaultRowHeight;     //@cmenber The default row height
  CRect     m_rcTable;               //@cmember The tables rectangle.
  UINT      m_nDefaultTxtFormat;     //@cmember Format used as default draw text format.
  UINT      m_nNofRows;              //@cmember Number of rows, if not set it will be the maximum row number were text was written
  bool      m_bNofRowsSet;           //@cmember Remember if the number of rows was set by the user.
  CRect     m_rcPrevMargins;         //@cmember Remember the previous margins
  CRect     m_rcPrevPrintRect;       //@cmember Remember the previous print rect
  bool      m_bMarginsSet;           //@cmember Remember if we have set the margins
  CArray<UINT, UINT> m_aRowHeight;   //@cmember Array holding the row heights.

};


#endif // !defined(AFX_PRNTABLE_H__130B0186_B25C_11D5_B7CC_0050BA1FAC52__INCLUDED_)
