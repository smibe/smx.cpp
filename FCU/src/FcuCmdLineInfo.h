/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FcuCmdLineInfo.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FcuCmdLineInfo.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_FCUCMDLINEINFO_H__0B383424_D17D_11D3_85BB_204C4F4F5020__INCLUDED_)
#define AFX_FCUCMDLINEINFO_H__0B383424_D17D_11D3_85BB_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFCUCmdLineInfo : public CCommandLineInfo
{
public:
	CFCUCmdLineInfo();
	virtual ~CFCUCmdLineInfo();
	virtual void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
  bool ParseCmdLine(CString strCmdLine);
  void Reset();

	bool IsInvalid();

//@access Public atributes
public:
  char  m_cLastOption;
  bool  m_bUninstall;
	bool  m_bCmpFile;
	bool  m_bCmpDir;
	bool  m_bInvalid;
  short m_nShowDlg;
  bool  m_bCVSMode;
	CString m_strLeftParam;
	CString m_strRightParam;
  CString m_strFileMask;
  CString m_strExcludeMask;
  CString m_strError;
};

//Global function for retrieving the command line informations
CFCUCmdLineInfo& GetCmdLineInfo();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCUCMDLINEINFO_H__0B383424_D17D_11D3_85BB_204C4F4F5020__INCLUDED_)
