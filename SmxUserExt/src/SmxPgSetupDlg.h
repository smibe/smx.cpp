#if !defined(AFX_PAGESETUPDLG_H__7AF37B44_9DC5_4947_BD81_84482EE2C018__INCLUDED_)
#define AFX_PAGESETUPDLG_H__7AF37B44_9DC5_4947_BD81_84482EE2C018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSetupDlg.h : header file
//

#include "FormEdit.h"

enum EShowCtrl;

class CSmxPgSetupDlg : public CPrintDialog
{
// Construction
public:
	CSmxPgSetupDlg(DWORD dwFlags, CWnd* pParent = NULL);   // standard constructor

  void StoreToProfile();
  bool LoadFromProfile();

  void StoreToProfile(CFormEdit *pEdit, LPCTSTR pszKey);
  void LoadFromProfile(CFormEdit *pEdit, LPCTSTR pszKey);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmxPgSetupDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSmxPgSetupDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	EShowCtrl m_eFooter;
  EShowCtrl m_eHeader;
	EShowCtrl m_eColumns;
  bool      m_bFooter;
  bool      m_bHeader;
  bool      m_bInch;
  bool      m_bLandscape;
  CString   m_strRegKey;

  //{{AFX_DATA(CSmxPgSetupDlg)
	enum { IDD = IDD_SMX_PAGE_SETUP };
	CFormEdit	m_ebTop;
	CFormEdit	m_ebRight;
	CFormEdit	m_ebLeft;
	CFormEdit	m_ebBottom;
	CFormEdit	m_ebNofCols;
	CFormEdit	m_ebFirstCol;
	//}}AFX_DATA

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETUPDLG_H__7AF37B44_9DC5_4947_BD81_84482EE2C018__INCLUDED_)
