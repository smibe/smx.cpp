/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: UserToolsDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/UserToolsDlg.cpp,v 1.2 2002/11/22 17:22:10 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "SMXRes.h"
#include "Util.h"
#include "UserToolsDlg.h"
#include "SelectVarDlg.h"
#include "VarMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT NOF_INTERNAL_COMMANDS  = 8;
LPCTSTR INTERNAL_COMMANDS[NOF_INTERNAL_COMMANDS] = 
{
  "copy", "move", "rd", "del", "dir", "xcopy", "move", "attrib"
};

bool IsInternalCommand(CString& strCmd)
{
  for (int i = 0; i < NOF_INTERNAL_COMMANDS; i++)
  {
    if (strCmd.CompareNoCase(INTERNAL_COMMANDS[i]) == 0)
      return true;
  }
  return false;
}
/////////////////////////////////////////////////////////////////////////////
// CUserToolsDlg dialog


CUserToolsDlg::CUserToolsDlg(CWnd* pParent /*=NULL*/)
	: CSmxDialog(CUserToolsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserToolsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_nCurrentIdx = 0;
  m_pMgr = NULL;
}


void CUserToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CSmxDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserToolsDlg)
	DDX_Control(pDX, IDC_lstUserTools, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserToolsDlg, CSmxDialog)
	//{{AFX_MSG_MAP(CUserToolsDlg)
	ON_BN_CLICKED(IDC_btnNew, OnNew)
	ON_LBN_SELCHANGE(IDC_lstUserTools, OnSelchangeUserTools)
	ON_BN_CLICKED(IDC_btnRemove, OnRemove)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_lstUserTools, OnItemChanged)
	ON_EN_KILLFOCUS(IDC_ebMenuString, OnKillfocusMenuString)
	ON_BN_CLICKED(IDC_btParameter, OnParameter)
	ON_BN_CLICKED(IDC_btInitialDir, OnInitialDir)
	ON_BN_CLICKED(IDC_ckBatchFile, OnBatchFile)
	ON_BN_CLICKED(IDC_btBatchFileName, OnBatchFileName)
	ON_BN_CLICKED(IDC_btCommand, OnCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserToolsDlg message handlers

BOOL CUserToolsDlg::OnInitDialog() 
{
	CSmxDialog::OnInitDialog();
	
	if (!m_pMgr)
  {
    ASSERT(false);
    return TRUE;
  }

  LVCOLUMN lvcol;
  CRect rcList;
  m_List.InitGrid();
  m_List.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_List.UseItemColors(true);
  m_List.GetClientRect(rcList);
  lvcol.mask = LVCF_WIDTH;
  lvcol.cx = rcList.Width();
  m_List.InsertColumn(0, &lvcol);
  for (int i = 0; i <= m_pMgr->m_UserTools.GetUpperBound(); i++)
  {
    CUserTool *pTool = m_pMgr->m_UserTools[i];
    m_List.InsertItem(m_List.GetItemCount(), pTool->m_strMenuString);
  }

  m_List.SetCurSel(0);
  m_nCurrentIdx = 0;
  LoadUserTool(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserToolsDlg::LoadUserTool(int nIdx)
{
  if (nIdx >= 0 && nIdx <= m_pMgr->m_UserTools.GetUpperBound())
  {
    CUserTool *pUserTool = m_pMgr->m_UserTools[nIdx];

    GetDlgItem(IDC_ebCommand)->SetWindowText(pUserTool->m_strCommand);
    GetDlgItem(IDC_ebParameter)->SetWindowText(pUserTool->m_strParameter);
    GetDlgItem(IDC_ebMenuString)->SetWindowText(pUserTool->m_strMenuString);
    GetDlgItem(IDC_ebInitialDir)->SetWindowText(pUserTool->m_strStartDir);
    GetDlgItem(IDC_ebDescr)->SetWindowText(pUserTool->m_strDescription);
    GetDlgItem(IDC_ebBatchFileName)->SetWindowText(pUserTool->m_strBatchFileName);
    ((CButton *) GetDlgItem(IDC_ckBatchNoExecute))->SetCheck(pUserTool->m_dwFlags & UT_BATCHNOEXECUTE);
    ((CButton *) GetDlgItem(IDC_ckBatchFile))->SetCheck(pUserTool->m_dwFlags & UT_BATCH);

    if (pUserTool->m_dwFlags & UT_MULTIPLEEXE)
    {
      ((CButton *) GetDlgItem(IDC_rdExecuteMultiple))->SetCheck(TRUE);
      ((CButton *) GetDlgItem(IDC_rdExecutOnce))->SetCheck(FALSE);
    }
    else
    {
      ((CButton *) GetDlgItem(IDC_rdExecuteMultiple))->SetCheck(FALSE);
      ((CButton *) GetDlgItem(IDC_rdExecutOnce))->SetCheck(TRUE);
    }

    OnBatchFile();
  }
  else
  {
    GetDlgItem(IDC_ebCommand)->SetWindowText(_T(""));
    GetDlgItem(IDC_ebParameter)->SetWindowText(_T(""));
    GetDlgItem(IDC_ebMenuString)->SetWindowText(_T(""));
    GetDlgItem(IDC_ebInitialDir)->SetWindowText(_T(""));
    GetDlgItem(IDC_ebDescr)->SetWindowText(_T(""));
    GetDlgItem(IDC_ebBatchFileName)->SetWindowText(_T(""));
    ((CButton *) GetDlgItem(IDC_ckBatchNoExecute))->SetCheck(FALSE);
    ((CButton *) GetDlgItem(IDC_ckBatchFile))->SetCheck(FALSE);
    ((CButton *) GetDlgItem(IDC_rdExecutOnce))->SetCheck(TRUE);
  }
}

bool CUserToolsDlg::StoreUserTool(int nIdx)
{
  if (nIdx >= 0 && nIdx <= m_pMgr->m_UserTools.GetUpperBound())
  {
    CUserTool *pUserTool = m_pMgr->m_UserTools[nIdx];
    GetDlgItem(IDC_ebCommand)->GetWindowText(pUserTool->m_strCommand);
    GetDlgItem(IDC_ebParameter)->GetWindowText(pUserTool->m_strParameter);
    GetDlgItem(IDC_ebInitialDir)->GetWindowText(pUserTool->m_strStartDir);
    GetDlgItem(IDC_ebMenuString)->GetWindowText(pUserTool->m_strMenuString);
    GetDlgItem(IDC_ebDescr)->GetWindowText(pUserTool->m_strDescription);
    GetDlgItem(IDC_ebBatchFileName)->GetWindowText(pUserTool->m_strBatchFileName);
    
    pUserTool->m_dwFlags = 0;
    if (((CButton *) GetDlgItem(IDC_ckBatchFile))->GetCheck())
      pUserTool->m_dwFlags |= UT_BATCH;
    
    if (((CButton *) GetDlgItem(IDC_ckBatchNoExecute))->GetCheck())
      pUserTool->m_dwFlags |= UT_BATCHNOEXECUTE;

    if (((CButton *) GetDlgItem(IDC_rdExecuteMultiple))->GetCheck())
      pUserTool->m_dwFlags |= UT_MULTIPLEEXE;
  }

  return true;

}


void CUserToolsDlg::OnNew() 
{
  int nIdx = -1;
  CString str = _T("NewTool");

  nIdx = m_List.InsertItem(m_List.GetItemCount(), str);
  
  if (nIdx != -1)
  {
    CUserTool *pTool = new CUserTool;
    pTool->m_strMenuString = str;
    m_pMgr->m_UserTools.InsertAt(nIdx, pTool);
    int n = m_pMgr->m_UserTools.GetUpperBound();
    pTool  = m_pMgr->m_UserTools[nIdx];
  }

  m_List.SetCurSel(nIdx);
  LoadUserTool(nIdx);
  GetDlgItem(IDC_ebMenuString)->SetFocus();
  ((CEdit *)GetDlgItem(IDC_ebMenuString))->SetSel(0, -1);
}

void CUserToolsDlg::OnSelchangeUserTools() 
{
	int     nIdx = m_List.GetCurSel();
  CString str;

  if (m_nCurrentIdx != nIdx)
  {
    if (!StoreUserTool(m_nCurrentIdx))
    {
      m_List.SetCurSel(m_nCurrentIdx);
      return;
    }

    GetDlgItem(IDC_ebMenuString)->GetWindowText(str);
    m_List.SetItemText(m_nCurrentIdx, 0, str);
  
    LoadUserTool(nIdx);
    m_nCurrentIdx = nIdx;
  }
}

void CUserToolsDlg::OnRemove() 
{
	int nIdx = m_List.GetCurSel();
  if (nIdx != -1)
  {
    if (m_pMgr->m_UserTools[nIdx])
      delete m_pMgr->m_UserTools[nIdx];

    m_pMgr->m_UserTools.RemoveAt(nIdx);
    m_List.DeleteItem(nIdx);
    if (nIdx >= m_List.GetItemCount())
      nIdx = m_List.GetItemCount() - 1;
    m_List.SetCurSel(nIdx);
  }

  if (m_List.GetItemCount() == 0)
  {
    GetDlgItem(IDC_btnRemove)->EnableWindow(FALSE);
  }
}

void CUserToolsDlg::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  UNREFERENCED_PARAMETER(pResult);
  if(pNMListView->uChanged & LVIF_STATE && pNMListView->uNewState & LVIS_SELECTED
     && !(pNMListView->uOldState & LVIS_SELECTED))
  { 
    OnSelchangeUserTools();    
  }
}

void CUserToolsDlg::OnOK()
{
	int nIdx = m_List.GetCurSel();

  if (m_List.GetItemCount() == 0)
  {
    CString strMenuText;
    GetDlgItem(IDC_ebMenuString)->GetWindowText(strMenuText);

    if (!strMenuText.IsEmpty())
    {
      //The user has entered a command but the list is empty, create one
      nIdx = m_List.InsertItem(m_List.GetItemCount(), strMenuText);
  
      if (nIdx != -1)
      {
        CUserTool *pTool = new CUserTool;
        pTool->m_strMenuString = strMenuText;
        m_pMgr->m_UserTools.InsertAt(nIdx, pTool);
      }

      m_List.SetCurSel(nIdx);
    }
  }

  if (nIdx != -1)
  {
    //Check if the menu text is unique
    if (!UpdateMenuText())
      return;

    if (!CheckCommandString())
      return;

    //Store the user tool
    if (!StoreUserTool(nIdx))
      return;
  }
  EndDialog(IDOK);
}

void CUserToolsDlg::OnCancel()
{
  OnOK();
}

BOOL CUserToolsDlg::PreTranslateMessage(MSG* pMsg) 
{
  if(pMsg->message == WM_KEYDOWN) 
  {
    switch(pMsg->wParam)
    {
      case VK_ESCAPE:
      case VK_RETURN:
        {
          CWnd *pWnd = GetFocus();
          if (pWnd == & m_List)
          {
            if (pMsg->wParam == VK_RETURN)
              OnOK();

            return TRUE;
          }
        }
        break;

      case VK_TAB:
        {
          CWnd *pWnd = GetFocus();
          if (pWnd == & m_List)
          {
            pWnd = GetNextDlgTabItem(pWnd, GetKeyState(VK_SHIFT) < 0);
            if (pWnd)
              GotoDlgCtrl(pWnd);
            return TRUE;
          }
        }
        break;

      default:
        ;
    }
  }
	
	return CSmxDialog::PreTranslateMessage(pMsg);
}

void CUserToolsDlg::OnKillfocusMenuString() 
{
  CWnd *pWnd = GetFocus();
  if (pWnd && IsChild(pWnd))
    UpdateMenuText();	
}

bool CUserToolsDlg::CheckCommandString()
{
  CString    strCmd;
  CString    strExeFile;
  GetDlgItem(IDC_ebCommand)->GetWindowText(strCmd);

  if (strCmd.IsEmpty())
  {
    AfxMessageBox("Please enter a command!");
    GotoDlgCtrl(GetDlgItem(IDC_ebCommand));
    return false;
  }

  //Check if the command is one of our defined commands using cmd.exe
  if (IsInternalCommand(strCmd))
    return true;

  HINSTANCE hInst =  FindExecutable(strCmd, NULL, strExeFile.GetBuffer(MAX_PATH));
  strExeFile.ReleaseBuffer();
  if ((UINT) hInst <= 32)
  {
    CString str;
    str.Format("Invalid Command '%s': %s.\nYou want to correct the command string?", strCmd, GetShellErrorString((DWORD) hInst));
    UINT nResult = AfxMessageBox(str, MB_YESNO);
    
    if (nResult == IDYES)
    {
      GotoDlgCtrl(GetDlgItem(IDC_ebCommand));
      return false;
    }
  }

  return true;  
}

bool CUserToolsDlg::UpdateMenuText()
{
	CString str;

  GetDlgItem(IDC_ebMenuString)->GetWindowText(str);
  int nIdx = m_List.GetCurSel();

  LVFINDINFO lvFindInfo;
  lvFindInfo.flags = LVFI_STRING;
  lvFindInfo.psz = str;
  
  int nFoundIdx = m_List.FindItem(&lvFindInfo);
  if (nFoundIdx != -1 && nFoundIdx != nIdx)
  {
    AfxMessageBox("The menu string exists! Pleas choose an other menu string.");
    GotoDlgCtrl(GetDlgItem(IDC_ebMenuString));
    return false;
  }

  if (nIdx != -1)
    m_List.SetItemText(nIdx, 0, str);
  return true;
}

void CUserToolsDlg::OnKillfocusCommand() 
{
  CWnd *pWnd = GetFocus();
  CWnd *pButton = GetDlgItem(IDC_btCommand);
  pButton = GetDlgItem(IDC_btnRemove);

  if (pWnd && IsChild(pWnd))
    CheckCommandString();	
}

void CUserToolsDlg::OnParameter() 
{
	CSelectVarDlg dlg;
  CRect rcButton;

  GetDlgItem(IDC_btParameter)->GetWindowRect(rcButton);
  
  dlg.SetTopLeft(rcButton.top, rcButton.right);
  if (dlg.DoModal() == IDOK)
  {
    CEdit *pEdit = (CEdit *) GetDlgItem(IDC_ebParameter);
    int nStart, nEnd;
    pEdit->GetSel(nStart, nEnd);
    if (nEnd == -1)
      nEnd = 0;

    pEdit->SetSel(nEnd, nEnd);

    if (dlg.m_eCmdVar >= CVarMgr::GetNofVars())
    {
      ASSERT(false);
      dlg.m_eCmdVar = (ECmdVar) 0;
    }

    CString strVar = CVarMgr::m_CmdVars[dlg.m_eCmdVar].szVar;
    pEdit->ReplaceSel(CVarMgr::m_CmdVars[dlg.m_eCmdVar].szVar, TRUE);
    pEdit->SetFocus();
  }
}

void CUserToolsDlg::OnInitialDir() 
{
	CSelectVarDlg dlg;
  CRect rcButton;

  GetDlgItem(IDC_btInitialDir)->GetWindowRect(rcButton);
  
  dlg.SetTopLeft(rcButton.top, rcButton.right);
  dlg.m_dwFlags = DIR_VAR;
  if (dlg.DoModal() == IDOK)
  {
    CEdit *pEdit = (CEdit *) GetDlgItem(IDC_ebInitialDir);
    int nStart, nEnd;
    pEdit->GetSel(nStart, nEnd);
    if (nEnd == -1)
      nEnd = 0;

    pEdit->SetSel(nEnd, nEnd);

    if (dlg.m_eCmdVar >= CVarMgr::GetNofVars())
    {
      ASSERT(false);
      dlg.m_eCmdVar = (ECmdVar) 0;
    }

    CString strVar = CVarMgr::m_CmdVars[dlg.m_eCmdVar].szVar;
    pEdit->ReplaceSel(CVarMgr::m_CmdVars[dlg.m_eCmdVar].szVar, TRUE);
    pEdit->SetFocus();
  }
	
}

void CUserToolsDlg::OnBatchFile() 
{
	BOOL bChecked = ((CButton *) GetDlgItem(IDC_ckBatchFile))->GetCheck();

  GetDlgItem(IDC_ebBatchFileName)->EnableWindow(bChecked);
  GetDlgItem(IDC_btBatchFileName)->EnableWindow(bChecked);
}

void CUserToolsDlg::OnBatchFileName() 
{
  CString strFileName;
  GetDlgItem(IDC_ebBatchFileName)->GetWindowText(strFileName);

  CFileDialog dlg(FALSE, NULL, strFileName, OFN_HIDEREADONLY, "Batch Files (*.bat)|*.bat|All Files (*.*)|*.*||");
	if(dlg.DoModal()==IDOK) 
  {
		GetDlgItem(IDC_ebBatchFileName)->SetWindowText(dlg.GetPathName());
	}
}

void CUserToolsDlg::OnCommand() 
{
  CString strFileName;
  GetDlgItem(IDC_ebCommand)->GetWindowText(strFileName);
  CString strExeFile;

  HINSTANCE hInst =  FindExecutable(strFileName, NULL, strExeFile.GetBuffer(MAX_PATH));
  strExeFile.ReleaseBuffer();

  if ((UINT) hInst > 32)
    strFileName = strExeFile;

  CFileDialog dlg(FALSE, NULL, strFileName, OFN_HIDEREADONLY, "Exe Files (*.exe)|*.exe|All Files (*.*)|*.*||");
	if(dlg.DoModal()==IDOK) 
  {
		GetDlgItem(IDC_ebCommand)->SetWindowText(dlg.GetPathName());
	}
	
}
