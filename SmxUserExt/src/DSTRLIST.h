/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DSTRLIST.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/DSTRLIST.h,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Specification : COXDoubleStringList
// ==========================================================================

// Header file : dStrList.h

// Source : Periphere NV (Frank Melendez)
//			Based upon orginal Microsoft Unsupported Tools on MSDN CD	
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	NO	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//		This class contains support for a doublely linked linked string list.  
//		Therefore the linked list contains previous and next pointers
// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////

#ifndef __DSTRLIST_H__
#define __DSTRLIST_H__


typedef struct StringList
	{
	LPTSTR				pString;
	BOOL 				bMarked;
	struct StringList*	pPrev;
	struct StringList*	pNext;
	} STRINGLIST;

class COXDoubleStringList
{
// Data members -------------------------------------------------------------
public:
protected:
private:
	STRINGLIST*	m_pHead;				
	STRINGLIST*	m_pCurrent;				
	STRINGLIST*	m_pTail;				
	short 		m_nCount;				

// Member functions ---------------------------------------------------------
public:
	COXDoubleStringList();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state

	short GetCount() 
		{ return m_nCount; }
	// --- In  : 
	// --- Out : 
	// --- Returns : Number of elements in this linked list
	// --- Effect : 

	short Add(LPCTSTR lpcszItem);				
	// --- In  : lpcszItem : the item to be added
	// --- Out : 
	// --- Returns : SUCCESS or F_MEMORY if memory exception occurred
	// --- Effect : add to the list

	short AddUnique(LPCTSTR lpcszItem);			
	// --- In  : lpcszItem : the item to be added
	// --- Out : 
	// --- Returns : SUCCESS or F_MEMORY if memory exception occurred
	// --- Effect : add to the list

	short Find(LPCTSTR pStr, int* pnSel = NULL);
	// --- In  :  pStr : the item to be searched
	// --- Out :  pnSel : pointer to the index of the item if found
	// --- Returns : SUCCESS or F_NOTFOUND if not found
	// --- Effect : search item in linked list

	short Remove(int nSel);
	// --- In  :  nSel : the index of item to be removed
	// --- Out :  
	// --- Returns : SUCCESS or F_NOTFOUND if not found
	// --- Effect : delete item in linked list

	LPTSTR GetFirst();					
	// --- In  :  
	// --- Out :  
	// --- Returns : The first string or NULL if END
	// --- Effect : get the first string

	LPTSTR GetPrev();					
	// --- In  :  
	// --- Out :  
	// --- Returns : The prev string or NULL if END
	// --- Effect : get the prev string

	LPTSTR GetLast();					
	// --- In  :  
	// --- Out :  
	// --- Returns : The last string or NULL if END
	// --- Effect : get the last string

	LPTSTR GetNext();					
	// --- In  :  
	// --- Out :  
	// --- Returns : The next string or NULL if END
	// --- Effect : get the next string

	LPTSTR GetAt(short nIndex);
	// --- In  : nIndex : the index of the item to be fetched 
	// --- Out :  
	// --- Returns : The string or NULL if not found
	// --- Effect : gets a string on an certain index

	void SetMarked(short nIndex, BOOL bMark);		// mark or unmark an entry
	// --- In  : nIndex : the item to be marked/unmarked
	//			 bMark : TRUE to mark, ... 
	// --- Out :  
	// --- Returns : 
	// --- Effect : mark or unmark an entry

	BOOL IsMarked(short nIndex);				
	// --- In  : nIndex : index of the item 
	// --- Out :  
	// --- Returns : whether the item with nIndex is marked or not
	// --- Effect : 

	void RemoveAll();					
	// --- In  :  
	// --- Out :  
	// --- Returns : 
	// --- Effect : Removes all items from this linked list

	virtual ~COXDoubleStringList();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:
private:
};		
#endif	// __DSTRLIST_H__
// //////////////////////////////////////////////////////////////////////////
