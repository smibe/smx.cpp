// FcuGenKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FcuGenKey.h"
#include "FcuGenKeyDlg.h"
#include "FcuKGen.h"
#include "..\..\SmxUserExt\src\Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyDlg dialog

CFcuGenKeyDlg::CFcuGenKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFcuGenKeyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFcuGenKeyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFcuGenKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFcuGenKeyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFcuGenKeyDlg, CDialog)
	//{{AFX_MSG_MAP(CFcuGenKeyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_btnGenerate, OnGenerate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFcuGenKeyDlg message handlers

BOOL CFcuGenKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CString str = AfxGetApp()->GetProfileString("init", "InFile");
  GetDlgItem(IDC_ebInFile)->SetWindowText(str);
	
  str = AfxGetApp()->GetProfileString("init", "OutFile");
  GetDlgItem(IDC_ebOutFile)->SetWindowText(str);

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFcuGenKeyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CFcuGenKeyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFcuGenKeyDlg::OnGenerate() 
{
	CString strInFile;
  GetDlgItem(IDC_ebInFile)->GetWindowText(strInFile);
	CString strOutFile;
  GetDlgItem(IDC_ebOutFile)->GetWindowText(strOutFile);

	AfxGetApp()->WriteProfileString("init", "InFile", strInFile);
  AfxGetApp()->WriteProfileString("init", "OutFile", strOutFile);

  CStdioFile InFile;
  CStdioFile OutFile;
  CString strLine;

  if (!InFile.Open(strInFile, CFile::modeRead))
  {
    AfxMessageBox("Couldn't open the input file!");
    return;
  }

  if (!OutFile.Open(strOutFile, CFile::modeCreate | CFile::modeWrite))
  {
    AfxMessageBox("Couldn't open the output file!");
    InFile.Close();
    return;
  }

  CStringArray strArray;
  CStringArray strOutArray;
  CString str;

  UINT Idx[5];
  int nLine = 0;
  strOutArray.SetSize(5);

  CFCUKeyGen Gen;
  while (InFile.ReadString(strLine))
  {
    SplitString(strLine, strArray, ';');

    if (strLine.GetLength() < 4)
      continue;

    if (nLine == 0)
    {
      Idx[0] = GetMatchedIdx("FIRSTNAME", strArray);
      Idx[1] = GetMatchedIdx("LASTNAME", strArray);
      Idx[2] = GetMatchedIdx("EMAIL", strArray);
      Idx[3] = GetMatchedIdx("REG_NAME", strArray);

      nLine++;
      continue;
    }

    strOutArray[0].Format("%s %s", strArray[Idx[0]], strArray[Idx[1]]);
    strOutArray[1] = strArray[Idx[2]];
    strOutArray[2] = strOutArray[0];

    strOutArray[3] = strArray[Idx[3]];

    str = strOutArray[3];
    strOutArray[4] = Gen.GetKey(strOutArray[3]);

    CatStrings(str, strOutArray, ';');
    OutFile.WriteString(str);
    OutFile.WriteString("\n");

    nLine++;
  }
  
  InFile.Close();
  OutFile.Close();
	
}

int CFcuGenKeyDlg::GetMatchedIdx(LPCTSTR pszKey, CStringArray& strArray)
{
  for (int i = 0; i < strArray.GetSize(); i++)
  {
    if (strArray[i].CompareNoCase(pszKey) == 0)
      return i;
  }

  ASSERT(false);
  return -1;
}
