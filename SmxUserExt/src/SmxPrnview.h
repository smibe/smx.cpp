/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SmxPrnview.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SmxPrnview.h,v 1.2 2002/11/22 17:23:17 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_PRNFORM_H__3F87FC46_9D14_11D5_B7BC_0050BA1FAC52__INCLUDED_)
#define AFX_PRNFORM_H__3F87FC46_9D14_11D5_B7BC_0050BA1FAC52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSmxPrnTable;
class CFormattedTextTag;
class CXMLElement;

//Flags used for updating the device context.
#define UPD_PEN    0x0001
#define UPD_FONT   0x0002
#define UPD_BRUSH  0x0004

//Flags used when framing the print rectangle.
#define FRM_TOP    0x0001
#define FRM_BOTTOM 0x0002
#define FRM_LEFT   0x0004
#define FRM_RIGHT  0x0008

//@enum Unicode symbols
enum EUnicodeSymbol
{
  sy_Alpha    = 0x03b1,             //@emem greek alpha character.
  sy_Beta     = 0x03b2,             //@emem greek beta character.
  sy_Diameter = 0x00d8,             //@emem diameter symbol.
  sy_Sigma    = 0x03a3,             //@emem greek upper case sigma character.
  sy_EUR      = 0x20ac,             //@emem EURO
};


//***************************************************************************
// @class CSmxPrnView |
// 
// Class used as base class for printing forms. The class encapsulates 
// basic printing possibilities.
//
// @base public  | CPrnModule
//***************************************************************************
class CSmxPrnView : public CScrollView  
{
friend class CSmxPrnTable;
#ifdef CScrollView
	DECLARE_DYNAMIC(CSmxPrnFormView)
#else
	DECLARE_DYNAMIC(CSmxPrnView)
#endif
public:
  typedef CArray<CFormattedTextTag*, CFormattedTextTag*> CFormattedTextArray;

  //@cmember,menum Flags used for a comfortable text formatting
  enum EPrnFlags
  {
    PRN_NONE         = 0,         //@@emem No formatting
    PRN_FORMATTED    = 0x0001,    //@@emem The text which will be printed uses formatted text tags like <bold text>
    PRN_MARKED       = 0x0002,    //@@emem The text should be printed as marked text. An virtual function Mark() will be called before the text is printed.
  };

#ifdef CScrollView
  CSmxPrnView::CSmxPrnView(UINT nID);
#else
	CSmxPrnView();                                                 //@cmember Constructor
#endif
	virtual ~CSmxPrnView();                                        //@cmember Destructor

  //----------------- Overloaded PrnModule functions ---------------
  virtual int  GetPageCount () const;                         //@cmember Retrieve the page count
  virtual void OnPrint (CDC* pDC, CPrintInfo* pInfo);         //@cmember Do the printing.
  virtual void OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo); //@cmember Called when beginning to print,
  virtual void OnEndPrintPreview( CDC* pDC, CPrintInfo* pInfo, 
                               POINT point, CPreviewView* pView );
  
  //----------------- Font functions -------------------------------
  void     SetFont(const LOGFONT *pLogFont);                  //@cmember Set the a new font.
  void     GetLogFont(LOGFONT *pLogFont);                     //@cmember Retrieve the current log font.
  void     GetDefaultFont(LOGFONT *pLogFont);                 //@cmember Retrieve the default font.
  void     SetFontSize(int nSize);                            //@cmember Set a new font size.
  void     SetFontWeight(int nWeight);                        //@cmember Set the font weight.
  void     SetFontName(LPCTSTR szFontName);                   //@cmember Set a new font face name.

  //------------------ Pen functions -------------------------------
  void     SetPen(const PLOGPEN pLogPen);                     //@cmember Set a new pen.
  void     GetDefaultPen(LOGPEN *pLogPen);                    //@cmember Retrieve the default pen.
  void     GetLogPen(PLOGPEN pLogPen);                        //@cmember Retrieve the current pen.
  void     SetPenStyle(UINT nStyle);                          //@cmember Set a new pen style.
  void     SetPenWidth(double dbWidth);                       //@cmember Set the pen width.

  //------------------ Brush functions -----------------------------
  void     SetBrush(const PLOGBRUSH PpLogBrush);              //@cmember Set the brush.
  void     GetDefaultBrush(LOGBRUSH *pLogBrush);              //@cmember Retrieve the default brush.
  void     SetBrushColor(COLORREF crBrush);                   //@cmember Set the brush color.

  //------------------ Text attributes and margins -----------------------------
  COLORREF GetTextColor() { return m_pDC->GetTextColor(); }                   //@cmember Retrieve the text color.
  COLORREF SetTextColor(COLORREF crColor);                                    //@cmember Set the text color.
  int      GetLeftMargin();                                                   //@cmember Return the left margin.
  int      GetTopMargin();                                                    //@cmember Return the top margin.
  int      GetRightMargin();                                                  //@cmember Return the right margin.
  int      GetBottomMargin();                                                 //@cmember Return the bottom margin.
  int      GetRectTopMargin() { return m_nRectTopMargin; }                    //@cmember Return the top margin for printing rectangles.
  int      GetRectBottomMargin() { return m_nRectBottomMargin; }              //@cmember Return the bottom margin for printing rectangles.
  int      GetRectLeftMargin() { return m_nRectLeftMargin; }                  //@cmember Return the left margin for printing rectangles.
  int      GetRectRightMargin() { return m_nRectRightMargin; }                //@cmember Return the right margin for printing rectangles.
  void     SetRectTopMargin(int nMargin) { m_nRectTopMargin = nMargin; }      //@cmember Set the top margin for printing rectangles.
  void     SetRectBottomMargin(int nMargin) { m_nRectBottomMargin = nMargin; }//@cmember Set the bottom margin for printing rectangles.
  void     SetRectLeftMargin(int nMargin) { m_nRectLeftMargin = nMargin; }    //@cmember Set the left margin for printing rectangles.
  void     SetRectRightMargin(int nMargin) { m_nRectRightMargin = nMargin; }  //@cmember Set the right margin for printing rectangles.
                                                                              
  //------------------ Special characters -----------------------------
  CString& GetUnicodeSymbol(EUnicodeSymbol eSymbol);                          //@cmember Return a string with the respective unicode symbol.
  CString& GetAlpha()     { return GetUnicodeSymbol(sy_Alpha);    }           //@cmember Return a string containing 'alpha' as unicode symbol.
  CString& GetBeta()      { return GetUnicodeSymbol(sy_Beta);     };          //@cmember Return a string containing 'alpha' as unicode symbol.
  CString& GetSigma()     { return GetUnicodeSymbol(sy_Sigma);    };          //@cmember Return a string containing 'alpha' as unicode symbol.
  CString& GetDiameter();                                                     //@cmember Return a string containing the diameter symbol.

  //----------------- Printing area --------------------------------------
  void SetPrintRect(LPCRECT pRect);                //@cmember Set the printing rectangle
  void GetPrintRect(CRect& rcRect);                //@cmember Get the printing rectangle.
  int  GetLeftPrintPos();                          //@cmember Retrieve the left printing position.
  int  GetRightPrintPos();                         //@cmember Retrive the right printing position.
  int  GetTopPrintPos();                           //@cmember Retrieve the top printing position.
  int  GetBottomPrintPos();                        //@cmember Retrieve the bottom printing position.
  int  GetCurrentLineHeight();                     //@cmember Retrieve the current line height.
  int  GetPageWidth();                             //@cmember Retrieve the page width (without left and right paper margins).
  int  CalcPrintRectHeight(UINT nRows);            //@cmember Calculate the height for a printing rectangle containing nRows of text.

  //----------------- Text drawing functions ------------------------------
  void PrintText(LPCTSTR pszText, UINT nFormat = DT_LEFT | DT_SINGLELINE, EPrnFlags ePrnFlags = PRN_NONE);  //@cmember Print the given text.
  void CalcTextRect(CRect&rcText, LPCTSTR pszText, UINT nFormat);
  void PrintEOL();                                                                                          //@cmember ´Move to the next line.
  void PrintXMLText(CXMLElement *pElt, UINT nFormat);

  //----------------- Printing values ------------------------------
  //@cmember Print a double value.
  void PrintValue(double dbValue, UINT nFormat, WORD nPrecision, WORD nGrpSymbol = -1, EPrnFlags ePrnFormat = PRN_NONE);

  void PrintCurrentDate(UINT nFormat = DT_LEFT | DT_SINGLELINE, EPrnFlags ePrnFlags = PRN_NONE);

  //----------------- Line and rectangle functions ------------------------
  bool PrintLineTo(int x, int y);                            //@cmember Print a line.
  bool PrintLine(int xFrom, int yFrom, int xTo, int yTo);    //@cmember Print a line
  bool PrintVLine();                                         //@cmember Print a vertical line.
  bool PrintHLine();                                         //@cmember Print a horizontal line.
  void FrameRect(LPCRECT pRect, UINT nFrameFlags);           //@cmember Frame a rectangle.
  void FramePrintRect(UINT nFrameFlags);                     //@cmember Frame the printing rectangle.
  void FillSolidRect(LPCRECT pRect, COLORREF crFill);        //@cmember Fill a rectangle with the given color.

  //----------------- Current point functions ------------------------
  CPoint MoveTo(int x, int y);                //@cmember Move the current position.
  CPoint MoveTo(CPoint Point);                //@cmember Move the current position.
  CPoint GetCurrentPosition();                //@cmember Retrieve the current positon.
  void   MoveHSpace(int dx);                  //@cmember Move the current positon horizontally.
  void   MoveVSpace(int dy);                  //@cmember Move the current positone vertically.

  //----------------- EMF functions ------------------------
  bool PrintMetaFile(UINT nMetaFileID, LPCRECT pRect, bool bEnlarge = false); //@cmember Print a meta file.

  //----------------- Conversion functions ------------------------
  int TwipsToLP(int nTwips) const;                      //@cmember Convert twips into logical points.
  int PointsToLP(double dbPoints) const;                //@cmember Convert points /!/72 inch) into logical  points.
  void LPtoDC(CPoint& Point);                           //@cmember Change y direction for drawing operations.
  void LPtoDC(CRect& rcRect);                           //@cmember Change y direction for drawing operations.
  int  LPtoDC_y(int nY);                                //@cmember Change y direction for drawing operations.
  void DCtoLP(CPoint& Point);                           //@cmember Change y direction for drawing operations.
  void DCtoLP(CRect& rcRect);                           //@cmember Change y direction for drawing operations.
  int  DCtoLP_y(int nY);                                //@cmember Change y direction for drawing operations.
  void LPtoHiMetric(CSize *pSize);                      //@cmember Convert a logical size into himetric size.
  void LPtoHiMetric(CRect *pRect);                      //@cmember Convert a logical rectangle into a himetric rectangle.
  void LPtoHiMetric(CPoint *pPoint);                    //@cmember Convert a logical point into an himetric point.

  int     GetClientLeftMargin()   const { return m_nClientLeftMargin; }  // Client area margin in logical units.
  int     GetClientRightMargin()  const { return m_nClientRightMargin; }
  int     GetClientTopMargin()    const { return m_nClientTopMargin; }
  int     GetClientBottomMargin() const { return m_nClientBottomMargin; }

  //------------------ Page printing functions -----------------------------
  virtual void PrintHeader(UINT nPage) {};
  virtual void PrintBody(UINT nPage) {};
  virtual void PrintFooter(UINT nPage) {};

  //------------------ Table printing functions -----------------------------
  void PrintTable(CSmxPrnTable& PrnTable);
  virtual void PrintTableHeader(CSmxPrnTable& PrnTable, UINT nPage) {};
  virtual bool PrintTableRow(CSmxPrnTable& PrnTable, UINT nPage, UINT nRow) {return true;};
  virtual void PrintTableFooter(CSmxPrnTable& PrnTable, UINT nPage) {};

  //------------------ General Printing -----------------------------
  virtual void Draw() { ASSERT(false); }
  
  //------------------ Virtual function for Print/Draw customization --------------------
  virtual bool    UseForPrintOnly() { return FALSE; }
  virtual bool    CalculateNofPagesInOnDraw() { return FALSE; }
  virtual void    SetPageOrientation(bool bLandscape) { }
  virtual LPCTSTR GetPageSetupSection() { return NULL; }

  //------------------ Other functions -------------------------------

  //----------------- Overloaded virtual MFC functions ------------------------
	//{{AFX_VIRTUAL(CPrnUstView)
	protected:
  virtual void OnPrepareDC( CDC* pDC, CPrintInfo* pInfo = NULL );
  virtual void OnInitialUpdate();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL


  //--------------------- Overrides
	//{{AFX_VIRTUAL(CBookJournalView)
	protected:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_VIRTUAL

	// not mapped commands - must be mapped in derived class
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
  DECLARE_MESSAGE_MAP()

  //Scrollint
  void DoScroll(int nScroll);

private:
  //@cmember,menum Special formatting flags
  enum EFormatFlags
  {
    fmt_None    = 0,         //@@emem  No formatting
    fmt_Bold    = 0x0001,    //@@emem  Change font to bold
    fmt_Sub     = 0x0002,    //@@emem  Subscript
    fmt_Symb    = 0x0004,    //@@emem  Change font to symbol font
    fmt_Unicode = 0x0008,    //@@emem  Print unicode characters 
  };

protected:
  void UpdateBrush();                        //@cmember Update the brush in the device context.
  void UpdateFont();                         //@cmember Update the font in the device context.
  void UpdatePen();                          //@cmember Update the pen in the device context.
  bool IsClientRect(const CRect& Rect);      //@cmember Check if the given rectangle is the client rectangle.

  //----------------- Helper functions for formatted text ------------------------
  void   SplitFormattedText(CFormattedTextArray& TextArray, LPCTSTR pszText);                           //@cmember Split a text with formatted text tags.
  void   PrintTextTokens(const CFormattedTextArray& TextArray, UINT nFormat);                           //@cmember Print text tokens. 
  void   MoveTextTokenBounds(CFormattedTextArray& TextArray, const CPoint& ptMove, int nBaselineY);     //@cmember Move the text token bounds.
  CPoint GetBoundsTopLeft(const CRect& rcBounds, UINT nFormat);                                         //@cmember Get the TopLeft point of the rectangle if it has to be alligned as specified in the format flags.
  void   GetFormattedTextBounds(CRect& rcBounds, int& nBaseLine, const CFormattedTextArray& TextArray); //@cmember Get the bounding rectangle for the specified text tags.

  const  TCHAR *GetFormatToken(EFormatFlags& eFormatFlags, LPCTSTR pszChar);                            //@cmember Retrieve the format token.
  void   CreateTextTag(CFormattedTextArray& TextArray, const CString& strText, 
                       UINT nStart, UINT nCount, DWORD dwTextFormatFlags);                              //@cmember Create a text tag.

  void PrintSimpleText(LPCTSTR pszText, UINT nFormat);                                                  //@cmember Print a simple text without formatted text tags.
  void PrintFormattedText(LPCTSTR pszText, UINT nFormat);                                               //@cmember Print a text with formatted text tags.
  int  GetPaperHeight();
  void UpdatePageSetup();

protected:
  int  m_nCurPage;
  int  m_nNofPages;
  int m_nCurrentLineHeight;

  int m_nMapMode;                      //@cmember The mapping mode to be used.
  int m_nPaperLeftMargin;              //@cmember Paper left margin.
  int m_nPaperRightMargin;             //@cmember Paper right margin.
  int m_nPaperTopMargin;               //@cmember Paper top margin.
  int m_nPaperBottomMargin;            //@cmember Paper bottom margin.
  int m_nHeaderHeight;                 //@cmember The height of the header.
  int m_nFooterHeight;                 //@cmember The height of the footer.

  
  CPrintInfo *m_poPrintInfo;           //@cmember Temporary variable to store the print info.
  CDC        *m_pDC;

  int  m_nX;                             //@cmember The current X position
  int  m_nY;                             //@cmember The current Y position
  int  m_nMaxX;                          //@cmember The maximum X position
  int  m_nMaxY;                          //@cmember The maximum Y position

  CFont    m_Font;                      //@cmember The current font.
  CPen     m_Pen;                       //@cmember The current brush.
  CBrush   m_Brush;                     //@cmember The current pen.
  UINT     m_nUpdateFlags;              //@cmember Flags used to remember if font brush or pen ave changed.
  LOGFONT  m_LogFont;                   //@cmember Logfont structure for storing the current font.
  LOGBRUSH m_LogBrush;                  //@cmember LogBrush structure for storing the current brush.
  LOGPEN   m_LogPen;                    //@cmember LogPen structure for storing the current pen.

  int      m_nRectTopMargin;            //@cmember Top margin used in printing rectangles different from the client rectangle.
  int      m_nRectBottomMargin;         //@cmember Bottom margin used in printing rectangles different from the client rectangle.
  int      m_nRectLeftMargin;           //@cmember Left margin used in printing rectangles different from the client rectangle.
  int      m_nRectRightMargin;          //@cmember Right margin used in printing rectangles different from the client rectangle.

  int      m_nNofColumns;               //@cmember Number of columns for column printing
  int      m_nFirstCol;                  //@cmember First column to use for column printing (start with 1)
  int      m_nBetweenCols;               //@cmember Size used between columns

  CRect m_rcClient;                      //@cmember Client area in logical units.
  int   m_nClientLeftMargin;             //@cmember  Client area left margin in logical units.
  int   m_nClientRightMargin;            //@cmember  Client area left margin in logical units
  int   m_nClientTopMargin;              //@cmember  Client area left margin in logical units
  int   m_nClientBottomMargin;           //@cmember  Client area left margin in logical units

  CRect    m_rcPrint;                   //@cmember The current printing rectangle.
  CString  m_strSymbol;                 //@cmember String used to return special symbol characters.

  bool     m_bCalcNofPages;

  HACCEL   m_hSavedFrmAccelTable;       //@cmember Save the accelerator table of the frame in print preview.

  //-------- Formatting keywords ----------------
  static const TCHAR   sm_szBold[];     //@cmember 'bold' formatting tag for bold text.
  static const TCHAR   sm_szSub[];      //@cmember 'sub'  formatting tag for subscript
  static const TCHAR   sm_szSymb[];     //@cmember 'symb' formatting tag for symbol font
  static const TCHAR   sm_szUnicode[];  //@cmember 'unicode' formatting tag for unicode text.

  //--------- Other static strings --------------
  static const TCHAR   sm_szSymbol[];    //@cmember The symbol font face ('Symbol')

};

//***************************************************************************
// @class CFormattedTextTag |
// 
//  Class used to store formatted text tags.
//
//***************************************************************************
class CFormattedTextTag
{
public:
  CFormattedTextTag() { m_pLogFont = NULL; };    //@cmember Constructor.
  ~CFormattedTextTag() { delete m_pLogFont; };   //@cmember Destructor.

public:
  CRect    m_rcBounds;                           //@cmember Rectangle were the text will be printed.
  LOGFONT *m_pLogFont;                           //@cmember Font which should be used for printing.
  CString  m_strText;                            //@cmember The text which will be printed.
  LONG     m_tmDescent{};                          //@cmember Remember the descent (textmetrics) for this text tag.
  DWORD    m_dwFormatFlags{};                      //@cmember  The format flags for this text tag
};


#endif // !defined(AFX_PRNFORM_H__3F87FC46_9D14_11D5_B7BC_0050BA1FAC52__INCLUDED_)
