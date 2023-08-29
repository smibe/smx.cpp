/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: VarMgr.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/VarMgr.h,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// VarMgr.h: interface for the CVarMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARMGR_H__6D251AE7_71E9_11D5_B2C4_000000000000__INCLUDED_)
#define AFX_VARMGR_H__6D251AE7_71E9_11D5_B2C4_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

//Flags
#define DIR_VAR 0x0001

enum ECmdVar
{
  var_LFilePath          = 0,
  var_LFileName          = 1,
  var_LFileDir           = 2,
  var_RFilePath          = 3,
  var_RFileName          = 4, 
  var_RFileDir           = 5, 
  var_CurFilePath        = 6,
  var_CurFileName        = 7,
  var_CurFileDir         = 8,
  var_ComposedFilePath   = 9,
  var_ComposedFileName   = 10,
  var_ComposedFileDir    = 11,
  var_CurLine            = 12,
  var_LDir               = 13,
  var_RDir               = 14,
  var_Comment            = 15,
};


struct CMD_VARIABLE
{
  ECmdVar     eStringVar;
  LPCTSTR     szVar;
  LPCTSTR     szDescription;
  DWORD       dwFlags;
};

class CVarMgr  
{
public:
	CVarMgr();
	virtual ~CVarMgr();

  static int GetNofVars();

  static bool IsVar(ECmdVar, LPCTSTR pszVar);
  static bool Lookup(LPCTSTR szKey, ECmdVar& eVar);
  static void InitializeVarMap();

public:
  static const CMD_VARIABLE m_CmdVars[];
  static CMap<CString, LPCTSTR, ECmdVar, ECmdVar> m_VarMap;
  static bool m_bVarMapInitialized;
};



#endif // !defined(AFX_VARMGR_H__6D251AE7_71E9_11D5_B2C4_000000000000__INCLUDED_)
