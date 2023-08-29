/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CmdAccelOb.h,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/CmdAccelOb.h,v 1.1 2002/09/20 09:37:07 GeH Exp $
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
// File    : CmdAccelOb.h
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __CMDACCEL_OB_INCLUDE
#define __CMDACCEL_OB_INCLUDE

#include <afxtempl.h>	// MFC Templates extension


////////////////////////////////////////////////////////////////////////
//
//
typedef struct tagMAPVIRTKEYS {
	WORD wKey;
	TCHAR szKey[15];
} MAPVIRTKEYS, *PMAPVIRTKEYS;


////////////////////////////////////////////////////////////////////////
//
//
#define sizetable(table) (sizeof(table)/sizeof(table[0]))


////////////////////////////////////////////////////////////////////////
//
//
class CAccelsOb : public CObject
{
public:
	CAccelsOb();
	CAccelsOb(CAccelsOb* pFrom);
	CAccelsOb(BYTE cVirt, WORD wKey, bool bLocked = false);
	CAccelsOb(LPACCEL pACCEL);

public:
	CAccelsOb& operator=(const CAccelsOb& from);

	void GetString(CString& szBuffer);
	bool IsEqual(WORD wKey, bool bCtrl, bool bAlt, bool bShift);
	DWORD GetData();
	bool SetData(DWORD dwDatas);

public:
	BYTE m_cVirt;
	WORD m_wKey;
	bool m_bLocked;
};


//////////////////////////////////////////////////////////////////////
//
//
class CCmdAccelOb : public CObject
{
public:
	CCmdAccelOb();
	CCmdAccelOb(UINT nCommandID);
	CCmdAccelOb(BYTE cVirt, UINT nCommandID, WORD wKey, bool bLocked = false);
	~CCmdAccelOb();

public:
	void Add(CAccelsOb* pAccel);
	void Add(BYTE cVirt, WORD wKey, bool bLocked = false);
	void Reset();
	void DeleteUserAccels();

	CCmdAccelOb& operator=(const CCmdAccelOb& from);

  UINT    GetCommandID();
  LPCTSTR GetCommandString();
  LPCTSTR GetCommandInfoString();
  void    SetCommandString(LPCTSTR pszCommand)          { m_strCommand     = pszCommand; }
  void    SetCommandInfoString(LPCTSTR pszCommandInfo)  { m_strCommandInfo = pszCommandInfo; };

protected:
  void InitStrings();

public:
	CList< CAccelsOb*, CAccelsOb*& > m_Accels;

protected:
	UINT    m_nCommandID;
	CString m_strCommand;
	CString m_strCommandInfo;

};


////////////////////////////////////////////////////////////////////////
#endif // __CMDACCEL_OB_INCLUDE


