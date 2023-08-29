// SongTedOptions.h: interface for the SongTedOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SONGTEDOPTIONS_H__9B7A047D_6F9E_4426_982E_E794C8EC7083__INCLUDED_)
#define AFX_SONGTEDOPTIONS_H__9B7A047D_6F9E_4426_982E_E794C8EC7083__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSongTedOptions : public CDialog
{
// Construction
public:
	CSongTedOptions(CWnd* pParent = NULL);   // standard constructor
  ~CSongTedOptions();

// Dialog Data
	//{{AFX_DATA(CSongTedOptions)
	enum { IDD = IDD_SongTedOptions };
	CEdit	m_FontSizeCord;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongTedOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSongTedOptions)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


  void LoadFromProfile(CFormEdit *pEdit, LPCTSTR pszKey);
  void StoreToProfile(CFormEdit *pEdit, LPCTSTR pszKey);
  void LoadFromProfile(CButton *pButton, LPCTSTR pszKey);
  void StoreToProfile(CButton *pButton, LPCTSTR pszKey);

public:
  bool printChords;
  int  fontSizeTitle;
  int  fontSizeText;
  int  fontSizeChords;
  int  fontSizeInfo;

	CFormEdit	m_ebFontSizeTitle;
	CFormEdit	m_ebFontSizeText;
	CFormEdit	m_ebFontSizeChords;
	CFormEdit	m_ebFontSizeInfo;
};

CSongTedOptions* GetOptions();

#endif
