// PageSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Util.h"
#include "SmxRes.h"
#include "SmxPgSetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TCHAR szInch[]         = _T("inch");
TCHAR szTopMargin[]    = _T("top");
TCHAR szBottomMargin[] = _T("bottom");
TCHAR szLeftMargin[]   = _T("left");
TCHAR szRightMargin[]  = _T("right");
TCHAR szLandscape[]    = _T("landscape");
TCHAR szNofCols[]      = _T("nof_cols");
TCHAR szFirstCol[]     = _T("first_col");

//UINT CALLBACK _AfxCommDlgProc(HWND hWnd, UINT, WPARAM, LPARAM);
BOOL CALLBACK AfxDlgProc(HWND, UINT, WPARAM, LPARAM);
typedef UINT (CALLBACK* COMMDLGPROC)(HWND, UINT, UINT, LONG);

CSmxPgSetupDlg::CSmxPgSetupDlg(DWORD dwFlags, CWnd* pParent /*=NULL*/)
	: CPrintDialog(TRUE, dwFlags, pParent)
{
	//{{AFX_DATA_INIT(CSmxPgSetupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_pd.Flags |= PD_ENABLESETUPTEMPLATE | PD_ENABLESETUPHOOK;
  m_pd.lpSetupTemplateName = MAKEINTRESOURCE(IDD_SMX_PAGE_SETUP);
  m_pd.hInstance = AfxGetInstanceHandle();
	m_pd.lpfnSetupHook = (COMMDLGPROC)AfxDlgProc;

  m_eHeader    = SWC_enabled;
  m_eFooter    = SWC_enabled;
  m_eColumns   = SWC_hidden;
  m_bLandscape = false;
  m_ebTop.SetValue(2.5);
  m_ebBottom.SetValue(2.5);
  m_ebLeft.SetValue(2.5);
  m_ebRight.SetValue(2);

  m_ebNofCols.SetValue(1);
  m_ebFirstCol.SetValue(1);
  
  TCHAR buf[2] = {0};
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, buf, 2);
  m_bInch = buf[0] == '1';

  if (m_bInch)
  {
    m_ebTop.SetConvFact(2.54);
    m_ebBottom.SetConvFact(2.54);
    m_ebLeft.SetConvFact(2.54);
    m_ebRight.SetConvFact(2.54);
  }
}


void CSmxPgSetupDlg::DoDataExchange(CDataExchange* pDX)
{
  CPrintDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmxPgSetupDlg)
	DDX_Control(pDX, IDC_ebTop, m_ebTop);
	DDX_Control(pDX, IDC_ebRight, m_ebRight);
	DDX_Control(pDX, IDC_ebLeft, m_ebLeft);
	DDX_Control(pDX, IDC_ebBottom, m_ebBottom);
	DDX_Control(pDX, IDC_ebNofCols, m_ebNofCols);
	DDX_Control(pDX, IDC_ebFirstCol, m_ebFirstCol);
	//}}AFX_DATA_MAP

  if (!pDX->m_bSaveAndValidate)
  {
    m_ebTop.SetDeciNumber(2);
    m_ebLeft.SetDeciNumber(2);
    m_ebBottom.SetDeciNumber(2);
    m_ebRight.SetDeciNumber(2);

    m_ebNofCols.SetDeciNumber(0);
    m_ebFirstCol.SetDeciNumber(0);

    if (!m_strRegKey.IsEmpty())
    {
      LoadFromProfile();
    }

    ((CButton *) GetDlgItem(IDC_ckHeader))->SetCheck(m_bHeader);
    ((CButton *) GetDlgItem(IDC_ckFooter))->SetCheck(m_bFooter);
    ((CButton *) GetDlgItem(rad1))->SetCheck(!m_bLandscape);
    ((CButton *) GetDlgItem(rad2))->SetCheck(m_bLandscape);

  }
  else
  {
    m_bHeader = ((CButton *) GetDlgItem(IDC_ckHeader))->GetCheck() != FALSE;
    m_bFooter = ((CButton *) GetDlgItem(IDC_ckFooter))->GetCheck() != FALSE;

    if (!m_strRegKey.IsEmpty())
    {
      StoreToProfile();
    }
  }
}


BEGIN_MESSAGE_MAP(CSmxPgSetupDlg, CPrintDialog)
	//{{AFX_MSG_MAP(CSmxPgSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmxPgSetupDlg message handlers

int CSmxPgSetupDlg::DoModal() 
{
	return CPrintDialog::DoModal();
}

BOOL CSmxPgSetupDlg::OnInitDialog() 
{
	CPrintDialog::OnInitDialog();
  
  DEVMODE *pXXX = GetDevMode();

  if (m_eHeader == SWC_hidden)
    GetDlgItem(IDC_ckHeader)->ShowWindow(SW_HIDE);
  else if (m_eHeader == SWC_disabled)
  {
    ((CButton *) GetDlgItem(IDC_ckHeader))->SetCheck(FALSE);
    GetDlgItem(IDC_ckHeader)->EnableWindow(FALSE);
  }

  if (m_eFooter == SWC_hidden)
    GetDlgItem(IDC_ckFooter)->ShowWindow(SW_HIDE);
  else if (m_eFooter == SWC_disabled)
  {
    ((CButton *) GetDlgItem(IDC_ckFooter))->SetCheck(FALSE);
    GetDlgItem(IDC_ckFooter)->EnableWindow(FALSE);
  }

  if (m_eFooter == SWC_hidden && m_eHeader == SWC_hidden)
    GetDlgItem(IDC_stHeaderFooter)->ShowWindow(SW_HIDE);

  if (m_eColumns == SWC_disabled)
  {
    m_ebNofCols.EnableWindow(FALSE);
    m_ebFirstCol.EnableWindow(FALSE);
  }
  else if (m_eColumns == SWC_hidden)
  {
    m_ebNofCols.ShowWindow(SW_HIDE);
    m_ebFirstCol.ShowWindow(SW_HIDE);
    GetDlgItem(IDC_stColumns)->ShowWindow(SW_HIDE);
    
  }


  TCHAR buf[2] = {0};
  GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IMEASURE, buf, 2);
  m_bInch = buf[0] == '1';

  if (m_bInch)
  {
    GetDlgItem(IDC_stUnit1)->SetWindowText(szInch);
    GetDlgItem(IDC_stUnit2)->SetWindowText(szInch);
    GetDlgItem(IDC_stUnit3)->SetWindowText(szInch);
    GetDlgItem(IDC_stUnit4)->SetWindowText(szInch);
  }

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSmxPgSetupDlg::LoadFromProfile(CFormEdit *pEdit, LPCTSTR pszKey)
{
  int nValue = AfxGetApp()->GetProfileInt(m_strRegKey, pszKey, (int) (pEdit->GetValue() * 1000 + 0.5));
  pEdit->SetValue(nValue / 1000.0);
}

void CSmxPgSetupDlg::StoreToProfile(CFormEdit *pEdit, LPCTSTR pszKey)
{
  AfxGetApp()->WriteProfileInt(m_strRegKey, pszKey, (int) (pEdit->GetValue() * 1000 + 0.5));
}


void CSmxPgSetupDlg::StoreToProfile()
{
  StoreToProfile(&m_ebTop, szTopMargin);
  StoreToProfile(&m_ebBottom, szBottomMargin);
  StoreToProfile(&m_ebLeft, szLeftMargin);
  StoreToProfile(&m_ebRight, szRightMargin);

  if (m_ebNofCols.GetValue() < 1)
    m_ebNofCols.SetValue(1);

  if (m_ebFirstCol.GetValue() < 1)
    m_ebFirstCol.SetValue(1);

  StoreToProfile(&m_ebNofCols, szNofCols);
  StoreToProfile(&m_ebFirstCol, szFirstCol);

  if (m_pd.hDevMode)
  {
    DEVMODE *pDevMode = (DEVMODE *) GetDevMode();
    if (pDevMode->dmFields & DM_ORIENTATION)
      m_bLandscape = pDevMode->dmOrientation == DMORIENT_LANDSCAPE ? true : false;
  }
  AfxGetApp()->WriteProfileInt(m_strRegKey, szLandscape, m_bLandscape);
}

bool CSmxPgSetupDlg::LoadFromProfile()
{
  m_bLandscape = AfxGetApp()->GetProfileInt(m_strRegKey, szLandscape, 0) != 0;
  if (m_pd.hDevMode)
  {
    DEVMODE *pDevMode = (DEVMODE *) GlobalLock(m_pd.hDevMode);
    pDevMode = GetDevMode();
    if (pDevMode->dmFields & DM_ORIENTATION)
      pDevMode->dmOrientation = m_bLandscape ? DMORIENT_LANDSCAPE : DMORIENT_PORTRAIT;

    GlobalUnlock(m_pd.hDevMode);
  }

  LoadFromProfile(&m_ebTop, szTopMargin);
  LoadFromProfile(&m_ebBottom, szBottomMargin);
  LoadFromProfile(&m_ebLeft, szLeftMargin);
  LoadFromProfile(&m_ebRight, szRightMargin);
  LoadFromProfile(&m_ebNofCols, szNofCols);
  LoadFromProfile(&m_ebFirstCol, szFirstCol);

  return true;
}
