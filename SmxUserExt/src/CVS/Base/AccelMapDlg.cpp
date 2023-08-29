/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: AccelMapDlg.cpp,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/AccelMapDlg.cpp,v 1.3 2003/02/21 08:56:52 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Thierry Maurel
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    tmaurel@caramail.com   (or tmaurel@hol.fr)
//
////////////////////////////////////////////////////////////////////////////////
// File    : AccelMapDlg.cpp
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Author : T.Maurel
// Date    : 17.08.98
//
// Remarks : implementation file
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmxRes.h"
#include "AccelMapDlg.h"

#include "CmdAccelOb.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAccelMapDlg dialog
/////////////////////////////////////////////////////////////////////////////
//
//
CAccelMapDlg::CAccelMapDlg(CAcceleratorManager* pAccelManager, 
                           CAcceleratorManager* pReadonlyAccelManager,
                           CWnd* pParent /*=NULL*/)
	: CDialog(CAccelMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccelMapDlg)
	//}}AFX_DATA_INIT

	m_pAccelManager = pAccelManager;
	m_pReadonlyAccelManager = pReadonlyAccelManager;
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccelMapDlg)
	DDX_Control(pDX, IDC_ALREADY_ASSIGNED, m_AlreadyAssigned);
	DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
	DDX_Control(pDX, IDC_EDIT_KEY,    m_Key);
	DDX_Control(pDX, IDC_CURRENTS,    m_Currents);
	DDX_Control(pDX, IDC_COMMANDS,    m_Commands);
	//}}AFX_DATA_MAP

  if (!pDX->m_bSaveAndValidate)
  {
    GetDlgItem(IDC_stView)->SetWindowText(m_pAccelManager->m_strView);
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//
BEGIN_MESSAGE_MAP(CAccelMapDlg, CDialog)
	//{{AFX_MSG_MAP(CAccelMapDlg)
	ON_BN_CLICKED(IDC_ASSIGN, OnAssign)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_LBN_SELCHANGE(IDC_COMMANDS, OnSelChangeCommands)
	ON_CBN_SELCHANGE(IDC_cbCategory, OnSelChangeCategory)
	ON_EN_CHANGE(IDC_EDIT_KEY, OnChangeEditKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAccelMapDlg message handlers
/////////////////////////////////////////////////////////////////////////////
//
//
BOOL CAccelMapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (m_pAccelManager == NULL) 
  {
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Force the dialog styles.
	ModifyStyle(0, WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU);

	// Initialize the helper
	m_helper.SetAccelManager(m_pAccelManager);
	m_helper.SetReadonlyAccelManager(m_pReadonlyAccelManager);
	m_helper.SetGUIObjects(&m_Key, &m_Currents, &m_Commands, &m_AlreadyAssigned, 
                         (CStatic *) GetDlgItem(IDC_ALREADY_ASSIGNED_LABEL), &m_Description,
                         (CComboBox*) GetDlgItem(IDC_cbCategory));
	
	// Control the number of commands available.
	if (m_pAccelManager->GetAccelTableMap().IsEmpty()) {

		((CButton*)GetDlgItem(IDC_ASSIGN))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_REMOVE))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RESET))->EnableWindow(FALSE);

		m_Key.EnableWindow(FALSE);

	} 
  else 
  {
		//Update the category combo box
    m_pAccelManager->InitAccelGroups();
    CString str;
    UINT   *pGroup;
    CComboBox *pComboBox = (CComboBox *) GetDlgItem(IDC_cbCategory);
    for (int i = 0; i < m_pAccelManager->m_aAccelGroups.GetSize(); i++)
    {
      pGroup = (UINT *) m_pAccelManager->m_aAccelGroups.GetAt(i);
      VERIFY(str.LoadString(*pGroup));
      if (!str.IsEmpty())
      {
        pComboBox->AddString(str);
      }
    }
    pComboBox->SetCurSel(0);
    int n = pComboBox->GetCurSel();
    OnSelChangeCategory();
    // Init the commands listbox
		//InitCommands();
	}

	// If the programmer has not called CreateDefaultTable()
	if (!m_pAccelManager->IsDefaultTableAvailable())
		((CButton*)GetDlgItem(IDC_RESET))->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// All of the below functions translate the call to the CAccelDlgHelper
// associated object.
/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelMapDlg::OnSelChangeCommands() 
{
	m_helper.SelChangeCommands();
	UpdateData(FALSE);
}

void CAccelMapDlg::OnSelChangeCategory() 
{
	m_helper.SelChangeGroup();
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelMapDlg::OnAssign() 
{
	UpdateData();
	m_helper.Assign();
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelMapDlg::OnRemove() 
{
	m_helper.Remove();
	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelMapDlg::OnReset() 
{
	m_helper.Reset();
  m_pAccelManager->InitAccelGroups();
}


/////////////////////////////////////////////////////////////////////////////
//
//
void CAccelMapDlg::InitCommands()
{
	m_helper.InitCommands();
}


void CAccelMapDlg::OnChangeEditKey() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
  WORD wVirtKey;
  bool bCtrl, bAlt, bShift;
	if (m_Key.GetAccelKey(wVirtKey, bCtrl, bAlt, bShift))
  {
    m_helper.UpdateAffected();
  }
}
