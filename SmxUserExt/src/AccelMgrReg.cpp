/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelMgrReg.cpp,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelMgrReg.cpp,v 1.3 2003/02/21 08:56:52 GeH Exp $
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
// File    : AccelMgrReg.cpp
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : implementation of the CAcceleratorManager class,
//           access to the registry.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Registry.h"

#include "AcceleratorManager.h"
#include "CmdAccelOb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// In/Out to the registry
//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::Load(HKEY hRegKey, LPCTSTR szRegKey, LPCTSTR szRegValue)
{
	ASSERT(szRegKey != NULL);

	m_hRegKey = hRegKey;
	m_szRegKey = szRegKey;
  m_strRegValue = szRegValue;

	CDWordArray AccelsDatasArray;
	CRegistry reg;
	reg.Open(m_hRegKey, m_szRegKey);
	if (! reg.VerifyValue  (m_strRegValue)) 
    return false;

  bool bRet = reg.Read(m_strRegValue, AccelsDatasArray);
	reg.Close();
	if (bRet == TRUE) 
  {
		CCmdAccelOb* pCmdAccel;
		CAccelsOb* pAccel;
		DWORD dwHeader, dwIDAccelData, dwAccelData;
		bool bExistID;
		int iIndex = 0;
		dwHeader = AccelsDatasArray.GetAt(iIndex++);

    if (LOWORD(dwHeader) == 65535) 
    {
			for (int i = 0; i < HIWORD(dwHeader); i++) 
      {
				dwIDAccelData = AccelsDatasArray.GetAt(iIndex++);
				WORD wIDCommand = LOWORD(dwIDAccelData);
				bExistID = m_mapAccelTable.Lookup(wIDCommand, pCmdAccel) != FALSE;
		
        if (bExistID)
					pCmdAccel->DeleteUserAccels();
        else
        {
		      pCmdAccel = new CCmdAccelOb(wIDCommand);
		      ASSERT(pCmdAccel != NULL);
		      m_mapAccelTable.SetAt(wIDCommand, pCmdAccel);
          bExistID = pCmdAccel != NULL;
        }
				
        for (int j = 0; j < HIWORD(dwIDAccelData); j++) 
        {
					dwAccelData = AccelsDatasArray.GetAt(iIndex++);
					if (pCmdAccel) 
          {
						pAccel = new CAccelsOb;
						ASSERT(pAccel != NULL);
						pAccel->SetData(dwAccelData);
						pCmdAccel->Add(pAccel);
					}
				}
			}
		}
		AccelsDatasArray.RemoveAll();
		UpdateWndTable();
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::Load()
{
	bool bRet = FALSE;
	if (!m_szRegKey.IsEmpty())
		bRet = Load(m_hRegKey, m_szRegKey, m_strRegValue);

	if (bRet == TRUE)
		return true;
	else
		return false;
}


//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::Write()
{
	CDWordArray AccelsDatasArray;
	CDWordArray CmdDatasArray;

	int iCount = 0;
	CCmdAccelOb* pCmdAccel;
	CAccelsOb* pAccel;
	DWORD dwAccelData;

	UINT nCmdID;
	POSITION pos = m_mapAccelTable.GetStartPosition();
	while (pos != NULL) 
  {
		m_mapAccelTable.GetNextAssoc(pos, nCmdID, pCmdAccel);

		CmdDatasArray.RemoveAll();
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) 
    {
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (!pAccel->m_bLocked) 
      {
				dwAccelData = pAccel->GetData();
				CmdDatasArray.Add(dwAccelData);
			}
		}
		if (pCmdAccel->GetCommandID() != 0) 
    {
			CmdDatasArray.InsertAt(0, MAKELONG((WORD) pCmdAccel->GetCommandID(), CmdDatasArray.GetSize()));
			
			AccelsDatasArray.Append(CmdDatasArray);
			iCount++;
		}
	}
	AccelsDatasArray.InsertAt(0, MAKELONG(65535, iCount));

	CRegistry reg;
	reg.Open(m_hRegKey, m_szRegKey);
	bool bRet = reg.Write(m_strRegValue, AccelsDatasArray);
	reg.Close();

	AccelsDatasArray.RemoveAll();
	if (bRet)
		return true;
	else
		return false;
}


//////////////////////////////////////////////////////////////////////
// Defaults values management.
//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::CreateDefaultTable(UINT nAccelTableID)
{
	UINT nCmdID = 0;
  HACCEL hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(nAccelTableID));
  if (!hAccel)
    return false;

  //Copy the accelerators
  int nNofAccels = ::CopyAcceleratorTable(hAccel, NULL, 0);
  if (nNofAccels <= 0)
    return false;

  LPACCEL pAccelTab = new ACCEL[nNofAccels];
  nNofAccels = ::CopyAcceleratorTable(hAccel, pAccelTab, nNofAccels);

  Reset(false);

  for (int i = 0; i < nNofAccels; i++)
  {
    AddAccel(pAccelTab[i].fVirt, pAccelTab[i].cmd, pAccelTab[i].key, false);
  }

  delete [] pAccelTab;

  m_nAccelTableID = nAccelTableID;
  return true;
}

// Check if there are new accelerators in the accelerator table 
// and add them.
bool CAcceleratorManager::Update(UINT nAccelTableID)
{
	UINT nCmdID = 0;
  HACCEL hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(nAccelTableID));
  if (!hAccel)
    return false;
  
  //Copy the accelerators
  int nNofAccels = ::CopyAcceleratorTable(hAccel, NULL, 0);
  if (nNofAccels <= 0)
    return false;

  LPACCEL pAccelTab = new ACCEL[nNofAccels];
  nNofAccels = ::CopyAcceleratorTable(hAccel, pAccelTab, nNofAccels);

  Reset(false);

	CCmdAccelOb* pCmdAccel = NULL;
  for (int i = 0; i < nNofAccels; i++)
  {
  	if (m_mapAccelTable.Lookup(pAccelTab[i].cmd, pCmdAccel) == FALSE && pCmdAccel) 
    {
      AddAccel(pAccelTab[i].fVirt, pAccelTab[i].cmd, pAccelTab[i].key, false);
    }
  }

  delete [] pAccelTab;

  m_nAccelTableID = nAccelTableID;
  return true;
}

//////////////////////////////////////////////////////////////////////
//
//
bool CAcceleratorManager::Default()
{
	return CreateDefaultTable(m_nAccelTableID);
}



