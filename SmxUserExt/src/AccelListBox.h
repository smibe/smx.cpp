/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelListBox.h,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelListBox.h,v 1.1 2002/09/20 09:37:07 GeH Exp $
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
// File    : AccelListBox.h
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : owner-drawn listbox, if the accel obj associated to the item is
//           locked, it will be drawing in gray.
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ACCELLISTBOX_H__5C990D04_2B60_11D2_BE77_006097AC8D00__INCLUDED_)
#define AFX_ACCELLISTBOX_H__5C990D04_2B60_11D2_BE77_006097AC8D00__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CAccelListBox window

class CAccelListBox : public CListBox
{
// Construction
public:
	CAccelListBox();

// Attributes
public:
	COLORREF m_rgbSelectBack;
	COLORREF m_rgbSelectText;
	COLORREF m_rgbLockText;
	COLORREF m_rgbLockBack;

// Operations
public:
	void RepaintSelectedItems();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccelListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAccelListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAccelListBox)
	afx_msg void OnKillfocus();
	afx_msg void OnSetFocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCELLISTBOX_H__5C990D04_2B60_11D2_BE77_006097AC8D00__INCLUDED_)
