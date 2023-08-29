/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: MDELDLG.CPP,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/MDELDLG.CPP,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 						Class Implementation : COXModelessDialog
// ==========================================================================

// Source file :mdeldlg.cpp

// Source : Periphere NV (Frank Melendez)
//			Based upon orginal MSJ article June 1995	
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard MFC include
#include "mdeldlg.h"	// class specification


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXModelessDialog, CDialog)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:
	// BOOL m_bAutoDelete;
	// --- Whether the c++ object should be deleted when the window is destroyed

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXModelessDialog::COXModelessDialog(COXModelessDialog** ppSelf /* = NULL */, BOOL bAutoDelete /* = TRUE */)
	: 
	m_bAutoDelete(bAutoDelete)
	{
	// ... Pointer to pointer to self
    m_ppSelf = ppSelf;                     
	}

void COXModelessDialog::PostNcDestroy() 
	{
	if (m_bAutoDelete)
    	delete this;
	}

void COXModelessDialog::OnOK()
	{
   	if (!UpdateData(TRUE))
   		// Do data exchange
		{
    	TRACE(_T("COXModelessDialog::OnOK : UpdateData failed during dialog termination\n"));
    
       	// ... The UpdateData routine will set focus to correct item
		return;
   		}
   	
   	DestroyWindow();
	}

void COXModelessDialog::OnCancel()
	{
	// ... (without updating data)
    DestroyWindow();       
	}

#ifdef _DEBUG
void COXModelessDialog::Dump(CDumpContext& dc) const
	{
	CDialog::Dump(dc);
	dc << _T("\nm_ppSelf : ") << (const void*)m_ppSelf;
	}
	
void COXModelessDialog::AssertValid() const
	{
	CObject::AssertValid();
    ASSERT(m_ppSelf == NULL || *m_ppSelf == this);
	}
#endif

COXModelessDialog::~COXModelessDialog()
	{
    ASSERT(m_ppSelf == NULL || *m_ppSelf == this);
    if (m_ppSelf != NULL && m_bAutoDelete)
		*m_ppSelf = NULL;
	}
	
// protected:

// private:

// ==========================================================================
