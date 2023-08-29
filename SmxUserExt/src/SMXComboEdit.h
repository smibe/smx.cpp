#if !defined(AFX_SMXCOMBOEDIT_H__CF8E969F_54D6_464A_B472_65DD87A098CA__INCLUDED_)
#define AFX_SMXCOMBOEDIT_H__CF8E969F_54D6_464A_B472_65DD87A098CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SMXComboEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSMXComboEdit window

#define IDC_COMBOEDIT 1001

class CSMXComboEdit : public CEdit
{
// Construction
public:
	CSMXComboEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXComboEdit)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSMXComboEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSMXComboEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMXCOMBOEDIT_H__CF8E969F_54D6_464A_B472_65DD87A098CA__INCLUDED_)
