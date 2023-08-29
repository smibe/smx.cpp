/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: PATHITER.CPP,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/PATHITER.CPP,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Implementation : COXPathIterator
// ==========================================================================

// Source file : pathiter.cpp

// Source : Periphere NV (F.Melendez & R.Mortelmans)
// Creation Date : 	   2nd November 1995
// Last Modification : 2nd November 1995
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard MFC include
#include "pathiter.h"		// class specification

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXPathIterator, CObject)

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

// private:

// Member functions ---------------------------------------------------------
// public:

COXPathIterator::COXPathIterator()
	: m_bValid(FALSE)
	{
	}
	
COXPathIterator::IsItValid()
	{
	return m_bValid;
	}               
	
#ifdef _DEBUG
void COXPathIterator::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
#ifdef WIN32
	dc << TEXT("\nm_hFindFile : ") << m_hFindFile;
	dc << TEXT("\nm_FindFileData : ") << (void*)&m_FindFileData;	
#else
    dc << TEXT("\nm_FileInfo : ") << (void*)&m_FileInfo;
#endif
	dc << TEXT("\nm_bValid : ") << (WORD)m_bValid;
	}

void COXPathIterator::AssertValid() const
	{
	CObject::AssertValid();
	}
#endif

COXPathIterator::~COXPathIterator()
	{
#ifdef WIN32	
	if (m_hFindFile != NULL)
		FindClose(m_hFindFile);
#endif
	}

// private:

// Message handlers ---------------------------------------------------------

// ==========================================================================
