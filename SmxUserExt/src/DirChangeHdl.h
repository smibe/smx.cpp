// DirChangeHdl.h: interface for the CDirChangeHdl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRCHANGEHDL_H__988FAAE1_0D71_4FAD_85D1_9E2DCE52C1B8__INCLUDED_)
#define AFX_DIRCHANGEHDL_H__988FAAE1_0D71_4FAD_85D1_9E2DCE52C1B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDirChangeThread;
class CDirWatchInfo;
class CDirChangeData;
class CRegExp;

#define WMU_DIRCHANGE (WM_USER + 100)

#define LOCK() CSingleLock __SLock(&m_CriticalSection);__SLock.Lock()

#include <afxmt.h>


class CDirChangeHdl  : public CStatic
{
public:
	CDirChangeHdl();
	virtual ~CDirChangeHdl();

  bool Initialize();
  void Exit();

  bool AddDirWatch(LPCTSTR pszDirectory, DWORD dwWatchFlags, bool bSubDir, LPCTSTR pszFileMask, LPCTSTR pszExcludeMask);
  bool RemoveDirWatch(LPCTSTR pszDirectory);
  bool RemoveAllWatches();

  virtual bool HandleNotification(CDirChangeData *pDirChangeData, DWORD dwLength);

  CDirWatchInfo* GetDirWatchInfo(LPCTSTR pszDirectory);

  LRESULT OnDirChange(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
protected:
  bool StartThread();
  void StopThread();

public:

  HANDLE   m_hComPort;                 // Handle To a Completion Port
  HANDLE   m_hThread;
  DWORD    m_dwThreadID;

  CCriticalSection m_CriticalSection;
protected:
  CMap<LPCTSTR, LPCTSTR, CDirWatchInfo*, CDirWatchInfo*> m_DirWatchMap;
};

class CDirWatchInfo
{
public:
  CDirWatchInfo();
  virtual ~CDirWatchInfo();

  bool ReadDirectoryChanges();

  bool Match(const CString& strFileName);

  void CreateRegExp();

public:
  CString m_strDirectory;
  CString m_strFileMask;
  CString m_strExcludeMask;
  bool    m_bSubDir;
  DWORD   m_dwWatchFlags;

  HANDLE      m_hDir;
  OVERLAPPED  m_Overlapped;

  CRegExp *m_pExcludeRegex;
  CRegExp *m_pMaskRegex;

  BYTE   *m_pBuffer;
  DWORD   m_dwBufLength;
};

class CDirChangeData
{
public:
  CDirChangeData(DWORD dwNofBytes);
  ~CDirChangeData();

public:
  BYTE          *m_pBuffer;
  DWORD          m_dwNofBytes;
  CDirWatchInfo *m_pInfo;
  CString        m_strDir;
};

#endif // !defined(AFX_DIRCHANGEHDL_H__988FAAE1_0D71_4FAD_85D1_9E2DCE52C1B8__INCLUDED_)
