// SMXSheet.h: interface for the CSMXSheet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMXSHEET_H__C60ABFC2_C042_43BF_BAB3_A887B0A27CB7__INCLUDED_)
#define AFX_SMXSHEET_H__C60ABFC2_C042_43BF_BAB3_A887B0A27CB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SMXRes.h"
class CSMXPropertyPg;
struct SMXTreeDescription
{
  UINT    nIndent;
  LPCTSTR pszItem;
  UINT    nDlgID;
};

typedef CSMXPropertyPg* (CWnd::*SMXPropertyPageFkt) (UINT);

class CSMXSheet : public CDialog  
{
public:
	CSMXSheet(CWnd* pParent = NULL);
	virtual ~CSMXSheet();

  void SetTreeDescription(SMXTreeDescription *pTreeDescr);
  void SetCreateFunction(CWnd *pWnd, SMXPropertyPageFkt pCreateFkt);

  void DisplayPage(HTREEITEM hItem);
  void SetSheetSize(UINT nWidth, UINT nHeight, bool bDialogUnits);

  //----------------- Overrides ------------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXSheet)
	public:
  virtual void OnOK();
  virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//------------------ Generated message map functions ------------------
	//{{AFX_MSG(CSMXSheet)
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
  UINT m_nDlgID;

protected:
  SMXTreeDescription *m_pTreeDescr;
  SMXPropertyPageFkt  m_pCreateFkt;
  CMap<UINT, UINT, CSMXPropertyPg*, CSMXPropertyPg*> m_PageMap;
  CWnd           *m_pWnd;
  CSMXPropertyPg *m_pPage;
  CRect           m_rcPage;
  bool            m_bDialogUnits;
  UINT            m_nInitialDlgID;
  
  //--------------- Dialog Data -----------------------
	//{{AFX_DATA(CSMXSheet)
	enum { IDD = IDD_SMX_SHEET };
	CTreeCtrl	m_Tree;
	//}}AFX_DATA
};

#endif // !defined(AFX_SMXSHEET_H__C60ABFC2_C042_43BF_BAB3_A887B0A27CB7__INCLUDED_)
