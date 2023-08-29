/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: ComparePg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/ComparePg.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "fcu.h"
#include "FcuOptions.h"
#include "DlgDataEx.h"
#include "diffDirDoc.h"
#include "DlgDataEx.h"
#include "ComparePg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CComparePg 


CComparePg::CComparePg()
	: CSMXPropertyPg()
{
	//{{AFX_DATA_INIT(CComparePg)
	//}}AFX_DATA_INIT

}

CComparePg::~CComparePg()
{
}

void CComparePg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComparePg)
	DDX_CheckOption(pDX, IDC_CK_IGN_BLANKS1,    OMSC_IgnoreBlanks);
	DDX_CheckOption(pDX, IDC_CK_IGN_EOL,       OMSC_IgnoreEOL);
	DDX_CheckOption(pDX, IDC_CK_IGN_CASE,      OMSC_IgnoreCase);
  DDX_CheckOption(pDX, IDC_RD_CMP_SIZE_TIME, OCD_UseSizeTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComparePg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CComparePg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CComparePg::OnInitDialog() 
{
	CSMXPropertyPg::OnInitDialog();
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

