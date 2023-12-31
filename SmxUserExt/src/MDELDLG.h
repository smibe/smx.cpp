/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: MDELDLG.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/MDELDLG.h,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 					Class Specification : COXModelessDialog
// ==========================================================================

// Header file : mdeldlg.h

// Source : Periphere NV (Frank Melendez)
//			Based upon orginal MSJ article June 1995	
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CDialog

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES	Has a message map
//	YES/NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class is a generic class which can be used for all modeless dialogs

// Remark:
//	The C++ object will de deleted when the the window is destroyed.
//	This is the default behaviour.  If you want to disable this,
//	 specify bAutoDelete = FALSE when constructing the dialog

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __MDELDLG_H__
#define __MDELDLG_H__

class COXModelessDialog : public CDialog
{
DECLARE_DYNAMIC(COXModelessDialog)

// Data members -------------------------------------------------------------
public:
	COXModelessDialog** m_ppSelf;
	// --- Pointer to pointer to self
	
protected:
	BOOL	m_bAutoDelete;					 

private:
	
// Member functions ---------------------------------------------------------
public:
	COXModelessDialog(COXModelessDialog** ppSelf = NULL, BOOL bAutoDelete = TRUE);
	// --- In  : ppSelf : pointer to this object, used to null out any references the
	//			 		  user could have made to this dialog
	//			 bAutoDelete : whether the object will destroy the C++ object
	//						   automatically when the Windows object is destroyed
	//						   (from within the PostNcDestroy fuction)
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

  	virtual void PostNcDestroy();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Called by the default OnNcDestroy member function after the window
	//				has been destroyed. Derived classes can use this function
	//				for custom cleanup such as the deletion of the this pointer.
	// 				Delete self after window has been destroyed, to avoid TRACE
	// 				message in CWnd. This assumes dialog object is created in the heap, 
	// 				not on the stack when you use the m_bAutoDelete parameter!!
 
    virtual void OnOK();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Called when the user clicks the OK button (the button with an ID of IDOK).
	//				DestroyWindow is called here

    virtual void OnCancel();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Called when the user clicks the Cancel button (the button with an ID of IDCANCEL).
	//				DestroyWindow is called here

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXModelessDialog();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object, NULL the self-pointer.

protected:      

private:
                   
};

#endif // __MDELDLG_H__
// ==========================================================================
