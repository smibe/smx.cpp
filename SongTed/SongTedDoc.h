// SongTedDoc.h : interface of the CSongTedDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SONGTEDDOC_H__01E2B596_EF7D_4A42_A601_CF65E807386D__INCLUDED_)
#define AFX_SONGTEDDOC_H__01E2B596_EF7D_4A42_A601_CF65E807386D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSongTedDoc : public CDocument
{
protected: // create from serialization only
	CSongTedDoc();
	DECLARE_DYNCREATE(CSongTedDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongTedDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
  virtual void OnCloseDocument();
  virtual BOOL CanCloseFrame(CFrameWnd *pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSongTedDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSongTedDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  SECEdit *m_pEdit;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONGTEDDOC_H__01E2B596_EF7D_4A42_A601_CF65E807386D__INCLUDED_)
