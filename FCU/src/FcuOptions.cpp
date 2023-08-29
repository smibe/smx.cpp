/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FcuOptions.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FcuOptions.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "FCUOptions.h"
#include "DlgDataEx.h"

#define PROFILE_VERSION 1400

LPCTSTR szInit                = _T("Init");

namespace
{
  LPCTSTR szVersion             = _T("Version");
  LPCTSTR szCmpFilOptions       = _T("CmpFilOptions");
  LPCTSTR szCmpDirOptions       = _T("CmpDirOptions");
  LPCTSTR szMiscOptions         = _T("MiscOptions");
  LPCTSTR szLeftColor           = _T("LeftColor");
  LPCTSTR szRightColor          = _T("RightColor");
  LPCTSTR szFoldedColor         = _T("FoldedColor");
  LPCTSTR szNotFolded           = _T("NotFolded");
  LPCTSTR szFilDiffPrint        = _T("FilDiffPrint");
  LPCTSTR szDiffDirPrint        = _T("DiffDirPrint");
  LPCTSTR szPrnFontName         = _T("PrnFontName");
  LPCTSTR szPrnFontSize         = _T("PrnFontSize");
  LPCTSTR szPrnFontWeight       = _T("PrnFontWeight");
  LPCTSTR szPrnFontItalic       = _T("PrnFontItalic");
  LPCTSTR szPrnLeftTxtColor     = _T("PrnLeftTxtColor");
  LPCTSTR szPrnRightTxtColor    = _T("PrnRightTxtColor");

  CFCUOptions *gl_pCopyOptions  = NULL;
}

LPCTSTR CFCUOptions::sm_szEditSection = _T("Editor");

CFCUOptions::CFCUOptions()
{
  m_nCmpFilOptions = OCF_ShowRight | OCF_ShowLeft | OCF_ShowComposed | OCF_ShowCmpFile;
  m_nCmpDirOptions = OCD_Identical | OCD_Different | OCD_LeftOnly | OCD_RightOnly | OCD_FilDate | OCD_FilSize |
                     OCD_FilChangeNotify | OCD_FilNotifyDlg;
  m_nMiscOptions   = OMSC_OneInstance | OMSC_IgnoreBlanks | OMSC_IgnoreEOL;

  m_crLeft  = RGB(255, 0, 0);
  m_crRight = RGB(255, 255, 0);
  m_crFolded = GetSysColor(COLOR_WINDOW);

  m_strSection = szInit;
  m_bEditFontChanged         = false;
  m_bEditTabWidthChanged     = false;
  m_bNofUnfoldedLinesChanged = false;

  m_nEditTabWidth     = AfxGetApp()->GetProfileInt(sm_szEditSection, SECEdit::sm_szTabSize, 4);
  m_nNofUnfoldedLines = AfxGetApp()->GetProfileInt(szInit, szNotFolded, 0);
  LoadEditFont();
}

CFCUOptions::~CFCUOptions()
{
}

CFCUOptions& CFCUOptions::operator=(CFCUOptions& rOther)
{
  m_nCmpFilOptions = rOther.m_nCmpFilOptions;
  m_nCmpDirOptions = rOther.m_nCmpDirOptions;
  m_nMiscOptions   = rOther.m_nMiscOptions  ;

  m_crLeft   = rOther.m_crLeft  ;
  m_crRight  = rOther.m_crRight ;
  m_crFolded = rOther.m_crFolded;

  m_strSection               = rOther.m_strSection              ;
  m_bEditFontChanged         = rOther.m_bEditFontChanged        ;
  m_bEditTabWidthChanged     = rOther.m_bEditTabWidthChanged    ;
  m_bNofUnfoldedLinesChanged = rOther.m_bNofUnfoldedLinesChanged;

  m_nEditTabWidth     = rOther.m_nEditTabWidth    ;
  m_nNofUnfoldedLines = rOther.m_nNofUnfoldedLines;

  LOGFONT logfont;
  rOther.m_EditFont.GetLogFont(&logfont);

  if (m_EditFont.m_hObject)
    m_EditFont.DeleteObject();
  
  m_EditFont.CreateFontIndirect(&logfont);
  return *this;
}

CFCUOptions& GetOptions()
{
  static CFCUOptions TheOptions;
  static CFCUOptions CopyOptions;
  if (gl_pCopyOptions == NULL)
    gl_pCopyOptions = &CopyOptions;

  return TheOptions;
}

CFCUOptions* GetPreviousOptions()
{
  return gl_pCopyOptions;
}

void CFCUOptions::SetEditTabWidth(int nTabWidth)
{
  if (m_nEditTabWidth != nTabWidth)
  {
    m_bEditTabWidthChanged = true;
    m_nEditTabWidth = nTabWidth;
  }
}

void CFCUOptions::StoreEditTabWidth()
{
  AfxGetApp()->WriteProfileInt(sm_szEditSection, SECEdit::sm_szTabSize, m_nEditTabWidth);
  m_bEditTabWidthChanged = false;
}

int CFCUOptions::GetNofUnfoldedLines()
{
  return m_nNofUnfoldedLines;
}

void CFCUOptions::SetNofUnfoldedLines(int nNofUnfoldedLines)
{
  if (m_nNofUnfoldedLines != nNofUnfoldedLines)
  {
    m_bNofUnfoldedLinesChanged = true;
    m_nNofUnfoldedLines = nNofUnfoldedLines;
  }
}

void CFCUOptions::StoreNofUnfoldedLines()
{
  AfxGetApp()->WriteProfileInt(szInit, szNotFolded, m_nNofUnfoldedLines);
  m_bNofUnfoldedLinesChanged = false;
}


int CFCUOptions::GetEditTabWidth()
{
  return m_nEditTabWidth;
}

CFont* CFCUOptions::GetEditFont()
{
  return &m_EditFont;
}

void CFCUOptions::LoadEditFont()
{
  LOGFONT logfont;

  CClientDC dc(AfxGetMainWnd());

  if (m_EditFont.m_hObject)
    m_EditFont.DeleteObject();

  ::ZeroMemory(&logfont, sizeof(logfont));
  strncpy(logfont.lfFaceName, AfxGetApp()->GetProfileString(sm_szEditSection,SECEdit::sm_szFontName,_T("Courier")), 32);
  int nFontSize = AfxGetApp()->GetProfileInt(sm_szEditSection,SECEdit::sm_szFontSize,10);
  logfont.lfHeight = MulDiv(-nFontSize, dc.GetDeviceCaps(LOGPIXELSY), 72);
	logfont.lfWeight = AfxGetApp()->GetProfileInt(sm_szEditSection,SECEdit::sm_szFontWeight,400);
	logfont.lfItalic = AfxGetApp()->GetProfileInt(sm_szEditSection,SECEdit::sm_szFontItalic,0);

  m_EditFont.CreateFontIndirect(&logfont);
}

void CFCUOptions::SetEditFont(LOGFONT *pLogFont)
{
  LOGFONT LogFont;
  if (m_EditFont.m_hObject)
    m_EditFont.GetLogFont(&LogFont);
  else
    ::ZeroMemory(&LogFont, sizeof(LOGFONT));

  if (memcmp(&LogFont,  pLogFont, sizeof(LOGFONT)))
  {
    if (m_EditFont.m_hObject)
      m_EditFont.DeleteObject();
    
    m_EditFont.CreateFontIndirect(pLogFont);
    m_bEditFontChanged = true;
  }
}

LPCTSTR CFCUOptions::GetPrintViewSection(EView eView)
{
  if (eView == VIEW_DiffDir)
    return szDiffDirPrint;
  else
    return szFilDiffPrint;
}


void CFCUOptions::SetPrnFont(EView eView, LOGFONT *pLogFont)
{
  CClientDC dc(AfxGetMainWnd());
  LPCTSTR pszSection = GetPrintViewSection(eView);

  int nFontSize = abs(MulDiv(pLogFont->lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY)));

  AfxGetApp()->WriteProfileString(szFilDiffPrint, szPrnFontName,pLogFont->lfFaceName);
  AfxGetApp()->WriteProfileInt(pszSection, szPrnFontSize, nFontSize);
  AfxGetApp()->WriteProfileInt(pszSection, szPrnFontWeight,pLogFont->lfWeight);
  AfxGetApp()->WriteProfileInt(pszSection, szPrnFontItalic,pLogFont->lfItalic);

}

void CFCUOptions::GetPrnFont(EView eView, LOGFONT *pLogFont, CDC *pDC)
{
  CClientDC dc(AfxGetMainWnd());
  LPCTSTR pszSection = GetPrintViewSection(eView);

  if (pDC == NULL)
    pDC = &dc;

  ::ZeroMemory(pLogFont, sizeof(LOGFONT));
  strncpy(pLogFont->lfFaceName, AfxGetApp()->GetProfileString(pszSection,szPrnFontName,_T("Arial")), 32);
  int nFontSize = AfxGetApp()->GetProfileInt(pszSection,szPrnFontSize,10);
  nFontSize = MulDiv(-nFontSize, pDC->GetDeviceCaps(LOGPIXELSY), 72);
  CSize Size(0, nFontSize);
  pDC->DPtoLP(&Size);
  pLogFont->lfHeight = Size.cy;
	pLogFont->lfWeight = AfxGetApp()->GetProfileInt(pszSection,szPrnFontWeight,400);
	pLogFont->lfItalic = AfxGetApp()->GetProfileInt(pszSection,szPrnFontItalic,0);
}

void CFCUOptions::SetLeftPrnColor(EView eView, COLORREF crColor)
{
  LPCTSTR pszSection = GetPrintViewSection(eView);
  AfxGetApp()->WriteProfileInt(pszSection, szPrnLeftTxtColor, crColor);
}

COLORREF CFCUOptions::GetLeftPrnColor(EView eView)
{
  LPCTSTR pszSection = GetPrintViewSection(eView);
  return (COLORREF) AfxGetApp()->GetProfileInt(pszSection, szPrnLeftTxtColor, RGB(0, 255, 0));
}

void CFCUOptions::SetRightPrnColor(EView eView, COLORREF crColor)
{
  LPCTSTR pszSection = GetPrintViewSection(eView);
  AfxGetApp()->WriteProfileInt(pszSection, szPrnRightTxtColor, crColor);
}

COLORREF CFCUOptions::GetRightPrnColor(EView eView)
{
  LPCTSTR pszSection = GetPrintViewSection(eView);
  return (COLORREF) AfxGetApp()->GetProfileInt(pszSection, szPrnRightTxtColor, RGB(0, 0, 255));
}

void CFCUOptions::StoreEditFont()
{
  CString   strEntry;
  CClientDC dc(AfxGetMainWnd());
  int       nFontSize = 0;
  LOGFONT   LogFont;

  m_EditFont.GetLogFont(&LogFont);
  nFontSize = abs(MulDiv(LogFont.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY)));

  AfxGetApp()->WriteProfileString(sm_szEditSection,SECEdit::sm_szFontName,LogFont.lfFaceName);
  AfxGetApp()->WriteProfileInt(sm_szEditSection,SECEdit::sm_szFontSize, nFontSize);
  AfxGetApp()->WriteProfileInt(sm_szEditSection,SECEdit::sm_szFontWeight,LogFont.lfWeight);
  AfxGetApp()->WriteProfileInt(sm_szEditSection,SECEdit::sm_szFontItalic,LogFont.lfItalic);

  m_bEditFontChanged = false;
}

bool CFCUOptions::GetOption(int nOptions, int nOptionBit)
{
  return (nOptions & nOptionBit) != NULL;
}

void CFCUOptions::SetOption(bool bSet, int& nOptions, int nOptionBit)
{ 
  if (bSet)
    nOptions |= nOptionBit; 
  else
    nOptions &= ~nOptionBit; 
}

void CFCUOptions::ToggleOption(int& nOptions, int nOptionBit)
{
  SetOption(!GetOption(nOptions, nOptionBit), nOptions, nOptionBit);
}

void CFCUOptions::LoadOptions()
{
	UINT nVersion = GetProfileInt(szInit, szVersion, 0);

	m_nCmpFilOptions  = AfxGetApp()->GetProfileInt(m_strSection, szCmpFilOptions, m_nCmpFilOptions);
	m_nCmpDirOptions  = AfxGetApp()->GetProfileInt(m_strSection, szCmpDirOptions, m_nCmpDirOptions);
	m_nMiscOptions    = AfxGetApp()->GetProfileInt(m_strSection, szMiscOptions,   m_nMiscOptions);

  m_crLeft  = AfxGetApp()->GetProfileInt(m_strSection, szLeftColor, m_crLeft);
  m_crRight = AfxGetApp()->GetProfileInt(m_strSection, szRightColor, m_crRight);
  m_crFolded = AfxGetApp()->GetProfileInt(m_strSection, szFoldedColor, m_crFolded);
}

void CFCUOptions::StoreOptions()
{
  AfxGetApp()->WriteProfileInt(m_strSection, szVersion, PROFILE_VERSION);

  AfxGetApp()->WriteProfileInt(m_strSection, szCmpFilOptions, m_nCmpFilOptions);
  AfxGetApp()->WriteProfileInt(m_strSection, szCmpDirOptions, m_nCmpDirOptions); 
  AfxGetApp()->WriteProfileInt(m_strSection, szMiscOptions,   m_nMiscOptions); 

  AfxGetApp()->WriteProfileInt(m_strSection, szLeftColor,  m_crLeft);
  AfxGetApp()->WriteProfileInt(m_strSection, szRightColor, m_crRight);
  AfxGetApp()->WriteProfileInt(m_strSection, szFoldedColor, m_crFolded);

  StoreEditOptions();
}

LPCTSTR CFCUOptions::GetSectionPath(LPCTSTR szSection)
{
  m_strSectionPath = m_strSection + '\\' + szSection;
  return m_strSectionPath;
}

CString CFCUOptions::GetLastCBEntry(LPCTSTR pszSection)
{
  return DDX_GetLastCBEntry(GetSectionPath(pszSection));
}

void CFCUOptions::ResetChanges()
{
  *gl_pCopyOptions = GetOptions();
}

bool CFCUOptions::HasChanged_CmpDirOption()
{
  CFCUOptions* pOpt = gl_pCopyOptions;
  return m_nCmpDirOptions != gl_pCopyOptions->m_nCmpDirOptions;
}

bool CFCUOptions::HasChanged_CmpFileOption()
{
  return m_nCmpFilOptions != gl_pCopyOptions->m_nCmpFilOptions;
}

bool CFCUOptions::HasChanged_CmpMiscOption()
{
  return m_nMiscOptions != gl_pCopyOptions->m_nMiscOptions;
}

bool CFCUOptions::HasChanged_Option(ECmpFilOption eOption)
{
  return (m_nCmpFilOptions  & eOption) != (gl_pCopyOptions->m_nCmpFilOptions & eOption);
}

bool CFCUOptions::HasChanged_Option(ECmpDirOption eOption)
{
  return (m_nCmpDirOptions & eOption) != (gl_pCopyOptions->m_nCmpDirOptions & eOption);
}

bool CFCUOptions::HasChanged_Option(EMiscOption   eOption)
{
  return (m_nMiscOptions  & eOption) != (gl_pCopyOptions->m_nMiscOptions & eOption);
}

bool CFCUOptions::HasChanged_EditColorLeft()
{
  return m_crLeft != gl_pCopyOptions->m_crLeft;
}

bool CFCUOptions::HasChanged_EditColorRight()
{
  return m_crRight != gl_pCopyOptions->m_crRight;
}

bool CFCUOptions::HasChanged_EditColorFolded()
{
  return m_crFolded != gl_pCopyOptions->m_crFolded;
}

void CFCUOptions::StoreEditOptions()
{
  AfxGetApp()->WriteProfileInt(sm_szEditSection, SECEdit::sm_szKeepTabs, (m_nCmpFilOptions & OCF_KeepTabs) == OCF_KeepTabs);
  AfxGetApp()->WriteProfileInt(sm_szEditSection, SECEdit::sm_szWordWrap, (m_nCmpFilOptions & OCF_WordWrap) == OCF_WordWrap);
  AfxGetApp()->WriteProfileInt(sm_szEditSection, SECEdit::sm_szShowTabs, (m_nCmpFilOptions & OCF_ShowTabs) == OCF_ShowTabs);
  AfxGetApp()->WriteProfileInt(sm_szEditSection, SECEdit::sm_szShowLineNo, (m_nCmpFilOptions & OCF_ShowLineNo) == OCF_ShowLineNo);
}