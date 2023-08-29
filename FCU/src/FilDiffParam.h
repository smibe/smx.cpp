/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffParam.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffParam.h,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_FILDIFFPARAM_H__D0908DC5_9194_45A5_B8D1_AB349269CA4C__INCLUDED_)
#define AFX_FILDIFFPARAM_H__D0908DC5_9194_45A5_B8D1_AB349269CA4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFilDiffParam  
{
public:
	CFilDiffParam();
	CFilDiffParam(LPCTSTR pszFile1, LPCTSTR pszFile2);
	virtual ~CFilDiffParam();

	operator LPCTSTR() const;
  static CFilDiffParam* FromString(LPCTSTR szFilDiffTag);

  void SetFile1 (LPCTSTR pszFile1)  { m_strFile1  = pszFile1; }
  void SetFile2 (LPCTSTR pszFile2)  { m_strFile2  = pszFile2; }
  void SetRelDir(LPCTSTR pszRelDir) { m_strRelDir = pszRelDir; }

  void SetFile1(LPCTSTR pszDir1, LPCTSTR pszFile1);
  void SetFile2(LPCTSTR pszDir2, LPCTSTR pszFile2);
  void AddFileToDir(CString& m_strFile, LPCTSTR pszDir, LPCTSTR pszFile);

  CString GetFile1()  { return m_strFile1; }
  CString GetFile2()  { return m_strFile2; }
  CString GetRelDir() { return m_strRelDir; }
  
  bool FromCmdLine()                 { return m_bFromCmdLine; }
  void SetFromCmdLine(bool bCmdLine) { m_bFromCmdLine = bCmdLine; }
protected:
  TCHAR m_pszTag[10];
  bool    m_bFromCmdLine;
  CString m_strFile1;
  CString m_strFile2;
  CString m_strRelDir;
};

#endif // !defined(AFX_FILDIFFPARAM_H__D0908DC5_9194_45A5_B8D1_AB349269CA4C__INCLUDED_)
