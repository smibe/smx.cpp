/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: Registry.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/Registry.cpp,v 1.2 2002/11/22 17:24:40 GeH Exp $
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

// last revised: 24 Apr 98
// Registry.cpp : implementation file
//
// Description:
// CRegistry is a wrapper for the Windows Registry API.  It allows
//  easy modification of the Registry with easy to remember terms like
//  Read, Write, Open, and Close.

#include "stdafx.h"
#include "Registry.h"

CRegistry::CRegistry(HKEY hKeyRoot)
{
	m_hKey = hKeyRoot;
}

CRegistry::~CRegistry()
{
	Close();
}


bool CRegistry::VerifyKey (HKEY hKeyRoot, LPCTSTR pszPath)
{
	ASSERT (hKeyRoot);
	ASSERT (pszPath);

	LONG ReturnValue = RegOpenKeyEx (hKeyRoot, pszPath, 0L,
		KEY_ALL_ACCESS, &m_hKey);
	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	return FALSE;
}

bool CRegistry::VerifyKey (LPCTSTR pszPath)
{
	ASSERT (m_hKey);

	LONG ReturnValue = RegOpenKeyEx (m_hKey, pszPath, 0L,
		KEY_ALL_ACCESS, &m_hKey);
	
	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::VerifyValue (LPCTSTR pszValue)
{
	ASSERT(m_hKey);
	LONG lReturn = RegQueryValueEx(m_hKey, pszValue, NULL,
		NULL, NULL, NULL);

	m_Info.lMessage = lReturn;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::CreateKey (HKEY hKeyRoot, LPCTSTR pszPath)
{
	DWORD dw;

	LONG ReturnValue = RegCreateKeyEx (hKeyRoot, pszPath, 0L, NULL,
		REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, 
		&m_hKey, &dw);

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Open (HKEY hKeyRoot, LPCTSTR pszPath)
{
	m_sPath = pszPath;

	LONG ReturnValue = RegOpenKeyEx (hKeyRoot, pszPath, 0L,
		KEY_ALL_ACCESS, &m_hKey);

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = 0L;
	m_Info.dwType = 0L;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

void CRegistry::Close()
{
	if (m_hKey)
	{
		RegCloseKey (m_hKey);
		m_hKey = NULL;
	}
}

bool CRegistry::Write (LPCTSTR pszKey, int iVal)
{
	DWORD dwValue;

	ASSERT(m_hKey);
	ASSERT(pszKey);
	
	dwValue = (DWORD)iVal;
	LONG ReturnValue = RegSetValueEx (m_hKey, pszKey, 0L, REG_DWORD,
		(CONST BYTE*) &dwValue, sizeof(DWORD));

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = sizeof(DWORD);
	m_Info.dwType = REG_DWORD;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, DWORD dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	return RegSetValueEx (m_hKey, pszKey, 0L, REG_DWORD,
		(CONST BYTE*) &dwVal, sizeof(DWORD)) != FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, LPCTSTR pszData)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	ASSERT(pszData);
	ASSERT(AfxIsValidAddress(pszData, strlen(pszData), FALSE));

	LONG ReturnValue = RegSetValueEx (m_hKey, pszKey, 0L, REG_SZ,
		(CONST BYTE*) pszData, strlen(pszData) + 1);

	m_Info.lMessage = ReturnValue;
	m_Info.dwSize = strlen(pszData) + 1;
	m_Info.dwType = REG_SZ;

	if(ReturnValue == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, CStringList& scStringList)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(scStringList.IsSerializable());
	scStringList.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, CByteArray& bcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(bcArray.IsSerializable());
	bcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, CDWordArray& dwcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwData = iMaxChars;
	BYTE* byData = new BYTE[iMaxChars * sizeof(TCHAR)];
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(dwcArray.IsSerializable());
	dwcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

  if(byData)
	{
		delete [] byData;
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, CWordArray& wcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(wcArray.IsSerializable());
	wcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write (LPCTSTR pszKey, CStringArray& scArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(scArray.IsSerializable());
	scArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_BINARY;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write(LPCTSTR pszKey, LPCRECT rcRect)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 30;
	CDWordArray dwcArray;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	dwcArray.SetSize(5);
	dwcArray.SetAt(0, rcRect->top);
	dwcArray.SetAt(1, rcRect->bottom);
	dwcArray.SetAt(2, rcRect->left);
	dwcArray.SetAt(3, rcRect->right);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(dwcArray.IsSerializable());
	dwcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_RECT;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Write(LPCTSTR pszKey, LPPOINT& lpPoint)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 20;
	CDWordArray dwcArray;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	dwcArray.SetSize(5);
	dwcArray.SetAt(0, lpPoint->x);
	dwcArray.SetAt(1, lpPoint->y);

	CMemFile file(byData, iMaxChars, 16);
	CArchive ar(&file, CArchive::store);
	ASSERT(dwcArray.IsSerializable());
	dwcArray.Serialize(ar);
	ar.Close();
	const DWORD dwLen = file.GetLength();
	ASSERT(dwLen < iMaxChars);
	LONG lReturn = RegSetValueEx(m_hKey, pszKey, 0, REG_BINARY,
		file.Detach(), dwLen);
	
	m_Info.lMessage = lReturn;
	m_Info.dwSize = dwLen;
	m_Info.dwType = REG_POINT;

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;
	
	return FALSE;
}

bool CRegistry::Read(LPCTSTR pszKey, int& iVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lReturn = RegQueryValueEx (m_hKey, (LPSTR) pszKey, NULL,
		&dwType, (BYTE *) &dwDest, &dwSize);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwSize;

	if(lReturn == ERROR_SUCCESS)
	{
		iVal = (int)dwDest;
		return TRUE;
	}

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, DWORD& dwVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;

	LONG lReturn = RegQueryValueEx (m_hKey, (LPSTR) pszKey, NULL, 
		&dwType, (BYTE *) &dwDest, &dwSize);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwSize;

	if(lReturn == ERROR_SUCCESS)
	{
		dwVal = dwDest;
		return TRUE;
	}

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, CString& sVal)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);

	DWORD dwType;
	DWORD dwSize = 200;
	char  szString[255];

	LONG lReturn = RegQueryValueEx (m_hKey, (LPSTR) pszKey, NULL,
		&dwType, (BYTE *) szString, &dwSize);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwSize;

	if(lReturn == ERROR_SUCCESS)
	{
		sVal = szString;
		return TRUE;
	}

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, CStringList& scStringList)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(scStringList.IsSerializable());
		scStringList.RemoveAll();
		scStringList.Serialize(ar);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, CByteArray& bcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	int OldSize = bcArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(bcArray.IsSerializable());
		bcArray.RemoveAll();
		bcArray.SetSize(10);
		bcArray.Serialize(ar);
		bcArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, CDWordArray& dwcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	int OldSize = dwcArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(dwcArray.IsSerializable());
		dwcArray.RemoveAll();
// Modified by T.Maurel, to avoid unexpectables errors
//		dwcArray.SetSize(10);
		dwcArray.Serialize(ar);
// Same as up...
//		dwcArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, CWordArray& wcArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	int OldSize = wcArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(wcArray.IsSerializable());
		wcArray.RemoveAll();
		wcArray.SetSize(10);
		wcArray.Serialize(ar);
		wcArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Read (LPCTSTR pszKey, CStringArray& scArray)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 4096;
	int OldSize = scArray.GetSize();
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	m_Info.lMessage = lReturn;
	m_Info.dwType = dwType;
	m_Info.dwSize = dwData;

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(scArray.IsSerializable());
		scArray.RemoveAll();
		scArray.SetSize(10);
		scArray.Serialize(ar);
		scArray.SetSize(OldSize);
		ar.Close();
		file.Close();
	}

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Read(LPCTSTR pszKey, LPRECT& rcRect)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 30;
	CDWordArray dwcArray;
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(dwcArray.IsSerializable());
		dwcArray.RemoveAll();
		dwcArray.SetSize(5);
		dwcArray.Serialize(ar);
		ar.Close();
		file.Close();
		rcRect->top = dwcArray.GetAt(0);
		rcRect->bottom = dwcArray.GetAt(1);
		rcRect->left = dwcArray.GetAt(2);
		rcRect->right = dwcArray.GetAt(3);
	}

	m_Info.lMessage = lReturn;
	m_Info.dwType = REG_RECT;
	m_Info.dwSize = sizeof(RECT);

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::Read(LPCTSTR pszKey, LPPOINT& lpPoint)
{
	ASSERT(m_hKey);
	ASSERT(pszKey);
	const int iMaxChars = 20;
	CDWordArray dwcArray;
	DWORD dwType;
	DWORD dwData = iMaxChars;
	BYTE* byData = (BYTE*)::calloc(iMaxChars, sizeof(TCHAR));
	ASSERT(byData);

	LONG lReturn = RegQueryValueEx(m_hKey, pszKey, NULL, &dwType,
		byData, &dwData);

	if(lReturn == ERROR_SUCCESS && dwType == REG_BINARY)
	{
		ASSERT(dwData < iMaxChars);
		CMemFile file(byData, dwData);
		CArchive ar(&file, CArchive::load);
		ar.m_bForceFlat = FALSE;
		ASSERT(ar.IsLoading());
		ASSERT(dwcArray.IsSerializable());
		dwcArray.RemoveAll();
		dwcArray.SetSize(5);
		dwcArray.Serialize(ar);
		ar.Close();
		file.Close();
		lpPoint->x = dwcArray.GetAt(0);
		lpPoint->y = dwcArray.GetAt(1);
	}

	m_Info.lMessage = lReturn;
	m_Info.dwType = REG_POINT;
	m_Info.dwSize = sizeof(POINT);

	if(byData)
	{
		free(byData);
		byData = NULL;
	}

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::DeleteValue (LPCTSTR pszValue)
{
	ASSERT(m_hKey);
	LONG lReturn = RegDeleteValue(m_hKey, pszValue);


	m_Info.lMessage = lReturn;
	m_Info.dwType = 0L;
	m_Info.dwSize = 0L;

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::DeleteValueKey (HKEY hKeyRoot, LPCTSTR pszPath)
{
	ASSERT(pszPath);
	ASSERT(hKeyRoot);

	LONG lReturn = RegDeleteKey(hKeyRoot, pszPath);

	m_Info.lMessage = lReturn;
	m_Info.dwType = 0L;
	m_Info.dwSize = 0L;

	if(lReturn == ERROR_SUCCESS)
		return TRUE;

	return FALSE;
}

bool CRegistry::CopyKey(LPCTSTR pszFrom, LPCTSTR pszTo, LPCTSTR pszSubKey)
{
	ASSERT (m_hKey);
	ASSERT (pszFrom);
	ASSERT (pszTo);

  HKEY hFrom = NULL;
  HKEY hTo = NULL;

	LONG ReturnValue = RegOpenKeyEx (m_hKey, pszFrom, 0L,
		KEY_ALL_ACCESS, &hFrom);
	if(ReturnValue != ERROR_SUCCESS)
  {
	  m_Info.lMessage = ReturnValue;
	  m_Info.dwSize = 0L;
	  m_Info.dwType = 0L;
    return FALSE;
  }

	ReturnValue = RegCreateKey(m_hKey, pszTo, &hTo);
	if(ReturnValue != ERROR_SUCCESS)
  {
	  m_Info.lMessage = ReturnValue;
	  m_Info.dwSize = 0L;
	  m_Info.dwType = 0L;
    RegCloseKey(hFrom);
    return FALSE;
  }

  bool bResult = RegCopyKey(hFrom, hTo, pszSubKey) == ERROR_SUCCESS;
  
  RegCloseKey(hFrom); 
  RegCloseKey(hTo); 

  return bResult;
}

//----------------------------------------------------------------------------------
//  free buffer
//----------------------------------------------------------------------------------
__forceinline void FreeBuff(unsigned char** Buff)
{
	if (*Buff) {
		free(*Buff);
		*Buff = NULL;
	}
}

//----------------------------------------------------------------------------------
//  allocate buffer
//----------------------------------------------------------------------------------
__forceinline void AllocBuff(unsigned char** Buff, DWORD BuffSize)
{
	FreeBuff(Buff);
	*Buff = (unsigned char*)malloc(BuffSize);
}

    //----------------------------------------------------------------------------------
//  copy key to new position
//----------------------------------------------------------------------------------
LONG CRegistry::RegCopyKey(HKEY SrcKey, HKEY TrgKey, LPCTSTR TrgSubKeyName)
{

	HKEY	SrcSubKey;
	HKEY	TrgSubKey;
	int	ValEnumIndx=0;
	int	KeyEnumIndx=0;
	char	ValName[MAX_PATH+1];
	char	KeyName[MAX_PATH+1];
	DWORD	size;	
	DWORD	VarType;
	DWORD	BuffSize;
	unsigned char*	Buff=NULL;
	LONG	Err;
	DWORD	KeyDisposition;
	FILETIME LastWriteTime; 

	// create target key
	if (RegCreateKeyEx(TrgKey,TrgSubKeyName,NULL,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&TrgSubKey,&KeyDisposition) != ERROR_SUCCESS)
		return GetLastError();

	do {
		do {
			// read value from source key
			Err = ERROR_NOT_ENOUGH_MEMORY;
			BuffSize = 1024;
			do {						 
				AllocBuff(&Buff,BuffSize);
				size=MAX_PATH+1;
				Err = RegEnumValue(SrcKey,ValEnumIndx,ValName,&size,NULL,&VarType,Buff,&BuffSize);
				if ((Err != ERROR_SUCCESS) && (Err != ERROR_NO_MORE_ITEMS))
					Err = GetLastError();
			} while (Err == ERROR_NOT_ENOUGH_MEMORY);

			// done copying this key
			if (Err == ERROR_NO_MORE_ITEMS)
				break;

			// unknown error return
			if (Err != ERROR_SUCCESS)
				goto quit_err;

			// write value to target key
			if (RegSetValueEx(TrgSubKey,ValName,NULL,VarType,Buff,BuffSize) != ERROR_SUCCESS)
				goto quit_get_err;

			// read next value
			ValEnumIndx++;
		} while (true);

		// free buffer
		FreeBuff(&Buff);

		// if copying under the same key avoid endless recursions
		do {
			// enum sub keys
			size=MAX_PATH+1;
			Err = RegEnumKeyEx(SrcKey,KeyEnumIndx++,KeyName,&size,NULL,NULL,NULL,&LastWriteTime);
		} while ((SrcKey == TrgKey) && !strnicmp(KeyName,TrgSubKeyName,strlen(KeyName)) && (Err == ERROR_SUCCESS));

		// done copying this key		
		if (Err == ERROR_NO_MORE_ITEMS)
			break;

		// unknown error return
		if (Err != ERROR_SUCCESS)
			goto quit_get_err;

		// open the source subkey
		if (RegOpenKeyEx(SrcKey,KeyName,NULL,KEY_ALL_ACCESS,&SrcSubKey) != ERROR_SUCCESS)
			goto quit_get_err;

		// recurs with the subkey
		if ((Err = RegCopyKey(SrcSubKey, TrgSubKey, KeyName)) != ERROR_SUCCESS)
			break;

		if (RegCloseKey(SrcSubKey) != ERROR_SUCCESS)
			goto quit_get_err;
	} while (true);

// normal quit
quit_err:
	FreeBuff(&Buff);
	RegCloseKey(TrgSubKey);
	if (Err == ERROR_NO_MORE_ITEMS)
		return ERROR_SUCCESS;	
	else
		return Err;

// abnormal quit
quit_get_err:
	FreeBuff(&Buff);
	RegCloseKey(TrgSubKey);
	return GetLastError();
}

bool CRegistry::RegRemoveKey(HKEY hKey, LPCTSTR pszSubKey)
{
	HKEY	hSubKey = NULL;
	int	  KeyEnumIndx=0;
	char	KeyName[MAX_PATH+1];
	DWORD	size;	
	LONG	Err;
	FILETIME LastWriteTime; 
  CStringArray aSubKey;
  int i= 0;

  aSubKey.SetSize(5);

  Err = RegOpenKey(hKey, pszSubKey, &hSubKey);

	// unknown error return
	if (Err != ERROR_SUCCESS)
		goto quit_get_err;

	//Remember all sub keys
  for( i = 0; i < 2; i++)
  {
    if (i == 1)
    {
      aSubKey.SetSize(KeyEnumIndx);
      KeyEnumIndx = 0;
    }

    do {
		  size=MAX_PATH+1;
		  Err = RegEnumKeyEx(hSubKey,KeyEnumIndx,KeyName,&size,NULL,NULL,NULL,&LastWriteTime);

		  // done removing the subkeys items
		  if (Err == ERROR_NO_MORE_ITEMS)
			  break;

		  // unknown error return
		  if (Err != ERROR_SUCCESS)
			  goto quit_get_err;

		  if (i == 1)
        aSubKey.SetAt(KeyEnumIndx++, KeyName);
      else
        KeyEnumIndx++;

	  } while (true);
  }

  //Remove the subkeys now
  for (i= 0; i < aSubKey.GetSize(); i++)
  {
    // recurs with the subkey
		if ((Err = RegRemoveKey(hSubKey, aSubKey[i])) != ERROR_SUCCESS)
			break;
  }

	if (RegCloseKey(hSubKey) != ERROR_SUCCESS)
		goto quit_get_err;

  hSubKey = NULL;

  //Remove the subkey
  RegDeleteKey(hKey, pszSubKey);

	if (Err == ERROR_NO_MORE_ITEMS)
		return true;	
	else
		return Err != FALSE;

// abnormal quit
quit_get_err:
  Err = GetLastError();
  if (hSubKey)
    RegCloseKey(hSubKey);
	return Err != FALSE;
}
