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
// EditCore.cpp
//
//
// Description: Low level SECEdit routines
//

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "secedit.h"

/////////////////////////////////////////////////////////////////////
//
// SECEditAlloc
//
//
// Description: Global function that allocates and zeroes memory
//
// Returns:     pointer to allocated memory
//

LPVOID SECEditAlloc(int iSize)
   {
   LPVOID lp;

   lp = malloc(iSize);
   if (lp)
      memset(lp,0,iSize);
   return lp;
   }


//@doc SECEdit
//@mfunc SECEdit constructor - initializes the SECEdit document data.
SECEdit::SECEdit()
   {
	InitEdit();
   }

//@doc SECEdit
//@mfunc SECEdit destructor - frees all memory from the SECEdit.
SECEdit::~SECEdit()
   {
	// if this ASSERT fires it means you have made a call to SetUndoGroup(TRUE) 
	// without a corresponding call to SetUndoGroup(FALSE).  This will result
	// in Undo records being erroneously bundled together for UndoEdit().
	ASSERT(m_dwUndoTimestamp == 0);
	ClearEdit();

   }

//@doc SECEdit
//@mfunc Internal function. Initializes SECEdit data items
//@rdesc void 
void SECEdit::InitEdit()
	{
   lpNames = NULL;
   glpMyIniFileBuffer = NULL;
	lpScanBuffer = NULL;
	m_pLine = MakeNewLine(64);
   m_pUndoBuffer = (PUNDORECORD)SECEditAlloc(sizeof(UNDORECORD) * OE_MAXUNDO);
	SetModified(FALSE);
	SetReadOnly(FALSE);
	ClearFileHandle();
	InitUndo();
	m_iLinesInFile = 1;
	SetFileMappingKey();
	InitFileTypeSettings();
	CString entry;		entry.LoadString(IDS_OE_LINEENDTYPE);
	m_iLineEndType = AfxGetApp()->GetProfileInt(m_strSection,entry,OE_CRLF);
	SetLineEndType(m_iLineEndType);

	}


//@doc SECEdit
//@mfunc Internal function. Releases the linked list of textlines.
//@rdesc void 
void SECEdit::ClearEdit()
	{
	if (m_pLine)
		LineListFree(m_pLine);
	FreeUndoBuffers();
	free(m_pUndoBuffer);
	FreeTempBuffers();
	m_pLine = NULL;
	}


//@doc SECEdit
//@mfunc Internal function. Sets default values for editor settings from the 
//              Registry.  See also SetRegistryKey().
//@rdesc int 
int SECEdit::InitFileTypeSettings()
   {
	CString entry;

   InitChroma();   // re-read the chroma stuff

	entry.LoadString(IDS_OE_TABSIZE);
	m_iTabSize = AfxGetApp()->GetProfileInt(m_strSection,entry,3);

	entry.LoadString(IDS_OE_KEEPTABS);
	m_bKeepTabs = AfxGetApp()->GetProfileInt(m_strSection,entry,1);

	entry.LoadString(IDS_OE_SHOWTABS);
	m_bShowTabs = AfxGetApp()->GetProfileInt(m_strSection,entry,0);

	entry.LoadString(IDS_OE_FONTNAME);
	m_fontname = AfxGetApp()->GetProfileString(m_strSection,entry,_T("Courier New"));

	entry.LoadString(IDS_OE_FONTSIZE);
	m_fontsize = AfxGetApp()->GetProfileInt(m_strSection,entry,9);

	entry.LoadString(IDS_OE_FONTCHARSET);
	m_fontcharset = AfxGetApp()->GetProfileInt(m_strSection,entry,0);

	entry.LoadString(IDS_OE_FONTWEIGHT);
	m_fontweight = AfxGetApp()->GetProfileInt(m_strSection,entry,400);

	entry.LoadString(IDS_OE_FONTITALIC);
	m_fontitalic = AfxGetApp()->GetProfileInt(m_strSection,entry,0);

	entry.LoadString(IDS_OE_WORDWRAP);
	m_bWordWrap = AfxGetApp()->GetProfileInt(m_strSection,entry,0);

	entry.LoadString(IDS_OE_INSERT);
	m_bInsertMode = AfxGetApp()->GetProfileInt(m_strSection,entry,1);

	entry.LoadString(IDS_OE_LOCKFILES);
	m_bFileLocking = AfxGetApp()->GetProfileInt(m_strSection,entry,0);

	return 0;

	}

//@doc SECEdit
//@mfunc Gets the SECEdit modified status
//@rdesc BOOL bOn TRUE - modified, FALSE - not modified.
BOOL SECEdit::GetModified(void)
{
	return m_bChanged;
}

//@doc SECEdit
//@mfunc Sets the SECEdit modified status
//@parm BOOL bOn TRUE - modified, FALSE - not modified.
void SECEdit::SetModified(BOOL bOn)
{
	m_bChanged = bOn;
}

//@doc SECEdit
//@mfunc Internal function. Determines if pFirst is before pSecond in the
//              linked list.
//@rdesc BOOL 
//@parm PTEXTPOS pFirst
//@parm  PTEXTPOS pSecond
BOOL SECEdit::InOrder(PTEXTPOS pFirst, PTEXTPOS pSecond)
   {
   PLINEDESC pLine;
   if (pFirst->pLine == pSecond->pLine)
      {
      if (pFirst->iOffset <= pSecond->iOffset)
         return TRUE;
      }
   else
      {
      pLine = pFirst->pLine->pNext;
      while (pLine)
         {
         if (pLine == pSecond->pLine)
            return TRUE;
         pLine = pLine->pNext;
         }
      }
   return FALSE;
   }

//@doc SECEdit
//@mfunc Internal function that Replaces tab characters in lpText with the 
//              appropriate number of spaces.
//@rdesc LPTSTR  pointer to the expanded text
//@parm LPTSTR lpText
//@parm int iTextSize
//@parm  int iTabSize
//@parm  int iLineEndLength
//@parm  int *iReturnSize
LPSTR SECEdit::ExpandTabs(LPSTR lpText,int iTextSize, int iTabSize, int iLineEndLength, int *iReturnSize)
  {
	iTabSize; //UNUSED
   LPSTR lpWorkBuffer;
   LPSTR lpNext;
   LPSTR lpStart;
   LPSTR lpTemp;
   int iNewTextSize;
   int iColNo,iInsert;
   int iLength,iCount;
	ASSERT(lpText);

	iLength = iTextSize;

   /* quick check for tabs, if any */
   lpNext = (LPSTR)memchr(lpText,9,iTextSize);

   if (!lpNext) /* no tabs, no work */
      {
      lpStart = (LPSTR)SECEditAlloc(iTextSize+1);
      memcpy(lpStart,lpText,iTextSize);
      *iReturnSize = iTextSize;
      return lpStart;
      }

   /* jump through, counting tabs, to get the needed workbuffer size */
   iCount = 0;
   iLength -= (lpNext - lpText);
   lpTemp = lpNext;
   while (lpNext && *lpNext)
      {
      lpNext = (LPSTR)memchr(lpTemp,9,iLength);
      if (lpNext)
         {
         *lpNext++;
         iCount++;
         iLength -= (lpNext - lpTemp);
         lpTemp = lpNext;
         }
      }

   lpNext = lpText;
     
   iCount += 3;  // a little fudge
   lpStart = lpWorkBuffer = (LPSTR)SECEditAlloc(iTextSize+(iCount*m_iTabSize));

   iNewTextSize = iColNo = 0;

   while (iTextSize)
      {
      switch (*lpNext)
         {
         case 0x09:
            lpNext++;
            iTextSize--;
            iInsert = m_iTabSize - (iColNo % m_iTabSize);
            if (!iInsert)
               iInsert = m_iTabSize;

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

   *iReturnSize = iNewTextSize;
   return lpStart;
   }
   
//@doc SECEdit
//@mfunc Internal function. Returns the actual text offset of a tabbed position
//@rdesc int The actual offset of the tabbed position
//@parm PLINEDESC pLine
//@parm  int iPos
int SECEdit::GetRealOffset(PLINEDESC pLine, int iPos)
   {
	LPSTR lpText;
	int i;       // the real offset count
	int iColNo;  // the apparent offset count
   int iAdd;
	ASSERT(pLine);

	i = iColNo = 0;
	lpText = pLine->pText;
   if (!lpText)
      return iPos;
	while (iColNo <= iPos)
      {
      if (*lpText)
         {
         if (*lpText == 0x09)
            {
            iAdd = m_iTabSize - (iColNo % m_iTabSize);
            if (!iAdd)
               iAdd = m_iTabSize;
            }
         else
            iAdd = 1;
         *lpText++;
         }
      else
         iAdd = 1;
      iColNo += iAdd;
      if (iColNo <= iPos)
         i++;
      }
   return i;
	}

//@doc SECEdit
//@mfunc Internal function. Returns the expanded text offset of an actual position
//@rdesc int The expanded text offset (e.g. the column position)
//@parm PLINEDESC pLine
//@parm  int iPos
int SECEdit::GetTabbedOffset(PLINEDESC pLine, int iPos)
   {
	LPSTR lpText;
	int i;       // the real offset count
	int iColNo;  // the apparent offset count
   int iAdd;
	ASSERT(pLine);

	i = iColNo = 0;
	lpText = pLine->pText;
   if (!lpText)
      return iPos;
	while (i < iPos)
      {
      if (*lpText)
         {
         if (*lpText == 0x09)
            {
            iAdd = m_iTabSize - (iColNo % m_iTabSize);
            if (!iAdd)
               iAdd = m_iTabSize;
            }
         else
            iAdd = 1;
         lpText++;
         }
      else
         iAdd = 1;
      iColNo += iAdd;
      i++;
      }
   return iColNo;
	}


//@doc SECEdit
//@mfunc Sets the SECEdit readonly status
//@rdesc void 
//@parm BOOL bOn TRUE - read only, FALSE - not read only.

void SECEdit::SetReadOnly(BOOL bOn)
   {
   m_bReadOnly = bOn;
   }


//@doc SECEdit
//@mfunc Gets the SECEdit readonly status
//@rdesc BOOL TRUE - read only, FALSE - not read only.
//@parm void
BOOL SECEdit::GetReadOnly(void)
   {
	return m_bReadOnly;
	}

//@doc SECEdit
//@mfunc Sets the SECEdit file locking status
//@rdesc void 
//@parm BOOL bOn TRUE - File locking is on, FALSE - File locking is off.

void SECEdit::SetFileLocking(BOOL bOn)
   {
	m_bFileLocking = bOn;
   }


//@doc SECEdit
//@mfunc Internal member function. - Clears the m_hFile member
//@rdesc void 
//@parm void
void SECEdit::ClearFileHandle(void)
   {
	m_hFile = NULL;
	}

//@doc SECEdit
//@mfunc Gets the SECEdit file locking status.
//@rdesc BOOL  TRUE - File locking is turned on. FALSE - file locking is turned off.
//@parm void
BOOL SECEdit::GetFileLocking(void)
   {
	return m_bFileLocking;
	}

//@doc SECEdit
//@mfunc Gets the current SECEdit tab size setting
//@rdesc int The tab size.
//@parm void
int SECEdit::GetTabSize(void)
   {
	return m_iTabSize;
	}

//@doc SECEdit
//@mfunc Sets the SECEdit tab size setting
//@rdesc void 
//@parm int iSize The new tab size.
void SECEdit::SetTabSize(int iSize)
   {
	m_iTabSize = iSize;
	}

// these are stubbed out, but are not implemented
void SECEdit::SetWordWrap(BOOL bOn)
{
	m_bWordWrap = bOn;
}

BOOL SECEdit::GetWordWrap(void)
{
	return m_bWordWrap;
}

void SECEdit::SetInsertMode(BOOL bOn)
{
	m_bInsertMode = bOn;
}

BOOL SECEdit::GetInsertMode(void)
{
	return m_bInsertMode;
}
// End stubs

//@doc SECEdit
//@mfunc Gets the SECEdit line end length.  Line ends can
//              be set to OE_CR, OE_LF, or OE_CR/OE_LF
//@rdesc int 
//@parm void
int SECEdit::GetLineEndLength(void)
   {
	return m_iLineEndLength;
	}
