/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: CmpFilDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/CmpFilDlg.cpp,v 1.2 2002/11/22 17:27:12 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "FilDiff_res.h"
#include "resource.h"
#include "registry.h"
#include "DlgDataEx.h"
#include "FcuOptions.h"
#include "DlgDataEx.h"
#include "cmpfildlg.h"
#include "path.h"
#include "util.h"
#include "FileDropTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCTSTR szInitPath;
extern LPCTSTR szFileMask;
extern LPCTSTR szInit;

namespace
{
  //old entries
  LPCTSTR szCmpFil     = _T("CmpFil");
  LPCTSTR szLeft       = _T("left%d");
  LPCTSTR szRight      = _T("right%d");

  LPCTSTR szLeftFiles  = _T("LeftFiles");
  LPCTSTR szRightFiles = _T("RightFiles");

  LPCTSTR szFileSelMask1 = _T("FileSelMask1");
  LPCTSTR szFileSelMask2 = _T("FileSelMask2");
}

CCmpFilDlg::CCmpFilDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmpFilDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCmpFilDlg)
	m_strLeftFileName = _T("");
	m_strRightFileName = _T("");
	//}}AFX_DATA_INIT

  m_nNofPathNames = 0;
	//Initialize the drag and drop
  m_pFileDropTarget = new CFileDropTarget();
  m_pFileDropTarget->m_pDragEnter = (DragEnterFP)&CCmpFilDlg::DragEnter;
  m_pFileDropTarget->m_pDragLeave = (DragLeaveFP) &CCmpFilDlg::DragLeave;
  m_pFileDropTarget->m_pDragOver  = (DragOverFP)  &CCmpFilDlg::DragOver;
  m_pFileDropTarget->m_pDrop      = (DropFP)      &CCmpFilDlg::Drop;

}

CCmpFilDlg::~CCmpFilDlg()
{
  delete m_pFileDropTarget;
}

void CCmpFilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
  DDX_CheckOption(pDX, IDC_CK_IGN_BLANKS, OMSC_IgnoreBlanks);
  
  if (pDX->m_bSaveAndValidate || !UpdateRegistryEntries())
  {
    DDX_CBStrings  (pDX, IDC_CB_FIL1, GetOptions().GetSectionPath(szLeftFiles), 8);
    DDX_CBStrings  (pDX, IDC_CB_FIL2, GetOptions().GetSectionPath(szRightFiles), 8);
  }

  //{{AFX_DATA_MAP(CCmpFilDlg)
	DDX_CBString(pDX, IDC_CB_FIL1, m_strLeftFileName);
	DDX_CBString(pDX, IDC_CB_FIL2, m_strRightFileName);
	//}}AFX_DATA_MAP

  if (!pDX->m_bSaveAndValidate)
  {
	  DDX_CBStrings  (pDX, IDC_CB_MASK,    GetOptions().GetSectionPath(szFileMask), 8, true);

    if (m_strLeftFileName.IsEmpty() && m_strRightFileName.IsEmpty())
    {
      ((CComboBox *) GetDlgItem(IDC_CB_FIL1))->SetCurSel(0);
      ((CComboBox *) GetDlgItem(IDC_CB_FIL2))->SetCurSel(0);
    }
  }

}


BEGIN_MESSAGE_MAP(CCmpFilDlg, CDialog)
	//{{AFX_MSG_MAP(CCmpFilDlg)
	ON_BN_CLICKED(IDC_BT_FIL1, OnBtFil1)
	ON_BN_CLICKED(IDC_BT_FIL2, OnBtFil2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CCmpFilDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_pFileDropTarget->Register(this);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCmpFilDlg::OnBtFil1() 
{
	CString strLeftFileName;
  GetDlgItem(IDC_CB_FIL1)->GetWindowText(strLeftFileName);
  COXDirSpec Path(strLeftFileName);
  if (((COXDirSpec) Path).Exists())
  {
    AddBackslashToDir(strLeftFileName);
    strLeftFileName += _T("*.*");
  }

  CString strFilter;
  GetFilter(strFilter, szFileSelMask1);
  CFileDialog dlg(TRUE, NULL, strLeftFileName, OFN_HIDEREADONLY, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		StoreLastFilter(dlg, strFilter, szFileSelMask1);
    strLeftFileName = dlg.GetPathName();
		GetDlgItem(IDC_CB_FIL1)->SetWindowText(strLeftFileName);
	}
	
}

void CCmpFilDlg::OnBtFil2() 
{
	CString strRightFileName;
  GetDlgItem(IDC_CB_FIL2)->GetWindowText(strRightFileName);

  COXDirSpec Path(strRightFileName);
  if (((COXDirSpec) Path).Exists())
  {
    AddBackslashToDir(strRightFileName);
    strRightFileName += _T("*.*");
  }

  CString strFilter;
  GetFilter(strFilter, szFileSelMask2);
  CFileDialog dlg(TRUE, NULL, strRightFileName, OFN_HIDEREADONLY, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		StoreLastFilter(dlg, strFilter, szFileSelMask2);
		strRightFileName = dlg.GetPathName();
		GetDlgItem(IDC_CB_FIL2)->SetWindowText(strRightFileName);
	}
	
}

DROPEFFECT CCmpFilDlg::DragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  m_nNofPathNames = m_pFileDropTarget->GetPathNames(pDataObject, NULL);
  switch (m_nNofPathNames)
  {
    case 1:
      {
        CWnd *pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
        if (pWnd == GetDlgItem(IDC_CB_FIL1) || pWnd == GetDlgItem(IDC_CB_FIL2))
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

void CCmpFilDlg::DragLeave()
{
  m_nNofPathNames = 0;
}

DROPEFFECT CCmpFilDlg::DragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  switch (m_nNofPathNames)
  {
    case 1:
      {
        CWnd *pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
        if (pWnd != 0 && pWnd != this)
        {
          int XXX = 0;
        }

        if (pWnd == GetDlgItem(IDC_CB_FIL1) || pWnd == GetDlgItem(IDC_CB_FIL2))
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

BOOL CCmpFilDlg::Drop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
  CStringArray strArray;
  CString      str;
  UINT nNofPathNames = m_pFileDropTarget->GetPathNames(pDataObject, &strArray);

  if (nNofPathNames == 1)
  {
    CWnd *pWnd = ChildWindowFromPoint(point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
    if (pWnd == GetDlgItem(IDC_CB_FIL1) || pWnd == GetDlgItem(IDC_CB_FIL2))
    {
      pWnd->SetWindowText(strArray[0]);
    }
    return TRUE;
  }

  if (nNofPathNames == 2)
  {
    GetDlgItem(IDC_CB_FIL1)->SetWindowText(strArray[0]);
    GetDlgItem(IDC_CB_FIL2)->SetWindowText(strArray[1]);
  }

  return TRUE;
}


bool CCmpFilDlg::UpdateRegistryEntries()
{
  CRegistry Reg(HKEY_CURRENT_USER);

  CString strKey;
  CString strSection;

  strSection = szInitPath;
  strSection += '\\';
  strKey = strSection + szLeftFiles;
  if (!Reg.VerifyKey(strKey))
  {
    CString str;
    strSection = GetOptions().GetSectionPath(szCmpFil);

    //We do not have entries yet, try to get them from older versions
    for (int i = 0; i < 4; i++)
    {
      strKey.Format(szLeft, i);
      str = AfxGetApp()->GetProfileString(strSection, strKey);
      if (str.IsEmpty())
          return i != 0;
        
      ((CComboBox*) GetDlgItem(IDC_CB_FIL1))->AddString(str);

      strKey.Format(szRight, i);
      str = AfxGetApp()->GetProfileString(strSection, strKey);
      if (!str.IsEmpty())
        ((CComboBox*) GetDlgItem(IDC_CB_FIL2))->AddString(str);
    }
  }

  return false;
}

void CCmpFilDlg::GetFilter(CString& str, LPCTSTR pszKey)
{
  CComboBox *pBox = (CComboBox *) GetDlgItem(IDC_CB_MASK);

  CString strAllFiles = GetResourceString(IDS_AllFiles);
  CString strFind = strAllFiles.Left(8);

  CString strItem;
  str.Empty();
  
  CString strLastSelected = AfxGetApp()->GetProfileString(szInit, pszKey);
  if (!strLastSelected.IsEmpty())
  {
    if (strLastSelected.Find(strFind) != -1)
    {
      str += strAllFiles;
      str = str.Left(str.GetLength() - 2);
    }
    else
    {
      str += strLastSelected;
      str += '|';
      str += strLastSelected;
    }
  }

  int nLastSelected = pBox->FindStringExact(0, strLastSelected);

  for (int nIdx = 0; nIdx < pBox->GetCount(); nIdx++)
  {
    pBox->GetLBText(nIdx, strItem);
    
    if (!strItem.IsEmpty() && nIdx != nLastSelected)
    {
      if (!str.IsEmpty())
        str += '|';
      str += strItem;
      str += '|';
      str += strItem;
    }
  }

  if (str.Find(strFind) == -1)
  {
    if (!str.IsEmpty())
      str += '|';
    str += strAllFiles;
  }
  else
  {
    str += '|';
  }
}

void CCmpFilDlg::StoreLastFilter(CFileDialog& dlg, CString& strFilter, LPCTSTR pszKey)
{
  int nIdx = dlg.m_ofn.nFilterIndex - 1;

  int nCount = 0;
  for (int i = 0; i < strFilter.GetLength(); i++)
  {
    if (nCount / 2 == nIdx)
    {
      CString str;
      str = strFilter.Mid(i);
      nIdx = str.Find('|');
      if (nIdx != -1)
        str = str.Left(nIdx);
      AfxGetApp()->WriteProfileString(szInit, pszKey, str);
      return;
    }

    if (strFilter[i] == '|')
      nCount ++;
  }
}
