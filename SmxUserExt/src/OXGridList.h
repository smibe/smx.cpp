/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: OXGridList.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/OXGridList.h,v 1.2 2002/11/22 17:15:16 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 					Class Specification : COXGridList
// ==========================================================================

// Header file : gridlist.h

// Source : Periphere NV (F.Melendez - R.Mortelmans)
// Creation Date : 	   29th November 1995
// Last Modification : 20th August 1996
                          
//270598:GeH: Colloring of items allowed through Item value

// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CListCtrl

//	YES	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	YES	Has a message map
//	YES	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class can be used to simulate a simple grid with a ListView
//	common control (Win95 and NT 3.51).  It lets the user select an entire
//	row in one time and draws vertical and horizontal gridlines, default header
//	sorting, font changing, text color changing, ...

// Remark:
//	The sorting algorithm uses the LPARAM associated with each item.
//	It points to a COXGridListData object which contains the original user data
//	Every item has its own COXGridListData object, so this pointer uniquely
//	 identifies the item
//	The original user data can still be used for other purposes and will still
//   be fully transparantly accessible

//	This file uses resources. (E.g. Bitmap IDB_STATEIMAGELIST)
//	The reserved ID ranges are : 23280 -> 23299 and 53280 -> 53299

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////

#ifndef __GRIDLIST_H__
#define __GRIDLIST_H__

#define STATEIMAGEMASKTOINDEX(i) (((i) >> 12) & 0xF)
#define ALLSTATEIMAGEMASKS INDEXTOSTATEIMAGEMASK(0xF)

#include "OXGridEdit.h"
class COXEditList;
class CSmxColSizer;
class COXGridList;


enum EFindResult
{
  FIND_Found     = 1,
  FIND_NotFound  = 2,
  FIND_Cleared   = 3,
  FIND_EndOfList = 4,
};

struct COXGridListSortInfo
{
public:
	COXGridList*  	m_pThis;
	int   			m_nSubIndex;
  bool        m_bAscending;

	COXGridListSortInfo(COXGridList* pThis = NULL, int nSubIndex = 0)
		:
		m_pThis(pThis),
		m_nSubIndex(nSubIndex)
			{
        m_bAscending = true;
			};
};


class COXGridList : public CListCtrl
{
friend class COXEditList;
// Data members -------------------------------------------------------------
public:
	BOOL	m_bModified;    

protected:
	BOOL 	m_bLastRowWasVisible;
	BOOL 	m_bInitialized;
	BOOL 	m_bSortable;
	BOOL 	m_bCheckable;
	UINT	m_nCheckStyle;
	BOOL	m_bAutoEdit;
	BOOL	m_bEdit;    
    bool    m_bUseItemColors;

  int   m_nFoundItem;
  int   m_nFoundSubItem;

	enum EUseExtendedData
		{
		EDNo = 1,
		EDAdding = 2,
		EDYes = 3,
		EDRemoving = 4,
		};
	EUseExtendedData m_eUseExtendedData;
	static EUseExtendedData m_eFirstED;
	static EUseExtendedData m_eLastED;

	int   	m_nNumOfCols;
	int		m_nImageColumn;

	CPen  	m_GridPen;
	BOOL	m_bGridLines;
	BOOL	m_bHorizontalGridLines;
	BOOL	m_bVerticalGridLines;

	CRect	m_SelectedRect;
	CFont   m_TextFont;
	CImageList m_stateImages;

	COXGridEdit m_gridEdit;
	CPoint m_lastClickPos;
	int m_nEditSubItem;

	CWordArray m_rgbEditable;
	CWordArray m_colType;
  CPtrArray  m_aCompareProc;

  int  m_nSortColumn;
  bool m_bSortAscending;

  static int m_nListEditXOffset;
	static int m_nListEditYOffset;
	static int m_nListEditCYOffset;

	static int m_nListEditCXOffset;

	struct COXGridListData
		{
		public:
			DWORD m_dwUserData;  //To be used as user data
			DWORD m_dwColors;    //Is used to store item colors.
			
			COXGridListData(DWORD dwUserData = 0)
				:
				m_dwUserData(dwUserData)
				{
          m_dwColors = 0;
				};
		};


private:
	
// Member functions ---------------------------------------------------------
public:
	COXGridList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	virtual void InitGrid();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Sets text color, text background color and the text font

	BOOL SetSortable(BOOL bSortable = TRUE);
	// --- In  : bSortable : automatic sort or not
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Sets the Grid in an automatic sort mode.  When clicking a header
	//				the strings in that column will be sorted

  void SetCompareProc(int nColumn,  PFNLVCOMPARE pfnCompare);

	BOOL GetSortable() const;
	// --- In  :
	// --- Out : 
	// --- Returns : whether automatic sorting is supported or not
	// --- Effect : 

	virtual BOOL SortColumn(int nColumn = 0);
	// --- In  : nColumn : Zero-based index of the column used to sort upon
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sorts the contents of the control by sorting a specific column

	virtual int GetNumCols() const;
	// --- In  :
	// --- Out : 
	// --- Returns : Number of columns inserted in the listctrl
	// --- Effect : 

	virtual BOOL SetEqualWidth();
	// --- In  :
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : Rearranges the inserted columns to have an equal width
	//				that is equidistant divided within the width of the listctrl
		 			
	virtual BOOL SetGridLines(BOOL bGridLines = TRUE,
		COLORREF LineColor = RGB(0,0,0), BOOL bUpdate = TRUE);
	// --- In  : bGridLines : True if gridlines are wanted
	//			 LineColor : the color you want your gridlines to have
	//			 bUpdate : Redraw Control now?
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : The listctrl can show gridlines with a special color

	virtual BOOL GetGridLines(BOOL& bGridLines, COLORREF& LineColor) const;
	// --- In  : 
	// --- Out : bGridLines : whether the control shows gridlines
	// 			 LineColor : which color they have 
	// --- Returns : succeeded or not
	// --- Effect : 

	virtual BOOL SetGridLineOrientation(BOOL bHorizontal = TRUE,
		BOOL bVertical = TRUE);
	// --- In  : bHorizontal : Whether horizontal gridlines should be shown
	//			 bVertical : Whether vertical gridlines should be shown
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : By default both horizontal and vertical gridlines will be shown
	//				Make sure the general setting to show gridlines (SetGridLines())
	//				is set to true

	BOOL GetGridLineOrientation(BOOL& bHorizontal, BOOL& bVertical) const;
	// --- In  : 
	// --- Out : bHorizontal : Whether horizontal gridlines should be shown
	//			 bVertical : Whether vertical gridlines should be shown
	// --- Returns : succeeded or not
	// --- Effect : 

	int	GetCurSel() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The current selected row
	//				 When no row is selected -1 is returned
	//				 When more than one row is selected the first is returned
	// --- Effect : 

	void ClearSelection();
	BOOL SetCurSel(int nSelectionItem, BOOL bSelect = TRUE);
	// --- In  : nSelectionItem : The item that should change selection state (-1 = all items)
	//           bSelect : Select (TRUE) or unselect (FALSE)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Selects or unselects an item(s)

	int	GetCurFocus() const;
	// --- In  :
	// --- Out : 
	// --- Returns : The current focussed row 
	//				 When no row has focus -1 is returned
	// --- Effect : 

	BOOL SetCurFocus(int nFocusItem, BOOL bFocus = TRUE);
	// --- In  : nFocusItem : The item that should change focused (-1 = all items)
	//           bFocus : Get (TRUE) or lose focuis (FALSE)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets or lose the focus of the specified item
	//				Note that at most one item can have focus

	virtual BOOL SetTextFont(CFont* pFont = NULL, BOOL bUpdate = TRUE);
	// --- In  : pFont : new font, if null then the standard list ctrl font	is used
	//			 bUpdate : whether to redraw the ctrl or not
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets a new font to draw the row text with

	virtual BOOL SetShowSelAlways(BOOL bShowSelAlways = TRUE);
	// --- In  : bShowSelAlways : Whether to show the selection always
	//							  (even when the control does not have focus)
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets selection display mode.  

	BOOL GetShowSelAlways() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : The control selection display mode
	//				 (whether a selection should always be shown)
	// --- Effect : 

	virtual void SetMultipleSelection(BOOL bMultiple = TRUE);
	// --- In  : bMultiple : Allow multiple selection or not
	// --- Out : 
	// --- Returns : 
	// --- Effect : Whether to allow multiple selection or not 
	//				By default it is not allowed

	BOOL GetMultipleSelection() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether multiple selection is allowed or not 
	// --- Effect : 

	virtual void SetEditable(BOOL bEdit = TRUE, int nColumn = -1);
	// --- In  : bEdit : Allow edit or not
	//			 nColumn : The column on which to allow editing (-1 = all columns)
	// --- Out : 
	// --- Returns : 
	// --- Effect : Whether to allow a subitem of a specified column to be edited or not
	//				By default editing is not allowed

	BOOL GetEditable(int nColumn) const;
	// --- In  : nColumn : The column of which the editing state is requested
	// --- Out : 
	// --- Returns : Whether a subitem of a specified column is allowed to be edited or not
	// --- Effect : 

	virtual BOOL SetCheckable(BOOL bCheckable = TRUE);
	// --- In  : bCheckable : Whether this control uses checkboxes for all items
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets this control in checkable mode.  

	virtual BOOL GetCheckable() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether this control in in checkable mode.  
	// --- Effect : 

	void SetCheckStyle(UINT nStyle = BS_AUTOCHECKBOX);
	// --- In  : nStyle : BS_CHECKBOX, BS_3STATE, BS_AUTOCHECKBOX or BS_AUTO3STATE
	// --- Out : 
	// --- Returns : Sets the checkstyle (see the documentation of Win32 for information
	//				 about the different types)
	// --- Effect : 

	UINT GetCheckStyle() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Get the present check style
	// --- Effect : 

	void SetCheck(int nIndex, int nCheck);
	// --- In  : nIndex : The index os the item to change (-1 = all items)
	//			 nCheck : The new check state ( 0 for clear, 1 for checked, and 2 for indeterminate)
	// --- Out : 
	// --- Returns : Whether this control in in checkable mode.  
	// --- Effect : 

	int GetCheck(int nIndex) const;
	// --- In  : nIndex : The index of the item to investigate
	// --- Out : 
	// --- Returns : The present check state of the specified item.  
	// --- Effect : 

	virtual BOOL OnCheck(int nCheckItem);
	// --- In  : nCheckItem : The index of item which checkbox has been clicked
	// --- Out : 
	// --- Returns : Whether to proceed (TRUE) or abort the check (FALSE);
	// --- Effect : The user clicked the specified item, the check state will change

	virtual BOOL OnCheckChange(int nCheckItem, int nCheck);
	// --- In  : nCheckItem : The index of item which check state is about to change
	//			 nCheck : The new check state
	// --- Out : 
	// --- Returns : Whether to proceed (TRUE) or abort the check (FALSE);
	// --- Effect : The user clicked an item and because this item was selected
	//				its check state will change as well

	virtual BOOL SetAutoEdit(BOOL bAutoEdit = TRUE);
	// --- In  : bAutoEdit : Whether the item will be edited 
	//                       when a valid character is typed
	// --- Out : 
	// --- Returns : Whether it succeeded or not
	// --- Effect : Sets this control in auto edit mode.  
	//				Normally valid characters (e.g. A, B, 1, etc) are used to
	//				jump to the item that starts with that character.
	//				This setting can change that behaviouir into editing the item

	BOOL GetAutoEdit() const;
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the control is in auto edit mode
	//               (whether focussed item will be edited when a valid character is typed)
	// --- Effect : 

	CEdit* EditLabel(int nItem, int nSubItem);
	// --- In  : nItem : The Item to edit
	//			 nSubItem : The subitem to edit
	// --- Out : 
	// --- Returns : If successful, a pointer to the CEdit object that is used to edit the item text; 
	//               otherwise NULL.
	// --- Effect : Begins in-place editing of the specified list view subitem
	//				To edit the first editable subitem of a specified item use
	//				nSubItem = -1
  
	BOOL SetImageColumn(int nColumnIndex, BOOL bUpdate = TRUE);
	// --- In  : nColumnIndex : the index of the column where the small icons must appear
	//			 bUpdate : Redraw Control now?
	// --- Out : 
	// --- Returns : succeeded or not
	// --- Effect : assigns the column to show the images attached to an item

	int GetImageColumn() const;
	// --- In  :
	// --- Out : 
	// --- Returns : the index of the column where the small icons currently appear
	// --- Effect : 

	DWORD GetDlgBaseUnits(CDC* pDC);
	// --- In  : pDC : the DC whose DlgBaseUnits will be calculated
	// --- Out : 
	// --- Returns : Dialog base units based on the font selected into the DC
	// --- Effect : If no special font selected, calls ::GetDialogBaseUnits()

	virtual BOOL IsLastRowVisible() const;
	// --- In  :
	// --- Out : 
	// --- Returns : Whether the last row is completely visible (TRUE)
	//				 FALSE is returned when the last row is not visible 
	//				 or only partially visible
	// --- Effect : 

	virtual void OnLastRowAppear();
	// --- In  :
	// --- Out : 
	// --- Returns : 
	// --- Effect : This overridable function will be called when the last
	//				row of the window was not completely visible before
	//				and becomes completely visible now.

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	// --- In  : lpDIS : A long pointer to a DRAWITEMSTRUCT structure that contains
	// 				information about the type of drawing required.
	// --- Out : 
	// --- Returns :
	// --- Effect : Called by CDialog::OnDrawItem for an ownerdrawn listview, for every
	//				item that needs redrawing.

	BOOL EnsureVisible(int nItem, int nSubItem, BOOL bPartialOK);
	// --- In  : nItem : Index of the item to make visible
	//			 nSubItem : Index of the subitem to make visible
	//			 bPartialOK : Specifies whether partial visibility is acceptable.
	// --- Out : 
	// --- Returns : 
	// --- Effect : Ensures that the specified subitem is visible
  
	BOOL GetSubItemFromPoint(CPoint pos, int& nItem, int& nSubItem, CRect& rect) const;
	// --- In  : pos : Position expressed in client coordinates of the list control
	// --- Out : nItem : The index of the item in which the point is located
	//           nSubItem : The sub item in which the point is located (0 = label)
	//			 rect : The rectangle of that subitem
	// --- Returns : Whether a valid subItem was clicked (not outside items)
	// --- Effect : Converts coordinates to index of item and subitem

	CRect GetRectFromSubItem(int nItem, int nSubItem, BOOL bIncludeImages = FALSE) const;
	// --- In  : nItem : The index of the item in which the point is located
	//           nSubItem : The sub item in which the point is located (0 = label)
	//			 bIncludeImages : Whether to take the size of possible images into account or not
	// --- Out : 
	// --- Returns : The rect of this subitem
	// --- Effect : Converts index of item and subitem to a rect

	CHeaderCtrl* GetHeaderCtrl();
	// --- In  : 
	// --- Out : 
	// --- Returns : The header control of this grid list control
	//				 This pointer may be temporary and should not be stored for later use
	//               NULL is returned when no no header control exists
	// --- Effect : 

	HWND GetHeaderCtrlHandle();
	// --- In  : 
	// --- Out : 
	// --- Returns : The handle of the header control of this grid list control
	//               or NULL when no header control exists
	// --- Effect : 

//270598:GeH --->
	enum Color 
  {
		defcol  = 0,
    red		  = 1,
		green	  = 2,
		blue	  = 3,
		cyan	  = 4,
		magenta	= 5,
		yellow	= 6,
		white	  = 7,
		black	  = 8,
    gray    = 9,
	};
	
	enum ColType 
  { 
    t_number    = 1, 
    t_string    = 2, 
    t_date      = 3, 
    t_date_time = 4, 
    t_double    = 5, 
  };

	EFindResult  Find(const CString& str);
	void UseItemColors(bool bUse);
	COLORREF GetItemTextColor(int itemID);
	void     SetItemTextColor(int itemID, COXGridList::Color color);
	COLORREF GetItemTextBkColor(int itemID);
	void     SetItemTextBkColor(int itemID, COXGridList::Color color);
	void SetColType(ColType cType, int nColumn /* = -1 */);
	WORD GetColType(int col);
  COLORREF GetRGBColor(COXGridList::Color colVal, bool bTextColor);
//270598:GeH <---

	int FindOriginalItemData(DWORD dwItemData);
  virtual BOOL SortItems( PFNLVCOMPARE pfnCompare, DWORD dwData );
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXGridList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object
													 
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(COXGridList)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	void Empty();

	static int CALLBACK GridCompareProc(LPARAM, LPARAM, LPARAM);

	virtual BOOL SetCheckStateImageList();
	virtual BOOL DrawGridLines(CDC* pDC, LPDRAWITEMSTRUCT lpDIS);
	virtual void SetRowText(LPDRAWITEMSTRUCT lpDIS, BOOL bShowItemSel);
	virtual void SetColItemText(CDC* pDC, CString& sCellText, CRect& rectText, UINT nJustify);

	virtual EUseExtendedData GetExtendedDataState();
	virtual void SetExtendedDataState(EUseExtendedData eUseExtendedData);
	BOOL AddExtendedData();
	BOOL RemoveExtendedData();
	BOOL AdjustNotification(NM_LISTVIEW* pNMListView);

	void RefreshFocusItem();
	void RefreshSelItems();

	BOOL DrawImage(CDC* pDC,  CRect& rectText, int iItemID, BOOL bSelected);
	BOOL ChangeItemText(int nItem, int nSubItem, LPCTSTR pszText);
	DWORD GetOriginalItemData(int nItem);
	BOOL SetOriginalItemData(int nItem, DWORD dwData);
	int GetCheckItemFromPoint(const CPoint& point) const;
	BOOL PostEditLabel(int nItem, int nSubItem);
	BOOL SearchNextEditItem(int nItemOffset, int nSubItemOffset, int& nItem, int& nSubItem);
	DWORD GetItemColorVal(int nIndex);
	void  SetItemColorVal(int nIndex, DWORD dwColorVal);



// Generated message map functions
   //{{AFX_MSG(COXGridList)
	afx_msg void OnPaint();
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnListCtrlNotify(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnKillFocus(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
   
  afx_msg void OnTrackHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnInsertItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertColumn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteColumn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteAllItems(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFindItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditLabel(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////
#endif      // __GRIDLIST_H__
