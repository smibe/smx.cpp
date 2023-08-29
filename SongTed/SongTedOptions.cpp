// SongTedOptions.cpp: implementation of the SongTedOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "FormEdit.h"
#include "SongTedOptions.h"
#include "Util.h"

LPCTSTR szOptions = _T("Options");

#define GetButton(i) ((CButton*) GetDlgItem(i))

CSongTedOptions::CSongTedOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CSongTedOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSongTedOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  printChords    = AfxGetApp()->GetProfileInt(szOptions, "printChords", 1) != FALSE;
  fontSizeTitle  = AfxGetApp()->GetProfileInt(szOptions, "fontSizeTitle", 12);
  fontSizeText   = AfxGetApp()->GetProfileInt(szOptions, "fontSizeText", 10);;
  fontSizeChords = AfxGetApp()->GetProfileInt(szOptions, "fontSizeChords", 6);;
  fontSizeInfo   = AfxGetApp()->GetProfileInt(szOptions, "printInfo", 6);;

}

CSongTedOptions::~CSongTedOptions()
{
  AfxGetApp()->WriteProfileInt(szOptions, "printChords", printChords);
  AfxGetApp()->WriteProfileInt(szOptions, "fontSizeTitle", fontSizeTitle);
  AfxGetApp()->WriteProfileInt(szOptions, "fontSizeText", fontSizeText);;
  AfxGetApp()->WriteProfileInt(szOptions, "fontSizeChords", fontSizeChords);;
  AfxGetApp()->WriteProfileInt(szOptions, "printInfo", fontSizeInfo);;
}


void CSongTedOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSongTedOptions)
	DDX_Control(pDX, IDC_ebFontSizeTitle, m_ebFontSizeTitle);
	DDX_Control(pDX, IDC_ebFontSizeText, m_ebFontSizeText);
	DDX_Control(pDX, IDC_ebFontSizeCords, m_ebFontSizeChords);
	DDX_Control(pDX, IDC_ebFontSizeInfo, m_ebFontSizeInfo);
	//}}AFX_DATA_MAP

  if (!pDX->m_bSaveAndValidate)
  {
    m_ebFontSizeTitle.SetDeciNumber(0);
    m_ebFontSizeChords.SetDeciNumber(0);
    m_ebFontSizeText.SetDeciNumber(0);
    m_ebFontSizeInfo.SetDeciNumber(0);

    GetButton(IDC_ckPrintChords)->SetCheck(printChords);
    m_ebFontSizeTitle.SetValue(fontSizeTitle);
    m_ebFontSizeText.SetValue(fontSizeText);
    m_ebFontSizeInfo.SetValue(fontSizeInfo);
    m_ebFontSizeChords.SetValue(fontSizeChords);
  }
  else
  {
    printChords = GetButton(IDC_ckPrintChords)->GetCheck() != 0;
    fontSizeTitle = (int) (m_ebFontSizeTitle.GetValue() + 0.5);
    fontSizeText = (int) (m_ebFontSizeText.GetValue() + 0.5);
    fontSizeInfo = (int) (m_ebFontSizeInfo.GetValue() + 0.5);
    fontSizeChords = (int) (m_ebFontSizeChords.GetValue() + 0.5);
  }
}

void CSongTedOptions::LoadFromProfile(CFormEdit *pEdit, LPCTSTR pszKey)
{
  int nValue = AfxGetApp()->GetProfileInt(szOptions, pszKey, (int) (pEdit->GetValue()));
  pEdit->SetValue(nValue / 1000.0);
}

void CSongTedOptions::StoreToProfile(CFormEdit *pEdit, LPCTSTR pszKey)
{
  AfxGetApp()->WriteProfileInt(szOptions, pszKey, (int) (pEdit->GetValue() * 1000 + 0.5));
}

void CSongTedOptions::LoadFromProfile(CButton *pButton, LPCTSTR pszKey)
{
  int nValue = AfxGetApp()->GetProfileInt(szOptions, pszKey, pButton->GetCheck());
  pButton->SetCheck(nValue != 0);
}

void CSongTedOptions::StoreToProfile(CButton *pButton, LPCTSTR pszKey)
{
  AfxGetApp()->WriteProfileInt(szOptions, pszKey, (int) (pButton->GetCheck()));
}


BEGIN_MESSAGE_MAP(CSongTedOptions, CDialog)
	//{{AFX_MSG_MAP(CSongTedOptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSongTedOptions* GetOptions()
{
  static CSongTedOptions options;
  return &options;
}
