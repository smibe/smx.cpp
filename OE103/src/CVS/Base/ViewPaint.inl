#ifndef __VIEWPAINT_INL__
#define __VIEWPAINT_INL__
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
// ViewPaint.inl
//
//
// Description: Routines to render the current view on screen
//
//



#include "SECEdit.h"


//@doc SECEditCore
//@mfunc Virtual function called to draw bookmarks, etc.
//              Override to draw custom marks.  The LINEDESC structure
//              #defines eight flags that can be set for your implementation.
//@rdesc void 
//@parm CPaintDC* dc The current paint DC.
//@parm  RECT* rect The rectangle to draw into.
//@parm  PLINEDESC pLine A line description structure, there are several fields you can use here to help you customize your drawing.

template <class BC>
void SECEditCore<BC>::DrawMarks(CPaintDC* dc, RECT* rect, PLINEDESC pLine)
{
	CRect temprect;
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);

	if (pLine->fBookMark)
		{
		temprect.top = rect->top;
		temprect.left = 2;
		temprect.right = ((m_iMarkSpace/2)*2)-2;
		temprect.top += (((rect->bottom - rect->top) / 2) - ((temprect.right - temprect.left) / 2));
		temprect.bottom = temprect.top + (temprect.right - temprect.left);
		CBrush Brush(RGB(0,0,255));  // blue
		CBrush* pOldBrush = dc->SelectObject(&Brush);
		dc->Ellipse(&temprect);
		dc->SelectObject(pOldBrush);
		dc->SetBkColor(pEdit->m_crBackground);
		}
}


//@doc SECEditCore
//@mfunc Draws the current Objective Edit view on the screen DC
//@rdesc int 0 on success, error code on failure.

template <class BC>
int SECEditCore<BC>::Paint(void)
{
	USES_CONVERSION_T;

	CPaintDC dc(this); // device context for painting
	// Use GetActiveDoc for SECEditCtrl
	CDocument* pDoc = GetActiveDoc();
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);
   POINT ptBlock;
   int Y;
   PLINEDESC lpLine;
   PLINEDESC lpNextLine;
   PLINEDESC pstTempLine;
   int iLength;
   BOOL bBlock;
   RECT rect;
   UINT wMaxChars;
   int bottom;
   int top;
   UINT uOptions;
   RECT temprect;
	char TextLine[OE_MAXLINELENGTH+1];

   uOptions = ETO_OPAQUE;

   lpLine = m_pTopLine;
   if (!lpLine)
      return 0;
      
   /* is a block defined?  */   
   if (m_BlockStart.pLine == NULL)
      bBlock = FALSE;
   else
      bBlock = TRUE;

   CFont* oldfont = dc.SelectObject(&m_font);

	dc.SetBkColor(pEdit->m_crBackground);
   dc.SetTextColor(pEdit->m_crText);

   pstTempLine = lpLine;

   Y=0;
   rect = m_rect;
   wMaxChars = (rect.right/m_iCharWidth)-1;

   rect.left += m_iMarkSpace;

   bottom = dc.m_ps.rcPaint.bottom;
   top    = dc.m_ps.rcPaint.top;

   while (Y < bottom)
   {
      if ((Y+m_iLineHeight) > top)
      {
         if (lpLine)
            iLength = PaintExpandTabs(pstTempLine->pText,TextLine,pstTempLine->iTxtSize,pEdit->m_iTabSize,pEdit->m_iLineEndLength);
         else
            iLength = 0;

         if (iLength)
            {
            /* (maybe) adjust for OE_CR/OE_LF */
				char cFirst,cSecond;
				cFirst = TextLine[iLength-pEdit->m_iLineEndLength];
				cSecond = pEdit->m_szLineEnd[0];
            if (cFirst == cSecond)
               iLength -= pEdit->m_iLineEndLength;
            /* adjust for leftedge */
            iLength -= m_iLeftEdge;
            }
         
         rect.top = rect.bottom = Y;
         rect.bottom += m_iLineHeight;

			// do an empty fullwidth rect
         temprect.top = rect.top;
         temprect.bottom = rect.bottom;
			temprect.left = 0;
			temprect.right = m_iMarkSpace;

			// Draw the Gutter Area
			COLORREF clrOldText = dc.GetTextColor();
			COLORREF clrGutter = GetGutterColor(pEdit->m_crBackground);
			
			dc.SetTextColor(clrGutter);
			dc.FillRect(&temprect, &m_brGutter);
			dc.SetTextColor(clrOldText);


			if (pstTempLine)
				DrawMarks(&dc,&temprect,pstTempLine);

			//GeH: trick to insert empty line
			if(!temprect.right) {
				Y += m_iLineHeight;
			}

			temprect.top = rect.top;
			temprect.bottom = rect.bottom;
			temprect.right = rect.right;

			bool bPaintMarkedText = false;
			bool bPaintEmptyMarkedLine = false;

			if (lpLine && bBlock && (pDoc == GetActiveDoc()) && 
					 (pEdit->m_pActiveView == this) && 
					 InBlock(pstTempLine,iLength+m_iLeftEdge,&ptBlock))
			{
				ptBlock.x -= m_iLeftEdge;
				if (ptBlock.x < 0)
				{
					ptBlock.y += ptBlock.x;
					ptBlock.x = 0;
				}
				if (ptBlock.y > 0)
				{
					bPaintMarkedText = true;
				}
				else if (iLength == 0 && bBlock && InBlockEMptyLine(pstTempLine))
				{
					bPaintEmptyMarkedLine = true;
				}
			}

         if (iLength > 0)
			{
				if (bPaintMarkedText)
				{
					COLORREF crTxt = dc.GetTextColor();
					dc.SetTextColor(GetSysColor(COLOR_WINDOW));
					COLORREF crBk = dc.GetBkColor();
					dc.SetBkColor(RGB(0, 0, 0));
					PaintTextOut(pEdit, 
									 lpLine, 				// pointer to the current line
									 TextLine, 				// the full text line, tabs expanded
									 iLength+m_iLeftEdge,// the full length of the text
									 m_iLeftEdge+ptBlock.x, 			// first character to draw
									 ptBlock.y, 			// max chars to draw
									 FALSE, 	            // no color
									 &dc,					 	// screen dc
									 ptBlock.x*m_iCharWidth+m_iMarkSpace, 			// textout coordinates
									 rect.top);				//		 "			 "
					dc.SetTextColor(crTxt);
					dc.SetBkColor(crBk);
					PaintTextOut(pEdit, lpLine,  TextLine, iLength+m_iLeftEdge,
									 m_iLeftEdge, ptBlock.x, pEdit->m_bChroma, &dc, rect.left, rect.top);
					PaintTextOut(pEdit, lpLine,  TextLine, iLength+m_iLeftEdge,
									 m_iLeftEdge + ptBlock.x + ptBlock.y, iLength - ptBlock.x - ptBlock.y, 
									 pEdit->m_bChroma, &dc, 
									 ptBlock.x*m_iCharWidth+m_iMarkSpace + ptBlock.y*m_iCharWidth,
									 rect.top);

				}
				else
				{
					PaintTextOut(pEdit, 
									 lpLine, 				// pointer to the current line
									 TextLine, 				// the full text line, tabs expanded
									 iLength+m_iLeftEdge,// the full length of the text
									 m_iLeftEdge, 			// first character to draw
									 wMaxChars, 			// max chars to draw
									 pEdit->m_bChroma, 	// do it in color?
									 &dc,					 	// screen dc
									 rect.left, 			// textout coordinates
									 rect.top);				//		 "			 "
				}

				// erase the balance of the rectangle, if any
				temprect.left = (m_iCharWidth * iLength) + m_iMarkSpace;
				temprect.right = rect.right;
				if (temprect.right > temprect.left)
					dc.ExtTextOut(temprect.left,temprect.top,uOptions,&temprect,
									  NULL,0,NULL);
         }
         else
			{
				// do an empty fullwidth rect
				temprect.left = m_iMarkSpace;
				temprect.right = rect.right;
				dc.ExtTextOut(temprect.left,temprect.top,uOptions,&temprect,
					        NULL,0,NULL);
			
				if (bPaintEmptyMarkedLine)
				{
					CRect rcChar(m_iMarkSpace,Y, m_iMarkSpace + m_iCharWidth, Y + m_iLineHeight);
					dc.FillSolidRect(rcChar, RGB(0, 0, 0));
				}
			}

      }
      if (lpLine)
         {
         lpNextLine = pstTempLine->pNext;
         if (lpNextLine)
            lpLine = lpNextLine;
         }
      else
         lpNextLine = NULL;
      Y += m_iLineHeight;
      if (lpLine && !lpNextLine)
         lpLine = lpLine->pNext;
      if (lpLine)
         pstTempLine = lpLine;
		else
			pstTempLine = NULL;
   }

   dc.SelectObject(oldfont);
	return 0;
}

//@doc SECEditCore
//@mfunc Internal function that determine if pstLine is in a block.  If it is, also
//              update the passed in POINT structure to return the beginning
//              column in ptBlock.x and the width in columns in ptBlock.y
//@rdesc BOOL Is it a block?
//@parm PLINEDESC pstLine The line structure to analyze.
//@parm  int iLength Length of the line.
//@parm  POINT *ptBlock Pointer to the block
template <class BC>
 BOOL SECEditCore<BC>::InBlock(PLINEDESC pstLine, int iLength, POINT *ptBlock)
             				
   {
   PLINEDESC pLine;
   PLINEDESC TheOther;

   if (!m_BlockStart.pLine || !m_BlockEnd.pLine)
      return FALSE;

   pLine = pstLine;

   if (m_iBlockType == OE_COLUMN)
      {
      if ((pLine == m_BlockStart.pLine) ||
          (pLine == m_BlockEnd.pLine))
         {
         ptBlock->x = min(m_BlockStart.iOffset,m_BlockEnd.iOffset);
         ptBlock->y = max(m_BlockStart.iOffset,m_BlockEnd.iOffset) - ptBlock->x;
         return TRUE;
         }
      else
         {
         // look backwards for BlockStart or BlockEnd
         TheOther = NULL;
         while (pLine)
            {
            if (!TheOther)  // haven't found either yet
               {
               if (pLine == m_BlockEnd.pLine)
                  TheOther = m_BlockStart.pLine;
               else if (pLine == m_BlockStart.pLine)
                  TheOther = m_BlockEnd.pLine;
               }
            if (TheOther) // we've found one, look for TheOther
               if (pLine == TheOther)  // found both, we're not in
                  return FALSE;
            pLine = pLine->pPrev;
            }
         }

      if (!pLine && TheOther)  // we were in between BlockStart and BlockEnd
         {
         ptBlock->x = min(m_BlockStart.iOffset,m_BlockEnd.iOffset);
         ptBlock->y = max(m_BlockStart.iOffset,m_BlockEnd.iOffset) - ptBlock->x;
         return TRUE;
         }
      }
   else
      {
      if (pLine == m_BlockStart.pLine)
         {
         ptBlock->x = min(m_BlockStart.iOffset,iLength);
         if (pLine == m_BlockEnd.pLine)
            ptBlock->y = min(m_BlockEnd.iOffset,iLength)  - ptBlock->x;
         else
            ptBlock->y = iLength - ptBlock->x;
         return TRUE;
         }
      else if (pLine == m_BlockEnd.pLine)
         {
         ptBlock->x = 0;
         ptBlock->y = min(m_BlockEnd.iOffset,iLength);
         return TRUE;
         }
      else
         {
         // look backwards for BlockStart or BlockEnd
         while (pLine)
            {
            if (pLine == m_BlockEnd.pLine)
               return FALSE;
            else if (pLine == m_BlockStart.pLine)
               {
               // the whole line is in
               ptBlock->x = 0;
               ptBlock->y = iLength;
               return TRUE;
               }
            pLine = pLine->pPrev;
            }
         return FALSE;
         }
      }
	return FALSE;
   }

//@doc SECEditCore
//@mfunc Internal function used to UnPatBlt the screen's selection
//@rdesc void 
//@parm void
template <class BC>
void SECEditCore<BC>::ClearBlockFromScreen(void)
   {
	CClientDC dc(this); // device context for painting
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);
   POINT ptBlock;
   int Y;
   PLINEDESC lpLine;
   LPSTR pTextLine;
   int iLength;
   RECT rect;

   //if (m_iBlockType == OE_COLUMN)
      {
      Invalidate(FALSE);
      return;
      }

   lpLine = m_pTopLine;
   if (!lpLine)
      return;

   if (m_BlockStart.pLine == NULL)
      return;

   pTextLine = lpLine->pText;
   rect = m_rect;
   Y=0;

   while (Y <= rect.bottom)
      {
      if ((Y+m_iLineHeight) >= rect.top)
      {
         if (pTextLine)
            iLength = lpLine->iTxtSize;
         else
            iLength = 0;
         if (iLength)
         {
            /* (maybe) adjust for OE_CR/OE_LF */
            if (pTextLine[iLength-pEdit->m_iLineEndLength] == pEdit->m_szLineEnd[0])
               iLength -= pEdit->m_iLineEndLength;
            // adjust for tabs
            iLength = pEdit->GetTabbedOffset(lpLine,iLength);
            if (InBlock(lpLine,iLength,&ptBlock))
            {
	            if (iLength > 0)
               {
						ptBlock.x -= m_iLeftEdge;
						if (ptBlock.x < 0)
						{
							ptBlock.y += ptBlock.x;
							ptBlock.x = 0;
						}
						if (ptBlock.y > 0)
							dc.PatBlt(ptBlock.x*m_iCharWidth+m_iMarkSpace,Y,
                         ptBlock.y*m_iCharWidth,m_iLineHeight,DSTINVERT);
               }
					//else
					//	dc.PatBlt(m_iMarkSpace,Y,
               //             m_iCharWidth/2,m_iLineHeight,DSTINVERT);
            }
         }
      }
      Y += m_iLineHeight;
      lpLine = lpLine->pNext;
      if (lpLine)
         {
         pTextLine = lpLine->pText;
         }
      else
         break;
      }

   }

// expands tabbed text in lpText into lpWorkBuffer, returns new expanded length


//@doc SECEditCore
//@mfunc Internal function that expands tabbed text in lpText into lpWorkBuffer
//@rdesc int Length of the lpWorkBuffer
//@parm LPTSTR lpText
//@parm  LPTSTR lpWorkBuffer
//@parm  int iTextSize
//@parm  int iTabSize
//@parm  int iLineEndLength
template <class BC>
int SECEditCore<BC>::PaintExpandTabs(LPSTR lpText, LPSTR lpWorkBuffer, int iTextSize, int iTabSize, int iLineEndLength)
   {
   LPSTR lpNext;
   LPSTR lpStart;
   int iNewTextSize,iInsert,iColNo,iLength;
	SECEdit* pEdit = GetEdit();
	ASSERT(pEdit != NULL);

   if (!iTextSize)
      return iTextSize;

	iLength = iTextSize;

   /* quick check for tabs, if any */
   lpNext = (LPSTR)memchr(lpText,9,iTextSize);

   if (!lpNext) /* no tabs, no work */
      {
      memcpy(lpWorkBuffer,lpText,iTextSize);
      lpWorkBuffer[iTextSize] = 0;
      return iTextSize;
      }

   lpNext = lpText;
     
   lpStart = lpWorkBuffer;

   iNewTextSize = iColNo = 0;

   while (iTextSize > 0)
      {
      switch (*lpNext)
         {
         case 0x09:
            lpNext++;
            iTextSize--;
            iInsert = iTabSize - (iColNo % iTabSize);
            if (!iInsert)
               iInsert = iTabSize;

				
				if (pEdit->m_bShowTabs)
					{
					// put a funny one in first
					iColNo++;
					iNewTextSize++;
					*lpWorkBuffer++ = '»';
					iInsert--;
					}
            for (;iInsert;iInsert--)
               {
               iColNo++;
               iNewTextSize++;
               *lpWorkBuffer++ = ' ';
               }
            break;

         case 0x0D:
         case 0x0A:
            /* skip OE_CR/LFs and reset our ColNo indicator */
            {
            int i;
            for (i=0;i<iLineEndLength;i++)
               *lpWorkBuffer++ = *lpNext++;
            iColNo = 0;
            iTextSize -= iLineEndLength;
            iNewTextSize += iLineEndLength;
            break;
            }

         default:
            *lpWorkBuffer++ = *lpNext++;
            iNewTextSize++;
            iColNo++;
            iTextSize--;

         }
      }

   lpStart[iNewTextSize] = 0;

   return iNewTextSize;
   }
#if 0
template <class BC>
int SECEditCore<BC>::GetWidth(CDC* dc, LPCTSTR lpText, int nCount)
	{
	CSize size = dc->GetTextExtent(lpText,nCount);
	return size.cx;
	}
#endif
#define OE_REGULAR 2
#define OE_KEYWORD 4	
#define OE_QUOTE   8
#define OE_COMMENT 16

template <class BC>
BOOL SECEditCore<BC>::PaintTextOut( SECEdit* pEdit, 
										   PLINEDESC lpLine, 	// pointer to the current line
											LPSTR str, 			// the full text line, tabs expanded
											int nLength,			// the full length of the text
											int nIndex, 			// first character to draw
											int nCount, 			// max characters to draw
											BOOL bChroma, 			// do it in color
											CDC* dc, 					// screen dc
											int x, 					// textout coordinates
											int y)					//		 "			 "
{
	USES_CONVERSION_T;

	if (!bChroma)
		return dc->TextOut( x, y, A2T(&str[nIndex]), min(nLength,nCount) );

	int iStart,iEnd;
	char crTemplate[OE_MAXLINELENGTH+1];
	memset(crTemplate,OE_REGULAR,sizeof(crTemplate));

	LPSTR pText = str;

   // keywords
   while (*pText && pEdit->KeyWord(&pText,&iStart,&iEnd))
      {
		// keywords get painted immediately because we
		// might change the value of m_crKeyWord from
		// word to word
      int iWordStart = pText - str;
      int iWordEnd = iWordStart + (iEnd-iStart);
		dc->SetTextColor(pEdit->m_crKeyWord);
		// find the starting point for the TextOut()
		if (iWordStart < nIndex)
			iWordStart = nIndex;
		if (iWordEnd > iWordStart)
			{
			int n = x + GetWidth(dc, &str[nIndex],iWordStart-nIndex);
			dc->TextOut( n, y, A2T(&str[iWordStart]), iWordEnd-iWordStart );
			for (;iWordStart!=iWordEnd;iWordStart++)
				crTemplate[iWordStart] |= OE_KEYWORD;
			}
		pText = &(str[iWordEnd]);
      }

   // comments
   int i = iStart = iEnd = 0;
   pText = str;
	int iTempLength = nLength;
	int iType;
	int iNext;
	BOOL bInComment = pEdit->InComment(lpLine, &iType);
   while (iTempLength > 0)
      {
      if (bInComment)
			{
         iNext = 0;
			bInComment = FALSE;
			}
      else
			iNext = pEdit->CommentStart(&pText[iStart],iTempLength,&iType);
                                                                        
      if (iNext >= 0)                                                  
         {
			iStart += iNext;
         iEnd = pEdit->CommentEnd(&pText[iStart],iTempLength-iNext,iType);
			int iCommentStart, iCommentEnd;
			iCommentStart = &pText[iStart] - pText;
			iCommentEnd = iStart+iEnd;
			for (;iCommentStart!=iCommentEnd;iCommentStart++)
				crTemplate[iCommentStart] |= OE_COMMENT;
                                                                        
         iStart += iEnd;
         iTempLength -= (iNext + iEnd);
         }
      else
         iTempLength = 0;
      }

   // quotes
   pText = str;
   iStart = -1;
   i = iEnd = 0;
   char quote;

	while (i < nLength)
		{
		// find the start
		while (nLength > i)
			{
			if ((pText[i] == '"') || (pText[i] == '\''))
				{
				// ignore if in comment
				if (!(crTemplate[i] & OE_COMMENT))
					{
					quote = pText[i];
					iStart = i+1;
					}
				}
			i++;

			if (iStart >= 0)                                                  
				{

				// find the matching end
				while (nLength > i)
					{
					if ((pText[i] == quote) && !(crTemplate[i] & OE_COMMENT))
						{
						iEnd = i++;
						break;
						}
               else
                  if ((pText[i] == '\\') && (!(crTemplate[i] & OE_COMMENT)))
                     i++; //skip next char as treated literally
					i++;
					}

				if (iEnd > iStart)
					{
					for (;iStart!=iEnd;iStart++)
						crTemplate[iStart] |= OE_QUOTE;
					}
				iStart = iEnd = -1;
				}
			}
		}

	// now we're ready to paint
	char szOutText[OE_MAXLINELENGTH+1];
	COLORREF cr;
	pText = str;
	int j = 0;
	int iLast = crTemplate[nIndex];
	int iOutMax = nCount;
	int iOutLength;
	szOutText[0] = 0;
	for (i = nIndex; pText[i] && i < nLength; i++)
		{
		// everytime our color value changes, do the textout
		if (crTemplate[i] != iLast)
			{
			iOutLength = min(j,iOutMax);
			if (iLast & OE_COMMENT)
				cr = pEdit->m_crComment;
			else if (iLast & OE_QUOTE)
				cr = pEdit->m_crQuote;
			else if (iLast & OE_KEYWORD)
				{
				x += GetWidth(dc,szOutText,j);
				iLast = crTemplate[i];
				j = 0;
				// keep building our output string
				szOutText[j++] = pText[i];
				szOutText[j] = 0;
				continue;
				}
			else
				cr = pEdit->m_crText;

			dc->SetTextColor(cr);
			dc->TextOut( x, y, A2T(szOutText), iOutLength );
			x += GetWidth(dc,szOutText,j);
			iLast = crTemplate[i];
			j = 0;
			}

		// keep building our output string
		szOutText[j++] = pText[i];
		szOutText[j] = 0;
		}

	// do the final piece
	if (szOutText[0])
		{
		iOutLength = min(j,iOutMax);
		if (iLast & OE_COMMENT)
			cr = pEdit->m_crComment;
		else if (iLast & OE_QUOTE)
			cr = pEdit->m_crQuote;
		else if (iLast & OE_KEYWORD)
			cr = pEdit->m_crKeyWord;
		else
			cr = pEdit->m_crText;

		dc->SetTextColor(cr);
		dc->TextOut( x, y, A2T(szOutText), iOutLength );
		}

	// reset to default text color
	dc->SetTextColor(pEdit->m_crText);

	return TRUE;
}

template <class BC>
BOOL SECEditCore<BC>::InBlockEMptyLine(PLINEDESC pstTempLine)
{
	bool bFound = false;
	if (m_BlockStart.pLine)
	{
		PLINEDESC pLine = m_BlockStart.pLine;
		while (pLine && pLine != m_BlockEnd.pLine)
		{
			if (pLine == pstTempLine)
			{
				bFound = true;
				break;
			}
			pLine = pLine->pNext;
		}
	}
	return bFound;
}

#endif //__VIEWPAINT_INL__