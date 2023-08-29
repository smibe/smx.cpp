#if !defined(AFX_FILEPATHCTRL_H__80302ED4_FDC8_4ED1_92E0_3F8AC0C65DD4__INCLUDED_)
#define AFX_FILEPATHCTRL_H__80302ED4_FDC8_4ED1_92E0_3F8AC0C65DD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilePathCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilePathCtrl window

class CFilePathCtrl : public CStatic
{
// Construction
public:
	CFilePathCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilePathCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFilePathCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFilePathCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEPATHCTRL_H__80302ED4_FDC8_4ED1_92E0_3F8AC0C65DD4__INCLUDED_)
