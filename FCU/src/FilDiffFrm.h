/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffFrm.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffFrm.h,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_FILDIFFFRM_H__08768DA6_AE65_11D3_8598_204C4F4F5020__INCLUDED_)
#define AFX_FILDIFFFRM_H__08768DA6_AE65_11D3_8598_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MDIChildWndEx.h"

class CAcceleratorManager;

class CFilDiffFrm : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CFilDiffFrm)

public:
  static CAcceleratorManager *GetAccelMgr();
  static void DeleteAccelMgr();

  //-------------- Overrides ------------------
	//{{AFX_VIRTUAL(CFilDiffFrm)
	//}}AFX_VIRTUAL

protected:
	CFilDiffFrm();
	virtual ~CFilDiffFrm();

  LPCTSTR GetToolbarRegKeyName() { return _T("CmpFilToolBar"); }
  virtual bool IsButtonInitialyHidden(UINT nCmdID);


	//---------------- Message map functions -----------------
  //{{AFX_MSG(CFilDiffFrm)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  static CAcceleratorManager* m_pAccelMgr;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILDIFFFRM_H__08768DA6_AE65_11D3_8598_204C4F4F5020__INCLUDED_)
