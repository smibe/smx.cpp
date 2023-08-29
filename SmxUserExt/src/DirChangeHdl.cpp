// DirChangeHdl.cpp: implementation of the CDirChangeHdl class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DirChangeHdl.h"
#include "RegEx.h"

typedef BOOL (WINAPI *ReadDirectoryChangesWFncPtr)(
                                  HANDLE hDirectory,
                                  LPVOID lpBuffer,
                                  DWORD nBufferLength,
                                  BOOL bWatchSubtree,
                                  DWORD dwNotifyFilter,
                                  LPDWORD lpBytesReturned,
                                  LPOVERLAPPED lpOverlapped,
                                  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
                                  );

/****
class CGetModule
{
public:
  CGetModule()  { m_hModule = NULL; }
  ~CGetModule() { if (m_hModule) ::FreeLibrary(m_hModule); }

  BOOL LoadModule(LPCTSTR pszLib) { m_hModule = ::LoadLibrary(pszLib); return m_hModule != NULL; }
public:
  HMODULE m_hModule;

};
***/

BOOL SmxReadDirectoryChangesW(
                                  HANDLE hDirectory,
                                  LPVOID lpBuffer,
                                  DWORD nBufferLength,
                                  BOOL bWatchSubtree,
                                  DWORD dwNotifyFilter,
                                  LPDWORD lpBytesReturned,
                                  LPOVERLAPPED lpOverlapped,
                                  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
                                  );
/***
{
  static ReadDirectoryChangesWFncPtr pFuncPtr = NULL;
  static CGetModule                  Module;
  if (pFuncPtr == NULL && Module.m_hModule == NULL)
  {
    
    if (Module.LoadModule("kernel32.dll"))
    {
      pFuncPtr = (ReadDirectoryChangesWFncPtr) ::GetProcAddress (Module.m_hModule, "ReadDirectoryChangesW");
    }
  }

  if (!pFuncPtr)
    return FALSE;

  return (*pFuncPtr)(hDirectory, lpBuffer, nBufferLength, bWatchSubtree, 
                     dwNotifyFilter, lpBytesReturned, lpOverlapped, lpCompletionRoutine);
}
****/


void WINAPI HandleDirectoryChange( DWORD dwParameter )
{
  CDirChangeHdl *pHandler = (CDirChangeHdl *) dwParameter;
  DWORD          dwNofBytes;
  CDirWatchInfo *pInfo = NULL;
  OVERLAPPED    *pOverlapped;

  while(true)
  {
    // Retrieve the directory info for this directory
    // through the completion key
    GetQueuedCompletionStatus( (HANDLE) pHandler->m_hComPort,
                               &dwNofBytes,
                               (DWORD*) &pInfo,
                               &pOverlapped,
                               INFINITE);

    if ( !pInfo )
      break;

    if (dwNofBytes > 0)
    {
      //Need to copy the data into a new buffer
      CDirChangeData *pData = new CDirChangeData(dwNofBytes);
      pData->m_pInfo = pInfo;
      pData->m_strDir = pInfo->m_strDirectory;
      memcpy(pData->m_pBuffer, pInfo->m_pBuffer, dwNofBytes);
      pHandler->PostMessage(WMU_DIRCHANGE, dwNofBytes, (LPARAM) pData);
    }

    // Reissue the watch command
    if (!pInfo->ReadDirectoryChanges())
    {
      DWORD dwError = GetLastError();
    }
  }
}


BEGIN_MESSAGE_MAP(CDirChangeHdl, CStatic)
	//{{AFX_MSG_MAP(CAccelListBox)
	ON_MESSAGE(WMU_DIRCHANGE, OnDirChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CDirChangeHdl::CDirChangeHdl()
{
  m_hThread    = NULL;
  m_hComPort  = NULL;
  m_dwThreadID = NULL;
}

CDirChangeHdl::~CDirChangeHdl()
{

}

bool CDirChangeHdl::Initialize()
{
  LOCK();
  if (!Create(NULL, 0, CRect(0, 0, 0, 0), AfxGetMainWnd()))
    return false;

  return true;
}

void CDirChangeHdl::Exit()
{
  if (m_hThread)
  {
    LOCK();
    RemoveAllWatches();
  }

  WaitForSingleObject( m_hThread, INFINITE );

  CloseHandle( m_hThread );
  m_hThread = NULL;

  DestroyWindow();
}

bool CDirChangeHdl::AddDirWatch(LPCTSTR pszDirectory, DWORD dwWatchFlags, bool bSubDir, LPCTSTR pszFileMask, LPCTSTR pszExcludeMask)
{
  LOCK();
  CDirWatchInfo *pInfo = new CDirWatchInfo();

  pInfo->m_strDirectory = pszDirectory;
  pInfo->m_dwWatchFlags = dwWatchFlags;
  pInfo->m_bSubDir = bSubDir;
  pInfo->m_strFileMask = pszFileMask;
  pInfo->m_strExcludeMask = pszExcludeMask;

  pInfo->CreateRegExp();

  pInfo->m_hDir = CreateFile( pszDirectory,
                             FILE_LIST_DIRECTORY,
                             FILE_SHARE_READ |
                             FILE_SHARE_WRITE |
                             FILE_SHARE_DELETE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_BACKUP_SEMANTICS |
                             FILE_FLAG_OVERLAPPED,
                             NULL);
	if( pInfo->m_hDir == INVALID_HANDLE_VALUE )
	{
		DWORD dwError = GetLastError();
  }

  m_hComPort = CreateIoCompletionPort( pInfo->m_hDir,
                                      m_hComPort,
                                      (DWORD) pInfo,
                                       0);

  m_DirWatchMap.SetAt(pInfo->m_strDirectory, pInfo);

  if (!pInfo->ReadDirectoryChanges())
  {
    DWORD dwError = GetLastError();
  }

  if (!m_hThread)
  {
    if (!StartThread())
    {
      ASSERT(false);
      CloseHandle(m_hComPort);
      delete pInfo;
    }

    ASSERT(m_hThread);
  }

  return true;
}

bool CDirChangeHdl::RemoveDirWatch(LPCTSTR pszDirectory)
{
  LOCK();

  CDirWatchInfo *pInfo;
  if (!m_DirWatchMap.Lookup(pszDirectory, pInfo))
    return true;

  m_DirWatchMap.RemoveKey(pszDirectory);
  delete pInfo;

  if (m_DirWatchMap.GetCount() == 0)
  {
    StopThread();
  }
  return true;
}

bool CDirChangeHdl::RemoveAllWatches()
{
  LOCK();
  CDirWatchInfo *pInfo;
  LPCTSTR pszDir = NULL;
  for(POSITION pos = m_DirWatchMap.GetStartPosition(); pos; )
  {
    m_DirWatchMap.GetNextAssoc(pos, pszDir, pInfo);
    delete pInfo;
  }
  m_DirWatchMap.RemoveAll();

  StopThread();
  return true;
}

CDirWatchInfo *CDirChangeHdl::GetDirWatchInfo(LPCTSTR pszDirectory)
{
  LOCK();
  CDirWatchInfo *pInfo = NULL;
  m_DirWatchMap.Lookup(pszDirectory, pInfo);
  return pInfo;
}

void CDirChangeHdl::StopThread()
{
  PostQueuedCompletionStatus( m_hComPort, 0, 0, NULL );
  WaitForSingleObject(m_hThread, INFINITE);
  m_hThread    = NULL;
  m_dwThreadID = NULL;
}

bool CDirChangeHdl::StartThread()
{
  ASSERT(m_DirWatchMap.GetCount() > 0);
  m_hThread = CreateThread( NULL, 0,
                            (LPTHREAD_START_ROUTINE) HandleDirectoryChange,
                            this,
                            0,
                            &m_dwThreadID);
  return m_hThread != NULL;
}

LRESULT CDirChangeHdl::OnDirChange(WPARAM wParam, LPARAM lParam)
{
  LOCK();
  HandleNotification((CDirChangeData *) lParam, wParam);

  MSG Msg;
  while (PeekMessage(&Msg, m_hWnd, WMU_DIRCHANGE, WMU_DIRCHANGE, PM_REMOVE))
  {
    HandleNotification((CDirChangeData *) Msg.lParam, Msg.wParam);
  }

  //We have got them all, signal it by setting CDirWatchInfo to NULL;
  HandleNotification(0, 0);

  return 1;
}

bool CDirChangeHdl::HandleNotification(CDirChangeData *pDirChangeData, DWORD dwLength)
{
  return true;
}



CDirWatchInfo::CDirWatchInfo()
{
  m_dwBufLength = 4096;
  m_pBuffer      = new BYTE [m_dwBufLength];
  m_bSubDir      = false;
  m_dwWatchFlags = NULL;

  m_hDir = NULL;

  m_pExcludeRegex = 0;
  m_pMaskRegex = 0;

  memset (&m_Overlapped, 0, sizeof(OVERLAPPED));
}

CDirWatchInfo::~CDirWatchInfo()
{
  if (m_hDir)
    CloseHandle(m_hDir);

  delete m_pBuffer;
  delete m_pExcludeRegex;
  delete m_pMaskRegex;
}

bool CDirWatchInfo::ReadDirectoryChanges()
{
  DWORD dwLength = NULL;
  memset(&m_Overlapped, 0 , sizeof (OVERLAPPED));
  return SmxReadDirectoryChangesW(m_hDir, m_pBuffer, m_dwBufLength, m_bSubDir, 
                                 m_dwWatchFlags, &dwLength, &m_Overlapped, NULL) != NULL;
}


bool CDirWatchInfo::Match(const CString& strFileName)
{
  if (m_pMaskRegex)
  {
    int nIdx = m_pMaskRegex->RegFind(strFileName);
    if (nIdx == -1)
      return false;
  }

  if (m_pExcludeRegex)
  {
    int nIdx = m_pExcludeRegex->RegFind(strFileName);
    if (nIdx != -1)
      return false;
  }

  return true;
}

void CDirWatchInfo::CreateRegExp()
{
  delete m_pMaskRegex;

  if (!m_strFileMask.IsEmpty())
  {
    m_strFileMask.MakeLower();
    m_pMaskRegex = new CRegExp ();
    if (!m_pMaskRegex->RegCompFileMask(m_strFileMask))
    {
      delete m_pMaskRegex;
      m_pMaskRegex = NULL;
    }
  }

  if (!m_strExcludeMask.IsEmpty())
  {
    m_strExcludeMask.MakeLower();
    m_pExcludeRegex = new CRegExp ();
    if (!m_pExcludeRegex->RegCompFileMask(m_strExcludeMask))
    {
      delete m_pExcludeRegex;
      m_pExcludeRegex = NULL;
    }
  }
}


CDirChangeData::CDirChangeData(DWORD dwNofBytes)
{
  m_pBuffer = new BYTE[dwNofBytes];
  m_dwNofBytes = dwNofBytes;
  m_pInfo = NULL;
}

CDirChangeData::~CDirChangeData()
{
  delete m_pBuffer;
}
