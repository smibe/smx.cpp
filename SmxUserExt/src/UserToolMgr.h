/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: UserToolMgr.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/UserToolMgr.h,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// UserToolMgr.h: interface for the CUserToolMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERTOOLMGR_H__1EEDF358_6C6D_11D5_B2C0_000000000000__INCLUDED_)
#define AFX_USERTOOLMGR_H__1EEDF358_6C6D_11D5_B2C0_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUserToolMgr;
class CAcceleratorManager;

//Flags
#define UT_BATCH            0x0001
#define UT_BATCHNOEXECUTE   0x0002
#define UT_MULTIPLEEXE      0x0004 

class CUserTool
{
public:
  CUserTool();
  CUserTool(LPCTSTR pszCommand, LPCTSTR pszParameter, LPCTSTR pszStartDir, LPCTSTR pszMenuString, LPCTSTR pszDescription);
  ~CUserTool();

  bool Store(CArchive& ar);
  bool Load(CArchive& ar);

public:
  CString m_strCommand;
  CString m_strStartDir;
  CString m_strParameter;
  CString m_strMenuString;
  CString m_strDescription;
  CString m_strBatchFileName;
  DWORD   m_dwFlags;
  static BYTE m_nVersion;
};

typedef bool (CWnd::*EXPAND_VAR_FKT) (CString&, CStringArray& strArray);
typedef bool (CWnd::*CAN_EXPAND_VAR_FKT) (CString&);

class CUserToolMgr  
{
public:

	CUserToolMgr ();
	virtual ~CUserToolMgr();

  UINT AddUserTool(LPCTSTR pszCommand, LPCTSTR pszParameter, LPCTSTR pszStartDir, LPCTSTR pszMenuString, LPCTSTR pszDescription);
  void RemoveUserTool(UINT nCmdID);
  void SetUserCmdID(UINT nFirstCmdID, UINT nLastCmdID);

  void UpdateUserToolMenu(CMenu *pMenu);
  void UpdateAccelMgr(CAcceleratorManager *pAccelMgr);
  bool Load(HKEY hRegKey, LPCTSTR szRegKey);
  void SetRegKey(HKEY hRegKey, LPCTSTR szRegKey);
  bool Store();
  void Clear();
  bool Load(CArchive& ar);
  bool Store(CArchive& ar);
  void SetCallbackFct(CWnd *pWnd, EXPAND_VAR_FKT pExpandVarFkt, CAN_EXPAND_VAR_FKT pCanExpandVarFkt);

  bool Execute(UINT nCmdID);
  bool CanExecute(UINT nCmdID);
  HINSTANCE Run(LPCTSTR pszCmd, LPCTSTR pszDir, LPCTSTR pszParam, UINT nShow, bool bCloseWindow);

  bool CanEval(CString& strString);
  bool Eval(CString& strString, CStringArray& strArray, bool bArray);
  void SplitString(CString& strString, CStringArray& strSplitArray);

  void GetUserToolsPath(CString& strFileName);

  CUserTool *GetUserTool(UINT nCmdID);

public:
  CArray<CUserTool *, CUserTool *> m_UserTools;

protected:
  UINT                m_nFirstCmdID;
  UINT                m_nLastCmdID;
  HKEY                m_hRegKey;
  CString             m_strRegKey;

  CWnd              *m_pWnd;
  EXPAND_VAR_FKT     m_pExpandVarFkt;
  CAN_EXPAND_VAR_FKT m_pCanExpandVarFkt;
  static  BYTE       m_nVersion;
};

#endif // !defined(AFX_USERTOOLMGR_H__1EEDF358_6C6D_11D5_B2C0_000000000000__INCLUDED_)
