// SelectTagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SongTed.h"
#include "SelectTagDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//XMLTags
extern TCHAR szSong[];
extern TCHAR szTitle[];
extern TCHAR szAuthor[];
extern TCHAR szMusic[];
extern TCHAR szLyrics[];
extern TCHAR szStave[];
extern TCHAR szVers[];
extern TCHAR szCord[];
extern TCHAR szRefrain[];
extern TCHAR szRefrainRef[];


CSelectTagDlg::CSelectTagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTagDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectTagDlg)
	m_strTag = _T("");
	//}}AFX_DATA_INIT
}


void CSelectTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectTagDlg)
	DDX_LBString(pDX, IDC_lbTag, m_strTag);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectTagDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectTagDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectTagDlg message handlers

BOOL CSelectTagDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CListBox *pLB = (CListBox *) GetDlgItem(IDC_lbTag);
  pLB->AddString(szSong);
  pLB->AddString(szAuthor);
  pLB->AddString(szTitle);
  pLB->AddString(szStave);
  pLB->AddString(szMusic);
  pLB->AddString(szLyrics);
  pLB->AddString(szRefrain);
  pLB->AddString(szRefrainRef);
  pLB->SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
