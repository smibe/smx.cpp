/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: ColorPg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/ColorPg.h,v 1.3 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_COLORPG_H__9ADA6E34_CD1C_11D3_85B7_204C4F4F5020__INCLUDED_)
#define AFX_COLORPG_H__9ADA6E34_CD1C_11D3_85B7_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SMXPropertyPg.h"

class CColorPg : public CSMXPropertyPg
{

public:
	CColorPg();
	~CColorPg();

  //-------------  Dialog Data ----------------------
	//{{AFX_DATA(CColorPg)
	enum { IDD = IDD_COLORS };
	CEdit	m_edRight;
	CEdit	m_edLeft;
	CEdit	m_edFolded;
	//}}AFX_DATA

  //---------------- Overrides ----------------------
	//{{AFX_VIRTUAL(CColorPg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//---------- Generated message map functions -----------------
	//{{AFX_MSG(CColorPg)
	afx_msg void OnBnColorLeft();
	afx_msg void OnBnColorRight();
	afx_msg void OnBnColorFolded();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  COLORREF m_crLeft;
	COLORREF m_crRight;
	COLORREF m_crFolded;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPG_H__9ADA6E34_CD1C_11D3_85B7_204C4F4F5020__INCLUDED_)
