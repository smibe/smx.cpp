#if !defined(AFX_SONGTEDCTRL_H__38D6400A_6CF9_4533_B9B7_79FDA373243C__INCLUDED_)
#define AFX_SONGTEDCTRL_H__38D6400A_6CF9_4533_B9B7_79FDA373243C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SongTedCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSongTedCtrl window

class CSongTedCtrl : public SECEditCtrl
{
// Construction
public:
	CSongTedCtrl();

  void InsertTag(LPCTSTR pszTag);
  void InsertChordsBetween();
  void UpdateChordsAbove();

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongTedCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSongTedCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSongTedCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
  TCHAR m_cStartChord;
  TCHAR m_cEndChord;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONGTEDCTRL_H__38D6400A_6CF9_4533_B9B7_79FDA373243C__INCLUDED_)
