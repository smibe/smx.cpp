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
// Remarks : the CAccelMapDlg class is used in the edit dlg and
//           property page. This class is friend of the CAcceleratorManager.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SMXRes.h"
#include "AcceleratorManager.h"
#include "AccelListBox.h"
#include "KeyboardEdit.h"
#include "AccelMapDlg.h"

#include "AcceleratorManager.h"
#include "CmdAccelOb.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Get the new user key and try to add it to the current list/command
//
void CAccelMapDlg::Assign()
{
	// Control if it's not already affected
	CCmdAccelOb* pCmdAccel;
	CAccelsOb* pAccel;
	UINT nCommandID;
	POSITION pos;
  POSITION PrevPos = NULL;

  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];
  CAcceleratorManager *pReadonlyAccelManager = &m_pReadonlyAccelManager[nIdx];

	WORD wKey;
	bool bCtrl, bAlt, bShift;
	if (!m_Key.GetAccelKey(wKey, bCtrl, bAlt, bShift))
		return; // no valid key, abort

	for (int index = 0; index < m_Commands.GetCount(); index++) 
  {
		nCommandID = LOWORD(m_Commands.GetItemData(index));
		VERIFY(pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE);

		pos = pCmdAccel->m_Accels.GetHeadPosition();
		while (pos != NULL) {
      PrevPos = pos;
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel->IsEqual(wKey, bCtrl, bAlt, bShift)) 
      {
	      int nIdx = m_Commands.GetCurSel();
	      if (nIdx == LB_ERR)
		      return;

	      // Get the object who manage the accels list, associated to the command.
	      nCommandID = LOWORD(m_Commands.GetItemData(nIdx));

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

  if (pReadonlyAccelManager)
  {
    for (POSITION posReadonly = pReadonlyAccelManager->m_mapAccelTable.GetStartPosition(); posReadonly; )
    {
		  pReadonlyAccelManager->m_mapAccelTable.GetNextAssoc(posReadonly, nCommandID, pCmdAccel);

		  pos = pCmdAccel->m_Accels.GetHeadPosition();
		  while (pos != NULL) {
			  pAccel = pCmdAccel->m_Accels.GetNext(pos);
			  if (pAccel->IsEqual(wKey, bCtrl, bAlt, bShift)) 
        {
				  // the key is already affected (in the same or other command) of the readonly access manager
				  CString str = pCmdAccel->GetCommandString();
          str += _T(" (readonly)");
          m_AlreadyAssigned.SetWindowText(str);
				  m_Key.SetSel(0, -1);
				  return; // abort
			  }
		  }
    }
  }
  

	// OK, we can add the accel key in the currently selected group
	index = m_Commands.GetCurSel();
	if (index == LB_ERR)
		return;

	// Get the object who manage the accels list, associated to the command.
	nCommandID = LOWORD(m_Commands.GetItemData(index));
	if (pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) != TRUE)
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
	index = m_Currents.AddString(szBuffer);
	m_Currents.SetItemDataPtr(index, (void*)pAccel);

	// Reset the key editor.
	m_Key.ResetKey();

}


//////////////////////////////////////////////////////////////////////
// Try to remove the selected key of the list of command's accels
//
void CAccelMapDlg::Remove()
{
  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];

  // Some controls
	int indexCurrent = m_Currents.GetCurSel();
	if (indexCurrent == LB_ERR)
		return;
	
	// 2nd part.
	int indexCmd = m_Commands.GetCurSel();
	if (indexCmd == LB_ERR)
		return;

	// Ref to the ID command
	UINT nCommandID = LOWORD(m_Commands.GetItemData(indexCmd));

	// Run through the accels,and control if it can be deleted.
	CCmdAccelOb* pCmdAccel;
	if (pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE) {
		CAccelsOb* pAccel;
		CAccelsOb* pAccelCurrent = (CAccelsOb*)(m_Currents.GetItemDataPtr(indexCurrent));
		CString szBuffer;
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		POSITION PrevPos;
		while (pos != NULL) 
    {
			PrevPos = pos;
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			if (pAccel == pAccelCurrent) {
				if (!pAccel->m_bLocked) {
					// not locked, so we delete the key
					pCmdAccel->m_Accels.RemoveAt(PrevPos);
					delete pAccel;
					// and update the listboxes/key editor/static text
					m_Currents.DeleteString(indexCurrent);
					m_Key.ResetKey();
					m_AlreadyAssigned.SetWindowText("");

					return;
				} 
        else 
        {
					AfxMessageBox("Unable to remove this\naccelerator (Locked)");
					return;
				}
			}
		}
		TRACE("internal error (CAccelMapDlg::Remove : pAccel unavailable)\n");
		return;
	}
	TRACE("internal error (CAccelMapDlg::Remove : Lookup failed)\n");

}


//////////////////////////////////////////////////////////////////////
// Simply call the CAcceleratorManager to erase all user accels.
//
void CAccelMapDlg::Reset()
{
  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];

  VERIFY(pAccelManager != NULL);

	pAccelManager->Default();
  if (pAccelManager->m_pAll && pAccelManager->m_aAccelGroups.GetSize() > 0)
  {
    pAccelManager->m_aAccelGroups.RemoveAt(pAccelManager->m_aAccelGroups.GetUpperBound());
    delete pAccelManager->m_pAll;
    pAccelManager->m_pAll = NULL;
  }

  if (pAccelManager->m_pUserTools && pAccelManager->m_aAccelGroups.GetSize() > 0)
  {
    pAccelManager->m_aAccelGroups.RemoveAt(pAccelManager->m_aAccelGroups.GetUpperBound());
    delete pAccelManager->m_pUserTools;
    pAccelManager->m_pUserTools = NULL;
  }
  pAccelManager->m_bGroupsInitialized = false;
  pAccelManager->InitAccelGroups();
	OnSelChangeCategory();	// update the listboxes.
}


//////////////////////////////////////////////////////////////////////
// Get the command selection and update the accels list.
//
void CAccelMapDlg::OnSelChangeCommands()
{
  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];

	// Check if some commands exist.
	int index = m_Commands.GetCurSel();
	if (index == LB_ERR)
		return;

	UINT nCommandID = LOWORD(m_Commands.GetItemData(index));
	m_Currents.ResetContent();

	CCmdAccelOb* pCmdAccel;
	if (pAccelManager->m_mapAccelTable.Lookup(nCommandID, pCmdAccel) == TRUE) 
  {
		CAccelsOb* pAccel;
		CString szBuffer;
		POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
		// Add the keys to the 'currents keys' listbox.
		while (pos != NULL) 
    {
			pAccel = pCmdAccel->m_Accels.GetNext(pos);
			pAccel->GetString(szBuffer);
			index = m_Currents.AddString(szBuffer);
			// and a pointer to the accel object.
			m_Currents.SetItemDataPtr(index, (void*)pAccel);
		}

    m_Description.SetWindowText(pCmdAccel->GetCommandInfoString());
	}
	// Init the key editor
	m_Key.ResetKey();
	GetDlgItem(IDC_ALREADY_ASSIGNED_LABEL)->ShowWindow(SW_HIDE);
  m_AlreadyAssigned.SetWindowText(_T(""));

  UpdateData(FALSE);
}

void CAccelMapDlg::UpdateAffected()
{
  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];
  CAcceleratorManager *pReadonlyAccelManager = &m_pReadonlyAccelManager[nIdx];

  if (!UpdateAffected(pAccelManager))
    UpdateAffected(pReadonlyAccelManager);
}


bool CAccelMapDlg::UpdateAffected(CAcceleratorManager *pAccelMgr)
{
	WORD wKey;
	bool bCtrl, bAlt, bShift;
  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];
  CAcceleratorManager *pReadonlyAccelManager = &m_pReadonlyAccelManager[nIdx];

  if (m_Key.GetAccelKey(wKey, bCtrl, bAlt, bShift))
  {
  	CCmdAccelOb* pCmdAccel = pAccelMgr->GetAffected(wKey, bCtrl, bAlt, bShift);

    if (pCmdAccel)
    {
      CString str;

			// the key is already affected (in the same or other command)
			GetDlgItem(IDC_ALREADY_ASSIGNED_LABEL)->ShowWindow(SW_SHOW);
    
      str = pCmdAccel->GetCommandString();
      if (pAccelMgr == pReadonlyAccelManager)
        str += _T(" (readonly)");

      m_AlreadyAssigned.SetWindowText(str);
			return true;
	  }
  }

	GetDlgItem(IDC_ALREADY_ASSIGNED_LABEL)->ShowWindow(SW_HIDE);
  m_AlreadyAssigned.SetWindowText(_T(""));
  return false;
}

void CAccelMapDlg::OnSelChangeCategory()
{
  int nIdx = m_View.GetCurSel();
  CAcceleratorManager *pAccelManager = &m_pAccelManager[nIdx];

	// Init the commands listbox
	m_Commands.ResetContent();

  // Erase the 'already affected' text
	m_AlreadyAssigned.SetWindowText("");

  UINT nItem = m_Category.GetCurSel();
  if (nItem == CB_ERR)
    return;

  UINT *pGroup = NULL;
  if (pAccelManager->m_aAccelGroups.GetSize() == NULL)
    pAccelManager->InitAccelGroups();

  UINT nGroups = pAccelManager->m_aAccelGroups.GetSize();
  if (nItem < nGroups)
  {
    pGroup = (UINT *) pAccelManager->m_aAccelGroups.GetAt(nItem);
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
		if (pAccelManager->m_mapAccelTable.Lookup(*pGroup++, pCmdAccelOb))
    {
		  nItem = m_Commands.AddString(pCmdAccelOb->GetCommandString());
		  m_Commands.SetItemData(nItem, MAKELONG(pCmdAccelOb->GetCommandID(),0));
    }
    else
    {
      ASSERT(false);
    }
	}

  // Update the currents accels associated with the selected command
	if (m_Commands.SetCurSel(0) != LB_ERR)
		OnSelChangeCommands();
   
}