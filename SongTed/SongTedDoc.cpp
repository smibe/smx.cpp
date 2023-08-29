// SongTedDoc.cpp : implementation of the CSongTedDoc class
//

#include "stdafx.h"
#include "SongTed.h"

#include "SongTedDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSongTedDoc

IMPLEMENT_DYNCREATE(CSongTedDoc, CDocument)

BEGIN_MESSAGE_MAP(CSongTedDoc, CDocument)
	//{{AFX_MSG_MAP(CSongTedDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSongTedDoc construction/destruction

CSongTedDoc::CSongTedDoc()
{
	m_pEdit = NULL;

}

CSongTedDoc::~CSongTedDoc()
{
}

BOOL CSongTedDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSongTedDoc serialization

void CSongTedDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSongTedDoc diagnostics

#ifdef _DEBUG
void CSongTedDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSongTedDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSongTedDoc commands

BOOL CSongTedDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	
  if (m_pEdit)
  {
    SetModifiedFlag(m_pEdit->GetModified());
    if (!SaveModified())
		  return FALSE;
    
    m_pEdit->ReadFile(lpszPathName);
  }
	
	
	return TRUE;
}

BOOL CSongTedDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
  return m_pEdit->WriteFile(lpszPathName) == 0;
}

void CSongTedDoc::OnCloseDocument()
{
  SetModifiedFlag(m_pEdit->GetModified());
  CDocument::OnCloseDocument();
}

BOOL CSongTedDoc::CanCloseFrame(CFrameWnd *pFrame)
{
  SetModifiedFlag(m_pEdit->GetModified());
  return CDocument::CanCloseFrame(pFrame);
}
