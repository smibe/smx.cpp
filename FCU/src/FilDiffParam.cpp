/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffParam.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffParam.cpp,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "Stdafx.h"
#include "FilDiffParam.h"

namespace
{
  LPCTSTR szFilDiffParamTag = _T("D908DC5_9");
}

CFilDiffParam::CFilDiffParam()
{
  _tcscpy(m_pszTag, szFilDiffParamTag);

  m_bFromCmdLine = false;
}

CFilDiffParam::CFilDiffParam(LPCTSTR pszFile1, LPCTSTR pszFile2)
{
  _tcscpy(m_pszTag, szFilDiffParamTag);
  m_strFile1 = pszFile1;
  m_strFile2 = pszFile2;
}

CFilDiffParam::~CFilDiffParam()
{

}

CFilDiffParam::operator LPCTSTR() const
{
  return m_pszTag;
}

CFilDiffParam* CFilDiffParam::FromString(LPCTSTR pszTag)
{
  CFilDiffParam Param;
  int nOffset = (BYTE *) Param.m_pszTag - (BYTE *) &Param;
  if (pszTag && _tcscmp(pszTag, szFilDiffParamTag) == 0)
    return (CFilDiffParam*) ((BYTE *) pszTag - nOffset);

  return NULL;
}

void CFilDiffParam::SetFile1(LPCTSTR pszDir1, LPCTSTR pszFile1)
{
  AddFileToDir(m_strFile1, pszDir1, pszFile1);
}

void CFilDiffParam::SetFile2(LPCTSTR pszDir2, LPCTSTR pszFile2)
{
  AddFileToDir(m_strFile2, pszDir2, pszFile2);
}

void CFilDiffParam::AddFileToDir(CString& m_strFile, LPCTSTR pszDir, LPCTSTR pszFile)
{
  m_strFile = pszDir;
  if (m_strFile[m_strFile.GetLength() - 1] != '\\')
    m_strFile += '\\';

  m_strFile += pszFile;
}
