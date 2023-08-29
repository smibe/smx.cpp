/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffDirDoc.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffDirDoc.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_DIFFDIRDOC_H__D2BCD8C5_EEFA_11D1_9E37_204C4F4F5020__INCLUDED_
#define AFX_DIFFDIRDOC_H__D2BCD8C5_EEFA_11D1_9E37_204C4F4F5020__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// 060698:GeH created.

#include "DiffDirView.h"	// Hinzugefügt von ClassView

class CDirectoryChangeWatcher;
class CDiffDirChangeHdl;
class CDirChangeHdl;
class COXPathSpec;
class CCmpDirDlg;

enum EDiffType
{
  diff_Equal  = 0,
  diff_LNewer = 1,
  diff_RNewer = 2,
  diff_LOnly  = 3,
  diff_ROnly  = 4
};

#define  DR_Diff		((WORD) 1 << 8)
#define  DR_Hide		((WORD) 1 << 9)

class CDiffRec : public CObject
{
public:
	CDiffRec(CString fileName, CTime leftDate, CTime rightDate);

	//@cmember mark this record for hide
	inline void SetHide(bool bHide)  { bHide? m_nFlag |= DR_Hide : m_nFlag &= (~ DR_Hide); };

	void SetDiff(bool bDiff) {bDiff ? m_nFlag |= DR_Diff : m_nFlag &= (~ DR_Diff); };
	bool GetDiff()           { return ((m_nFlag & DR_Diff) == DR_Diff); }
	bool GetHide()           { return ((m_nFlag & DR_Hide) == DR_Hide); }

public:
  CString   m_strFileName; //@cmember first file
	EDiffType m_eDiffType;   //@cmember (0 if eq.; 1 if file1 newer, 2 if file2 newer)
	WORD	    m_nFlag;	     //@cmember flags: DR_DIFF, DR_HIDE; etc
	CTime	    m_LeftDate;
	CTime	    m_RightDate;
  ULONG     m_nLeftSize;
  ULONG     m_nRightSize;
};

/* @class The CDiffDirDoc manages different file lists
 * 
 * @base CDocument
 */	
class CDiffDirDoc : public CDocument
{
	DECLARE_DYNCREATE(CDiffDirDoc)
public:
  struct CMP_DIR_SETTINGS
  {
		CString strDir1;
		CString strDir2;
		CString strFileMask;
	};

 	CDiffDirDoc();
	~CDiffDirDoc();

	CString       GetRightDate(int i);
	CString       GetLeftDate (int i);
	CString       GetRightSize(int i);
	CString       GetLeftSize (int i);
  CString       GetDate(CTime& rTime);
	CDiffDirView *GetDiffDirView();


public:
	//@cmember Compares two directories and stores the ancestor, 
	// the previous compare list is deleted
	bool CmpDir(CString relDir = _T(""));

	LPCTSTR   GetFileName(int itemNo);
	LPCTSTR   GetStatus(int itemNo);
	CDiffRec *GetRecAt(int itemNo);
	int ItemCount();
	inline void SetHide(int idx, bool hide) {GetRecAt(idx)->SetHide(hide);};
	inline bool GetHide(int idx) {return(GetRecAt(idx)->GetHide());};
	void UpdateHide();
  void Clear();
  void Refresh(bool bShowDialog, LPCTSTR pszFileName = NULL);
  void WatchDirectories();

  int  GetRecord(LPCTSTR pszFileName);
  bool UpdateRecord(LPCTSTR pszFileName);
  EDiffType GetDiff(COXPathSpec& File1, COXPathSpec& File2);


  //------------------ Generated message map functions ------------------
	//{{AFX_MSG(CDiffDirDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  //----------- Overrides ---------------------------
	//{{AFX_VIRTUAL(CDiffDirDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
  virtual void SetTitle( LPCTSTR lpszTitle );
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

public:
	CString          m_strDir1;         //@cmember first directory
	CString          m_strDir2;         //@cmember second directory
	CString          m_strFileMask;     //@cmember file mask
	CString          m_strExcludeMask;  //@cmember exclude mask

protected: 
	//@cmember list of the different files
	CObArray         m_DiffList;
	bool             m_bInvalidDir;     //@cmember Set to true if the directory is invalid.
	bool             m_bHideChanged;
  bool             m_bBreak;

	CDiffDirChangeHdl*        m_pDirChangeHdl;
  DWORD                     m_dwNotifyTimer;
  bool                      m_bAskingDirNotify;

  static CCmpDirDlg         *sm_pCmpDirDlg;

  CRegExp m_MaskRegExp;
  CRegExp m_ExcludeRegExp;
};


#endif
