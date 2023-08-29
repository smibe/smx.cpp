#ifndef __VIEWPASTE_INL__
#define __VIEWPASTE_INL__
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


#include "SECEdit.h"


//@doc SECEditCore
//@mfunc Pastes the contents of the clipboard into the document
//@rdesc int 
//@parm BOOL bUpdate
//@parm  BOOL bSaveUndoRecord
template <class BC>
int SECEditCore<BC>::Paste(BOOL bUpdate, BOOL bSaveUndoRecord)
{
	USES_CONVERSION_T;

	HANDLE hClipData;
	LPSTR lpClipData;
	LPSTR lpTemp;
	LPSTR lpText;
	TEXTPOS TextPos;
	int iSize;
	int iRval;
	char search[4] = {0x0D,0x0A,0,0};
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);

	// open the clipboard
	if (!OpenClipboard())
		return IDS_OE_EMPTYCLIPBOARD;

	// get text from the clipboard
	hClipData = GetClipboardData(CF_TEXT);
	if (!hClipData)
	{
		CloseClipboard();
		return IDS_OE_EMPTYCLIPBOARD;
	}
	pEdit->SetUndoGroup(TRUE);

	// if there's a block marked, cut it
	Cut(FALSE,bSaveUndoRecord);

	lpClipData = (LPSTR)GlobalLock(hClipData);

	iSize = strlen(lpClipData);

	lpText = (LPSTR)SECEditAlloc(iSize+1);
	strcpy(lpText,lpClipData);
	GlobalUnlock(hClipData);
	CloseClipboard();

	// is it a column block?
	if ((iSize > 21) && !memcmp(lpText,"*SECEdit Column Block*",22))
	{
		delete [] lpText;
		pEdit->SetUndoGroup();
		return ColumnPaste(bUpdate,bSaveUndoRecord);
	}

	// is there more than one line?
	lpTemp = strpbrk(lpText,search);
	if (!lpTemp)
	{
		int iSaveColNo = m_iColNo;
		iRval = InsertString(A2T(lpText),bUpdate,bSaveUndoRecord);
		if (m_bPasteAfter)
			m_iColNo = iSaveColNo;
	}
	else
	{
		TextPos.pLine = m_pCurLine;
		TextPos.iOffset = pEdit->GetRealOffset(m_pCurLine,m_iColNo);
		iRval = pEdit->Paste(&TextPos,lpText,iSize,bSaveUndoRecord);
		if (!m_bPasteAfter)
		{
			// find out if we have to adjust for a trailing lineend
			BOOL bAdjust = FALSE;
			if (lpText[iSize-pEdit->m_iLineEndLength] == pEdit->m_szLineEnd[0])
				bAdjust = TRUE;

			PLINEDESC pLine = TextPos.pLine;
			int iPos = TextPos.iOffset;
			while (iPos <= iSize)
				{
				if ((iPos + pLine->iTxtSize) >= iSize)
					{
					if (bAdjust && pLine->pNext)
						{
						pLine = pLine->pNext;
						m_iColNo = 0;
						}
					else
						m_iColNo = iSize - iPos;
					break;
					}
				else
					iPos += pLine->iTxtSize;
				if (pLine)
					pLine = pLine->pNext;
				}
			m_pCurLine = pLine;
			CountLines();
			m_iLineNo = pEdit->GetLineNo(m_pCurLine);
			int nNumLines = m_rect.bottom / m_iLineHeight;
			nNumLines--;
			if ((m_iLineNo-nNumLines) > m_iFirstLineNo)
				{
				m_iFirstLineNo = (m_iLineNo - nNumLines);
				m_pTopLine = pEdit->GoToLineNo(m_iFirstLineNo);
				}
			}
		}

	delete [] lpText;

	if (bUpdate)
	{
		CountLines();
		MakeCursorVisible();
		InvalidateRect(NULL,FALSE);
		UpdateWindow();
		UpdateViews(this,NULL,NULL);
	}

	pEdit->SetUndoGroup();
	return iRval;
}

//@doc SECEditCore
//@mfunc Pastes a column block into the view
//@rdesc int 
//@parm BOOL bUpdate
//@parm  BOOL bSaveUndoRecord
template <class BC>
int SECEditCore<BC>::ColumnPaste(BOOL bUpdate, BOOL bSaveUndoRecord)
   {
	USES_CONVERSION_T;

   HANDLE hClipData;
   LPSTR lpClipData;
   LPSTR lpTemp;
   LPSTR lpText;
   TEXTPOS TextPos;
   int iSize;
   int iRval;
   int iOffset;
   char search[4] = {0x0D,0x0A,0,0};
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);

   // open the clipboard
   if (!OpenClipboard())
      return IDS_OE_EMPTYCLIPBOARD;

   // get text from the clipboard
   hClipData = GetClipboardData(CF_TEXT);
   if (!hClipData)
      {
      CloseClipboard();
      return IDS_OE_EMPTYCLIPBOARD;
      }
   pEdit->SetUndoGroup(TRUE);

   // if there's a block marked, cut it
   Cut(FALSE,bSaveUndoRecord);

   lpClipData = (LPSTR)GlobalLock(hClipData);

   iSize = strlen(lpClipData);
   lpText = (LPSTR)SECEditAlloc(iSize+1);
   strcpy(lpText,lpClipData);
   GlobalUnlock(hClipData);
   CloseClipboard();

   // if it has our column block header, strip it off
   if ((iSize > 21) && !memcmp(lpText,"*SECEdit Column Block*",22))
      iOffset = 24;
   else
      iOffset = 0;

   // is there more than one line?
   lpTemp = strpbrk(&lpText[iOffset],search);
   if (!lpTemp)
      {
      iRval = InsertString(A2T(&lpText[iOffset]),bUpdate,bSaveUndoRecord);
  		delete [] lpText;
	  pEdit->SetUndoGroup();
      return iRval;
      }

   TextPos.pLine = m_pCurLine;
	TextPos.iOffset = pEdit->GetRealOffset(m_pCurLine,m_iColNo);

   iRval = pEdit->ColumnPaste(&TextPos,&lpText[iOffset],iSize,bSaveUndoRecord);

	delete [] lpText;

   if (iRval)
      return iRval;

   if (bUpdate)
      {
		CountLines();
      MakeCursorVisible();
      InvalidateRect(NULL,FALSE);
      UpdateWindow();
	  UpdateViews(this, NULL, NULL);
   }

   pEdit->SetUndoGroup();
   return 0;
   }

#endif //__VIEWPASTE_INL__