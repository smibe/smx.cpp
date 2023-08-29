#ifndef __SMXTREECTRL_H__
#define __SMXTREECTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

class CSMXTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CSMXTreeCtrl)

public:

	CSMXTreeCtrl();
	virtual ~CSMXTreeCtrl();

public:

	virtual HTREEITEM GetNextItem( HTREEITEM hItem, UINT nCode );
	virtual HTREEITEM GetNextItem( HTREEITEM hItem );
	virtual HTREEITEM GetPrevItem( HTREEITEM hItem );
	// Gets last item in the branch, returns the last item.
	virtual HTREEITEM GetLastItem( HTREEITEM hItem );
	// Returns the previously selected tree item in a multi selection tree control.
	virtual HTREEITEM GetPrevSelectedItem( HTREEITEM hItem );

	// Returns the next selected item in a multi selection tree control.
	virtual HTREEITEM GetNextSelectedItem( HTREEITEM hItem );
	
	// Returns the first selected item in a multi selection tree control.
	virtual HTREEITEM GetFirstSelectedItem();

	// Returns the RGB text value for the referenced tree item.
	// Returns (COLORREF)-1 if color was not set.
	//
	virtual COLORREF GetItemColor( HTREEITEM hItem);

	// Clears all selections from multi selection tree control.
	virtual void ClearSelection();

	// sets the font for the reference tree item.
	virtual void SetItemFont( HTREEITEM hItem, LOGFONT& logfont);

	// sets the font to bold for the refernece tree item.
	virtual void SetItemBold( HTREEITEM hItem, BOOL bBold=TRUE);

	// Sets the text color for the tree item.
	virtual void SetItemColor( HTREEITEM hItem, COLORREF color);

	// enables multi selection tree control.
	virtual void EnableMultiSelect( BOOL bMultiSelect=TRUE);

	// returns the current font used by the tree item.
	virtual BOOL GetItemFont( HTREEITEM hItem, LOGFONT * plogfont);

	// returns TRUE if the tree item has a bold font.
	virtual BOOL GetItemBold( HTREEITEM hItem);

	// Selects items from hItemFrom to hItemTo in a multi selection tree control.
	// Does not select child item if parent is collapsed. Removes selection from 
	// all other items.
	virtual BOOL SelectItems( HTREEITEM hItemFrom, HTREEITEM hItemTo);

	// Finds an item that contains the search string, returns the handle
	// to the item or NULL.
	virtual HTREEITEM FindItem( LPCTSTR lpszSearch, BOOL bCaseSensitive = FALSE, 
      BOOL bDownDir = TRUE, BOOL bWholeWord = FALSE,
		  HTREEITEM hItem = NULL);

protected:

	// Virtual function used by FindItem to allow this function to filter the 
	// result of FindItem returns TRUE if item matches the criteria.
	virtual BOOL IsFindValid(
		HTREEITEM hti);

  //--------------- Overrides ----------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXTreeCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	//---------- Generated message map functions -----------------
	//{{AFX_MSG(CSMXTreeCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

	HTREEITEM	m_hItemFirstSel;	// first selected item in a multi selection tree control.
	DWORD		m_dwDragStart;		  // the number of milliseconds that have elapsed since the system was started.
	BOOL		m_bMultiSelect;		  // TRUE for a multiselection tree control.

	struct Color_Font {	COLORREF color;	LOGFONT logfont; };
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;
	
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

inline HTREEITEM CSMXTreeCtrl::GetNextItem( HTREEITEM hItem, UINT nCode )
	{ return CTreeCtrl::GetNextItem( hItem, nCode ); }

inline void CSMXTreeCtrl::EnableMultiSelect(BOOL bMultiSelect)
	{ m_bMultiSelect = bMultiSelect; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __SMXTREECTRL_H__
