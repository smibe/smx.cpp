/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CPYSTDLG.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/CPYSTDLG.h,v 1.1 2002/10/12 07:20:53 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Specification : COXCopyStatusDialog
// ==========================================================================

// Header file : cpystdlg.h

// Source : Periphere NV (Frank Melendez)
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from COXModelessDialog

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	YES/NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class is used to give the user some feedback regarding the copy process
//	The user is also able to cancel the process via the cancel button

// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __COPYSTATUS_H__
#define __COPYSTATUS_H__

#include "mdeldlg.h"

class COXCopyStatusDialog : public COXModelessDialog
{
DECLARE_DYNAMIC(COXCopyStatusDialog)

// Data members -------------------------------------------------------------
public:
	enum ECopyStatus	
		{ 
		CSUndefined		=   0, 
		CSCopying		=   1,
		CSRemoving		=   2,
		};

	static const ECopyStatus CS_FIRST;
	static const ECopyStatus CS_LAST;
							   	
protected:
	BOOL 		m_bCancelled;
	CWnd*		m_pParentWnd;

	CStatic*	m_pFromLabel;
	CStatic*	m_pToLabel;
	CStatic*	m_pFromMsg;
	CStatic*	m_pToMsg;
						 
private:
	
// Member functions ---------------------------------------------------------
public:
	COXCopyStatusDialog(COXCopyStatusDialog** ppSelf = NULL, CWnd* pParentWnd = NULL);
	// --- In  : ppSelf : a pointer to itself, used to null out automatic
	//					  any reference the user made to this dialog
	//			 pParentWnd : the parent of this modeless dialog
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	virtual BOOL Create();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Creates the Resource template in memory and Calls CreateIndirect
	//				to finally create the modeless dlg with it.

	virtual BOOL IsCancelled();
	// --- In  : 
	// --- Out : 
	// --- Returns : Whether the user has pushed the cancel button or not
	// --- Effect : 
    
    virtual void OnCancel();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Called when the user clicks the Cancel button (the button with an ID of IDCANCEL).
	//				The Cancelled member is set

	virtual void SetStatusText(ECopyStatus eCopyStatus, LPCTSTR pszFromText = _T(""), LPCTSTR pszToText = _T(""));
	// --- In  : eCopyStatus : The status of the copy process
	//			 pszFromText : the text representing the file you're copying from
	//			 pszToText : the text representing the file you're copying to
	// --- Out : 
	// --- Returns : Succeeded or not
	// --- Effect : Gives the user some feedback on the copy process by setting some text to
	//				a few Static variables.

	static int nCopyAnsiToWideChar (LPWORD lpWCStr, LPTSTR lpAnsiIn);
	// --- In  : lpWCStr : Wide char String to copy to
	//			 lpAnsiIn : ansi String to copy from  
	// --- Out : 
	// --- Returns :
	// --- Effect :Takes second parameter as Ansi string, copies
	//     		   it to first parameter as wide character (16-bits / char) string,
	//     		   and returns integer number of wide characters (words) in string
	//     		   including the trailing wide char NULL)

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXCopyStatusDialog();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      
	BOOL CreateControls();
	BOOL CreateStatic(UINT nIDC, LPCTSTR pszText = _T("") , const CRect& Position = CRect(0,0,0,0));

   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //{{AFX_MSG(CFindDialog)
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()

private:
                   
};

#endif
// ==========================================================================
