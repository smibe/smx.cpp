// SongTedView.cpp : implementation of the CSongTedView class
//

#include "stdafx.h"
#include "SongTed.h"
#include <atlconv.h>

#include "Util.h"
#include "SmxRes.h"
#include "SongTedDoc.h"
#include "SongTedView.h"
#include "SmxPgSetupDlg.h"
#include "XMLDoc.h"
#include "SelectTagDlg.h"
#include "SongTedOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//XMLTags
TCHAR szSong[]   = _T("song");
TCHAR szTitle[]   = _T("title");
TCHAR szAuthor[]  = _T("author");
TCHAR szMusic[]  = _T("music");
TCHAR szLyrics[]  = _T("lyrics");
TCHAR szStave[]   = _T("stave");
TCHAR szVers[]   = _T("vers");
TCHAR szCord[]   = _T("cord");
TCHAR szRefrain[] = _T("refrain");
TCHAR szRefrainRef[] = _T("refrain_ref");

IMPLEMENT_DYNCREATE(CSongTedView, CSmxPrnFormView)

BEGIN_MESSAGE_MAP(CSongTedView, CSmxPrnFormView)
	//{{AFX_MSG_MAP(CSongTedView)
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_AUTHOR, OnEditAuthor)
	ON_COMMAND(ID_EDIT_CONVERT_CHORDS, OnEditInsertChords)
	ON_COMMAND(ID_EDIT_INSERT_TAG, OnEditInsertTag)
	ON_COMMAND(ID_EDIT_INSERT_CHORD, OnEditInsertChord)
	ON_COMMAND(ID_EDIT_CREATE_MASTER, OnEditCreateMaster)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_PREVIEW, OnPreview)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CSmxPrnFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CSmxPrnFormView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSongTedView construction/destruction

CSongTedView::CSongTedView()
: CSmxPrnFormView(IDD_SongTed)
{
  m_pXMLDoc = NULL;
  m_pMasterStave = NULL;

  m_nNofColumns  = 1;
  m_nBetweenCols = 4000;
  m_nStartY      = 0;
  m_nFirstCol    = 1;
  m_bPreview = false;
}

void CSongTedView::DoDataExchange(CDataExchange* pDX)
{
	CSmxPrnFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilDiffView)
  //DDX_Control(pDX, IDC_ebText, m_SongTed);
	//}}AFX_DATA_MAP

}

CSongTedView::~CSongTedView()
{
  delete m_pXMLDoc;

  delete m_pMasterStave;
}

BOOL CSongTedView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CSmxPrnFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSongTedView drawing

void CSongTedView::OnDraw(CDC* pDC)
{
  CSmxPrnFormView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CSongTedView diagnostics

#ifdef _DEBUG
void CSongTedView::AssertValid() const
{
	CSmxPrnFormView::AssertValid();
}

void CSongTedView::Dump(CDumpContext& dc) const
{
	CSmxPrnFormView::Dump(dc);
}

CSongTedDoc* CSongTedView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSongTedDoc)));
	return (CSongTedDoc*)m_pDocument;
}
#endif //_DEBUG

void CSongTedView::OnFilePrint()
{
  CSmxPrnFormView::OnFilePrint();
}
/////////////////////////////////////////////////////////////////////////////
// CSongTedView message handlers

void CSongTedView::OnSize(UINT nType, int cx, int cy) 
{
	CSmxPrnFormView::OnSize(nType, cx, cy);
  
  if (!GetDlgItem(IDC_ebText)->GetSafeHwnd())
    return;
  CRect rcClient;
  CRect rcEdit;
  GetClientRect(rcClient);
  GetDlgItem(IDC_ebText)->GetWindowRect(rcEdit);
  ScreenToClient(rcEdit);
  rcEdit.right = rcClient.right - rcEdit.left;
  rcEdit.bottom = rcClient.bottom - rcEdit.top;
  GetDlgItem(IDC_ebText)->MoveWindow(rcEdit);


}

void CSongTedView::OnInitialUpdate() 
{
    CSmxPrnFormView::OnInitialUpdate();
	
	if (!m_SongTed.GetSafeHwnd())
  {
    m_SongTed.Initialize(IDC_ebText, this);
    GetDocument()->m_pEdit = m_SongTed.GetEdit();
    if (!GetDocument()->GetPathName().IsEmpty())
    {
      m_SongTed.GetEdit()->ReadFile(GetDocument()->GetPathName());
    }
  }

  m_SongTed.InitView();
  if (m_bPreview)
  {
  	m_SongTed.ShowWindow(SW_HIDE);
  }
  else
  {
	m_SongTed.Invalidate();
  }
	
}

void CSongTedView::Draw()
{
  int nColumn = m_nFirstCol;

  if (m_bCalcNofPages || m_pXMLDoc == NULL)
  {
    delete m_pXMLDoc;
    m_pXMLDoc = new CXMLDoc();

    CString str;
    GetText(str);
    //str = "<xx>xx</xx>";

    if (!str.IsEmpty())
    {
      if (!m_pXMLDoc->LoadXML(str))
      {
        AfxMessageBox(m_pXMLDoc->GetError());
        delete m_pXMLDoc;
        m_pXMLDoc = NULL;
        return;
      }
    }

  }
  else
  {
    //Set the print rect
    CRect rcPrint = m_rcClient;
    int nColWidth = (m_rcClient.Width() - (m_nNofColumns - 1) * m_nBetweenCols) / m_nNofColumns;
    rcPrint.left = m_rcClient.left + (nColumn -1) * (nColWidth + m_nBetweenCols);
    SetPrintRect(rcPrint);
    SetRectTopMargin(0);
  }

  if (!m_pXMLDoc)
    return;

  CXMLElement *pRoot = m_pXMLDoc->GetRoot();

  if (!pRoot)
    return;

  CXMLElement *pElt  = pRoot->GetChild(szTitle);

  if (m_bCalcNofPages || m_nCurPage == 1)
  {
    SetFontName("Times New Roman");
    SetFontSize(GetOptions()->fontSizeTitle);

    if (!pRoot)
      return;

    PrintXMLText(pElt, DT_LEFT);

    SetFontSize(GetOptions()->fontSizeInfo);
    PrintEOL();

    pElt = pRoot->GetChild(szAuthor);
    if (pElt)
    {
      PrintXMLText(pElt, DT_LEFT);
      PrintEOL();
    }

    int nTab = 0;
    pElt = pRoot->GetChild(szLyrics);
    if (pElt)
    {
      int nX = m_nX;
      PrintText(LoadString(IDS_Lyrics));
      nTab = m_nX + 100;
      if (pRoot->GetChild(szMusic))
      {
        nTab = max(nTab, nX + m_pDC->GetTextExtent(LoadString(IDS_Music)).cx + 100);
      }
      MoveTo(nTab, m_nY);
      PrintXMLText(pElt, DT_LEFT | DT_SINGLELINE);
      PrintEOL();
    }

    pElt = pRoot->GetChild(szMusic);
    if (pElt)
    {
      PrintText(LoadString(IDS_Music));
      if (!nTab)
        nTab = m_nX + 100;
      MoveTo(nTab, m_nY);
      PrintXMLText(pElt, DT_LEFT | DT_SINGLELINE);
    }
  }

  pElt = pRoot->GetChild();
  while (pElt)
  {
    if (pElt->GetTag() == szStave || pElt->GetTag() == szRefrain || pElt->GetTag() == szRefrainRef)
      break;

    pElt = pElt->GetNext();
  }

  if (pElt == NULL)
    return;

  SetFontSize(GetOptions()->fontSizeChords);
  int nCordY = CalcPrintRectHeight(1);
  SetFontSize(GetOptions()->fontSizeText);

  int nX = m_nX + 500;
  int nCount = 0;
  int nIdx    = 0;
  bool bNewPageOrColumn = false;

  m_nStartY = m_nY;
  if (m_bCalcNofPages || m_bPreview)
  {
    m_aPages.RemoveAll();
    m_aPages.Add(pElt);
  }
  else
  {
    nIdx = (m_nCurPage - 1) * m_nNofColumns + nColumn - (m_nFirstCol - 1)  - 1;
    pElt = (CXMLElement *) m_aPages[nIdx];
    if (m_nCurPage > 1)
    {
      //Count the staves up to here
      CXMLElement *pStave = pRoot->GetChild();
      while (pStave && pStave != pElt)
      {
        if (pStave->GetTag() == szStave)
          nCount++;
        
        pStave = pStave->GetNext(szStave);
      }
    }
  }

  while(pElt)
  {
    PrintEOL();

    if (pElt->GetTag() == szRefrainRef)
    {
      CString strText = pElt->GetCompleteText();
      if (strText.IsEmpty())
        strText = LoadString(IDS_RefrainRef);
      
      PrintText(strText, DT_LEFT | DT_SINGLELINE);
      PrintEOL();
    }
    else
    {
      PrintStave(pElt, nCordY, nCount, bNewPageOrColumn);
    }
    

    if (!bNewPageOrColumn)
    {
      if (pElt && pElt->GetTag() == szStave)
        nCount++;

      pElt = pElt->GetNext();
    }
    else
    {
      //This is a new page or new column
      if (m_nNofColumns <= nColumn)
      {
        m_nNofPages++;
        nColumn = m_nFirstCol;
      }
      else
      {
        nColumn++;
      }
      
      nIdx = (m_nNofPages - 1) * m_nNofColumns + nColumn - (m_nFirstCol - 1)  - 1;
      m_aPages.Add(pElt);
    }

    
    if (!m_bCalcNofPages)
    {
      nIdx = (m_nCurPage - 1) * m_nNofColumns +  nColumn - (m_nFirstCol - 1);
      if (nIdx < m_aPages.GetSize() && pElt == m_aPages[nIdx])
      {
        if (nColumn < m_nNofColumns)
        {
          nColumn++;
          CRect rcPrint = m_rcClient;
          rcPrint.top = m_nStartY;
          int nColWidth = (m_rcClient.Width() - (m_nNofColumns - 1) * m_nBetweenCols) / m_nNofColumns;
          rcPrint.left = m_rcClient.left + (nColumn -1) * (nColWidth + m_nBetweenCols);
          SetPrintRect(rcPrint);
          SetRectTopMargin(0);
        }
        else
        {
          //new page
          break;
        }
      }
    }

    bNewPageOrColumn = false;

  }

}


void CSongTedView::OnPageSetup()
{
	CSmxPgSetupDlg dlg(0, this);
  dlg.m_eHeader = SWC_hidden;
  dlg.m_eFooter = SWC_hidden;
  dlg.m_eColumns = SWC_enabled;
  dlg.m_strRegKey = GetPageSetupSection();

  AfxGetApp()->DoPrintDialog(&dlg);
}

bool CSongTedView::GetText(CString& str, bool bSelection)
{
  SECEdit *pEdit = m_SongTed.GetEdit();
  CString strTemp;
  static bool bStave = false;

  TEXTPOS Start, End;

  PLINEDESC pLine = pEdit->m_pLine;
  if (bSelection)
  {
    m_SongTed.GetSelection(Start, End);
    pLine = Start.pLine;
  }

  while (pLine)
  {
    strTemp = pLine->pText;
    if (strTemp.Find("<stave>") != -1 || strTemp.Find("<refrain>") != -1)
      bStave = true;
    
    if (strTemp.Find("</stave>") != -1 || strTemp.Find("</refrain>") != -1)
      bStave = false;

    if (bStave)
    {
      int nIdx = strTemp.Find('<');
      if (nIdx == -1)
      {
        strTemp = "<vers>";
        strTemp += pLine->pText;
      }

      while(strTemp.Replace("[", "<cord>"))
        ;
      while(strTemp.Replace("]", "</cord>"))
        ;

      if (nIdx == -1)
      {
        strTemp += "</vers>";
      }
    }

    str += strTemp;
    if (bSelection && pLine == End.pLine)
      break;

    pLine = pLine->pNext;
  }
  return true;
}



void CSongTedView::OnEditAuthor() 
{
	m_SongTed.InsertTag("author");
	
}

BOOL CSongTedView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (nID == 32773)
  {
    int XXX = 1;
  }
  if (m_SongTed.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo) )
    return TRUE;
	
	return CSmxPrnFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CSongTedView::OnEditInsertChords() 
{
	m_SongTed.InsertChordsBetween();
	
}

void CSongTedView::OnEditInsertTag() 
{
	CSelectTagDlg dlg;
  if (dlg.DoModal() == IDOK)
  {
    m_SongTed.InsertTag(dlg.GetTag());
  }
}

void CSongTedView::PrintStave(CXMLElement *pElt, int nCordY, int nCount, bool& bNewPageOrColumn)
{
 CXMLElement *pVers = 0;
  CXMLElement *pFragment = 0;

  //Print the number
  int nX = m_nX + 500;

  MoveTo(m_nX, m_nY + nCordY);
  if (pElt->GetTag() == szRefrain)
  {
    PrintText(LoadString(IDS_Refrain));
    PrintText(":", DT_LEFT | DT_SINGLELINE);
    nX = m_nX + 200;
  }
  else
  {
    PrintValue(nCount + 1, DT_LEFT | DT_SINGLELINE, 0);
    PrintText(".", DT_LEFT | DT_SINGLELINE);
  }
  pVers = pElt->GetChild(szVers);
  int nCy = 0;
  if (GetOptions()->printChords)
  {
    for(pFragment = pVers->GetChild(); pFragment; pFragment = pFragment->GetNext())
    {
      if (pFragment->GetTag() == szCord)
      {
        nCy = nCordY;
        break;
      }
    }
  }
  MoveTo(m_nX, m_nY - nCy);
  while (pVers)
  {
    if (m_bCalcNofPages)
    {
      if (m_nY + 2 * nCordY > m_rcClient.bottom)
      {
        m_nY = m_rcClient.top;
        bNewPageOrColumn = true;
        break;
      }
    }

    nCy = 0;
    if (GetOptions()->printChords)
    {
      for(pFragment = pVers->GetChild(); pFragment; pFragment = pFragment->GetNext())
      {
        if (pFragment->GetTag() == szCord)
        {
          nCy = nCordY;
          break;
        }
      }
    }

    MoveTo(nX, m_nY + nCy);
    for(pFragment = pVers->GetChild(); pFragment; pFragment = pFragment->GetNext())
    {
      if (pFragment->GetTag() == szCord)
      {
        if (GetOptions()->printChords)
        {
          int nStart = m_nX;
          MoveTo(m_nX, m_nY - nCy);
          SetFontSize(GetOptions()->fontSizeChords);
          PrintXMLText(pFragment, DT_LEFT | DT_SINGLELINE);
          SetFontSize(GetOptions()->fontSizeText);
          MoveTo(nStart, m_nY + nCy);
        }
      }
      else
      {
        PrintXMLText(pFragment, DT_LEFT | DT_SINGLELINE);
      }
    }
  
    //PrintXMLText(pVers, DT_LEFT | DT_SINGLELINE);
    PrintEOL();
    pVers = pVers->GetNext();
  }
}

void CSongTedView::OnEditInsertChord() 
{
	if (!m_pMasterStave)
  {
    m_SongTed.InsertString(_T("[]"), TRUE, TRUE);
    return;
  }

	CXMLElement *pElt = m_pMasterStave->GetRoot();

  PLINEDESC pCurLine = m_SongTed.m_pCurLine;
  PLINEDESC pLine = pCurLine;
  int nCountIdx = -1;
  while (pLine && strstr(pLine->pText, pElt->GetTag()) == NULL)
  {
    nCountIdx++;
    pLine = pLine->pPrev;
  }

  if (!pLine)
  {
    AfxMessageBox("Can't insert chords with master if the <stave> tag is not complete!");
    return;
  }


  CString str1 = pElt->GetTag();
  bool bEq = (str1 == szStave);
  bEq = (str1 == szRefrain);
  if (pElt->GetTag() != szStave && pElt->GetTag() != szRefrain)
  {
    AfxMessageBox("The master should begin with a stave tag!");
    return;
  }

  pElt = pElt->GetChild(szVers);
  while (nCountIdx && pElt)
  {
    pElt = pElt->GetNext(szVers);
    nCountIdx--;
  }

  if (!pElt)
  {
    AfxMessageBox("Number of verses in master is smaller than current number of verses.!");
    return;      
  }

  //Count the chord number to insert
  TCHAR *pChar = pCurLine->pText;
  int nCol = m_SongTed.m_iColNo;
  int nCordNo = 0;
  while (pChar && *pChar && nCol > 0)
  {
    if (*pChar == ']')
      nCordNo++;
    pChar++;
    nCol--;
  }

  pElt = pElt->GetChild(szCord);
  while (pElt && nCordNo)
  {
    pElt = pElt->GetNext(szCord);
    nCordNo--;
  }

  if (!pElt)
  {
    AfxMessageBox("Chords are complete for this vers!");
    return;
  }

  CString str;
  str.Format(_T("[%s]"), pElt->GetCompleteText());
  m_SongTed.InsertString((LPTSTR)(LPCTSTR)str, TRUE, TRUE);
}

void CSongTedView::OnEditCreateMaster() 
{
	if (m_pMasterStave)
  {
    delete m_pMasterStave;
    m_pMasterStave = NULL;
  }

  CString str;
  GetText(str, true);
  if (!str.IsEmpty())
  {
    m_pMasterStave = new CXMLDoc;
    if (!m_pMasterStave->LoadXML(str))
    {
      AfxMessageBox(m_pMasterStave->GetError());
      delete m_pXMLDoc;
      m_pXMLDoc = NULL;
      return;
    }
  }
	
}

BOOL CSongTedView::OnEraseBkgnd(CDC* pDC) 
{
	if (m_bPreview)
  {
    CRect rcWindow;
    GetClientRect(rcWindow);
    pDC->FillSolidRect(rcWindow, GetSysColor(COLOR_WINDOW));
    return FALSE;
  }
	
	return CSmxPrnFormView::OnEraseBkgnd(pDC);
}

void CSongTedView::OnPreview() 
{
	m_bPreview = !m_bPreview;

  if (m_bPreview)
  {
    delete m_pXMLDoc;
    m_pXMLDoc = NULL;
  	m_SongTed.ShowWindow(SW_HIDE);
  }
  else
  {
    delete m_pXMLDoc;
    m_pXMLDoc = NULL;
  	m_SongTed.ShowWindow(SW_SHOW);
    m_SongTed.Invalidate();
  }
	
}

void CSongTedView::OnViewOptions() 
{
	GetOptions()->DoModal();
	
}
