// SmxLib.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "winbase.h"
#include "SmxLib.h"
#include "..\..\exchange\projects\smxuserext\src\PxShlApi.h"
#include "..\..\exchange\projects\smxuserext\src\CShellFIleOp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#undef EXPORT
#define EXPORT _declspec(dllexport)
//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSmxLibApp

BEGIN_MESSAGE_MAP(CSmxLibApp, CWinApp)
	//{{AFX_MSG_MAP(CSmxLibApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmxLibApp construction

CSmxLibApp::CSmxLibApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmxLibApp object

CSmxLibApp theApp;

extern "C"
{

EXPORT int CreateStringArray()
{
  return (int ) new CStringArray();
}


EXPORT void DeleteStringArray(int handle)
{
  delete (CStringArray*) handle;
}

EXPORT void AddToStringArray(int handle, LPCTSTR str)
{
  ((CStringArray*) handle)->Add(str);
}

EXPORT void ExecuteShell(LPCTSTR strPath, LPCTSTR strCmd)
{
  CWnd *pWnd = AfxGetMainWnd();
  ExecuteContextCommand(strCmd, strPath, pWnd->GetSafeHwnd());
}

EXPORT void ExecuteShellOnFiles(DWORD dwStringArray, LPCTSTR strCmd)
{
  CStringArray *strArray = (CStringArray* ) dwStringArray;
  CWnd *pWnd = AfxGetMainWnd();
  ExecuteContextCommand(strCmd, *strArray, pWnd->GetSafeHwnd());
}

EXPORT void TrackShellMenu(LPCTSTR strFile, int x, int y)
{
  CWnd *pWnd = AfxGetMainWnd();
  CPoint Point(x,y);
  TrackItemIDContextMenu(strFile, 0, (LPPOINT) &Point, AfxGetMainWnd()->GetSafeHwnd());
}

EXPORT void SmxShellExecute(LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory)
{
  ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), lpOperation, lpFile, lpParameters, lpDirectory, SW_SHOWNORMAL);
}

EXPORT DWORD CreateItemIdList(DWORD handle)
{
  CStringArray   *pFiles = (CStringArray *) handle;
  IShellFolderPtr pBase = NULL;
  LPCITEMIDLIST*  pIdList = NULL;
  BOOL            bResult = TRUE;

  if (BindToFolder(pBase, pIdList, *pFiles, FDIR_Desktop))
    return (DWORD) pIdList;
  else
  {
    GetMalloc()->Free(pIdList);
    return 0;
  }
}

EXPORT void ShellFileOperation(DWORD hSrc, DWORD hDest, int op)
{
  CStringArray *pSrc = (CStringArray *) hSrc;
  CStringArray *pDest = (CStringArray *) hDest;

  CShellFileOp shOp;
  for (int i = 0; i < pSrc->GetSize(); i++)
  {
    shOp.AddSourceFile(pSrc->GetAt(i));
  }

  for (i = 0; i < pDest->GetSize(); i++)
  {
    shOp.AddDestFile(pDest->GetAt(i));
  }

  WORD wFlags = FOF_NOCONFIRMMKDIR | FOF_ALLOWUNDO;

  if (pDest->GetSize() > 1)
    wFlags |= FOF_MULTIDESTFILES;

  shOp.SetOperationFlags(op, AfxGetMainWnd(), wFlags);
  BOOL bDummy;
  shOp.Go(&bDummy);
}

EXPORT int HasSubdirectories(LPCTSTR pszDir)
{
		WIN32_FIND_DATA FindData = {0};
    CString strDir = pszDir;
    CString strFound;
    BOOL bResult = FALSE;

    if (strDir.Find("tmp") != -1)
    {
      int XXX = 0;
    }
    HANDLE hFind = FindFirstFileEx(strDir + "\\*", FindExInfoStandard, &FindData, FindExSearchLimitToDirectories, 0, 0);

		if (hFind == INVALID_HANDLE_VALUE)
			return FALSE;

    strFound = FindData.cFileName;
		if (strFound != "." && strFound != ".." && ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
    {
      bResult = TRUE;
      goto cleanup;
    }

		while (FindNextFile(hFind, &FindData) != 0)
		{
      strFound = FindData.cFileName;
		  if (strFound != "." && strFound != ".." && ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
      {
        bResult = TRUE;
        break;
      }
		}

cleanup:
    FindClose(hFind);
		return bResult;
}


}