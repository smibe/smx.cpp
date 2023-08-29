/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: OXDirDlg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/OXDirDlg.h,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Specification : COXDirectoryDialog
// ==========================================================================

// Header file : oxdirdlg.h

// Source : R.Mortelmans & F.Melendez
// Creation Date : 	   28th April 1995
// Last Modification : 10th June 1996
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CFileDialog

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	YES Needs a resource (dialog template : FILEOPENORD)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This is new class derived from CFileDialog and that creates a new dialog box 
//  that allows the user to select only a directory. In Windows 95 you can specify
//	EXPLORER look.
//  For more information, read Knowledge Base article Q105497.

// Remark:
//	This file uses resources.
//	The reserved ID ranges are : 23040 -> 23059 and 53040 -> 53059

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __DIRDLG_H__
#define __DIRDLG_H__

class COXDirectoryDialog : public CFileDialog
{
// Data members -------------------------------------------------------------
public:
	
protected:
	CString m_sTitle;
	CString m_sFullDir;
	CString m_sExistingDir;
	BOOL	m_bNewDirAllowed;

	BOOL	m_bAfterInit;
	BOOL	m_bExplorerOn95;

private:
	
// Member functions ---------------------------------------------------------
public:
    COXDirectoryDialog(LPCTSTR pszDefaultDir = NULL,
    	LPCTSTR pszTitle = NULL, BOOL bNewDirAllowed = FALSE,
        DWORD dwFlags = OFN_HIDEREADONLY, CWnd* pParentWnd = NULL);
	// --- In  : pszDefaultDir : The default dir 
	//			 pszTitle : The title of the dialog
	//			 bNewDirAllowed : Whether the creation of a new 
	//                            (not yet existing) directory is allowed
	//			 dwFlags : A combination of one or more flags that allow 
	//					   you to customize the dialog box. 
	//					   For a description of these flags, see the 
	//					   OPENFILENAME structure description in the Windows 
	//					   SDK documentation. 
	//			 pParentWnd :The directory dialog's parent or owner window
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	bool GetCaseSensitive(CString& sDir);
	CString GetDirectory();
	// --- In : 
	// --- Out : 
	// --- Returns : The directory the user specified or an empty string
	//               when the user pressed Cancel
	// --- Effect : 

protected:
	virtual void OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode);

	virtual void OnOK();
	virtual void OnCancel();

#ifdef WIN32
	virtual void OnFolderChange();
	static BOOL IsWin95();
	CString GetFolderPath() const;
#endif

private:
                   
// Message handlers ---------------------------------------------------------
protected:
    //{{AFX_MSG(COXDirectoryDialog)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __DIRDLG_H__
// ==========================================================================
