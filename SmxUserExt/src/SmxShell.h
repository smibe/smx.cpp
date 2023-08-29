
#ifndef __SMXSHELL_H__
#define __SMXSHELL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <shlobj.h>

#define WM_SHELL_NOTIFY		WM_USER + 1
#define NM_SH_SHELLMENU		1

// helper struct that holds list item data
typedef struct tagLVID
{
   LPSHELLFOLDER pParent;
   LPITEMIDLIST  pIDL;
   ULONG         ulAttribs;
} LVITEMDATA, *LPLVITEMDATA;

// helper struct that holds tree item data
typedef struct tagTVID
{
   LPSHELLFOLDER pParent;
   LPITEMIDLIST  pIDL;
   LPITEMIDLIST  pIDLfq;
} TVITEMDATA, *LPTVITEMDATA;

// CSMXShell class is used to perform shell file operations
class CSMXShell
{

public:
	CSMXShell(LPCTSTR lpszFileName=NULL);
	virtual ~CSMXShell();

    int			  m_iImageIndex;	// index into system image list
    CString		m_strPath;		// full file path used with BrowseForFolder(...)
    CString		m_strInitDir;	// start directory used with BrowseForFolder(...)
    CString		m_strSelDir;	// selected directory used with BrowseForFolder(...)
    CString		m_strTitle;		// dialog title used with BrowseForFolder(...)


protected:
  LPMALLOC    m_pMalloc;
  CImageList *m_pSmallImageList;
  CImageList *m_pLargeImageList;

	CString		m_strFileName;			// contains the full path to a file or folder
	char		m_szDrive[_MAX_DRIVE];	// contains the drive letter
	char		m_szDir[_MAX_DIR];		// contains the directory string
	char		m_szFname[_MAX_FNAME];	// contains the file name minus path and ext
	char		m_szExt[_MAX_EXT];		// contains the file extension

public:
  LPMALLOC GetMalloc();
  void Free (LPITEMIDLIST pIDL);
  CImageList* GetSmallImageList();
  CImageList* GetLargeImageList();

  virtual void SplitPath(CString strFileName, char* szDrive, char* szDir, char* szFile, char* szExt);

	virtual LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
	virtual HINSTANCE ShellExec(LPCTSTR lpszExec, int nShow);

	virtual CString GetFileSizeString(LPCTSTR lpszFileSize);
	virtual CString GetLastWriteTime(CTime* pTime);
	virtual CString GetFileDrive(LPCTSTR lpszFileName=NULL);
	virtual BOOL Exist(LPCTSTR lpszFileName=NULL);
	virtual int	GetIconIndex(LPCTSTR lpszFileName = NULL, UINT uFlags = SHGFI_SMALLICON);
	virtual CString GetDisplayName(LPCTSTR lpszFileName = NULL);
	virtual CString GetFileType(LPCTSTR lpszFileName = NULL);
	virtual CString GetFileExt(LPCTSTR lpszFileName = NULL);
	virtual CString GetFileTitle(LPCTSTR lpszFileName=NULL);
	virtual CString GetRoot(LPCTSTR lpszFileName=NULL);
	virtual CString GetFileName(LPCTSTR lpszFileName=NULL);
	virtual void GetSystemImageList(CImageList* pSmallList, CImageList* pLargeList);
	virtual BOOL BrowseForFolder(CWnd* pParentWnd);
	static int CALLBACK BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

  //--------------- Functions that deal with  PIDLs -------------------------- 
	virtual LPITEMIDLIST Next(LPCITEMIDLIST pidl);
	virtual UINT GetSize(LPCITEMIDLIST pidl);
	virtual LPITEMIDLIST Create(UINT cbSize);
	virtual LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	virtual LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);
	virtual BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPTSTR lpFriendlyName);
	virtual LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);
	virtual BOOL DoTheMenuThing( HWND hwnd, LPSHELLFOLDER lpsfParent, LPITEMIDLIST lpi, LPPOINT lppt);
	virtual int	GetIcon( LPITEMIDLIST lpi, UINT uFlags);
	virtual void GetNormalAndSelectedIcons( LPITEMIDLIST lpifq, LPTV_ITEM lptvitem);
	virtual LPITEMIDLIST DuplicateItem ( LPMALLOC pMalloc, LPITEMIDLIST pidl);
	virtual LPITEMIDLIST GetNextItem ( LPITEMIDLIST pidl);
	virtual UINT GetItemCount ( LPITEMIDLIST pidl); 
	virtual BOOL DoExplorerMenu ( HWND hwnd, LPCTSTR pszPath, POINT point);


};


#endif // __SMXSHELL_H__

