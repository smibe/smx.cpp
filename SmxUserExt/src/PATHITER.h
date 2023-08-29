/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: PATHITER.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/PATHITER.h,v 1.1 2002/10/12 07:20:55 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Specification : COXPathIterator
// ==========================================================================

// Header file : pathiter.h

// Source : Periphere NV (F.Melendez & R.Mortelmans)
// Creation Date : 	   2nd November 1995
// Last Modification : 2nd November 1995
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		This class encapsulates the a path iterator

// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __PATHITER_H__
#define __PATHITER_H__         

#ifndef WIN32
	#include <dos.h>
#endif

class COXPathSpec;
class COXPathIterator : public CObject
{
friend class COXPathSpec;

DECLARE_DYNAMIC(COXPathIterator)

// Data members -------------------------------------------------------------
public:
	
protected:                   

#ifdef WIN32
	HANDLE 			m_hFindFile;
	WIN32_FIND_DATA m_FindFileData;	
#else
    _find_t			m_FileInfo;
#endif

	BOOL 			m_bValid;
private:
	
// Member functions ---------------------------------------------------------
public:
	COXPathIterator();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
	
	BOOL IsItValid();
	// --- In  :
	// --- Out : 
	// --- Returns : whether the iterator can be used in a iteration proces
	//				 or not.
	// --- Effect : 
	
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXPathIterator();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

};

#endif
// ==========================================================================
