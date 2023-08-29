/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SelectFilesDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/SelectFilesDlg.cpp,v 1.1 2002/09/20 09:34:48 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// SelectFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fcu.h"
#include "SelectFilesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectFilesDlg dialog


CSelectFilesDlg::CSelectFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectFilesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectFilesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectFilesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectFilesDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectFilesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectFilesDlg message handlers
