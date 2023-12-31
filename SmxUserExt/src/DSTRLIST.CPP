/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DSTRLIST.CPP,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/DSTRLIST.CPP,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 						Class Implementation : COXDoubleStringList
// ==========================================================================

// Source file :dstrlist.cpp

// Source : Periphere NV (Frank Melendez)
//			Based upon orginal Microsoft Unsupported Tools on MSDN CD	
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"			// standard MFC include
#include "returns.h"		// internal return codes
#include "dstrlist.h"   	// class specification

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:

// private:
//	STRINGLIST*	m_pHead;
// --- pointer to the head of the linked list

//	STRINGLIST*	m_pCurrent;	
// --- pointer to the current entry in the list

//	STRINGLIST*	m_pTail;
// --- pointer to the tail of the linked list

//	short 		m_nCount;
// --- number of entries in the list

// Member functions ---------------------------------------------------------
// public:

COXDoubleStringList::COXDoubleStringList()
	{
	m_pHead = NULL;
	m_pCurrent = NULL;
	m_pTail = NULL;
	m_nCount = 0;
	}

COXDoubleStringList::~COXDoubleStringList()
	{
	RemoveAll();
	}			  

short COXDoubleStringList::Find(LPCTSTR pStr, int* pnSel /* = NULL */)
	{
	LPTSTR lp;
	int i;

	for (i = 0, lp = GetFirst(); lp; lp = GetNext(), i++)
		{
		if (_tcscmp(pStr, lp) == 0)
			{
			if (pnSel != NULL)
				*pnSel = i;

			return SUCCESS;
			}
		}

	return F_NOTFOUND;
	}

LPTSTR COXDoubleStringList::GetAt(short nIndex)
	{
	LPTSTR lp;
	int i;

	if (nIndex > m_nCount)
		return NULL;

	for (i = 0, lp = GetFirst(); lp; lp = GetNext(), i++)
		{
		if (i == nIndex)
			return lp;
		}

	return NULL;
	}

void COXDoubleStringList::SetMarked(short nIndex, BOOL bMark)
	{
	int i;
	STRINGLIST *pList;

	if (nIndex > m_nCount)
		return;

	for (i = 0, pList = m_pHead; pList; pList = pList->pNext, i++)
		{
		if (i == nIndex)
			{		
			pList->bMarked = bMark;	
			return;
			}
		}
	}

BOOL COXDoubleStringList::IsMarked(short nIndex)
	{
	int i;
	STRINGLIST *pList;

	if (nIndex > m_nCount)
		return FALSE;

	for (i = 0, pList = m_pHead; pList; pList = pList->pNext, i++)
		{
		if (i == nIndex)
			{		
			return pList->bMarked;
			}
		}
	
	return FALSE;
	}

short COXDoubleStringList::AddUnique(LPCTSTR pStr)
	{
	if (Find(pStr) == SUCCESS)
		return F_CANCEL;

	return Add(pStr);
	}

short COXDoubleStringList::Add(LPCTSTR pStr)
	{
	int nLen;

	if (m_pHead == NULL)
	    {
    	m_pHead = new STRINGLIST;
        if (m_pHead == NULL)
	        {
            return F_MEMORY;
    	    }

		m_pHead->pPrev = NULL;
        m_pTail = m_pHead;
    	}
    else
    	{
        m_pTail->pNext = new STRINGLIST;
        if (m_pTail->pNext == NULL)
        	{
			RemoveAll();
            return F_MEMORY;
       		}

		m_pTail->pNext->pPrev = m_pTail;
        m_pTail = m_pTail->pNext;
    	}                   

	// Init the entry                          
	m_pTail->pNext = NULL;

	nLen = _tcsclen(pStr);

    m_pTail->pString = new TCHAR[nLen+1];
    if (m_pTail->pString == NULL)
    	{
		RemoveAll();
        return F_MEMORY;
    	}

	_tcscpy(m_pTail->pString, pStr);
	m_pTail->bMarked = FALSE;
	m_nCount ++;

	return SUCCESS;
	}

short COXDoubleStringList::Remove(int nSel)
	{
	int i;

	for (i = 0, m_pCurrent = m_pHead; m_pCurrent; m_pCurrent = m_pCurrent->pNext, i++)
		{
		if (i == nSel)
			{
			// is this the head
			if (m_pHead == m_pCurrent)
				{
				m_pHead = m_pCurrent->pNext;
				if (m_pHead)
					m_pHead->pPrev = NULL;
				}
			else if (m_pCurrent == m_pTail) // is this the tail
				{
				m_pCurrent->pPrev->pNext = NULL;
				m_pTail = m_pCurrent->pPrev;
				}
			else
				{
				m_pCurrent->pPrev->pNext = m_pCurrent->pNext;
				m_pCurrent->pNext->pPrev = m_pCurrent->pPrev;
				}

			if (m_pCurrent->pString)
				{
				delete [] m_pCurrent->pString;
				}
        	
        	delete m_pCurrent;
			m_nCount --;

			m_pCurrent = NULL;

			return SUCCESS;
			}
		}
	
	return F_NOTFOUND;
	}

LPTSTR COXDoubleStringList::GetLast()
	{
	if (m_pTail == NULL)
		return NULL;
	m_pCurrent = m_pTail;
	return m_pCurrent->pString;
	}

LPTSTR COXDoubleStringList::GetPrev()
	{
	if (m_pCurrent->pPrev == NULL)
		return NULL;
	
	m_pCurrent = m_pCurrent->pPrev;
	return m_pCurrent->pString;
	}

LPTSTR COXDoubleStringList::GetFirst()
	{
	if (m_pHead == NULL)
		return NULL;
	
	m_pCurrent = m_pHead;
	return m_pCurrent->pString;
	}

LPTSTR COXDoubleStringList::GetNext()
	{
	if (m_pCurrent->pNext == NULL)
		return NULL;
	
	m_pCurrent = m_pCurrent->pNext;
	return m_pCurrent->pString;
	}


void COXDoubleStringList::RemoveAll()
	{
    // Free all the memory allocated
    m_pCurrent = m_pHead;
    while (m_pHead != NULL)
    	{
        m_pHead = m_pCurrent->pNext;
		if (m_pCurrent->pString)
			{
			delete [] m_pCurrent->pString;
			}
        delete m_pCurrent;
        m_pCurrent = m_pHead;
    	}
    
    m_pHead = NULL;
    m_pTail = NULL;
    m_pCurrent = NULL;
	m_nCount = 0;
	}
