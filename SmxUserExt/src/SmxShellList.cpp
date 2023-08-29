
#include "stdafx.h"
#include "SMXShellTree.h"
#include "SMXRes.h"
#include "SMXShellList.h"
#include "CJSortClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.

/////////////////////////////////////////////////////////////////////////////
// CSMXShellList

CSMXShellList::CSMXShellList()
{
	m_pTreeCtrl			= NULL;
	m_bBuildingList		= FALSE;
	m_bShowHidden		= FALSE;
}

CSMXShellList::~CSMXShellList()
{

}

IMPLEMENT_DYNAMIC(CSMXShellList, CSMXListCtrl)

BEGIN_MESSAGE_MAP(CSMXShellList, CSMXListCtrl)
	//{{AFX_MSG_MAP(CSMXShellList)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnRclick)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHELL_NOTIFY, OnShellNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMXShellList message handlers

static DATA_TYPE m_arrColType[] = 
{
	DT_STRING,		// col 0 sort type string
	DT_INT,			// col 1 sort type int
	DT_STRING,		// col 2 sort type string
	DT_DATETIME		// col 3 sort type date / time
};

bool CSMXShellList::SortList( int nCol, bool bAscending )
{
	if (nCol == 0)
  {
    CCJSortClass csc( this, nCol );
    SortItems(ListViewCompareProc, (DWORD) this);
  }
  else
  {
    CCJSortClass csc( this, nCol );
	  csc.Sort( bAscending, m_arrColType[nCol] );
  }
	return true;

}

void CSMXShellList::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POINT pt;
	LV_HITTESTINFO lvhti;
	LV_ITEM        lvi;
	LPLVITEMDATA   lplvid;  //Long pointer to ListView item data

	::GetCursorPos((LPPOINT)&pt);
	::ScreenToClient(GetSafeHwnd(), &pt);
	lvhti.pt=pt;
	HitTest(&lvhti);
	if (lvhti.flags & LVHT_ONITEM)
	{
		::ClientToScreen(GetSafeHwnd(), &pt);
		lvi.mask = LVIF_PARAM;
		lvi.iItem = lvhti.iItem;
		lvi.iSubItem = 0;
		
		if (!GetItem(&lvi))
			return;
		
		lplvid=(LPLVITEMDATA)lvi.lParam;
		
		switch (pNMListView->hdr.code)
		{
		case NM_RCLICK:
			{
				CString strFolderPath;
				m_pTreeCtrl->GetSelectedFolderPath(strFolderPath);
				if (strFolderPath.GetAt(strFolderPath.GetLength()-1) != '\\')
					strFolderPath += "\\";
				
				strFolderPath += GetItemText(pNMListView->iItem, 0);
				m_shell.DoExplorerMenu(GetSafeHwnd(), strFolderPath, pt);
				break;
			}
		case NM_DBLCLK:
			{
				CString strFolderPath;
				m_pTreeCtrl->GetSelectedFolderPath(strFolderPath);
				
				if (strFolderPath.GetAt(strFolderPath.GetLength()-1) != '\\')
					strFolderPath += "\\";
				
				strFolderPath += GetItemText(pNMListView->iItem, 0);

                if ((lplvid->ulAttribs & SFGAO_FOLDER) == SFGAO_FOLDER) {
					m_pTreeCtrl->TunnelTree(strFolderPath);
				}
				else {
					m_shell.ShellExec(strFolderPath, SW_SHOW);
				}
			}
			break;
		}
	}
	
	*pResult = 0;
}

void CSMXShellList::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	HRESULT        hr;
	LPMALLOC       lpMalloc;
	LPLVITEMDATA   lplvid = (LPLVITEMDATA)pNMListView->lParam;
	
	if (lplvid)
	{
		//Let's free the memory for the ListView item data...
		hr=SHGetMalloc(&lpMalloc);
		if (FAILED(hr)) {
			return;
		}

		lplvid->pParent->Release();
		lpMalloc->Free(lplvid->pIDL);  
		lpMalloc->Free(lplvid);  
		lpMalloc->Release();
	}

	*pResult = 0;
}

void CSMXShellList::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LPLVITEMDATA   lplvid;  //Long pointer to ListView item data
	LPTVITEMDATA   lptvid;  //Long pointer to TreeView item data
	
	lplvid = (LPLVITEMDATA)pDispInfo->item.lParam;
	
	if ((pDispInfo) &&(pDispInfo->item.mask & LVIF_IMAGE))
	{
		ASSERT(m_pTreeCtrl); // Must have a buddy window.

		TV_ITEM tvi;
		HTREEITEM hti;
		
		hti = m_pTreeCtrl->GetSelectedItem();
		tvi.mask = TVIF_PARAM;
		tvi.hItem = hti;
		
		m_pTreeCtrl->GetItem(&tvi);
		
		lptvid=(LPTVITEMDATA)tvi.lParam;
		
		pDispInfo->item.iImage = m_shell.GetIcon(m_shell.ConcatPidls(lptvid->pIDLfq, lplvid->pIDL),
			SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
		
		if (lplvid->ulAttribs & SFGAO_LINK)
		{
			pDispInfo->item.mask |= LVIF_STATE;
			pDispInfo->item.stateMask = LVIS_OVERLAYMASK;
			pDispInfo->item.state = INDEXTOOVERLAYMASK(2);
		}
		if (pDispInfo->item.mask & LVIF_TEXT)
			m_shell.GetName(lplvid->pParent, lplvid->pIDL, SHGDN_NORMAL, pDispInfo->item.pszText);
	}
	
	*pResult = 0;
}

BOOL CSMXShellList::InitSystemImageLists()
{
    SHFILEINFO sfi;

    HIMAGELIST himlSmall = (HIMAGELIST)::SHGetFileInfo( _T("C:\\"), 0, &sfi, 
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

    HIMAGELIST himlLarge = (HIMAGELIST)::SHGetFileInfo( _T("C:\\"), 0, &sfi, 
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON );

    if( himlSmall && himlLarge ) {
        SetImageList( CImageList::FromHandle( himlSmall ), LVSIL_SMALL  );
        SetImageList( CImageList::FromHandle( himlLarge ), LVSIL_NORMAL );
		return TRUE;
    }

    return FALSE;
}

BOOL CSMXShellList::PopulateListView(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf)
{
	m_bBuildingList = TRUE;
    CWaitCursor wait;   // display wait cursor
	SetRedraw(FALSE);

	// If no image list is defined yet...
	if( GetImageList( LVSIL_NORMAL ) == NULL ) {
		if( !InitSystemImageLists( )) {
			return FALSE;
		}
	}

	// Clear the list control.
    DeleteAllItems();

	// Initialize the list control.
    if ( !InitListViewItems( lptvid, lpsf )) {
		m_bBuildingList = FALSE;
        return FALSE;
	}

	// if the list has been previously sorted, sort the
	// new contents based upon last sort.
	if( m_nSortedCol != -1 ) {
		SortList( m_nSortedCol, m_bAscending );
	}

	m_bBuildingList = FALSE;
	SetRedraw();
    return TRUE;
}

BOOL CSMXShellList::InitListViewItems(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf)
{
    int          iCtr;
    HRESULT      hr;
    LPMALLOC     lpMalloc;                                             
    LPITEMIDLIST lpifqThisItem=NULL;
    LPITEMIDLIST lpi=NULL;
    LPENUMIDLIST lpe=NULL;
    LPLVITEMDATA lplvid;
    ULONG        ulFetched, ulAttrs;
    HWND         hwnd=::GetParent(GetSafeHwnd());

    hr=SHGetMalloc(&lpMalloc);
    if (FAILED(hr))
       return FALSE;

    if (SUCCEEDED(hr))
    {
  		DWORD dwFlags = SHCONTF_FOLDERS | SHCONTF_NONFOLDERS;
	  	if( m_bShowHidden )
		  	dwFlags |= SHCONTF_INCLUDEHIDDEN;

      hr=lpsf->EnumObjects( hwnd, dwFlags, &lpe );

      if (SUCCEEDED(hr))
      {
        iCtr = 0;

        while (S_OK==lpe->Next(1, &lpi, &ulFetched))
        {
            //OK, let's get some memory for our ITEMDATA struct
            lplvid = (LPLVITEMDATA)lpMalloc->Alloc(sizeof(LVITEMDATA));
            if (!lplvid)
				    {
					    if (lpe) 
              {
						    lpe->Release();
					    }
					    
              if (lpi && lpMalloc) 
              {
						    lpMalloc->Free(lpi);
					    }
					    
              if (lpifqThisItem && lpMalloc) 
              {
						    lpMalloc->Free(lpifqThisItem);
					    }
					    
              if (lpMalloc) 
              {
						    lpMalloc->Release();
					    }
					    return FALSE;
				    }

            //Note that since we are interested in the display attributes as well as
            //the other attributes, we need to set ulAttrs to SFGAO_DISPLAYATTRMASK
            //before calling GetAttributesOf();

            ulAttrs=SFGAO_DISPLAYATTRMASK;

            lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);
            lplvid->ulAttribs=ulAttrs;

            lpifqThisItem=m_shell.ConcatPidls(lptvid->pIDLfq, lpi);

            lplvid->pParent=lpsf;
            lpsf->AddRef();

            //Now, make a copy of the ITEMIDLIST
            lplvid->pIDL=m_shell.CopyITEMID(lpMalloc, lpi);

				    TCHAR szBuff[256];
				    if( m_shell.GetName(lpsf, lpi, SHGDN_NORMAL, szBuff) == FALSE )
				    {
					    if (lpe) {
						    lpe->Release();
					    }
					    if (lpi && lpMalloc) {
						    lpMalloc->Free(lpi);
					    }
					    if (lpifqThisItem && lpMalloc) {
						    lpMalloc->Free(lpifqThisItem);
					    }
					    if(lplvid && lpMalloc) {
						    lplvid->pParent->Release();
						    lpMalloc->Free(lplvid->pIDL);  
						    lpMalloc->Free(lplvid);  
					    }
					    if (lpMalloc) {
						    lpMalloc->Release();
					    }
					    return FALSE;
				    }

                // Add the item to the listview
				int nIndex = InsertListItem(szBuff);
				if (nIndex != -1)
					SetItemData(nIndex, (LPARAM)lplvid);

                lpMalloc->Free(lpifqThisItem);  
                lpMalloc->Free(lpi);  //Finally, free the pidl that the shell gave us...

                lpifqThisItem=0;
                lpi=0;
            }
        }
    }

    if (lpe) {
        lpe->Release();
	}
    if (lpMalloc) {
        lpMalloc->Release();
	}
 
    return TRUE;
}

int CALLBACK CSMXShellList::ListViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM /*lparamSort*/)
{
  	CSortItem* item1 = (CSortItem *) lparam1;
	  CSortItem* item2 = (CSortItem *) lparam2;
    
    LPLVITEMDATA lplvid1=(LPLVITEMDATA)item1->m_dwData;
    LPLVITEMDATA lplvid2=(LPLVITEMDATA)item2->m_dwData;
    HRESULT   hr;
	
    hr = lplvid1->pParent->CompareIDs(0,lplvid1->pIDL,lplvid2->pIDL);
    
    if (FAILED(hr))
  		return 0;
	
    if ((lplvid1->ulAttribs & SFGAO_FOLDER) && !(lplvid2->ulAttribs & SFGAO_FOLDER))
    {
      return -1;
    }

    if (!(lplvid1->ulAttribs & SFGAO_FOLDER) && (lplvid2->ulAttribs & SFGAO_FOLDER))
    {
      return 1;
    }

    return item1->m_strItemText.CompareNoCase(item2->m_strItemText);
}

void CSMXShellList::RefreshList()
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	HTREEITEM		hItem = m_pTreeCtrl->GetSelectedItem();
	HRESULT			hr;

	if( hItem != NULL )
	{
		lptvid=(LPTVITEMDATA)m_pTreeCtrl->GetItemData(hItem);

		if (lptvid && lptvid->pParent && lptvid->pIDL)
		{
			hr=lptvid->pParent->BindToObject(lptvid->pIDL,
				0,IID_IShellFolder,(LPVOID *)&lpsf2);
			
			if (SUCCEEDED(hr))
			{
				PopulateListView(lptvid,lpsf2);
			}
		}
	}
}

long CSMXShellList::OnShellNotify(WPARAM wParam, LPARAM /*lParam*/)
{
	switch (wParam)
	{
	case NM_SH_SHELLMENU:
		RefreshList();
		break;
	}

	return 0;
}

int CSMXShellList::InsertListItem(CString strFileName, BOOL bParse/*=TRUE*/)
{
	WIN32_FIND_DATA FindData;
	memset (&FindData, 0, sizeof(WIN32_FIND_DATA));
	
	CString strFolderPath;
	
	if (bParse)
	{
		m_pTreeCtrl->GetSelectedFolderPath(strFolderPath);
		
		if (strFolderPath.IsEmpty())
			return -1;

		if (strFolderPath.GetAt(strFolderPath.GetLength()-1) != '\\')
			strFolderPath += "\\";
					
		strFolderPath += strFileName;
	}
	
	else
	{
		strFolderPath = strFileName;
	}

	// First see if the original file exists.
	HANDLE hFind = FindFirstFile(strFolderPath, &FindData);

	if ( hFind == INVALID_HANDLE_VALUE ) {
		strFolderPath += _T(".pif");
		hFind = FindFirstFile(strFolderPath, &FindData);
	}

	if ( hFind != INVALID_HANDLE_VALUE )
	{
		// close the specified search handle, fixes memory leak.
		FindClose(hFind);

		CString strBuff;
		strBuff.Format(_T("%d"), FindData.nFileSizeLow);
		
		// Add the item to the listview
		int nIndex = InsertItem(GetItemCount(),
			m_shell.GetDisplayName(strFolderPath), m_shell.GetIconIndex(strFolderPath));
		
		if (nIndex == -1)
			return -1;
		
		if (strBuff.Compare(_T("0")))
		{
			SetItem(nIndex, 1, LVIF_TEXT | LVIF_STATE, m_shell.GetFileSizeString(strBuff),
				0, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, NULL);
		}
		
		strBuff = m_shell.GetFileType(strFolderPath);
		
		SetItem(nIndex, 2, LVIF_TEXT | LVIF_STATE, strBuff,
			0, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, NULL);
		CTime time( FindData.ftLastWriteTime );
		strBuff = m_shell.GetLastWriteTime( &time );
		
		SetItem(nIndex, 3, LVIF_TEXT | LVIF_STATE, strBuff,
			0, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, NULL);

		return nIndex;
	}

	return -1;
}

void CSMXShellList::BuildDefaultColumns()
{
	// build the columns used with this control.
	int nCols = 4;
	int nWidth[] = { 150, 80, 80, 80 };
	int nColString[] = { IDS_Name, IDS_Size, IDS_Type, IDS_Modified };

	BuildColumns(nCols, nWidth, nColString);
}

#endif // (_WIN32_IE >= 0x0400)