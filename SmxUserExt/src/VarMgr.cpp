/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: VarMgr.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/VarMgr.cpp,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// VarMgr.cpp: implementation of the CVarMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "fcu.h"
#include "StdAfx.h"
#include "VarMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const CMD_VARIABLE CVarMgr::m_CmdVars[] = 
{
  { var_LFilePath         , _T("$LFilePath"), _T("Left File Path"), NULL },
  { var_LFileName         , _T("$LFileName"), _T("Left File Name"), NULL  },
  { var_LFileDir          , _T("$LFileDir"), _T("Left File Directory"), DIR_VAR },
  { var_RFilePath         , _T("$RFilePath"), _T("Right File Path"), NULL  },
  { var_RFileName         , _T("$RFileName"), _T("Right File Name"), NULL  },
  { var_RFileDir          , _T("$RFileDir"), _T("Right File Directory"), DIR_VAR},
  { var_CurFilePath       , _T("$CurFilePath"), _T("Current File Path"), NULL  },
  { var_CurFileName       , _T("$CurFileName"), _T("Current File Name"), NULL  },
  { var_CurFileDir        , _T("$CurFileDir"), _T("Current File Directory"), DIR_VAR},
  { var_ComposedFilePath  , _T("$ComposedFilePath"), _T("Composed File Path"), NULL  },
  { var_ComposedFileName  , _T("$ComposedFileName"), _T("Composed File Name"), NULL  },
  { var_ComposedFileDir   , _T("$ComposedFileDir"), _T("Composed File Directory"), DIR_VAR},
  { var_CurLine           , _T("$CurLine"), _T("Current Line Number"), NULL},
  { var_LDir              , _T("$LDir"), _T("Left Directory"), DIR_VAR},
  { var_RDir              , _T("$RDir"), _T("Right Directory"), DIR_VAR},
  { var_Comment           , _T("$Comment"), _T("Comment (Compare Dir view)"), NULL},

};

CMap<CString, LPCTSTR, ECmdVar, ECmdVar> CVarMgr::m_VarMap;
bool CVarMgr::m_bVarMapInitialized = false;


CVarMgr::CVarMgr()
{

#ifdef _DEBUG
  for (int i = 0; i < sizeof (m_CmdVars); i++)
  {
    ASSERT(m_CmdVars[i].eStringVar == i );
  }
#endif;
}

CVarMgr::~CVarMgr()
{

}

bool CVarMgr::IsVar(ECmdVar eVar, LPCTSTR pszVar)
{
  ASSERT((int) eVar < GetNofVars());
  return !(_tcscmp(m_CmdVars[eVar].szVar, pszVar));
}

int CVarMgr::GetNofVars()
{ 
  return sizeof (m_CmdVars) / sizeof(CMD_VARIABLE); 
}

bool CVarMgr::Lookup(LPCTSTR szKey, ECmdVar& eVar)
{
  if (!m_bVarMapInitialized)
  {
    InitializeVarMap();
  }
  
  return m_VarMap.Lookup(szKey, eVar) != FALSE;
}

void CVarMgr::InitializeVarMap()
{
  for (int i = 0; i < GetNofVars(); i++)
  {
    m_VarMap.SetAt(m_CmdVars[i].szVar, (ECmdVar) i); 
  }
  m_bVarMapInitialized = true;
}
