/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AcceleratorManager.cpp,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AcceleratorManager.cpp,v 1.3 2003/02/21 08:56:51 GeH Exp $
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
// File    : AcceleratorManager.cpp
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : implementation of the CAcceleratorManager class.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "AcceleratorManager.h"
#include "SmxRes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


int CmpUint(const void* elem1, const void* elem2)
{
  int nVal1 = *((int*) elem1);
  int nVal2 = *((int*) elem2);
  return nVal1 - nVal2;
}

//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////
//
//
CAcceleratorManager::CAcceleratorManager()
{
	m_hRegKey       = HKEY_CURRENT_USER;
	m_bAutoSave     = FALSE;
	m_pWndConnected = NULL;
  m_nAccelTableID = NULL;
  m_pDocTemplate  = NULL;
  m_pFrameClass   = NULL;
  m_nUserCmdIDFirst = 0;
  m_nUserCmdIDLast  = 0;
  m_bGroupsInitialized = false;
  m_pAll = NULL;
  m_pUserTools = NULL;
}


//////////////////////////////////////////////////////////////////////
//
//
CAcceleratorManager::~CAcceleratorManager()
{
	if ((m_bAutoSave == true) && (m_szRegKey.IsEmpty() == FALSE)) 
  {
		bool bRet = Write();
		if (!bRet)
			AfxMessageBox("CAcceleratorManager::~CAcceleratorManager\nError in CAcceleratorManager::Write...");
	}

  delete [] m_pAll;
  delete [] m_pUserTools;

	Reset();
}


//////////////////////////////////////////////////////////////////////
// Internal fcts
//////////////////////////////////////////////////////////////////////
//
//
void CAcceleratorManager::Reset(bool bRemoveUserCmd)
{
	CCmdAccelOb* pCmdAccel;
	UINT nCmdID;
	POSITION pos = m_mapAccelTable.GetStartPosition();
	while (pos != NULL) 
  {
    m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);

    if (bRemoveUserCmd)
		  delete pCmdAccel;
    else
    { 
      if (!(m_nUserCmdIDFirst <= nCmdID && nCmdID <= m_nUserCmdIDLast))
      {
        //This is not a user command, remove it
  		  delete pCmdAccel;
        m_mapAccelTable.RemoveKey(nCmdID);
      }
      else
      {
        //This is an user command, just remove the accelerators (Could be the same as our default ones)
        pCmdAccel->DeleteUserAccels();
      }
      
    }
	}

  if (bRemoveUserCmd)
  	m_mapAccelTable.RemoveAll();
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::AddAccel(BYTE cVirt, UINT nCommandID, WORD wKey, bool bLocked)
{
	CCmdAccelOb* pCmdAccel = NULL;
	if (wKey && m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE) 
  {
    CAccelsOb* pAccel;
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) 
    {
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel->m_cVirt == cVirt &&	pAccel->m_wKey == wKey)
				return FALSE;
		}

    // Adding the accelerator
		pCmdAccel->Add(cVirt, wKey, bLocked);

	} 
  else 
  {
		pCmdAccel = new CCmdAccelOb(cVirt, nCommandID, wKey, bLocked);
		ASSERT(pCmdAccel != NULL);
		m_mapAccelTable.SetAt(nCommandID, pCmdAccel);
	}

  return true;
}


void CAcceleratorManager::Connect(CFrameWnd* pWnd, bool bAutoSave)
{
	ASSERT(m_pWndConnected == NULL && m_pDocTemplate == NULL);
	m_pWndConnected = pWnd;
	m_bAutoSave = bAutoSave;
  UpdateWndTable();
}

void CAcceleratorManager::Connect(CMultiDocTemplate* pDocTempl, CRuntimeClass *pFrameClass, bool bAutoSave)
{
	ASSERT(m_pWndConnected == NULL && m_pDocTemplate == NULL);
  m_pDocTemplate  = pDocTempl;
  m_pFrameClass   = pFrameClass;
	m_bAutoSave = bAutoSave;
  UpdateWndTable();
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::GetRegKey(HKEY& hRegKey, CString& szRegKey)
{
	if (m_szRegKey.IsEmpty())
		return false;

	hRegKey = m_hRegKey;
	szRegKey = m_szRegKey;
	return true;
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::SetRegKey(HKEY hRegKey, LPCTSTR szRegKey)
{
	ASSERT(hRegKey != NULL);
	ASSERT(szRegKey != NULL);

	m_szRegKey = szRegKey;
	m_hRegKey = hRegKey;
	return true;
}


//////////////////////////////////////////////////////////////////////
// Update the application's ACCELs table
//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::UpdateWndTable()
{
  bool bResult  = true;
  
  if (m_pWndConnected)
  {
    return UpdateWndTable(m_pWndConnected);
  }
  if (m_pDocTemplate)
  {
    return UpdateWndTable(m_pDocTemplate);
  }
  return false;
}

bool CAcceleratorManager::UpdateWndTable(CFrameWnd *pFrame)
{
  if (!pFrame)
    return false;

  HACCEL hNewTable = CreateAcceleratorTable();

  if (!hNewTable)
    return false;

  if (!::DestroyAcceleratorTable(pFrame->m_hAccelTable)) 
  {
		::DestroyAcceleratorTable(hNewTable);
		return false;
	}

  pFrame->m_hAccelTable = hNewTable;

  return true;
}

bool CAcceleratorManager::UpdateWndTable(CMultiDocTemplate *pTemplate)
{
  if (!pTemplate)
    return false;

  HACCEL hNewTable = CreateAcceleratorTable();
  HACCEL hOldTable = pTemplate->m_hAccelTable;
  if (!hNewTable)
    return false;

  if (!::DestroyAcceleratorTable(hOldTable)) 
  {
		::DestroyAcceleratorTable(hNewTable);
		return false;
	}

  pTemplate->m_hAccelTable = hNewTable;

  //Loop through all MDI child windows and set the new Accelerator table
  CWnd *pWnd = AfxGetMainWnd();
  if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
  {
    CMDIFrameWnd *pFrame = (CMDIFrameWnd *) pWnd;
    if (pFrame->m_hWndMDIClient)
    {
      pWnd = CWnd::FromHandle(::GetWindow(pFrame->m_hWndMDIClient, GW_CHILD));

      while(pWnd)
      {
        if (pWnd->IsKindOf(m_pFrameClass))
        {
          ASSERT(((CFrameWnd *)pWnd)->m_hAccelTable == hOldTable);
    
          ((CFrameWnd *)pWnd)->m_hAccelTable = hNewTable;
        }

        pWnd = pWnd->GetWindow(GW_HWNDNEXT);
      }
    }
  }

  return true;
}

HACCEL CAcceleratorManager::CreateAcceleratorTable()
{
	int iLoop = 0;
	CTypedPtrArray<CPtrArray, LPACCEL> arrayACCEL;

	CCmdAccelOb* pCmdAccel;
	UINT nCmdID;
	LPACCEL pACCEL;
	CAccelsOb* pAccelOb;

  POSITION pos = m_mapAccelTable.GetStartPosition();
	while (pos != NULL) 
  {
		m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) 
    {
			pAccelOb = pCmdAccel->m_Accels.GetNext(pos);

			pACCEL = new ACCEL;
			ASSERT(pACCEL != NULL);
			pACCEL->fVirt = pAccelOb->m_cVirt;
			pACCEL->key = pAccelOb->m_wKey;
			pACCEL->cmd = pCmdAccel->GetCommandID();
			arrayACCEL.Add(pACCEL);
		}
	}
	
	int nAccel = arrayACCEL.GetSize();
	LPACCEL lpAccel = (LPACCEL)LocalAlloc(LPTR, nAccel * sizeof(ACCEL));
	if (!lpAccel) 
  {
		for (iLoop = 0; iLoop < nAccel; iLoop++)
			delete arrayACCEL.GetAt(iLoop);
		arrayACCEL.RemoveAll();

		return NULL;
	}

	for (iLoop = 0; iLoop < nAccel; iLoop++) 
  {
		
		pACCEL = arrayACCEL.GetAt(iLoop);
		lpAccel[iLoop].fVirt = pACCEL->fVirt;
		lpAccel[iLoop].key = pACCEL->key;
		lpAccel[iLoop].cmd = pACCEL->cmd;

		delete pACCEL;
	}
	arrayACCEL.RemoveAll();

  HACCEL hNewTable = ::CreateAcceleratorTable(lpAccel, nAccel);
	if (!hNewTable) 
  {
		::LocalFree(lpAccel);
		return NULL;
	}

  ::LocalFree(lpAccel);
	return hNewTable;
}

/*
bool CAcceleratorManager::UpdateWndTable(CWnd *pWnd, HACCEL& hWndAccel, CMultiDocTemplate *pTemplate)
{
	int iLoop = 0;
	CTypedPtrArray<CPtrArray, LPACCEL> arrayACCEL;

	CCmdAccelOb* pCmdAccel;
	UINT nCmdID;
	LPACCEL pACCEL;
	CAccelsOb* pAccelOb;

  if (pWnd == NULL && pTemplate == NULL)
  {
    //There is nothing to do
    return true;
  }

  POSITION pos = m_mapAccelTable.GetStartPosition();
	while (pos != NULL) 
  {
		m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) {
			pAccelOb = pCmdAccel->m_Accels.GetNext(pos);

			pACCEL = new ACCEL;
			ASSERT(pACCEL != NULL);
			pACCEL->fVirt = pAccelOb->m_cVirt;
			pACCEL->key = pAccelOb->m_wKey;
			pACCEL->cmd = pCmdAccel->GetCommandID();
			arrayACCEL.Add(pACCEL);
		}
	}
	
	int nAccel = arrayACCEL.GetSize();
	LPACCEL lpAccel = (LPACCEL)LocalAlloc(LPTR, nAccel * sizeof(ACCEL));
	if (!lpAccel) 
  {
		for (iLoop = 0; iLoop < nAccel; iLoop++)
			delete arrayACCEL.GetAt(iLoop);
		arrayACCEL.RemoveAll();

		return false;
	}

	for (iLoop = 0; iLoop < nAccel; iLoop++) 
  {
		
		pACCEL = arrayACCEL.GetAt(iLoop);
		lpAccel[iLoop].fVirt = pACCEL->fVirt;
		lpAccel[iLoop].key = pACCEL->key;
		lpAccel[iLoop].cmd = pACCEL->cmd;

		delete pACCEL;
	}
	arrayACCEL.RemoveAll();

  HACCEL hNewTable = CreateAcceleratorTable(lpAccel, nAccel);
	if (!hNewTable) 
  {
		::LocalFree(lpAccel);
		return false;
	}

	
  HACCEL hOldTable = NULL;
  
  if (pWnd)
    hOldTable = hWndAccel;
  else if (pTemplate)
    hOldTable = pTemplate->m_hAccelTable;

	if (!::DestroyAcceleratorTable(hOldTable)) 
  {
		::DestroyAcceleratorTable(hNewTable);
    ::LocalFree(lpAccel);
		return false;
	}
	
  if (pWnd)
    hWndAccel = hNewTable;
  else if (pTemplate)
  {
    pTemplate->m_hAccelTable = hNewTable;

    //Loop through all MDI child windows and set the new Accelerator table
    CWnd *pWnd = AfxGetMainWnd();
    if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
    {
      CMDIFrameWnd *pFrame = (CMDIFrameWnd *) pWnd;
      if (pFrame->m_hWndMDIClient)
      {
        pWnd = CWnd::FromHandle(::GetWindow(pFrame->m_hWndMDIClient, GW_CHILD));

        while(pWnd)
        {
          if (pWnd->IsKindOf(m_pFrameClass))
          {
            ASSERT(((CFrameWnd *)pWnd)->m_hAccelTable == hOldTable);
      
            ((CFrameWnd *)pWnd)->m_hAccelTable = hNewTable;
          }

          pWnd = pWnd->GetWindow(GW_HWNDNEXT);
        }
      }
    }
  }

  ::LocalFree(lpAccel);
	return true;
}
***/


//////////////////////////////////////////////////////////////////////
// Create/Destroy accelerators
//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::DeleteAccel(BYTE cVirt, UINT nCommandID, WORD wKey)
{
	CCmdAccelOb* pCmdAccel = NULL;
	if (m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE) {
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		POSITION PrevPos;
		CAccelsOb* pAccel = NULL;
		while (pos != NULL) {
			PrevPos = pos;
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel->m_bLocked == true)
				return false;

			if (pAccel->m_cVirt == cVirt && pAccel->m_wKey == wKey) {
				pCmdAccel->m_Accels.RemoveAt(PrevPos);
				delete pAccel;
				return true;
			}
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::DeleteEntry(UINT nCommandID)
{
	CCmdAccelOb* pCmdAccel = NULL;
	VERIFY(m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE);

	CAccelsOb* pAccel;
	POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
	while (pos != NULL) {
		pAccel = pCmdAccel->m_Accels.GetNext(pos);
		if (pAccel->m_bLocked == true)
			return false;
	}
	m_mapAccelTable.RemoveKey(nCommandID);
	delete pCmdAccel;

	return true;
}



//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::SetAccel(BYTE cVirt, UINT nCommandID, WORD wKey, bool bLocked)
{
	return AddAccel(cVirt, nCommandID, wKey, bLocked);
}


//////////////////////////////////////////////////////////////////////
//
//
/*
bool CAcceleratorManager::AddCommandAccel(UINT nCommandID, bool bLocked)
{
	ASSERT(m_pWndConnected != NULL || m_pDocTemplate != NULL);
	HACCEL hOriginalTable = NULL;
  
  if (m_pWndConnected)
    hOriginalTable = m_pWndConnected->m_hAccelTable;
  else 
    hOriginalTable = m_pWndConnected->m_hAccelTable;

	int nAccel = ::CopyAcceleratorTable(hOriginalTable, NULL, 0);
	LPACCEL lpAccel = (LPACCEL)LocalAlloc(LPTR, (nAccel) * sizeof(ACCEL));
	if (!lpAccel)
		return false;
	::CopyAcceleratorTable(hOriginalTable, lpAccel, nAccel);

	bool bRet = false;
	for (int i = 0; i < nAccel; i++) 
  {
		if (lpAccel[i].cmd == nCommandID)
			bRet = AddAccel(lpAccel[i].fVirt, nCommandID, lpAccel[i].key, bLocked);
	}
	::LocalFree(lpAccel);
	return bRet;
}
*/


//////////////////////////////////////////////////////////////////////
//
//
CCmdAccelOb *CAcceleratorManager::CreateEntry(UINT nCommandID)
{
	CCmdAccelOb* pCmdAccel = new CCmdAccelOb(nCommandID);
	ASSERT(pCmdAccel != NULL);
	m_mapAccelTable.SetAt(nCommandID, pCmdAccel);

	return pCmdAccel;
}


//////////////////////////////////////////////////////////////////////
// Get a string from the ACCEL definition
//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::GetStringFromACCEL(ACCEL* pACCEL, CString& strAccel)
{
	ASSERT(pACCEL != NULL);
	
	CAccelsOb accel(pACCEL);
	accel.GetString(strAccel);

	if (strAccel.IsEmpty())
		return false;
	else
		return true;
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::GetStringFromACCEL(BYTE cVirt, WORD nCode, CString& strAccel)
{
	CAccelsOb accel(cVirt, nCode);
	accel.GetString(strAccel);

	if (strAccel.IsEmpty())
		return false;
	else
		return true;
}


//////////////////////////////////////////////////////////////////////
// Copy function
//
CAcceleratorManager& CAcceleratorManager::operator=(const CAcceleratorManager& accelmgr)
{
	Reset();

	CCmdAccelOb* pCmdAccel;
	CCmdAccelOb* pNewCmdAccel;
	UINT nCmdID;
	// Copy the 2 tables : normal accel table...
	POSITION pos = accelmgr.m_mapAccelTable.GetStartPosition();
	while (pos != NULL) 
  {
		accelmgr.m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);
		pNewCmdAccel = new CCmdAccelOb;
		ASSERT(pNewCmdAccel != NULL);
		*pNewCmdAccel = *pCmdAccel;
		m_mapAccelTable.SetAt(nCmdID, pNewCmdAccel);
	}

  m_nAccelTableID = accelmgr.m_nAccelTableID;
  m_nUserCmdIDFirst = accelmgr.m_nUserCmdIDFirst;
  m_nUserCmdIDLast  = accelmgr.m_nUserCmdIDLast;

  m_bGroupsInitialized = accelmgr.m_bGroupsInitialized;
  m_strView      = accelmgr.m_strView;
  
  if (m_aAccelGroups.GetSize() == 0)
  {
    int nSize = accelmgr.m_aAccelGroups.GetSize();
    m_aAccelGroups.SetSize(nSize);
    for (int i = 0; i < nSize; i++)
    {
      UINT *pGroup = (UINT*) accelmgr.m_aAccelGroups[i];
      m_aAccelGroups[i] = pGroup;
    }

    if (accelmgr.m_pAll)
    {
      delete [] m_pAll;
      m_pAll = new UINT [accelmgr.m_pAll[1] + 2];
      memcpy(m_pAll, accelmgr.m_pAll, sizeof(UINT) * (accelmgr.m_pAll[1] + 2));
      m_aAccelGroups.SetAt(m_aAccelGroups.GetUpperBound(), m_pAll);
    }

    if (accelmgr.m_pUserTools)
    {
      ASSERT(accelmgr.m_pAll);
      delete [] m_pUserTools;
      m_pUserTools = new UINT [accelmgr.m_pUserTools[1] + 2];
      memcpy(m_pUserTools, accelmgr.m_pUserTools, sizeof(UINT) * (accelmgr.m_pUserTools[1] + 2));
      m_aAccelGroups.SetAt(m_aAccelGroups.GetUpperBound() - 1, m_pUserTools);
    }
  }

	return *this;
}



void CAcceleratorManager::SetUserCmdIDs(UINT nCmdIDFirst, UINT nCmdIDLast)
{
  if (nCmdIDLast < nCmdIDFirst)
  {
    ASSERT(FALSE);
    return;
  }

  m_nUserCmdIDFirst = nCmdIDFirst;
  m_nUserCmdIDLast  = nCmdIDLast;
  
  if (m_aAccelGroups.GetSize() <= 0)
    return;

  bool bInsertUserTools = true;
  if (m_pUserTools)
  {
    delete [] m_pUserTools;
    m_pUserTools = NULL;
    bInsertUserTools = false;
  }

  if (nCmdIDFirst > 0 && nCmdIDLast > 0)
  {
    m_pUserTools = new UINT [nCmdIDLast - nCmdIDFirst + 3];
    m_pUserTools[0] = IDS_UserTools;
    m_pUserTools[1] = nCmdIDLast - nCmdIDFirst + 1;
    for (UINT i = nCmdIDFirst; i <= nCmdIDLast; i++)
      m_pUserTools[2 + i - nCmdIDFirst]  = i;

    if (bInsertUserTools)
      m_aAccelGroups.InsertAt(m_aAccelGroups.GetUpperBound(), m_pUserTools);
    else
      m_aAccelGroups.SetAt(m_aAccelGroups.GetUpperBound() - 1, m_pUserTools);
  }
}

void CAcceleratorManager::AddAccelGroup(UINT *pGroup)
{
  m_aAccelGroups.Add(pGroup);
}

void CAcceleratorManager::InitAccelGroups()
{
  if (m_bGroupsInitialized)
    return;

  int   nCountAllGroups = 0;
  UINT *pGroups   = NULL;
  for (int i = 0; i < m_aAccelGroups.GetSize(); i++)
  {
    pGroups = (UINT*)m_aAccelGroups.GetAt(i);
    ASSERT(pGroups);
    nCountAllGroups += pGroups[1];
  }

  UINT *pAllGroups = NULL;
  if (nCountAllGroups)
    pAllGroups = new UINT[nCountAllGroups];
  int nIdx = 0;
  for (i = 0; i < m_aAccelGroups.GetSize(); i++)
  {
    pGroups = (UINT*)m_aAccelGroups.GetAt(i);
    memcpy(&pAllGroups[nIdx], &pGroups[2], pGroups[1] * sizeof(UINT));
    nIdx += pGroups[1];
  }
  
  int nCountAll = m_mapAccelTable.GetCount();
  m_pAll = new UINT[m_mapAccelTable.GetCount()];
  i = 0;
	UINT nCmdID;
	CCmdAccelOb* pCmdAccel;
  int nUserTools = 0;
  for (POSITION pos = m_mapAccelTable.GetStartPosition(); pos; )
  {
		m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);
    m_pAll[i++] = nCmdID;

    if (nCmdID >= m_nUserCmdIDFirst && nCmdID <= m_nUserCmdIDLast)
      nUserTools++;
  }

  qsort(m_pAll, nCountAll, sizeof(UINT), CmpUint);

  if (pAllGroups)
    qsort(pAllGroups, nCountAllGroups, sizeof(UINT), CmpUint);

  int j = 0;
  i = 0;
  //Add the commands not found in the map but available in the group
  LPACCEL  pAccelTab = NULL;
  LPACCEL  pAccel    = NULL;
  int      nAccelCount = 0;
  if (pAllGroups)
  {
    while (i +j < nCountAll + nCountAllGroups)
    {
      if (i < nCountAll && j < nCountAllGroups && m_pAll[i] == pAllGroups[j])
      {
        i++; j++;
        continue;
      }

      if (i >= nCountAll || pAllGroups[j] < m_pAll[i])
      {
        if (pAllGroups[j] == 32812)
        {
          int XXX = 0;
        }
        // Add without shortcut, otherwise we could overwrite user defined 
        // shortcuts
        if (!pAccelTab)
        {
          nAccelCount = CreateResourceAccelTable(pAccelTab, m_nAccelTableID);
        }

        if (pAccelTab)
        {
          pAccel = GetResourceShortcut(pAllGroups[j], pAccelTab, nAccelCount);
          if (pAccel && GetAffected(pAccel))
            pAccel = NULL;
        }

        if (pAccel)
          AddAccel(pAccel->fVirt, pAllGroups[j], pAccel->key, false);
        else
          AddAccel(0, pAllGroups[j], 0, false);

        j++;
      }
      else if (i < nCountAll)
      {
        //ASSERT(false);
        TRACE1("Found accelerator not defined in a group: %d.\n", m_pAll[i]);
        i++;
      }
      else
      {
        ASSERT(false);
      }
    }
  }

  //Update the all tab#le
  delete [] m_pAll;
  m_pAll = 0;
  delete [] pAllGroups;
  pAllGroups = 0;

  if (nUserTools > 0)
  {
    delete [] m_pUserTools;
    m_pUserTools = new UINT[nUserTools + 2];
    m_pUserTools[0] = IDS_UserTools;
    m_pUserTools[1] = nUserTools;
    nUserTools = 2;
  }

  nCountAll = m_mapAccelTable.GetCount();
  m_pAll    = new UINT[nCountAll + 2];
  m_pAll[0] = IDS_All;
  m_pAll[1] = nCountAll;
  i = 2;
  for (pos = m_mapAccelTable.GetStartPosition(); pos; )
  {
		m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);
    m_pAll[i++] = nCmdID;
    if (m_pUserTools && nCmdID >= m_nUserCmdIDFirst && nCmdID <= m_nUserCmdIDLast)
    {
      m_pUserTools[nUserTools++] = nCmdID;
    }
  }

  if (m_pUserTools)
    m_aAccelGroups.Add(m_pUserTools);
  m_aAccelGroups.Add(m_pAll);
  m_bGroupsInitialized = true;
  delete pAccelTab;
}

int CAcceleratorManager::CreateResourceAccelTable(LPACCEL& pAccelTab, UINT nAccelTableID)
{
  HACCEL hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(nAccelTableID));
  if (!hAccel)
    return 0;

  //Copy the accelerators
  int nNofAccels = ::CopyAcceleratorTable(hAccel, NULL, 0);
  if (nNofAccels <= 0)
    return 0;

  pAccelTab = new ACCEL[nNofAccels];
  nNofAccels = ::CopyAcceleratorTable(hAccel, pAccelTab, nNofAccels);
  return nNofAccels;
}

LPACCEL CAcceleratorManager::GetResourceShortcut(UINT nCmdID, LPACCEL pAccelTab, int nNofAccels)
{
  for (int i = 0; i < nNofAccels; i++)
  {
    if (nCmdID == pAccelTab[i].cmd)
      return &pAccelTab[i];
  }

  return NULL;
}

CCmdAccelOb *CAcceleratorManager::GetAffected(LPACCEL pAccel)
{
  return GetAffected(pAccel->key, pAccel->fVirt & FCONTROL != 0, 
                     pAccel->fVirt & FALT != 0, pAccel->fVirt & FSHIFT != 0);
}


CCmdAccelOb *CAcceleratorManager::GetAffected(WORD wKey, bool bCtrl, bool bAlt, bool bShift)
{
	CCmdAccelOb* pCmdAccel;
	CAccelsOb* pAccel;
	UINT nCommandID;
	POSITION pos;

  for (POSITION posCmdAccel = m_mapAccelTable.GetStartPosition(); posCmdAccel; )
  {
		m_mapAccelTable.GetNextAssoc(posCmdAccel, nCommandID, pCmdAccel);

		pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) 
    {
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel->IsEqual(wKey, bCtrl, bAlt, bShift)) 
      {
        return pCmdAccel;
			}
		}
	}

  return NULL;
}

void CAcceleratorManager::SetAccelGroups(UINT *pGroups, UINT nCount)
{
  UINT *pGroup = pGroups;
  UINT  nGroupCount = 0;
  for (UINT i = 0; i < nCount; i++)
  {
    if (pGroups[i] == 0)
    {
      m_aAccelGroups.Add(pGroup);
      ASSERT(nGroupCount > 2);

      pGroup[1] = nGroupCount - 2;
      pGroup = &pGroups[i];
      pGroup++;
      nGroupCount = 0;
    }
    else
    {
      nGroupCount++;
    }
  }
}
