/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelListBox.cpp,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelListBox.cpp,v 1.1 2002/09/20 09:37:07 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Thierry Maurel
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    tmaurel@caramail.com   (or tmaurel@hol.fr)
//
////////////////////////////////////////////////////////////////////////////////
// File    : AccelListBox.cpp
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AccelListBox.h"

#include "CmdAccelOb.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAccelListBox
/////////////////////////////////////////////////////////////////////////////
//
//
CAccelListBox::CAccelListBox()
{
	m_rgbSelectBack = GetSysColor(COLOR_HIGHLIGHT);
	m_rgbSelectText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_rgbLockText = GetSysColor(COLOR_GRAYTEXT);
	m_rgbLockBack = GetSysColor(COLOR_WINDOW);

}


/////////////////////////////////////////////////////////////////////////////
//
//
CAccelListBox::~CAccelListBox()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//
BEGIN_MESSAGE_MAP(CAccelListBox, CListBox)
	//{{AFX_MSG_MAP(CAccelListBox)
	ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAccelListBox message handlers
/////////////////////////////////////////////////////////////////////////////
// Draws the item, from the information stocked in the itemData field. This is
// a pointer to the corresponding CAccelOb.
//
void CAccelListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (lpDrawItemStruct->itemID == -1 || lpDrawItemStruct->itemID == LB_ERR)
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	if (GetCount() == 0) {
		// No datas, we draw an empty rectangle if the listbox is focused
		pDC->FillSolidRect(&(lpDrawItemStruct->rcItem), RGB(255,255,255));
		if (lpDrawItemStruct->itemState & ODS_SELECTED && 
			lpDrawItemStruct->itemAction & ODA_FOCUS)
			pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));
		return;

	} else {
		// OK, get the datas and draw them...
		CString szText;
		GetText(lpDrawItemStruct->itemID, szText);

		COLORREF oldTxtCol, oldBkCol;
		CAccelsOb* pAccel = (CAccelsOb*)(lpDrawItemStruct->itemData);
		if (pAccel->m_bLocked == TRUE) {
			pDC->FillSolidRect(&(lpDrawItemStruct->rcItem), m_rgbLockBack);
			oldTxtCol = pDC->SetTextColor(m_rgbLockText);
		} else {
			if (lpDrawItemStruct->itemState & ODS_SELECTED) {
				pDC->FillSolidRect(&(lpDrawItemStruct->rcItem), m_rgbSelectBack);
				oldTxtCol = pDC->SetTextColor(m_rgbSelectText);
				oldBkCol = pDC->SetBkColor(m_rgbSelectBack);
			} else
				pDC->FillSolidRect(&(lpDrawItemStruct->rcItem), m_rgbLockBack);
		}

		pDC->DrawText(szText, &(lpDrawItemStruct->rcItem), DT_LEFT);

		if (lpDrawItemStruct->itemState & ODS_SELECTED && 
			lpDrawItemStruct->itemAction & ODA_FOCUS)
			pDC->DrawFocusRect(&(lpDrawItemStruct->rcItem));

		if (pAccel->m_bLocked)
			pDC->SetTextColor(oldTxtCol);
		else {
			if (lpDrawItemStruct->itemState & ODS_SELECTED) {
				pDC->SetTextColor(oldTxtCol);
				pDC->SetBkColor(oldBkCol);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelListBox::RepaintSelectedItems()
{
	// invalidate focused item so it can repaint properly
	int nIndex = GetCurSel();
	if (nIndex != LB_ERR) {
		CRect rcItem;
		GetItemRect(nIndex, rcItem);
		InvalidateRect(rcItem,FALSE);
		// update changes 
		UpdateWindow();
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelListBox::OnKillfocus() 
{
	RepaintSelectedItems();
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelListBox::OnSetFocus() 
{
	RepaintSelectedItems();
}
