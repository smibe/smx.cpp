
#ifndef __SMXLISTCTRL_H__
#define __SMXLISTCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCJFlatHeaderCtrl;

// CSMXListCtrl is a CListCtrl derived class which implements a
// flat header and generic sorting routines.
class CSMXListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CSMXListCtrl)

public:

	// Default constructor
	//
	CSMXListCtrl();

	// Virtual destructor
	//
	virtual ~CSMXListCtrl();

protected:

	int					m_nMinColWidth;		// minimum width for columns
	int					m_nMaxColWidth;		// maximum width for columns
	int					m_nSortedCol;		// index of currently selected column
	CCJFlatHeaderCtrl*	m_pHeaderCtrl;		// points to subclassed flat header control.
	CString				m_strSection;		// registry section name.
	CString				m_strEntry;			// registry entry name.
	CString				m_strDefault;		// registry default value.
	CListCtrl*			m_pListCtrl;		// points to the list control associated with this view.
	COLORREF			m_clrText;			// RGB value for row text color.
	COLORREF			m_clrTextBk;		// RGB value for row text background color.
	bool				m_bNoColSizing;		// if true, column sizing is disabled.
	int					m_nMinSize;			// if set, columns will have a minimum size requirement.
	bool				m_bAscending;		// flag to determine sorting order

private:

	bool				m_bAutoSave;		// true if saving column widths.

// Operations
public:
	
	// this member function will return the column width stored in the registry.
	//
	virtual int GetStoredWidth(int nCol);

	// this member function will automatically size a given column.
	//
	virtual void AutoSizeColumn(int nCol=-1);

	// this member function will set the column width from the
	// registry settings.
	//
	virtual void SetColumnWidth(int nCol);

	// this member function loads column widths from the registry.
	//
	virtual void LoadColumnWidths();

	// this member function saves column widths to the registry.
	//
	virtual void SaveColumnWidths();

	// this member function will initialize the autosave feature. If called
	// column widths will be saved to system registry when the list control
	// window is destroyed.
	//
	virtual void AutoSaveColumns(
		// registry section name, if NULL the default is "Settings".
		LPCTSTR lpszSection=NULL,
		// registry entry name, if NULL the default is "Column Info".
		LPCTSTR lpszEntry=NULL,
		// registry default value, if NULL the default id "".
		LPCTSTR lpszDefault=NULL);

	// call this member function to subclass the the flat header control.
	//
	virtual bool SubclassHeader(bool bBoldFont=false);

	// override this member function in your derived class to
	// peform custom sort routines.
	//
	virtual bool SortList(
		// passed in from control, index of column clicked.
		int nCol,
		// passed in from control, true if sort order should be ascending.
		bool bAscending );

	// this member function is called to set extended styles for the list
	// control ie: LVS_EX_FULLROWSELECT, LVS_EX_GRIDLINES, LVS_EX_HEADERDRAGDROP.
	//
	virtual void SetExtendedStyle( DWORD dwExStyle );

	// this member function is called to return the list control extended style.
	//
	virtual DWORD GetExtendedStyle();

	// call this member function to enable/disable column sizing
	//
	virtual void DisableColumnSizing(bool bDisable);

	// call this member function to set a minimum column size
	//
	virtual void SetMinimumColSize(int nSize);

	// call this member function to determine the row index and column index for a point.
	// returns the row index or -1 if point is not over a row.
	//
	virtual int HitTestEx(
		// point to be tested.
		CPoint &point,
		// to hold the column index
		int *col) const;

	// adds column after the right most column.
	//
	virtual int AddColumn(
		// the column's heading7
		LPCTSTR lpszColHeading,
		// Width of the column, in pixels. If this parameter is -1, 
		// the column width is the same as previous column
		int nWidth = -1,
		// the alignment of the column. Can be LVCFMT_LEFT,
		// LVCFMT_RIGHT, or LVCFMT_CENTER.
		int nFormat=LVCFMT_LEFT);
	
	// this member function will build the columns for the list control. Returns
	// false if an error occured.
	//
	virtual bool BuildColumns(
		// number of columns the list control contains
		int nCols,
		// int array containing the column widths
		int* nWidth,
		// CString array containing the string for each column
		CString* strColString );
	
	// this member function will build the columns for the list control. Returns
	// false if an error occured.
	//
	virtual bool BuildColumns(
		// number of columns the list control contains
		int nCols,
		// int array containing the column widths
		int* nWidth,
		// int array containing the  string resource id for each column
		int* nColString );

	// sets the row text and background colors.
	virtual void SetRowColors(COLORREF clrText, COLORREF clrTextBk);
	
	// returns the number of columns in the list.
	virtual int GetColumnCount() const;

	// copies a row to a new location.
	//
	virtual bool CopyRow(int nFrom, int nTo);

	// moves a row to a new location.
	//
	virtual bool MoveRow(int nFrom, int nTo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXListCtrl)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CSMXListCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


inline DWORD CSMXListCtrl::GetExtendedStyle()
{ 
  ASSERT(::IsWindow(m_hWnd)); return SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0); 
}

inline void CSMXListCtrl::SetExtendedStyle(DWORD dwExStyle)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, GetExtendedStyle()|dwExStyle); }

inline void CSMXListCtrl::SetRowColors(COLORREF clrText, COLORREF clrTextBk)
	{ m_clrText = clrText; m_clrTextBk = clrTextBk; }

inline void CSMXListCtrl::SetMinimumColSize(int nSize)
	{ m_nMinSize = nSize; }

inline void CSMXListCtrl::DisableColumnSizing(bool bDisable)
	{ m_bNoColSizing = bDisable; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SMXLISTCTRL_H__
