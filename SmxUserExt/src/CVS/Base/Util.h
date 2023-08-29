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


LPCTSTR GetResourceString(UINT nStrID);

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

#endif // !defined(AFX_UTIL_H__934CAA37_AD33_4782_BA6E_A54DF632701D__INCLUDED_)
