/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: ComparePg.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/ComparePg.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc


#ifndef AFX_COMPAREPG_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_
#define AFX_COMPAREPG_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_

#include "SMXPropertyPg.h"

class CComparePg : public CSMXPropertyPg
{
public:
	 ~CComparePg();
	CComparePg();

	//{{AFX_VIRTUAL(CComparePg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(CComparePg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  //{{AFX_DATA(CComparePg)
	enum { IDD = IDD_COMPARE };
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_COMPAREPG_H__6D5EE142_FAF6_11D1_9E4E_8AC1BB000000__INCLUDED_
