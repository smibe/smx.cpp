/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelDlgHelper.h,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelDlgHelper.h,v 1.2 2003/02/21 08:56:51 GeH Exp $
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
// File    : AccelDlgHelper.h
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : the CAccelDlgHelper class is used in the edit dlg and
//           property page. This class is friend of the CAcceleratorManager.
////////////////////////////////////////////////////////////////////////////////
#if !defined (AFX_ACCELDLGHELPER_H__14910615_3284_11D2_BE80_006097AC8D00__INCLUDED_)
#define AFX_ACCELDLGHELPER_H__14910615_3284_11D2_BE80_006097AC8D00__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#include "AcceleratorManager.h"
#include "AccelListBox.h"
#include "KeyboardEdit.h"


//////////////////////////////////////////////////////////////////////
class CAccelDlgHelper  
{
public:
	CAccelDlgHelper();
	virtual ~CAccelDlgHelper();

// Helper Datas
public:
	// The object which be edited
	CAcceleratorManager* m_pAccelManager;
  CAcceleratorManager* m_pReadonlyAccelManager;
	// Ref to the GUI objects in dlg or page
	CKeyboardEdit* m_pKey;
	CAccelListBox* m_pCurrents;
	CListBox* m_pCommands;
	CStatic* m_pAlreadyAffected;
	CStatic* m_pAlreadyAffectedLabel;
	CStatic* m_pDescription;
  CComboBox *m_pGroups;
	
// Operations
public:
	// Assignation fcts (init)
	bool SetAccelManager(CAcceleratorManager* pAccelManager);
	bool SetReadonlyAccelManager(CAcceleratorManager* pAccelManager);
	bool SetGUIObjects(CKeyboardEdit* pKey,
					CAccelListBox* pCurrents,
					CListBox* pCommands,
					CStatic* pAlreadyAffected,
					CStatic* pAlreadyAffectedLabel,
          CStatic* pDescription,
          CComboBox *pGroups);

	// Called by the Dlg or Page, at each user action. 
	void InitCommands();
	void Assign();
	void Remove();
	void Reset();
	void SelChangeCommands();
  void UpdateAffected();
  bool UpdateAffected(CAcceleratorManager *pAccelMgr);
  void SelChangeGroup();

};

#endif // !defined(AFX_ACCELDLGHELPER_H__14910615_3284_11D2_BE80_006097AC8D00__INCLUDED_)
