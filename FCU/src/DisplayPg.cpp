/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DisplayPg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DisplayPg.cpp,v 1.3 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "fcu.h"
#include "DlgDataEx.h"
#include "FCUOptions.h"
#include "diffDirDoc.h"
#include "DisplayPg.h"
#include "FilDiffView.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CFilCmpPg::CFilCmpPg()
: CSMXPropertyPg()
{
	//{{AFX_DATA_INIT(CFilCmpPg)
	//}}AFX_DATA_INIT
  m_nNotFolded  = GetOptions().GetNofUnfoldedLines();

}

void CFilCmpPg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilCmpPg)
	DDX_CheckOption(pDX, IDC_CB_COMPOSED_WND,   OCF_ShowComposed);
	DDX_CheckOption(pDX, IDC_CB_LEFT_WND,       OCF_ShowLeft);
	DDX_CheckOption(pDX, IDC_RIGHT_WND,         OCF_ShowRight);
  DDX_CheckOption(pDX, IDC_rdWrap,            OCF_Wrap);
  DDX_CheckOption(pDX, IDC_CB_SYNCSCROLLING,  OCF_SyncScrolling);
  DDX_CheckOption(pDX, IDC_ckShowFileNames,   OCF_ShowFileName);
  DDX_CheckOption(pDX, IDC_rdInsLeftIdentSect,OCF_InsLeftIdentSect);
  DDX_CheckOption(pDX, IDC_ckFoldLines,       OCF_FoldLines);
	DDX_Text(pDX,        IDC_NOTFOLDED,         m_nNotFolded);
	DDV_MinMaxInt(pDX, m_nNotFolded, 0, 64);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    GetOptions().SetNofUnfoldedLines(m_nNotFolded);
  }
  else
  {
    OnFoldLines();
  }
}


BEGIN_MESSAGE_MAP(CFilCmpPg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CFilCmpPg)
	ON_BN_CLICKED(IDC_ckFoldLines, OnFoldLines)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CFilCmpPg::OnFoldLines()
{
	BOOL bChecked = ((CButton *) GetDlgItem(IDC_ckFoldLines))->GetCheck();	

  if (bChecked)
  {
    GetDlgItem(IDC_NOTFOLDED)->EnableWindow(TRUE);
  }
  else
  {
    GetDlgItem(IDC_NOTFOLDED)->EnableWindow(FALSE);
  }
  
}



CEditorPg::CEditorPg()
: CSMXPropertyPg()
{
	//{{AFX_DATA_INIT(CEditorPg)
	//}}AFX_DATA_INIT
  m_nTabWidth   = GetOptions().GetEditTabWidth();
  m_pEdit       = NULL;
}

void CEditorPg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CEditorPg)
	DDX_CheckOption(pDX, IDC_ckKeepTabs, OCF_KeepTabs);
	DDX_CheckOption(pDX, IDC_ckShowTabs, OCF_ShowTabs);
	DDX_CheckOption(pDX, IDC_ckShowLineNo, OCF_ShowLineNo);
  DDX_Control(pDX, IDC_EB_FONT,   m_ebFontCtrl);
	DDX_Text   (pDX, IDC_TAB_WIDTH, m_nTabWidth);
	DDV_MinMaxInt(pDX, m_nTabWidth, 1, 64);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    GetOptions().SetEditTabWidth(m_nTabWidth);
  }
}

BOOL CEditorPg::OnInitDialog() 
{
	CSMXPropertyPg::OnInitDialog();
	
  LOGFONT logfont;
  CFont *pFont = GetOptions().GetEditFont();

  ASSERT(pFont->m_hObject != NULL);
  pFont->GetLogFont(&logfont);

  m_ebFontCtrl.SetFont(pFont);
  m_ebFontCtrl.SetWindowText(logfont.lfFaceName);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


BEGIN_MESSAGE_MAP(CEditorPg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CEditorPg)
	ON_BN_CLICKED(IDC_BTN_FONT_CHANGE, OnBtnFontChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditorPg::OnBtnFontChange() 
{
	CFontDialog dlg;
  LOGFONT logfont;
  CFont *pFont = GetOptions().GetEditFont();
  
  pFont->GetLogFont(&logfont);
  dlg.m_cf.Flags |= CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
  dlg.m_cf.Flags &= ~CF_EFFECTS;
  dlg.m_cf.lpLogFont = &logfont;

  if (dlg.DoModal() == IDOK)
  {
    LOGFONT templogfont;
    pFont->GetLogFont(&templogfont);

    if (memcmp(&templogfont, &logfont, sizeof(logfont)) != NULL)
    {
      //This will change the options font which is our pFont!!
      GetOptions().SetEditFont(&logfont);

      //Set the new font and window text
      m_ebFontCtrl.SetFont(pFont);
      m_ebFontCtrl.SetWindowText(logfont.lfFaceName);

    }
  }
}







CFilDiffPrintPg::CFilDiffPrintPg()
: CSMXPropertyPg()
{
	//{{AFX_DATA_INIT(CFilDiffPrintPg)
	//}}AFX_DATA_INIT
  m_eView = VIEW_FilDiff;
}

void CFilDiffPrintPg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CEditorPg)
	DDX_CheckOption(pDX, IDC_ckPrintWithColors, OCF_PrintWithColors);
  DDX_Control(pDX, IDC_EB_FONT,   m_ebFontCtrl);
  DDX_Control(pDX, IDC_ebLeftColor, m_ebLeftColor);
  DDX_Control(pDX, IDC_ebRightColor, m_ebRightColor);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    LOGFONT logfont = {0};
    m_Font.GetLogFont(&logfont);
    GetOptions().SetPrnFont(m_eView, &logfont);
    GetOptions().SetLeftPrnColor(m_eView, m_crLeft);
    GetOptions().SetRightPrnColor(m_eView, m_crRight);
  }
}

BOOL CFilDiffPrintPg::OnInitDialog() 
{
	CSMXPropertyPg::OnInitDialog();
	
  LOGFONT logfont;
  GetOptions().GetPrnFont(m_eView, &logfont);
  m_Font.CreateFontIndirect(&logfont);

  m_crLeft = GetOptions().GetLeftPrnColor(m_eView);
  m_crRight = GetOptions().GetRightPrnColor(m_eView);

  m_ebFontCtrl.SetFont(&m_Font);
  m_ebFontCtrl.SetWindowText(logfont.lfFaceName);

  m_ebLeftColor.SetFont(&m_Font);
  m_ebLeftColor.SetWindowText(GetResourceString(IDS_left));
  m_ebRightColor.SetFont(&m_Font);
  m_ebRightColor.SetWindowText(GetResourceString(IDS_right));
  
  OnPrintWithColors();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


BEGIN_MESSAGE_MAP(CFilDiffPrintPg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CEditorPg)
	ON_BN_CLICKED(IDC_BTN_FONT_CHANGE, OnBtnFontChange)
	ON_BN_CLICKED(IDC_btnLeftColor, OnBtnLeftColorChange)
	ON_BN_CLICKED(IDC_btnRightColor, OnBtnRightColorChange)
	ON_BN_CLICKED(IDC_ckPrintWithColors, OnPrintWithColors)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CFilDiffPrintPg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSMXPropertyPg::OnCtlColor(pDC, pWnd, nCtlColor);

	if(nCtlColor == CTLCOLOR_EDIT && pWnd == &m_ebLeftColor)
	{
		pDC->SetTextColor(m_crLeft);
		return hbr;
	}

	if(nCtlColor == CTLCOLOR_EDIT && pWnd == &m_ebRightColor)
	{
		pDC->SetTextColor(m_crRight);
		return hbr;
	}

  pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));

	return hbr;
}

void CFilDiffPrintPg::OnPrintWithColors()
{
  bool bEnable = false;
  if (((CButton *) GetDlgItem(IDC_ckPrintWithColors))->GetCheck())
    bEnable = true;

  m_ebRightColor.EnableWindow(bEnable);
  m_ebLeftColor.EnableWindow(bEnable);
  GetDlgItem(IDC_btnLeftColor)->EnableWindow(bEnable);
  GetDlgItem(IDC_btnRightColor)->EnableWindow(bEnable);
}

void CFilDiffPrintPg::OnBtnRightColorChange()
{
	CColorDialog dlg;
  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = m_crRight;
	if(dlg.DoModal() == IDOK)
	{
		m_crRight = dlg.GetColor();
	}
	
	m_ebRightColor.Invalidate();
}

void CFilDiffPrintPg::OnBtnLeftColorChange()
{
	CColorDialog dlg;
  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = m_crLeft;
	if(dlg.DoModal() == IDOK)
	{
		m_crLeft = dlg.GetColor();
	}
	
	m_ebLeftColor.Invalidate();
}

void CFilDiffPrintPg::OnBtnFontChange() 
{
	CFontDialog dlg;
  LOGFONT logfont;
  
  m_Font.GetLogFont(&logfont);
  dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
  dlg.m_cf.Flags &= ~CF_EFFECTS;
  dlg.m_cf.lpLogFont = &logfont;

  if (dlg.DoModal() == IDOK)
  {
    m_Font.DeleteObject();
    m_Font.CreateFontIndirect(&logfont);

    //Set the new font and window text
    m_ebFontCtrl.SetFont(&m_Font);
    m_ebFontCtrl.SetWindowText(logfont.lfFaceName);

    m_ebLeftColor.SetFont(&m_Font);
    m_ebRightColor.SetFont(&m_Font);
  }
}








void CDirCmpPg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirCmpPg)
	DDX_CheckOption(pDX, IDC_CB_DIFF,         OCD_Different);
	DDX_CheckOption(pDX, IDC_CB_IDENTICAL,    OCD_Identical);
	DDX_CheckOption(pDX, IDC_CB_RIGHT,        OCD_RightOnly);
	DDX_CheckOption(pDX, IDC_CB_LEFT,         OCD_LeftOnly);
	DDX_CheckOption(pDX, IDC_CB_SIZE,         OCD_FilSize);
	DDX_CheckOption(pDX, IDC_CB_DATE,         OCD_FilDate);
	DDX_CheckOption(pDX, IDC_CB_TIME,         OCD_FilTime);
	DDX_CheckOption(pDX, IDC_CB_SECONDS,      OCD_FilSeconds);
	DDX_CheckOption(pDX, IDC_CB_COMMENT,      OCD_Comment);
  DDX_CheckOption(pDX, IDC_CB_FIleNotifications, OCD_FilChangeNotify);
  DDX_CheckOption(pDX, IDC_CB_ShowNotifyDialog, OCD_FilNotifyDlg);
  //}}AFX_DATA_MAP

  if (!pDX->m_bSaveAndValidate)
  {
    OnCbDate();
    OnCbTime();
  }
}


BEGIN_MESSAGE_MAP(CDirCmpPg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CDirCmpPg)
	ON_BN_CLICKED(IDC_CB_DATE, OnCbDate)
	ON_BN_CLICKED(IDC_CB_TIME, OnCbTime)
  ON_BN_CLICKED(IDC_CB_FIleNotifications, OnCbFileNotifications)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCmdLinePg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdLinePg)
	DDX_CheckOption(pDX, IDC_CK_FILECMP_DLG,  OCF_ShowCmpFile);
	DDX_CheckOption(pDX, IDC_CK_DIRCMP_DLG,   OCD_ShowCmpDir);
	DDX_CheckOption(pDX, IDC_ckOneInstance,   OMSC_OneInstance);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CCmdLinePg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CCmdLinePg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDirCmpPg::OnCbDate() 
{
	BOOL bChecked = ((CButton *) GetDlgItem(IDC_CB_DATE))->GetCheck();	
  GetDlgItem(IDC_CB_TIME)->EnableWindow(bChecked);
  GetDlgItem(IDC_CB_SECONDS)->EnableWindow(bChecked);
}

void CDirCmpPg::OnCbTime() 
{
	BOOL bChecked = ((CButton *) GetDlgItem(IDC_CB_TIME))->GetCheck();
  BOOL bEnabled = GetDlgItem(IDC_CB_TIME)->IsWindowEnabled();
  GetDlgItem(IDC_CB_SECONDS)->EnableWindow(bChecked && bEnabled);
	
}

void CDirCmpPg::OnCbFileNotifications()
{
	BOOL bChecked = ((CButton *) GetDlgItem(IDC_CB_FIleNotifications))->GetCheck();
  GetDlgItem(IDC_CB_ShowNotifyDialog)->EnableWindow(bChecked);
}