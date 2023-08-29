/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FILELIST.CPP,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/FILELIST.CPP,v 1.2 2002/11/22 17:12:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Implementation : COXFileList
// ==========================================================================

// Source file : filelist.cpp

// Source : Periphere NV (R.Mortelmans)
// Creation Date : 	   2nd November 1995
// Last Modification : 2nd November 1995

// 270598:GeH:Extended SEARCH() for pattern search
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"		// standard MFC include
#include "filelist.h"	// class specification
#include "dos.h"		// for _dos_findfirst, ...
#include "regex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:
	// COXPathSpec m_path;
	// --- The path specification where to look for files
	
	// CObList m_fileList;			// Contains objects of class COXFileSpec
	// --- The list of file specifications found in the above specified path

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXFileList::COXFileList()
	:
	m_path()
	{
	}
	
COXPathSpec COXFileList::GetPath() const
	{
	return m_path;
	}
	
BOOL COXFileList::SetPath(COXPathSpec path)
	{
	if (path.GetFileName().IsEmpty())
		path.SetFileName(_T("*.*"));
	if (path.MakeAbsolute())
		{
		m_path = path;
		return TRUE;
		}
	else
		{
		TRACE(TEXT("COXFileList::SetPath : Path spec is invalid : %s\n"), path.GetPath());
		return FALSE;
		}
	}   

//270598:GeH --->
static CString GetFileMask(CString &fileMask)
{
	CString mask;

	int kk = fileMask.Find(';');

	if(kk == -1) 
	{
		mask  = fileMask;
		fileMask = "";
		return (mask);
	}

	mask = fileMask.Left(kk);
	fileMask = fileMask.Mid(kk+1);
	return(mask);
}
//270598:GeH <---

	
BOOL COXFileList::Search(CString pattern, CString relDir, 
                         CRegExp *pMaskRegExp, CRegExp *pExcludeRegExp)
	{
#ifdef WIN32
	COXFileSpec* pFile;
	WIN32_FIND_DATA fileData;
	HANDLE hFindFile;
	BOOL bFileFound(TRUE);
		
//270598:GeH --->
	CString path = m_path.GetPath();
	CString newPath;
	CString mask;
	if(m_path.GetFileName() != "*.*")
		pattern = "333";				//remember: no pattern search
	else
	{
		if(pattern == "")
			pattern = "*.*";				//default search
		path = path.Left(path.Find('*'));
	}

	while(pattern != "") 
	{
		if(pattern == "333")
		{
			newPath= path;
			pattern = "";
		}
		else
		{
			mask = GetFileMask(pattern);
			newPath = path + mask;
		}

		bFileFound = TRUE;
		hFindFile = FindFirstFile(newPath, &fileData);
		if(hFindFile != INVALID_HANDLE_VALUE)
			{
			while (bFileFound)
			{  
				if (((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
					(fileData.cFileName[0] != _T('.')))
        {
          if (pMaskRegExp || pExcludeRegExp)
          {
            bool bContinue = false;
            CString str = relDir + fileData.cFileName;
            str.MakeLower();

            if (pMaskRegExp && pMaskRegExp->RegFind(str) == -1)
              bContinue = true;
            
            if (pExcludeRegExp && pExcludeRegExp->RegFind(str) != -1)
              bContinue = true;

            if (bContinue)
            {
      				bFileFound = FindNextFile(hFindFile,&fileData);
              continue;
            }
          }

          if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
          {
            pFile = new COXFileSpec;
					  pFile->SetFileName(fileData.cFileName);
					  pFile->SetTime(CTime(fileData.ftLastWriteTime));
					  ASSERT(fileData.nFileSizeHigh == 0);
					  pFile->SetLength(fileData.nFileSizeLow);
					  pFile->SetAttributes((CFile::Attribute)fileData.dwFileAttributes);
					  m_fileArray.Add(pFile);
          }
        }

				bFileFound = FindNextFile(hFindFile,&fileData);
				}
			FindClose(hFindFile);
			}
	}
	Sort();
	return TRUE;

#else
	COXFileSpec* pFile;
	_find_t fileInfo;
	BOOL bFileFound;
	
	bFileFound = (_dos_findfirst(m_path.GetPath(), _A_NORMAL | _A_ARCH, &fileInfo) == 0);
	while (bFileFound)
		{
		pFile = new COXFileSpec;
		pFile->SetFileName(fileInfo.name);
		pFile->SetTime(CTime((WORD)fileInfo.wr_date, (WORD)fileInfo.wr_time));
		pFile->SetLength(fileInfo.size);
		pFile->SetAttributes((CFile::Attribute)fileInfo.attrib);
		m_fileArray.Add(pFile);
		bFileFound = (_dos_findnext(&fileInfo) == 0);
		}		
	return TRUE;
#endif
	}
	
const CPtrArray* COXFileList::GetList() const
	{
	return &m_fileArray;
	}

const COXFileSpec* COXFileList::GetAt(int nIndex) const
{
	return (COXFileSpec*) m_fileArray.GetAt(nIndex);
}
	
void COXFileList::Sort()
	{
	// This algorithm will sort a list using selection sort
	// The array contains objects from position 0 to END
	// nIndexNotSorted points to the first not yet sorted object,
	//  so the set [0..nIndexNotSorted - 1] is already sorted
	// This algorithm searches the not yet sorted objects in the set
	//  [nIndexNotSorted..END] for the smallest object and adds this 
	//  to the sorted objects
	// nIndexCompare point to the last object in this set that has 
	//  already been examined
	// The algorithm ends when all objects are sorted
	int nIndexNotSorted = 0;
	int nIndexCompare = 0;
	COXFileSpec* pSmallestFile = NULL;	// point to array[nIndexSorted]
	COXFileSpec* pCompareFile = NULL;		// point to array[nIndexCompare]
	COXFileSpec* pSwapFile;
	
	while (nIndexNotSorted <= m_fileArray.GetUpperBound())
		{
		pSmallestFile = (COXFileSpec*)m_fileArray[nIndexNotSorted];
		nIndexCompare = nIndexNotSorted;
		nIndexCompare++;
		
		while (nIndexCompare <= m_fileArray.GetUpperBound()) 
			{
			pCompareFile = (COXFileSpec*)m_fileArray[nIndexCompare];
			if (*pCompareFile < *pSmallestFile)
				{
				pSwapFile = pSmallestFile;
				m_fileArray[nIndexNotSorted] = m_fileArray[nIndexCompare];
				m_fileArray[nIndexCompare] = pSwapFile;
				pSmallestFile = (COXFileSpec*)m_fileArray[nIndexNotSorted];
				}
			nIndexCompare++;
			}
		nIndexNotSorted++;
		}
	}
	
void COXFileList::ClearList()
	{
	COXFileSpec* pFile;
	int nIndex = 0;
	int nMaxIndex = m_fileArray.GetUpperBound();
	while (nIndex <= nMaxIndex) 
		{
		pFile = (COXFileSpec*)m_fileArray[nIndex];
		delete pFile;
		nIndex++;
		}
	m_fileArray.RemoveAll();
	}
	
#ifdef _DEBUG
void COXFileList::Dump(CDumpContext& dc) const
	{
	CObject::Dump(dc);
	dc << _T("\nm_path : ");
	m_path.Dump(dc);
	dc << _T("\nm_fileArray : ") << m_fileArray;	
	}

void COXFileList::AssertValid() const
	{
	CObject::AssertValid();
	}
#endif

COXFileList::~COXFileList()
	{
	ClearList();
	}
	
// protected:

// private:

// Message handlers ---------------------------------------------------------

// ==========================================================================
