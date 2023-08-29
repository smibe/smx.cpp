/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: ColorPg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/ColorPg.cpp,v 1.3 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "Util.h"
#include "fcuOptions.h"
#include "colorpg.h"
#include "DiffEditCtrl.h"
#include "SMXPropertyPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CColorPg::CColorPg() : CSMXPropertyPg()
{
	//{{AFX_DATA_INIT(CColorPg)
	//}}AFX_DATA_INIT
}

CColorPg::~CColorPg()
{
}

void CColorPg::DoDataExchange(CDataExchange* pDX)
{
	CSMXPropertyPg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPg)
	DDX_Control(pDX, IDC_ED_RIGHT, m_edRight);
	DDX_Control(pDX, IDC_ED_LEFT, m_edLeft);
	DDX_Control(pDX, IDC_ED_FOLDED, m_edFolded);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate)
	{
		GetOptions().SetEditColorLeft(m_crLeft);
		GetOptions().SetEditColorRight(m_crRight);
		GetOptions().SetEditColorFolded(m_crFolded);
	}
	else
	{
		m_crLeft  = GetOptions().GetEditColorLeft();
		m_crRight = GetOptions().GetEditColorRight();
		m_crFolded = GetOptions().GetEditColorFolded();

		
    m_edLeft.SetWindowText(GetResourceString(IDS_LeftDifferentSection));
		m_edRight.SetWindowText(GetResourceString(IDS_RightDifferentSection));
		m_edFolded.SetWindowText(GetResourceString(IDS_FoldedLineMarker));
	}
}


BEGIN_MESSAGE_MAP(CColorPg, CSMXPropertyPg)
	//{{AFX_MSG_MAP(CColorPg)
	ON_BN_CLICKED(IDC_BN_COLOR_LEFT, OnBnColorLeft)
	ON_BN_CLICKED(IDC_BN_COLOR_RIGHT, OnBnColorRight)
	ON_BN_CLICKED(IDC_BN_COLOR_FOLDED, OnBnColorFolded)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColorPg::OnBnColorLeft() 
{
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		m_crLeft = dlg.GetColor();
	}

	m_edLeft.Invalidate();
}

void CColorPg::OnBnColorRight() 
{
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		m_crRight = dlg.GetColor();
	}
	
	m_edRight.Invalidate();
}

void CColorPg::OnBnColorFolded() 
{
	CColorDialog dlg;
	if(dlg.DoModal() == IDOK)
	{
		m_crFolded = dlg.GetColor();
	}
	
	m_edFolded.Invalidate();
}

HBRUSH CColorPg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSMXPropertyPg::OnCtlColor(pDC, pWnd, nCtlColor);
	static CBrush bgBrush;
	
	if(bgBrush.GetSafeHandle() != NULL)
	{
		bgBrush.DeleteObject();
	}

	if(nCtlColor == CTLCOLOR_EDIT && pWnd == &m_edLeft)
	{
		bgBrush.CreateSolidBrush(m_crLeft);
		pDC->SetBkColor(m_crLeft);
		return (HBRUSH) bgBrush;
	}

	if(nCtlColor == CTLCOLOR_EDIT && pWnd == &m_edRight)
	{
		bgBrush.CreateSolidBrush(m_crRight);
		pDC->SetBkColor(m_crRight);
		return (HBRUSH) bgBrush;
	}

	if(nCtlColor == CTLCOLOR_EDIT && pWnd == &m_edFolded)
	{
		bgBrush.CreateSolidBrush(m_crFolded);
		pDC->SetBkColor(m_crFolded);
		return (HBRUSH) bgBrush;
	}

	return hbr;
}
