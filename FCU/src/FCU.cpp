/*******************************************************************************
// Copyright (C), GeH
// All rights reserved.
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FCU.cpp,v $
//
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FCU.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include <wincrypt.h>
#include <locale.h>
#include "SInstance.h"
#include "FCU.h"
#include "FCUOptions.h"
#include "FilDiffParam.h"
#include "resource.h"
#include "OXGridList.h"
#include "HtmlHelp.h"

#include "SMXRes.h"
#include "MainFrm.h"
#include "diffDirDoc.h"
#include "diffDirView.h"
#include "FilDiffView.h"
#include "fcucmdlineinfo.h"
#include "UsageDlg.h"
#include "DisplayPg.h"
#include "Registry.h"
#include "Crypt.h"
#include "PasswdDlg.h"
#include "WaitDlg.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	UINT gl_AccelGroups[] =
	{
	IDS_File, 1,
	  ID_CMP_DIR, ID_CMP_FILE, ID_APP_EXIT, ID_FILE_CLOSE,
	  0,
	IDS_View, 1,
	  ID_VIEW_TOOLBAR,
	  ID_VIEW_STATUS_BAR,
	  ID_VIEW_OPTIONS,
	  0,
	IDS_Tools, 1,
	  ID_TOOLS_CUSTOMIZE_TOOLBAR,
	  ID_TOOLS_CUSTOMIZE_KEYBOARD, ID_TOOLS_CUSTOMIZE_USERTOOLS,
	  0,
	};
}

#define PASSWD_LEN    12  //length of a password
#define LIC_START_IDX 4   //the start of password in license string
#define LIC_LEN       36  //length of the license string

// Declare the variables needed
#ifdef _DEBUG
CMemoryState oldMemState, newMemState, diffMemState;
#endif

#define CURRENT_VERSION 1300
LPCTSTR szInitPath = _T("Software\\smibe\\FCU\\Init");
LPCTSTR szFCULicense = _T("Licenses\\BB36F480-EA08-11D5-B2E8-174B3E6E2A01");
namespace
{
	LPCTSTR szVersion = _T("Version");
	LPCTSTR szRegKey = _T("smibe");
	LPCTSTR szPrevRegKey = _T("Software\\GeHWinTools");
	LPCTSTR szFCURegKey = _T("Software\\smibe\\FCU");
	LPCTSTR szPasswd = _T("Passwd");
	LPCTSTR szUserName = _T("UserName");
	LPCTSTR szCmdMode = _T("cmdMode");
	LPCTSTR szInit = _T("Init");
	LPCTSTR szLeftColorHigh = _T("LeftColorHigh");
	LPCTSTR szLeftColorLow = _T("LeftColorLow");
	LPCTSTR szRightColorHigh = _T("RightColorHigh");
	LPCTSTR szRightColorLow = _T("RightColorLow");
	LPCTSTR szIgnoreBlank = _T("ignoreBlank");
	LPCTSTR szIgnoreEol = _T("ignoreEol");
	LPCTSTR szIgnoreCase = _T("ignoreCase");
	LPCTSTR szCmpFlags = _T("ignoreCase");
	LPCTSTR szIs = _T("is");

	LPCTSTR szFCUHelp = _T("FCU.chm");

	//Variables used for calculating the evaluation days
	bool  gl_bRegistered = false;
	int   gl_nRemindHim = 0;
	CTime gl_PwdTime;

}

//Old option defines
#define  FG_SF_Identical   ((DWORD) 1 << 1)
#define  FG_SF_Different   ((DWORD) 1 << 2)
#define  FG_SF_LeftOnly    ((DWORD) 1 << 3)
#define  FG_SF_RightOnly   ((DWORD) 1 << 4)
#define  FG_SW_Right  	   ((DWORD) 1 << 5)
#define  FG_SW_Left  		   ((DWORD) 1 << 6)
#define  FG_SW_Composed    ((DWORD) 1 << 7)
#define  FG_SD_CmpFile     ((DWORD) 1 << 8)
#define  FG_SD_CmpDir      ((DWORD) 1 << 9)
#define  FG_AP_OneInstance ((DWORD) 1 << 10)
#define  FG_CF_Wrap        ((DWORD) 1 << 11)

bool HasRegistered()
{
	return gl_bRegistered;
}

extern LPCTSTR szCode;

CString GetPasswdKey(LPCTSTR pszRegName)
{
	CString str1;
	CString str2;
	str1.LoadString(IDS_small);
	str2.LoadString(IDS_beautiful);

	if (pszRegName)
		return str1.Left(2) + TCHAR('#') + str1.Mid(2) + szIs[0] + str2.Left(3) + szCode + pszRegName + str2.Mid(3);
	else
		return str1.Left(2) + TCHAR('#') + str1.Mid(2) + szIs[0] + str2.Left(3) + szCode + str2.Mid(3);
}


/////////////////////////////////////////////////////////////////////////////
// CFCUApp

BEGIN_MESSAGE_MAP(CFCUApp, CWinApp)
	//{{AFX_MSG_MAP(CFCUApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	// Standard-Druckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCUApp Konstruktion

CFCUApp::CFCUApp()
	: m_RegisteredTime(1972, 11, 14, 0, 0, 0),
	m_cmdInfo(GetCmdLineInfo())
{
	m_bUninstall = false;
	m_bActivateOtherInstance = false;
	m_bInitialized = false;
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CFCUApp-Objekt

CFCUApp theApp;


BOOL CFCUApp::InitInstance()
{
#ifdef _DEBUG
	oldMemState.Checkpoint();
#endif
	AfxEnableControlContainer();

	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.

	CRegistry regOld(HKEY_CURRENT_USER);
	setlocale(LC_ALL, _T(""));
	UpdatePrinterSelection(FALSE);

	if (!regOld.VerifyKey(szInitPath))
	{
		//We have older versions, convert them to our new registry settings
		regOld.SetKeyHandle(HKEY_CURRENT_USER);
		if (regOld.CopyKey(szPrevRegKey, "Software", szRegKey))
			regOld.RegRemoveKey(HKEY_CURRENT_USER, szPrevRegKey);
	}

	SetRegistryKey(szRegKey);

	if (!SetPassword(false))
		return FALSE;

	/*
	CTime Time = CTime::GetCurrentTime();
	//CTime Time(1972, 11, 14, 0, 0, 0);
	CTimeSpan OneDay(1, 0, 0, 0);
	int nYear;
	int nMonth;
	int nDay;
	CString strIn;
	CString str;
	CString strOut;
	CCrypt Crypt;
	CString strTrace;

	for (int i = 0; i < 100; i++)
	{
	  nYear = Time.GetYear();
	  nMonth = Time.GetMonth();
	  nDay = Time.GetDay();
	  strIn.Format("%02d%02d%02d", nDay, nMonth,  nYear - 1966);

	  for (int k=0; k<5; k++)
	  {
		strOut = strIn;
		Crypt.EncryptDigits(strOut, m_strCode, PASSWD_LEN);

		str.Format("%02d.%02d.%04d", nDay, nMonth,  nYear);
		strTrace.Format("%s: %s\n", str, strOut);
		TRACE(strTrace);

		Crypt.DecryptDigits(strOut, m_strCode);
		ASSERT(strOut == strIn);

	  }

	  Time += OneDay;
	}
	*/

	//Load the profile before parsing the command line
	OleInitialize(NULL);
	LoadProfile();

	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	ParseCommandLine(m_cmdInfo);

	if (__argc > 1 && (m_cmdInfo.IsInvalid() || (!m_cmdInfo.m_bCmpDir && !m_cmdInfo.m_bCmpFile)))
	{
		AfxInitRichEdit();
		CUsageDlg dlg;
		dlg.m_strError = m_cmdInfo.m_strError;
		dlg.DoModal();
		m_cmdInfo.Reset();
	}

	bool bOneInstance = GetOption(OMSC_OneInstance) && m_lpCmdLine && *m_lpCmdLine;

	if (bOneInstance)
	{
		if (m_instanceChecker.PreviousInstanceRunning())
		{
			if (m_instanceChecker.ActivatePreviousInstance(m_lpCmdLine))
			{
				m_bActivateOtherInstance = true;
				return FALSE;
			}
		}
	}

	TRY
	{
	  m_UserToolMgr.Load(HKEY_CURRENT_USER, szInitPath);
	}
		CATCH(CArchiveException, e)
	{
		m_UserToolMgr.Clear();
		m_UserToolMgr.SetRegKey(HKEY_CURRENT_USER, szInitPath);
		AfxMessageBox("Could not load the user tools!");
	}
	END_CATCH;

	m_UserToolMgr.SetUserCmdID(ID_USER_TOOLS_FIRST, ID_USER_TOOLS_LAST);

	//	LoadStdProfileSettings();
	// Main-MDI-Frame
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	//Disable the recent file list
	m_pRecentFileList = NULL;

	// Das Hauptfenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	m_AccelMgr.Connect(pMainFrame);
	m_AccelMgr.m_strView = "Main";
	m_AccelMgr.SetAccelGroups(gl_AccelGroups, sizeof(gl_AccelGroups) / sizeof(UINT));

	if (!m_AccelMgr.Load(HKEY_CURRENT_USER, szInitPath, _T("MainAccel")))
	{
		m_AccelMgr.CreateDefaultTable(IDR_MAINFRAME);
	}
	else
	{
		m_AccelMgr.SetAccelTableID(IDR_MAINFRAME);
	}

	m_UserToolMgr.UpdateAccelMgr(&m_AccelMgr);
	m_AccelMgr.InitAccelGroups();

	if (m_cmdInfo.m_bCmpFile)
	{
		CFilDiffParam* pParam = new CFilDiffParam(m_cmdInfo.m_strLeftParam, m_cmdInfo.m_strRightParam);
		pParam->SetFromCmdLine(true);
		pMainFrame->PostMessage(WMU_CMPFILE, (WPARAM)pParam, (LPARAM)TRUE);
	}

	if (m_cmdInfo.m_bCmpDir)
		pMainFrame->PostMessage(WM_COMMAND, ID_CMP_DIR, NULL);

	// If this is the first instance of our App then track it so any other instances can find it.
	if (bOneInstance && !m_instanceChecker.PreviousInstanceRunning())
		m_instanceChecker.TrackFirstInstanceRunning();

	m_bInitialized = true;
	return TRUE;
}

//bRegister set to true if the user wants to register
bool CFCUApp::SetPassword(bool bRegister)
{
	CString strPasswd = GetProfileString(szInit, szPasswd);
	CString strUserName = GetProfileString(szInit, szUserName);

	m_strCode = GetPasswdKey(strUserName);

	if (strPasswd.IsEmpty())
	{
		//Try to find the password from previous installation
		CRegistry reg(HKEY_CLASSES_ROOT);
		CString   strKey = szFCULicense;
		CString   strLicString;
		CString   strLicFile;
		BOOL bPrevInstallation = reg.VerifyKey(strKey);

		GetLicFileName(strLicFile);

		if (!bPrevInstallation)
		{
			//Try to find our secret license file

			if (!strLicFile.IsEmpty())
			{
				GetPrivateProfileString(strKey, _T("@"), _T(""), strLicString.GetBuffer(64), 64, strLicFile);
				strLicString.ReleaseBuffer();
			}
		}
		else
		{
			reg.Read("", strLicString);
		}

		if (strLicString.IsEmpty())
		{
			GetCurrentLicString(strLicString);

			//There is no version installed yet, write the licenses.
			if (!bPrevInstallation)
			{
				reg.CreateKey(HKEY_CLASSES_ROOT, strKey);
			}

			reg.Write("", strLicString);
			CString strNewLicString;
			strKey = _T("HKEY_CLASSES_ROOT\\") + strKey;
			strNewLicString.Format(" \"%s\" ", strLicString);

			WritePrivateProfileString(strKey, _T("@ "), strNewLicString, strLicFile);

			//Reset the file time
			CFile File;
			if (File.Open(strLicFile, CFile::modeWrite))
			{
				FILETIME FileTime;
				FileTime.dwLowDateTime = 0xb227d899;
				FileTime.dwHighDateTime = 0x01bd7942;

				if (::SetFileTime((HANDLE)File.m_hFile, &FileTime, &FileTime, &FileTime) == NULL)
				{
					DWORD dwError = GetLastError();
				}
				File.Close();
			}

		}

		GetPasswordFromLicString(strPasswd, strLicString);

		if (!strPasswd.IsEmpty())
		{
			//Remember the password
			WriteProfileString(szInit, szPasswd, strPasswd);
		}
	}

	CPasswdDlg dlg;
	dlg.SetCode(m_strCode);

	dlg.m_bRegister = bRegister;

	if (strPasswd.IsEmpty() || bRegister)
	{
		dlg.SetUserName(strUserName);
		if (dlg.DoModal() != IDOK)
		{
			if (!bRegister)
				WriteProfileString(szInit, szPasswd, "");
			return false;
		}

		dlg.GetPasswd(strPasswd);
		dlg.GetUserName(strUserName);
		dlg.GetCode(m_strCode);

		//Remember the password and user name
		WriteProfileString(szInit, szPasswd, strPasswd);
		WriteProfileString(szInit, szUserName, strUserName);
	}

	CWaitDlg WaitDlg;

	if (!CPasswdDlg::GetTimeFromPasswd(gl_PwdTime, strPasswd, m_strCode))
	{
		WriteProfileString(szInit, szPasswd, "");
		gl_PwdTime = CTime(1973, 11, 18, 0, 0, 0);
		WaitDlg.SetText("The password is invalid!");
	}

	if (gl_PwdTime != m_RegisteredTime)
	{
		//Check if the date is OK
		int nEvalDays = GetEvalDays();
		int nRemaining = GetProfileInt(szInit, "EvalDays", 0);
		if (nEvalDays > 0 && nEvalDays < 5 && nRemaining != nEvalDays)
		{
			CString strMsg;
			strMsg.Format("Your password expires in %d days!", nEvalDays);
			AfxMessageBox(strMsg);
			WriteProfileInt(szInit, "EvalDays", nEvalDays);
		}
		else if (nEvalDays <= 0)
		{
			WaitDlg.DoModal();
		}
	}
	else if (strUserName.IsEmpty())
	{
		UINT nVersion = GetProfileInt(szInit, szVersion, 1200);
		if (nVersion < 1400)
		{
			CString strMsg;
			strMsg.Format("Your license key is not valid for this version!\n"
				"Please contact fcu@smibe.com for an updated license key!");

			AfxMessageBox(strMsg);
			CString strLicString;
			CString strPasswd;
			GetCurrentLicString(strLicString);
			GetPasswordFromLicString(strPasswd, strLicString);
			WriteProfileString(szInit, szPasswd, strPasswd);
			WriteProfileInt(szInit, "EvalDays", 30);
			gl_PwdTime = CTime::GetCurrentTime();
		}
		else
		{
			WaitDlg.DoModal();
		}
	}
	else
	{
		gl_bRegistered = true;
	}

	return true;
}


CRecentFileList* CFCUApp::GetRecentFileList()
{
	return m_pRecentFileList;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialogfelddaten
		//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CShExecCtrl	m_MailTo;
	CShExecCtrl	m_WWW;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	void UpdateEvalVersion();

	// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_btnMailTo, m_MailTo);
	DDX_Control(pDX, IDC_btnWWW, m_WWW);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_btnRegister, OnRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl, um das Dialogfeld aufzurufen
void CFCUApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFCUApp-Befehle

void CFCUApp::OnHelp()
{
	CWinApp::OnHelp();
	//WinHelp(0, HELP_FINDER);

}

void CFCUApp::OnHelpContents()
{
	WinHelp(0, HELP_CONTENTS);
}

int CFCUApp::ExitInstance()
{
	if (!m_bUninstall && !m_bActivateOtherInstance && m_bInitialized)
	{
		StoreProfile();

		TRY
		{
		  m_UserToolMgr.Store();
		}
			CATCH(CArchiveException, e)
		{
			AfxMessageBox("Could not store the user tools!");
		}
		END_CATCH;
	}

	OleUninitialize();
	return CWinApp::ExitInstance();
}

CAcceleratorManager& GetAccelMgr()
{
	return theApp.m_AccelMgr;
}

CUserToolMgr& GetUserToolMgr()
{
	return theApp.m_UserToolMgr;
}

void CFCUApp::LoadProfile()
{
	UINT nVersion = GetProfileInt(szInit, szVersion, 1200);
	CString strKey = szFCURegKey;
	CRegistry reg;

	BOOL bInstalled = reg.VerifyKey(HKEY_CURRENT_USER, strKey);

	if (bInstalled && nVersion < 1300)
	{
		//Older version
		SetOption(GetProfileInt(szInit, szIgnoreBlank, 1) != 0, OMSC_IgnoreBlanks);
		SetOption(GetProfileInt(szInit, szIgnoreEol, 1) != 0, OMSC_IgnoreEOL);
		SetOption(GetProfileInt(szInit, szIgnoreCase, 1) != 0, OMSC_IgnoreCase);

		RemoveProfile(szInit, szIgnoreBlank);
		RemoveProfile(szInit, szIgnoreEol);
		RemoveProfile(szInit, szIgnoreCase);

		UINT nHigh = GetProfileInt(szInit, szLeftColorHigh, HIWORD(RGB(255, 0, 0)));
		UINT nLow = GetProfileInt(szInit, szLeftColorLow, LOWORD(RGB(255, 0, 0)));
		GetOptions().SetEditColorLeft(MAKELONG(nLow, nHigh));
		RemoveProfile(szInit, szLeftColorLow);
		RemoveProfile(szInit, szLeftColorHigh);

		nHigh = GetProfileInt(szInit, szRightColorHigh, HIWORD(RGB(255, 255, 0)));
		nLow = GetProfileInt(szInit, szRightColorLow, LOWORD(RGB(255, 255, 0)));
		GetOptions().SetEditColorRight(MAKELONG(nLow, nHigh));
		RemoveProfile(szInit, szRightColorLow);
		RemoveProfile(szInit, szRightColorHigh);
	}

	if (bInstalled && nVersion < 1310)
	{
		DWORD dwFlags = 0;
		SetOption(GetProfileInt(szInit, szCmdMode, 0) != 0, OCF_CmdMode);
		dwFlags = GetProfileInt(szInit, szCmpFlags, 0);

		SetOption((dwFlags & FG_SF_Identical) != FG_SF_Identical, OCD_Identical);
		SetOption((dwFlags & FG_SF_Different) != FG_SF_Different, OCD_Different);
		SetOption((dwFlags & FG_SF_LeftOnly) != FG_SF_LeftOnly, OCD_LeftOnly);
		SetOption((dwFlags & FG_SF_RightOnly) != FG_SF_RightOnly, OCD_RightOnly);
		SetOption((dwFlags & FG_SW_Right) != FG_SW_Right, OCF_ShowRight);
		SetOption((dwFlags & FG_SW_Left) != FG_SW_Left, OCF_ShowLeft);
		SetOption((dwFlags & FG_SW_Composed) != FG_SW_Composed, OCF_ShowComposed);
		SetOption((dwFlags & FG_SD_CmpFile) != FG_SD_CmpFile, OCF_ShowCmpFile);
		SetOption((dwFlags & FG_SD_CmpDir) != FG_SD_CmpDir, OCD_ShowCmpDir);
		SetOption((dwFlags & FG_AP_OneInstance) != FG_AP_OneInstance, OMSC_OneInstance);
		SetOption((dwFlags & FG_CF_Wrap) != FG_CF_Wrap, OCF_Wrap);

		RemoveProfile(szInit, szCmdMode);
		RemoveProfile(szInit, szCmpFlags);
	}

	GetOptions().LoadOptions();
}

void CFCUApp::StoreProfile()
{
	GetOptions().StoreOptions();
}

//Remove the specified entry from the profile
void CFCUApp::RemoveProfile(LPCTSTR szSection, LPCTSTR szValue)
{
	HKEY hKey;
	CString strKey;

	strKey.Format("%s\\%s", szFCURegKey, szSection);
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strKey, NULL, KEY_WRITE, &hKey) == ERROR_SUCCESS)
	{
		VERIFY(RegDeleteValue(hKey, szValue) == ERROR_SUCCESS);
		RegCloseKey(hKey);
	}
}

CMainFrame* GetMainFrame()
{
	return (CMainFrame*)theApp.m_pMainWnd;
}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	m_MailTo.GetWindowText(str);
	str = "mailto:" + str;
	m_MailTo.SetCommand(str);

	UpdateEvalVersion();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFCUApp::WinHelp(DWORD dwData, UINT nCmd)
{
	if (!GetHelpInfo(dwData, nCmd))
	{
		dwData = NULL;
		nCmd = HH_DISPLAY_TOPIC;
	}


	//CWinApp::WinHelp(dwData, nCmd);
	CString strFileName;
	GetModuleFileName(AfxGetInstanceHandle(), strFileName.GetBuffer(MAX_PATH), MAX_PATH);
	strFileName.ReleaseBuffer();
	int nIdx = strFileName.ReverseFind('\\');
	if (nIdx)
	{
		strFileName = strFileName.Left(nIdx + 1);
		strFileName += szFCUHelp;
	}
	else
	{
		strFileName = szFCUHelp;
	}
	if (!SmxHtmlHelp(GetDesktopWindow(), strFileName, nCmd, dwData))
	{
		AfxMessageBox(_T("Sorry, could not find the help file!"));
	}

}

bool CFCUApp::GetHelpInfo(DWORD& dwData, UINT& nCmd)
{
	UINT nID = dwData - 0x00020000;

	switch (nID)
	{
	case IDR_DIFF_DIR:
	case IDR_DIFF_FIL:
	case IDD_USER_TOOLS:
	case IDD_COPY_FILES:
		nCmd = HH_HELP_CONTEXT;
		return true;
	}

	//Dialogs
	switch (nID)
	{
	case IDD_SMX_SHEET:
		nCmd = HH_HELP_CONTEXT;
		return true;
	}

	return false;
}

int GetEvalDays()
{
	CTime Time;
	CTime CurTime = CTime::GetCurrentTime();

	CTimeSpan ThirtyDays(31, 0, 0, 0);
	Time = gl_PwdTime + ThirtyDays;
	CTimeSpan Remaining = Time - CurTime;

	return Remaining.GetDays();
}

void CheckEvaluation()
{
	if (!gl_bRegistered)
	{
		int nEvalDays = GetEvalDays();
		if (nEvalDays <= 0 && ++gl_nRemindHim > 4)
		{
			CWaitDlg WaitDlg;
			WaitDlg.DoModal();
			gl_nRemindHim = 0;
		}
	}
}


void CAboutDlg::OnRegister()
{
	theApp.SetPassword(true);
	UpdateEvalVersion();
}

void CAboutDlg::UpdateEvalVersion()
{
	if (!HasRegistered())
	{
		int nEvalDays = GetEvalDays();
		CString strEval;

		if (nEvalDays > 0)
			strEval.Format(_T("Evaluation Version  (%d days remaining)"), nEvalDays);
		else
			strEval.Format(_T("Evaluation Version  (expired)"));

		GetDlgItem(IDC_stEvalVersion)->SetWindowText(strEval);
	}
	else
	{
		CString strUserName = AfxGetApp()->GetProfileString(szInit, szUserName);
		if (!strUserName.IsEmpty())
		{
			GetDlgItem(IDC_stEvalVersion)->SetWindowText("This copy of FCU is licensed to:");
			GetDlgItem(IDC_stUserName)->SetWindowText(strUserName);
			GetDlgItem(IDC_stUserName)->ShowWindow(SW_SHOW);
		}
		else
		{
			GetDlgItem(IDC_stEvalVersion)->SetWindowText("Registered Version");
		}

		GetDlgItem(IDC_btnRegister)->ShowWindow(SW_HIDE);
	}
}

void CFCUApp::GetPasswordFromLicString(CString& strPasswd, CString& strLicString)
{
	strPasswd = strLicString.Mid(LIC_START_IDX, PASSWD_LEN);
}

void CFCUApp::GetCurrentLicString(CString& strLicString)
{
	CString strPasswd;
	LPTSTR  pBuf = strLicString.GetBuffer(LIC_LEN + 1);

	GetPasswdString(strPasswd, CTime::GetCurrentTime());

	srand((unsigned)GetTickCount());

	int i = 0;
	for (i = 0; i < LIC_LEN; i++)
	{
		if (i < LIC_START_IDX)
			pBuf[i] = (TCHAR)((rand() % 26) + TCHAR('a'));
		else if (i < strPasswd.GetLength() + LIC_START_IDX)
			pBuf[i] = strPasswd[i - LIC_START_IDX];
		else
			pBuf[i] = (TCHAR)((rand() % 26) + TCHAR('a'));
	}
	pBuf[i] = TCHAR('\0');
	strLicString.ReleaseBuffer();
	strLicString.MakeLower();
}

void CFCUApp::GetPasswdString(CString& strPasswd, CTime& Time)
{
	int nYear;
	int nMonth;
	int nDay;
	CCrypt Crypt;

	nYear = Time.GetYear();
	nMonth = Time.GetMonth();
	nDay = Time.GetDay();
	strPasswd.Format("%02d%02d%02d", nDay, nMonth, nYear - 1966);

	Crypt.EncryptDigits(strPasswd, m_strCode, PASSWD_LEN);
}

void CFCUApp::GetLicFileName(CString& strLicFileName)
{
	CString strTempFile;
	CString strTempPath;
	GetSystemDirectory(strLicFileName.GetBuffer(MAX_PATH), MAX_PATH);
	strLicFileName.ReleaseBuffer();

	if (!strLicFileName.IsEmpty() && strLicFileName[strLicFileName.GetLength() - 1] != TCHAR('\\'))
		strLicFileName += TCHAR('\\');

	CString strTemp;
	strTemp = strLicFileName + _T("MSCOMM.SRG");

	CFile TempFile;
	if (TempFile.Open(strTemp, CFile::modeRead))
	{
		FILETIME Creation;
		FILETIME Access;
		FILETIME Write;
		if (GetFileTime((HANDLE)TempFile.m_hFile, &Creation, &Access, &Write) == 0)
		{
			DWORD dwError = GetLastError();
		}
		TempFile.Close();
	}

	strLicFileName += _T("MSCOMSB.SRG");

	//Check if the file exists
	CFile File;
	if (File.Open(strLicFileName, CFile::modeRead))
	{
		File.Close();
	}
	else
	{
		//The file does not exist, check if we have write access
		if (File.Open(strLicFileName, CFile::modeCreate))
		{
			File.Close();
			DeleteFile(strLicFileName);
		}
		else
		{
			LPITEMIDLIST pidl = NULL;

			// we can't write the file, create a file in the users application directory.
			SHGetSpecialFolderLocation(AfxGetMainWnd()->GetSafeHwnd(), CSIDL_APPDATA, &pidl);
			SHGetPathFromIDList(pidl, strLicFileName.GetBuffer(MAX_PATH));
			strLicFileName.ReleaseBuffer();

			if (!strLicFileName.IsEmpty() && strLicFileName[strLicFileName.GetLength() - 1] != TCHAR('\\'))
				strLicFileName += TCHAR('\\');

			strLicFileName += _T("MSCOMSB.SRG");
		}
	}
}

void CFCUApp::SetPageOrientation(bool bLandscape)
{
	if (m_hDevMode)
	{
		DEVMODE* pDevMode = (DEVMODE*)GlobalLock(m_hDevMode);
		if (pDevMode->dmFields & DM_ORIENTATION)
			pDevMode->dmOrientation = bLandscape ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;
		GlobalUnlock(m_hDevMode);
	}
}

