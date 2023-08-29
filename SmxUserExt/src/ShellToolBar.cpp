// ShellToolBar.cpp: implementation of the CShellToolBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShellToolBar.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShellToolBar::CShellToolBar()
{
  m_pBitmap    = NULL;
  m_pDriveInfo = NULL;
  
  m_nFirstID = 40000;  
  m_nLastID  = 40100;  
}

CShellToolBar::~CShellToolBar()
{
  delete m_pBitmap;
  delete [] m_pDriveInfo;
}

void CShellToolBar::SetIDRange(UINT nFirstID, UINT nLastID)
{
  m_nFirstID = nFirstID;
  m_nLastID  = nLastID;
}

bool CShellToolBar::Initialize()
{
  LPITEMIDLIST  pDrives        = NULL;
  LPITEMIDLIST  pDrive         = NULL;
  LPITEMIDLIST  pFullDrive     = NULL;
  LPSHELLFOLDER pDesktop       = NULL;
  LPSHELLFOLDER pDriveFolder = NULL;
  
  ::SHGetDesktopFolder(&pDesktop);
  ::SHGetSpecialFolderLocation(GetSafeHwnd(), CSIDL_DRIVES, &pDrives);
  pDesktop->BindToObject(pDrives, 0, IID_IShellFolder,(LPVOID *)&pDriveFolder);

  LPENUMIDLIST pEnum;
  pDriveFolder->EnumObjects(GetSafeHwnd(), OLECONTF_ONLYIFRUNNING  | SHCONTF_FOLDERS, &pEnum);
  
  ULONG           ulFetched;
  int nCount = 0;
  CString str;

  while (S_OK==pEnum->Next(1, &pDrive, &ulFetched))
  {
    Sleep(1);
    nCount++;

    m_Shell.GetName(pDriveFolder, pDrive, SHGDN_NORMAL, str.GetBuffer(MAX_PATH));
    str.ReleaseBuffer();
    m_Shell.Free(pDrive);
  }

  pEnum->Reset();
  pEnum->Release();
  pDriveFolder->EnumObjects(GetSafeHwnd(), SHCONTF_FOLDERS, &pEnum);
  delete m_pBitmap;
  m_pBitmap = new CBitmap;
  m_pDriveInfo = new SDriveInfo[nCount];

  CDC dc;
  dc.CreateCompatibleDC(GetToolBarCtrl().GetDC());
  CBitmap *pPrev = NULL;

  CRect rcButton(0, 0, 30, 16);
  SHFILEINFO  FileInfo;
  HIMAGELIST hImgList = NULL;
  HICON hIcon = 0;

  m_pBitmap->CreateCompatibleBitmap(GetToolBarCtrl().GetDC(), rcButton.Width() * nCount, rcButton.Height());
  pPrev = dc.SelectObject(m_pBitmap);
  dc.FillSolidRect(0, 0,rcButton.Width() * nCount, rcButton.Height(), GetSysColor(COLOR_BTNFACE));
  dc.SetBkColor(GetSysColor(COLOR_BTNFACE));

  int nDrive = 0;
  CString *pStr = NULL;
  for (int i = 0; i < nCount; i++)
  {

    pEnum->Next(1, &pDrive, &ulFetched);
    
    pStr = &m_pDriveInfo[nDrive].strDisplayName;
    m_Shell.GetName(pDriveFolder, pDrive, SHGDN_NORMAL, pStr->GetBuffer(MAX_PATH));
    pStr->ReleaseBuffer();
    
    char c[2] = {0};
    int nIdx = pStr->ReverseFind(':');
    if (nIdx >= 1)
      c[0] = pStr->GetAt(nIdx-1);

    FileInfo.hIcon = 0;
    if (nIdx != -1)
    {
      pStr = &m_pDriveInfo[nDrive].strPath;
      *pStr = c[0];
      *pStr += ':';
      *pStr += '\\';
      hImgList = (HIMAGELIST)::SHGetFileInfo(*pStr, 0, &FileInfo, 
                            sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON);
    }
    else
    {
      continue;
    }

    m_Shell.Free(pDrive);

    if (FileInfo.hIcon)
    {
      ICONINFO IconInfo;
      GetIconInfo(FileInfo.hIcon, &IconInfo);
      BITMAP Bitmap;
      CBitmap::FromHandle(IconInfo.hbmColor)->GetBitmap(&Bitmap);
    
      DrawIconEx(dc.GetSafeHdc(), rcButton.left, 0, FileInfo.hIcon, Bitmap.bmWidth, Bitmap.bmHeight, 0, 0, DI_COMPAT | DI_NORMAL);      //dc.DrawIcon(rcButton.left, 0, AfxGetApp()->LoadStandardIcon(IDI_HAND));
    }
    
    if (c[0])
      dc.DrawText(c, rcButton, DT_RIGHT );
  
    int nWidth = rcButton.Width();
    rcButton.left += nWidth;
    rcButton.right += nWidth;
    nDrive++;
  }

  dc.SelectObject(pPrev);

  TBBUTTON tButton;
  memset(&tButton, 0, sizeof(tButton));

  GetToolBarCtrl().SetButtonSize(CSize(rcButton.Width(), rcButton.Height()));
  GetToolBarCtrl().SetBitmapSize(CSize(rcButton.Width(), rcButton.Height()));

  int nBmp = GetToolBarCtrl().AddBitmap(nCount, m_pBitmap);
  int nID = 1;
  
  if ((UINT) nCount > m_nLastID - m_nFirstID)
  {
    ASSERT(false);
    return false;
  }

  for (i = 0; i < nDrive; i++)
  {
    tButton.idCommand = m_nFirstID + i;
    tButton.fsState   = TBSTATE_ENABLED;
    tButton.fsStyle   = TBSTYLE_BUTTON;
    tButton.iBitmap   = nBmp + i;
    tButton.dwData    = (DWORD) &m_pDriveInfo[i];
    nID = GetToolBarCtrl().AddButtons(1, &tButton);
  }
  
  return true;
}

