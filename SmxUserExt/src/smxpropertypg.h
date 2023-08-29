// SMXSheet.h: interface for the CSMXSheet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMXPROPERTY_H__C60ABFC2_C042_43BF_BAB3_A887B0A27CB7__INCLUDED_)
#define AFX_SMXPROPERTY_H__C60ABFC2_C042_43BF_BAB3_A887B0A27CB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmxDialog.h"
class CSMXSheet;

class CSMXPropertyPg : public CSmxDialog  
{
public:
  CSMXPropertyPg() ;
	virtual ~CSMXPropertyPg();

  BOOL Create(UINT nDlgID, CSMXSheet *pSheet);
  void SetNextDlgTabItem(bool bPrevious);

  UINT GetID() { return m_nID; }

  //----------------- Overrides ------------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXPropertyPg)
	public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void OnOK();
  virtual void OnCancel();
	//}}AFX_VIRTUAL

	//------------------ Generated message map functions ------------------
	//{{AFX_MSG(CSMXPropertyPg)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
  afx_msg BOOL ShowTooltip(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	DECLARE_MESSAGE_MAP()

  BOOL HandleHotKey(UINT nKey);
protected:
  UINT       m_nID;
  CSMXSheet *m_pSheet;
  DWORD      m_dwTimer;
  CPoint     m_ptCursor;
  int        m_nToolTipRequest;

  //--------------- Dialog Data -----------------------
	//{{AFX_DATA(CSMXPropertyPg)
	//}}AFX_DATA
};

#endif // !defined(AFX_SMXPROPERTY_H__C60ABFC2_C042_43BF_BAB3_A887B0A27CB7__INCLUDED_)
