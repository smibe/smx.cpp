/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FCUOptions.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FCUOptions.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_FCUOPTIONS_H__C6DF8867_A3E6_4614_BEE9_F5AEA6EEDDD0__INCLUDED_)
#define AFX_FCUOPTIONS_H__C6DF8867_A3E6_4614_BEE9_F5AEA6EEDDD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum ECmpFilOption
{
  OCF_ShowRight        = 0x0001,
  OCF_ShowLeft         = 0x0002,
  OCF_ShowComposed     = 0x0004,
  OCF_ShowCmpFile      = 0x0008,
  OCF_Wrap             = 0x0010,
  OCF_CmdMode          = 0x0020,
  OCF_SyncScrolling    = 0x0040,
  OCF_InsLeftIdentSect = 0x0080,
  OCF_FoldLines        = 0x0100,
  OCF_KeepTabs         = 0x0200,
  OCF_ShowTabs         = 0x0400,
  OCF_WordWrap         = 0x0800,
  OCF_ShowLineNo       = 0x1000,
  OCF_ShowFileName     = 0x2000,
  OCF_PrintWithColors  = 0x4000,
  OCF_Editor           = OCF_ShowLineNo | OCF_WordWrap | OCF_ShowTabs | OCF_KeepTabs,
};

enum ECmpDirOption
{
  OCD_Identical   = 0x0001,
  OCD_Different   = 0x0002,
  OCD_LeftOnly    = 0x0004,
  OCD_RightOnly   = 0x0008,
  OCD_ShowCmpDir  = 0x0010,
  OCD_UseSizeTime = 0x0010,
  OCD_Subdir      = 0x0020,
  OCD_FilSize     = 0x0040,
  OCD_FilDate     = 0x0080,
  OCD_FilTime     = 0x0100,
  OCD_FilSeconds  = 0x0200,
  OCD_Comment     = 0x0400,
  OCD_FilChangeNotify = 0x0800,
  OCD_FilNotifyDlg    = 0x1000,
};                

enum EMiscOption
{
  OMSC_OneInstance      = 0x0001,
  OMSC_IgnoreBlanks     = 0x0002,
  OMSC_IgnoreEOL        = 0x0004,
  OMSC_IgnoreCase       = 0x0008,
};

enum EView
{
  VIEW_FilDiff = 1,
  VIEW_DiffDir = 2,
};

class CFCUOptions  
{
public:
	CFCUOptions();
	virtual ~CFCUOptions();

  void SetOption(bool bSet, ECmpFilOption eOption) { SetOption(bSet, m_nCmpFilOptions, eOption); }
  void SetOption(bool bSet, ECmpDirOption eOption) { SetOption(bSet, m_nCmpDirOptions, eOption); }
  void SetOption(bool bSet, EMiscOption   eOption) { SetOption(bSet, m_nMiscOptions, eOption); }

  bool GetOption(ECmpFilOption eOption) { return GetOption(m_nCmpFilOptions, eOption); }
  bool GetOption(ECmpDirOption eOption) { return GetOption(m_nCmpDirOptions, eOption); }
  bool GetOption(EMiscOption   eOption) { return GetOption(m_nMiscOptions, eOption); }

  void ToggleOption(ECmpFilOption eOption) { ToggleOption(m_nCmpFilOptions, eOption); }
  void ToggleOption(ECmpDirOption eOption) { ToggleOption(m_nCmpDirOptions, eOption); }
  void ToggleOption(EMiscOption   eOption) { ToggleOption(m_nMiscOptions, eOption); }

  bool GetIgnoreBlanks()  { return GetOption(OMSC_IgnoreBlanks); };
  bool GetIgnoreEOL()     { return GetOption(OMSC_IgnoreEOL); };
  bool GetIgnoreCase()    { return GetOption(OMSC_IgnoreCase); };

  CFont* GetEditFont();
  void   SetEditFont(LOGFONT *pLogFont);
  void   StoreEditFont();
  void   LoadEditFont();
  bool   EditFontChanged() { return m_bEditFontChanged; }

  void SetPrnFont(EView eView, LOGFONT *pLogFont);
  void GetPrnFont(EView eView, LOGFONT *pLogFont, CDC *pDC = NULL);
  void SetLeftPrnColor(EView eView, COLORREF crColor);
  COLORREF GetLeftPrnColor(EView eView);
  void SetRightPrnColor(EView eView, COLORREF crColor);
  COLORREF GetRightPrnColor(EView eView);

  void SetEditTabWidth(int nTabWidth);
  int  GetEditTabWidth();
  void StoreEditTabWidth();
  bool EditTabSizeChanged() { return m_bEditTabWidthChanged; }
  bool NofUnfoldedLinesChanged() { return m_bNofUnfoldedLinesChanged; }
  void StoreEditOptions();

  int  GetNofUnfoldedLines();
  void SetNofUnfoldedLines(int nNofUnfoldedLines);
  void StoreNofUnfoldedLines();
  void SetEditColorLeft(COLORREF crColor)  { m_crLeft  = crColor; }
  void SetEditColorRight(COLORREF crColor) { m_crRight = crColor; }
  void SetEditColorFolded(COLORREF crColor) { m_crFolded = crColor; }

  COLORREF GetEditColorLeft()   { return m_crLeft;  }
  COLORREF GetEditColorRight()  { return m_crRight; }
  COLORREF GetEditColorFolded()  { return m_crFolded; }

  CFCUOptions& CFCUOptions::operator=(CFCUOptions& rOther);

  void LoadOptions();
  void StoreOptions();

  //Changes
  void ResetChanges();
  bool HasChanged_CmpDirOption();
  bool HasChanged_CmpFileOption();
  bool HasChanged_CmpMiscOption();
  bool HasChanged_Option(ECmpFilOption eOption);
  bool HasChanged_Option(ECmpDirOption eOption);
  bool HasChanged_Option(EMiscOption   eOption);
  bool HasChanged_EditColorLeft();
  bool HasChanged_EditColorRight();
  bool HasChanged_EditColorFolded();

  void SetSection(LPCTSTR pszSection) { m_strSection = pszSection; }
  LPCTSTR GetSection() { return m_strSection; }
  int  GetProfileVersion();
  LPCTSTR GetSectionPath(LPCTSTR szSection);

  CString GetLastCBEntry(LPCTSTR szSection);

protected:
  bool GetOption(int nOptions, int nOptionFlag);
  void SetOption(bool bSet, int& nOptions, int nOptionFlag);
  void ToggleOption(int& nOptions, int nOptionFlag);

  LPCTSTR GetPrintViewSection(EView eView);

public:
  static LPCTSTR sm_szEditSection;
protected:
  int m_nVersion;

  int m_nCmpFilOptions;
  int m_nCmpDirOptions;
  int m_nMiscOptions;
  int m_nNofUnfoldedLines;

  //Color options
	COLORREF m_crLeft;
	COLORREF m_crRight;
	COLORREF m_crFolded;

  //Edit window settings
  CFont m_EditFont;
  int   m_nEditTabWidth;

  CString m_strSection;
  CString m_strSectionPath;

  bool m_bEditTabWidthChanged;
  bool m_bEditFontChanged;
  bool m_bNofUnfoldedLinesChanged;

};

template <typename EOpt>
void DDX_CheckOption(CDataExchange* pDX, int nIDC, EOpt eOption)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  int  nValue   = 0;
  
  if (!pDX->m_bSaveAndValidate)
    nValue = GetOptions().GetOption(eOption);

  if (::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0L) & DLGC_RADIOBUTTON)
  {
    nValue = !nValue;                  //need to invert
    DDX_Radio(pDX, nIDC, nValue);
    nValue = !nValue;
  }
  else
  {
    DDX_Check(pDX, nIDC, nValue);
  }

  if (pDX->m_bSaveAndValidate)
    GetOptions().SetOption(nValue != 0, eOption);
}

template <typename EOpt>
bool GetOption(EOpt eOption)
{
 return GetOptions().GetOption(eOption);
}

template <typename EOpt>
void SetOption(bool bSet, EOpt eOption)
{
  GetOptions().SetOption(bSet, eOption);
}

template <typename EOpt>
void ToggleOption(EOpt eOption)
{
  GetOptions().ToggleOption(eOption);
}

//Global function for retrieving the options
CFCUOptions& GetOptions();
CFCUOptions* GetPreviousOptions();

#endif // !defined(AFX_FCUOPTIONS_H__C6DF8867_A3E6_4614_BEE9_F5AEA6EEDDD0__INCLUDED_)
