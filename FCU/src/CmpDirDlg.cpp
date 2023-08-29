/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CmpDirDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/CmpDirDlg.cpp,v 1.2 2002/11/22 17:27:12 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "afxadv.h"
#include "fcu.h"
#include "FcuOptions.h"
#include "DlgDataEx.h"
#include "OXDirDlg.h"
#include "diffDirDoc.h"
#include "CmpDirDlg.h"
#include "Registry.h"
#include "FileDropTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFCUApp theApp;
extern LPCTSTR szInitPath;

//Old entries
LPCTSTR szFile     = _T("file%d");
LPCTSTR szDir1     = _T("dir1");
LPCTSTR szDir2     = _T("dir2");
LPCTSTR szfileMask = _T("fileMask");

//New entries
LPCTSTR szLeftDir  = _T("LeftDir");
LPCTSTR szRightDir = _T("RightDir");
LPCTSTR szFileMask = _T("FileMask");
LPCTSTR szExcludeMask = _T("ExcludeMask");

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCmpDirDlg 


CCmpDirDlg::CCmpDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmpDirDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCmpDirDlg)
	m_strDir1     = _T("");
	m_strDir2     = _T("");
	m_strFileMask = _T("");
	//}}AFX_DATA_INIT

  m_nNofPathNames = 0;
	//Initialize the drag and drop
  m_pFileDropTarget = new CFileDropTarget();
  m_pFileDropTarget->m_pDragEnter = (DragEnterFP) DragEnter;
  m_pFileDropTarget->m_pDragLeave = (DragLeaveFP) DragLeave;
  m_pFileDropTarget->m_pDragOver  = (DragOverFP)  DragOver;
  m_pFileDropTarget->m_pDrop      = (DropFP)      Drop;

}

CCmpDirDlg::~CCmpDirDlg()
{
  delete m_pFileDropTarget;
}

void CCmpDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

  ;

  DDX_CheckOption(pDX, IDC_CK_SUBDIR1,       OCD_Subdir);
	DDX_CheckOption(pDX, IDC_CK_IGN_BLANKS1,  OMSC_IgnoreBlanks);

  if (pDX->m_bSaveAndValidate || !UpdateRegistryEntries())
  {
    DDX_CBStrings  (pDX, IDC_CB_DIR1,         GetOptions().GetSectionPath(szLeftDir), 8);
	  DDX_CBStrings  (pDX, IDC_CB_DIR2,         GetOptions().GetSectionPath(szRightDir), 8);
	  DDX_CBStrings  (pDX, IDC_CB_FILE_MASK,    GetOptions().GetSectionPath(szFileMask), 8, true);
  }

	DDX_CBStrings  (pDX, IDC_CB_EXCLUDE_MASK, GetOptions().GetSectionPath(szExcludeMask), 8, true);

	//{{AFX_DATA_MAP(CCmpDirDlg)
  DDX_CBString(pDX, IDC_CB_DIR1,      m_strDir1);
	DDX_CBString(pDX, IDC_CB_DIR2,      m_strDir2);
  //}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
  {
    CString str;
    if (m_strDir1.IsEmpty() && m_strDir2.IsEmpty())
    {
      ((CComboBox *) GetDlgItem(IDC_CB_DIR1))->SetCurSel(0);
      ((CComboBox *) GetDlgItem(IDC_CB_DIR2))->SetCurSel(0);
    }

    if (!m_strFileMask.IsEmpty())
      DDX_CBString(pDX, IDC_CB_FILE_MASK, m_strFileMask);

    if (!m_strExcludeMask.IsEmpty())
      DDX_CBString(pDX, IDC_CB_EXCLUDE_MASK, m_strExcludeMask);

    CComboBox *pBox = (CComboBox *)GetDlgItem(IDC_CB_FILE_MASK);
    GetDlgItem(IDC_CB_FILE_MASK)->GetWindowText(str);
    if (pBox->GetCount() == 0 && str.IsEmpty())
    {
      pBox->AddString("*.c;*.cpp;*.h;*.rc;*.dsp");
      pBox->AddString("*.txt");
      pBox->AddString("*.*");
      pBox->SetCurSel(0);
    }

    pBox = (CComboBox *)GetDlgItem(IDC_CB_EXCLUDE_MASK);
    GetDlgItem(IDC_CB_EXCLUDE_MASK)->GetWindowText(str);
    if (pBox->GetCount() == 0 && str.IsEmpty())
    {
      pBox->AddString("");
      pBox->AddString("*.obj;CVS\\*.*");
      pBox->SetCurSel(0);
    }
  }
  else
  {
    DDX_CBString(pDX, IDC_CB_FILE_MASK, m_strFileMask);
    DDX_CBString(pDX, IDC_CB_EXCLUDE_MASK, m_strExcludeMask);
  }

}


BEGIN_MESSAGE_MAP(CCmpDirDlg, CDialog)
	//{{AFX_MSG_MAP(CCmpDirDlg)
	ON_BN_CLICKED(IDC_BT_DIR1, OnBtDir1)
	ON_BN_CLICKED(IDC_BT_DIR2, OnBtDir2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCmpDirDlg 

void CCmpDirDlg::OnBtDir1() 
{
	UpdateData(TRUE);
	DWORD flags = OFN_SHOWHELP | OFN_HIDEREADONLY;
	COXDirectoryDialog dirDlg(m_strDir1, _T("Select the first directory"), TRUE, flags);
	if (dirDlg.DoModal() == IDOK)
	{
		m_strDir1 = dirDlg.GetDirectory();
		GetDlgItem(IDC_CB_DIR1)->SetWindowText(m_strDir1);
	}
	
}

void CCmpDirDlg::OnBtDir2() 
{
	UpdateData(TRUE);
	DWORD flags = OFN_SHOWHELP | OFN_HIDEREADONLY;
	COXDirectoryDialog dirDlg(m_strDir2, _T("Select the second directory"), TRUE, flags);
	if (dirDlg.DoModal() == IDOK)
	{
		m_strDir2 = dirDlg.GetDirectory();
		GetDlgItem(IDC_CB_DIR2)->SetWindowText(m_strDir2);
	}
}

BOOL CCmpDirDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_pFileDropTarget->Register(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

DROPEFFECT CCmpDirDlg::DragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  m_nNofPathNames = m_pFileDropTarget->GetPathNames(pDataObject, NULL);
  switch (m_nNofPathNames)
  {
    case 1:
      {
        CWnd *pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
        if (pWnd == GetDlgItem(IDC_CB_DIR1) || pWnd == GetDlgItem(IDC_CB_DIR2))
        {
          return DROPEFFECT_LINK;
        }
      }
      break;
    case 2:
      return DROPEFFECT_LINK;
    default:
      ;
      
  }

  return DROPEFFECT_NONE;
}

void CCmpDirDlg::DragLeave()
{
  m_nNofPathNames = 0;
}

DROPEFFECT CCmpDirDlg::DragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  switch (m_nNofPathNames)
  {
    case 1:
      {
        CWnd *pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
        
        if (pWnd == GetDlgItem(IDC_CB_DIR1) || pWnd == GetDlgItem(IDC_CB_DIR2))
        {
          return DROPEFFECT_LINK;
        }
      }
      break;
    case 2:
      return DROPEFFECT_LINK;
    default:
      ;
      
  }

  return DROPEFFECT_NONE;
}

BOOL CCmpDirDlg::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
  CStringArray strArray;
  CString      str;
  UINT nNofPathNames = m_pFileDropTarget->GetPathNames(pDataObject, &strArray);

  if (nNofPathNames == 1)
  {
    CWnd *pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
    if (pWnd == GetDlgItem(IDC_CB_DIR1) || pWnd == GetDlgItem(IDC_CB_DIR2))
    {
      pWnd->SetWindowText(strArray[0]);
    }
    return TRUE;
  }

  if (nNofPathNames == 2)
  {
    GetDlgItem(IDC_CB_DIR1)->SetWindowText(strArray[0]);
    GetDlgItem(IDC_CB_DIR2)->SetWindowText(strArray[1]);
  }

  return TRUE;
}

bool CCmpDirDlg::UpdateRegistryEntries()
{
  CRegistry Reg(HKEY_CURRENT_USER);

  CString strKey;
  CString strSection;

  strSection = szInitPath;
  strSection += '\\';
  strKey = strSection + szLeftDir;
  if (!Reg.VerifyKey(strKey))
  {
    CString str;

    //We do not have entries yet, try to get them from older versions
    for (int i = 0; i < 4; i++)
    {
      strKey.Format(szFile, i);
      strKey = GetOptions().GetSectionPath(strKey);
      str = AfxGetApp()->GetProfileString(strKey, szDir1);
      if (str.IsEmpty())
          return i != 0;
        
      ((CComboBox*) GetDlgItem(IDC_CB_DIR1))->AddString(str);

      str = AfxGetApp()->GetProfileString(strKey, szDir2);
      if (!str.IsEmpty())
        ((CComboBox*) GetDlgItem(IDC_CB_DIR2))->AddString(str);
  
      str = AfxGetApp()->GetProfileString(strKey, szFileMask);
      if (!str.IsEmpty())
        ((CComboBox*) GetDlgItem(IDC_CB_FILE_MASK))->AddString(str);
    }
  }

  return false;
}
