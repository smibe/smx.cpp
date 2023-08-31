/*******************************************************************************
// Copyright (C), GeH
// All rights reserved.
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffDoc.cpp,v $
//
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffDoc.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "AfxPriv.h"
#include "direct.h"
#include "Resource.h"
#include "FilDiff_res.h"
#include "Util.h"
#include "SaveCompDlg.h"
#include "FcuOptions.h"
#include "FilDiffParam.h"
#include "cmpfildlg.h"
#include "list.h"
#include "state.h"
#include "section.h"
#include "FilDiffDoc.h"
#include "Path.h"

#include "XmlExport.h"
#define SMX_STRCODE
#include "FilDiffXmlTags.h"
#undef SMX_STRCODE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCmpFilDlg* CFilDiffDoc::sm_pCmpFilDlg = NULL;


IMPLEMENT_DYNCREATE(CFilDiffDoc, CDocument)

CFilDiffDoc::CFilDiffDoc()
{
	m_secsLeft = NULL;
	m_secsRight = NULL;
	m_secsComposite = NULL;
	m_strFileName = "";
	m_curDiffIdx = -1;
	m_bBreak = false;
}

CFilDiffDoc::~CFilDiffDoc()
{
	Reset();
}

void CFilDiffDoc::Reset()
{
	m_curDiffIdx = -1;

	if (m_secsComposite) {
		section_deletelist(m_secsComposite);
		m_secsComposite = NULL;
	}
	if (m_secsLeft) {
		section_deletelist(m_secsLeft);
		m_secsLeft = NULL;
	}
	if (m_secsRight) {
		section_deletelist(m_secsRight);
		m_secsRight = NULL;
	}

	for (int i = 0; i <= m_secDiffList.GetUpperBound(); i++)
	{
		delete m_secDiffList[i];
	}
	m_secDiffList.RemoveAll();
}


BEGIN_MESSAGE_MAP(CFilDiffDoc, CDocument)
	//{{AFX_MSG_MAP(CFilDiffDoc)
	ON_COMMAND(ID_EDIT_CMDMODE, OnEditCmdmode)
	ON_COMMAND(ID_BREAK, OnBreak)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CMDMODE, OnUpdateEditCmdmode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFilDiffDoc::OnBreak()
{
	m_bBreak = true;
}

BOOL CFilDiffDoc::OnNewDocument()
{
	return OnOpenDocument(NULL);
}


BOOL CFilDiffDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString fileName;
	BOOL left = TRUE;
	BOOL right = TRUE;

	SECEdit::sm_nFoldedState = STATE_FOLDED;
	SECEdit::sm_nUnFoldedState = STATE_SAME;

#ifdef _CHECK_EVALUATION
	CheckEvaluation();
#endif

	CFilDiffParam* pParam = CFilDiffParam::FromString(lpszPathName);

	bool bShowDialog = false;

	if (pParam && pParam->FromCmdLine())
	{
		//FCU was started with command line parameters, initialize the file names
		CString strCurDir;
		bool bOk = _getcwd(strCurDir.GetBuffer(MAX_PATH), MAX_PATH) != FALSE;
		strCurDir.ReleaseBuffer();

		if (bOk)
		{
			AddBackslashToDir(strCurDir);

			m_strLeftFileName = pParam->GetFile1();
			m_strRightFileName = pParam->GetFile2();

			if (!m_strLeftFileName.IsEmpty() && IsRelativePath(m_strLeftFileName))
				m_strLeftFileName = strCurDir + m_strLeftFileName;

			if (!m_strRightFileName.IsEmpty() && IsRelativePath(m_strRightFileName))
				m_strRightFileName = strCurDir + m_strRightFileName;
		}

		if (GetOption(OCF_ShowCmpFile) || m_strLeftFileName.IsEmpty() || m_strRightFileName.IsEmpty())
		{
			bShowDialog = true;
		}

	}
	else
	{
		if (pParam == NULL || pParam->GetFile1().IsEmpty() || pParam->GetFile2().IsEmpty())
		{
			bShowDialog = true;
		}

		if (pParam)
		{
			m_strRelDir = pParam->GetRelDir();
			m_strLeftFileName = pParam->GetFile1();
			m_strRightFileName = pParam->GetFile2();
		}
	}

	if (bShowDialog || sm_pCmpFilDlg)
	{
		bool bNew = false;
		//Open the dialog box
		if (!sm_pCmpFilDlg)
		{
			sm_pCmpFilDlg = new CCmpFilDlg;
			bNew = true;
		}

		sm_pCmpFilDlg->m_strLeftFileName = m_strLeftFileName;
		sm_pCmpFilDlg->m_strRightFileName = m_strRightFileName;

		if (!bNew)
		{
			sm_pCmpFilDlg->UpdateData(FALSE);
			return NULL;
		}

		if (sm_pCmpFilDlg->DoModal() != IDOK)
		{
			delete sm_pCmpFilDlg;
			sm_pCmpFilDlg = NULL;
			return (FALSE);
		}

		m_strLeftFileName = sm_pCmpFilDlg->m_strLeftFileName;
		m_strRightFileName = sm_pCmpFilDlg->m_strRightFileName;
		m_strMergeFileName.Empty();

		delete sm_pCmpFilDlg;
		sm_pCmpFilDlg = NULL;
	}

	m_strFileName = GetFileNameFromPath(m_strLeftFileName);

	SetTitle(NULL);
	return CompareFiles();
}

bool CFilDiffDoc::CompareFiles()
{
	CString strErr;
	int nRetLeft = m_leftEdit.ReadFile(m_strLeftFileName);
	int nRetRight = m_rightEdit.ReadFile(m_strRightFileName);

	if ((nRetLeft || nRetRight))
	{
		if (nRetLeft == IDS_OE_CANTREAD || nRetLeft == ERROR_FILE_NOT_FOUND || nRetLeft == ERROR_PATH_NOT_FOUND)
			nRetLeft = 0;

		if (nRetRight == IDS_OE_CANTREAD || nRetRight == ERROR_FILE_NOT_FOUND || nRetRight == ERROR_PATH_NOT_FOUND)
			nRetRight = 0;
	}

	int nError = nRetLeft ? nRetLeft : nRetRight;
	if (nError)
	{
		if (!strErr.LoadString(nError))
		{
			//nRetLeft is the last system error occured, report it.
			if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nRetLeft, 0, strErr.GetBuffer(256), 256, NULL))
				strErr.ReleaseBuffer();
			else
				strErr.LoadString(IDS_UnknownErrorWhileReadingFile);
		}

		CString strMsg;
		if (nRetLeft)
			AfxFormatString1(strMsg, IDS_ErrorReadingLeftFile, strErr);
		else
			AfxFormatString1(strMsg, IDS_ErrorReadingRightFile, strErr);

		AfxMessageBox(strMsg);
		return FALSE;
	}
	m_leftEdit.SetReadOnly(TRUE);
	m_rightEdit.SetReadOnly(TRUE);

	BeginWaitCursor();
	m_bBreak = false;
	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, IDS_ComparingFiles);

	//compare the two files, informations are stored in
	//the EditCtrl's
	bool bResult = DoCompareFiles();
	if (bResult)
	{
		//SetLineState();
		MakeComposedEdit();

		//Initialize the Diff List
		InitDiffList();
	}

	FoldEqualSections(&m_leftEdit);
	FoldEqualSections(&m_rightEdit);

	m_mergeEdit.SetNotifyObject(this);
	m_mergeEdit.m_pOnFoldedLines = (LineChgFP)&CFilDiffDoc::OnFoldedLines;
	m_mergeEdit.m_pOnUnFoldLine = (LineChgFP)&CFilDiffDoc::OnUnFoldLines;

	FoldEqualSections(&m_mergeEdit);

	//Reset the status text.
	if (AfxGetMainWnd())
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);
	EndWaitCursor();

	if (!bResult)
		return FALSE;

	if (!CDocument::OnNewDocument())
		return FALSE;

	return bResult;
}

/***************************************************************************
 * Function: ci_compare
 *
 * Purpose:
 *
 * Compare files and build a composite list.
 *
 * Comments:
 *
 * Comparison method:
 *
 *    0   Break each file into lines and hash each line.  Lines which
 *        don't match can be rapidly eliminated by comparing the hash code.
 *
 *        Store the hash codes in a binary search tree that
 *        will give for each hash code the number of times that it
 *        occurred in each file and one of the lines where it occurred
 *        in each file.  The tree is used to rapidly find the partner
 *        of a line which occurs exactly once in each file.
 *
 *    1   Make a section covering the whole file (for both)
 *        and link unique lines between these sections (i.e. link lines
 *        which occur exactly once in each file as they must match each other).
 *        These are referred to as anchor points.
 *
 *    2   Build section lists for both files by traversing line lists and
 *        making a section for each set of adjacent lines that are unmatched
 *        and for each set of adjacent lines that match a set of adjacent
 *        lines in the other file.  In making a section we start from a
 *        known matching line and work both forwards and backwards through
 *        the file including lines which match, whether they are unique or not.
 *
 *    3   Establish links between sections that match
 *        and also between sections that don't match but do
 *        correspond (by position in file between matching sections)
 *
 *    4   For each section pair that don't match but do correspond,
 *        link up matching lines unique within that section.  (i.e. do
 *        the whole algorithm again on just this section).
 *
 *    There may be some lines which occur many times over in each file.
 *    As these occurrences are matched up, so the number left to match
 *    reduces, and may reach one in each file.  At this point these two
 *    can be matched.  Therefore we...
 *
 *    Repeat steps 0-4 until no more new links are added, but (especially
 *    in step 0) we only bother with lines which have not yet been matched.
 *    This means that a line which has only one unmatched instance in each
 *    file gets a count of one and so is a new anchor point.
 *
 *    Finally build a composite list from the two lists of sections.
 *
 ***************************************************************************/
bool CFilDiffDoc::DoCompareFiles()
{
	PLINEDESC lines_left, lines_right;
	SECTION whole_left, whole_right;
	BOOL bChanges;

	/* get the list of lines for each file */
	lines_left = m_leftEdit.m_pLine;
	lines_right = m_rightEdit.m_pLine;

	if ((lines_left == NULL) || (lines_right == NULL))
	{
		m_secsLeft = NULL;
		m_secsRight = NULL;
		m_secsComposite = NULL;
		return FALSE;
	}

	MSG msg;

	do
	{
		// Allow interrupting the execution.
		while (::PeekMessage(&msg, NULL, WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE))
		{
			//NOTE: all WM_KEYDOWN messages will be removed.
			if (msg.wParam == VK_CANCEL)
			{
				if (AfxMessageBox(IDS_FileCmpBreakReceived, MB_YESNO) == IDYES)
					return FALSE;
			}
		}

		/* we have made no changes so far this time round the
		 * loop
		 */
		bChanges = FALSE;

		/* make a section covering the whole file */
		whole_left = section_new(line_get_first(lines_left),
			line_get_last(lines_left), NULL);

		whole_right = section_new(line_get_first(lines_right),
			line_get_last(lines_right), NULL);

		/* link up matching unique lines between these sections */
		if (::section_match(whole_left, whole_right))
		{
			bChanges = TRUE;
		}

		/* delete the two temp sections */
		section_delete(whole_left);
		section_delete(whole_right);

		/* discard previous section lists if made */
		if (m_secsLeft)
		{
			section_deletelist(m_secsLeft);
			m_secsLeft = NULL;
		}
		if (m_secsRight)
		{
			section_deletelist(m_secsRight);
			m_secsRight = NULL;
		}
		/* build new section lists for both files */
		m_secsLeft = section_makelist(lines_left, TRUE);
		m_secsRight = section_makelist(lines_right, FALSE);

		/* match up sections - make links and corresponds between
		 * sections. Attempts to section_match corresponding
		 * sections that are not matched. returns true if any
		 * further links were made
		 */
		if (section_matchlists(m_secsLeft, m_secsRight))
		{
			bChanges = TRUE;
		}

		/* repeat as long as we keep adding new links */

	} while (bChanges);

	/* all possible lines linked, and section lists made .
	 * combine the two section lists to get a view of the
	 * whole comparison - the composite section list. This also
	 * sets the state of each section in the composite list.
	 */

	 //GeH: we don't handle moved sections in this version
	m_secsComposite = section_makecomposite(m_secsLeft, m_secsRight);
	SetLineState(m_secsLeft, FALSE);
	SetLineState(m_secsRight, TRUE);

	return TRUE;
}

void CFilDiffDoc::SetLineState(LIST secLst, bool bRight)
{
	SECTION sect;

	Sect_TRAVERSE(secLst, sect)
	{
		if (section_getlink(sect) == NULL && section_getcorrespond(sect) == NULL)
		{
			//we have to mark next or previous line to show the difference
			if (sect == (SECTION)List_Last(secLst) && List_Prev(sect))
			{
				SECTION sec1 = section_getlink((SECTION)List_Prev(sect));
				if (sec1)
				{
					section_getlastline(sec1)->state =
						bRight ? STATE_DELLEFTAFTER : STATE_DELRIGHTAFTER;
				}
			}
			else if (List_Next(sect))
			{
				SECTION sec1 = section_getlink((SECTION)List_Next(sect));
				if (sec1)
				{
					section_getfirstline(sec1)->state =
						bRight ? STATE_DELLEFTBEFORE : STATE_DELRIGHTBEFORE;
				}
			}
		}
	}
}

void CFilDiffDoc::MakeComposedEdit()
{
	MakeComposedEdit(m_mergeEdit, true);
}

void CFilDiffDoc::MakeComposedEdit(SECEdit& MergeEdit, bool bLink)
{
	MergeEdit.ClearEdit();
	MergeEdit.InitEdit(GetOptions().sm_szEditSection);
	PLINEDESC line;
	PLINEDESC lpNewLine = NULL;
	PLINEDESC lpFirstLine = NULL;
	PLINEDESC lpPrevLine = NULL;
	SECTION sec;

	int lineNo = 0;
	Sect_TRAVERSE(m_secsComposite, sec) {

		if (sec == NULL)
			return;

		for (line = section_getfirstline(sec)
			; line != NULL;
			line = line_get_next(line))
		{

			lpNewLine = MergeEdit.AllocCopyLine(line);
			ASSERT(lpNewLine);

			if (bLink)
			{
				if (line->state != STATE_LEFTONLY && line->state != STATE_MOVEDLEFT &&
					line->state != STATE_RIGHTONLY && line->state != STATE_MOVEDRIGHT)
				{
					//We link left -> right -> merge ->left
					if (GetOptions().GetOption(OCF_InsLeftIdentSect))
					{
						lpNewLine->link = line;
						if (line->link)
							line->link->link = lpNewLine;
						else
							line->link = lpNewLine;
					}
					else
					{
						if (line->link)
							lpNewLine->link = line->link;
						else
							lpNewLine->link = line;
						line->link = lpNewLine;
					}
				}
				else
				{
					//ASSERT(line->link == NULL);
					line->link = lpNewLine;
					lpNewLine->link = line;
				}
			}

			if (line == section_getfirstline(sec))
				section_setfirstline(sec, lpNewLine);

			lpNewLine->iLineNo = ++lineNo;

			if (!lpFirstLine)
				lpFirstLine = lpPrevLine = lpNewLine;
			else
			{
				MergeEdit.ListInsertAfter(lpNewLine, lpPrevLine);
				lpPrevLine = lpNewLine;
			}

			if (line == section_getlastline(sec)) {
				section_setlastline(sec, lpNewLine);
				break;
			}
		}
	}

	MergeEdit.AttachLineList(lpFirstLine);
	MergeEdit.m_iLinesInFile = MergeEdit.CountLines(MergeEdit.m_pLine);
}


int CFilDiffDoc::SaveComposedFile(CString fileName)
{
	COXPathSpec PathSpec(fileName);
	if (!((COXDirSpec)PathSpec).Exists())
	{
		CString str;
		AfxFormatString1(str, IDS_YouWantToCreateDir, PathSpec.GetDirectory());

		if (AfxMessageBox(str, MB_YESNO) == IDNO)
			return IDCANCEL;

		((COXDirSpec)PathSpec).DoMakeNew();
	}
	m_strMergeFileName = fileName;
	return m_mergeEdit.WriteFile(fileName);

}

LPCTSTR CFilDiffDoc::GetFileName()
{
	return m_strFileName;
}

void CFilDiffDoc::InitDiffList()
{
	InitDiffList(m_secDiffList);
}

void CFilDiffDoc::InitDiffList(CObArray& SecDiffList)
{
	SecDiffList.SetSize(GetDiffSectSize());

	CDiffSectElt* pElt;
	PLINEDESC pStart, pEnd;
	SECTION sec, sec1;
	SECTION secPrev;
	int idx = -1;
	for (sec = (SECTION)List_First(m_secsComposite);  sec != NULL                      \
		;  sec = (SECTION)List_Next((LPVOID)sec))
	{

		if (section_getstate(sec) == STATE_SAME)
			continue;

		idx++;
		if (section_getstate(sec) == STATE_MOVEDLEFT
			|| section_getstate(sec) == STATE_LEFTONLY)
		{
			sec1 = (SECTION)List_Next(sec);
			if (sec1 && section_getstate(sec1) == STATE_MOVEDLEFT
				|| sec1 && section_getstate(sec1) == STATE_RIGHTONLY)
			{
				//This means that we have two consecutive 
						//Different sections, which correspond.
				pElt = new CDiffSectElt(
					section_getlink(sec),		//left
					section_getlink(sec1),		//right
					sec, sec1);					//composed

				//Now skip one section
				secPrev = (SECTION)List_Prev(sec);
				sec = (SECTION)List_Next(sec);

				if (secPrev && section_getstate(secPrev) == STATE_SAME)
				{
					pElt->m_pAfter = pElt->GetCLStart()->pPrev;
					ASSERT(pElt->m_pAfter);
				}

			}
			else
			{
				//We have only one left section, right is empty
				pElt = new CDiffSectElt(
					section_getlink(sec),		//left
					NULL,						//right
					sec, NULL);					//composed
				GetDiffPos(sec, pStart, pEnd);
				pElt->SetRLines(pStart, pEnd);

				secPrev = (SECTION)List_Prev(sec);
				if (secPrev && section_getstate(secPrev) == STATE_SAME)
				{
					pElt->m_pAfter = pElt->GetCLStart()->pPrev;
					ASSERT(pElt->m_pAfter);
				}
			}
		}
		else
		{
			//We have only one right section
			ASSERT(section_getstate(sec) == STATE_MOVEDRIGHT
				|| section_getstate(sec) == STATE_RIGHTONLY);

			pElt = new CDiffSectElt(
				NULL,						//left
				section_getlink(sec),		//right
				NULL, sec);					//composed
			GetDiffPos(sec, pStart, pEnd);
			pElt->SetLLines(pStart, pEnd);

			secPrev = (SECTION)List_Prev(sec);
			if (secPrev && section_getstate(secPrev) == STATE_SAME)
			{
				pElt->m_pAfter = pElt->GetCRStart()->pPrev;
				ASSERT(pElt->m_pAfter);
			}
		}
		ASSERT(idx >= 0 && idx < SecDiffList.GetSize());

#ifdef _DEBUG
		ASSERT(pElt->GetLStart() || pElt->GetLEnd() || SecDiffList.GetSize() == 1);
		ASSERT(pElt->GetRStart() || pElt->GetREnd() || SecDiffList.GetSize() == 1);
		ASSERT(pElt->GetCStart() || pElt->GetCEnd());
#endif

		SecDiffList[idx] = pElt;
		pElt->m_nIdx = idx;
	}
}

int GetState(SECTION sec1)
{
	int state;

	state = section_getfirstline(sec1)->state;
	return(state);
}

/* Search for the position of the empty section
	pStart & pEnd are Output
  */
void CFilDiffDoc::GetDiffPos(SECTION sec,
	PLINEDESC& pStart,
	PLINEDESC& pEnd
)
{
	ASSERT(sec);
	SECTION sec1 = section_getlink(sec);
	SECTION secOther = NULL;
	SECTION secIdx = sec1;
	pStart = NULL;
	pEnd = NULL;

	ASSERT(sec1);
	if (!List_Next(sec1) && !List_Prev(sec1)) {
		//seems that other file doesn't exist
		pStart = pEnd = NULL;
		return;
	}

	//Search downwards for matching sections
	while ((secIdx = (SECTION)List_Next(secIdx)) != 0)
	{
		secOther = section_getlink(secIdx);
		//if(!secOther)
			//secOther = section_getcorrespond(secIdx);

		if (secOther)
		{
			//Difference is before this section
			pStart = NULL;
			pEnd = section_getfirstline(secOther);
			ASSERT(pStart || pEnd);
			return;
		}

	}

	//Could not find the position, search upwards
	secIdx = sec1;
	while ((secIdx = (SECTION)List_Prev(secIdx)) != NULL)
	{
		secOther = section_getlink(secIdx);

		//if(!secOther)
			//secOther = section_getcorrespond(secIdx);

		if (secOther)
		{
			//Difference is before this section
			pStart = section_getlastline(secOther);
			pEnd = NULL;
			ASSERT(pStart || pEnd);
			return;
		}
	}

	ASSERT(false);

	return;
}

BOOL CFilDiffDoc::NextDiff()
{
	m_curDiffIdx++;
	if (m_curDiffIdx > m_secDiffList.GetUpperBound())
	{
		m_curDiffIdx = -1;
		return(FALSE);
	}
	return TRUE;
}

BOOL CFilDiffDoc::PrevDiff()
{
	if (m_curDiffIdx == -1)
	{
		m_curDiffIdx = m_secDiffList.GetUpperBound();
		return m_curDiffIdx != -1;
	}

	m_curDiffIdx--;
	if (m_curDiffIdx == -1)
	{
		return(FALSE);
	}
	return TRUE;
}

CDiffSectElt* CFilDiffDoc::GetActualSect()
{
	if (m_curDiffIdx == -1)
		return NULL;

	ASSERT(m_curDiffIdx < m_secDiffList.GetSize());
	return (CDiffSectElt*)m_secDiffList[m_curDiffIdx];
}

int CFilDiffDoc::GetDiffSectSize()
{
	int size = 0;
	SECTION sec, sec1;
	for (sec = (SECTION)List_First(m_secsComposite);  sec != NULL                      \
		;  sec = (SECTION)List_Next((LPVOID)sec))
	{

		if (section_getstate(sec) == STATE_SAME)
			continue;

		size++;

		if (section_getstate(sec) == STATE_MOVEDLEFT
			|| section_getstate(sec) == STATE_LEFTONLY)
		{
			sec1 = (SECTION)List_Next(sec);
			if (sec1 && section_getstate(sec1) == STATE_MOVEDLEFT
				|| sec1 && section_getstate(sec1) == STATE_RIGHTONLY)
			{
				//This means that we have to consecutive 
				//Skip one section
				sec = (SECTION)List_Next(sec);
			}
		}
	}
	return size;
}

void CFilDiffDoc::OnEditCmdmode()
{
	ToggleOption(OCF_CmdMode);
}

void CFilDiffDoc::OnUpdateEditCmdmode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetOptions().GetOption(OCF_CmdMode));

}

PLINEDESC CFilDiffDoc::GetSectAfter(CDiffSectElt* pSect)
{
	return GetSectAfter(pSect->m_nIdx);
}

PLINEDESC CFilDiffDoc::GetSectAfter(int nDiffIdx)
{
	if (nDiffIdx == -1)
		return NULL;

	ASSERT(nDiffIdx < m_secDiffList.GetSize());
	CDiffSectElt* pDiffElt = (CDiffSectElt*)m_secDiffList[nDiffIdx];

	if (pDiffElt->m_pAfter)
		return pDiffElt->m_pAfter;
	else if (nDiffIdx > 0)
	{
		CDiffSectElt* pPrevSect = (CDiffSectElt*)m_secDiffList[nDiffIdx - 1];

		if (pPrevSect->GetCEnd())
			return pPrevSect->GetCEnd();
		else if (pPrevSect->GetCStart())
		{
			ASSERT(FALSE);
			return pPrevSect->GetCStart();
		}
	}

	//default 
	return GetSectAfter(--nDiffIdx);
}
void CFilDiffDoc::OnDeleteLine(PLINEDESC pLine)
{
	int nIndex = 0;

	while (GetRefSect(nIndex, pLine))
	{
		SetRefSect(nIndex, pLine);
		nIndex++;
	}
}

void CFilDiffDoc::OnDeleteLine(PLINEDESC pStart, PLINEDESC pEnd)
{
	if (pStart == pEnd)
	{
		OnDeleteLine(pStart);
		return;
	}

	PLINEDESC pLine = pStart;

	int nIndex = 0;
	while (pLine)
	{
		while (GetRefSect(nIndex, pLine))
		{
			SetRefSect(nIndex, pLine, false, pStart, pEnd);
			nIndex++;
		}

		if (pLine == pEnd)
			break;

		nIndex = 0;
		pLine = pLine->pNext;
	}
	return;
}

bool CFilDiffDoc::GetRefSect(int& nIdxStart, PLINEDESC pLine)
{
	CDiffSectElt* pElt;
	bool          bFound = false;

	int i{};
	for (i = nIdxStart; i <= m_secDiffList.GetUpperBound(); i++)
	{
		pElt = (CDiffSectElt*)m_secDiffList[i];

		if (pElt->m_pAfter == pLine ||
			pElt->GetCLStart() == pLine ||
			pElt->GetCLEnd() == pLine ||
			pElt->GetCRStart() == pLine ||
			pElt->GetCREnd() == pLine)
		{
			bFound = true;
		}


		if (bFound)
			break;
	}

	nIdxStart = i;
	return bFound;
}

void CFilDiffDoc::ReplaceRef(int nIndex, PLINEDESC pLine, PLINEDESC pSetLine)
{
	CDiffSectElt* pElt;

	pElt = (CDiffSectElt*)m_secDiffList[nIndex];

	if (pElt->m_pAfter == pLine)
	{
		pElt->m_pAfter = pSetLine;
	}

	if (pElt->GetCLStart() == pLine)
	{
		pElt->m_compLeft.start = pSetLine;
	}

	if (pElt->GetCRStart() == pLine)
	{
		pElt->m_compRight.start = pSetLine;
	}

	if (pElt->GetCLEnd() == pLine)
	{
		pElt->m_compLeft.end = pSetLine;
	}

	if (pElt->GetCREnd() == pLine)
	{
		pElt->m_compRight.end = pSetLine;
	}
}

void CFilDiffDoc::SetRefSect(int nIndex, PLINEDESC pFind, bool bNextMovedUp, PLINEDESC pStart, PLINEDESC pEnd)
{
	CDiffSectElt* pElt = NULL;
	PLINEDESC pNext = NULL;


	if (pStart == NULL)
		pStart = pFind;

	if (pEnd == NULL)
		pEnd = pFind;

	pNext = pEnd->pNext;
	if (pEnd == pFind && bNextMovedUp)
	{
		pNext = pFind;
	}

	pElt = (CDiffSectElt*)m_secDiffList[nIndex];

	if (pElt->m_pAfter == pFind)
	{
		pStart = pStart->pPrev;
		while (pStart && pStart->state != STATE_SAME)
			pStart = pStart->pPrev;

		pElt->m_pAfter = pStart;

		if (pElt->m_nIdx > 0 && pElt->m_pAfter)
		{
			CDiffSectElt* pPrevSect = (CDiffSectElt*)m_secDiffList[pElt->m_nIdx - 1];
			if ((pPrevSect->GetCStart() && pPrevSect->GetCStart()->iLineNo >= pStart->iLineNo) ||
				(pPrevSect->m_pAfter && pPrevSect->m_pAfter->iLineNo >= pStart->iLineNo))
			{
				// The previous section is also deleted and has M-pAfter set.
				pElt->m_pAfter = pStart = NULL; //Will be calculated from pPrevSect!!
			}
		}

	}

	if (pElt->GetCLStart() == pFind)
	{
		if (!bNextMovedUp && pStart && pStart->iLineNo <= pElt->GetCLStart()->iLineNo && pEnd &&
			pEnd->iLineNo >= pElt->GetCLEnd()->iLineNo)
		{
			//Whole section will be deleted
			pElt->m_compLeft.start = NULL;
			pElt->m_compLeft.end = NULL;
		}

		if (pElt->GetCLEnd() && pNext && pNext->iLineNo <= pElt->GetCLEnd()->iLineNo)
		{
			pElt->m_compLeft.start = pNext;
		}
		else
		{
			pElt->m_compLeft.start = NULL;
			pElt->m_compLeft.end = NULL;
		}
	}

	if (pElt->GetCRStart() == pFind)
	{
		if ((!bNextMovedUp && pStart->iLineNo <= pElt->GetCRStart()->iLineNo &&
			pEnd->iLineNo >= pElt->GetCREnd()->iLineNo))
		{
			//Whole section will be deleted
			pElt->m_compRight.start = NULL;
			pElt->m_compRight.end = NULL;
		}

		if (pElt->GetCREnd() && pNext && pNext->iLineNo <= pElt->GetCREnd()->iLineNo)
		{
			pElt->m_compRight.start = pNext;
		}
		else
		{
			pElt->m_compRight.start = NULL;
			pElt->m_compRight.end = NULL;
		}
	}

	if (pElt->GetCLEnd() == pFind)
	{
		if (pElt->GetCLStart() && pStart && pStart->pPrev && pStart->pPrev->iLineNo >= pElt->GetCLStart()->iLineNo)
			pElt->m_compLeft.end = pStart->pPrev;
		else
		{
			pElt->m_compLeft.end = NULL;
			pElt->m_compLeft.start = NULL;
		}
	}

	if (pElt->GetCREnd() == pFind)
	{
		if (pElt->GetCRStart() && pStart && pStart->pPrev && pStart->pPrev->iLineNo >= pElt->GetCRStart()->iLineNo)
			pElt->m_compRight.end = pStart->pPrev;
		else
		{
			pElt->m_compRight.end = NULL;
			pElt->m_compRight.start = NULL;
		}
	}
}

void CFilDiffDoc::OnJoinLines(PTEXTPOS pTextPos, PLINEDESC pOther)
{
	int nIndex = 0;

	if (!pOther)
	{
		ASSERT(false);
		return;
	}

	while (GetRefSect(nIndex, pTextPos->pLine))
	{
		SetRefSect(nIndex, pTextPos->pLine, true);
		nIndex++;
	}

	//pOther will be deleted, make sure it does not exist as e reference.
	nIndex = 0;
	while (GetRefSect(nIndex, pOther))
	{
		ReplaceRef(nIndex, pOther, pTextPos->pLine);
		nIndex++;
	}

	//Replace the state for this line
	pTextPos->pLine->state = pOther->state;

}

void CFilDiffDoc::OnSplitLines(PLINEDESC pLine)
{
	int nIndex = 0;

	while (GetRefSect(nIndex, pLine))
	{
		VERIFY(MoveRefDown(nIndex, pLine));
		nIndex++;
	}
}

bool CFilDiffDoc::MoveRefDown(int nIndex, PLINEDESC pLine)
{
	CDiffSectElt* pElt = NULL;

	pElt = (CDiffSectElt*)m_secDiffList[nIndex];

	ASSERT(pLine->pNext);

	if (pElt->m_pAfter == pLine)
	{
		pElt->m_pAfter = pLine->pNext;
	}

	if (pElt->GetCLEnd() == pLine)
	{
		pElt->m_compLeft.end = pLine->pNext;
	}

	if (pElt->GetCREnd() == pLine)
	{
		pElt->m_compRight.end = pLine->pNext;
	}

	return true;
}

//Just for debugging
bool CFilDiffDoc::VerifyDiffArray()
{
	for (int i = 0; i <= m_secDiffList.GetUpperBound() - 1; i++)
	{
		CDiffSectElt* pElt = (CDiffSectElt*)m_secDiffList[i];
		ASSERT(pElt->GetCLStart() == NULL || AfxIsValidAddress(pElt->GetCLStart(), sizeof(LINEDESC)) && pElt->GetCLStart()->iLineNo >= 0);
		ASSERT(pElt->GetCLEnd() == NULL || AfxIsValidAddress(pElt->GetCLEnd(), sizeof(LINEDESC)) && pElt->GetCLEnd()->iLineNo >= 0);
		ASSERT(pElt->GetCRStart() == NULL || AfxIsValidAddress(pElt->GetCRStart(), sizeof(LINEDESC)) && pElt->GetCRStart()->iLineNo >= 0);
		ASSERT(pElt->GetCREnd() == NULL || AfxIsValidAddress(pElt->GetCREnd(), sizeof(LINEDESC)) && pElt->GetCREnd()->iLineNo >= 0);
		ASSERT(pElt->m_pAfter == NULL || AfxIsValidAddress(pElt->m_pAfter, sizeof(LINEDESC)));
		ASSERT(pElt->m_pAfter == NULL || pElt->m_pAfter->pPrev == NULL || AfxIsValidAddress(pElt->m_pAfter->pPrev, sizeof(LINEDESC)));
	}
	return true;
}

BOOL CFilDiffDoc::SaveModified()
{
	UINT nResult = IDOK;

	if (m_mergeEdit.GetModified())
	{
		nResult = AfxMessageBox(IDS_ComposedFileWasChanged, MB_YESNOCANCEL | MB_ICONQUESTION);
		switch (nResult)
		{
		case IDYES:
			if (SaveComposed() == IDCANCEL)
				return FALSE;;
		case IDNO:
			break;
		default:
			return FALSE;
		}
	}

	return TRUE;
}

UINT CFilDiffDoc::SaveComposed()
{
	UINT nID = 0;
	int nResult = 0;
	CString fileName;

	//INitialize values from private profile
	CSaveCompDlg dlgSave;

	dlgSave.SetFileNames(m_strRelDir, m_strLeftFileName, m_strRightFileName);
	if (!m_strMergeFileName.IsEmpty())
	{
		dlgSave.m_strFileName = m_strMergeFileName;
	}

	nID = dlgSave.DoModal();
	if (nID == IDOK)
	{
		fileName = dlgSave.GetFileName();
		if (fileName.IsEmpty())
		{
			AfxMessageBox(IDS_NoFilenameSpecified);
			return IDCANCEL;
		}

		nResult = SaveComposedFile(fileName);
		switch (nResult)
		{
		case IDS_OE_READONLY:
			AfxMessageBox(IDS_FileReadonly);
			return IDCANCEL;
		case 0:
			break;
		default:
		{
			LPVOID lpMsgBuf;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				nResult,
				MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				(LPTSTR)&lpMsgBuf,
				0,
				NULL);
			AfxMessageBox((LPCTSTR)lpMsgBuf, MB_OK | MB_ICONINFORMATION);
			LocalFree(lpMsgBuf);

		};
		return IDCANCEL;
		}
	}
	return nID;
}

void CFilDiffDoc::SetTitle(LPCTSTR lpszTitle)
{
	UNREFERENCED_PARAMETER(lpszTitle);

	if (GetOptions().GetOption(OCF_ShowFileName))
	{
		CDocument::SetTitle("File compare");
	}
	else
	{
		CDocument::SetTitle(m_strLeftFileName + " : " + m_strRightFileName);
	}
}

// MCN
PLINEDESC CFilDiffDoc::SyncLeftWithMerge(PLINEDESC pLine)
{
	SECTION secComp = section_search_in_list(m_secsComposite, pLine);
	ASSERT(secComp != NULL);

	int posLine = section_exists_line(secComp, pLine);
	ASSERT(posLine > 0);

	SECTION secLeft = section_get_left_from_comp(secComp);
	if (secLeft == NULL)
		return NULL;

	return line_iterate(section_getfirstline(secLeft), posLine);
}

// MCN
PLINEDESC CFilDiffDoc::SyncRightWithMerge(PLINEDESC pLine)
{
	SECTION secComp = section_search_in_list(m_secsComposite, pLine);
	ASSERT(secComp != NULL);

	int posLine = section_exists_line(secComp, pLine);
	ASSERT(posLine > 0);

	SECTION secRight = section_get_right_from_comp(secComp);
	if (secRight == NULL)
		return NULL;

	return line_iterate(section_getfirstline(secRight), posLine);
}

void CFilDiffDoc::GetLinkedLines(PLINEDESC& pLeft, PLINEDESC& pRight, PLINEDESC& pMerge, SECEdit* pEdit, PLINEDESC pLine)
{
	if (!pLine)
		return;

	pLeft = pRight = pMerge = NULL;

	pLine = CheckIfFolded(pLine, pEdit);

	if (!pLine)
		return;

	if (pLine->state != STATE_LEFTONLY || pLine->state != STATE_MOVEDLEFT ||
		pLine->state != STATE_RIGHTONLY || pLine->state != STATE_MOVEDRIGHT)
	{
		if (pLine->link == NULL)
		{
			UpdateLineParam(pLeft, pRight, pMerge, pLine, pEdit);
		}
		else if (pLine->link && pLine->link->link == pLine)
		{
			UpdateLineParam(pLeft, pRight, pMerge, pLine, pEdit);
			UpdateLineParam(pLeft, pRight, pMerge, pLine->link);
		}
		else
		{
			pLeft = pLine;
			if (pEdit == &m_rightEdit)
				pLeft = pLine->link->link;
			else if (pEdit == &m_mergeEdit)
				pLeft = pLine->link;

			pRight = pLeft->link;
			pMerge = pLeft->link->link;
		}
	}
	else
	{
		CDiffSectElt* pSect = GetNextDiffFromPos(pEdit, pLine, false);
		if (pLine->state == STATE_LEFTONLY || pLine->state == STATE_MOVEDLEFT)
		{
			if (pEdit == &m_mergeEdit)
			{
				pLeft = pLine->link;
				pMerge = pLine;
			}
			else if (pEdit == &m_leftEdit)
			{
				pLeft = pLine;
				pMerge = pLine->link;
			}
			else
				ASSERT(false);
		}
		else
		{
			ASSERT(pLine->state == STATE_RIGHTONLY || pLine->state == STATE_MOVEDRIGHT);
			if (pEdit == &m_mergeEdit)
			{
				pRight = pLine->link;
				pMerge = pLine;
			}
			else if (pEdit == &m_rightEdit)
			{
				pRight = pLine;
				pMerge = pLine->link;
			}
			else
				ASSERT(false);
		}
	}

	pLeft = CheckIfFolded(pLeft, &m_leftEdit);
	pRight = CheckIfFolded(pRight, &m_rightEdit);
	pMerge = CheckIfFolded(pMerge, &m_mergeEdit);
}

// MCN
PLINEDESC CFilDiffDoc::SyncMergeWithLeft(PLINEDESC pLine)
{
	SECTION secLeft = section_search_in_list(m_secsLeft, pLine);
	ASSERT(secLeft != NULL);

	int posLine = section_exists_line(secLeft, pLine);
	ASSERT(posLine > 0);

	SECTION secMerge = section_get_comp_from_left(m_secsComposite, secLeft);
	ASSERT(secMerge != NULL);

	return line_iterate(section_getfirstline(secMerge), posLine);
}

// MCN
PLINEDESC CFilDiffDoc::SyncMergeWithRight(PLINEDESC pLine)
{
	SECTION secRight = section_search_in_list(m_secsRight, pLine);
	ASSERT(secRight != NULL);

	int posLine = section_exists_line(secRight, pLine);
	ASSERT(posLine > 0);

	SECTION secMerge = section_get_comp_from_right(m_secsComposite, secRight);
	ASSERT(secMerge != NULL);

	return line_iterate(section_getfirstline(secMerge), posLine);
}

void CFilDiffDoc::FoldEqualSections(SECEdit* pEdit)
{
	LPCTSTR pszFoldedText = NULL;
	CString strFoldedText;

	if (!GetOptions().GetOption(OCF_FoldLines))
		return;

	int nOffset1 = GetOffset(pEdit, true);
	int nOffset2 = GetOffset(pEdit, false);
	CDiffSectElt* pDiffSectElt = NULL;
	DIFF_SECT* pDiffSect1;
	DIFF_SECT* pDiffSect2;

	int nUnfold = GetOptions().GetNofUnfoldedLines();

	PLINEDESC pStart = NULL;
	PLINEDESC pEnd = NULL;
	PLINEDESC pNextStart = NULL;

	pNextStart = pEdit->m_pLine;

	for (int i = 0; i < m_secDiffList.GetSize(); i++)
	{
		pStart = pEnd = NULL;

		pDiffSectElt = (CDiffSectElt*)m_secDiffList[i];
		pDiffSect1 = (DIFF_SECT*)(((BYTE*)pDiffSectElt) + nOffset1);
		pDiffSect2 = (DIFF_SECT*)(((BYTE*)pDiffSectElt) + nOffset2);

		if (pDiffSect1->start == NULL && pDiffSect1->end == NULL)
			pDiffSect1 = pDiffSect2;

		if (pDiffSect2->start == NULL && pDiffSect2->end == NULL)
			pDiffSect2 = pDiffSect1;

		pStart = pNextStart;

		if (pDiffSect1->start)
			pEnd = pDiffSect1->start->pPrev;
		else if (pDiffSect1->end)
			pEnd = pDiffSect1->end->pPrev;
		else
			pEnd = NULL;

		if (pDiffSect2->end)
			pNextStart = pDiffSect2->end->pNext;
		else if (pDiffSect2->start)
			pNextStart = pDiffSect2->start->pNext;

		pStart = MoveForward(pStart, nUnfold);
		pEnd = MoveBackward(pEnd, nUnfold);

		pszFoldedText = NULL;

		if (pStart && pEnd && pStart->iLineNo < pEnd->iLineNo)
			pEdit->FoldLines(pStart, pEnd, pszFoldedText);
	}
	pStart = pNextStart;

	if (pNextStart)
		pEnd = pEdit->ListGetLast(pNextStart);

	pStart = MoveForward(pStart, nUnfold);
	pEnd = MoveBackward(pEnd, nUnfold);

	pszFoldedText = NULL;

	if (pStart && pEnd && pStart->iLineNo < pEnd->iLineNo)
		pEdit->FoldLines(pStart, pEnd, pszFoldedText);
}

PLINEDESC CFilDiffDoc::MoveForward(PLINEDESC pStart, int nCount)
{
	while (nCount && pStart)
	{
		pStart = pStart->pNext;
		nCount--;
	}
	return pStart;
}

PLINEDESC CFilDiffDoc::MoveBackward(PLINEDESC pStart, int nCount)
{
	while (nCount && pStart)
	{
		pStart = pStart->pPrev;
		nCount--;
	}
	return pStart;
}

int CFilDiffDoc::GetOffset(SECEdit* pEdit, bool bSecond)
{
	CDiffSectElt Diff;

	if (pEdit == &m_leftEdit)
		return (DWORD)&Diff.m_left - (DWORD)&Diff;

	if (pEdit == &m_rightEdit)
		return (DWORD)&Diff.m_right - (DWORD)&Diff;

	if (pEdit == &m_mergeEdit)
	{
		if (bSecond)
			return (DWORD)&Diff.m_compLeft - (DWORD)&Diff;
		else
			return (DWORD)&Diff.m_compRight - (DWORD)&Diff;
	}

	ASSERT(false);
	return 0;
}

void CFilDiffDoc::UpdateFolding()
{
	m_leftEdit.UnFoldAll();
	m_rightEdit.UnFoldAll();
	m_mergeEdit.UnFoldAll();

	FoldEqualSections(&m_leftEdit);
	FoldEqualSections(&m_rightEdit);
	FoldEqualSections(&m_mergeEdit);

}

void CFilDiffDoc::Refresh()
{
	int nCurrDiffIdx = m_curDiffIdx;
	Reset();

	CompareFiles();

	if (nCurrDiffIdx < m_secDiffList.GetSize())
		m_curDiffIdx = nCurrDiffIdx;
	else
		m_curDiffIdx = m_secDiffList.GetUpperBound();

}

CDiffSectElt* CFilDiffDoc::GetNextDiffFromPos(SECEdit* pEdit, PLINEDESC pLine, bool bSetCurrent)
{
	int nIdx = GetNextDiffIdxFromPos(pEdit, pLine);

	if (nIdx != -1)
	{
		if (bSetCurrent)
			m_curDiffIdx = nIdx;

		return (CDiffSectElt*)m_secDiffList[nIdx];
	}

	return NULL;
}

int CFilDiffDoc::GetNextDiffIdxFromPos(SECEdit* pEdit, PLINEDESC pLine)
{
	int nOffset1 = GetOffset(pEdit, true);
	int nOffset2 = GetOffset(pEdit, false);
	CDiffSectElt* pDiffSectElt = NULL;
	DIFF_SECT* pDiffSect1;
	DIFF_SECT* pDiffSect2;

	for (int i = 0; i < m_secDiffList.GetSize(); i++)
	{

		pDiffSectElt = (CDiffSectElt*)m_secDiffList[i];
		pDiffSect1 = (DIFF_SECT*)(((BYTE*)pDiffSectElt) + nOffset1);
		pDiffSect2 = (DIFF_SECT*)(((BYTE*)pDiffSectElt) + nOffset2);

		if (pDiffSect1->end && pDiffSect1->end->iLineNo >= pLine->iLineNo)
			return i;
		if (pDiffSect1->start && pDiffSect1->start->iLineNo >= pLine->iLineNo)
			return i;

		if (pDiffSect1 != pDiffSect2)
		{
			if (pDiffSect2->end && pDiffSect2->end->iLineNo >= pLine->iLineNo)
				return i;
			if (pDiffSect2->start && pDiffSect2->start->iLineNo >= pLine->iLineNo)
				return i;
		}
	}
	return -1;
}


/*
  PLINEDESC pStart = NULL;
  PLINEDESC pEnd   = NULL;
  PLINEDESC pNext  = NULL;
  PLINEDESC pLine = pEdit->m_pLine;

  //int nUnfolded = GetOptions().GetNofUnfoldedLines();
  int nUnfolded = 0;

  if (nUnfolded <= 0)
	return;

  int nCount = 0;
  int nForward = 0;
  while (pLine)
  {
	pNext = pLine->pNext;
	if (pStart && pLine->state != STATE_SAME)
	{
	  if (pStart && pEnd && pStart->iLineNo < pEnd->iLineNo)
		pEdit->FoldLines(pStart, pEnd);
	  nCount = 0;
	  pStart = NULL;
	  pEnd   = NULL;
	}
	else
	{
	  if (nCount == 0)
		pEnd = pLine;

	  if (nCount == nUnfolded)
	  {
		pStart = pLine;
	  }

	  nCount++;
	}

	pLine = pNext;

	if (pEnd && nCount > nUnfolded)
	  pEnd = pEnd->pNext;
  }

  if (pStart && pEnd && pStart->iLineNo < pEnd->iLineNo)
	pEdit->FoldLines(pStart, pEnd);
*/

void CFilDiffDoc::UpdateLineParam(PLINEDESC& pLeft, PLINEDESC& pRight, PLINEDESC&
	pMerge, PLINEDESC& pLine, SECEdit* pEdit)
{
	PLINEDESC pFirstLine = NULL;
	if (pEdit == NULL)
	{
		pFirstLine = m_leftEdit.ListGetFirst(pLine, NULL);
	}
	else
	{
		pFirstLine = pEdit->m_pLine;
	}

	if (pFirstLine == m_leftEdit.m_pLine)
		pLeft = pLine;
	else if (pFirstLine == m_rightEdit.m_pLine)
		pRight = pLine;
	else if (pFirstLine == m_mergeEdit.m_pLine)
		pMerge = pLine;
}

PLINEDESC CFilDiffDoc::CheckIfFolded(PLINEDESC pLine, SECEdit* pEdit)
{
	if (pLine == NULL)
		return NULL;

	if (pLine->state == STATE_FOLDED && pLine->pPrev && pLine->pPrev->state == STATE_FOLDED)
	{
		// This is one of the lines in the folded list, should be linked to 
		// the folded line.
		while (pLine && pLine->pPrev && pLine->pPrev->pNext == pLine)
			pLine = pLine->pPrev;

		if (!pLine)
		{
			ASSERT(false);
			return NULL;
		}

		return pLine->pPrev;
	}
	return pLine;
}

void CFilDiffDoc::OnFoldedLines(SECEdit* pEdit, PLINEDESC pLine)
{
	PLINEDESC pList = pEdit->GetFoldedLineList(pLine);

	if (pList)
		pList = pEdit->ListGetLast(pList);

	CDiffSectElt* pSect = GetNextDiffFromPos(pEdit, pLine, false);
	if (pSect && pSect->m_pAfter == pList)
	{
		pSect->m_pAfter = pLine;
	}
}

void CFilDiffDoc::OnUnFoldLines(SECEdit* pEdit, PLINEDESC pLine)
{
	PLINEDESC pList = pEdit->GetFoldedLineList(pLine);

	if (pList)
		pList = pEdit->ListGetLast(pList);

	CDiffSectElt* pSect = GetNextDiffFromPos(pEdit, pLine, false);
	if (pSect && pSect->m_pAfter == pLine)
	{
		pSect->m_pAfter = pList;
	}
}

void CFilDiffDoc::XmlExport()
{
	CFileDialog dlg(FALSE, NULL, _T("FileCompare"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, GetResourceString(IDS_XmlFiles));

	if (dlg.DoModal() != IDOK)
		return;

	CXmlExport Exp(dlg.GetPathName());
	VERIFY(Exp.Open(szFileCompare));
	{
		CXmlEntity FileCompare(&Exp, szFileCompare);
		Exp.Write(&FileCompare);

		{
			CXmlEntity LeftFile(&Exp, szLeftFile);
			Exp.Write(&LeftFile);

			COXPathSpec Left(m_strLeftFileName);
			Left.DoGetInfo();

			Exp.WriteDate(Left.GetTime());
			Exp.Write(szName, m_strLeftFileName);
		}

		{
			CXmlEntity RightFile(&Exp, szRightFile);
			Exp.Write(&RightFile);

			COXPathSpec Right(m_strRightFileName);
			Right.DoGetInfo();
			Exp.WriteDate(Right.GetTime());
			Exp.Write(szName, m_strRightFileName);
		}

		{
			CXmlEntity Sections(&Exp, szSections);
			Exp.Write(Sections);


			CDiffSectElt* pElt;
			DIFF_SECT Sec;
			Sec.state = STATE_SAME;
			Sec.start = m_mergeEdit.m_pLine;
			Sec.end = m_mergeEdit.m_pLine;
			for (int i = 0; i < m_secDiffList.GetSize(); i++)
			{
				pElt = (CDiffSectElt*)m_secDiffList[i];
				if (Sec.start && Sec.start->state == STATE_SAME)
				{
					Sec.end = Sec.start;
					while (Sec.end->pNext && Sec.end->pNext->state == STATE_SAME)
						Sec.end = Sec.end->pNext;
				}
				else
				{
					Sec.start = NULL;
					Sec.end = NULL;
				}
				if (Sec.end && Sec.end->pNext == pElt->GetCStart())
				{
					XmlSection(Exp, &Sec);
				}
				if (!pElt->IsCREmpty())
				{
					XmlSection(Exp, &pElt->m_compRight);
				}
				if (!pElt->IsCLEmpty())
				{
					XmlSection(Exp, &pElt->m_compLeft);
				}
				Sec.start = pElt->GetCEnd()->pNext;
			}

			if (Sec.start)
			{
				XmlSection(Exp, &Sec);
			}
		}
	}
	Exp.Close();
}

void CFilDiffDoc::XmlSection(CXmlExport& Exp, DIFF_SECT* pDiffSect)
{
	if (!pDiffSect->start)
		return;

	CXmlEntity Section(&Exp, szSection);
	Exp.Write(Section);
	Exp.Write(szStatus, pDiffSect->GetStatus());
	Exp.Write(szStartLineNo, GetDisplayLineNo(pDiffSect->start));

	if (!pDiffSect->end)
	{
		pDiffSect->end = pDiffSect->start;
		while (pDiffSect->end->pNext)
			pDiffSect->end = pDiffSect->end->pNext;
	}
	Exp.Write(szEndLineNo, GetDisplayLineNo(pDiffSect->end));
	XmlLines(Exp, pDiffSect->start, pDiffSect->end);
}

void CFilDiffDoc::XmlLines(CXmlExport& Exp, PLINEDESC pStart, PLINEDESC pEnd)
{
	CXmlEntity Lines(&Exp, szLines);
	Exp.Write(Lines);
	while (pStart && pStart != pEnd)
	{
		Exp.Write(szLine, pStart->pText);
		pStart = pStart->pNext;
	}
	Exp.Write(szLine, pEnd->pText);
}

//Works only for lines in the composed edit
int CFilDiffDoc::GetDisplayLineNo(PLINEDESC pLine)
{
	if (pLine->state == STATE_LEFTONLY || pLine->state == STATE_MOVEDLEFT)
	{
		if (pLine->link && pLine->link != pLine)
			return pLine->link->iDisplayLineNo;
	}
	else if (pLine->state == STATE_RIGHTONLY || pLine->state == STATE_MOVEDRIGHT)
	{
		if (pLine->link && pLine->link != pLine)
			return pLine->link->iDisplayLineNo;
	}
	else
	{
		if (GetOptions().GetOption(OCF_InsLeftIdentSect))
		{
			if (pLine->link && pLine->link != pLine)
				return pLine->link->iDisplayLineNo;
		}
		else
		{
			if (pLine->link && pLine->link != pLine && pLine->link->link &&
				pLine->link->link != pLine)
			{
				return pLine->link->link->iDisplayLineNo;
			}
		}
	}
	return -1;
}

bool CFilDiffDoc::CompareLines(PLINEDESC pLLine, PLINEDESC pRLine, CUIntArray* pLSect, CUIntArray* pRSect)
{
	pLSect->RemoveAll();
	pRSect->RemoveAll();

	if (pLLine->iTxtSize == 0 || pRLine->iTxtSize == 0)
	{
		return true;
	}

	int nLLength = 0;
	int nRLength = 0;
	LPTSTR pszLeft = m_mergeEdit.ExpandTabs(pLLine->pText, pLLine->iTxtSize, &nLLength);
	LPTSTR pszRight = m_mergeEdit.ExpandTabs(pRLine->pText, pRLine->iTxtSize, &nRLength);

	BYTE* pLMatch = new BYTE[nLLength];
	BYTE* pRMatch = new BYTE[nRLength];

	memset(pLMatch, 0, nLLength);
	memset(pRMatch, 0, nRLength);

	for (int i = 5; i > 0; i--)
	{
		MatchStrings(i, pszLeft, pszRight, pLMatch, pRMatch);
	}

	CreateSect(pLMatch, nLLength, pLSect);
	CreateSect(pRMatch, nRLength, pRSect);

	delete[] pszLeft;
	delete[] pszRight;
	delete[] pLMatch;
	delete[] pRMatch;

	return pLSect->GetSize() > 0;
}

void CFilDiffDoc::CreateSect(BYTE* pMatch, int nLength, CUIntArray* pSect)
{
	int j = 0;
	int i = 0;
	for (i = 0; i < nLength; i++)
	{
		if (j % 2 == 0 && pMatch[i] == 0)
		{
			pSect->Add(i);
			j++;
		}
		else if (j % 2 == 1 && pMatch[i] == 1)
		{
			pSect->Add(i);
			j++;
		}
	}

	if (j % 2 == 1)
		pSect->Add(i);

}

void CFilDiffDoc::MatchStrings(int nStrLen, LPCTSTR pszLeft, LPCTSTR pszRight, BYTE* pLMatch, BYTE* pRMatch)
{
	CTextSectMap Left;
	CTextSectMap Right;

	bool bChanged = false;
	do
	{
		/*
			if (MatchStart(pszLeft, pszRight, pLMatch, pRMatch)
				|| MatchEnd(pszLeft, pszRight, pLMatch, pRMatch)
			{
			  bChanged = true;
			}
			*/

		bChanged = false;
		CreateTextSectMap(nStrLen, pszLeft, pLMatch, Left);
		CreateTextSectMap(nStrLen, pszRight, pRMatch, Right);

		if (Match(Left, Right, pLMatch, pRMatch) ||
			Match(Right, Left, pRMatch, pLMatch))
		{
			bChanged = true;
		}

	} while (bChanged);

}

bool CFilDiffDoc::Match(CTextSectMap& Map1, CTextSectMap& Map2, BYTE* pMatch1, BYTE* pMatch2)
{
	//Loop through all left, and search for unique strings
	CTextSect TextSectKey1;
	CTextSect TextSectKey2;
	CTextSect TextSect1;
	CTextSect TextSect2;
	for (POSITION pos = Map1.GetStartPosition(); pos; )
	{
		Map1.GetNextAssoc(pos, TextSectKey1, TextSect1);
		if (TextSect1.m_pszText != 0)
		{
			Map2.Lookup(TextSect1, TextSect2);
			if (TextSect2.m_pszText)
			{
				//They match
				MarkMatch(TextSect1.m_pszText, TextSect2.m_pszText, pMatch1, pMatch2, TextSect1.m_nStart, TextSect2.m_nStart);

				//Remove them from the map
				Map1.RemoveKey(TextSectKey1);
				Map2.RemoveKey(TextSectKey1);
				return true;
			}
		}
	}
	return false;
}

void CFilDiffDoc::MarkMatch(LPCTSTR pszText1, LPCTSTR pszText2, BYTE* pMatch1,
	BYTE* pMatch2, WORD nPos1, WORD nPos2)
{
	//Forwards
	int i = 0;
	while (pszText1[nPos1 + i] && pszText2[nPos2 + i] && pszText1[nPos1 + i] == pszText2[nPos2 + i] &&
		pMatch1[nPos1 + i] == 0 && pMatch2[nPos2 + i] == 0)
	{
		pMatch1[nPos1 + i] = 1;
		pMatch2[nPos2 + i] = 1;
		i++;
	}

	//Backwards
	i = 1;
	while (nPos1 - i > 0 && nPos2 - i > 0 && pszText1[nPos1 - i] == pszText2[nPos2 - i] &&
		pMatch1[nPos1 - i] == 0 && pMatch2[nPos2 - i] == 0)
	{
		pMatch1[nPos1 - i] = 1;
		pMatch2[nPos2 - i] = 1;
		i++;
	}
}

void CFilDiffDoc::CreateTextSectMap(int nStrLen, LPCTSTR pszText, BYTE* pMatch, CTextSectMap& Map)
{
	CTextSect TextSect;
	TextSect.m_pszText = pszText;
	for (int i = 0; pszText[i] != 0; i++)
	{
		if (pMatch[i] == 0 && TextSect.m_nLength < nStrLen)
		{
			if (TextSect.m_nLength == 0)
				TextSect.m_nStart = i;

			TextSect.m_nLength++;
		}
		else
		{
			if (TextSect.m_nLength > 0)
			{
				CTextSect Found;
				if (Map.Lookup(TextSect, Found))
				{
					if (Found.m_pszText)
					{
						Found.m_pszText = NULL;
						Map.SetAt(TextSect, Found);
					}
				}
				else
				{
					Map.SetAt(TextSect, TextSect);
				}
			}

			TextSect.m_nLength = 0;
		}
	}
}


template <> UINT AFXAPI HashKey(CTextSect TextSect)
{
	UINT nHash = 0;
	TCHAR* p = const_cast<TCHAR*>(&TextSect.m_pszText[TextSect.m_nStart + TextSect.m_nLength]);
	TCHAR c = *p;
	*p = NULL;
	nHash = HashKey((LPCTSTR)&TextSect.m_pszText[TextSect.m_nStart]);
	*p = c;
	return nHash;
}

BOOL AFXAPI CompareElements(const CTextSect** ppTextSect1, const CTextSect** ppTextSect2)
{
	const CTextSect* pTextSect1 = *ppTextSect1;
	const CTextSect* pTextSect2 = *ppTextSect2;

	TCHAR* p1 = const_cast<TCHAR*>(&pTextSect1->m_pszText[pTextSect1->m_nStart + pTextSect1->m_nLength]);
	TCHAR* p2 = const_cast<TCHAR*>(&pTextSect2->m_pszText[pTextSect2->m_nStart + pTextSect2->m_nLength]);
	TCHAR c1 = *p1;
	TCHAR c2 = *p2;
	BOOL bResult = CompareElements((LPCTSTR)&pTextSect1->m_pszText[pTextSect1->m_nStart],
		(LPCTSTR)&pTextSect2->m_pszText[pTextSect2->m_nStart]);

	*p1 = c1;
	*p2 = c2;
	return bResult;
}

