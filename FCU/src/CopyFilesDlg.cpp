/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CopyFilesDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/CopyFilesDlg.cpp,v 1.1 2002/09/20 09:34:46 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// CopyFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fcu.h"
#include "util.h"
#include "OXDirDlg.h"
#include "FCUOptions.h"
#include "DlgDataEx.h"
#include "DiffDirDoc.h"
#include "CopyFilesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD        CCopyFilesDlg::m_dwFlags = 0;
CStringArray CCopyFilesDlg::m_strDirs;

//Directory flags
#define DIR_SRC_LEFT     0x0001
#define DIR_SRC_RIGHT    0x0002
#define DIR_DEST_LEFT    0x0004
#define DIR_DEST_RIGHT   0x0008
#define DIR_DEST_SPEC    0x0010

#define FILES_LR_ONLY          0x0100
#define FILES_LR_IDENTICAL     0x0200
#define FILES_LR_OLDER         0x0400
#define FILES_LR_NEWER         0x0800
#define FILES_LR_SELECTED      0x1000
#define FLAG_PRESERVE_SUBDIR   0x2000

namespace
{
  LPCTSTR szCopyFlags = _T("CopyFlags");
  LPCTSTR szCopyToDir = _T("CopyToDir");
}

CCopyFilesDlg::CCopyFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyFilesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopyFilesDlg)
	//}}AFX_DATA_INIT
  m_bShowFileList = true;
  m_pSelected     = NULL;
}


void CCopyFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyFilesDlg)
	DDX_Control(pDX, IDC_lstFiles, m_List);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
    m_dwFlags = 0;
  else
    m_dwFlags = AfxGetApp()->GetProfileInt(GetOptions().GetSection(), szCopyFlags, DIR_SRC_LEFT | DIR_DEST_RIGHT | FILES_LR_SELECTED | FLAG_PRESERVE_SUBDIR);
    
  DDX_Button(pDX, m_dwFlags, IDC_rdFromLeftDir, DIR_SRC_LEFT);
  DDX_Button(pDX, m_dwFlags, IDC_rdFromRightDir, DIR_SRC_RIGHT);

  DDX_Button(pDX, m_dwFlags, IDC_rdToLeftDir, DIR_DEST_LEFT);
  DDX_Button(pDX, m_dwFlags, IDC_rdToRightDir, DIR_DEST_RIGHT);
  DDX_Button(pDX, m_dwFlags, IDC_rdToSelectedDir, DIR_DEST_SPEC);

  DDX_Button(pDX, m_dwFlags, IDC_ckLeftRightOnly, FILES_LR_ONLY);
  DDX_Button(pDX, m_dwFlags, IDC_ckIndentical, FILES_LR_IDENTICAL);
  DDX_Button(pDX, m_dwFlags, IDC_cbOlder, FILES_LR_OLDER);
  DDX_Button(pDX, m_dwFlags, IDC_ckNewer, FILES_LR_NEWER);
  DDX_Button(pDX, m_dwFlags, IDC_ckMarkedFiles, FILES_LR_SELECTED);

  DDX_Button(pDX, m_dwFlags, IDC_ckPreserveSubdir, FLAG_PRESERVE_SUBDIR);

  CString strSection = szCopyToDir;
  GetOptions().GetSectionPath(strSection);
  DDX_CBStrings(pDX, IDC_cbToSelectedDir, strSection, 4);

  if (pDX->m_bSaveAndValidate)
    AfxGetApp()->WriteProfileInt(GetOptions().GetSection(), szCopyFlags, m_dwFlags);
  else
  {
    UpdateControls();
    OnShowFileList();
  }

  if (pDX->m_bSaveAndValidate)
  {
    m_strFrom.SetSize(m_List.GetItemCount());
    m_strTo.SetSize(m_List.GetItemCount());

    for (int i = 0; i < m_List.GetItemCount(); i++)
    {
      m_strFrom[i] = m_List.GetItemText(i, 0);
      m_strTo[i]   = m_List.GetItemText(i, 1);
    }
  }

}


BEGIN_MESSAGE_MAP(CCopyFilesDlg, CDialog)
	//{{AFX_MSG_MAP(CCopyFilesDlg)
	ON_BN_CLICKED(IDC_rdFromLeftDir, OnrdFromLeftDir)
	ON_BN_CLICKED(IDC_rdFromRightDir, OnrdFromRightDir)
	ON_BN_CLICKED(IDC_btnShowFileList, OnShowFileList)
	ON_BN_CLICKED(IDC_cbOlder, OnCkFiles)
	ON_BN_CLICKED(IDC_rdToLeftDir, OnToLeftDir)
	ON_BN_CLICKED(IDC_rdToRightDir, OnToRightDir)
	ON_BN_CLICKED(IDC_rdToSelectedDir, OnToSelectedDir)
	ON_CBN_EDITCHANGE(IDC_cbToSelectedDir, OnEditchangeToSelectedDir)
	ON_CBN_SELCHANGE(IDC_cbToSelectedDir, OnSelchangeToSelectedDir)
	ON_WM_SIZE()
	ON_CBN_KILLFOCUS(IDC_cbToSelectedDir, OnKillfocuscbToSelectedDir)
	ON_BN_CLICKED(IDC_btToSelectedDir, OnSelectToDir)
	ON_BN_CLICKED(IDC_ckNewer, OnCkFiles)
  ON_BN_CLICKED(IDC_ckMarkedFiles, OnCkFiles)
  ON_BN_CLICKED(IDC_ckIndentical, OnCkFiles)
  ON_BN_CLICKED(IDC_ckLeftRightOnly, OnCkFiles)
	ON_BN_CLICKED(IDC_ckPreserveSubdir, OnPreserveSubdir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CCopyFilesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  CRect rcList;
  m_List.InitGrid();
  m_List.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_List.UseItemColors(true);
  m_List.GetClientRect(rcList);
  int nColWidth = rcList.Width() / 2;
  m_List.InsertColumn( 0, "From", LVCFMT_LEFT, nColWidth);
  m_List.InsertColumn( 1, "To", LVCFMT_LEFT, rcList.Width() - nColWidth);

  UpdateFileList();
  UpdateSelectedDirCtrl();
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCopyFilesDlg::DDX_Button(CDataExchange* pDX, DWORD& dwFlags, UINT nID, DWORD dwFlagMask)
{
  if (pDX->m_bSaveAndValidate)
  {
    if (((CButton *) GetDlgItem(nID))->GetCheck())
      dwFlags |= dwFlagMask;
  }
  else
  {
    ((CButton *) GetDlgItem(nID))->SetCheck(dwFlags  & dwFlagMask);
  }
}

void CCopyFilesDlg::UpdateControls()
{
  if (((CButton *) GetDlgItem(IDC_rdFromLeftDir))->GetCheck())
  {
    if (((CButton *) GetDlgItem(IDC_rdToLeftDir))->GetCheck())
    {
      ((CButton *) GetDlgItem(IDC_rdToRightDir))->SetCheck(true);
      ((CButton *) GetDlgItem(IDC_rdToLeftDir))->SetCheck(false);
    }

    ((CButton *) GetDlgItem(IDC_rdToLeftDir))->EnableWindow(false);
    ((CButton *) GetDlgItem(IDC_rdToRightDir))->EnableWindow(true);

    //Update the strings
    GetDlgItem(IDC_ckLeftRightOnly)->SetWindowText("Left only files");
    GetDlgItem(IDC_ckIndentical)->SetWindowText("Left identical  files");
    GetDlgItem(IDC_cbOlder)->SetWindowText("Left older files");
    GetDlgItem(IDC_ckNewer)->SetWindowText("Left newer files");
    GetDlgItem(IDC_ckMarkedFiles)->SetWindowText("Left marked files");

  }
  else
  {
    if (((CButton *) GetDlgItem(IDC_rdToRightDir))->GetCheck())
    {
      ((CButton *) GetDlgItem(IDC_rdToLeftDir))->SetCheck(true);
      ((CButton *) GetDlgItem(IDC_rdToRightDir))->SetCheck(false);
    }

    ((CButton *) GetDlgItem(IDC_rdToRightDir))->EnableWindow(false);
    ((CButton *) GetDlgItem(IDC_rdToLeftDir))->EnableWindow(true);

    //Update the strings
    GetDlgItem(IDC_ckLeftRightOnly)->SetWindowText("Right only files");
    GetDlgItem(IDC_ckIndentical)->SetWindowText("Right identical  files");
    GetDlgItem(IDC_cbOlder)->SetWindowText("Right older files");
    GetDlgItem(IDC_ckNewer)->SetWindowText("Right newer files");
    GetDlgItem(IDC_ckMarkedFiles)->SetWindowText("Right marked files");
  }

  UpdateStaticText();
  UpdateFileList();
}

void CCopyFilesDlg::OnrdFromLeftDir() 
{
	UpdateControls();
	
}

void CCopyFilesDlg::OnrdFromRightDir() 
{
	UpdateControls();
	
}

void CCopyFilesDlg::UpdateStaticText()
{
  CString strFromDir;
  CString strToDir;

  if (((CButton *) GetDlgItem(IDC_rdFromRightDir))->GetCheck())
    strFromDir = m_pDoc->m_strDir2;
  else
    strFromDir = m_pDoc->m_strDir1;
  
  if (((CButton *) GetDlgItem(IDC_rdToLeftDir))->GetCheck())
    strToDir = m_pDoc->m_strDir1;
  else if (((CButton *) GetDlgItem(IDC_rdToRightDir))->GetCheck())
    strToDir = m_pDoc->m_strDir2;
  else
    GetDlgItem(IDC_cbToSelectedDir)->GetWindowText(strToDir);

  GetDlgItem(IDC_stFromDir)->SetWindowText(strFromDir);
  GetDlgItem(IDC_stToDir)->SetWindowText(strToDir);
}

void CCopyFilesDlg::OnShowFileList() 
{
	CRect rcWindow;
	CRect rcList;
  GetWindowRect(rcWindow);
  m_List.GetWindowRect(rcList);

  if (m_bShowFileList)
  {
    GetDlgItem(IDC_btnShowFileList)->SetWindowText("Show file list  >>");
    m_List.ShowWindow(SW_HIDE);
    rcWindow.bottom = rcList.top;
    MoveWindow(rcWindow);
    m_bShowFileList = false;
  }
  else
  {
    GetDlgItem(IDC_btnShowFileList)->SetWindowText(">>  Hide file list");
    rcWindow.bottom = rcList.bottom + 8;
    MoveWindow(rcWindow);
    m_List.ShowWindow(SW_SHOW);
    m_bShowFileList = true;
  }
	
}

void CCopyFilesDlg::UpdateFileList()
{
  CString strDirFrom;
  CString strDirTo;
  CString strFileTo;
  EDiffType eNewer;
  EDiffType eOlder;
  EDiffType eOnly;
  EDiffType eCantCopy;
  int       nItem = 0;

  if (GetCheck(IDC_rdFromLeftDir))
  {
    strDirFrom = m_pDoc->m_strDir1;
    strDirTo   = m_pDoc->m_strDir2;
    eNewer     = diff_LNewer;
    eOnly      = diff_LOnly;
    eOlder     = diff_RNewer;
    eCantCopy  = diff_ROnly;
  }
  else
  {
    strDirFrom = m_pDoc->m_strDir2;
    strDirTo   = m_pDoc->m_strDir1;
    eNewer     = diff_RNewer;
    eOnly      = diff_ROnly;
    eOlder     = diff_LNewer;
    eCantCopy  = diff_LOnly;
  }

  if (GetCheck(IDC_rdToSelectedDir))
    GetDlgItem(IDC_cbToSelectedDir)->GetWindowText(strDirTo);

  if (!strDirFrom.IsEmpty() && strDirFrom[strDirFrom.GetLength() -1] != TCHAR('\\'))
    strDirFrom += TCHAR('\\');

  if (!strDirTo.IsEmpty() && strDirTo[strDirTo.GetLength() -1] != TCHAR('\\'))
    strDirTo += TCHAR('\\');

  m_List.DeleteAllItems();

	CDiffRec *rec;

	for (int i=0; i < m_pDoc->ItemCount(); i++) 
  {
		rec = m_pDoc->GetRecAt(i);
    strFileTo = rec->m_strFileName;

    if (!GetCheck(IDC_ckPreserveSubdir))
    {
      //Cut off the relative directory if there is any.
      int nIdx = strFileTo.ReverseFind(TCHAR('\\'));
      if (nIdx != -1)
        strFileTo = strFileTo.Mid(nIdx+1);
    }

		if(!rec) 
			continue;
    
    if (m_pSelected && GetCheck(IDC_ckMarkedFiles) && rec->m_eDiffType != eCantCopy)
    {
      if (m_pSelected[i])
      {
        //Add this marked file
        nItem = m_List.InsertItem(m_List.GetItemCount(), strDirFrom + rec->m_strFileName);
        m_List.SetItemText(nItem, 1, strDirTo + strFileTo);
        continue;
      }
    }
    if (GetCheck(IDC_ckLeftRightOnly) && rec->m_eDiffType == eOnly)
    {
      nItem = m_List.InsertItem(m_List.GetItemCount(), strDirFrom + rec->m_strFileName);
      m_List.SetItemText(nItem, 1, strDirTo + strFileTo);
      continue;
    }
    if (GetCheck(IDC_ckIndentical) && rec->m_eDiffType == diff_Equal)
    {
      nItem = m_List.InsertItem(m_List.GetItemCount(), strDirFrom + rec->m_strFileName);
      m_List.SetItemText(nItem, 1, strDirTo + strFileTo);
      continue;
    }
    if (GetCheck(IDC_cbOlder) && rec->m_eDiffType == eOlder)
    {
      nItem = m_List.InsertItem(m_List.GetItemCount(), strDirFrom + rec->m_strFileName);
      m_List.SetItemText(nItem, 1, strDirTo + strFileTo);
      continue;
    }
    if (GetCheck(IDC_ckNewer) && rec->m_eDiffType == eNewer)
    {
      nItem = m_List.InsertItem(m_List.GetItemCount(), strDirFrom + rec->m_strFileName);
      m_List.SetItemText(nItem, 1, strDirTo + strFileTo);
      continue;
    }

  }
}

bool CCopyFilesDlg::GetCheck(UINT nID)
{
  return ((CButton *)GetDlgItem(nID))->GetCheck() != NULL;
}

void CCopyFilesDlg::OnCkFiles() 
{
	UpdateFileList();
}

void CCopyFilesDlg::OnToLeftDir() 
{
  UpdateStaticText();
  UpdateFileList();
  UpdateSelectedDirCtrl();
	
}

void CCopyFilesDlg::UpdateSelectedDirCtrl()
{
  if (GetCheck(IDC_rdToSelectedDir))
  {
    GetDlgItem(IDC_cbToSelectedDir)->EnableWindow(true);
    GetDlgItem(IDC_btToSelectedDir)->EnableWindow(true);
  }
  else
  {
    GetDlgItem(IDC_cbToSelectedDir)->EnableWindow(false);
    GetDlgItem(IDC_btToSelectedDir)->EnableWindow(false);
  }
}

void CCopyFilesDlg::OnToRightDir() 
{
  UpdateStaticText();
  UpdateFileList();
  UpdateSelectedDirCtrl();
	
}

void CCopyFilesDlg::OnToSelectedDir() 
{
  UpdateStaticText();
  UpdateFileList();
  UpdateSelectedDirCtrl();
  
}

void CCopyFilesDlg::OnEditchangeToSelectedDir() 
{
  UpdateStaticText();
	
}

void CCopyFilesDlg::OnSelchangeToSelectedDir() 
{
  UpdateStaticText();
	
}

void CCopyFilesDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
  if (!m_List.GetSafeHwnd())
    return;

  CRect rcList;

  m_List.GetClientRect(rcList);
  int nColWidth = rcList.Width() / 2;
  m_List.SetColumnWidth( 0, nColWidth);
  m_List.SetColumnWidth( 0, rcList.Width() - nColWidth);
}

void CCopyFilesDlg::OnKillfocuscbToSelectedDir() 
{
  UpdateFileList();
	
}

void CCopyFilesDlg::OnSelectToDir()
{
	CString str;
  GetDlgItem(IDC_cbToSelectedDir)->GetWindowText(str);
	DWORD flags = OFN_SHOWHELP | OFN_HIDEREADONLY;
	COXDirectoryDialog dirDlg(str, _T("Select the destination directory"), TRUE, flags);
	if (dirDlg.DoModal() == IDOK)
	{
		str = dirDlg.GetDirectory();
		GetDlgItem(IDC_cbToSelectedDir)->SetWindowText(str);
	}
  
}

void CCopyFilesDlg::OnOK() 
{
  if (AfxMessageBox("Are you sure you want to copy the specified files?", MB_YESNO) == IDYES)
  {
  	CDialog::OnOK();
  }
}

void CCopyFilesDlg::OnPreserveSubdir() 
{
  UpdateFileList();
}
