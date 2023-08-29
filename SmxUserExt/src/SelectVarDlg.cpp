/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SelectVarDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SelectVarDlg.cpp,v 1.2 2002/11/22 17:24:15 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "SMXRes.h"
#include "selectvardlg.h"
#include "VarMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectVarDlg dialog


CSelectVarDlg::CSelectVarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectVarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectVarDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_top = m_left = 0;
  m_dwFlags = 0;
}


void CSelectVarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectVarDlg)
	DDX_Control(pDX, IDC_lstSelectVar, m_List);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    int nIdx = m_List.GetCurSel();

    if (nIdx != -1)
      m_eCmdVar = (ECmdVar) m_List.GetItemData(nIdx);
  }
}


BEGIN_MESSAGE_MAP(CSelectVarDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectVarDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectVarDlg message handlers

BOOL CSelectVarDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  LVCOLUMN lvcol;
  CRect rcList;
  m_List.InitGrid();
  m_List.SetBkColor(GetSysColor(COLOR_WINDOW));
	m_List.UseItemColors(true);
  m_List.GetClientRect(rcList);
  lvcol.mask = LVCF_WIDTH;
  lvcol.cx = rcList.Width();
  m_List.InsertColumn(0, &lvcol);

  int nItem; 
  for (int i = 0; i < CVarMgr::GetNofVars(); i++)
  {
    if (m_dwFlags)
    {
      //Show only variables with the given flags set
      if ((CVarMgr::m_CmdVars[i].dwFlags & m_dwFlags) != m_dwFlags)
        continue;
    }

    nItem = m_List.InsertItem(m_List.GetItemCount(), CVarMgr::m_CmdVars[i].szDescription);
    m_List.SetItemData(nItem, i);
  }

  m_List.GetClientRect(rcList);
  m_List.SetColumnWidth(0, rcList.Width());

  m_List.SetCurSel(0);
  m_List.SetCurFocus(0);

  if (m_top != 0 || m_left != 0)
  {
    CRect rcWnd;
    GetWindowRect(rcWnd);

    CPoint pt(m_left - rcWnd.left, m_top - rcWnd.top);
    rcWnd += pt;
    MoveWindow(rcWnd);
  }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSelectVarDlg::PreTranslateMessage(MSG* pMsg) 
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
            if (pMsg->wParam == VK_ESCAPE)
              OnCancel();
            else
              OnOK();

            return TRUE;
          }
        }
        break;
    }
  }
	
	return CDialog::PreTranslateMessage(pMsg);
}
