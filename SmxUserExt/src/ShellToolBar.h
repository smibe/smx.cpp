// ShellToolBar.h: interface for the CShellToolBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLTOOLBAR_H__FAAEEBAC_F7A9_4BB6_A26B_5B5B1F77B457__INCLUDED_)
#define AFX_SHELLTOOLBAR_H__FAAEEBAC_F7A9_4BB6_A26B_5B5B1F77B457__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolbarEx.h"
#include "SMXShell.h"

struct SDriveInfo
{
  CString strPath;
  CString strDisplayName;
};

class CShellToolBar : public CToolBarEx  
{
public:
	CShellToolBar();
	virtual ~CShellToolBar();

  bool Initialize();

  void SetIDRange(UINT nFirstID, UINT nLastID);

protected:
  CBitmap   *m_pBitmap;
  CSMXShell m_Shell;
  UINT m_nFirstID;
  UINT m_nLastID;
  SDriveInfo *m_pDriveInfo;
};

#endif // !defined(AFX_SHELLTOOLBAR_H__FAAEEBAC_F7A9_4BB6_A26B_5B5B1F77B457__INCLUDED_)
