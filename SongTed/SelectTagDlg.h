#if !defined(AFX_SELECTTAGDLG_H__38905D6B_52A7_46C9_8920_C8D9BB42EE49__INCLUDED_)
#define AFX_SELECTTAGDLG_H__38905D6B_52A7_46C9_8920_C8D9BB42EE49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectTagDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectTagDlg dialog

class CSelectTagDlg : public CDialog
{
// Construction
public:
	CSelectTagDlg(CWnd* pParent = NULL);   // standard constructor

  CString GetTag() { return m_strTag; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectTagDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectTagDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
  // Dialog Data
	//{{AFX_DATA(CSelectTagDlg)
	enum { IDD = IDD_SELECT_TAG };
	CString	m_strTag;
	//}}AFX_DATA

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTTAGDLG_H__38905D6B_52A7_46C9_8920_C8D9BB42EE49__INCLUDED_)
