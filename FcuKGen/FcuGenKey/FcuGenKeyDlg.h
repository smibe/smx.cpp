// FcuGenKeyDlg.h : header file
//

#if !defined(AFX_FCUGENKEYDLG_H__558211A3_9F9D_41AA_851B_F4B97C26D121__INCLUDED_)
#define AFX_FCUGENKEYDLG_H__558211A3_9F9D_41AA_851B_F4B97C26D121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyDlg dialog

class CFcuGenKeyDlg : public CDialog
{
// Construction
public:
	CFcuGenKeyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFcuGenKeyDlg)
	enum { IDD = IDD_FCUGENKEY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFcuGenKeyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

  int GetMatchedIdx(LPCTSTR pszKey, CStringArray& strArray);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFcuGenKeyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGenerate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCUGENKEYDLG_H__558211A3_9F9D_41AA_851B_F4B97C26D121__INCLUDED_)
