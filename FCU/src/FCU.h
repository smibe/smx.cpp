/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FCU.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FCU.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// 060698:GeH created.

#if !defined(AFX_FCU_H__2867E822_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_)
#define AFX_FCU_H__2867E822_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole
#include "fcucmdlineinfo.h"
#include "AcceleratorManager.h"
#include "UserToolMgr.h"
#include "SInstance.h"

class CMainFrame;

class CFCUApp : public CWinApp
{
public:
	CFCUApp();

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CFCUApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CFCUApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelp();
	afx_msg void OnHelpContents();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
		CMultiDocTemplate* m_pDiffTemplate;
		CRecentFileList* GetRecentFileList();
    bool SetPassword(bool bRegister);

    void SetPageOrientation(bool bLandscape);
protected:
    void RemoveProfile(LPCTSTR szSection, LPCTSTR szValue);
    void StoreProfile();
    void LoadProfile();
    
    bool GetHelpInfo(DWORD& dwData, UINT& nCmd);

//License functions
  void GetLicFileName(CString& strLicFileName);
  void GetPasswdString(CString& strPasswd, CTime& Time);
  void GetCurrentLicString(CString& strLicString);
  void GetPasswordFromLicString(CString& strPasswd, CString& strLicString);

public:
	bool m_bUninstall;
  bool m_bActivateOtherInstance;
  bool m_bInitialized;
  CString             m_strCode;
	CFCUCmdLineInfo&    m_cmdInfo;
  CAcceleratorManager m_AccelMgr;
  CUserToolMgr        m_UserToolMgr;
  CInstanceChecker    m_instanceChecker;
  CTime               m_RegisteredTime;
};

CAcceleratorManager& GetAccelMgr();
CUserToolMgr&        GetUserToolMgr();
CMainFrame*          GetMainFrame();
void                 CheckEvaluation();   //Check if the evaluation copy has expired
int                  GetEvalDays();       //Get the number of remaining evaluation days.
bool                 HasRegistered();


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_FCU_H__2867E822_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_)
