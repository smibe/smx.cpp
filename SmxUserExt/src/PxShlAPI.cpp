/*
 * Copyright (c) 1998,1999 Vassili Bourdo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice in the documentation and/or other materials provided with 
 *    the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "stdafx.h"
#include "shobjidl_core.h"
#include "PxShlAPI.h"
_COM_SMARTPTR_TYPEDEF(IContextMenu, __uuidof(IContextMenu));
_COM_SMARTPTR_TYPEDEF(IContextMenu2, __uuidof(IContextMenu2));


#ifdef _UNICODE
	#define IShellLinkPtr	IShellLinkWPtr	
	#define IExtractIconPtr	IExtractIconWPtr
#else
	#define IShellLinkPtr	IShellLinkAPtr	
	#define IExtractIconPtr	IExtractIconAPtr
#endif

BOOL ExecuteContextCommand(LPCTSTR pszCommand, LPCITEMIDLIST pidlShellItem, HWND hWnd);

LPITEMIDLIST GetItemIDFromPath(LPCTSTR pszPath)
{
	if( !(bool)GetDesktopFolder() )
		return NULL;

	HRESULT       hr;
	LPITEMIDLIST  pidl;
	ULONG         chEaten;
	ULONG         dwAttributes;
	OLECHAR		  olePath[_MAX_PATH];
	
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
		pszPath, -1, olePath, sizeof(olePath));
	
	hr = GetDesktopFolder()->ParseDisplayName(
		NULL,NULL,olePath,&chEaten,&pidl,&dwAttributes);
    
	if( FAILED(hr) ) 
		return NULL;

	return pidl;
}

IShellFolderPtr& GetDesktopFolder()
{
	static IShellFolderPtr g_pDesktopFolder;
	if( !(bool)g_pDesktopFolder ) {
		VERIFY(SUCCEEDED(::SHGetDesktopFolder(&g_pDesktopFolder)));
	}
	return g_pDesktopFolder;
}

IMallocPtr& GetMalloc()
{
	static IMallocPtr g_pMalloc;
	if( !(bool)g_pMalloc ) {
		VERIFY(SUCCEEDED(::SHGetMalloc(&g_pMalloc)));
	}
	return g_pMalloc;
}

// get size of PIDL - returns PIDL size (even if it's composite
// PIDL - i.e. composed from more than one PIDLs)
int GetItemIDSize(LPCITEMIDLIST pidl) 
{
	if( !pidl ) return 0;
	int nSize = 0;
	while( pidl->mkid.cb ) {
		nSize += pidl->mkid.cb;
		pidl = (LPCITEMIDLIST)(((LPBYTE)pidl) + pidl->mkid.cb);
	}
	return nSize;
}

int GetItemIDCount(LPCITEMIDLIST pidl)
{
	if( !pidl )	return 0;
	int nCount = 0;
	BYTE* pCur = (BYTE*)pidl;
	while( ((LPCITEMIDLIST)pCur)->mkid.cb ) {
		nCount ++;
		pCur += ((LPCITEMIDLIST)pCur)->mkid.cb;
	}
	return nCount;
}

int CompareItemID(LPCITEMIDLIST pidl1,LPCITEMIDLIST pidl2)
{
	if( pidl1 == pidl2 ) return 0;
	if( !pidl1 || !pidl2 ) return -1; 
	if( GetItemIDSize(pidl1) != GetItemIDSize(pidl2) ) return -1;
	return memcmp(pidl1,pidl2,GetItemIDSize(pidl1) );
}

int CompareItemID(LPCITEMIDLIST pidl1,int nSpecialFolder)
{
	CPidl pidl2(nSpecialFolder);
	return CompareItemID(pidl1, pidl2);
}

int CompareItemID(int nSpecialFolder,LPCITEMIDLIST pidl2)
{
	return CompareItemID(pidl2, nSpecialFolder);
}

LPBYTE GetItemIDPos(LPCITEMIDLIST pidl, int nPos)
{
	if( !pidl )	return 0;
	int nCount = 0;
	BYTE* pCur = (BYTE*)pidl;
	while( ((LPCITEMIDLIST)pCur)->mkid.cb ) {
		if( nCount == nPos ) return pCur;
		nCount ++;
		pCur += ((LPCITEMIDLIST)pCur)->mkid.cb;// + sizeof(pidl->mkid.cb);
	}
	if( nCount == nPos ) return pCur;
	return NULL;
}

LPITEMIDLIST CopyItemID(LPCITEMIDLIST pidl, int cb/*=-1*/) 
{ 
	if( !(bool)::GetMalloc() ) return NULL;

//	Get the size of the specified item identifier. 
	if( cb == -1 )
		cb = GetItemIDSize(pidl); 

//	Allocate a new item identifier list. 
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)GetMalloc()->Alloc(cb+sizeof(USHORT)); 
	if(pidlNew == NULL) return NULL; 

//	Copy the specified item identifier. 
	CopyMemory(pidlNew, pidl, cb); 

//	Append a terminating zero. 
	*((USHORT *)(((LPBYTE) pidlNew) + cb)) = 0; 

	return pidlNew; 
}

LPITEMIDLIST MergeItemID(LPCITEMIDLIST pidl,...) 
{
	va_list marker;
	int nSize = GetItemIDSize(pidl) + sizeof(pidl->mkid.cb);
	LPITEMIDLIST p;

//	count size of pidl
	va_start(marker,pidl);
	while( p = va_arg(marker, LPITEMIDLIST) )
		nSize += GetItemIDSize(p);
	va_end(marker);
//	allocate and merge pidls
	LPITEMIDLIST pidlNew = (LPITEMIDLIST)GetMalloc()->Alloc(nSize); 
	if(pidlNew == NULL) return NULL; 

	va_start(marker,pidl);
    CopyMemory(((LPBYTE)pidlNew), pidl, nSize = GetItemIDSize(pidl)); 
	while( p = va_arg(marker, LPITEMIDLIST) ) {
		   CopyMemory(((LPBYTE)pidlNew) + nSize, p, GetItemIDSize(p)); 
		   nSize += p->mkid.cb;
	}
	va_end(marker);
	*((USHORT *)(((LPBYTE) pidlNew) + nSize)) = 0; 
	return pidlNew; 
}

/****
HRESULT
SHBindToParent(LPCITEMIDLIST pidl, REFIID riid, VOID **ppv, LPCITEMIDLIST *ppidlLast)
{
	HRESULT hr;
	if( !pidl || !ppv )
		return E_POINTER;
	int nCount = ::GetItemIDCount(pidl);
	if( nCount == 0 ) {
	//	this is desktop pidl or invalid PIDL
		return E_POINTER;
	}
	if( nCount == 1 ) try {
	//	this is desktop item
		if( SUCCEEDED(hr = GetDesktopFolder()->QueryInterface(riid, ppv)) )
		{
			if( ppidlLast ) *ppidlLast = ::CopyItemID(pidl);
		}
		return hr;
	} catch ( _com_error e ) {
		return e.Error();
	}
	try {
		LPBYTE pRel = ::GetItemIDPos(pidl, nCount-1);
		CPidl pidlPar(::CopyItemID(pidl, pRel-(LPBYTE)pidl));
		IShellFolderPtr pFolder;
		
		if( FAILED(hr = GetDesktopFolder()->BindToObject(pidlPar, NULL,
			__uuidof(pFolder), (void**)&pFolder)) )
		{
			return hr;
		}
		if( SUCCEEDED(hr = pFolder->QueryInterface(riid, ppv)) )
		{
			if( ppidlLast ) *ppidlLast = ::CopyItemID((LPCITEMIDLIST)pRel);
		}
		return hr;
	} catch ( _com_error e ) {
		return e.Error();
	}
}
***/

HRESULT
SHGetBaseFolder(LPCITEMIDLIST pidl, REFIID riid, VOID **ppv)
{
	HRESULT hr;
	if( !pidl || !ppv )
		return E_POINTER;
	int nCount = ::GetItemIDCount(pidl);
	if( nCount == 0 ) {
	//	this is desktop pidl or invalid PIDL
		return E_POINTER;
	}
	if( nCount == 1 ) try 
  {
	//	this is desktop item
		if( SUCCEEDED(hr = GetDesktopFolder()->QueryInterface(riid, ppv)) )
		return hr;
	} 
  catch ( _com_error e ) 
  {
		return e.Error();
	}

  try 
  {
		//LPBYTE pRel = ::GetItemIDPos(pidl, nCount - 1);
		LPBYTE pRel = ::GetItemIDPos(pidl, 1);
		CPidl pidlPar(::CopyItemID(pidl, pRel-(LPBYTE)pidl));
		IShellFolderPtr pFolder;
		
		if( FAILED(hr = GetDesktopFolder()->BindToObject(pidlPar, NULL,
			__uuidof(pFolder), (void**)&pFolder)) )
		{
			return hr;
		}
		if( SUCCEEDED(hr = pFolder->QueryInterface(riid, ppv)) )
		return hr;
	} catch ( _com_error e ) {
		return e.Error();
 	}
  return TRUE;
}

static LRESULT CALLBACK
_CtxMenuWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	typedef struct {
		IContextMenu2*	pContextMenu2;
		WNDPROC			pOldWndProc;
	} _SUBCLASSDATA;

	_SUBCLASSDATA *pSubclassData = (_SUBCLASSDATA*)::GetProp(hWnd, "PxShlApi::SubclassData");
	ASSERT(pSubclassData);
	if( pSubclassData ) {
		switch( uMsg ) {
		case WM_INITMENUPOPUP:
		case WM_DRAWITEM:
		case WM_MENUCHAR:
		case WM_MEASUREITEM:
			if( pSubclassData->pContextMenu2 && 
				SUCCEEDED(pSubclassData->pContextMenu2->HandleMenuMsg(uMsg, wParam, lParam)) )
				return TRUE;
			return FALSE;
		}
		return CallWindowProc(pSubclassData->pOldWndProc,
			hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL TrackItemIDContextMenu(LPCITEMIDLIST pidlShellItem,
	UINT nFlags, LPPOINT ptPoint, HWND hWnd)
{
	typedef struct {
		IContextMenu2*	pContextMenu2;
		WNDPROC			pOldWndProc;
	} _SUBCLASSDATA;

	HRESULT hr;
	CPidl pidlRel;
	IShellFolderPtr  pParent;
	IContextMenuPtr  pCtxMenu;
	IContextMenu2Ptr pCtxMenu2;

	if( !pidlShellItem || !IsWindow(hWnd) )
		return FALSE;

	if( SUCCEEDED(hr = SHBindToParent(pidlShellItem, __uuidof(pParent),
		(void**)&pParent, pidlRel)) )
	{
		if( SUCCEEDED(pParent->GetUIObjectOf(NULL, 1, pidlRel, 
			__uuidof(pCtxMenu), NULL, (void**)&pCtxMenu)) )
		{
			CMenu ctxMenu;
			if( !ctxMenu.CreatePopupMenu() )
				return FALSE;
		//	
			if( FAILED(hr = pCtxMenu->QueryContextMenu(ctxMenu, 0,
				1, 10000, CMF_NORMAL)) )
				return FALSE;

			try{ pCtxMenu2 = pCtxMenu; } catch( ... ) {/*do nothing*/}
			_SUBCLASSDATA *pSubclassData  = new _SUBCLASSDATA;
			pSubclassData->pContextMenu2 = (IContextMenu2*)pCtxMenu2;
			pSubclassData->pOldWndProc = (WNDPROC)::SetWindowLong(hWnd,
				GWL_WNDPROC, (LONG)_CtxMenuWndProc);
			::SetProp(hWnd, "PxShlApi::SubclassData", (HANDLE)pSubclassData);

			nFlags &= ~TPM_NONOTIFY;
			nFlags |= TPM_RETURNCMD;
			UINT nResult = ::TrackPopupMenuEx(ctxMenu, nFlags,
				ptPoint->x, ptPoint->y, hWnd, NULL);
			::SetWindowLong(hWnd,
				GWL_WNDPROC, (LONG)pSubclassData->pOldWndProc);
			::RemoveProp(hWnd, "PxShlApi::SubclassData");
			delete pSubclassData;

			if( nResult > 0 && nResult < 10000 ) {
				CMINVOKECOMMANDINFO cmi;
				ZeroMemory(&cmi, sizeof(cmi));
				cmi.cbSize  = sizeof(cmi);
				cmi.hwnd    = hWnd;
				cmi.lpVerb  = MAKEINTRESOURCE(nResult - 1);
				cmi.nShow   = SW_SHOWNORMAL;

        //Lookup the command
        CString str;
        pCtxMenu->GetCommandString(nResult - 1, GCS_VERB, NULL, str.GetBuffer(_MAX_PATH), _MAX_PATH);
        str.ReleaseBuffer();

        cmi.lpVerb = (LPCSTR) str;

        if (str.CompareNoCase("copy") == 0)
        {
          CString strFile;
          SHGetPathFromIDList(pidlShellItem, strFile.GetBuffer(MAX_PATH));
          strFile.ReleaseBuffer();
          CStringArray strArray;
          strArray.SetSize(1);
          strArray[0] = strFile;
          ExecuteContextCommand(str, strArray, hWnd, FDIR_Parent);
          CopyToClipboard(strArray, hWnd);

          /**
          IDataObject *pData;
          if (!SUCCEEDED(pParent->GetUIObjectOf(hWnd, 1, pidlRel, __uuidof(pData), NULL, (void **) &pData)))
              return FALSE;

    	    int nCount = ::GetItemIDCount(pidlRel);
          if (pData)
          {
            OleSetClipboard(pData);
            pData->Release();
          }
          **/
        }
        else
        {
          hr = pCtxMenu->InvokeCommand(&cmi);
        }
  			return SUCCEEDED(hr);
			}
		}
	}
	return FALSE;
}

BOOL TrackItemIDContextMenu(LPCTSTR pszShellItemPath,
	UINT nFlags, LPPOINT ptPoint, HWND hWnd)
{
	if( pszShellItemPath == NULL || pszShellItemPath[0] == 0 )
		return FALSE;

  CPidl pidlAbsolute(pszShellItemPath);

  return pidlAbsolute.IsEmpty() ||	TrackItemIDContextMenu(pidlAbsolute, nFlags, ptPoint, hWnd);
}


BOOL ExecuteContextCommand(LPCTSTR pszCommand, LPCITEMIDLIST pidlShellItem, HWND hWnd)
{
	typedef struct {
		IContextMenu2*	pContextMenu2;
		WNDPROC			pOldWndProc;
	} _SUBCLASSDATA;

	HRESULT hr;
	CPidl pidlRel;
	IShellFolderPtr  pParent;
	IContextMenuPtr  pCtxMenu;
	IContextMenu2Ptr pCtxMenu2;

	if( !pidlShellItem)
		return FALSE;

	if( SUCCEEDED(hr = SHBindToParent(pidlShellItem, __uuidof(pParent),
		(void**)&pParent, pidlRel)) )
  //pParent = GetDesktopFolder();
  {
		if( SUCCEEDED(pParent->GetUIObjectOf(NULL, 1, pidlRel, 
		//if( SUCCEEDED(pParent->GetUIObjectOf(NULL, 1, &pidlShellItem, 
			__uuidof(pCtxMenu), NULL, (void**)&pCtxMenu)) )
		{
		//	


				CMINVOKECOMMANDINFO cmi;
				ZeroMemory(&cmi, sizeof(cmi));
				cmi.cbSize  = sizeof(cmi);
				cmi.hwnd    = hWnd;
				cmi.lpVerb  = pszCommand;
				cmi.nShow   = SW_SHOWNORMAL;

        hr = pCtxMenu->InvokeCommand(&cmi);

				return SUCCEEDED(hr);
		}
	}
	return FALSE;
}

BOOL ExecuteContextCommand(LPCTSTR pszCommand, 	IShellFolderPtr  pDrive, LPCITEMIDLIST *pidlShellItemList, int nSize, HWND hWnd)
{

	HRESULT hr;
	IContextMenuPtr  pCtxMenu;

	if( !pidlShellItemList)
		return FALSE;

  {
		if( SUCCEEDED(pDrive->GetUIObjectOf(NULL, nSize, pidlShellItemList, 
			__uuidof(pCtxMenu), NULL, (void**)&pCtxMenu)) )
		{

				CMINVOKECOMMANDINFO cmi;
				ZeroMemory(&cmi, sizeof(cmi));
				cmi.cbSize  = sizeof(cmi);
				cmi.hwnd    = hWnd;
				cmi.lpVerb  = pszCommand;
				cmi.nShow   = SW_SHOWNORMAL;

        hr = pCtxMenu->InvokeCommand(&cmi);

				return SUCCEEDED(hr);
		}
	}
	return FALSE;
}

BOOL ExecuteContextCommand(LPCTSTR pszCommand, LPCTSTR pszShellItemPath, HWND hWnd)
{
	if( pszShellItemPath == NULL ||
		pszShellItemPath[0] == 0 )
		return FALSE;
	CPidl pidlAbsolute(pszShellItemPath);
	return 
		pidlAbsolute.IsEmpty() ||
		ExecuteContextCommand(pszCommand, pidlAbsolute, hWnd);
}

typedef CPidl* PCPidl;

BOOL ExecuteContextCommand(LPCTSTR pszCommand, CStringArray& strPathArray, HWND hWnd, EFileDir eFDir)
{
	IShellFolderPtr  pBase;
  BOOL             bResult = true;

  int nSize = strPathArray.GetSize();
  if (nSize == 0)
    return FALSE;

  
  CPidl** pPIDLArray = new PCPidl[nSize];
  memset (pPIDLArray, 0, sizeof (CPidl*) * nSize);

  LPCITEMIDLIST* pIdList = new LPCITEMIDLIST[nSize];
  memset (pIdList, 0, sizeof (LPCITEMIDLIST) * nSize);
  
  int nCount = NULL;
  for (int i = 0; i < strPathArray.GetSize(); i++)
  {
    pPIDLArray[i] = new CPidl(strPathArray[i]);
    if (pPIDLArray[i]->IsEmpty())
    {
      bResult = FALSE;
      goto cleanup;
    }

    if (nCount == NULL)
    {
      if (eFDir == FDIR_Drive)
  	    nCount = ::GetItemIDCount(*pPIDLArray[i]);
      else
        nCount = 2;
    }

    pIdList[i] = ::CopyItemID((LPCITEMIDLIST)::GetItemIDPos(*pPIDLArray[i], nCount - 1));
  }

  HRESULT hr;
  if (eFDir == FDIR_Drive)
  {
    if( !SUCCEEDED(hr = SHGetBaseFolder(*pPIDLArray[0], __uuidof(pBase), (void**)&pBase)) )
      goto cleanup;
  }
  else
  {
    if( !SUCCEEDED(hr = SHBindToParent(*pPIDLArray[0], __uuidof(pBase), (void**)&pBase, NULL)) )
      goto cleanup;
  }

	bResult = ExecuteContextCommand(pszCommand, pBase, pIdList, nSize, hWnd);

cleanup:
  for (i = 0; i < strPathArray.GetSize(); i++)
  {
    delete pPIDLArray[i];
  }
  delete pPIDLArray;
  delete pIdList;
  return bResult;
}

BOOL BindToFolder(IShellFolderPtr&  pBase, LPCITEMIDLIST*& pIdList, CStringArray& strPathArray, EFileDir eFDir)
{
  BOOL             bResult = true;

  int nSize = strPathArray.GetSize();
  if (nSize == 0)
    return FALSE;

  
  CPidl** pPIDLArray = new PCPidl[nSize];
  memset (pPIDLArray, 0, sizeof (CPidl*) * nSize);

  pIdList = new LPCITEMIDLIST[nSize];
  memset (pIdList, 0, sizeof (LPCITEMIDLIST) * nSize);
  
  int nCount = NULL;
  for (int i = 0; i < strPathArray.GetSize(); i++)
  {
    pPIDLArray[i] = new CPidl(strPathArray[i]);
    if (pPIDLArray[i]->IsEmpty())
    {
      bResult = FALSE;
      goto cleanup;
    }

    if (nCount == NULL)
    {
      switch(eFDir)
      {
        case FDIR_Parent:
  	      nCount = ::GetItemIDCount(*pPIDLArray[i]) - 1;
          break;
        case FDIR_Drive:
          nCount = 1;
          break;
        case FDIR_Desktop:
          nCount = 0;
          break;
        default:
          ASSERT(FALSE);
          return FALSE;
      }
    }

    pIdList[i] = ::CopyItemID((LPCITEMIDLIST)::GetItemIDPos(*pPIDLArray[i], nCount));
  }

  HRESULT hr;
  switch(eFDir)
  {
    case FDIR_Parent:
      bResult = SUCCEEDED(hr = SHBindToParent(*pPIDLArray[0], __uuidof(pBase), (void**)&pBase, NULL));
      break;
    case FDIR_Drive:
      bResult = SUCCEEDED(hr = SHGetBaseFolder(*pPIDLArray[0], __uuidof(pBase), (void**)&pBase));
      break;
    case FDIR_Desktop:
      break;
    default:
      ASSERT(FALSE);
      bResult = FALSE;
  }

cleanup:
  for (i = 0; i < strPathArray.GetSize(); i++)
  {
    delete pPIDLArray[i];
  }
  delete pPIDLArray;

  if (!bResult)
  {
    delete pIdList;
    pIdList = NULL;
  }

  return bResult;
}


BOOL CopyToClipboard(CStringArray& strPathArray, HWND hWnd)
{
  IShellFolderPtr pBase = NULL;
  LPCITEMIDLIST*  pIdList = NULL;

  if (BindToFolder(pBase, pIdList, strPathArray, FDIR_Parent))
  {
    IDataObject *pData;
    for (int i = 0; i < strPathArray.GetSize(); i++)
    {
      if (!SUCCEEDED(pBase->GetUIObjectOf(hWnd, strPathArray.GetSize(), pIdList, __uuidof(pData), NULL, (void **) &pData)))
        return FALSE;

      if (pData)
      {
        OleSetClipboard(pData);
        pData->Release();
      }
    } 
  }

  delete pIdList;
  return TRUE;
}

BOOL ExecuteContextCommand(LPCTSTR pszCommand, CStringArray& strPathArray, HWND hWnd)
{
  IShellFolderPtr pBase = NULL;
  LPCITEMIDLIST*  pIdList = NULL;
  BOOL            bResult = TRUE;

  CString str = strPathArray[0];
  if (BindToFolder(pBase, pIdList, strPathArray, FDIR_Parent))
  {
   	bResult = ExecuteContextCommand(pszCommand, pBase, pIdList, strPathArray.GetSize(), hWnd);
  }

  delete pIdList;
  return TRUE;
}