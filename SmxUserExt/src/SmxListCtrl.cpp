
#include "stdafx.h"
#include "SMXListCtrl.h"
#include "CJFlatHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMXListCtrl

CSMXListCtrl::CSMXListCtrl()
{
	m_nMinSize		= 0;
	m_nMinColWidth	= 50;
	m_nMaxColWidth	= 500;
	m_nSortedCol	= -1;
	m_bAscending	= true;
	m_bAutoSave		= false;	
	m_bNoColSizing	= false;
	m_pHeaderCtrl	= NULL;
	m_clrText		= ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk 	= ::GetSysColor(COLOR_WINDOW);
}

CSMXListCtrl::~CSMXListCtrl()
{
	 delete m_pHeaderCtrl;
}

IMPLEMENT_DYNAMIC(CSMXListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CSMXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSMXListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMXListCtrl message handlers

// Taken from: Copying/Moving Rows in CListCtrl by James Spibey [spib@bigfoot.com]. 
// http://www.codeguru.com/listview/moving_rows.html
bool CSMXListCtrl::MoveRow(int nFrom, int nTo)
{
	//Can't move to the same place, or from or to a negative index
	if(nFrom == nTo || nFrom < 0 || nTo < 0)
		return false;
	
	//First Copy the row to the new location
	if(CopyRow(nFrom, nTo))
	{
		//If we have just inserted a row before
		//this one in the list, we need to increment
		//our index.
		if(nFrom > nTo)
			DeleteItem(nFrom + 1);
		else
			DeleteItem(nFrom);
		
		return true;
	}
	else
		return false;
}

// Taken from: Copying/Moving Rows in CListCtrl by James Spibey [spib@bigfoot.com]. 
// http://www.codeguru.com/listview/moving_rows.html
bool CSMXListCtrl::CopyRow(int nFrom, int nTo)
{
	//Can't move to the same place, or from or to a negative index
	if(nFrom == nTo || nFrom < 0 || nTo < 0)
		return false;

	//Copy the row to the new index
	InsertItem(nTo, GetItemText(nFrom, 0));

	//If row has been inserted before original
	//increment the original
	if(nFrom > nTo)
		nFrom++;

	//Loop through subitems
	for(int i = 1; i < GetColumnCount(); i++)
	{
		SetItemText(nTo, i, GetItemText(nFrom, i));
	}

	return true;
}

// Taken from: Getting the number of columns in report view by Zafir Anjum.
// http://www.codeguru.com/listview/num_cols.shtml
int CSMXListCtrl::GetColumnCount() const
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	return pHeader->GetItemCount();
}

// Taken from: Simpler way to do this (using new version of Comctl32.dll) by Vidas [vibal@lrtc.lt].
// http://www.codeguru.com/mfc/comments/6432.shtml
void CSMXListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	
    switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
		
    case CDDS_ITEMPREPAINT: // Requested notification
		if(lplvcd->nmcd.dwItemSpec % 2)
		{
			lplvcd->clrText   = m_clrText;
			lplvcd->clrTextBk = m_clrTextBk;
		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

BOOL CSMXListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY *pHDNotify = (HD_NOTIFY*)lParam;

	switch( pHDNotify->hdr.code )
	{
	// Sorting the list when user clicks on column header by Zafir Anjum.
	// http://www.codeguru.com/listview/sort_on_col_click.shtml
	case HDN_ITEMCLICKA:
	case HDN_ITEMCLICKW:
		{
			if( pHDNotify->iButton == 0 )
			{
				// set the sort order.
				if( pHDNotify->iItem == m_nSortedCol )
					m_bAscending = !m_bAscending;
				else
					m_bAscending = true;

				// save the column index.
				m_nSortedCol = pHDNotify->iItem;

				// virtual call to sort list.
				SortList( m_nSortedCol, m_bAscending );

				// set the sort image in the header control.
				if( m_pHeaderCtrl && m_pHeaderCtrl->GetSafeHwnd( ))
					m_pHeaderCtrl->SetSortImage( m_nSortedCol, m_bAscending );
			}
		}
		break;

	// How to force a minimum column width Cees Mechielsen.
	// http://www.codeguru.com/mfc/comments/866.shtml
	case HDN_ITEMCHANGINGA:
	case HDN_ITEMCHANGINGW:
	case HDN_ENDTRACK:
		{
			if( pHDNotify->pitem->mask & HDI_WIDTH &&
				pHDNotify->pitem->cxy < m_nMinSize &&
				pHDNotify->pitem->cxy >= 0 )
			{
				pHDNotify->pitem->cxy = m_nMinSize;	// Set the column width
			}
		}
		break;

	// Prevent CListCtrl column resizing by Zafir Anjum.
	// http://www.codeguru.com/listview/no_col_resize.shtml
	case HDN_DIVIDERDBLCLICKA:
	case HDN_DIVIDERDBLCLICKW:
	case HDN_BEGINTRACKW:
	case HDN_BEGINTRACKA:
		{
			if(m_bNoColSizing)
			{
				*pResult = TRUE;                // disable tracking
				return TRUE;                    // Processed message
			}
			break;
		}
	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

bool CSMXListCtrl::SortList(int nCol, bool bAscending)
{
#ifdef _DEBUG
	TRACE2("Column sort on column index %d, ascending=%d", nCol, bAscending);
#else
	UNUSED_ALWAYS( nCol );
	UNUSED_ALWAYS( bAscending );
#endif

	return true;
}

// Taken from: Adding a column by Zafir Anjum.
// http://www.codeguru.com/listview/add_column.shtml

// AddColumn      - adds column after the right most column
// Returns        - the index of the new column if successful or -1 otherwise.
// lpszColHeading - the column's heading
// nWidth         - Width of the column, in pixels. If this parameter is -1, 
//                  the column width is the same as previous column
// nFormat        - the alignment of the column. Can be LVCFMT_LEFT,
//                  LVCFMT_RIGHT, or LVCFMT_CENTER.
int CSMXListCtrl::AddColumn(LPCTSTR lpszColHeading, int nWidth/*=-1*/, int nFormat/*=LVCFMT_LEFT*/)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	
	if( nWidth == -1 )
	{
		// The line below return zero for ICON views
		//nWidth = GetColumnWidth( nColumnCount - 1 );
		
		// Get the column width of the previous column from header control
		HD_ITEM hd_item;
		hd_item.mask = HDI_WIDTH;               //indicate that we want the width
		pHeader->GetItem( nColumnCount - 1, &hd_item );
		nWidth = hd_item.cxy;
	}
	return InsertColumn( nColumnCount, lpszColHeading, nFormat, nWidth, nColumnCount );
}

bool CSMXListCtrl::BuildColumns(int nCols, int* nWidth, int* nColString)
{
	for( int i = 0; i < nCols; ++i ) {
		CString	strTemp; strTemp.LoadString( nColString[i] );
		if(AddColumn(strTemp, nWidth[i])==-1)
			return false;
	}

	return true;
}

bool CSMXListCtrl::BuildColumns(int nCols, int* nWidth, CString* strColString)
{
	for( int i = 0; i < nCols; ++i ) {
		if(AddColumn(strColString[i], nWidth[i])==-1)
			return false;
	}

	return true;
}

// Taken from: Detecting column index of the item clicked by Zafir Anjum.
// http://www.codeguru.com/listview/col_index.shtml

// HitTestEx	- Determine the row index and column index for a point
// Returns		- the row index or -1 if point is not over a row
// point		- point to be tested.
// nCol			- to hold the column index
int CSMXListCtrl::HitTestEx(CPoint& point, int* nCol) const
{
	int colnum = 0;
	int row = HitTest( point, NULL );
	
	if( nCol ) *nCol = 0;

	// Make sure that the ListView is in LVS_REPORT
	if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		return row;

	// Get the top and bottom row visible
	row = GetTopIndex();
	int bottom = row + GetCountPerPage();
	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for( ;row <= bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect( row, &rect, LVIR_BOUNDS );
		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				int colwidth = GetColumnWidth(colnum);
				if( point.x >= rect.left 
					&& point.x <= (rect.left + colwidth ) )
				{
					if( nCol ) *nCol = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

bool CSMXListCtrl::SubclassHeader(bool bBoldFont/*=false*/)
{
	// allocate memory for the flat header.
	m_pHeaderCtrl = new CCJFlatHeaderCtrl;
	ASSERT( m_pHeaderCtrl );

	// get the window handle to the existing header
	// control.
	HWND hWnd = GetDlgItem(0)->GetSafeHwnd();
	ASSERT( hWnd );

	// subclass the flat header control.
	if( !m_pHeaderCtrl->SubclassWindow( hWnd ))
		return false;

	// finish header initialization.
	m_pHeaderCtrl->InitializeHeader( bBoldFont );
	
	return true;
}

void CSMXListCtrl::AutoSaveColumns(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	m_bAutoSave = true;

	// initialize registry strings.
	if( lpszSection == NULL ) m_strSection = _T("Settings");
	else m_strSection = lpszSection;

	if( lpszEntry == NULL ) m_strEntry = _T("Column Info");
	else m_strEntry = lpszEntry;

	if( lpszDefault == NULL ) m_strDefault = _T("");
	else m_strDefault = lpszDefault;

	LoadColumnWidths();
}

void CSMXListCtrl::SaveColumnWidths()
{
	// get a pointer to the header control.
	CHeaderCtrl* pHeader = DYNAMIC_DOWNCAST(CHeaderCtrl, GetDlgItem(0));
	ASSERT_KINDOF( CHeaderCtrl, pHeader );

	CString strValue;
	for( int i = 0; i < pHeader->GetItemCount(); ++i )
	{
		CString strTemp;
		strTemp.Format(_T("%d,"), GetColumnWidth( i ));
		strValue += strTemp;
	}

	AfxGetApp()->WriteProfileString( m_strSection, m_strEntry, strValue );
}

void CSMXListCtrl::LoadColumnWidths()
{
	// get a pointer to the header control.
	CHeaderCtrl* pHeader = DYNAMIC_DOWNCAST(CHeaderCtrl, GetDlgItem(0));
	ASSERT_KINDOF( CHeaderCtrl, pHeader );

	for( int i = 0; i < pHeader->GetItemCount(); ++i )
		SetColumnWidth(i);
}

void CSMXListCtrl::SetColumnWidth(int nCol)
{
	int nWidth = GetStoredWidth( nCol );
	if( nWidth > m_nMinColWidth )
		CListCtrl::SetColumnWidth( nCol, nWidth );
	else
		AutoSizeColumn( nCol );
}

int CSMXListCtrl::GetStoredWidth(int nCol)
{
	// get the value from the registry.
	CString strValue = AfxGetApp()->GetProfileString(
		m_strSection, m_strEntry, m_strDefault );

	// extract the sub string to get the column width.
	CString strSubString;
	AfxExtractSubString( strSubString, strValue, nCol, _T(','));

	// return the width from the registry.
	return _ttoi( strSubString );
}

void CSMXListCtrl::OnDestroy() 
{
	if( m_bAutoSave ) SaveColumnWidths();
	CListCtrl::OnDestroy();
}

// Taken from: Autosize a column to fit its content by Roger Onslow.
// http://www.codeguru.com/listview/autosize_col.shtml
// If you don't supply a column number, it will resize all columns.
void CSMXListCtrl::AutoSizeColumn(int nCol/*=-1*/)
{
	// Call this after your list control is filled
	SetRedraw( false );
	
	int nMinCol = nCol < 0 ? 0 : nCol;
	int nMaxCol = nCol < 0 ? GetColumnCount()-1 : nCol;
	
	for (nCol = nMinCol; nCol <= nMaxCol; nCol++)
	{
		CListCtrl::SetColumnWidth( nCol, LVSCW_AUTOSIZE );
		int wc1 = GetColumnWidth( nCol );
		
		CListCtrl::SetColumnWidth( nCol, LVSCW_AUTOSIZE_USEHEADER );
		int wc2 = GetColumnWidth( nCol );
		int wc = max( m_nMinColWidth, max( wc1, wc2 ));
	
		if( wc > m_nMaxColWidth )
			wc = m_nMaxColWidth;
		
		// set the column width.
		CListCtrl::SetColumnWidth( nCol,wc );
	}
	
	SetRedraw();
}

BOOL CSMXListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CSMXListCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// KStowell - Get the client rect.
	CRect rcClient, rcClip;
	dc.GetClipBox( &rcClip );
	GetClientRect( &rcClient );

	// Exclude the header control from being painted over.
	CWnd* pWnd = GetDlgItem(0);
	if(pWnd && pWnd->IsWindowVisible()) {
		CRect rcItem;
		pWnd->GetClientRect(&rcItem);
		dc.ExcludeClipRect(&rcItem);
	}

	// KStowell - Create a memory device-context. This is done to help reduce
	// screen flicker, since we will paint the entire control to the
	// off screen device context first.
	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

	// KStowell - Repaint the background.
	memDC.FillSolidRect( rcClient, ::GetSysColor(COLOR_WINDOW) );

	// let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// KStowell - Copy the memory device context back into the original DC via BitBlt().
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );

	// KStowell - Cleanup resources.
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}
