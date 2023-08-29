/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: Registry.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/Registry.h,v 1.1 2002/09/20 09:37:07 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Shane Martin
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
//    shane.kim@kaiserslautern.netsurf.de
/////////////////////////////////////////////////////////////////////////////
#include <winreg.h>

#define REG_RECT	0x0001
#define REG_POINT	0x0002

class CRegistry : public CObject
{
// Construction
public:
	CRegistry(HKEY hKeyRoot = HKEY_LOCAL_MACHINE);
	virtual ~CRegistry();

	struct REGINFO
	{
		LONG lMessage;
		DWORD dwType;
		DWORD dwSize;
	} m_Info;
// Operations
public:
	bool VerifyKey (HKEY hKeyRoot, LPCTSTR pszPath);
	bool VerifyKey (LPCTSTR pszPath);
	bool VerifyValue (LPCTSTR pszValue);
	bool CreateKey (HKEY hKeyRoot, LPCTSTR pszPath);
	bool Open (HKEY hKeyRoot, LPCTSTR pszPath);
	void Close();

	bool DeleteValue (LPCTSTR pszValue);
	bool DeleteValueKey (HKEY hKeyRoot, LPCTSTR pszPath);

	bool Write (LPCTSTR pszKey, int iVal);
	bool Write (LPCTSTR pszKey, DWORD dwVal);
	bool Write (LPCTSTR pszKey, LPCTSTR pszVal);
	bool Write (LPCTSTR pszKey, CStringList& scStringList);
	bool Write (LPCTSTR pszKey, CByteArray& bcArray);
	bool Write (LPCTSTR pszKey, CStringArray& scArray);
	bool Write (LPCTSTR pszKey, CDWordArray& dwcArray);
	bool Write (LPCTSTR pszKey, CWordArray& wcArray);
	bool Write (LPCTSTR pszKey, LPCRECT rcRect);
	bool Write (LPCTSTR pszKey, LPPOINT& lpPoint);

	bool Read (LPCTSTR pszKey, int& iVal);
	bool Read (LPCTSTR pszKey, DWORD& dwVal);
	bool Read (LPCTSTR pszKey, CString& sVal);
	bool Read (LPCTSTR pszKey, CStringList& scStringList);
	bool Read (LPCTSTR pszKey, CStringArray& scArray);
	bool Read (LPCTSTR pszKey, CDWordArray& dwcArray);
	bool Read (LPCTSTR pszKey, CWordArray& wcArray);
	bool Read (LPCTSTR pszKey, CByteArray& bcArray);
	bool Read (LPCTSTR pszKey, LPPOINT& lpPoint);
	bool Read (LPCTSTR pszKey, LPRECT& rcRect);


  bool CopyKey(LPCTSTR pszFrom, LPCTSTR pszTo, LPCTSTR pszSubKey);
  LONG RegCopyKey(HKEY SrcKey, HKEY TrgKey, LPCTSTR TrgSubKeyName);
  bool RegRemoveKey(HKEY hKey, LPCTSTR pszSubKey);

  void SetKeyHandle(HKEY hKey) { ASSERT(m_hKey == NULL); m_hKey = hKey; }
protected:	
	HKEY 	m_hKey;
	CString m_sPath;
};
