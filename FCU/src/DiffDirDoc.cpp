/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffDirDoc.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffDirDoc.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// 060698:GeH created.

#include "stdafx.h"
#include "AfxPriv.h"
#include "direct.h"
#include "fcu.h"
#include "Util.h"
#include "FcuOptions.h"
#include "fileList.h"
#include "dirList.h"
#include "resource.h"
#include "diffDirDoc.h"
#include "CmpDirDlg.h"
#include "DirectoryChanges.h"
#include "DirChangeHdl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCTSTR szFileMask;
extern LPCTSTR szExcludeMask;

static bool IsEqual(CString strPath1, CString strPath2);
       bool InsertString(CStringArray& strArray, CString str);

CCmpDirDlg *CDiffDirDoc::sm_pCmpDirDlg = NULL;
       
//Registration
extern bool gl_bRegistered;
extern int gl_nRemindHim;
int GetEvalDays();

class CDiffDirChangeHdl : public CDirChangeHdl
{
public:
  CDiffDirChangeHdl(CDiffDirDoc *pDiffDirDoc)
  {
    m_pDiffDirDoc  = pDiffDirDoc;
  }
  virtual ~CDiffDirChangeHdl() {};

protected:
  void RefreshView()
  {
    if (!m_pDiffDirDoc)
      return;

    LOCK();
    void *pDummy = NULL;
    CString str;
    for (POSITION pos = m_FileMap.GetStartPosition(); pos; )
    {
      m_FileMap.GetNextAssoc(pos, str, pDummy);

      m_pDiffDirDoc->Refresh(true, str);
    }
    m_FileMap.RemoveAll();
  }

  virtual bool HandleNotification(CDirChangeData *pDirChangeData, DWORD dwLength)
  {
    LOCK();
    DWORD   dwOffset;
    CString str;
    CDirWatchInfo *pInfo = NULL;
    FILE_NOTIFY_INFORMATION *pFilNotify = NULL;


    if (pDirChangeData == NULL)
      RefreshView();
      
    if(dwLength == NULL)
      goto cleanup;

    pFilNotify = (FILE_NOTIFY_INFORMATION*) pDirChangeData->m_pBuffer;

    //Check if the watch was not removed
    pInfo = GetDirWatchInfo(pDirChangeData->m_strDir);
    if (!pFilNotify || pInfo == NULL)
      goto cleanup;

    do
    {
        USES_CONVERSION;
        dwOffset = pFilNotify->NextEntryOffset;

        {
          WCHAR *pBuf = new WCHAR[pFilNotify->FileNameLength / 2 + 1];
          wcsncpy(pBuf, pFilNotify->FileName, pFilNotify->FileNameLength / 2);
          pBuf[pFilNotify->FileNameLength / 2] = NULL;
          str = W2T(pBuf);
  
          if (pInfo->Match(str))
            m_FileMap.SetAt(str, 0);

          delete[] pBuf;
          pBuf = NULL;
        }

        pFilNotify = (FILE_NOTIFY_INFORMATION *)((BYTE*) pFilNotify + dwOffset);

    } while( dwOffset );

    cleanup:
      delete pDirChangeData;
      return true;
  }

protected:
  CDiffDirDoc *m_pDiffDirDoc;
  CMapStringToPtr m_FileMap;
};



IMPLEMENT_DYNCREATE(CDiffDirDoc, CDocument)

BEGIN_MESSAGE_MAP(CDiffDirDoc, CDocument)
	//{{AFX_MSG_MAP(CDiffDirDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDiffDirDoc::CDiffDirDoc() 
{
	m_bHideChanged = false; 
	m_bInvalidDir = false;
  m_bBreak      = false;
  
  m_pDirChangeHdl = NULL;

  m_dwNotifyTimer    = NULL;
  m_bAskingDirNotify = false;

}

BOOL CDiffDirDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;

	return true;
}


CDiffDirDoc::~CDiffDirDoc()
{
	Clear();

  if (m_pDirChangeHdl)
    m_pDirChangeHdl->Exit();

  delete  m_pDirChangeHdl;

}

void CDiffDirDoc::Clear()
{
	CDiffRec *rec;

	for( int i=0; i < m_DiffList.GetSize(); i++)   
	{
		rec = (CDiffRec *) m_DiffList[i];    
		m_DiffList[i] = NULL;
		delete rec;
	}
  m_DiffList.RemoveAll();
}

BOOL CDiffDirDoc::OnOpenDocument(LPCTSTR pstr)
{
	int i        = 0;
	bool bResult = true;

	if (pstr == NULL) 
		return NULL;

	if(((CString) pstr) != "DiffDir")
		return NULL;

  CheckEvaluation();

	bool bShowDialog = true;

	if (GetCmdLineInfo().m_bCmpDir)
	{
		//FCU was started with command line parameters, initialize the directory names
		TCHAR buf[MAX_PATH];
		if(_getcwd(buf, MAX_PATH))
		{
			if(buf[strlen(buf)-1] != '\\')
				strcat(buf, "\\");

			m_strDir1 = GetCmdLineInfo().m_strLeftParam;
			m_strDir2 = GetCmdLineInfo().m_strRightParam;

			if(!m_strDir1.IsEmpty() && IsRelativePath(m_strDir1))
				m_strDir1 = buf + m_strDir1;

			if(!m_strDir2.IsEmpty() && IsRelativePath(m_strDir2))
				m_strDir2 = buf + m_strDir2;
		}

		if (!GetOption(OCD_ShowCmpDir) && !m_strDir1.IsEmpty() && !m_strDir2.IsEmpty())
		{
			bShowDialog = false;
		}

	}

	do
	{
		if (bShowDialog || sm_pCmpDirDlg != NULL)
		{
			bool bNewDlg = false;
      //Open the dialog
			if (!sm_pCmpDirDlg)
      {
        sm_pCmpDirDlg = new CCmpDirDlg;
        bNewDlg = true;
      }

			if (GetCmdLineInfo().m_bCmpDir)
			{
				sm_pCmpDirDlg->m_strDir1 = m_strDir1;
				sm_pCmpDirDlg->m_strDir2 = m_strDir2;
        sm_pCmpDirDlg->m_strFileMask = GetCmdLineInfo().m_strFileMask;
        sm_pCmpDirDlg->m_strExcludeMask = GetCmdLineInfo().m_strExcludeMask;
			}

			if (!bNewDlg)
      {
        sm_pCmpDirDlg->UpdateData(FALSE);
        return FALSE;
      }

      if (sm_pCmpDirDlg->DoModal() != IDOK)
      {
        delete sm_pCmpDirDlg;
        sm_pCmpDirDlg = NULL;
        return NULL;
      }

			//Store the selected values
			m_strDir1     =  sm_pCmpDirDlg->m_strDir1;
			m_strDir2     =  sm_pCmpDirDlg->m_strDir2;
			m_strFileMask =  sm_pCmpDirDlg->m_strFileMask;
			m_strExcludeMask =  sm_pCmpDirDlg->m_strExcludeMask;

      //cleanup
      delete sm_pCmpDirDlg;
      sm_pCmpDirDlg = NULL;
		}
		else
		{
			bShowDialog = true;
      if (!GetCmdLineInfo().m_strFileMask.IsEmpty())
        m_strFileMask = GetCmdLineInfo().m_strFileMask;
      else
			  m_strFileMask = GetOptions().GetLastCBEntry(szFileMask);

      if (!GetCmdLineInfo().m_strExcludeMask.IsEmpty())
        m_strExcludeMask = GetCmdLineInfo().m_strExcludeMask;
      else
			  m_strExcludeMask = GetOptions().GetLastCBEntry(szExcludeMask);
		}


		m_bBreak = false;
    AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, IDS_ComparingDirectories);

    //Set up the directory watch
    WatchDirectories();

    //Compile the regular expressions
    if (!m_strFileMask.IsEmpty())
    {
      m_strFileMask.MakeLower();
      m_MaskRegExp.RegCompFileMask(m_strFileMask);
    }
    
    if (!m_strExcludeMask.IsEmpty())
    {
      m_strExcludeMask.MakeLower();
      m_ExcludeRegExp.RegCompFileMask(m_strExcludeMask);
    }
    
    bResult = CmpDir();
	} while (!bResult && m_bInvalidDir);

	GetCmdLineInfo().Reset();

  AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	
	return bResult;
}


CDiffRec * CDiffDirDoc::GetRecAt(int itemNo)
{
	if(0 > itemNo || itemNo > m_DiffList.GetSize())
		return NULL;

	return((CDiffRec *) m_DiffList.GetAt(itemNo));
}

LPCTSTR CDiffDirDoc::GetFileName(int itemNo)
{
	CDiffRec *rec = GetRecAt(itemNo);

	if(rec) 
		return rec->m_strFileName;
	else
		return _T("");
}

LPCTSTR CDiffDirDoc::GetStatus(int itemNo)
{
	CDiffRec *rec = GetRecAt(itemNo);

	if(!rec) 
		return _T("");

	switch(rec->m_eDiffType) 
	{
		case diff_Equal:
			return _T("identical");
		case diff_LNewer:
			return _T("left is newer");
		case diff_RNewer:
			return _T("right is newer");
		case diff_LOnly:
			return _T("left only");
		case diff_ROnly:
			return _T("right only");
		default:
			return _T("unknown");
	}
}

int CDiffDirDoc::ItemCount()
{
	return m_DiffList.GetSize();
}

bool CDiffDirDoc::CmpDir(CString relDir)
{

	int  n       = 0;					//counts the file list
	bool bResult = true;
	COXFileList fList1;
	COXFileList fList2;
	CString p1 = m_strDir1;
	CString p2 = m_strDir2;

	m_bInvalidDir = false;

	if (p1[p1.GetLength() - 1] != '\\')
		p1 += '\\';

	if (p2[p2.GetLength() - 1] != '\\')
		p2 += '\\';

	p1 += relDir;
	p2 += relDir;

	COXPathSpec p1Spec(p1);
	COXPathSpec p2Spec(p2);

	//check if the directory exists
	CString strError;
  if (relDir.IsEmpty() && !p1Spec.COXDirSpec::Exists())
	{
		strError.Format("The directory %s does not exist!", p1);
		AfxMessageBox(strError);

		m_bInvalidDir = true; //Open the compare dialog once again.
		return false;
	}
		
	if (relDir.IsEmpty() && !p2Spec.COXDirSpec::Exists())
	{
		strError.Format("The directory %s does not exist!", p2);
		AfxMessageBox(strError);
		
		//Open the compare dialog once again.
		m_bInvalidDir = true; 
		return false;
	}
	
  if (relDir.Find("CVS") != -1)
  {
    int XX = 1;
  }

  CRegExp *pExcludeRegExp = NULL;
  CRegExp *pMaskRegExp = NULL;

  if (!m_strExcludeMask.IsEmpty())
    pExcludeRegExp = & m_ExcludeRegExp;

	if (!m_strFileMask.IsEmpty())
    pMaskRegExp = & m_MaskRegExp;

  if(fList1.SetPath(p1Spec))
	{
    fList1.Search(m_strFileMask, relDir, pMaskRegExp, pExcludeRegExp);
		fList1.Sort();
	}

	if(fList2.SetPath(p2Spec))
	{
		fList2.Search(m_strFileMask, relDir, pMaskRegExp, pExcludeRegExp);
		fList2.Sort();
	}

	const CPtrArray *fArray1 = fList1.GetList();
	const CPtrArray *fArray2 = fList2.GetList();

	int arSize1 = fArray1->GetSize();
	int arSize2 = fArray2->GetSize();


  ASSERT(arSize1 >= 0 || arSize2 >= 0);

	CDiffRec *rec;
	CString file1, file2;

	int oldSize = m_DiffList.GetSize();
	m_DiffList.SetSize(oldSize + arSize1 + arSize2);
	n = oldSize;
	int i=0,j=0;
	MSG msg;

	BeginWaitCursor();
	while(i+j < arSize1 + arSize2) 
	{
    // Allow interrupting the execution.
		while( ::PeekMessage (&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE))
		{
			//NOTE: all WM_KEYDOWN messages will be removed.
			if (msg.wParam == VK_CANCEL)
			{
				if (AfxMessageBox("Break received! Abort directory compare?", MB_YESNO) == IDYES)
					return false;
			}
		}

		ASSERT(i<=arSize1);
		ASSERT(j<=arSize2);

		const COXFileSpec *fSpec1 = (i<arSize1) ? fList1.GetAt(i) : NULL;
		const COXFileSpec *fSpec2 = (j<arSize2) ? fList2.GetAt(j) : NULL;

		file1 = fSpec1 ? fSpec1->GetFileName() : "";
		file2 = fSpec2 ? fSpec2->GetFileName() : "";
		CTime dat1, dat2;
		dat1 = fSpec1 ? fSpec1->GetTime():0;
		dat2 = fSpec2 ? fSpec2->GetTime():0;
    long  nSize1 = fSpec1 ? fSpec1->GetLength() : -1;
    long  nSize2 = fSpec2 ? fSpec2->GetLength() : -1;

    if(!file1.CompareNoCase(file2) && file1 != "") 
    {

			ASSERT(fSpec1);
			ASSERT(fSpec2);

			CString strPath1 = m_strDir1 + "\\" + relDir + file1;
			CString strPath2 = m_strDir2 + "\\" + relDir + file2;
			//found equal filenames, check the files
			rec = new CDiffRec(relDir + file1, dat1, dat2);
      rec->m_nLeftSize  = nSize1;
      rec->m_nRightSize = nSize2;
      
      //rec->m_eDiffType = GetDiff(fSpec1, fSpec2);
      //rec->SetDiff(rec->m_eDiffType != diff_Equal);

      bool bEqual = (*fSpec1 == *fSpec2) != NULL;     //Comparing size here

      if (GetOption(OCD_UseSizeTime))
      {
        if (!GetOption(OMSC_IgnoreBlanks) && !GetOption(OMSC_IgnoreEOL))
          bEqual = bEqual && IsEqual(strPath1, strPath2);
        else
          bEqual = IsEqual(strPath1, strPath2);
      }
      else
      {
        bool bCompare = false;

        if (!bEqual && (GetOptions().GetIgnoreBlanks() || GetOptions().GetIgnoreEOL()) )
        {
          //The files have different sizes, but it still could be that they are equal
          bCompare = true;
        }

        if (bEqual && dat1 != dat2)
        {
          //The dates are different, but the files still could be equal!
          bCompare = true;
        }

        if (bCompare)
          bEqual = IsEqual(strPath1, strPath2);
      }

      if (bEqual)
			{
				rec->SetDiff(false);
				rec->m_eDiffType = diff_Equal;
			} 
      else 
			{
				rec->SetDiff(true);
				if(dat1 > dat2) 
					rec->m_eDiffType = diff_LNewer;
				else
					rec->m_eDiffType = diff_RNewer;
			}
			i++; j++;
		} 
		else if (file1.CompareNoCase(file2) > 0 && file2 != "") 
		{
			rec = new CDiffRec(relDir + file2, 0, dat2);
      rec->m_nLeftSize  = -1;
      rec->m_nRightSize = nSize2;
			rec->m_eDiffType = diff_ROnly;
			j++;
		}
		else if (file1 != "") 
		{
			rec = new CDiffRec(relDir + file1, dat1, 0);
      rec->m_nLeftSize  = nSize1;
      rec->m_nRightSize = -1;
			rec->m_eDiffType = diff_LOnly;
			i++;
		} 
    else if (file2 != "") 
		{
			rec = new CDiffRec(relDir + file2, 0, dat2);
      rec->m_nLeftSize  = -1;
      rec->m_nRightSize = nSize2;
			rec->m_eDiffType = diff_ROnly;
			j++;
		} 
		else
			ASSERT(false);

		m_DiffList[n++] = rec;
	}
	EndWaitCursor();

	m_DiffList.SetSize(n);

	//Return if no subdirectory search requested
	if (!GetOption(OCD_Subdir))
	{
		bResult = true;
		goto cleanup;
	}
	else 
	{
		//Search the subdirectories
		COXDirList dList1;
		COXDirList dList2;
		CString dir1, dir2;

		if(dList1.SetPath(p1Spec))
		{
			dList1.Search();
			dList1.Sort();
		}

		if(dList2.SetPath(p2Spec))
		{
			dList2.Search();
			dList2.Sort();
		}

		fArray1 = dList1.GetList();
		fArray2 = dList2.GetList();

		arSize1 = fArray1->GetSize();
		arSize2 = fArray2->GetSize();
		if(arSize1 < 0 || arSize2 < 0)
			return false;

		i=0,j=0;
		while(i + j < arSize1 + arSize2) 
		{

			ASSERT(i <= arSize1);
			ASSERT(j <= arSize2);

			const COXDirSpec *dSpec1 = (i<arSize1) ? dList1.GetAt(i) : NULL;
			const COXDirSpec *dSpec2 = (j<arSize2) ? dList2.GetAt(j) : NULL;

			dir1 = dSpec1 ? dSpec1->GetLastDirectory() : "";
			dir2 = dSpec2 ? dSpec2->GetLastDirectory() : "";

			if(!dir1.CollateNoCase(dir2) && dir1 != "") 
			{
				//found equal directory names, compare them
				if(!CmpDir(relDir + dir1))
					return false;
				i++; j++;
			} 
			else if (dir1 > dir2 && dir2 != "") 
			{
				if(!CmpDir(relDir + dir2))
					return false;
				j++;
			} 
			else if (dir1 != "") 
			{
				if(!CmpDir(relDir + dir1))
					return false;
				i++;
			} 
			else if (dir2 != "") 
			{
				if(!CmpDir(relDir + dir2))
					return false;
				j++;
			} 
			else
				ASSERT(false);
		}
	}

cleanup:
	UpdateHide();
	return(bResult);
}

/////////////////////////////////////////////////////////////////////////////
// Serialisierung CDiffDirDoc 

void CDiffDirDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: no loading possible
	}
}



CDiffRec::CDiffRec(CString fileName, CTime leftDate, CTime rightDate)
{
	m_strFileName  = fileName;
	m_eDiffType    = diff_Equal;
	m_nFlag         = 0;
	m_LeftDate  = leftDate;
	m_RightDate = rightDate;
}

void CDiffDirDoc::OnCloseDocument() 
{

	CDocument::OnCloseDocument();
}

void CDiffDirDoc::UpdateHide()
{
	CDiffRec *rec;
  bool      bChanged = false;

	for (int i=0; i < ItemCount(); i++) {
		rec = GetRecAt(i);

		if(!rec) 
			continue;

		switch(rec->m_eDiffType) 
    {
			case diff_Equal:
				if (rec->GetHide() != (!GetOption(OCD_Identical)))
        {
          rec->SetHide(!GetOption(OCD_Identical));
          bChanged = true;
        }
				break;
			case diff_LNewer:
			case diff_RNewer:
				if (rec->GetHide() != (!GetOption(OCD_Different)))
        {
		  		rec->SetHide(!GetOption(OCD_Different));
          bChanged = true;
        }
				break;
			case diff_LOnly:
				if (rec->GetHide() != (!GetOption(OCD_LeftOnly)))
        {
	  			rec->SetHide(!GetOption(OCD_LeftOnly));
          bChanged = true;
        }
				break;
			case diff_ROnly:
				if (rec->GetHide() != (!GetOption(OCD_RightOnly)))
        {
  				rec->SetHide(!GetOption(OCD_RightOnly));
          bChanged = true;
        }
				break;
		}
	}
	
  if (bChanged)
    UpdateAllViews(NULL);
};

CDiffDirView *CDiffDirDoc::GetDiffDirView()
{
	POSITION pos = GetFirstViewPosition();
	if(pos)
		return((CDiffDirView *) GetNextView(pos));
	else
		return NULL;
}

bool IsEqual(CString strPath1, CString strPath2)
{
	FILE *pFile1 = NULL;
	FILE *pFile2 = NULL;
	bool  bDiff  = false;

	pFile1 = fopen(strPath1, "rb");

	pFile2 = fopen(strPath2, "rb");

	if(!pFile1 || !pFile2) 
	{
		//One of the directories does not exist.
		bDiff = true;
		goto cleanup;
	}

	int c1, c2;
	while(!feof(pFile1) && !feof(pFile2))
	{
    c1 = getc(pFile1);
    c2 = getc(pFile2);

    if (c1 == 0x0a)
    {
      int XXX = 1;
    }

		if(GetOptions().GetIgnoreBlanks()) 
		{
			while(c1 == ' ' || c1 == '\t')
				c1 = getc(pFile1);

			while(c2 == ' ' || c2 == '\t')
				c2 = getc(pFile2);
		}

		if(GetOptions().GetIgnoreEOL()) 
		{
			while(c1 == 0x0a || c1 == 0x0d)
				c1 = getc(pFile1);

			while(c2 == 0x0a || c2 == 0x0d)
				c2 = getc(pFile2);
		}

		if (GetOptions().GetIgnoreCase())
    {
      if(toupper(c1) != toupper(c2)) 
		  {
			  bDiff = true;
			  break;
		  }
    }
    else
    {
      if(c1 != c2) 
		  {
			  bDiff = true;
			  break;
		  }
    }
	}

cleanup:
	if(pFile1)
		fclose (pFile1);
	if(pFile2)
		fclose (pFile2);
	return (!bDiff);
}


CString CDiffDirDoc::GetRightDate(int itemNo)
{
	CDiffRec *rec = GetRecAt(itemNo);

	if(rec && rec->m_RightDate != 0)  
    return GetDate(rec->m_RightDate);
	else
		return _T("");

}

CString CDiffDirDoc::GetLeftDate(int itemNo)
{
	CDiffRec *rec = GetRecAt(itemNo);

	if(rec && rec->m_LeftDate != 0)  
  {
    return GetDate(rec->m_LeftDate);
  }
	else
  {
		return _T("");
  }

}

CString CDiffDirDoc::GetDate(CTime& rTime)
{
  if (GetOptions().GetOption(OCD_FilTime))
  {
    if (GetOptions().GetOption(OCD_FilSeconds))
		  return rTime.Format("%x  %H:%M:%S");
    else
		  return rTime.Format("%x  %H:%M");
  }
  else
		return rTime.Format("%x");
}

CString CDiffDirDoc::GetLeftSize(int itemNo)
{
	CDiffRec *rec = GetRecAt(itemNo);
  CString str;

	if(rec && rec->m_nLeftSize != -1)  
  {
		str.Format("%0.2lf KB ", (double) rec->m_nLeftSize / 1024.0);
  }
  return str;
}

CString CDiffDirDoc::GetRightSize(int itemNo)
{
	CDiffRec *rec = GetRecAt(itemNo);
  CString str;

	if(rec && rec->m_nRightSize != -1)  
		str.Format("%0.2lf KB ", (double) rec->m_nRightSize / 1024.0);

  return str;
}

void CDiffDirDoc::SetTitle( LPCTSTR lpszTitle )
{
  UNREFERENCED_PARAMETER(lpszTitle);
  
  //We don't want the default MFC title, set our own!!
  CDocument::SetTitle(m_strDir1 + " : " + m_strDir2);
}

void CDiffDirDoc::Refresh(bool bShowDialog, LPCTSTR pszFileName)
{
  if (pszFileName != NULL &&  *pszFileName != 0)
  {
    if (UpdateRecord(pszFileName))
    {
      return;
    }
  }

  if (bShowDialog && GetOptions().GetOption(OCD_FilNotifyDlg))
  {
    int nResult = IDYES;

    if (m_bAskingDirNotify)
      return;

    m_bAskingDirNotify = true;
    nResult = AfxMessageBox("The files have changed on disk. You want toe refresh the view?", MB_ICONQUESTION | MB_YESNO);
    m_bAskingDirNotify = false;
    if (nResult == IDNO)
      return;
  }

  AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, IDS_ComparingDirectories);

  Clear();
	CmpDir();
  AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

}

void CDiffDirDoc::WatchDirectories()
{
  if (GetOptions().GetOption(OCD_FilChangeNotify))
  {
	  DWORD dwChangeFilter = 
        FILE_NOTIFY_CHANGE_FILE_NAME  | 
		    FILE_NOTIFY_CHANGE_DIR_NAME   |
		    FILE_NOTIFY_CHANGE_LAST_WRITE;

    if (m_pDirChangeHdl == NULL)
    {
	    m_pDirChangeHdl = new CDiffDirChangeHdl(this);

      VERIFY( m_pDirChangeHdl->Initialize() );
    }
  
    m_pDirChangeHdl->AddDirWatch(m_strDir1, dwChangeFilter, GetOption(OCD_Subdir), m_strFileMask, m_strExcludeMask);
    m_pDirChangeHdl->AddDirWatch(m_strDir2, dwChangeFilter, GetOption(OCD_Subdir), m_strFileMask, m_strExcludeMask);
  }
  else
  {
    if (m_pDirChangeHdl)
      m_pDirChangeHdl->RemoveAllWatches();
  }
}


bool CDiffDirDoc::UpdateRecord(LPCTSTR pszFileName)
{
  int       nIdx = GetRecord(pszFileName);
  CDiffRec *pRec = NULL;
  if (nIdx != -1)
  {
 		pRec   = (CDiffRec *) m_DiffList[nIdx];
    COXPathSpec fSpec1(m_strDir1 + '\\' + pRec->m_strFileName);
		COXPathSpec fSpec2(m_strDir2 + '\\' + pRec->m_strFileName);
    BOOL bEmpty1 = !fSpec1.Exists();
    BOOL bEmpty2 = !fSpec2.Exists();

    if (fSpec1.Exists())
      fSpec1.DoGetInfo();
    else 
      fSpec1.SetPath("");

    if (fSpec2.Exists())
      fSpec2.DoGetInfo();
    else 
      fSpec2.SetPath("");

    if (bEmpty1 && bEmpty2)
      return false;

    if (bEmpty1 && !bEmpty2)
    {
      pRec->m_eDiffType = diff_ROnly;
      pRec->m_nLeftSize = -1;
      pRec->m_LeftDate = 0;
      pRec->m_nRightSize = fSpec2.GetLength();
      pRec->m_RightDate  = fSpec2.GetTime();
    }
    else if (!bEmpty1 && bEmpty2)
    {
      pRec->m_eDiffType = diff_LOnly;
      pRec->m_nLeftSize = fSpec1.GetLength();
      pRec->m_LeftDate  = fSpec1.GetTime();
      pRec->m_nRightSize = -1;
      pRec->m_RightDate  = 0;
    }
    else
    {
      pRec->m_eDiffType = GetDiff(fSpec1, fSpec2);
      pRec->m_nLeftSize = fSpec1.GetLength();
      pRec->m_LeftDate  = fSpec1.GetTime();
      pRec->m_nRightSize = fSpec2.GetLength();
      pRec->m_RightDate  = fSpec2.GetTime();
    }
  }
  else
  {
    return false;
  }
  
  if (pRec)
  {
    switch(pRec->m_eDiffType)
    {
      case diff_LOnly:
        pRec->SetHide(!GetOption(OCD_LeftOnly));
        break;
      case diff_ROnly:
        pRec->SetHide(!GetOption(OCD_RightOnly));
        break;
      case diff_LNewer:
      case diff_RNewer:
        pRec->SetHide(!GetOption(OCD_Different));
        break;
      case diff_Equal:
        pRec->SetHide(!GetOption(OCD_Identical));
        break;
    }
  }

  GetDiffDirView()->UpdateRecord(nIdx);
  return true;
}


int CDiffDirDoc::GetRecord(LPCTSTR pszFileName)
{
	CDiffRec *pRec = NULL;

	for( int i=0; i < m_DiffList.GetSize(); i++)   
	{
		pRec = (CDiffRec *) m_DiffList[i];
    if (!pRec)
      continue;
  
    if (pRec->m_strFileName.CompareNoCase(pszFileName) == 0)
      return i;
  }

  return -1;
}

EDiffType CDiffDirDoc::GetDiff(COXPathSpec& File1, COXPathSpec& File2)
{
  bool bEqual = (File1 == File2) != NULL;     //Comparing size here
  
  CString strPath1 = File1.GetPath();
  CString strPath2 = File2.GetPath();

  if (GetOption(OCD_UseSizeTime))
  {
    if (!GetOption(OMSC_IgnoreBlanks) && !GetOption(OMSC_IgnoreEOL))
      bEqual = bEqual && IsEqual(strPath1, strPath2);
    else
      bEqual = IsEqual(strPath1, strPath2);
  }
  else
  {
    bool bCompare = false;

    if (!bEqual && (GetOptions().GetIgnoreBlanks() || GetOptions().GetIgnoreEOL()) )
    {
      //The files have different sizes, but it still could be that they are equal
      bCompare = true;
    }

    if (bEqual && File1.GetTime() != File2.GetTime())
    {
      //The dates are different, but the files still could be equal!
      bCompare = true;
    }

    if (bCompare)
      bEqual = IsEqual(strPath1, strPath2);
  }
  
  if (!bEqual)
  {
	  if(File1.GetTime() > File2.GetTime()) 
		  return diff_LNewer;
	  else
		  return diff_RNewer;
  }

	return diff_Equal;
}