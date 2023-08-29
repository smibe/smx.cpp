/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: OXGridList.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/OXGridList.cpp,v 1.2 2002/11/22 17:15:16 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 					Class Implementation : COXGridList
// ==========================================================================

// $Archive:: /FCU/octoclss/OXGridList.cpp                                $
// Source : Periphere NV (F.Melendez - R.M ortelmans)
// Creation Date : 	   29th November 1995
// $Author: GeH $
// $Date: 2002/11/22 17:15:16 $
// $Revision: 1.2 $
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"			// standard MFC include
#include "OXGridList.h"		// class specification
#include "OXGridListRes.h"	// class resources

#ifdef _DEBUG
   #undef THIS_FILE
   static char BASED_CODE THIS_FILE[]  = __FILE__;
   static char BASED_CODE _FILE_NAME_[] = "OXGRIDLIST";
#else
   #define  _FILE_NAME_  __FILE__
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
int COXGridList::m_nListEditXOffset = -1;
	// --- The number by which the subitem X position must be adjusted to get the edit X position
int COXGridList::m_nListEditYOffset = -2;
	// --- The number by which the subitem Y position must be adjusted to get the edit T position
int COXGridList::m_nListEditCYOffset = +7;
	// --- The number by which the subitem height must be adjusted to get the edit height

int COXGridList::m_nListEditCXOffset = 15;
	// --- The number by which the edit width must be enlarged starting from the text extent

COXGridList::EUseExtendedData COXGridList::m_eFirstED = COXGridList::EDNo;
	// --- The first valid value for the enumeration type EUseExtendedData 
COXGridList::EUseExtendedData COXGridList::m_eLastED = COXGridList::EDRemoving;
	// --- The last valid value for the enumeration type EUseExtendedData 

///////////////////////////////////////////////////////////////////
// Data members -------------------------------------------------------------
// protected:

	// BOOL m_bLastRowWasVisible;
	// --- Whether the last row is visible or not
	
	// BOOL m_bInitialized;
	// --- The standard behaviour of the grid ctrl needs some initialization
	//	   This has to be performed only once.
	
	// BOOL m_bSortable;
	// --- Sort when the header is clicked.
	
	// BOOL m_bCheckable;
	// --- Whether the control is checkable or not (shows a checkbox in front of each item)

	// UINT m_nCheckStyle;
	// --- The style used to check

	// BOOL m_bAutoEdit;
	// ---  Whether the control is in auto edit mode
	//		(Start editing the item when a valid character is typed)

	// EUseExtendedData m_eUseExtendedData;
	// --- The present state of the extended data
	//		EDNo : No COXGridListData objects are used
	//		EDAdding : COXGridListData objects are being added
	//		EDYes : COXGridListData objects are being used for all items
	//		EDRemoving : COXGridListData objects are being removed

	// int m_nNumOfCols;
	// --- Number of columns in the list ctrl
   
	// int m_nImageColumn;
	// --- Index of column where image(small icon will be drawn,)
	
	// CPen	m_GridPen;
	// --- Pen used to draw gridlines with
	
	// BOOL	m_bGridLines;
	// --- Whether to draw gridlines or not

	// BOOL m_bHorizontalGridLines;
	// BOOL m_bVerticalGridLines;
	// --- Whether horizontal or vertical gridlines should be shown
	//	   This setting will only have effect if m_bGridLines == TRUE

	// CFont m_TextFont;
	// --- The font to draw the ROW text with (not header text)

	// CImageList m_stateImages;
	// --- The image list of the check images

	// COXGridEdit m_gridEdit;
	// --- Contains the subclassed edit control during editing

	// CPoint m_lastClickPos
	// --- Poisition were the last mouse click occured (in this window)
    //     Position in client coordinates

	// int m_nEditSubItem;
	// --- The index of the sub item that is being edited
    //     (Only valid between LVN_BEGINLABELEDIT and LVN_ENDLABELEDIT)

	// CWordArray m_rgbEditable;
	// --- Wether the column is allowed to be edited or not (array of BOOL)
	//	   The size of this array always equals the number of columns of the control

	// CWordArray m_colType;
	// --- Type of the column contents (number = 1, string= 1)
	//	   The size of this array always equals the number of columns of the control
// private:
	
// Member functions ---------------------------------------------------------
// public:

COXGridList::COXGridList()
	{
	// ... Initialize all the data members
	Empty();
	ASSERT_VALID(this);
  	}

void COXGridList::InitGrid()
	{
	m_bInitialized = TRUE;
	m_bEdit = FALSE;
	m_bModified = FALSE;

	// General Background is Light Gray
	SetBkColor(GetSysColor(COLOR_SCROLLBAR));

	// Text Background is color of standard window
	SetTextBkColor(GetSysColor(COLOR_WINDOW));

	// Text color of text in standard window
	SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	
	// Standard list control font + Invalidates everything
	SetTextFont();
	}

BOOL COXGridList::SetSortable(BOOL bSortable /* = TRUE */)
	{
	m_bSortable = bSortable;

	if (bSortable)
		AddExtendedData();
	else if (!m_bUseItemColors)
		RemoveExtendedData();

 	return TRUE;
	}

BOOL COXGridList::GetSortable() const
	{
	return m_bSortable;
	}

BOOL COXGridList::SortColumn(int nColumn /* = 0 */)
	{
	ASSERT((0 <= nColumn) && (nColumn < GetNumCols()));

  if (m_nSortColumn   == nColumn)
    m_bSortAscending = !m_bSortAscending;
  else
    m_bSortAscending = true;

  m_nSortColumn = nColumn;
	COXGridListSortInfo gridListSortInfo(this, nColumn);
  gridListSortInfo.m_bAscending = m_bSortAscending;
	return SortItems(GridCompareProc, (LPARAM)(&gridListSortInfo));
	}

int COXGridList::GetNumCols() const
	{
#ifdef _DEBUG
#ifdef XXX
	// Make sure the number of columns we have stored as data memner 
	// is the correct value
	int nNumber = 0;

	LVCOLUMN lvc;
	memset(&lvc, 0, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT;
	while(GetColumn(nNumber, &lvc))
		{
		++nNumber;
		}

	ASSERT(m_nNumOfCols == nNumber);
#endif
#endif

	return m_nNumOfCols;
	}

BOOL COXGridList::SetEqualWidth()
	{
	ASSERT_VALID(this);

	// Set the columns to be of equal width
	CRect rectLV;
	GetClientRect(rectLV);
	
	// Calculate the common width and the remaining space
	// Total width = (common width * Number of columns) + remaining space
	if (GetNumCols() > 0)
		{
		int nColWidth  = (rectLV.Width()) / GetNumCols();
		int nRemainder = (rectLV.Width()) % GetNumCols();
	
		for (int i = 0; i < GetNumCols(); i++)
			{
			SetColumnWidth(i, nColWidth);
			}
		// ... Let the last column take the remaining width as well
		SetColumnWidth(GetNumCols() - 1, nColWidth + nRemainder);
		return TRUE;
		}

	return FALSE;
	}

BOOL COXGridList::SetGridLines(BOOL bGridLines /* = TRUE */, 
	COLORREF LineColor /* = RGB(0,0,0) */, BOOL bUpdate /* = TRUE */)
	{
	m_bGridLines = bGridLines;

	if (m_bGridLines)
		{
		// First reset Pen
		m_GridPen.DeleteObject();

		// Recreate pen with alternating dot - space
		// (PS_DOT does not give a good result, so we use
		//  PS_COSMETIC | PS_ALTERNATE)
		LOGBRUSH logBrush;
		logBrush.lbColor = LineColor;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbHatch = HS_HORIZONTAL;
		if (!m_GridPen.CreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &logBrush, 0, NULL))
			if (!m_GridPen.CreatePen(PS_COSMETIC, 1, &logBrush, 0, NULL))
				TRACE0("COXGridList::SetGridLines : Error creating the grid lines Pen\n");

		}

	if (bUpdate)
		RedrawWindow();

	return TRUE;
	}

BOOL COXGridList::GetGridLines(BOOL& bGridLines, COLORREF& LineColor) const
	{
	bGridLines = m_bGridLines;
	
	//Query the Pen for its current color
	if (m_GridPen.GetSafeHandle() != NULL)
		{
		LOGPEN logPen;
		m_GridPen.GetObject(sizeof(LOGPEN), &logPen);
		LineColor = logPen.lopnColor;
		}
	else
		{
		// ... Color not set yet, return default
		LineColor = RGB(0,0,0);
		}
	
	return TRUE;
	}
	 
BOOL COXGridList::SetGridLineOrientation(BOOL bHorizontal /* = TRUE */, BOOL bVertical /* = TRUE */)
	{
	if ((bHorizontal != m_bHorizontalGridLines) || (bVertical != m_bVerticalGridLines))
		{
		m_bHorizontalGridLines = bHorizontal ;
		m_bVerticalGridLines = bVertical;
		// ... The grid line settings changed, if they are visible : refresh the control
		if (m_bGridLines)
			Invalidate();
		}
	return TRUE;
	}

BOOL COXGridList::GetGridLineOrientation(BOOL& bHorizontal, BOOL& bVertical) const
	{
	ASSERT_VALID(this);
	bHorizontal = m_bHorizontalGridLines;
	bVertical  = m_bVerticalGridLines;
	return TRUE;
	}

int COXGridList::GetCurSel() const
	{
	ASSERT_VALID(this);
	return GetNextItem(-1, LVNI_SELECTED);
	}

BOOL COXGridList::SetCurSel(int nSelectionItem, BOOL bSelect /* = TRUE */)
	{
	ASSERT_VALID(this);
	if (nSelectionItem == -1)
		{
		BOOL bSuccess = TRUE;
		int nIndex = 0;
		int nLastIndex = GetItemCount() - 1;
		while (bSuccess && (nIndex <= nLastIndex))
			{
			bSuccess = SetItemState(nIndex++, bSelect ? LVNI_SELECTED : 0, LVNI_SELECTED);
			}
		return bSuccess;
		}
	return SetItemState(nSelectionItem, bSelect ? LVNI_SELECTED : 0, LVNI_SELECTED);
	}

int COXGridList::GetCurFocus() const
	{
	ASSERT_VALID(this);
	return GetNextItem(-1, LVNI_FOCUSED);
	}

BOOL COXGridList::SetCurFocus(int nFocusItem, BOOL bFocus /* = TRUE */)
	{
	ASSERT_VALID(this);
	if (nFocusItem == -1)
		// ... Can have at most one item with focus, no need to iterate
		nFocusItem = GetNextItem(-1, LVNI_FOCUSED);
	return SetItemState(nFocusItem, bFocus ? LVIS_FOCUSED : 0, LVIS_FOCUSED);
	}

BOOL COXGridList::SetTextFont(CFont* pFont /* = NULL */, BOOL bUpdate /* = TRUE */)
	{
	m_TextFont.DeleteObject();
	SetFont(NULL, bUpdate);

	LOGFONT logFont;                                // Logical font struct
	if (pFont != NULL)
		{	
		pFont->GetObject(sizeof(LOGFONT), &logFont);   // Get font attributes

		if (!m_TextFont.CreateFontIndirect(&logFont))
			{
			m_TextFont.CreateStockObject(SYSTEM_FONT);         // Create stock font
			return FALSE;
			}
	
		SetFont(&m_TextFont, bUpdate);
		}

	if (bUpdate)
		RedrawWindow();

	return TRUE;
	}


BOOL COXGridList::SetShowSelAlways(BOOL bShowSelAlways /* = TRUE */)
	{
	if (GetShowSelAlways() != bShowSelAlways)
		{
		// ... Change the window style 
		ModifyStyle(bShowSelAlways ? 0 : LVS_SHOWSELALWAYS, bShowSelAlways ? LVS_SHOWSELALWAYS : 0);
		// .... Invalidate the control to reflect the change
		Invalidate();
		}

	return TRUE;
	}

BOOL COXGridList::GetShowSelAlways() const
	{
	return ((GetStyle() & LVS_SHOWSELALWAYS) == LVS_SHOWSELALWAYS);
	}

void COXGridList::SetMultipleSelection(BOOL bMultiple /* = TRUE */)
	{
	ASSERT(::IsWindow(m_hWnd));

 	long dwStyle = GetStyle();

	if (!bMultiple && ((dwStyle & LVS_SINGLESEL) != LVS_SINGLESEL))
		{
		// Chenge from multiple to single selection
		// Remove all selection except the first one
		int nItem = GetNextItem(-1, LVNI_SELECTED);
		nItem = GetNextItem(nItem, LVNI_SELECTED);
		while (nItem != -1)
			{
			// ... Deselect item
			SetItemState(nItem, 0, LVNI_SELECTED);
			nItem = GetNextItem(nItem, LVNI_SELECTED);
			}
		}

	ModifyStyle(bMultiple ? LVS_SINGLESEL : 0, bMultiple ? 0 : LVS_SINGLESEL);

	ASSERT(bMultiple == GetMultipleSelection());
	}

BOOL COXGridList::GetMultipleSelection() const
	{
	ASSERT(::IsWindow(m_hWnd));
 	return !((GetStyle() & LVS_SINGLESEL) == LVS_SINGLESEL);
	}

void COXGridList::SetEditable(BOOL bEdit /* = TRUE */, int nColumn /* = -1 */)
	{
	// Always set the control to allow editing
 	long dwStyle = GetStyle();
	if ((dwStyle & LVS_EDITLABELS) != LVS_EDITLABELS)
		{
		ModifyStyle(0, LVS_EDITLABELS);
		}

	// Keep record of which columns are allowed to be edited and which are not
	// ... Array must have correct size
	ASSERT(m_rgbEditable.GetSize() == GetNumCols());
	if (nColumn != -1)
		{
		// ... Must be valid column
		ASSERT(0 <= nColumn);
		ASSERT(nColumn < m_rgbEditable.GetSize());
		m_rgbEditable.SetAt(nColumn, (WORD)bEdit);
		}
	else
		{
		for (int nIndex = 0; nIndex < m_rgbEditable.GetSize(); nIndex++)
			{
			m_rgbEditable.SetAt(nIndex, (WORD)bEdit);
			}
		}
	} 

BOOL COXGridList::GetEditable(int nColumn) const
	{
	ASSERT((0 <= nColumn) && (nColumn <= GetNumCols()));
	return (BOOL)m_rgbEditable.GetAt(nColumn);
	}

void COXGridList::SetColType(COXGridList::ColType cType, int nColumn /* = -1 */)
{

	ASSERT(m_colType.GetSize() == GetNumCols());
	if (nColumn != -1)
	{
		// ... Must be valid column
		ASSERT(0 <= nColumn);
		ASSERT(nColumn < m_colType.GetSize());
		m_colType.SetAt(nColumn, (WORD)cType);
	}
	else
	{
		for (int nIndex = 0; nIndex < m_colType.GetSize(); nIndex++)
		{
			m_colType.SetAt(nIndex, (WORD)cType);
		}
	}
}

WORD COXGridList::GetColType(int col)
{
	ASSERT((0 <= col) && (col <= GetNumCols()));
	return (WORD) m_colType.GetAt(col);
}

BOOL COXGridList::SetCheckable(BOOL bCheckable /* = TRUE */)
	{
	if (bCheckable && !m_bCheckable)
		{
		// Check whether a valid state image list is already available
		CImageList* pStateImageList = GetImageList(LVSIL_STATE);
		if(pStateImageList == NULL)
			VERIFY(SetCheckStateImageList());

		// Uncheck all items
		m_bCheckable = bCheckable;
		SetCheck(-1, 0);
		}
	if (!bCheckable && m_bCheckable)
		{
		// Remove checks (no state image) of all items
		SetCheck(-1, -1);
		m_bCheckable = bCheckable;
		}

	return TRUE;
	}

BOOL COXGridList::GetCheckable() const
	{
	return m_bCheckable;
	}

void COXGridList::SetCheckStyle(UINT nStyle /* = BS_AUTOCHECKBOX */)
	{
	m_nCheckStyle = nStyle;
	}

UINT COXGridList::GetCheckStyle() const
	{
	return m_nCheckStyle;
	}

void COXGridList::SetCheck(int nIndex, int nCheck)
	{
	if (GetCheckable())
		{
		if ((nCheck < -1) || (2 < nCheck))
			{
			TRACE1("COXGridList::SetCheck : Invalid check state (%i) specified.\n", nCheck);
			return;
			}
		if ((nCheck == 2) && (m_nCheckStyle == BS_CHECKBOX || m_nCheckStyle == BS_AUTOCHECKBOX))
			{
			TRACE1("COXGridList::SetCheck : Invalid check state (%i) specified.\n", nCheck);
			return;
			}
		int nStartIndex;
		int nEndIndex;
		if (nIndex == -1)
			{
			nStartIndex = 0;
			nEndIndex = GetItemCount() - 1;
			}
		else
			{
			nStartIndex = nIndex;
			nEndIndex = nIndex;
			}
		for (int nItemIndex = nStartIndex; nItemIndex <= nEndIndex; nItemIndex++)
			{
			SetItemState(nItemIndex, INDEXTOSTATEIMAGEMASK(nCheck + 1), ALLSTATEIMAGEMASKS);
			}
		}
	else
		TRACE0("COXGridList::SetCheck : Control is not checkable.\n");
	}

int COXGridList::GetCheck(int nIndex) const
	{
	if (GetCheckable())
		return STATEIMAGEMASKTOINDEX(GetItemState(nIndex, ALLSTATEIMAGEMASKS)) - 1;
	else
		{
		TRACE0("COXGridList::GetCheck : Control is not checkable.\n");
		return 0;
		}
	}

BOOL COXGridList::OnCheck(int nCheckItem)
	{
	// Control must be in checkable mode
	ASSERT(GetCheckable());

	if ((m_nCheckStyle == BS_CHECKBOX) || (m_nCheckStyle == BS_3STATE))
		// No auto check, just return
		return TRUE;

	int nCheck = GetCheck(nCheckItem);
	nCheck++;
	nCheck %= (m_nCheckStyle == BS_AUTOCHECKBOX ? 2 : 3);
	// ... The check state must actually change
	ASSERT(GetCheck(nCheckItem) != nCheck);
	OnCheckChange(nCheckItem, nCheck);

	// If this item is selected, iterate all the other selected items and 
	// check them as well
	if (GetItemState(nCheckItem, LVIS_SELECTED) == LVIS_SELECTED)
		{
		int nItemIndex = -1;
		nItemIndex = GetNextItem(nItemIndex, LVNI_SELECTED);
		while (nItemIndex != -1)
			{
			if ((nItemIndex != nCheckItem) && (GetCheck(nItemIndex) != nCheck))
				OnCheckChange(nItemIndex, nCheck);
			nItemIndex = GetNextItem(nItemIndex, LVNI_SELECTED);
			}
		}
	return TRUE;
	}

BOOL COXGridList::OnCheckChange(int nCheckItem, int nCheck)
	{
	// ... Control must be in checkable mode
	ASSERT(GetCheckable());
	// ... The check state must actually change
	ASSERT(GetCheck(nCheckItem) != nCheck);

	// Parent will be informed because the state of an item changes
	SetCheck(nCheckItem, nCheck);

	return TRUE;
	}

BOOL COXGridList::SetAutoEdit(BOOL bAutoEdit /* = TRUE */)
	{
	m_bAutoEdit = bAutoEdit;
	return TRUE;
	}

BOOL COXGridList::GetAutoEdit() const
	{
	return m_bAutoEdit;
	}

CEdit* COXGridList::EditLabel(int nItem, int nSubItem)
	{
	ASSERT(::IsWindow(m_hWnd)); 

	// If nSubItem == -1, search the first editable subitem
	int nSubItemIndex = 0;
	while ((nSubItem == -1) && (nSubItemIndex < GetNumCols()))
		{
		if (GetEditable(nSubItemIndex))
			nSubItem = nSubItemIndex;
		nSubItemIndex++;
		}

	if (nSubItem == -1)
		{
		TRACE0("COXGridList::EditLabel : No editable column is found, ignoring edit request\n");
		return NULL;
		}

	return (CEdit*)CWnd::FromHandle( (HWND)::SendMessage(m_hWnd, LVM_EDITLABEL, nItem, nSubItem));
	}

BOOL COXGridList::SetImageColumn(int nColumnIndex, BOOL bUpdate /* = TRUE */)
	{
	ASSERT(nColumnIndex < GetNumCols());
	if (GetNumCols() <= nColumnIndex)
		return FALSE;
	
	m_nImageColumn = nColumnIndex;

	if (bUpdate)
		RedrawWindow();
	
	return TRUE;
	}

int COXGridList::GetImageColumn() const
	{
	return m_nImageColumn;
	}

DWORD COXGridList::GetDlgBaseUnits(CDC* pDC)
	{
	ASSERT_VALID(this);

	CString stAlpaChars = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

	DWORD dwBaseUnits;
	CFont* pFont = GetFont();

	// Is it a system font?
	if (pFont == NULL)
		{
		dwBaseUnits = ::GetDialogBaseUnits();
		}
	else
		{
		CFont* pOldFont = pDC->SelectObject(pFont);
		ASSERT(pOldFont);

		// Get the horizontal base units
		CSize stSize = pDC->GetTextExtent(stAlpaChars, stAlpaChars.GetLength());
		WORD xBaseUnits = WORD(MulDiv(1, stSize.cx, stAlpaChars.GetLength()));

		// Get the vertical base units
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		WORD yBaseUnits = (WORD)tm.tmHeight;

		dwBaseUnits = MAKELONG(xBaseUnits, yBaseUnits);

		// Make sure to restore the old font
		pDC->SelectObject(pOldFont);
		}

	return dwBaseUnits;
	}

BOOL COXGridList::IsLastRowVisible() const
	{
	BOOL bResult;
	bResult = GetItemCount() <= GetTopIndex() + GetCountPerPage();
	return bResult;
	}

void COXGridList::OnLastRowAppear()
	{
	// Default implementation does nothing
	}

void COXGridList::DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
	ASSERT_VALID(this);
	ASSERT(m_bInitialized);
	
	// Note that 'lpDIS->itemID' = item id of the current row being
	// painted.
	if(lpDIS->itemID == -1)
		return;
		
	// You should get the pointer to the device context from the "lpDIS" ptr.
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	switch(lpDIS->itemAction)
		{	
		case  ODA_SELECT:
		case  ODA_DRAWENTIRE:
			{
			// 1 - Set the forground and background colors. For
			//     selected   = bkgd(COLOR_HIGHLIGHT),
			//                    fgd(COLOR_HIGHLIGHTTEXT)
			//     otherwise  = bkgd(GetTextBkColor()),
			//                    fgd(GetTextColor())
			// 2 - Save the "forground" and "background"
			//     colors of the device context in the "crlXXXXXX" variables.
			//
			// YOU MUST RESTORE THE DEVICE CONTEXT TO ITS ORIGINAL
			// STATE by restoring the old colors.

			BOOL bShowSelAlways = GetShowSelAlways();
			BOOL bFocus = (::GetFocus() == m_hWnd);
			BOOL bShowSel = bShowSelAlways && (GetFocus() != &m_gridEdit);
			BOOL bEnabled = IsWindowEnabled();
			BOOL bShowItemSel = bShowSel && ((lpDIS->itemState & ODS_SELECTED) == ODS_SELECTED);
      BOOL bShowItemSelFocus = bShowItemSel && bFocus;

			COLORREF clrForground, clrBackground;
			clrForground = pDC->SetTextColor(bShowItemSelFocus ?
		              		GetSysColor(COLOR_HIGHLIGHTTEXT) : GetItemTextColor(lpDIS->itemID));

			if (bEnabled)
      {
				if (bShowItemSelFocus)
          clrBackground = pDC->SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
        else if (bShowItemSel)
          clrBackground = pDC->SetBkColor(GetSysColor(COLOR_INACTIVEBORDER));
        else
          clrBackground = pDC->SetBkColor(GetItemTextBkColor(lpDIS->itemID));
      }
			else
				// ... Use disabled background color
				clrBackground = pDC->SetBkColor(::GetSysColor(COLOR_INACTIVEBORDER));

			// Populate the listview with column text
			SetRowText(lpDIS, bShowItemSel);
			
			// REQUIRED: Restore the original device context colors
			pDC->SetTextColor(clrForground);
			pDC->SetBkColor(clrBackground);

			////////
			// Is the item selected? Then draw a rectangle around the
			// entire selection. Note that I'm using 'lpDIS->rcItem'.
			////////
			if ((lpDIS->itemState & ODS_FOCUS) && bFocus)
				{
				m_SelectedRect = lpDIS->rcItem;
				pDC->DrawFocusRect(&lpDIS->rcItem);
				}

			if (m_bGridLines)
				// Draw all the gridlines
				DrawGridLines(pDC, lpDIS);
			
			break;
			}
		default:
			TRACE1("COXGridList::DrawItem : Unexpected case in switch : %i\n", lpDIS->itemAction);
			ASSERT(FALSE);
		}
	}

BOOL COXGridList::EnsureVisible(int nItem, int nSubItem, BOOL bPartialOK)
	{
	// First make item visible
	if (!CListCtrl::EnsureVisible(nItem, bPartialOK))
		return FALSE;

	if ((nSubItem < 0) || (GetNumCols() <= nSubItem))
		// Cannot make nonexistant subitem visible
		return FALSE;

	// Then make subitem visible;
	// ... Get the rect of the subitem and its possible images
	CRect subItemRect = GetRectFromSubItem(nItem, nSubItem, TRUE);
	CRect clientRect;
	CRect intersectRect;
	GetClientRect(clientRect);
	BOOL bIntersect = intersectRect.IntersectRect(subItemRect, clientRect);
	if (bIntersect && bPartialOK)
		// The subitem rect is partially visible and that is enough
		return TRUE;
	else
		{
		// Scroll subitem into view as best as possible
		CSize scrollSize(0,0);
		// First check whether right side is visible
		if (clientRect.right < subItemRect.right)
			{
			scrollSize.cx += subItemRect.right - clientRect.right;
			subItemRect += scrollSize;
			}
		// Then check whether left side is visible
		// this may override the previous scroll size of the right side check
		if (subItemRect.left < clientRect.left)
			{
			scrollSize.cx += subItemRect.left - clientRect.left;
			subItemRect += scrollSize;
			}
		// Now do the actual scrolling
		ASSERT(scrollSize.cy == 0);
		if (scrollSize.cx != 0)
			Scroll(scrollSize);

		// We did our best to make the most of the specified subitem visible
		return TRUE;
		}
	}

BOOL COXGridList::GetSubItemFromPoint(CPoint pos, int& nItem, int& nSubItem, CRect& rect) const
	{
	nItem = -1;
	nSubItem = -1;
	rect.SetRectEmpty();

	// First locate the index of the item containing the point
	nItem = HitTest(pos);
	if (nItem < 0)
		{
		TRACE0("COXGridList::GetSubItemFromPoint : Point outside valid item range\n");
		return FALSE;
		}

	// Then locate the sub index containing the point
	BOOL bValidColumn = TRUE;
	BOOL bSubItemFound = FALSE;
	int nCol = 0;
	int nColWidth;
	CRect subItemRect;
	LVCOLUMN lvColumn;
  memset (&lvColumn, 0, sizeof(LVCOLUMN));
  lvColumn.mask = LVCF_WIDTH;

	// Get the size of the images that can be shown in front of the label
	CRect boundsRect;
	CRect labelRect;
	VERIFY(GetItemRect(nItem, boundsRect, LVIR_BOUNDS));
	VERIFY(GetItemRect(nItem, labelRect, LVIR_LABEL));

	int nImagesWidth = labelRect.left - boundsRect.left;
	CImageList* pStateImageList = GetImageList(LVSIL_STATE);
	if ((pStateImageList != NULL) && (STATEIMAGEMASKTOINDEX(GetItemState(nItem, ALLSTATEIMAGEMASKS)) == 0))
		{
		// The state image list exists, but this item has a state of 0 (do not show state image)
		// So we subtract the size of the state image from the images width
		IMAGEINFO imageInfo;
		pStateImageList->GetImageInfo(0, &imageInfo);
		nImagesWidth -= imageInfo.rcImage.right;
		}
	
	// Get the rect of the label (without incorporating the image widths)
	subItemRect = boundsRect;
	subItemRect.right = boundsRect.left + GetColumnWidth(0);

	if (m_nImageColumn == nCol)
		subItemRect.left += nImagesWidth;
	bSubItemFound  = subItemRect.PtInRect(pos);
	while (!bSubItemFound && bValidColumn)
		{
		nCol++;
		bValidColumn = GetColumn(nCol, &lvColumn);
		if (bValidColumn)
			{
			nColWidth = GetColumnWidth(nCol);
			subItemRect.left = subItemRect.right;
			subItemRect.right += nColWidth;
			if (m_nImageColumn == nCol)
				subItemRect.left += nImagesWidth;
			bSubItemFound  = subItemRect.PtInRect(pos);
			}
		}
	if (bSubItemFound)
		{
		nSubItem = nCol;
		rect = subItemRect;
		}
	else
		{
		TRACE0("COXGridList::GetSubItemFromPoint : Point outside valid subitem range\n");
		return FALSE;
		}
	return TRUE;
	}

CRect COXGridList::GetRectFromSubItem(int nItem, int nSubItem, BOOL bIncludeImages /* = FALSE */) const
	{
	// Then locate the sub index containing the point
	CRect resultRect(0,0,0,0);
	BOOL bValidColumn = TRUE;
	BOOL bSubItemFound = FALSE;
	int nCol = 0;
	int nColWidth;
	CRect subItemRect;
	LVCOLUMN lvColumn;
  memset (&lvColumn, 0, sizeof(LVCOLUMN));
	lvColumn.mask = LVCF_WIDTH;

	// Get the size of the images that can be shown in front of the label
	CRect boundsRect;
	CRect labelRect;
	VERIFY(GetItemRect(nItem, boundsRect, LVIR_BOUNDS));
	VERIFY(GetItemRect(nItem, labelRect, LVIR_LABEL));

	int nImagesWidth = labelRect.left - boundsRect.left;
	CImageList* pStateImageList = GetImageList(LVSIL_STATE);
	if ((pStateImageList != NULL) && (STATEIMAGEMASKTOINDEX(GetItemState(nItem, ALLSTATEIMAGEMASKS)) == 0))
		{
		// The state image list exists, but this item has a state of 0 (do not show state image)
		// So we subtract the size of the state image from the images width
		IMAGEINFO imageInfo;
		pStateImageList->GetImageInfo(0, &imageInfo);
		nImagesWidth -= imageInfo.rcImage.right;
		}
	
	// Get the rect of the label (without incorporating the image widths)
	subItemRect = boundsRect;
	subItemRect.right = boundsRect.left + GetColumnWidth(0);

	if (!bIncludeImages && (m_nImageColumn == nCol))
		subItemRect.left += nImagesWidth;
	while ((nCol != nSubItem) && bValidColumn)
		{
		nCol++;
		bValidColumn = GetColumn(nCol, &lvColumn);
		if (bValidColumn)
			{
			nColWidth = GetColumnWidth(nCol);
			subItemRect.left = subItemRect.right;
			subItemRect.right += nColWidth;
			if (!bIncludeImages && (m_nImageColumn == nCol))
				subItemRect.left += nImagesWidth;
			}
		}

	if (nCol == nSubItem)
		resultRect = subItemRect;
#ifdef _DEBUG
	else
		{
		TRACE2("COXGridList::GetRectFromSubItem : Item : %i and subitem  : %i not found\n",
			nItem, nSubItem);
		}
#endif // _DEBUG

	return resultRect;
	}

CHeaderCtrl* COXGridList::GetHeaderCtrl()
	{
	ASSERT_VALID(this);
	HWND hHeaderWnd = GetHeaderCtrlHandle();
	if (hHeaderWnd  != NULL)
		{
		// CHeaderCtrl does not have additional members than those already present 
		// in CWnd.  So we just cast it to CHeaderCtrl*.
		ASSERT(sizeof(CHeaderCtrl) == sizeof(CWnd));
		return (CHeaderCtrl*)CWnd::FromHandle(hHeaderWnd);
		}
	else
		return NULL;
	}

HWND COXGridList::GetHeaderCtrlHandle()
	{
	ASSERT_VALID(this);
	if (m_hWnd == NULL)
		// ... This grid list has not been created uet
		return NULL;

	// Get the first child of the list control
	// Normally the list only has one child window : the header control
	HWND hHeaderWnd = ::GetWindow(m_hWnd, GW_CHILD);
	if (hHeaderWnd != NULL)
		{
#ifdef _DEBUG
		// Make extra sure we actually have a header ctrl
		const int nMaxClassNameLength = 50;
		TCHAR szClass[nMaxClassNameLength + 1];
		::GetClassName(hHeaderWnd, szClass, nMaxClassNameLength);
		ASSERT(_tcscmp(szClass, _T("SysHeader32")) == 0); 
#endif // _DEBUG
		return hHeaderWnd;
		}
	else
		{
		TRACE0("CGridCtrlView::GetHeaderCtrlHandle : No child window found\n");
		return NULL;
		}
	}

#ifdef _DEBUG
void COXGridList::AssertValid() const
	{
	CListCtrl::AssertValid();
	}

void COXGridList::Dump(CDumpContext& dc) const
	{
	CListCtrl::Dump(dc);
	dc << _T("\nm_bLastRowWasVisible: ") << m_bLastRowWasVisible;
	dc << _T("\nm_bInitialized: ") << m_bInitialized;
	dc << _T("\nm_bSortable: ") << m_bSortable;
	dc << _T("\nm_bCheckable: ") << m_bCheckable;
	dc << _T("\nm_nCheckStyle: ") << m_nCheckStyle;
	dc << _T("\nm_eUseExtendedData: ") << (DWORD)m_eUseExtendedData;
	dc << _T("\nm_nNumOfCols: ") << m_nNumOfCols;
	dc << _T("\nm_nImageColumn: ") << m_nImageColumn;
	dc << _T("\nm_GridPen: ") << m_GridPen;
	dc << _T("\nm_bGridLines: ") << m_bGridLines;
	dc << _T("\nm_bHorizontalGridLines: ") << m_bHorizontalGridLines;
	dc << _T("\nm_bVerticalGridLines: ") << m_bVerticalGridLines;
	dc << _T("\nm_SelectedRect: ") << m_SelectedRect;
	dc << _T("\nm_TextFont: ") << (void*)&m_TextFont;
	dc << _T("\nm_stateImages: ") << (void*)&m_stateImages;
	dc << _T("\nm_gridEdit: ") << (void*)&m_gridEdit;
	dc << _T("\nm_lastClickPos: ") << m_lastClickPos;
	dc << _T("\nm_nEditSubItem: ") << m_nEditSubItem;
	dc << _T("\nm_rgbEditable: ") << (void*)&m_rgbEditable;
	dc << _T("\nm_nListEditXOffset: ") << m_nListEditXOffset;
	dc << _T("\nm_nListEditYOffset: ") << m_nListEditYOffset;
	dc << _T("\nm_nListEditCYOffset: ") << m_nListEditCYOffset;
	dc << _T("\nm_nListEditCXOffset: ") << m_nListEditCXOffset;
	dc << _T("\n");
	}
#endif //_DEBUG

COXGridList::~COXGridList()
	{
	ASSERT_VALID(this);

	m_GridPen.DeleteObject();
	m_TextFont.DeleteObject();
	}


BEGIN_MESSAGE_MAP(COXGridList, CListCtrl)
   //{{AFX_MSG_MAP(COXGridList)
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT_EX(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT_EX(LVN_BEGINDRAG, OnListCtrlNotify)
	ON_WM_CHAR()
	ON_NOTIFY_REFLECT_EX(NM_SETFOCUS, OnSetFocus)
	ON_NOTIFY_REFLECT_EX(NM_KILLFOCUS, OnKillFocus)
	ON_WM_LBUTTONUP()
	ON_MESSAGE(LVM_INSERTITEM, OnInsertItem)
	ON_MESSAGE(LVM_INSERTCOLUMN, OnInsertColumn)
	ON_MESSAGE(LVM_DELETECOLUMN, OnDeleteColumn)
	ON_MESSAGE(LVM_DELETEALLITEMS, OnDeleteAllItems)
	ON_MESSAGE(LVM_DELETEITEM, OnDeleteItem)
	ON_MESSAGE(LVM_FINDITEM, OnFindItem)
	ON_MESSAGE(LVM_GETITEM, OnGetItem)
	ON_MESSAGE(LVM_SETITEM, OnSetItem)
	ON_MESSAGE(LVM_EDITLABEL, OnEditLabel)
	ON_NOTIFY_REFLECT_EX(LVN_BEGINRDRAG, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_DELETEALLITEMS, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_GETDISPINFO, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_INSERTITEM, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGING, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_KEYDOWN, OnListCtrlNotify)
	ON_NOTIFY_REFLECT_EX(LVN_SETDISPINFO, OnListCtrlNotify)
	ON_WM_GETDLGCODE()
	ON_NOTIFY(NM_RELEASEDCAPTURE, 0, OnTrackHeader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COXGridList::Empty()
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Initializes all the data members
	//				This function should only be called at specific moments in time
	//				(e.g. right after construction)
	{
	m_bLastRowWasVisible = FALSE;
	m_bGridLines = FALSE;
	m_bHorizontalGridLines = TRUE;
	m_bVerticalGridLines = TRUE;
	m_bInitialized = FALSE;
	m_bSortable = FALSE;
	m_bCheckable = FALSE;
	m_nCheckStyle = BS_AUTOCHECKBOX;
	m_bAutoEdit = FALSE;
	m_eUseExtendedData = EDNo;
	m_nNumOfCols = 0;
	m_nImageColumn = 0;
	m_lastClickPos = CPoint(0,0);
	m_nEditSubItem = 0;
	m_SelectedRect.SetRectEmpty();
  m_nSortColumn    = -1;
  m_bSortAscending = true;
	}


int CALLBACK COXGridList::GridCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	// --- In  : lParam1 : lParam of the first item to compare
	//			 lParam2 : lParam of the second item to compare
	//			 lParamSort : parem of the sort object (COXGridListSortInfo)
	// --- Out : 
	// --- Returns : A negative value if the first item should precede the second, 
	//				 a positive value if the first item should follow the second, 
	//				 or zero if the two items are equivalent.
	// --- Effect : Compares two items of the control
	//				The COXGridListSortInfo contains information about which subitem to use
	{
	COXGridListSortInfo* pGridListSortInfo = (COXGridListSortInfo*)lParamSort;
	ASSERT(pGridListSortInfo != NULL);
	ASSERT(AfxIsValidAddress(pGridListSortInfo, sizeof(COXGridListSortInfo)));

	int nIndex1, nIndex2;
	CString sItemText1, sItemText2;

	// Lookup the first and second Item
	nIndex1 = pGridListSortInfo->m_pThis->FindOriginalItemData(lParam1);
	nIndex2 = pGridListSortInfo->m_pThis->FindOriginalItemData(lParam2);

	// query the text
	sItemText1 = pGridListSortInfo->m_pThis->GetItemText(nIndex1, pGridListSortInfo->m_nSubIndex);
	sItemText2 = pGridListSortInfo->m_pThis->GetItemText(nIndex2, pGridListSortInfo->m_nSubIndex);

	int iResult;
								
	ColType cType;
	cType = (ColType) (pGridListSortInfo->m_pThis->m_colType.GetAt(pGridListSortInfo->m_nSubIndex));

	if(cType == t_number)
	{
		// Compare the two numbers
		int a1, a2;
		a1 = atoi((const char*) sItemText1);
		a2 = atoi((const char*) sItemText2);

		if(a1 < a2)
			iResult = -1;
		else if (a1 == a2)
			iResult = -1;
		else
			iResult = 1;
	} 
	else if (cType == t_double)
  {
		// Compare the two numbers
		double a1, a2;
		a1 = atol((const char*) sItemText1);
		a2 = atol((const char*) sItemText2);

		if(a1 < a2)
			iResult = -1;
		else if (a1 == a2)
			iResult = -1;
		else
			iResult = 1;
  }
	else if (cType == t_date)
  {
    COleDateTime Date1(1971, 1, 1, 0, 0, 0);
    COleDateTime Date2(1971, 1, 1, 0, 0, 0);

    if (!sItemText1.IsEmpty())
      Date1.ParseDateTime(sItemText1, VAR_DATEVALUEONLY);
    
    if (!sItemText2.IsEmpty())
      Date2.ParseDateTime(sItemText2, VAR_DATEVALUEONLY);

    if(Date1 < Date2)
			iResult = -1;
		else if (Date1 == Date2)
			iResult = -1;
		else
			iResult = 1;

  }
  else if (cType == t_date_time)
  {
    COleDateTime Date1;
    COleDateTime Date2;
    Date1.ParseDateTime(sItemText1);
    Date2.ParseDateTime(sItemText2);

    if(Date1 < Date2)
			iResult = -1;
		else if (Date1 == Date2)
			iResult = -1;
		else
			iResult = 1;

  }
  else
	{
		// Compare both string on a No Case basis
		iResult = sItemText1.CompareNoCase(sItemText2);
	}

  if (!pGridListSortInfo->m_bAscending)
    iResult = -iResult;
	return(iResult);
	}

BOOL COXGridList::SetCheckStateImageList()
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Sets the state image list of this control to three images :
	//				unchecked checkbox, checked checkbox and grayed (3-state) checkbox
	//				Their size is 13 (width) by 11 (height) each
	//				You can easily change this by using 
	//				SetImageList(pStateImages, LVSIL_STATE) with your own list pStateImages
	{
	// The bitmap is read from resource now
	// ... Must find the resource
	//     (Make sure OXGridList.rc is included in your resource file)
	ASSERT(AfxFindResourceHandle(MAKEINTRESOURCE(IDB_STATEIMAGELIST), RT_BITMAP) != NULL);
	// ... Create it
	BOOL bSuccess = m_stateImages.Create(IDB_STATEIMAGELIST,13,0,RGB(255,0,255));
	if (bSuccess)
		{
		m_stateImages.SetBkColor(GetSysColor(COLOR_WINDOW));
		/// ... Attach image list to list control
		SetImageList(&m_stateImages, LVSIL_STATE);
		}
	else
		TRACE0("COXGridList::SetCheckStateImageList : Failed to create the state image list\n");
	return bSuccess;
	}

BOOL COXGridList::DrawGridLines(CDC* pDC, LPDRAWITEMSTRUCT lpDIS)
	// --- In  : pDC : the device context to draw on 
	//			 lpDIS : A long pointer to a DRAWITEMSTRUCT structure that contains
	// 				information about the type of drawing required.
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Draws the vertical and horizontal gridlines with a certain color
	{
	CPen* pOldPen;
	pOldPen = pDC->SelectObject(&m_GridPen); 

	// Paint vertical lines
	if (m_bVerticalGridLines)
		{
		int nNumberOfItems = GetItemCount();
		int xPos = GetColumnWidth(0);
		int nCol = 0;

		// The line will be drawn in an PS_ALTERNATE style
		// To have a better visual effect let them start on an even vertical coordinate
		int nTop = lpDIS->rcItem.top + (lpDIS->rcItem.top % 2);

		int nWidth = lpDIS->rcItem.right - lpDIS->rcItem.left;
		pDC->SetWindowOrg(0,0);

		while(xPos <= nWidth && nCol < m_nNumOfCols)
			{
			pDC->MoveTo(xPos - 1 + lpDIS->rcItem.left, nTop);
			pDC->LineTo(xPos - 1 + lpDIS->rcItem.left, lpDIS->rcItem.bottom);
			nCol++;
			xPos += GetColumnWidth(nCol);
			}
		}

	// Paint horizontal lines
	if (m_bHorizontalGridLines)
		{
		if (lpDIS->rcItem.bottom != 0)
			{
			pDC->MoveTo(0, lpDIS->rcItem.bottom - 1);
			pDC->LineTo(lpDIS->rcItem.right, lpDIS->rcItem.bottom - 1);
			}
		}

	pDC->SelectObject(pOldPen); 

	return TRUE;
	}

void COXGridList::SetRowText(LPDRAWITEMSTRUCT lpDIS, BOOL bShowItemSel)
	// --- In  : lpDIS : A long pointer to a DRAWITEMSTRUCT structure that contains
	// 				information about the type of drawing required.
	//			 bShowItemSel : Whether the item whould be shown as selected or not
	// --- Out : 
	// --- Returns : 
	// --- Effect : Draws the text line of a entire row
	{
	ASSERT_VALID(this);
	CString sItemText;

	// Retrieve the item rectangle size. 
	CRect rectText  = lpDIS->rcItem;

	// Now, start displaying the columns text.
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	int iColumn = 0;

	LVCOLUMN lvc;
  memset (&lvc, 0, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT;
	while(iColumn < GetNumCols() && GetColumn(iColumn, &lvc))
		{
		// get the text to be drawn and calculate its bounding rectangle
		sItemText = GetItemText(lpDIS->itemID, iColumn);
		rectText.right  = rectText.left + GetColumnWidth(iColumn);
		if (pDC->RectVisible(rectText))
			{
			// draws the image of the imagelist attached to the Listcontrol
			// adjusts the rectText rectangle to draw the text behind the image
			if (iColumn == m_nImageColumn)
				DrawImage(pDC, rectText, lpDIS->itemID, bShowItemSel);

			SetColItemText(pDC, sItemText, rectText, lvc.fmt);
			}
		
		// move the left side of the previous rect already forward to
		// be good for the next Text.  The right of the previous bounding
		// rect is the left of the following
		rectText.left = rectText.right;
		iColumn++;
		}

	// Back to the old state
	lpDIS->rcItem.right  = rectText.right;
	lpDIS->rcItem.bottom = rectText.bottom;
	}

BOOL COXGridList::DrawImage(CDC* pDC,  CRect& rectText, int iItemID, BOOL bSelected)
	// --- In  : pDC : DC to draw on
	//			 rectText : Rect in which to draw the images
	//			 iItemID : Index of the item to draw
	//			 bSelected : Whether this item is selected or not
	// --- Out : rectText : Rect in which to draw the text
	// --- Returns : Whether the images could be drawn successfully
	// --- Effect : Draws the state image and the small image of this item
	{
	// only small images can be shown in report mode
	CImageList* pSmallImageList = GetImageList(LVSIL_SMALL);
	CImageList* pStateImageList = GetImageList(LVSIL_STATE);

	LV_ITEM lvI;
	lvI.mask = LVIF_IMAGE | LVIF_STATE;
	lvI.iItem = iItemID;
	lvI.iSubItem   = 0;
	lvI.state = 0;
	lvI.stateMask = ALLSTATEIMAGEMASKS;

	CSize stateImageSize(0,0);
	CSize smallImageSize(0,0);

	// Query the image and state this item is linked with
	if (GetItem(&lvI))
		{
		// We need the width of the images.  It will be used to determine from
		// where we can begin writing the text
		IMAGEINFO imageInfo;

		// First get the size of the small image and the state image
		// ... Convert from one-based to zero-based index
		int nStateIndex = STATEIMAGEMASKTOINDEX(lvI.state) - 1;
		if ((pStateImageList != NULL) && (0 <= nStateIndex))
			{
			pStateImageList->GetImageInfo(nStateIndex, &imageInfo);
			stateImageSize = CRect(imageInfo.rcImage).Size();
			}
		if (pSmallImageList != NULL)
			{
			pSmallImageList->GetImageInfo(lvI.iImage, &imageInfo);
			smallImageSize = CRect(imageInfo.rcImage).Size();
			}

		// Draw a rectangle under both images because it is possible
		// that one (or both) of the images does not occupy the entire space
		CRect imagesRect(rectText.left, rectText.top, 
			rectText.left + smallImageSize.cx + stateImageSize.cx,
			rectText.bottom);
		pDC->FillSolidRect(imagesRect, pDC->GetBkColor());

		// Draw the state image
		if ((pStateImageList != NULL) && (0 <= nStateIndex))
			{
			// Because imageList could be shared between different controls, we have to
			// restore its original background color.
			COLORREF oldColor = pStateImageList->GetBkColor();
			pStateImageList->SetBkColor(pDC->GetBkColor());
			CPoint pos = rectText.TopLeft();
			// Align the image to the center of the line
			if (stateImageSize.cy < rectText.Height())
				pos.y += (rectText.Height() - stateImageSize.cy) / 2;
			VERIFY(pStateImageList->Draw(pDC, nStateIndex, pos, ILD_NORMAL));
			pStateImageList->SetBkColor(oldColor);
			// ... to set the startpoint for text output
			rectText.left += stateImageSize.cx;
			}

		// Draw the small image
		if (pSmallImageList != NULL)
			{
			// Because imageList could be shared between different controls, we have to
			// restore its original background color.
			COLORREF oldColor = pSmallImageList->GetBkColor();
			pSmallImageList->SetBkColor(pDC->GetBkColor());
			CPoint pos = rectText.TopLeft();
			// Align the image to the center of the line
			if (smallImageSize.cy < rectText.Height())
				pos.y += (rectText.Height() - smallImageSize.cy) / 2;
			pSmallImageList->Draw(pDC, lvI.iImage, pos, bSelected ? ILD_BLEND50 : ILD_NORMAL);
			pSmallImageList->SetBkColor(oldColor);
			// ... to set the startpoint for text output
			rectText.left += smallImageSize.cx;
			}
		}
	else
		{
		TRACE0("COXGridList::DrawImage : Failed to get item info\n");
		return FALSE;
		}

	return TRUE;
	}
   
BOOL COXGridList::ChangeItemText(int nItem, int nSubItem, LPCTSTR pszText)
	// --- In  : nItem: Index of the item
	//			 nSubItem : Index of the subitem
	//			 pszText : Text to set
	// --- Out : 
	// --- Returns : Whether the txet of the specified subitem could be set successfully
	// --- Effect : This functions sets the text of a subitem by calling SetItemText()
	//				and it also takes care of the pre- and post-notifications :
	//				LVN_ITEMCHANGING and LVN_ITEMCHANGED
	{
	ASSERT((0 <= nItem) && (nItem < GetItemCount()));
	ASSERT((0 <= nSubItem) && (nSubItem < GetNumCols()));
	ASSERT(pszText != NULL);

	// Windows only send a notification if the label text changes,
	// not if the text of another subitem changes.
	// We will add the latter functionality here
	BOOL bSuccess = FALSE;

	if (nSubItem != 0)
		{
		// Notify that item is about to be changed
		NM_LISTVIEW nmListView;

		memset(&nmListView, 0, sizeof(NM_LISTVIEW));
		nmListView.hdr.hwndFrom = m_hWnd;
		nmListView.hdr.idFrom = GetDlgCtrlID();
		nmListView.hdr.code = LVN_ITEMCHANGING;
		nmListView.iItem = nItem;
		nmListView.iSubItem = nSubItem;
		nmListView.uChanged = LVIF_TEXT;
		nmListView.lParam = GetItemData(nItem);
		bSuccess = !GetParent()->SendMessage(WM_NOTIFY, 
			(WPARAM)nmListView.hdr.idFrom, (LPARAM)&nmListView);
  		if (!bSuccess)
			return FALSE;
		}

	// Now change the item
	bSuccess = SetItemText(nItem, nSubItem, pszText);

	if (bSuccess && (nSubItem != 0))
		{
		// Notify that item has changed
		NM_LISTVIEW nmListView;
		memset(&nmListView, 0, sizeof(NM_LISTVIEW));
		nmListView.hdr.hwndFrom = m_hWnd;
		nmListView.hdr.idFrom = GetDlgCtrlID();
		nmListView.hdr.code = LVN_ITEMCHANGED;
		nmListView.iItem = nItem;
		nmListView.iSubItem = nSubItem;
		nmListView.uChanged = LVIF_TEXT;
		nmListView.lParam = GetItemData(nItem);
		GetParent()->SendMessage(WM_NOTIFY, 
			(WPARAM)nmListView.hdr.idFrom, (LPARAM)&nmListView);
		}

	return bSuccess;
	}

DWORD COXGridList::GetOriginalItemData(int nItem) 
	// --- In  : nItem: index of the item
	// --- Out : 
	// --- Returns : The original data associated with this item or 0 when nItem is not valid
	// --- Effect : This functions calls the underlying window procedure directly 
	//			    instead of sending a message
	{
	ASSERT(::IsWindow(m_hWnd));
	LV_ITEM lvi;
	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;
	if(DefWindowProc(LVM_GETITEM, 0, (LPARAM)&lvi))
		return (DWORD)lvi.lParam;
	else
		// ... Not a valid nItem, return 0
		return 0;
	}

BOOL COXGridList::SetOriginalItemData(int nItem, DWORD dwData)
	// --- In  : nItem: index of the item
	//			 dwData : Data to set
	// --- Out : 
	// --- Returns : Success or not
	// --- Effect : This functions calls the underlying window procedure directly 
	//			    instead of sending a message
	{
	ASSERT(::IsWindow(m_hWnd)); 
	LV_ITEM lvi;
	memset(&lvi, 0, sizeof(LV_ITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;
	lvi.lParam = (LPARAM)dwData; 
	return DefWindowProc(LVM_SETITEM, 0, (LPARAM)&lvi);
	}

int COXGridList::FindOriginalItemData(DWORD dwItemData)
	// --- In  : dwItemData : Original data to search for
	// --- Out : 
	// --- Returns : THe index of the item that has this original data associated with it
	// --- Effect : This functions calls the underlying window procedure directly 
	//			    instead of sending a message
	{
	ASSERT(::IsWindow(m_hWnd)); 
	LV_FINDINFO lvfi;
	memset(&lvfi, 0, sizeof(LV_FINDINFO));
	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = (LPARAM)dwItemData;
	return (int) DefWindowProc(LVM_FINDITEM, (WPARAM)-1, (LPARAM)&lvfi); 
	}

int COXGridList::GetCheckItemFromPoint(const CPoint& point) const
	// --- In  : point : Coordinate th check
	// --- Out : 
	// --- Returns : The item of which the check box contains the specified point
	//				 or -1 otherwise
	// --- Effect : 
	{
	if (!GetCheckable())
		// Checkability not enabled, can never have checked anything
		return -1;

	CImageList* pStateImageList = GetImageList(LVSIL_STATE);
	if (pStateImageList != NULL)
		{
		IMAGEINFO imageInfo;
		pStateImageList->GetImageInfo(0, &imageInfo);
		CRect checkImageRect;
		// ... Get the left
		if(!GetItemRect(0, checkImageRect, LVIR_BOUNDS))
			{
			// ... Cannot even get the rect of the first item : no items in control
			ASSERT(GetItemCount() == 0);
			return -1;
			}
		// ... Adjust the width
		checkImageRect.right = checkImageRect.left + (imageInfo.rcImage.right - imageInfo.rcImage.left);
		// Adjust the begin position if the images are not displayed in the label column
		for (int nIndex = 0; nIndex < m_nImageColumn; nIndex++)
			{
			checkImageRect += CPoint(GetColumnWidth(nIndex), 0);
			}
		// ... Check left and right limits (top and bottom are not important)
		if ((checkImageRect.left <= point.x) && (point.x < checkImageRect.right))
			return HitTest(point);
		}
	return -1;
	}

BOOL COXGridList::PostEditLabel(int nItem, int nSubItem)
	// --- In  : nItem : The Item to edit
	//			 nSubItem : The subitem to edit
	// --- Out : 
	// --- Returns : Whether the posting was successful
	// --- Effect : Post a message to begins in-place editing of the specified list view subitem
	//				To edit the first editable subitem of a specified item use
	//				nSubItem = -1
	{
	ASSERT(::IsWindow(m_hWnd)); 

	// If nSubItem == -1, search the first editable subitem
	int nSubItemIndex = 0;
	while ((nSubItem == -1) && (nSubItemIndex < GetNumCols()))
		{
		if (GetEditable(nSubItemIndex))
			nSubItem = nSubItemIndex;
		nSubItemIndex++;
		}

	if (nSubItem == -1)
		{
		TRACE0("COXGridList::PostEditLabel : No editable column is found, ignoring edit request\n");
		return FALSE;
		}

	return (BOOL)::PostMessage(m_hWnd, LVM_EDITLABEL, nItem, nSubItem);
	}

BOOL COXGridList::SearchNextEditItem(int nItemOffset, int nSubItemOffset, int& nItem, int& nSubItem)
	// --- In  : nItemOffset : In which direction to change the item index
	//			 nSubItemOffset : In which direction to change the sub item index
	//			 nItem : The current item index
	//			 nSubItem : The current subitem index
	// --- Out : nItem : The new item index
	//			 nSubItem : The new subitem index
	// --- Returns : Whether a new and different item or subitem was found
	// --- Effect : Computes the next item and subitem
	{
	ASSERT((-1 <= nItemOffset) && (nItemOffset <= 1));
	ASSERT((-1 <= nSubItemOffset) && (nSubItemOffset <= 1));

	int nNextItem = -1;
	int nNextSubItem = -1;

	// Find next item
	if (nItemOffset != 0)
		{
		nNextItem = nItem + nItemOffset;
		if (nNextItem < 0)
			// ... Wrap to end
			nNextItem = GetItemCount() - 1;
		if (GetItemCount() <= nNextItem)
			// ... Wrap to begin
			nNextItem = 0;
		if (nNextItem == nItem)
			// ... Item index must really have changed, otherwise ignore movement
			nNextItem = -1;
		}

	// Find next subitem
	if (nSubItemOffset != 0)
		{
		// Get the next (or previous) editable subitem (wrap around the end)
		int nSubItemIndex = nSubItem + nSubItemOffset;
		while ((nNextSubItem == -1) && (nSubItemIndex < GetNumCols()) && 
			   (0 <= nSubItemIndex))
			{
			if (GetEditable(nSubItemIndex))
				nNextSubItem = nSubItemIndex;
			nSubItemIndex += nSubItemOffset;
			}

		// Wrap around to begin
		if (nSubItemOffset == 1)
			nSubItemIndex = 0;
		else
			nSubItemIndex = GetNumCols() - 1;

		// Search until we reach the current edited subitem
		while ((nNextSubItem == -1) && (nSubItemIndex != m_nEditSubItem))
			{
			if (GetEditable(nSubItemIndex))
				nNextSubItem = nSubItemIndex;
			nSubItemIndex += nSubItemOffset;
			}
		}

	// Fill in result
	if (nNextItem != -1)
		nItem = nNextItem;
	if (nNextSubItem != -1)
		nSubItem = nNextSubItem;

	// Have item or subitem changed
	return ((nNextItem != -1) || (nNextSubItem != -1));
	}

void COXGridList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	// ... Store the old state
	BOOL bLastRowWasVisible = IsLastRowVisible();

	// ... Call base class implementation
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	if (!bLastRowWasVisible && IsLastRowVisible())
		// ... Last row has just appeared
		OnLastRowAppear();
	}

void COXGridList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	if (GetCheckable() && (nChar == VK_SPACE))
		{
		int nIndex = GetCurFocus();
		if (0 <= nIndex)
			OnCheck(nIndex);
		}

	if (nChar == VK_INSERT || nChar == VK_F2)
		{
		// Edit first editable subitem of the focussed item
		int nIndex = GetCurFocus();
		if (0 <= nIndex)
			EditLabel(nIndex, -1);
		}
	// ... Store the old state
	BOOL bLastRowWasVisible = IsLastRowVisible();

	// ... Call base class implementation
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	m_bLastRowWasVisible = IsLastRowVisible();
	if (!bLastRowWasVisible && m_bLastRowWasVisible)
		{
		// ... Last row has just appeared
		OnLastRowAppear();
		m_bLastRowWasVisible = TRUE;
		}
/***
	if (nChar == VK_RETURN && !m_bEdit)
			{
			//send notification
			// Notify that item has changed
			NM_LISTVIEW nmListView;
			memset(&nmListView, 0, sizeof(NM_LISTVIEW));
			nmListView.hdr.hwndFrom = m_hWnd;
			nmListView.hdr.idFrom = GetDlgCtrlID();
			nmListView.hdr.code = NM_RETURN;
			//nmListView.iItem = GetCurSel();
			//nmListView.iSubItem = nSubItem;
			//nmListView.lParam = GetItemData(nItem);
			GetParent()->SendMessage(WM_NOTIFY, 
				(WPARAM)nmListView.hdr.idFrom, (LPARAM)&nmListView);
			}
			****/
	}

void COXGridList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	// ... Store the old state
	BOOL bLastRowWasVisible = IsLastRowVisible();

	// ... Call base class implementation
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);

	if ((!bLastRowWasVisible || !m_bLastRowWasVisible) && IsLastRowVisible())
		{
		// ... Last row has just appeared
		OnLastRowAppear();
		m_bLastRowWasVisible = TRUE;
		}
	if(nChar == VK_ESCAPE && m_bEdit) 
		m_bEdit = FALSE;

	if(nChar == VK_RETURN) 
		{
		if(m_bEdit) 
			{
			//don't handle this, it just endet editing
			m_bEdit = FALSE;
			} 
		}

	}

BOOL COXGridList::OnEraseBkgnd(CDC* pDC) 
	{
	// When using the ListCtrl we do not need the space covered	by
	// the added rows to be painted by the OnEraseBkgnd of the CListCtrl.
	// This avoids some annoying flicker 

	// Exclude the area occupied by the rows from the region to be erased
	// we are drawing all that stuff ourselves
	int nCount;
	BOOL bReturn;
	CRect ItemRect;
	CRect topItemRect;
	CRect ClipRect;

	nCount = GetItemCount();
	if (nCount > 0)
		{
		pDC->GetClipBox(ClipRect);
		GetItemRect(nCount - 1, ItemRect, LVIR_BOUNDS);
		GetItemRect(0, topItemRect, LVIR_BOUNDS);
		// Include all the rows
		ItemRect.top = topItemRect.top;
		// ... Exclude area that should not be filled with background
		pDC->ExcludeClipRect(ItemRect);
		}

	bReturn = CListCtrl::OnEraseBkgnd(pDC);

	if (nCount > 0)
		{
		// Make the intersection between the Clipping rect and the whole rect
		// covered by the rows, so we're certainly not drawing to much again
		ItemRect &= ClipRect;
		// ... Include area that was not filled with background
		InvalidateRect(ItemRect, FALSE);
		}
	
	return bReturn;
	}


void COXGridList::SetColItemText(CDC* pDC, CString& stColText, CRect& rectText, UINT nJustify)
	// --- In  : pDC : the device context to draw on
	//			 sCellText : the text to be drawn
	//			 rectText : the bounding rect of the text 
	// --- Out : 
	// --- Returns :
	// --- Effect : Draws text on a DC within a rectangle
	{
	static TCHAR szTChar[300];
	static CSize TextSize;
	static CSize DotsTextWidth = pDC->GetTextExtent(_T("..."), 3);

	// 1 - Calculate the width of the column in pixels and subtract
	//     '6' from it to take into account the edges of the column
	//     and the added '2' to the left margin in the ExtTextOut()'
	//     call.
	//
	// 2 - Get the size of the text in pixels.

	int nColWidth = rectText.right - rectText.left - 6;
	_tcscpy(szTChar, stColText);
	int len = stColText.GetLength();
	TextSize = pDC->GetTextExtent(szTChar, len);
	//	int nColTextWidth = GetStringWidth(stColText);


	// Now, if the width of the colum is LESS than the length of
	// the string in pixels, truncate the string and add '...' to
	// the end of the string to let the user know of it.

	/*
  if (nColWidth < TextSize.cx)
		{
		while (nColWidth < (TextSize.cx + DotsTextWidth.cx) && TextSize.cx)
			{
			szTChar[--len] = _T('\x0');
			TextSize = pDC->GetTextExtent(szTChar, len);
			}
		
		_tcscat(szTChar, _T("..."));
		len += 3;
		}
    */
	

    // Align the text in the whole grid and
	// calculate the point adjusted to the alignment
	/*
  int nInsertPoint = 0;
	switch(nJustify & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_LEFT:
			pDC->SetTextAlign(TA_LEFT);
			nInsertPoint = rectText.left + 4;
			break;
		case LVCFMT_RIGHT:
			pDC->SetTextAlign(TA_RIGHT);
			nInsertPoint = rectText.right - 4;
			break;
		case LVCFMT_CENTER:
			pDC->SetTextAlign(TA_CENTER);
			nInsertPoint = rectText.left + ((rectText.right - rectText.left)/2);
			break;
		default:
			ASSERT(FALSE);
			break;
		}

  // Draw the text
	pDC->ExtTextOut(nInsertPoint, rectText.top+1,
					ETO_OPAQUE | ETO_CLIPPED, rectText,
					szTChar, len, NULL);
  */

	UINT nFormat = DT_SINGLELINE | DT_PATH_ELLIPSIS | DT_BOTTOM;
  switch(nJustify & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_LEFT:
      nFormat |= DT_LEFT;
			break;
		case LVCFMT_RIGHT:
      nFormat |= DT_RIGHT;
			break;
		case LVCFMT_CENTER:
      nFormat |= DT_CENTER;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
   
  pDC->FillSolidRect(rectText, pDC->GetBkColor());  
  rectText.left += 4;
  rectText.right -= 4;

  pDC->DrawText(szTChar, len, rectText, nFormat);

  rectText.left -= 4;
  rectText.right += 4;
	}

COXGridList::EUseExtendedData COXGridList::GetExtendedDataState()
	// --- In  : 
	// --- Out : 
	// --- Returns : The extended data state at this moment
	// --- Effect : 
	{
	ASSERT(m_eFirstED <= m_eUseExtendedData);
	ASSERT(m_eUseExtendedData <= m_eLastED);
	return m_eUseExtendedData;
	}

void COXGridList::SetExtendedDataState(EUseExtendedData eUseExtendedData)
	// --- In  : eUseExtendedData : The new extended data state
	// --- Out : 
	// --- Returns : 
	// --- Effect : Only certain state changes are allowed
	//				EDNo -> EDAdding -> EDYes -> EDRemoving -> ...
	{
#ifdef _DEBUG
	// ... Range check requested state
	ASSERT(m_eFirstED <= eUseExtendedData);
	ASSERT(eUseExtendedData <= m_eLastED);
	// ... Range check current state
	ASSERT(m_eFirstED <= m_eUseExtendedData);
	ASSERT(m_eUseExtendedData <= m_eLastED);
	
	// Only certain state transistions are valid
	// EDNo -> EDAdding -> EDYes -> EDRemoving -> ...
	// ... EDNo -> EDNo, EDAdding
	if (m_eUseExtendedData == EDNo)
		ASSERT((eUseExtendedData == EDNo) || (eUseExtendedData == EDAdding));
	// ... EDYes -> EDYes, EDRemoving
	if (m_eUseExtendedData == EDYes)
		ASSERT((eUseExtendedData == EDYes) || (eUseExtendedData == EDRemoving));
	// ... EDAdding -> EDAdding, EDYes
	if (m_eUseExtendedData == EDAdding)
		ASSERT((eUseExtendedData == EDAdding) || (eUseExtendedData == EDYes));
	// ... EDRemoving -> EDRemoving, EDNo
	if (m_eUseExtendedData == EDRemoving)
		ASSERT((eUseExtendedData == EDRemoving) || (eUseExtendedData == EDNo));
#endif // _DEBUG

	m_eUseExtendedData = eUseExtendedData;
	}

BOOL COXGridList::AddExtendedData()
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it was successful or not
	// --- Effect : Adds extended data (COXGridListData objects) to each item
	//              if this was not already done
	{
	if (GetExtendedDataState() == EDYes)
		// Already using extended data, nothing to do
		return TRUE;

	// ... Extended data should not be in use, nor in an intermediate state
	ASSERT(GetExtendedDataState() == EDNo);

	// Mark that we are adding
	SetExtendedDataState(EDAdding);

	// Iterate all present rows and create COXGridListData objects 
	// and attach them to each item
	int nIndex = 0;
	DWORD dwUserData;
	COXGridListData* pGridListData;
	for ( ; nIndex < GetItemCount(); nIndex++)
		{
		dwUserData = GetOriginalItemData(nIndex);
		pGridListData = new COXGridListData(dwUserData);
		// ... Must succeed, may not be cancelled
		VERIFY(SetOriginalItemData(nIndex, (DWORD)pGridListData));
		}

	// Mark that we have finished adding
	SetExtendedDataState(EDYes);

	return TRUE;
	}

BOOL COXGridList::RemoveExtendedData()
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether it was successful or not
	// --- Effect : Removes extended data (COXGridListData objects) from each item
	//              if this was not already done
	{
	if (GetExtendedDataState() == EDNo)
		// Not using extended data, nothing to do
		return TRUE;

	// ... Extended data should be in use
	ASSERT(GetExtendedDataState() == EDYes);

	// Mark that we are removing
	SetExtendedDataState(EDRemoving);

	// Iterate all present rows and delete the COXGridListData objects 
	int nIndex = 0;
	DWORD dwUserData;
	COXGridListData* pGridListData;
	for ( ; nIndex < GetItemCount(); nIndex++)
		{
		pGridListData = (COXGridListData*)GetOriginalItemData(nIndex);
		ASSERT(pGridListData != NULL);
		ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));
		dwUserData = pGridListData->m_dwUserData;
		delete pGridListData;
		// ... Must succeed, may not be cancelled
		VERIFY(SetOriginalItemData(nIndex, dwUserData));
		}

	// Mark that we have finished removing
	SetExtendedDataState(EDNo);

	return TRUE;
	}

BOOL COXGridList::AdjustNotification(NM_LISTVIEW* pNMListView)
	// --- In  : pNMListView
	// --- Out : 
	// --- Returns : Whether this message should be eaten (not passed on to the parent)
	// --- Effect : Adjusts the pNMListView to reflect the correct lParam value
	{
	// ... Do not eat the message by default
	BOOL bEaten = FALSE;

	if ((GetExtendedDataState() != EDYes) && (GetExtendedDataState() != EDNo))
		{
		// ... Item in intermediate change, do not pass this message to the parent, eat it
		bEaten = TRUE;
		}
	else if (GetExtendedDataState() == EDYes)
		{
		// ... Substitute the lParam for the user data by dereferencing COXGridListData
		COXGridListData* pGridListData;
		pGridListData = (COXGridListData*)GetOriginalItemData(pNMListView->iItem);
		if (pGridListData != NULL)
			{
			ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));
			pNMListView->lParam = pGridListData->m_dwUserData;
			}
		}
	
	return bEaten;
	}

void COXGridList::RefreshFocusItem()
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Invalidates the visible item that has focus
	{
	ASSERT_VALID(this);
	int nFocusItem = GetCurFocus();
	if (nFocusItem != -1)
		RedrawItems(nFocusItem, nFocusItem);
	}

void COXGridList::RefreshSelItems()
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : Invalidates the visible itess that are selected
	{
	ASSERT_VALID(this);

	// ... Item with an index greater than nMaxVisibleItem are surely not visible
	int nMaxVisibleItem = GetTopIndex() + GetCountPerPage();
	// ... We start searching for a selected item from the first visible item
	int nSelItem = GetNextItem(GetTopIndex() - 1, LVNI_SELECTED);
	while ((0 <= nSelItem) && (nSelItem <= nMaxVisibleItem))
		{
		RedrawItems(nSelItem, nSelItem);
		nSelItem = GetNextItem(nSelItem, LVNI_SELECTED);
		}
	}

// private:
// ==========================================================================

int COXGridList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ... Window must be valid
	ASSERT(m_hWnd != NULL);
	ASSERT(::IsWindow(m_hWnd));

	// ... List control must be in report view and owner drawn
	ASSERT((GetStyle() & LVS_REPORT) == LVS_REPORT);
	ASSERT((GetStyle() & LVS_OWNERDRAWFIXED) == LVS_OWNERDRAWFIXED);

	// Initialize this control
	InitGrid();

	return 0;
	}

afx_msg LRESULT COXGridList::OnInsertItem(WPARAM wParam, LPARAM lParam)
	{
	// ... Use original LV_ITEM by default;
	const LV_ITEM* pLviOriginal = (const LV_ITEM*)lParam;
	BOOL bUseCopy = FALSE;
	LV_ITEM lviCopy;

	// Extended should be in use or not, not in an intermediate state
	ASSERT((GetExtendedDataState() == EDNo)  || (GetExtendedDataState() == EDYes));
	if (GetExtendedDataState() == EDYes)
		{
		// We're using lParam ourselves (for sorting), so change the mask
		// to tell the List ctrl this member is also valid
		// We use copy because pItem is const
		if (!bUseCopy)
			{
			// Copy not yet used : initialise it first
			ASSERT(AfxIsValidAddress(pLviOriginal, sizeof(LV_ITEM)));
			memcpy(&lviCopy, pLviOriginal, sizeof(LV_ITEM));
			bUseCopy = TRUE;
			}

		if ((lviCopy.mask & LVIF_PARAM) != LVIF_PARAM)
			{
			lviCopy.lParam = 0;
			lviCopy.mask |= LVIF_PARAM;
			}
		lviCopy.lParam = (DWORD)new COXGridListData(lviCopy.lParam);
		}

	// If the control is checkable but no image state was specified, add it
	if (GetCheckable() && 
		(((pLviOriginal->mask & LVIF_STATE) != LVIF_STATE) || !(pLviOriginal->stateMask & ALLSTATEIMAGEMASKS)) )
		{
		// The state is not filled out, so we set it to not checked by default
		// We have to tell the List ctrl the state member is also valid
		// We use copy because pItem is const
		if (!bUseCopy)
			{
			// Copy not yet used : initialise it first
			ASSERT(AfxIsValidAddress(pLviOriginal, sizeof(LV_ITEM)));
			memcpy(&lviCopy, pLviOriginal, sizeof(LV_ITEM));
			bUseCopy = TRUE;
			}

		// ... Remove all state image masks and add our (non checked)
		if ((lviCopy.mask & LVIF_STATE) == LVIF_STATE)
			{
			// ...State already used, remove all state images
			lviCopy.state &= ~ALLSTATEIMAGEMASKS;
			}
		else
			{
			// ... State not yet used, initialize it and use it now
			lviCopy.state = 0;
			lviCopy.stateMask = 0;
			lviCopy.mask |= LVIF_STATE;
			}
		lviCopy.state |= INDEXTOSTATEIMAGEMASK(0 + 1);
		lviCopy.stateMask |= ALLSTATEIMAGEMASKS;
		}
	
	// pass the message and the (changed) item struct directly to the windows
	// list ctrl, otherwise we get a recursive call of this function
	return DefWindowProc(LVM_INSERTITEM, wParam, bUseCopy ? (LPARAM)&lviCopy : lParam);
	}

afx_msg LRESULT COXGridList::OnInsertColumn(WPARAM wParam, LPARAM lParam)
	{
	// pass the message and the changed item struct directly to the windows
	// list ctrl otherwise we get a recursive call of this function
	DWORD dwIndex = DefWindowProc(LVM_INSERTCOLUMN, wParam, lParam);

	if (dwIndex != -1)
		{
		// success, so increase the number of columns
		m_nNumOfCols++;
		// Adjust our array of editable columns
		// Disallow editing by default
		m_rgbEditable.InsertAt(dwIndex, (WORD)FALSE);
		ASSERT(GetNumCols() == m_rgbEditable.GetSize());

		// Set String type as default
		m_colType.InsertAt(dwIndex, (WORD) t_string);
		ASSERT(GetNumCols() == m_rgbEditable.GetSize());

		// Workaround for Windows bug :
		//  Window does not repaint the list control after a column has been inserted
		//  So the old text is still shown.  We invalidate it now.
		Invalidate();
		}

	return dwIndex;
	}

afx_msg LRESULT COXGridList::OnDeleteColumn(WPARAM wParam, LPARAM lParam)
	{
	// pass the message and the changed item struct directly to the windows
	// list ctrl otherwise we get a recursive call of this function
	BOOL bSuccess = DefWindowProc(LVM_DELETECOLUMN, wParam, lParam);

	if (bSuccess)
		{
		// Success, so decrease the number of columns
		m_nNumOfCols--;
		// Adjust our array of editable columns
		// Disallow editing by default
		m_rgbEditable.RemoveAt(wParam);
		ASSERT(GetNumCols() == m_rgbEditable.GetSize());

		// Workaround for Windows bug :
		//  Window does not repaint the list control after a column has been deleted
		//  So the old text is still shown.  We invalidate it now.
		Invalidate();
		}

	return bSuccess;
	}

BOOL COXGridList::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;
	// ... Do not eat this message by default
	BOOL bEaten = FALSE;
	int nItem = pDispInfo->item.iItem;


	HWND hWndEdit = (HWND)::SendMessage(m_hWnd, LVM_GETEDITCONTROL, 0, 0L);
	// Normally m_gridEdit.GetSafeHwnd() == NULL but in some rare situations this is not true.
	// The normal behaviour of notification messages is this : First Windows sends the notification
	// message to the parent of the window/control that has to be notified.  This parent first sends 
	// this notification message to the control itself.  If the control does not handle the notification
	// message, the parent will try to handle it.  But in the occasion that this parent is a MFC 
	// CControlBar, we noticed that, if the control does not handle the notification message, CControlBar::
	// WindowProc(...) passes this message on to its OWNER = the mainframe in most cases.
	// See the following code fragment :
	//
	//	LRESULT CControlBar::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
	//		{
	//		ASSERT_VALID(this);
	//
	//		LRESULT lResult;
	//		switch (nMsg)
	//		{
	//		case WM_NOTIFY:
	//		case WM_COMMAND:
	//		case WM_DRAWITEM:
	//		case WM_MEASUREITEM:
	//		case WM_DELETEITEM:
	//		case WM_COMPAREITEM:
	//		case WM_VKEYTOITEM:
	//		case WM_CHARTOITEM:
	//			// send these messages to the owner if not handled
	//			if (OnWndMsg(nMsg, wParam, lParam, &lResult))
	//				return lResult;
	//			else
	//				return GetOwner()->SendMessage(nMsg, wParam, lParam);
	//		}
	//
	//		// otherwise, just handle in default way
	//		lResult = CWnd::WindowProc(nMsg, wParam, lParam);
	//		return lResult;
	//	}
	// 
	// The result of this implementation causes the mainframe to get the notification message and
	// thsi mainframe wil first pass the message to the control, which then receives this message
	// for the second time.  We'll test whether the editcontrol is allready subclassed. If so,
	// return immediately and in case of the mainframe which sent us this second message, the message
	// passing will stop there, if it's not handled.

	// Therefore we skip it here.  
	if (m_gridEdit.GetSafeHwnd() == hWndEdit)
		return bEaten;
	
	ASSERT(m_gridEdit.GetSafeHwnd() == NULL);
	m_gridEdit.Initialize();
	m_gridEdit.SubclassWindow(hWndEdit);

	int nFoundItem;
	CRect subItemRect;
	
	if (!GetSubItemFromPoint(m_lastClickPos, nFoundItem, m_nEditSubItem, subItemRect))
		{
		// The user did not click on a valid subitem, nothing to do
		// ... Prevent editing by returning TRUE
		*pResult = TRUE;
		// ... Handled this message, do not pass on to the parent
		bEaten = TRUE;
		return bEaten;
		}

	ASSERT(0 <= m_nEditSubItem);
	ASSERT(m_nEditSubItem < GetNumCols());
	if (!GetEditable(m_nEditSubItem))
		{
		// Editing for this column is not allowed
		// ... Prevent editing by returning TRUE
		*pResult = TRUE;
		// ... Handled this message, do not pass on to the parent
		bEaten = TRUE;
		return bEaten;
		}

	if (nFoundItem != pDispInfo->item.iItem)
		{
		// Normally (nFoundItem == pDispInfo->item.iItem) but in the rare situation that
		// m_lastClickPos does not point to the focused item this is not true
		// (e.g. state change notification aborted the setting of the focus)
		// Therfore we assign it here.
		nFoundItem = pDispInfo->item.iItem;
		subItemRect = GetRectFromSubItem(nFoundItem, m_nEditSubItem);
		}

	// Set the edit window left pos to the left of the corresponding column
	CPoint ptEdit = subItemRect.TopLeft();
	if (ptEdit.x < 0)
		// ... Never let the edit control start left of the left border
		ptEdit.x = 0;
	m_gridEdit.SetWindowPos(CPoint(ptEdit.x + m_nListEditXOffset, ptEdit.y + m_nListEditYOffset));
	m_gridEdit.SetWindowHeight(subItemRect.Height() + m_nListEditCYOffset);
	m_gridEdit.SetWindowWidth(subItemRect.Width() + m_nListEditCXOffset);

	// When the label is not completely visible, we have to adjust the width of the edit control
	// Because this adjustment is less than optimal, we only use it when absolutely needed
	CRect labelRect;
	VERIFY(GetItemRect(nFoundItem, labelRect, LVIR_LABEL));
	if (labelRect.left < 0)
		m_gridEdit.AdjustWindowWidth(m_nListEditCXOffset);

	// Set the new window text to the contents of the corresponding column
	CString sText = GetItemText(nItem, m_nEditSubItem);
	m_gridEdit.SetDeferedWindowText(sText);

	// Remove the selection of all items except the focused item
	int nSelItem = -1;
	while((nSelItem = GetNextItem(nSelItem, LVNI_SELECTED)) != -1)
		{
		if (nSelItem != nItem)
			SetItemState(nSelItem, 0, LVNI_SELECTED);
		}

	//remember, I'm editing
	m_bEdit = TRUE;
	m_bModified = TRUE;

	return bEaten;
	}

BOOL COXGridList::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// ... Never eat this message
	BOOL bEaten = FALSE;
	*pResult = 0;
	
	// If not cancelled, set the text in the correct column
	if ((pDispInfo->item.pszText != NULL) && (pDispInfo->item.iItem != -1))
		{
		if (!ChangeItemText(pDispInfo->item.iItem, m_nEditSubItem, pDispInfo->item.pszText))
			// It the setting of the new text was prevented by LVN_ITEMCHANGING, do not continue
			return bEaten;
		}

	// If the editing was ended by pressing the a key
	// start editing the next (or previous) subitem
	UINT nChar;
	BOOL bShift;
	BOOL bCtrl;
	if (m_gridEdit.GetEndKey(nChar, bShift, bCtrl))
		{
		int nItem = pDispInfo->item.iItem;
		int nSubItem = m_nEditSubItem;
		int nItemOffset = 0;
		int nSubItemOffset = 0;

		// Switch different possible keys
		if ((nChar == VK_TAB) && !bShift)
			// ... Next subitem
			nSubItemOffset = 1;
		if ((nChar == VK_TAB) && bShift)
			// ... Previuos subitem
			nSubItemOffset = -1;
		if (nChar == VK_INSERT)
			// ... Next subitem
			nSubItemOffset = 1;
		if (nChar == VK_UP)
			// ... Previous item
			nItemOffset = -1;
		if (nChar == VK_DOWN)
			// ... Next item
			nItemOffset = 1;

		// ... Always edit a subitem, even when it is the same as before
		SearchNextEditItem(nItemOffset, nSubItemOffset, nItem, nSubItem);
		// ... Make sure the item is visible before we edit it
		EnsureVisible(nItem, nSubItem, FALSE);
		PostEditLabel(nItem, nSubItem);
		m_bEdit = FALSE;
		}

	return bEaten;
	}

void COXGridList::OnLButtonDown(UINT nFlags, CPoint point) 
	{
	// Store last point that was clicked
	m_lastClickPos = point;

	m_bEdit = FALSE;
	// Check whether the state icon was clicked
	int nCheckItem = GetCheckItemFromPoint(point);
	if (0 <= nCheckItem)
		{
		OnCheck(nCheckItem);
		// If we do not have focus, get it
		if (GetFocus() != this)
			SetFocus();
		}
	else
		CListCtrl::OnLButtonDown(nFlags, point);
	}

void COXGridList::OnDestroy() 
	{
	// Make sure list control is not sortable anymore and thus all
	// COXGridListData objects are cleaned up
	VERIFY(SetSortable(FALSE));
	UseItemColors(false);

	// ... Re-initialize all the data members
	Empty();

	// Call base class implmentation
	CListCtrl::OnDestroy();
	}

afx_msg LRESULT COXGridList::OnDeleteAllItems(WPARAM wParam, LPARAM lParam)
	{
	// First destroy all COXGridListData objects (if any)
	EUseExtendedData eUseExtendedData = GetExtendedDataState();
	if (eUseExtendedData == EDYes)
		VERIFY(RemoveExtendedData());

	// Pass the message directly to the windows
	LRESULT result = DefWindowProc(LVM_DELETEALLITEMS, wParam, lParam);

	// Set the extended data state back to the original state
	if (eUseExtendedData == EDYes)
		VERIFY(AddExtendedData());

	return result;
	}

afx_msg LRESULT COXGridList::OnDeleteItem(WPARAM wParam, LPARAM lParam)
	{
	// First delete the COXGridListData object if one is associated with this item
	// ... Extended should be in use or not, not in an intermediate state
	ASSERT((GetExtendedDataState() == EDNo)  || (GetExtendedDataState() == EDYes));
	if (GetExtendedDataState() == EDYes)
		{
		COXGridListData* pGridListData;
		pGridListData = (COXGridListData*)GetOriginalItemData((int)wParam);
		ASSERT(pGridListData != NULL);
		ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));
		delete pGridListData;
		}

	// Pass the message directly to the windows
	return DefWindowProc(LVM_DELETEITEM, wParam, lParam);
	}

afx_msg LRESULT COXGridList::OnFindItem(WPARAM wParam, LPARAM lParam)
	{
	int nStart = (int)wParam;
	const LV_FINDINFO* plvfi = (const LV_FINDINFO*)lParam;
	ASSERT(AfxIsValidAddress(plvfi, sizeof(LV_FINDINFO)));

	// If the control is not sortable or we are not searching for an LPARAM
	// then let the default implementation handle it
	// ... Extended should be in use or not, not in an intermediate state
	ASSERT((GetExtendedDataState() == EDNo)  || (GetExtendedDataState() == EDYes));
	if ((GetExtendedDataState() == EDNo) || ((plvfi->flags & LVFI_PARAM) != LVFI_PARAM))
		{
		// Pass the message directly to the windows
		return DefWindowProc(LVM_FINDITEM, wParam, lParam);
		}
	else
		{
		// We will iterate all the items and look for the requested
		// LPARAM by dereferencing all the COXGridListData objects
		int nIndex = nStart + 1;
		int nLastIndex = GetItemCount() - 1;
		COXGridListData* pGridListData;
		for ( ; nIndex <= nLastIndex; nIndex++)
			{
			pGridListData = (COXGridListData*)GetOriginalItemData(nIndex);
			ASSERT(pGridListData != NULL);
			ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));
			if (pGridListData->m_dwUserData == (DWORD)plvfi->lParam)
				return nIndex;
			}
		// Specified LPARAM not found, returning -1
		return (LRESULT)-1;
		}
	}

afx_msg LRESULT COXGridList::OnGetItem(WPARAM wParam, LPARAM lParam)
	{
	// Pass the message directly to the windows
	HRESULT result = DefWindowProc(LVM_GETITEM, wParam, lParam);
	LV_ITEM* plvi = (LV_ITEM*)lParam;
	ASSERT(AfxIsValidAddress(plvi, sizeof(LV_ITEM)));

	// If it succeeded and the list control is sortable and
	// the LPARAM of and item was requested
	// then fill it out correctly (dereference the COXGridListData object)
	// ... Extended should be in use or not, not in an intermediate state
	ASSERT((GetExtendedDataState() == EDNo)  || (GetExtendedDataState() == EDYes));
	if (result && (GetExtendedDataState() == EDYes) && ((plvi->mask & LVIF_PARAM) == LVIF_PARAM))
		{
		COXGridListData* pGridListData;
		pGridListData = (COXGridListData*)plvi->lParam;
		ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));
		plvi->lParam = pGridListData->m_dwUserData;
		}
	return result;
	}

afx_msg LRESULT COXGridList::OnSetItem(WPARAM wParam, LPARAM lParam)
	{
	const LV_ITEM* plvi = (const LV_ITEM*)lParam;
	ASSERT(AfxIsValidAddress(plvi, sizeof(LV_ITEM)));

	// ... Extended should be in use or not, not in an intermediate state
	ASSERT((GetExtendedDataState() == EDNo)  || (GetExtendedDataState() == EDYes));
	if ((GetExtendedDataState() == EDYes) && ((plvi->mask & LVIF_PARAM) == LVIF_PARAM))
		{
		// The list is sortable and the LPARAM will be changed
		// First get the corresponding COXGridListData object,
		// change its user data value 
		COXGridListData* pGridListData = (COXGridListData*)GetOriginalItemData(plvi->iItem);
		ASSERT(pGridListData != NULL);
		ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));
		pGridListData->m_dwUserData = plvi->lParam;
		// Then let the default implementation handle the REST
		// So we remove the LVIF_PARAM request
		// We have to copy the plvi struct because it is const
		LV_ITEM lvi;
		memcpy(&lvi, plvi, sizeof(LV_ITEM));
		lvi.mask &= (~LVIF_PARAM);
		return DefWindowProc(LVM_SETITEM, wParam, (LPARAM)&lvi);
		}

	// Pass the message directly to the windows
	return DefWindowProc(LVM_SETITEM, wParam, lParam);
	}

void COXGridList::OnLButtonDblClk(UINT nFlags, CPoint point) 
	{
	int nCheckItem = HitTest(point);
	if (GetCheckable() && (0 <= nCheckItem))
		{
		OnCheck(nCheckItem);
		}
	else
		CListCtrl::OnLButtonDblClk(nFlags, point);
	}

afx_msg LRESULT COXGridList::OnEditLabel(WPARAM wParam, LPARAM lParam)
	{
	ASSERT(::IsWindow(m_hWnd));
	int nItem = (int)wParam;
	int nSubItem = (int)lParam;

	// Simulate that the user clicked in the specified subitem 
	CRect subItemRect = GetRectFromSubItem(nItem, nSubItem);
	if (!subItemRect.IsRectNull())
		m_lastClickPos = subItemRect.TopLeft();
	
	return (HRESULT) DefWindowProc(LVM_EDITLABEL, wParam, 0);
	}


BOOL COXGridList::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	BOOL bEaten = FALSE;
	
	// The user clicked on one of the column headings - sort by this column.
	if (GetSortable())
		{
		// ... Extended should be in use 
		ASSERT(GetExtendedDataState() == EDYes);
		CWaitCursor wc;
		SortColumn(pNMListView->iSubItem);
		// ... Handled this message, do not pass on to the parent
		bEaten = TRUE;
		}

	*pResult = 0;
	return bEaten;
	}



void COXGridList::PreSubclassWindow() 
	{
	// ... Attached window must be valid
	ASSERT(m_hWnd != NULL);
	ASSERT(::IsWindow(m_hWnd));
	// ... List control must be in report view and owner drawn
	ASSERT((GetStyle() & LVS_REPORT) == LVS_REPORT);
	ASSERT((GetStyle() & LVS_OWNERDRAWFIXED) == LVS_OWNERDRAWFIXED);
	
	// Initialize this control
	InitGrid();

	CListCtrl::PreSubclassWindow();
	}

BOOL COXGridList::OnListCtrlNotify(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	// Adjust the pNMListView and eat the message if necessary
	return AdjustNotification(pNMListView);
	}


void COXGridList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
	if (m_bAutoEdit)
		{
		// Start editing the item that has focus
		int nFocusItem = GetCurFocus();
		if (nFocusItem < 0)
			{
			// ... No item has focus
			return;
			}
		CEdit* pEdit;
		pEdit = EditLabel(nFocusItem, -1);
		// pEdit can be NULL if e.g. no column is editable
		if (pEdit != NULL)
			{
			CString sText((TCHAR)nChar);
			pEdit->SetWindowText(sText);
			pEdit->SetSel(1, 1);
			}
		else
			{
			// .. No editable subitem is found
			::MessageBeep(0xFFFFFFFF);
			return;
			}

		// We handled the message, do not call base class implementation
		return;
		}
	
	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
	}

BOOL COXGridList::OnSetFocus(NMHDR* /* pNMHDR */, LRESULT* pResult) 
	{
	// ... Never eat this message
	BOOL bEaten = FALSE;

	// Refresh the selected and focussed items
	RefreshFocusItem();
	if (!GetShowSelAlways())
		RefreshSelItems();
	
	*pResult = 0;
	return bEaten;
	}

BOOL COXGridList::OnKillFocus(NMHDR* /* pNMHDR */, LRESULT* pResult) 
	{
	// ... Never eat this message
	BOOL bEaten = FALSE;

	// Refresh the selected and focussed items
	RefreshFocusItem();
	if (!GetShowSelAlways())
		RefreshSelItems();
	
	*pResult = 0;
	return bEaten;
	}


COLORREF COXGridList::GetItemTextColor(int itemID)
{
	DWORD colVal = GetItemColorVal(itemID);
	colVal &= 0x0000FFFF;

  return GetRGBColor((Color) colVal, true);
}

 
COLORREF COXGridList::GetRGBColor(COXGridList::Color colVal, bool bTextColor)
{
  switch(colVal)
	{
		case COXGridList::red:
			return(RGB(255, 0, 0));
 
		case COXGridList::green:
			return(RGB( 0, 255, 0));
 
		case COXGridList::blue:
			return(RGB( 0, 0, 255));
 
		case COXGridList::cyan:
			return(RGB( 0, 255, 255));
 
		case COXGridList::magenta:
			return(RGB( 255, 0, 255));
 
		case COXGridList::yellow:
			return(RGB( 255, 255, 0));
 
		case COXGridList::white:
			return(RGB( 255, 255, 255));
 
		case COXGridList::black:
			return(RGB( 0, 0, 0));

		default:
      if (bTextColor)
			  return(GetTextColor());
      else
  			return(GetTextBkColor());
	}
}

void COXGridList::SetItemTextColor(int itemID, COXGridList::Color color)
{
	DWORD val = GetItemData(itemID);
	val &= 0x11110000;
	val += (int) color;
	SetItemColorVal(itemID, val);
}

COLORREF COXGridList::GetItemTextBkColor(int itemID)
{
	DWORD colVal = GetItemColorVal(itemID);
	colVal &= 0xFFFF0000;
	colVal = colVal >> 16;

  return GetRGBColor((Color) colVal, false);
}

void COXGridList::SetItemTextBkColor(int itemID, COXGridList::Color color)
{
	DWORD val = GetItemColorVal(itemID);
	val &= 0x00001111;
	val += ((int) color << 16);
	SetItemColorVal(itemID, val);
}

void COXGridList::SetItemColorVal(int nIndex, DWORD dwColorVal)
{
	COXGridListData *pGridListData = (COXGridListData*)GetOriginalItemData(nIndex);
	ASSERT(pGridListData != NULL);
	ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));

	pGridListData->m_dwColors = dwColorVal;
}

DWORD COXGridList::GetItemColorVal(int nIndex)
{
	COXGridListData *pGridListData = (COXGridListData*)GetOriginalItemData(nIndex);
	ASSERT(pGridListData != NULL);
	ASSERT(AfxIsValidAddress(pGridListData, sizeof(COXGridListData)));

	return pGridListData->m_dwColors;
}

void COXGridList::UseItemColors(bool bUse)
{
	m_bUseItemColors = bUse;

	if (bUse)
		AddExtendedData();
	else if (!m_bSortable)
		RemoveExtendedData();
}

void COXGridList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	CListCtrl::OnLButtonUp(nFlags, point);
}

UINT COXGridList::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return DLGC_WANTALLKEYS  | CListCtrl::OnGetDlgCode();
}

void COXGridList::OnPaint()
{
  CListCtrl::OnPaint();
}

void COXGridList::OnTrackHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (GetParent())
  {
    pNMHDR->hwndFrom = GetSafeHwnd();
    pNMHDR->idFrom   = GetDlgCtrlID();
    *pResult = GetParent()->SendMessage(WM_NOTIFY, (WPARAM) pNMHDR->idFrom, (LPARAM) pNMHDR);
  }
}

void COXGridList::OnContextMenu(CWnd* pWnd, CPoint point)
{
  //Let the parent handle this one
  if (GetParent())
    GetParent()->SendMessage(WM_CONTEXTMENU, (LPARAM) pWnd->GetSafeHwnd(), (WPARAM) MAKELPARAM(point.x, point.y));
}
