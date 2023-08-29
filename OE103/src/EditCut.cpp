// 
// Stingray Software Extension Classes
// Copyright (C) 1997 Steve Schauer
// Copyright (C) 1997 Stingray Software Inc.
// All Rights Reserved.
// 
// This source code is only intended as a supplement to the
// Stingray Extension Class product.
// See the SEC help files for detailed information
// regarding using SEC classes.
// 
//----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
//
// EditCut.cpp
//
//
// Description: Low level support for cutting a stream of text from
//              the linked list.
//

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "secedit.h"

//
// cuts from pStartPos to pEndPos.  Never deletes pStartPos->pLine
//
// Returns:      0 on success
//               errorcode on failure

int SECEdit::sm_nFoldedState   = 0;
int SECEdit::sm_nUnFoldedState = 0;

//@doc SECEdit
//@mfunc Internal function. Cuts the text from pStartPos to pEndPos.  Never deletes
//              pStartPos->pLine.
//@rdesc int 
//@parm PTEXTPOS pStartPos
//@parm  PTEXTPOS pEndPos
//@parm  BOOL bSaveUndoRecord
int SECEdit::Cut(PTEXTPOS pStartPos, PTEXTPOS pEndPos, BOOL bSaveUndoRecord)
   {
   int iRval;
   UNDORECORD UndoRecord;
   LPSTR lpText;
   PLINEDESC pFirstDeadLine,pNextDeadLine,pLastDeadLine;
   TEXTPOS TempPos;
	ASSERT(pStartPos);
	ASSERT(pEndPos);

   iRval = 0;

   if (m_bReadOnly)
      return IDS_OE_READONLY;

   if ((pStartPos->pLine == pEndPos->pLine) && 
		 (pStartPos->iOffset == pEndPos->iOffset))
      return IDS_OE_NOSELECTION;

   if ((pStartPos->pLine == NULL) || (pEndPos->pLine == NULL))
      return IDS_OE_NOSELECTION;

   // save the undo record
   if (bSaveUndoRecord)
      {
      UndoRecord.iRow    = GetLineNo(pStartPos->pLine);
		if (pStartPos->pLine == pEndPos->pLine)
			UndoRecord.iEndRow = UndoRecord.iRow;
		else
			UndoRecord.iEndRow = GetLineNo(pEndPos->pLine);
      UndoRecord.iCol    = pStartPos->iOffset;
      UndoRecord.iEndCol = pEndPos->iOffset;
      UndoRecord.iUndoType = OE_UNDO_CUT;
      lpText = Copy(pStartPos, pEndPos, FALSE);
      }

   if (pStartPos->pLine == pEndPos->pLine)
		{
      iRval = DeleteString(pStartPos, pEndPos->iOffset-pStartPos->iOffset, FALSE);
		}
   else
      {
      // lop the starting line off at iOffset
      if (pStartPos->iOffset < (pStartPos->pLine->iTxtSize-m_iLineEndLength))
         {
         pStartPos->pLine->pText[pStartPos->iOffset] = 0;
         pStartPos->pLine->iTxtSize = pStartPos->iOffset;
         }

      // delete the marked part of the last line
      if (pEndPos->iOffset > pEndPos->pLine->iTxtSize)
			{
			pEndPos->iOffset = pEndPos->pLine->iTxtSize;
         /* (maybe) adjust for OE_CR/OE_LF */
			char cFirst,cSecond;
			cFirst = pEndPos->pLine->pText[pEndPos->pLine->iTxtSize-m_iLineEndLength];
			cSecond = m_szLineEnd[0];
         if (cFirst == cSecond)
				{
	         pEndPos->iOffset -= m_iLineEndLength;
		      UndoRecord.iEndCol -= m_iLineEndLength;
				}
			}

      TempPos.pLine = pEndPos->pLine;
      TempPos.iOffset = 0;
      iRval = DeleteString(&TempPos, pEndPos->iOffset, FALSE);

      // link the two together, and unlink the ones in between
      if ((!iRval) || (iRval == IDS_OE_JOINLINE))
         {
         if (pStartPos->pLine->pNext != pEndPos->pLine)
            {
            pFirstDeadLine = pStartPos->pLine->pNext;
				if (pEndPos->pLine)
					{
               pLastDeadLine = pEndPos->pLine->pPrev;
	            pEndPos->pLine->pPrev = pStartPos->pLine;
					}
				else
					pLastDeadLine = NULL;
            pStartPos->pLine->pNext = pEndPos->pLine;
				if (pLastDeadLine)
               pLastDeadLine->pNext = NULL;
            while (pFirstDeadLine)
               {
               pNextDeadLine = pFirstDeadLine->pNext;
               FreeTextLine(pFirstDeadLine);
               pFirstDeadLine = pNextDeadLine;
               }
            }
         // join the chopped start with the chopped end
         iRval = JoinLines(pStartPos, FALSE);
			if (iRval == IDS_OE_EOF)
				iRval = 0;
         }
      }

   if (!iRval)
      {
      if (bSaveUndoRecord)
			{
         SaveUndoRecord(&UndoRecord,lpText);
		   delete [] lpText;
			}

      // set the changed flag
      SetModified(TRUE);
      }

   return iRval;
   }

//@doc SECEdit
//@mfunc Internal function.  Cuts the column of text from pStartPos to pEndPos, 
//              from column pStartPos->iOffset to pEndPos->iOffset.
//@rdesc int 
//@parm PTEXTPOS pStartPos
//@parm  PTEXTPOS pEndPos
//@parm  BOOL bSaveUndoRecord
int SECEdit::ColumnCut(PTEXTPOS pStartPos, PTEXTPOS pEndPos, BOOL bSaveUndoRecord)
   {
   int iRval;
   UNDORECORD UndoRecord;
   LPSTR lpText;
   PLINEDESC pLine;
   TEXTPOS TextPos;
   TEXTPOS StartPos,EndPos;

   if (m_bReadOnly)
      return IDS_OE_READONLY;

   // column blocks may not be in order...
   if (!InOrder(pStartPos,pEndPos))
      {
      StartPos = *pEndPos;
      EndPos = *pStartPos;
      }
   else
      {
      StartPos = *pStartPos;
      EndPos = *pEndPos;
      }

   iRval = 0;

	UndoRecord.iCol    = StartPos.iOffset;
	UndoRecord.iEndCol = EndPos.iOffset;

	if (bSaveUndoRecord)
		{
		UndoRecord.iRow    = GetLineNo(StartPos.pLine);
		UndoRecord.iEndRow = GetLineNo(EndPos.pLine);
		UndoRecord.iUndoType = OE_UNDO_COLUMNCUT;
		lpText = ColumnCopy(&StartPos, &EndPos, FALSE);
		}

	int iTabbedOffset = GetTabbedOffset(StartPos.pLine,StartPos.iOffset);
	int iCount = UndoRecord.iEndCol - UndoRecord.iCol;
   pLine = StartPos.pLine;
   while (pLine)
      {
      TextPos.pLine = pLine;
		TextPos.iOffset = GetRealOffset(TextPos.pLine,iTabbedOffset);
      iRval = DeleteString(&TextPos, iCount, FALSE);
		if (pLine == EndPos.pLine)
			break;
		pLine = pLine->pNext;
      }

   if (!iRval && bSaveUndoRecord)
		{
      SaveUndoRecord(&UndoRecord, lpText);
		   delete [] lpText;
		}

   return iRval;
   }

PLINEDESC SECEdit::DetachLines(PLINEDESC pStart, PLINEDESC pEnd)
{
  if(pStart->pPrev == NULL)
  {
    m_pLine = pEnd->pNext;
    if(pEnd->pNext)
      pEnd->pNext->pPrev = NULL;
  }
  else
  {
    pStart->pPrev->pNext = pEnd->pNext;
    if(pEnd->pNext)
      pEnd->pNext->pPrev = pStart->pPrev;
  }

  pEnd->pNext   = NULL;
  
  return pStart;
}
BOOL SECEdit::CutLines(PLINEDESC pStart, PLINEDESC pEnd)
{
  if (DetachLines(pStart, pEnd) == 0)
    return FALSE;

  return FreeLines(pStart, pEnd);
}

BOOL SECEdit::FreeLines(PLINEDESC pStart, PLINEDESC pEnd)
{
  if (!DeleteLines(pStart, pEnd))
    return FALSE;

  SetModified(TRUE);
  return TRUE;
}
		
BOOL SECEdit::DeleteLines(PLINEDESC pStart, PLINEDESC pEnd)
{
  //Now delete the lines
  bool bDone = false;
  PLINEDESC pLine = pStart;
  PLINEDESC pLink = NULL;

  if (m_pLine == pStart)
    m_pLine = pEnd->pNext;

  while(pLine)
  {
    pLine = pLine->pNext;
    if(pStart == pEnd)
    {
      bDone = true;
    }

    //Unlink the lines
    FreeTextLine(pStart);
    pStart = pLine;

    if(bDone)
      break;
  }

  if (m_pLine == NULL)
    m_pLine = AllocLine(_T(""));
  return TRUE;
}

void  SECEdit::UnFoldAll()
{
  PLINEDESC pLine = m_pLine;
  PLINEDESC pNext = m_pLine;
  if (sm_nFoldedState <= 0)
    return;

  while (pLine)
  {
    pNext = pLine->pNext;

    if ((int) pLine->state == sm_nFoldedState)
      UnFoldLines(pLine);

    pLine = pNext;
  }
}

PLINEDESC SECEdit::UnFoldLines(PLINEDESC pStart, bool bSaveUndoRecord)
{
  BOOL bModified = GetModified();
  PLINEDESC pFoldedLines = NULL;
  PLINEDESC pLastFolded  = NULL;

  if (m_FoldedMap.Lookup(pStart, (void *&)pFoldedLines))
  {
    PLINEDESC pBef        = NULL;
    PLINEDESC pAfter      = NULL;

    if (m_pOnUnFoldLine)
      (m_pObj->*m_pOnUnFoldLine)(this, pStart);

    pLastFolded = pFoldedLines;
    while(pLastFolded && pLastFolded->pNext)
      pLastFolded = pLastFolded->pNext;

    pAfter = pStart->pPrev;
    pBef   = pStart->pNext;

    if (DetachLines(pStart, pStart) != NULL)
    {
      PLINEDESC pTemp = pFoldedLines;

      //Restore the state to STATE_SAME
      while (pTemp)
      {
        pTemp->state = sm_nUnFoldedState;
        pTemp = pTemp->pNext;
      }

      InsertLines(pBef, pAfter, pFoldedLines);

      // Link the first line 
      pFoldedLines->link = pStart->link;
      pTemp = pStart->link;
      while (pTemp && pTemp->link && pTemp->link != pStart)
        pTemp = pTemp->link;
  
      if (pTemp)
        pTemp->link = pFoldedLines;

    }
    else
    {
      ASSERT(FALSE);
    }
  }
  else
  {
    ASSERT(FALSE);
  }

   
  // save the undo record
  if (bSaveUndoRecord)
  {
    UNDORECORD UndoRecord;

    UndoRecord.iRow    = GetLineNo(pFoldedLines);
    UndoRecord.iEndRow = GetLineNo(pLastFolded);

    UndoRecord.iCol    = 0;
    UndoRecord.iEndCol = 0;
    UndoRecord.iUndoType = OE_UNDO_UNFOLD;

    LPCTSTR pszText = NULL;
    if (strncmp(pStart->pText, "+---", 4) != 0)
      pszText = pStart->pText;

    SaveUndoRecord(&UndoRecord, pszText);
  }

  //Don't change the modified flag.
  SetModified(bModified);

  if (pFoldedLines)
  {
    pStart->link = NULL;
    FreeLines(pStart);
    m_FoldedMap.RemoveKey(pStart);
  }
  return pFoldedLines;
}

PLINEDESC SECEdit::FoldLines(PLINEDESC pStart, PLINEDESC pEnd, LPCTSTR pszFoldedText)
{
  PLINEDESC pLine       = NULL;
  PLINEDESC pFoldedLine = NULL;
  PLINEDESC pBef        = NULL;
  PLINEDESC pAfter      = NULL;
  BOOL bModified = GetModified();

  pAfter = pStart->pPrev;
  pBef   = pEnd->pNext;

  pLine = DetachLines(pStart, pEnd);

  pFoldedLine = CreateFoldedLine(pStart->iLineNo, pEnd->iLineNo, pszFoldedText);

  //Set the state to folded
  PLINEDESC pTemp = pStart;
  while (pTemp)
  {
    pTemp->state = sm_nFoldedState;
    pTemp = pTemp->pNext;
  }

  // Link it in
  pFoldedLine->link = pStart->link;
  pTemp = pStart->link;
  while (pTemp && pTemp->link && pTemp->link != pStart)
    pTemp = pTemp->link;
  if (pTemp)
    pTemp->link = pFoldedLine;
  else
    ASSERT(pStart->link == NULL);

  pStart->link = NULL;

  pFoldedLine->state = sm_nFoldedState;
  m_FoldedMap.SetAt(pFoldedLine, pLine);

  InsertLines(pBef, pAfter, pFoldedLine);

  pLine->pPrev = pFoldedLine;

  //Don't change the modified flag.
  SetModified(bModified);

  if (m_pOnFoldedLines)
    (m_pObj->*m_pOnFoldedLines)(this, pFoldedLine);

  return pFoldedLine;
}

PLINEDESC SECEdit::CreateFoldedLine(int iStart, int iEnd, LPCTSTR pszFoldedText)
{
   CString str;

   if (pszFoldedText)
     str = pszFoldedText;
   else
     str.Format("+---- %d lines hidden ------------", iEnd - iStart + 1);

   PLINEDESC pFoldedLine = AllocLine(str);
   return pFoldedLine;
}

void SECEdit::Unlink(PLINEDESC pLine)
{
  PLINEDESC pLink = NULL;
  if (pLine && pLine->link)
  {
    int i = 0;
    pLink = pLine->link;

    while (pLink && pLink->link != pLine && i < 10)
    {
      i++;
      pLink = pLink->link;
    }

    ASSERT(i < 3);
  
    if (pLink)
    {
      pLink->link = pLine->link;
      //pLine->link = NULL;
    }
  }
}
   


#ifdef _DEBUG
LINEDESC::~LINEDESC()
{
  //ASSERT(link == NULL);
  PLINEDESC pLink;
  if (link)
  {
    int i = 0;
    pLink = link;

    if (link != this)
    {
      while (pLink &&  i < 10)
      {
        ASSERT(pLink->link != this);
        i++;
        pLink = pLink->link;
      }
    }
  }
}
#endif
