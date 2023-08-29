/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SaveCompDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/SaveCompDlg.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "afxadv.h"
#include "FilDiff_res.h"
#include "Util.h"
#include "Resource.h"
#include "fcuOptions.h"
#include "DlgDataEx.h"
#include "OXDirDlg.h"
#include "savecompdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  LPCTSTR szSaveCompDlg = _T("SaveCompDlg");
  LPCTSTR szSelSave     = _T("SelSave");
  LPCTSTR szSubDir      = _T("SubDir");
}

CSaveCompDlg::CSaveCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveCompDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveCompDlg)
	m_bSubdir = FALSE;
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
}

CSaveCompDlg::~CSaveCompDlg()
{
}

void CSaveCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	CString strSection = GetOptions().GetSectionPath(szSaveCompDlg);

  if (!pDX->m_bSaveAndValidate)
  {
    m_nSelSave = AfxGetApp()->GetProfileInt(strSection, szSelSave, 2);
	  m_bSubdir = AfxGetApp()->GetProfileInt(strSection, szSubDir, 1);
  }
  
  DDX_CBStrings  (pDX, IDC_CB_DIR, strSection, 4);

  //{{AFX_DATA_MAP(CSaveCompDlg)
	DDX_Radio(pDX, IDC_R_RIGHT, m_nSelSave);
	DDX_Check(pDX, IDC_CK_SUBDIR, m_bSubdir);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
  	DDX_Text(pDX, IDC_ED_COMPFILENAME, m_strFileName);
    AfxGetApp()->WriteProfileInt(strSection, szSelSave, m_nSelSave);
	  AfxGetApp()->WriteProfileInt(strSection, szSubDir,  m_bSubdir);
  }
}


BEGIN_MESSAGE_MAP(CSaveCompDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveCompDlg)
	ON_BN_CLICKED(IDC_R_RIGHT, OnRRight)
	ON_BN_CLICKED(IDC_R_LEFT, OnRLeft)
	ON_BN_CLICKED(IDC_R_SAVE_AS, OnRSaveAs)
	ON_BN_CLICKED(IDC_R_MERGE, OnRMerge)
	ON_BN_CLICKED(IDC_B_SEL, OnBSelDir)
	ON_CBN_EDITCHANGE(IDC_CB_DIR, OnEditchangeCbDir)
	ON_BN_CLICKED(IDC_B_SAVE_AS, OnBSaveAs)
	ON_BN_CLICKED(IDC_CK_SUBDIR, OnCkSubdir)
	ON_CBN_SELCHANGE(IDC_CB_DIR, OnSelchangeCbDir)
	ON_CBN_KILLFOCUS(IDC_CB_DIR, OnKillfocusCbDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSaveCompDlg::OnOK()
{
  CString strFileName;	
  GetDlgItem(IDC_ED_COMPFILENAME)->GetWindowText(strFileName);
  if (strFileName.IsEmpty())
  {
    AfxMessageBox(IDS_NoFilenameSpecified);
    return;
  }

  CDialog::OnOK();
}

void CSaveCompDlg::OnRRight() 
{
	m_nSelSave = sel_right;
	EnableDirSet(FALSE);	
	GetDlgItem(IDC_B_SAVE_AS)->EnableWindow(FALSE);
	UpdateFileName();	
}

void CSaveCompDlg::OnRLeft() 
{
	m_nSelSave = sel_left;
	EnableDirSet(FALSE);	
	GetDlgItem(IDC_B_SAVE_AS)->EnableWindow(FALSE);
	UpdateFileName();	
}

void CSaveCompDlg::EnableDirSet(BOOL enable)
{
	GetDlgItem(IDC_GR_DIR)->EnableWindow(enable);
	GetDlgItem(IDC_CB_DIR)->EnableWindow(enable);
	GetDlgItem(IDC_B_SEL)->EnableWindow(enable);
	
  if (!m_strRelDir.IsEmpty())
    GetDlgItem(IDC_CK_SUBDIR)->EnableWindow(enable && !m_strRelDir.IsEmpty());
}

void CSaveCompDlg::OnRSaveAs() 
{
	m_nSelSave = sel_save;
	EnableDirSet(FALSE);	
	GetDlgItem(IDC_B_SAVE_AS)->EnableWindow(TRUE);
	UpdateFileName();	
	
}

void CSaveCompDlg::OnRMerge() 
{
	m_nSelSave = sel_merge;
	EnableDirSet(TRUE);	
	GetDlgItem(IDC_B_SAVE_AS)->EnableWindow(FALSE);
	UpdateFileName();	
	
}

BOOL CSaveCompDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  GetDlgItem(IDC_CB_DIR)->GetWindowText(m_strDir);
	GetDlgItem(IDC_CK_SUBDIR)->EnableWindow(!m_strRelDir.IsEmpty());
	GetDlgItem(IDC_B_SAVE_AS)->EnableWindow(m_nSelSave == sel_merge);
	EnableDirSet(m_nSelSave == sel_save);

  UpdateFileName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSaveCompDlg::OnBSelDir() 
{
	UpdateData(FALSE);
	DWORD flags = OFN_SHOWHELP | OFN_HIDEREADONLY;

  COXDirectoryDialog dirDlg(m_strDir, GetResourceString(IDS_SelectDirectory), TRUE, flags);
	if (dirDlg.DoModal() == IDOK)
	{
		m_strDir = dirDlg.GetDirectory();
		GetDlgItem(IDC_CB_DIR)->SetWindowText(m_strDir);
	}
	UpdateFileName();
}

void CSaveCompDlg::SetFileNames(CString strRelDir, CString strLeftPath, CString strRightPath)
{
	m_strRelDir        = strRelDir;

  AddBackslashToDir(m_strRelDir);

  m_strLeftFileName  = strLeftPath;
	m_strRightFileName = strRightPath;

	//Initialize the directory
	GetCurrentDirectory(_MAX_PATH, m_strDir.GetBuffer(_MAX_PATH));
	m_strDir.ReleaseBuffer();
}

void CSaveCompDlg::UpdateFileName()
{
	CString strName = "";
	
  switch(m_nSelSave)
	{
		case sel_left:
			strName = m_strLeftFileName;
			break;
		
		case sel_right:
			strName = m_strRightFileName;
			break;

		case sel_merge:
      {
			  if (!m_strDir.IsEmpty() && m_strDir[m_strDir.GetLength() - 1] != '\\')
				  m_strDir += '\\';
			  
        if (m_bSubdir)
          strName = m_strDir + m_strRelDir + GetFileNameFromPath(m_strRightFileName);
        else
          strName = m_strDir + GetFileNameFromPath(m_strRightFileName);
      }
			break;

		case sel_save:
			strName = m_strFileName;
			break;
		default:
			ASSERT(FALSE);
			;
	}
	
	GetDlgItem(IDC_ED_COMPFILENAME)->SetWindowText(strName);
}

void CSaveCompDlg::OnEditchangeCbDir() 
{
	UpdateFileName();
}

void CSaveCompDlg::OnBSaveAs() 
{
	CFileDialog dlg(FALSE, NULL, m_strFileName, OFN_HIDEREADONLY, GetResourceString(IDS_AllFiles));

  if(dlg.DoModal()==IDOK) 
  {
		m_strFileName = dlg.GetPathName();
	}
	
	UpdateFileName();
}

CString CSaveCompDlg::GetFileName()
{
	return m_strFileName;
}

void CSaveCompDlg::OnCkSubdir() 
{
	m_bSubdir = ((CButton *) GetDlgItem(IDC_CK_SUBDIR))->GetCheck();
  UpdateFileName();
}

void CSaveCompDlg::OnSelchangeCbDir() 
{
	CComboBox *pBox = (CComboBox *) GetDlgItem(IDC_CB_DIR);
	if (pBox->GetCurSel() != CB_ERR)
		pBox->GetLBText(pBox->GetCurSel(), m_strDir);
  else
	  pBox->GetWindowText(m_strDir);

  UpdateFileName();
}

void CSaveCompDlg::OnKillfocusCbDir() 
{
	UpdateFileName();
}
