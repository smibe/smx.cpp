// SongTedCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SongTed.h"
#include "SongTedCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSongTedCtrl

CSongTedCtrl::CSongTedCtrl()
{
  m_cStartChord = '[';
  m_cEndChord   = ']';
}

CSongTedCtrl::~CSongTedCtrl()
{
}


BEGIN_MESSAGE_MAP(CSongTedCtrl, SECEditCtrl)
	//{{AFX_MSG_MAP(CSongTedCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSongTedCtrl::InsertTag(LPCTSTR pszTag)
{
  CString str;
  TEXTPOS Start, End;
	GetSelection( Start, End);
  SetSelection(&Start, &Start);
  str.Format(_T("<%s>"), pszTag);
  InsertString(str.GetBuffer(1), TRUE, TRUE);
  str.ReleaseBuffer();

  if (Start.pLine == End.pLine)
    End.iOffset += str.GetLength();

  SetSelection(&End, &End);
  str.Format(_T("</%s>"), pszTag);
  InsertString(str.GetBuffer(1), TRUE, TRUE);
  str.ReleaseBuffer();

  SetSelection(&Start, &Start);
  ClearSelection(TRUE);
}

void CSongTedCtrl::InsertChordsBetween()
{
  TEXTPOS Start, End;
  TEXTPOS Pos;
  PLINEDESC pLine = NULL;
  PLINEDESC pNext = NULL;
  CString   str;
  CString   strChord;
  char *pChar = NULL;
  int   nCol = 0;
  int   nAdd = 0;

  GetSelection( Start, End);
  for (pLine = Start.pLine; pLine != End.pLine && pLine; pLine = pLine->pNext)
  {
    pNext = pLine->pNext;
    pChar = pLine->pText;
    nAdd = 0;
    while (pChar && *pChar)
    {
      if (*pChar != 0xd && *pChar != 0xa && *pChar != ' ')
      {
        str += *pChar;
      }
      else
      {
        if (!str.IsEmpty())
        {
          nCol = pChar - pLine->pText - str.GetLength() + nAdd;
          Pos.pLine = pNext;
          Pos.iOffset = nCol;
          strChord.Format(_T("%c%s%c"), m_cStartChord, str, m_cEndChord);
          SetSelection(&Pos, &Pos);
          InsertString(strChord.GetBuffer(1), TRUE, TRUE);
          strChord.ReleaseBuffer();
          nAdd += strChord.GetLength();
          str.Empty();
        }
      }
      pChar++;
    }

    pLine = pNext;
  }

  TEXTPOS StartCut, EndCut;
  for (pLine = Start.pLine; pLine != End.pLine && pLine; pLine = pLine->pNext)
  {
    StartCut.pLine = pLine; StartCut.iOffset = 0;
    if (pLine->pNext)
    {
      EndCut.pLine = pLine->pNext; 
      EndCut.iOffset = 0;
    }
    else
    {
      EndCut.pLine = pLine; 
      EndCut.iOffset = pLine->iTxtSize;
    }

    GetEdit()->Cut(&StartCut, &EndCut, TRUE );
  }

	CheckLines(GetEdit()->m_pLine);  // might have toasted m_pTopLine
	CountLines();

  SetSelection(&Start, &Start);
  ClearSelection(TRUE);
  MakeCursorVisible();
}

void CSongTedCtrl::UpdateChordsAbove()
{
}