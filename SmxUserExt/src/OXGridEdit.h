/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: OXGridEdit.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/OXGridEdit.h,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 					Class Specification : COXGridEdit
// ==========================================================================

// Header file : OXGridEdit.h

// Source : R.Mortelmans
// Creation Date : 	   26th April 1996
// Last Modification : 18th June 1996
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CEdit

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO	Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :       
//	This class is used to subclass the edit control of a list control (during editing)  

// Remark:
//	Most functions are only interesting for a subclassed edit control of the list control

//	Setting the window text of the edit control right after it has been subclassed
//   seems to destroy (!) the edit control, therefore SetDeferedWindowText will
//	 postpone this until the first OnSize event is generated.
//	Setting the window text earlier (e.g. OnSetFocus, OnShowWindow etc) will
//	 also destroy the window
//	This behaviuor was detected on Win NT 3.51

// The editing can be ended by several different keys
//		Enter : keep the changes
//		Esc : Discard the changes
//		Tab or Insert : Keep the changes and start editing the next subitem in the list control
//		Shift Tab : Keep the changes and start editing the previous subitem in the list control
//	These last two ways are additions of these class, the list control can
//	differentiate between them by calling GetEndTabKey()

// Prerequisites (necessary conditions):

/////////////////////////////////////////////////////////////////////////////

#ifndef __OXGRIDEDIT_H__
#define __OXGRIDEDIT_H__

class COXGridEdit : public CEdit
{
DECLARE_DYNAMIC(COXGridEdit);

// Data members -------------------------------------------------------------
public:
protected:
	CString m_sDeferedWindowText;
	BOOL m_bDeferedWindowText;
	CPoint m_ptWindowPos;
	BOOL m_bWindowPos;
	int m_nWindowHeight;
	BOOL m_bWindowHeight;
	int m_nWindowWidth;
	BOOL m_bWindowWidth;
	int m_nCXOffset;
	BOOL m_bAdjustWindowWidth;
	UINT m_nEndKeyChar;
	BOOL m_bEndKeyShift;
	BOOL m_bEndKeyCtrl;
	int	m_nRightParentBorder;

private:
	
// Member functions ---------------------------------------------------------
public:
	COXGridEdit();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Constructs the object

	void Initialize();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Initializes the object
	//				This function should be called before subclassing an edit control

	void SetDeferedWindowText(LPCTSTR lpszString);
	// --- In  : lpszString : The new window text
	// --- Out : 
	// --- Returns :
	// --- Effect : Set the new window text of this control
	//				This text will only be set during the next OnSize event

	void SetWindowPos(const CPoint& pt);
	// --- In  : pt : The new top left corner of the control
	// --- Out : 
	// --- Returns :
	// --- Effect : This new position will be used to overide every other position
	//				during a OnWindowPosChanging

	void SetWindowHeight(int nHeight);
	// --- In  : nHeight : The new height of the control
	// --- Out : 
	// --- Returns :
	// --- Effect : This height will be used to overide every other height
	//				during a OnWindowPosChanging

	void SetWindowWidth(int nWidth);
	// --- In  : nWidth : The new width of the control
	// --- Out : 
	// --- Returns :
	// --- Effect : This width will be used to overide every other width
	//				during a OnWindowPosChanging

	void AdjustWindowWidth(int nWidth);
	// --- In  : nWidth : An offset that will be added to width of the wondow text
	//					  to calculate the width of the edit control
	// --- Out : 
	// --- Returns :
	// --- Effect : By setting this additional width, the origianl width will be discarded
	//				The width will be calculated from the width of the window text
	//				and the nWidth will be added to this number

	BOOL GetEndKey(UINT& nChar, BOOL& bShift, BOOL& bCtrl);
	// --- In  :
	// --- Out : nChar : Virtual key code of the key that ended the editing
	//			 bShift : Whether Shift was pressed as well
	//			 bCtrl : Whether Ctrl was pressed as well
	// --- Returns : Whether the editing was ended by pressing a key
	// --- Effect : 

#ifdef _DEBUG
	virtual void AssertValid() const;
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : AssertValid performs a validity check on this object 
	//				by checking its internal state. 
	//				In the Debug version of the library, AssertValid may assert and 
	//				thus terminate the program.
	
	virtual void Dump(CDumpContext& dc) const;
	// --- In  : dc : The diagnostic dump context for dumping, usually afxDump.
	// --- Out : 
	// --- Returns :
	// --- Effect : Dumps the contents of the object to a CDumpContext object. 
	//				It provides diagnostic services for yourself and 
	//				 other users of your class. 
	//				Note  The Dump function does not print a newline character
	//				 at the end of its output. 
#endif

	virtual ~COXGridEdit();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of the object

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COXGridEdit)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(COXGridEdit)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
                   
};

#endif // __OXGRIDEDIT_H__
// ==========================================================================
