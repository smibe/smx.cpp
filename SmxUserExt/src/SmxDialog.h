// SmxDialog.h: interface for the CSmxDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMXDIALOG_H__25800AD6_1D1E_4FED_92F4_7C25648A04E1__INCLUDED_)
#define AFX_SMXDIALOG_H__25800AD6_1D1E_4FED_92F4_7C25648A04E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSmxDialog : public CDialog  
{
public:
	CSmxDialog();
  CSmxDialog(UINT nID, CWnd* pParent = NULL);
	virtual ~CSmxDialog();

  //----------------- Overrides ------------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXDialog)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//------------------ Generated message map functions ------------------
	//{{AFX_MSG(CSMXDialog)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
  afx_msg BOOL ShowTooltip(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()

protected:
  DWORD      m_dwTimer;
  CPoint     m_ptCursor;
  UINT       m_nToolTipCtrlID;
  int        m_nToolTipRequest;
  static int       sm_nCursorHeight;

  //--------------- Dialog Data -----------------------
	//{{AFX_DATA(CSMXPropertyPg)
	//}}AFX_DATA
};

#endif // !defined(AFX_SMXDIALOG_H__25800AD6_1D1E_4FED_92F4_7C25648A04E1__INCLUDED_)
