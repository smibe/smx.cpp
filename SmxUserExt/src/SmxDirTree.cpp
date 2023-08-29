// SmxDirTree.cpp : implementation file
//

#include "stdafx.h"
#include "smxres.h"
#include "Path.h"
#include "Dir.h"
#include "DirList.h"
#include "SmxDirTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmxDirTree

CSmxDirTree::CSmxDirTree()
{
  SHFILEINFO sfi;

  HIMAGELIST hImageList = (HIMAGELIST)::SHGetFileInfo( _T("C:\\"), 0, &sfi, 
	sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

  if( hImageList ) {
      SetImageList( CImageList::FromHandle( hImageList ), TVSIL_NORMAL  );
}

CSmxDirTree::~CSmxDirTree()
{
}

bool CSmxDirTree::Load(HTREEITEM hItem, LPCTSTR pszDir)
{
  if (hItem == NULL)
    hItem = TVI_ROOT;

  HTREEITEM hChild = 0;
  for (hChild = GetNextItem(hItem, TVGN_CHILD); hChild; hChild = GetNextItem(hItem, TVGN_NEXT))
    DeleteItem(hChild);

  if (GetItemText(hItem).IsEmpty())
    SetItemText(hItem, pszDir);

  COXPathSpec Path(pszDir);
  COXDirList  DirList;
  DirList.SetPath(Path);
  DirList.Search();

  for (int i = 0; i < DirList.GetList()->GetSize(); i++)
  {
    CString strDir = ((COXDirSpec *)DirList.GetList()->GetAt(i))->GetDirName();
    InsertItem(strDir, hItem);
  }
  return true;
}

BEGIN_MESSAGE_MAP(CSmxDirTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CSmxDirTree)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmxDirTree message handlers

void CSmxDirTree::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  	
	*pResult = 0;
}
