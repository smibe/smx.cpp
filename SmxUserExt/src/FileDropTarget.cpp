// FileDropTarget.cpp: implementation of the CFileDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AfxOle.h"
#include "PxShlApi.h"
#include "FileDropTarget.h"

#define GetPIDLFolder(pida) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define GetPIDLItem(pida, i) (LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])

UINT CFileDropTarget::sm_cfFileName = 0;
UINT CFileDropTarget::sm_cfShellIdList = 0;

CFileDropTarget::CFileDropTarget()
{
  m_pDragEnter = NULL;
  m_pDragLeave = NULL;
  m_pDragOver  = NULL;
  m_pDrop      = NULL;

//Clipboard formats
  if (sm_cfFileName == NULL)
  {
    sm_cfFileName    = RegisterClipboardFormat(CFSTR_FILENAME);
    sm_cfShellIdList = RegisterClipboardFormat(CFSTR_SHELLIDLIST);
  }
}

CFileDropTarget::~CFileDropTarget()
{

}

DROPEFFECT CFileDropTarget::OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  if (!m_pDragEnter)
    return DROPEFFECT_NONE;

  return (m_pObject->*m_pDragEnter)(pDataObject, dwKeyState, point, this);
}

void CFileDropTarget::OnDragLeave( CWnd* pWnd )
{
  if (!m_pDragLeave)
    return ;

  (m_pObject->*m_pDragLeave)(this);
}

DROPEFFECT CFileDropTarget::OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point )
{
  if (!m_pDragOver)
    return DROPEFFECT_NONE;

  return (m_pObject->*m_pDragOver)(pDataObject, dwKeyState, point, this);
}

BOOL CFileDropTarget::OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point )
{
  if (!m_pDrop)
    return FALSE;

  return (m_pObject->*m_pDrop)(pDataObject, dropEffect, point, this);
}

UINT CFileDropTarget::GetPathNames(COleDataObject* pDataObject, CStringArray* pPathArray)
{
  STGMEDIUM stgMedium;
  UINT      nResult = 0;
  if (pDataObject->GetData(sm_cfFileName, &stgMedium) && stgMedium.tymed == TYMED_FILE)
  {
    if (!stgMedium.lpszFileName)
      return 0;

    if (pPathArray)
    {
      pPathArray->SetSize(1);
      (*pPathArray)[0] = stgMedium.lpszFileName;
    }
    return 1;
  }

  if (pDataObject->GetData(sm_cfShellIdList, &stgMedium))
  {
    if (stgMedium.tymed != TYMED_HGLOBAL)
      return 0;

    CIDA *pCIDA = (CIDA *) GlobalLock(stgMedium.hGlobal);

    if (!pCIDA || pCIDA->cidl <= 0)
    {
      GlobalUnlock((HGLOBAL) pCIDA);
      return 0;
    }

    LPCITEMIDLIST pIdlFolder = GetPIDLFolder(pCIDA);
    CString strFolder;
    SHGetPathFromIDList(pIdlFolder, strFolder.GetBuffer(MAX_PATH));
    strFolder.ReleaseBuffer();

    LPCITEMIDLIST pIdlFile   = NULL;
    LPCITEMIDLIST pIdlPath   = NULL;

    if (pPathArray)
      pPathArray->SetSize(pCIDA->cidl);

    for (int i = 0; i < (int) pCIDA->cidl; i++)
    {
      if (pPathArray)
      {
        pIdlFile = GetPIDLItem(pCIDA, i);
        pIdlPath = MergeItemID(pIdlFolder, pIdlFile, NULL);
        SHGetPathFromIDList(pIdlPath, (*pPathArray)[i].GetBuffer(MAX_PATH));
        (*pPathArray)[i].ReleaseBuffer();
      }
    }

    return pCIDA->cidl;
  }
  
  return 0;
}
