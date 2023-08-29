/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DlgDataEx.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/DlgDataEx.h,v 1.2 2002/11/22 17:11:25 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
#ifndef AFX_DLGDATAEX_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_
#define AFX_DLGDATAEX_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_

void DDX_CheckFlag(CDataExchange* pDX, int nIDC, DWORD& dwFlags, DWORD dwMask);
void DDX_RadioFlag(CDataExchange* pDX, int nIDC, DWORD& dwFlags, DWORD dwMask);

void DDX_Check(CDataExchange* pDX, int nIDC, bool& bValue);
void DDX_CheckRegistry(CDataExchange* pDX, int nIDC, LPCTSTR pszSection, LPCTSTR pszKey, bool bDefault = false);

void DDX_CBStrings(CDataExchange *pDX, int nIDC, LPCTSTR pszSection, int nMaxStrings, bool bEmptyAllowed = false);
CString DDX_GetLastCBEntry(LPCTSTR pszSection);

#endif // AFX_DLGDATAEX_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_
