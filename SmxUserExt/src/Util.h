/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: Util.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/Util.h,v 1.2 2002/11/22 17:21:29 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc


#if !defined(AFX_UTIL_H__934CAA37_AD33_4782_BA6E_A54DF632701D__INCLUDED_)
#define AFX_UTIL_H__934CAA37_AD33_4782_BA6E_A54DF632701D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef DWORD_PTR
#ifndef _WIN64
#define DWORD_PTR DWORD
#endif
#endif


// Floating point numbers are handled in ways that don't guarantee expressions to yield exact 
// results. It's risky to test for exact equality. Therefore it is better to test the 
// relative error against a user defined EPSILON.
// For all comparisions of floating point valued (doubles) the following macro definitions
// should be used.

#ifndef DBL_EPSILON
#define DBL_EPSILON (1.0e-10)
#endif


#define EQU(x,y)        ( (fabs((x) - (y)) / max(1.0, max(fabs(x), fabs(y)))) < DBL_EPSILON )
#define NEQ(x,y)        ( !EQU((x), (y)) )
#define LT(x,y)         ( (((x) - (y)) / max(1.0, max(fabs(x), fabs(y)))) < -DBL_EPSILON )
#define LTE(x,y)        ( (((x) - (y)) / max(1.0, max(fabs(x), fabs(y)))) < +DBL_EPSILON )
#define GT(x,y)         ( LT(y, x) )
#define GTE(x,y)        ( LTE(y, x) )
#define SGN(__x)        ( __x < 0 ? -1 : 1)

#define GetButtonCtrl(nID)   ((CButton *)   GetDlgItem(nID))
#define GetEditCtrl(nID)     ((CEdit *)     GetDlgItem(nID))
#define GetComboBoxCtrl(nID) ((CComboBox *) GetDlgItem(nID))

enum EShowCtrl
{
  SWC_enabled  = 0,
  SWC_disabled = 1,
  SWC_hidden   = 2,
};


LPCTSTR GetResourceString(UINT nStrID);

CString FormatString(UINT nFormatID, ...);
CString GetBackupFileName(LPCTSTR pszFileName, int nBackup);
CString BackupFile(LPCTSTR pszPathName, LPCTSTR pszBackupDir, int nMaxFiles);


bool IsRelativePath(CString& str);


void GetFileName(CString& strFilePath);
void GetFileDir(CString& strFilePath);
void AddBackslashToDir(CString& strDir);

CString GetFileNameFromPath(const CString& strPath);
CString GetDirFromPath(const CString& strPath);

bool FileExists(LPCTSTR pszFilePath);

double String2Double(LPCTSTR str);
CString Double2String(double dbValue, int nPrecision = 2, int nGrpNumber = -1);

void CatStrings(CString& strLine, CStringArray& strStringArray, TCHAR cSep);
void SplitString(CString& strLine, CStringArray& strStringArray, TCHAR cSep);

TCHAR GetGrpSymbol();
int GetGrpNumber();
TCHAR GetDeciSymbol();

double Round(double dbValue, int nNofDecDigits = 2);

LPCTSTR GetShellErrorString(DWORD dwError);

CString LoadString(UINT nID);

HWND SmxHtmlHelp(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD_PTR dwData, bool bShowError = true);

class CSmxLock
{
public:  
  CSmxLock(bool& bLock) : m_rbLock(bLock) { m_bMyLock = false;}
  ~CSmxLock() { if (m_bMyLock) m_rbLock = false; }
  void Lock() { ASSERT(m_rbLock == false); m_rbLock = true; m_bMyLock = true; }
  bool IsLocked() { return m_rbLock; }
protected:
  bool& m_rbLock;
  bool  m_bMyLock;
};

#endif // !defined(AFX_UTIL_H__934CAA37_AD33_4782_BA6E_A54DF632701D__INCLUDED_)
