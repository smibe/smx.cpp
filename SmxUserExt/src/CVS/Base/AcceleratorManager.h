/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AcceleratorManager.h,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AcceleratorManager.h,v 1.2 2003/02/21 08:56:52 GeH Exp $
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
// File    : AcceleratorManager.h
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : interface for the CAcceleratorManager class.
//
////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ACCELERATORMANAGER_H__A6D76F4B_26C6_11D2_BE72_006097AC8D00__INCLUDED_)
#define AFX_ACCELERATORMANAGER_H__A6D76F4B_26C6_11D2_BE72_006097AC8D00__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include "CmdAccelOb.h"


// Helper map
#include <afxtempl.h>	// MFC Templates extension
#ifndef CMapStringToWord
typedef CMap< CString, LPCSTR, WORD, WORD& > CMapStringToWord;
#endif

#ifndef CMapWordToCCmdAccelOb
typedef CMap< UINT, UINT, CCmdAccelOb*, CCmdAccelOb*& > CMapWordToCCmdAccelOb;
#endif


//////////////////////////////////////////////////////////////////////
//
//
class CAcceleratorManager : public CObject
{
public:
	CAcceleratorManager();
	virtual ~CAcceleratorManager();

	// Helper class who will edit the datas...
	friend class CAccelDlgHelper;

// Operations
public:
	// Connection to the main application wnd
	void Connect(CFrameWnd* pWnd, bool bAutoSave = true);
  void Connect(CMultiDocTemplate* pDocTempl, CRuntimeClass *pFrameClass, bool bAutoSave);

	// In/Out with the registry
	bool Load(HKEY hRegKey, LPCTSTR szRegKey, LPCTSTR szRegValue);
	bool Load();
	bool Write();
	// Get the initials accels, not the user's
	bool Default(); 
	// Save a copy in the 2 maps called xxxSaved, which are used in case
	// of Default(), to reload the defaults accels.
	bool CreateDefaultTable(UINT nAccelTableID);

	// Registry access configuration
	bool GetRegKey(HKEY& hRegKey, CString& szRegKey);
	bool SetRegKey(HKEY hRegKey, LPCTSTR szRegKey);
	bool IsAutoSave() {return m_bAutoSave;}
	void SetAutoSave(bool bAutoSave) {m_bAutoSave = bAutoSave;}

	// Helper fct, used for new menus strings
	bool GetStringFromACCEL(ACCEL* pACCEL, CString& szAccel);
	bool GetStringFromACCEL(BYTE cVirt, WORD nCode, CString& szAccel);

	// Update the ACCELS table in the application, from the specified
	// datas in the manager.
	bool UpdateWndTable();

	// Modification helper fcts
	bool SetAccel(BYTE cVirt, UINT nCommandID, WORD wNewCaract, bool bLocked = false);
	bool AddCommandAccel(UINT nCommandID, bool bLocked = true);
	CCmdAccelOb *CreateEntry(UINT nCommandID);

	bool DeleteEntry(UINT nCommandID);
	bool DeleteAccel(BYTE cVirt, UINT nCommandID, WORD wNewCaract);

	// Affectation operator
	CAcceleratorManager& operator=(const CAcceleratorManager& accelmgr);

  const CMapWordToCCmdAccelOb& GetAccelTableMap() { return m_mapAccelTable; }
  bool  IsDefaultTableAvailable()                 { return m_nAccelTableID != NULL;  };
  void  SetAccelTableID(UINT nAccelTableID)       { m_nAccelTableID = nAccelTableID; }

  void SetUserCmdIDs(UINT nCmdIDFirst, UINT nCmdIDLast);

  bool Update(UINT nAccelTableID);

  void AddAccelGroup(UINT *pGroup);
  void InitAccelGroups();
  CCmdAccelOb *GetAffected(WORD wKey, bool bCtrl, bool bAlt, bool bShift);
  CCmdAccelOb *GetAffected(LPACCEL pAccel);

  int      CreateResourceAccelTable(LPACCEL& pAccelTab, UINT nAccelTableID);
  LPACCEL  GetResourceShortcut(UINT nCmdID, LPACCEL pAccelTab, int nNofAccels);
  void     SetAccelGroups(UINT *pGroups, UINT nCount);

public:
  //Group handling for display in the dialog
  CPtrArray m_aAccelGroups;
  CString   m_strView;

protected:
	// Erase all the datas
	void Reset(bool bRemoveUserCmd = true);
	// Internal affect fct.
	bool AddAccel(BYTE cVirt, UINT nCommandID, WORD wKey, bool bLocked);


// Attributes
protected:
  CFrameWnd         *m_pWndConnected;
  CMultiDocTemplate *m_pDocTemplate;
  CRuntimeClass     *m_pFrameClass;

  //User commands
  UINT m_nUserCmdIDFirst;
  UINT m_nUserCmdIDLast;

    ;

	// User datas
	CMapWordToCCmdAccelOb m_mapAccelTable;

	// Where the users datas will be saved in the registry
	HKEY    m_hRegKey;
	CString m_szRegKey;
  CString m_strRegValue;

	// if true, there is an auto-save in the registry, when the destructor is called
	bool m_bAutoSave;
  UINT m_nAccelTableID;
  bool m_bGroupsInitialized;

  UINT *m_pUserTools;
  UINT *m_pAll;
};


#endif // !defined(AFX_ACCELERATORMANAGER_H__A6D76F4B_26C6_11D2_BE72_006097AC8D00__INCLUDED_)
