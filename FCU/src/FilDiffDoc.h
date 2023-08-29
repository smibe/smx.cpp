/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffDoc.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffDoc.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_FILDIFFDOC_H__0D5EEC22_0510_11D2_9E5A_3068BF000000__INCLUDED_
#define AFX_FILDIFFDOC_H__0D5EEC22_0510_11D2_9E5A_3068BF000000__INCLUDED_


//includes for comparing two files (from windiff)
#include "List.h"
#include "Line.h"
#include "Section.h"
#include "DiffSectElt.h"

enum ELineState;
class CCmpFilDlg;
class CXmlExport;

class CTextSect
{
public:
  CTextSect() 
  {
    m_nStart = 0;
    m_nLength = 0;
    m_pszText = 0;
  }

  CTextSect(WORD nStart, WORD nLength, LPCTSTR pszText)
  {
    m_nStart = nStart;
    m_nLength = nLength;
    m_pszText = m_pszText;
  };

  BOOL operator==(const CTextSect& rOther) const
  {
    if (m_nLength == rOther.m_nLength)
    {
      for (int i = 0; i < rOther.m_nLength; i++)
      {
        if (m_pszText[m_nStart +i] != rOther.m_pszText[rOther.m_nStart + i])
          return FALSE;
      }
      return TRUE;
    }
    return FALSE;
  }

  WORD m_nStart;
  WORD m_nLength;
  LPCTSTR m_pszText;
};

typedef CMap<CTextSect, CTextSect, CTextSect, CTextSect> CTextSectMap;


class CFilDiffDoc : public CDocument
{
DECLARE_DYNCREATE(CFilDiffDoc)
//@access Public member functions
public:
	CFilDiffDoc();                  //@cmember Constructor
	virtual ~CFilDiffDoc();         //@cmember Destructor
  void Reset();

	bool DoCompareFiles();            //@cmember  Compare the files.

  CObArray* GetDiffArray()  { return &m_secDiffList; };

	CDiffSectElt * GetActualSect();
	BOOL    NextDiff();
	BOOL    PrevDiff();
	int     GetDiffNo()  { return m_secDiffList.GetUpperBound()+1; };
	int     GetActDiff() { return m_curDiffIdx+1; };
	LPCTSTR GetFileName();
	UINT    SaveComposed();

  PLINEDESC GetSectAfter(CDiffSectElt *pSect);
  PLINEDESC GetSectAfter(int nDiffIdx);

  void SetRefSect(int nIndex, PLINEDESC pLine, bool bNextMovedUp = false, PLINEDESC pStart = NULL, PLINEDESC pEnd = NULL);
	bool GetRefSect(int& nIdxStart, PLINEDESC pLine);
	bool VerifyDiffArray();

  void OnDeleteLine(PLINEDESC pLine);
	void OnDeleteLine(PLINEDESC pStart, PLINEDESC pEnd);
	void OnJoinLines(PTEXTPOS pTextPos, PLINEDESC pOther);
	void OnSplitLines(PLINEDESC pLine);

  void UpdateLineParam(PLINEDESC& pLeft,  PLINEDESC&  pRight, 
                       PLINEDESC& pMerge, PLINEDESC&  pLine, SECEdit* pEdit = NULL);
  void GetLinkedLines(PLINEDESC& pLeft, PLINEDESC& pRight, PLINEDESC& pMerge, 
                      SECEdit *pEdit, PLINEDESC pLine);
  PLINEDESC CheckIfFolded(PLINEDESC pLine, SECEdit *pEdit);


  // ------------- Get functions ------------------
  const CString& GetLeftFileName() { return m_strLeftFileName; }
  const CString& GetRightFileName() { return m_strRightFileName; }
  const CString& GetMergeFileName() { return m_strMergeFileName; }

  //--------------- Syncronization functions -------------------------
	PLINEDESC SyncLeftWithMerge(PLINEDESC pLine);
	PLINEDESC SyncRightWithMerge(PLINEDESC pLine);
	PLINEDESC SyncMergeWithLeft(PLINEDESC pLine);
	PLINEDESC SyncMergeWithRight(PLINEDESC pLine);

  void XmlExport();
  void XmlSection(CXmlExport& Exp, DIFF_SECT *pDiffSect);
  void XmlLines(CXmlExport& Exp, PLINEDESC pStart, PLINEDESC pEnd);
  
  void UpdateFolding();
  void Refresh();
  int           GetNextDiffIdxFromPos(SECEdit *pEdit, PLINEDESC pLine);
  CDiffSectElt *GetNextDiffFromPos(SECEdit *pEdit, PLINEDESC pLine, bool bSetCurrent = false);
  
  bool CompareLines(PLINEDESC pLLine, PLINEDESC pRLine, CUIntArray *pLSect, CUIntArray *pRSect);

  //--------------- Virtual functions --------------------------------
  //{{AFX_VIRTUAL(CFilDiffDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
  virtual BOOL OnNewDocument( );
  virtual BOOL SaveModified();
  virtual void SetTitle( LPCTSTR lpszTitle );
	//}}AFX_VIRTUAL

protected:
	void SetLineState(LIST secLst, bool bRight);
	int  GetDiffSectSize();
	void GetDiffPos(SECTION sect, PLINEDESC& pStart, PLINEDESC& pEnd);
	void InitDiffList();
  void InitDiffList(CObArray& SecDiffList);
  bool CompareFiles();
	void MakeComposedEdit();
  void MakeComposedEdit(SECEdit& MergeEdit, bool bLink);
	int  SaveComposedFile(CString fileName);
  int  GetDisplayLineNo(PLINEDESC pLine);

	void ReplaceRef(int nIndex, PLINEDESC pLine, PLINEDESC pSetLine);
	bool MoveRefDown(int nIndex, PLINEDESC pLine);
  PLINEDESC CFilDiffDoc::MoveForward(PLINEDESC pStart, int nCount);
  PLINEDESC CFilDiffDoc::MoveBackward(PLINEDESC pStart, int nCount);
  int GetOffset(SECEdit *pEdit, bool bSecond);

  void OnFoldedLines(SECEdit *pEdit, PLINEDESC pLine);
  void OnUnFoldLines(SECEdit *pEdit, PLINEDESC pLine);

  void CreateSect(BYTE *pMatch, int nLength, CUIntArray* pSect);
  void MatchStrings(int nStrLen, LPCTSTR pszLeft, LPCTSTR pszRight, BYTE *pLMatch, BYTE *pRMatch);
  void CreateTextSectMap(int nStrLen, LPCTSTR pszText, BYTE *pMatch, CTextSectMap& Map);
  void MarkMatch(LPCTSTR pszText1, LPCTSTR pszText2, BYTE *pMatch1, 
                            BYTE *pMatch2, WORD nPos1, WORD nPos2);
  bool Match(CTextSectMap& Map1, CTextSectMap& Map2, BYTE *pMatch1, BYTE *pMatch2);



	//------------------ Message map functions ---------------
  //{{AFX_MSG(CFilDiffDoc)
	afx_msg void OnEditCmdmode();
	afx_msg void OnUpdateEditCmdmode(CCmdUI* pCmdUI);
	afx_msg void OnBreak();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  void FoldEqualSections(SECEdit *pEdit);

//@access Public attributes.
public:
	SECEdit    m_leftEdit;
	SECEdit    m_rightEdit;
	SECEdit    m_mergeEdit;
	bool       m_bDiscard;          // true if not alloc-ed on list

protected:
  CString    m_strRelDir;
	CString    m_strFileName;
	CString    m_strRightFileName;
	CString    m_strLeftFileName;
	CString    m_strMergeFileName;
	LIST       m_secsComposite;     // list of sections (composite file)
	LIST       m_secsLeft;          // list of sections (left file)
	LIST       m_secsRight;         // list of sections (right file)
	ELineState m_state;             // compitem state - result of compare
	LPSTR      m_pszTag;              // text for tag (title of compitem)
	LPSTR      m_pszResult;           // text equivalent of state

	int      m_curDiffIdx;
	CObArray m_secDiffList;
  bool     m_bBreak;

  static CCmpFilDlg *sm_pCmpFilDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_FILDIFFDOC_H__0D5EEC22_0510_11D2_9E5A_3068BF000000__INCLUDED_
