/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelDlgHelper.cpp,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelDlgHelper.cpp,v 1.3 2003/02/21 08:56:51 GeH Exp $
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
// File    : AccelDlgHelper.cpp
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : the CAccelDlgHelper class is used in the edit dlg and
//           property page. This class is friend of the CAcceleratorManager.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SMXRes.h"
#include "AccelDlgHelper.h"


#include "AcceleratorManager.h"
#include "CmdAccelOb.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
//
CAccelDlgHelper::CAccelDlgHelper()
{
	m_pAccelManager = NULL;

	m_pKey = NULL;
	m_pCurrents = NULL;
	m_pCommands = NULL;
	m_pAlreadyAffected = NULL;
	m_pAlreadyAffectedLabel = NULL;
	m_pDescription     = NULL;
  m_pGroups = NULL;
}


//////////////////////////////////////////////////////////////////////
//
//
CAccelDlgHelper::~CAccelDlgHelper()
{
}



//////////////////////////////////////////////////////////////////////
// Assign fcts
//////////////////////////////////////////////////////////////////////
//
//
bool CAccelDlgHelper::SetAccelManager(CAcceleratorManager* pAccelManager)
{
	VERIFY(pAccelManager != NULL);
	m_pAccelManager = pAccelManager;

	return TRUE;
}

bool CAccelDlgHelper::SetReadonlyAccelManager(CAcceleratorManager* pAccelManager)
{
	VERIFY(pAccelManager != NULL);
	m_pReadonlyAccelManager = pAccelManager;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//
bool CAccelDlgHelper::SetGUIObjects(CKeyboardEdit* pKey,
									CAccelListBox* pCurrents,
									CListBox* pCommands,
									CStatic* pAlreadyAffected,
									CStatic* pAlreadyAffectedLabel,
                  CStatic* pDescription,
                  CComboBox *pGroups)
{
	VERIFY(pKey != NULL);
	VERIFY(pCurrents != NULL);
	VERIFY(pCommands != NULL);
	VERIFY(pAlreadyAffected != NULL);
	
	m_pKey = pKey;
	m_pCurrents = pCurrents;
	m_pCommands = pCommands;
	m_pAlreadyAffected = pAlreadyAffected;
	m_pAlreadyAffectedLabel = pAlreadyAffectedLabel;
  m_pDescription = pDescription;
  m_pGroups = pGroups;

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Fill the command listbox, and update the accels associated to the
// first command.
//
void CAccelDlgHelper::InitCommands()
{
  SelChangeGroup();
}


//////////////////////////////////////////////////////////////////////
// Get the new user key and try to add it to the current list/command
//
void CAccelDlgHelper::Assign()
{
	VERIFY(m_pKey != NULL);
	VERIFY(m_pCurrents != NULL);
	VERIFY(m_pCommands != NULL);
	VERIFY(m_pAlreadyAffected != NULL);

	// Control if it's not already affected
	CCmdAccelOb* pCmdAccel;
	CAccelsOb* pAccel;
	UINT nCommandID;
	POSITION pos;
  POSITION PrevPos = NULL;

	WORD wKey;
	bool bCtrl, bAlt, bShift;
	if (!m_pKey->GetAccelKey(wKey, bCtrl, bAlt, bShift))
		return; // no valid key, abort

	for (int index = 0; index < m_pCommands->GetCount(); index++) 
  {
		nCommandID = LOWORD(m_pCommands->GetItemData(index));
		VERIFY(m_pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE);

		pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) {
      PrevPos = pos;
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel->IsEqual(wKey, bCtrl, bAlt, bShift)) 
      {
	      int nIdx = m_pCommands->GetCurSel();
	      if (nIdx == LB_ERR)
		      return;

	      // Get the object who manage the accels list, associated to the command.
	      nCommandID = LOWORD(m_pCommands->GetItemData(nIdx));

				// the key is already affected (in the same or other command)
        if (nCommandID != pCmdAccel->GetCommandID())
        {
          //remove it
				  if (!pAccel->m_bLocked) {
					  // not locked, so we delete the key
					  pCmdAccel->m_Accels.RemoveAt(PrevPos);
					  delete pAccel;
				  } 
          else 
          {
					  return; //Abort
				  }
        }
			}
		}
	}

  if (m_pReadonlyAccelManager)
  {
    for (POSITION posReadonly = m_pReadonlyAccelManager->m_mapAccelTable.GetStartPosition(); posReadonly; )
    {
		  m_pReadonlyAccelManager->m_mapAccelTable.GetNextAssoc(posReadonly, nCommandID, pCmdAccel);

		  pos = pCmdAccel->m_Accels.GetHeadPosition();
		  while (pos != NULL) {
			  pAccel = pCmdAccel->m_Accels.GetNext(pos);
			  if (pAccel->IsEqual(wKey, bCtrl, bAlt, bShift)) 
        {
				  // the key is already affected (in the same or other command) of the readonly access manager
				  CString str = pCmdAccel->GetCommandString();
          str += _T(" (readonly)");
          m_pAlreadyAffected->SetWindowText(str);
				  m_pKey->SetSel(0, -1);
				  return; // abort
			  }
		  }
    }
  }
  

	// OK, we can add the accel key in the currently selected group
	index = m_pCommands->GetCurSel();
	if (index == LB_ERR)
		return;

	// Get the object who manage the accels list, associated to the command.
	nCommandID = LOWORD(m_pCommands->GetItemData(index));
	if (m_pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) != TRUE)
		return;
	
	BYTE cVirt = 0;
	if (bCtrl)
		cVirt |= FCONTROL;
	if (bAlt)
		cVirt |= FALT;
	if (bShift)
		cVirt |= FSHIFT;

	cVirt |= FVIRTKEY;

	// Create the new key...
	pAccel = new CAccelsOb(cVirt, wKey, false);
	ASSERT(pAccel != NULL);
	// ...and add in the list.
	pCmdAccel->m_Accels.AddTail(pAccel);

	// Update the listbox.
	CString szBuffer;
	pAccel->GetString(szBuffer);
	index = m_pCurrents->AddString(szBuffer);
	m_pCurrents->SetItemDataPtr(index, (void*)pAccel);

	// Reset the key editor.
	m_pKey->ResetKey();

}


//////////////////////////////////////////////////////////////////////
// Try to remove the selected key of the list of command's accels
//
void CAccelDlgHelper::Remove()
{
	VERIFY(m_pKey != NULL);
	VERIFY(m_pCurrents != NULL);
	VERIFY(m_pCommands != NULL);

	// Some controls
	int indexCurrent = m_pCurrents->GetCurSel();
	if (indexCurrent == LB_ERR)
		return;
	
	// 2nd part.
	int indexCmd = m_pCommands->GetCurSel();
	if (indexCmd == LB_ERR)
		return;

	// Ref to the ID command
	UINT nCommandID = LOWORD(m_pCommands->GetItemData(indexCmd));

	// Run through the accels,and control if it can be deleted.
	CCmdAccelOb* pCmdAccel;
	if (m_pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE) {
		CAccelsOb* pAccel;
		CAccelsOb* pAccelCurrent = (CAccelsOb*)(m_pCurrents->GetItemDataPtr(indexCurrent));
		CString szBuffer;
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		POSITION PrevPos;
		while (pos != NULL) {
			PrevPos = pos;
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel == pAccelCurrent) {
				if (!pAccel->m_bLocked) {
					// not locked, so we delete the key
					pCmdAccel->m_Accels.RemoveAt(PrevPos);
					delete pAccel;
					// and update the listboxes/key editor/static text
					m_pCurrents->DeleteString(indexCurrent);
					m_pKey->ResetKey();
					m_pAlreadyAffected->SetWindowText("");

					return;
				} else {
					AfxMessageBox("Unable to remove this\naccelerator (Locked)");
					return;
				}
			}
		}
		AfxMessageBox("internal error (CAccelDlgHelper::Remove : pAccel unavailable)");
		return;
	}
	AfxMessageBox("internal error (CAccelDlgHelper::Remove : Lookup failed)");

}


//////////////////////////////////////////////////////////////////////
// Simply call the CAcceleratorManager to erase all user accels.
//
void CAccelDlgHelper::Reset()
{
	VERIFY(m_pAccelManager != NULL);

	m_pAccelManager->Default();
  if (m_pAccelManager->m_pAll && m_pAccelManager->m_aAccelGroups.GetSize() > 1)
  {
    m_pAccelManager->m_aAccelGroups.RemoveAt(m_pAccelManager->m_aAccelGroups.GetUpperBound());
    delete m_pAccelManager->m_pAll;
    m_pAccelManager->m_pAll = NULL;
  }

  if (m_pAccelManager->m_pUserTools && m_pAccelManager->m_aAccelGroups.GetSize() > 1)
  {
    m_pAccelManager->m_aAccelGroups.RemoveAt(m_pAccelManager->m_aAccelGroups.GetUpperBound());
    delete m_pAccelManager->m_pUserTools;
    m_pAccelManager->m_pUserTools = NULL;
  }
  m_pAccelManager->m_bGroupsInitialized = false;
  m_pAccelManager->InitAccelGroups();
	InitCommands();	// update the listboxes.
}


//////////////////////////////////////////////////////////////////////
// Get the command selection and update the accels list.
//
void CAccelDlgHelper::SelChangeCommands()
{
	VERIFY(m_pKey != NULL);
	VERIFY(m_pCurrents != NULL);
	VERIFY(m_pCommands != NULL);

	// Check if some commands exist.
	int index = m_pCommands->GetCurSel();
	if (index == LB_ERR)
		return;

	UINT nCommandID = LOWORD(m_pCommands->GetItemData(index));
	m_pCurrents->ResetContent();

	CCmdAccelOb* pCmdAccel;
	if (m_pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE) 
  {
		CAccelsOb* pAccel;
		CString szBuffer;
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		// Add the keys to the 'currents keys' listbox.
		while (pos != NULL) 
    {
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			pAccel->GetString(szBuffer);
			index = m_pCurrents->AddString(szBuffer);
			// and a pointer to the accel object.
			m_pCurrents->SetItemDataPtr(index, (void*)pAccel);
		}

    m_pDescription->SetWindowText(pCmdAccel->GetCommandInfoString());
	}
	// Init the key editor
	m_pKey->ResetKey();
	m_pAlreadyAffectedLabel->ShowWindow(SW_HIDE);
  m_pAlreadyAffected->SetWindowText(_T(""));
}

void CAccelDlgHelper::UpdateAffected()
{
  if (!UpdateAffected(m_pAccelManager))
    UpdateAffected(m_pReadonlyAccelManager);
}


bool CAccelDlgHelper::UpdateAffected(CAcceleratorManager *pAccelMgr)
{
	WORD wKey;
	bool bCtrl, bAlt, bShift;
	if (m_pKey->GetAccelKey(wKey, bCtrl, bAlt, bShift))
  {
  	CCmdAccelOb* pCmdAccel = pAccelMgr->GetAffected(wKey, bCtrl, bAlt, bShift);

    if (pCmdAccel)
    {
      CString str;

			// the key is already affected (in the same or other command)
			m_pAlreadyAffectedLabel->ShowWindow(SW_SHOW);
    
      str = pCmdAccel->GetCommandString();
      if (pAccelMgr == m_pReadonlyAccelManager)
        str += _T(" (readonly)");

      m_pAlreadyAffected->SetWindowText(str);
			return true;
	  }
  }

	m_pAlreadyAffectedLabel->ShowWindow(SW_HIDE);
  m_pAlreadyAffected->SetWindowText(_T(""));
  return false;
}

void CAccelDlgHelper::SelChangeGroup()
{
 	VERIFY(m_pCommands != NULL);
	VERIFY(m_pAccelManager != NULL);
	VERIFY(m_pAlreadyAffected != NULL);

	// Init the commands listbox
	m_pCommands->ResetContent();

  // Erase the 'already affected' text
	m_pAlreadyAffected->SetWindowText("");

  UINT nItem = m_pGroups->GetCurSel();
  if (nItem == CB_ERR)
    return;

  UINT *pGroup = NULL;
  if (m_pAccelManager->m_aAccelGroups.GetSize() == NULL)
    m_pAccelManager->InitAccelGroups();

  UINT nGroups = m_pAccelManager->m_aAccelGroups.GetSize();
  if (nItem < nGroups)
  {
    pGroup = (UINT *) m_pAccelManager->m_aAccelGroups.GetAt(nItem);
  }
  else
  {
    ASSERT(false);
  }

  //Skip the string ID
  pGroup++;
  int nCount = *pGroup;
  pGroup++;


  CCmdAccelOb *pCmdAccelOb = NULL;
	UINT         nCommandID  = 0;

  for (int i = 0; i < nCount; i++)
  {
		if (m_pAccelManager->m_mapAccelTable.Lookup(*pGroup++, pCmdAccelOb))
    {
		  nItem = m_pCommands->AddString(pCmdAccelOb->GetCommandString());
		  m_pCommands->SetItemData(nItem, MAKELONG(pCmdAccelOb->GetCommandID(),0));
    }
    else
    {
      ASSERT(false);
    }
	}

  // Update the currents accels associated with the selected command
	if (m_pCommands->SetCurSel(0) != LB_ERR)
		SelChangeCommands();
   
}