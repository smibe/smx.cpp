/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffDirView.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffDirView.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "AfxPriv.h"
#include "fcu.h"
#include "FCUOptions.h"
#include "mainfrm.h"
#include "Util.h"
#include "resource.h"
#include "OXGridList.h"
#include "DiffDirDoc.h"
#include "DiffDirView.h"
#include "CopyFilesDlg.h"
#include "SelectFilesDlg.h"
#include "FilDiffView.h"
#include "CShellFileOp.h"
#include "VarMgr.h"
#include "FilDiffParam.h"
#include "Popup.h"
#include "PxshlApi.h"
#include "SmxPrnTable.h"
#include "SmxRes.h"
#include "SmxPgSetupDlg.h"


UINT gl_AccelGroups[] = 
{ 
  IDS_File, 1, 
    ID_CMP_DIR, ID_CMP_FILE, ID_FILE_SAVE_DIFF_LIST, ID_FILE_PRINT, ID_FILE_PRINT_PREVIEW,
    ID_FILE_PAGE_SETUP, 
    0,
  IDS_Edit, 1,
    ID_BREAK,
    ID_EDIT_COMMENT,
    0,
  IDS_View, 1, 
    ID_VIEW_REFRESH, ID_VIEW_TOOLBAR, ID_VIEW_STATUS_BAR, ID_VIEW_OPTIONS,
    0,
  IDS_Tools, 1, 
    ID_TOOLS_COPYFILES, ID_TOOLS_CUSTOMIZE_TOOLBAR, 
    ID_TOOLS_CUSTOMIZE_KEYBOARD, ID_TOOLS_CUSTOMIZE_USERTOOLS,
    0,
  IDS_Windows, 1,
    ID_WINDOWS_NEXT,
    0,
};


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void GetFileName(CString& strFilePath);
void GetFileDir(CString& strFilePath);

extern LPCTSTR szInitPath;

#define MYPERCENT_DBL 1000.0
#define MYPERCENT     1000

#define COL_NR       0
#define COL_FILE     1
#define COL_STATUS   2
#define COL_LDATE    3
#define COL_LSIZE    4
#define COL_RDATE    5
#define COL_RSIZE    6
#define COL_COMMENT  7
#define NOF_COLS     8


LPCTSTR szProperties = _T("properties");

namespace 
{
  LPCTSTR szColWidth       = _T("ColWidth");
  LPCTSTR szDiffDirPgSetup = _T("DiffDirPgSetup");

  //Shell command
  LPCTSTR szDelete     = _T("delete");
  LPCTSTR szCopy       = _T("copy");
  LPCTSTR szOpen       = _T("open");
  LPCTSTR aColHeaders[]  = 
  {
	  _T("No."),
    _T("File"),
	  _T("Status"),
	  _T("L. Date"),
	  _T("L. Size"),
	  _T("R. Date"),
	  _T("R. Size"),
	  _T("Comment"),
  };
}

UINT aDefaultColWidth[] = { 2, 8, 4, 4, 3, 4, 3, 7 };
UINT TOTAL_COL_WIDTH = 2 + 3 * 4 + 8 + 7 + 2 * 3;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDiffDirView 
CAcceleratorManager* CDiffDirFrame::m_pAccelMgr = NULL;


CDiffDirView::CDiffDirView(CWnd* pParent /*=NULL*/)
	: CSmxPrnFormView(CDiffDirView::IDD),
	m_bInitialized(FALSE)
{
	UNREFERENCED_PARAMETER(pParent);

  m_pColSizes = NULL;
  
  //{{AFX_DATA_INIT(CDiffDirView)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}

CDiffDirView::~CDiffDirView()
{
  delete [] m_pColSizes;

}

IMPLEMENT_DYNCREATE(CDiffDirView, CFormView)

void CDiffDirView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
  

  //{{AFX_DATA_MAP(CDiffDirView)
	DDX_Control(pDX, IDC_DIFF_LIST, m_diffList);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDiffDirView, CFormView)
	//{{AFX_MSG_MAP(CDiffDirView)
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_DIFF_LIST, OnDblclkDiffList)
	ON_WM_KEYUP()
	ON_NOTIFY(NM_RETURN, IDC_DIFF_LIST, OnReturnDiffList)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_SAVE_DIFF_LIST, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_DIFF_LIST, OnUpdateFileSaveAs)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TOOLS_COPYFILES, OnToolsCopyfiles)
	ON_COMMAND(ID_TOOLS_SELECTFILES, OnToolsSelectFiles)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_DIFF_LIST, OnTrackHeader)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES_SELECTED_L, OnProperties_L)
	ON_COMMAND(ID_PROPERTIES_SELECTED_R, OnProperties_R)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SELECTED_L, OnUpdateCommand_L)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SELECTED_R, OnUpdateCommand_R)
	ON_UPDATE_COMMAND_UI(ID_COPY_SELECTED_L, OnUpdateOneDir_L)
	ON_UPDATE_COMMAND_UI(ID_COPY_SELECTED_R, OnUpdateOneDir_R)
	ON_COMMAND(ID_COPY_SELECTED_L, OnCopy_L)
	ON_COMMAND(ID_COPY_SELECTED_R, OnCopy_R)
	ON_COMMAND(ID_DELETE_SELECTED_L, OnDelete_L)
	ON_COMMAND(ID_DELETE_SELECTED_R, OnDelete_R)
	ON_COMMAND(ID_OPEN_SELECTED_L, OnOpen_L)
	ON_COMMAND(ID_OPEN_SELECTED_R, OnOpen_R)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_EDIT_COMMENT, OnEditComment)
	ON_COMMAND(ID_FILE_SAVE, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_OPEN_SELECTED_L, OnUpdateCommand_L)
	ON_UPDATE_COMMAND_UI(ID_OPEN_SELECTED_R, OnUpdateCommand_R)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES_SELECTED_L, OnUpdateOneDir_L)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES_SELECTED_R, OnUpdateOneDir_R)
	ON_COMMAND(ID_VIEW_COMPAREAGAIN, OnViewCompareagain)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CSmxPrnFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CSmxPrnFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CSmxPrnFormView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PAGE_SETUP, OnPageSetup)
END_MESSAGE_MAP()



IMPLEMENT_DYNCREATE(CDiffDirFrame, CMDIChildWndEx)

CDiffDirFrame::CDiffDirFrame()
{

}

CDiffDirFrame::~CDiffDirFrame()
{
}


BEGIN_MESSAGE_MAP(CDiffDirFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CDiffDirFrame)
	ON_WM_CLOSE()
  ON_WM_MDIACTIVATE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDiffDirView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if(m_bInitialized) 
  {
    ResizeControls();
  }
}

void CDiffDirView::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp) 
{
  CFormView::OnNcCalcSize(bCalcValidRects, lpncsp);

  //StoreColWidth();
}

  void CDiffDirView::OnInitialUpdate() 
{
	
	CFormView::OnInitialUpdate();

	m_diffList.LockWindowUpdate();

	m_diffList.SetSortable(TRUE);
	m_diffList.UseItemColors(true);
	m_diffList.SetGridLineOrientation(TRUE, TRUE);
	m_diffList.SetGridLines();
  m_diffList.SetMultipleSelection();

	// Add Headers to Grid Control
	m_diffList.InsertColumn(COL_NR, "No.");
	m_diffList.InsertColumn(COL_FILE,    _T("File"));
	m_diffList.InsertColumn(COL_STATUS,  _T("Status"));
	m_diffList.InsertColumn(COL_LDATE,   _T("L. Date"));
	m_diffList.InsertColumn(COL_LSIZE,   _T("L. Size"));
	m_diffList.InsertColumn(COL_RDATE,   _T("R. Date"));
	m_diffList.InsertColumn(COL_RSIZE,   _T("R. Size"));
	m_diffList.InsertColumn(COL_COMMENT, _T("Comment"));
	LVCOLUMN lvc;
  memset (&lvc, 0, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT;
  lvc.fmt  = LVCFMT_RIGHT;
  m_diffList.SetColType(COXGridList::t_number, COL_NR);
  m_diffList.SetColumn(COL_NR, &lvc);
  m_diffList.SetColType(COXGridList::t_date, COL_LDATE);
  m_diffList.SetColumn(COL_LDATE, &lvc);
  m_diffList.SetColType(COXGridList::t_date, COL_RDATE);
  m_diffList.SetColumn(COL_RDATE, &lvc);
	m_diffList.SetColType(COXGridList::t_double, COL_LSIZE);
  m_diffList.SetColumn(COL_LSIZE, &lvc);
	m_diffList.SetColType(COXGridList::t_double, COL_RSIZE);
  m_diffList.SetColumn(COL_RSIZE, &lvc);
	m_diffList.SetEditable(TRUE, COL_COMMENT);

	WINDOWPLACEMENT *pWplmt;
	int len;
	AfxGetApp()->GetProfileBinary("Init", "LayoutDir",(BYTE **) &pWplmt, 
		                          (UINT *) &len);
	if(len == sizeof(WINDOWPLACEMENT))
	{
		GetParent()->SetWindowPlacement(pWplmt);
		delete pWplmt;
	}

  if (m_pColSizes == NULL)
  {
    UINT nNofColSizes = 0;
    AfxGetApp()->GetProfileBinary(GetOptions().GetSection(), szColWidth,(BYTE **) &m_pColSizes, (UINT *) &nNofColSizes);
    nNofColSizes /= sizeof(UINT);
    if (m_pColSizes == NULL || nNofColSizes < NOF_COLS)
    {
      UINT *pPrev = m_pColSizes; 
      m_pColSizes = new UINT [NOF_COLS];
      memset(m_pColSizes, 0, NOF_COLS * sizeof(int));
      
      if (pPrev)
      {
        for (UINT i = 0; i < nNofColSizes; i++)
          m_pColSizes[i] = pPrev[i];
        delete pPrev;
      }
    }
  }

  ResizeControls();

	CDiffDirDoc *doc = (CDiffDirDoc *) GetDocument();

	//InvalidateRect(rect);
	m_diffList.UnlockWindowUpdate();

	SetWindowText(doc->GetTitle());

  m_nPaperLeftMargin = 1000;
  m_nPaperRightMargin = 2000;
  m_nPaperTopMargin = 2000;
  m_nPaperBottomMargin = 2000;

	m_bInitialized = TRUE;
	OnUpdate(NULL, NULL, NULL);
}
	

void CDiffDirView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	UNREFERENCED_PARAMETER(lHint);
	UNREFERENCED_PARAMETER(pHint);
	UNREFERENCED_PARAMETER(pSender);
  if(!m_bInitialized)
		return;

	bool bHaveScrollBar = IsVisibleVScroll();
  m_diffList.DeleteAllItems();
	CDiffDirDoc *doc = (CDiffDirDoc *) GetDocument();
	
	m_diffList.LockWindowUpdate();
	////////   entries. 'lParam' points
	// to the data to be displayed in each column.
	////////
	LV_ITEM lvi;
	lvi.mask       = LVIF_TEXT;
	lvi.cchTextMax = 0;
	lvi.lParam     = 0;
	
	int iSubItem, j;
	TCHAR buf[20];
	// Insert the item
	j=0;
	for (int i=0; i < doc->ItemCount(); i++) {
		iSubItem = 0;
		
		lvi.iItem      = j;
		lvi.iSubItem   = 0;
		lvi.iImage	  = 0;
		
		wsprintf(buf, _T("%i "),j + 1);
		lvi.pszText = buf;
		
		if(doc->GetHide(i))
			continue;

		int nRtnValue = m_diffList.InsertItem(&lvi);
		ASSERT(-1 != nRtnValue);
		
		// Note that we're starting from iSubItem '1'
		m_diffList.SetItemText(j, ++iSubItem, doc->GetFileName(i));

		CString status = doc->GetStatus(i);
		if(status.Left(5) == "right")
			m_diffList.SetItemTextColor(j, COXGridList::blue);
		else if (status.Left(4) == "left")
			m_diffList.SetItemTextColor(j, COXGridList::red);

    m_diffList.SetItemText(j, ++iSubItem, status);
		m_diffList.SetItemText(j, ++iSubItem, doc->GetLeftDate(i));
		m_diffList.SetItemText(j, ++iSubItem, doc->GetLeftSize(i));
		m_diffList.SetItemText(j, ++iSubItem, doc->GetRightDate(i));
		m_diffList.SetItemText(j, ++iSubItem, doc->GetRightSize(i));
		
		//Remember the index from the document.
		m_diffList.SetItemData(j, i);		                        
		j++;
	}
	
	ECmpDirOption eChangedOption =  (ECmpDirOption) (OCD_FilSize | OCD_FilDate | 
                                                   OCD_FilTime  | OCD_FilSeconds |
                                                   OCD_Comment);

  if (bHaveScrollBar != IsVisibleVScroll() || GetOptions().HasChanged_Option(eChangedOption))
  {
    ResizeControls();
  }

  m_diffList.UnlockWindowUpdate();

}


void CDiffDirView::OnTrackHeader(NMHDR* pNMHDR, LRESULT* pResult) 
{
	StoreColWidth();
  *pResult = 0;
}

void CDiffDirView::OnDblclkDiffList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nRow = m_diffList.GetCurSel();
	CString fileName = m_diffList.GetItemText(nRow, 1);
	
  UNREFERENCED_PARAMETER(pNMHDR);
  if (nRow != -1)
	{
		int nIdx = m_diffList.GetItemData(nRow);
		CDiffRec *pRec = ((CDiffDirDoc *) GetDocument())->GetRecAt(nIdx);
		CString strMsg;
		if (pRec->m_eDiffType == diff_LOnly)
		{
			if (AfxMessageBox("The right file does not exist! You still want to open the file compare view?", MB_YESNO | MB_DEFBUTTON2) == IDNO)
				return;
		}

		if (pRec->m_eDiffType == diff_ROnly)
		{
			if (AfxMessageBox("The left file does not exist! You still want to open the file compare view?", MB_YESNO | MB_DEFBUTTON2) == IDNO)
				return;
		}

		CFilDiffParam Param;
    Param.SetRelDir(GetDirFromPath(pRec->m_strFileName));
    Param.SetFile1(GetDiffDirDoc()->m_strDir1, pRec->m_strFileName);
    Param.SetFile2(GetDiffDirDoc()->m_strDir2, pRec->m_strFileName);
    AfxGetMainWnd()->SendMessage(WMU_CMPFILE, (WPARAM) &Param);
	}

	*pResult = 0;
	
}


void CDiffDirView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	MessageBox("Got Key Up in Dir View");
	
	CFormView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CDiffDirView::OnReturnDiffList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDblclkDiffList(pNMHDR, pResult);
}

void CDiffDirView::OnDestroy() 
{
	CFormView::OnDestroy();
	
	WINDOWPLACEMENT wplmt;
	GetParent()->GetWindowPlacement(&wplmt);
	AfxGetApp()->WriteProfileBinary("Init", "LayoutDir", (BYTE *) &wplmt, sizeof(WINDOWPLACEMENT));

  StoreColWidth();
  AfxGetApp()->WriteProfileBinary(GetOptions().GetSection(), szColWidth, (BYTE *) m_pColSizes, NOF_COLS * sizeof(UINT));
}

void CDiffDirView::OnFileSaveAs() 
{
	FILE *fp;
	int i;
	CDiffDirDoc *doc = (CDiffDirDoc *) GetDocument();
	CString fileName, msg;
	CFileDialog dlg(FALSE, NULL, "DiffList", OFN_HIDEREADONLY, "CSV (Comma delimited) (*.*)|*.*||");
	if(dlg.DoModal()==IDOK) 
	{
		fileName = dlg.GetPathName();
	}
	else
	{
		return;
	}

	fp = fopen(fileName, "w");
  if(fp == NULL) 
  {
		msg = "Can't open " + fileName;
		AfxMessageBox(msg, MB_ICONEXCLAMATION);
		return;
	}

	fprintf(fp, "%s\n", GetDocument()->GetTitle());
	char buf[256];
	LV_COLUMN lv;
	lv.mask = LVCF_TEXT;
	lv.pszText = (char *) &buf;
	lv.cchTextMax = 256;

	//Print the headings
	for(i = 0; i < NOF_COLS; i++) 
  {
    if (IsColHidden(i))
      continue;
		m_diffList.GetColumn(i, &lv);
		fprintf(fp, "%s", buf);
		if(i != NOF_COLS - 1)
			fprintf(fp, ";");
	}
	fprintf(fp, "\n");

	//Print the data
	for (i=0; i < doc->ItemCount(); i++) 
  {
    if(m_diffList.GetItemText(i, 2) == "")
			break;				//list seems to be empty here
		
    for(int j=0; j<NOF_COLS; j++) 
    {
      if (IsColHidden(j))
        continue;

			msg = m_diffList.GetItemText(i, j);
			fprintf(fp, "%s",msg);
			if(j != NOF_COLS - 1)
				fprintf(fp, ";");
		}
		fprintf(fp, "\n");
	}

	if(fp)
		fclose(fp);

}

void CDiffDirView::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
}

void CDiffDirView::OnClose() 
{
	CFormView::OnClose();
}

void CDiffDirFrame::OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd )
{
  CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
}

void CDiffDirFrame::OnClose() 
{
	CDiffDirView *wnd = (CDiffDirView *) GetActiveView();

	if(wnd->IsModified()) 
  {
		int result;
		result = AfxMessageBox("The file list was modified.\n"
			"You want to save the changes?",
			MB_YESNOCANCEL | MB_ICONQUESTION);
	
    switch(result)
		{
			case IDYES:
				wnd->OnFileSaveAs();
			case IDNO:
				break;
			default:
				//Do nothing
				return;
		}
	}

	CMDIChildWndEx::OnClose();
}

bool CDiffDirFrame::IsButtonInitialyHidden(UINT nCmdID)
{
  switch (nCmdID)
  {
    case ID_FILE_SAVE_DIFF_LIST:
    case ID_CMP_DIR:
    case ID_EDIT_CUT:
    case ID_EDIT_COPY:
    case ID_EDIT_PASTE:
    case -1:
      return false;
    default:
      ;
  }
  return true;
}

CAcceleratorManager *CDiffDirFrame::GetAccelMgr()
{
  if (m_pAccelMgr == NULL)
  {
    m_pAccelMgr = new CAcceleratorManager;
    m_pAccelMgr->m_strView = "Directory Compare";

    m_pAccelMgr->SetAccelGroups(gl_AccelGroups, sizeof(gl_AccelGroups) / sizeof(UINT) );
      
    if (!m_pAccelMgr->Load(HKEY_CURRENT_USER, szInitPath, _T("DiffDirAccel")))
    {
      m_pAccelMgr->CreateDefaultTable(IDR_DIFF_DIR);
    }
    else
    {
      m_pAccelMgr->SetAccelTableID(IDR_DIFF_DIR);
    }

    m_pAccelMgr->InitAccelGroups();
    GetUserToolMgr().UpdateAccelMgr(m_pAccelMgr);
  }

  return m_pAccelMgr;
}



bool CDiffDirView::IsModified()
{
	return(m_diffList.m_bModified != FALSE);
}

void CDiffDirView::ResizeControls()
{
    CRect rect;
		GetClientRect(rect);
		m_diffList.SetWindowPos(NULL, rect.left, rect.top, 
								 rect.Width(), rect.Height(), SWP_SHOWWINDOW);

    m_diffList.GetClientRect(rect);
		int nWidth = rect.Width();

    int nTotal = 0;
    int nAdded = 0;
    UINT nIdx   = 0;
    UINT i      = 0;
    UINT aToModify[NOF_COLS];
    UINT nLastNotHidden = NOF_COLS - 1;

    for (i = NOF_COLS - 1; i >= 0; i--)
    {
      if (!IsColHidden(i))
      {
        nLastNotHidden = i;
        break;
      }
    }

    int  nToModifyTotal = 0;
    for (i = 0; i < NOF_COLS; i++)
    {
      if (m_pColSizes[i] == NULL && !IsColHidden(i))
      {
        m_pColSizes[i] = (UINT) (MYPERCENT_DBL * aDefaultColWidth[i] / TOTAL_COL_WIDTH + 0.5);
        nAdded += m_pColSizes[i];
      }
      else if (m_pColSizes[i] && IsColHidden(i))
      {
        nAdded -= m_pColSizes[i];
        m_pColSizes[i] = 0;
      }
      else
      {
        nToModifyTotal += m_pColSizes[i];
        aToModify[nIdx++] = i;
      }

      /*
      if (i == nLastNotHidden)
      {
        if (nTotal + m_pColSizes[i] > MYPERCENT - 40 || nTotal + m_pColSizes[i] < MYPERCENT + 40)
          m_pColSizes[i] = max(0, MYPERCENT - nTotal);
        else
          m_pColSizes[i] = max((int) m_pColSizes[i], MYPERCENT - nTotal);
      }
      */

      nTotal += m_pColSizes[i];
    }

    int nLeft = 0;
    if (nTotal - nAdded < MYPERCENT + 40)
      nLeft = MYPERCENT - nTotal + nAdded;

    if (nToModifyTotal && (nAdded || nLeft))
    {
      //Split up the added space
      int nModified = 0;
      int nDiff;
      for (UINT i = 0; i < nIdx; i++)
      {
        nDiff = -nLeft * (int) m_pColSizes[aToModify[i]] / (int) nToModifyTotal;
        nDiff += nAdded * (int) m_pColSizes[aToModify[i]] / (int) nToModifyTotal;
        m_pColSizes[aToModify[i]] -= nDiff;
        if (i == nLastNotHidden && nTotal - nAdded == MYPERCENT)
        {
          m_pColSizes[i] = max(0, MYPERCENT - nModified);
        }
      
      }
    }

		m_diffList.LockWindowUpdate();
    int nColWidth = 0;
    int nTotalWidth = 0;
    for (i = 0; i < NOF_COLS; i++)
    {
      nColWidth = (UINT) (nWidth * m_pColSizes[i] / MYPERCENT_DBL + 0.5);

      if (i == nLastNotHidden && nTotal < MYPERCENT + 40)
        nColWidth = nWidth - nTotalWidth;

      m_diffList.SetColumnWidth(i, nColWidth);
      nTotalWidth += nColWidth;
    }
		m_diffList.UnlockWindowUpdate();

}

void CDiffDirView::OnToolsCopyfiles() 
{
	CCopyFilesDlg dlg;

  dlg.SetDiffList(&m_diffList);
  dlg.SetDiffDoc((CDiffDirDoc *) GetDocument());
  UINT nSelected = m_diffList.GetSelectedCount();
	CDiffDirDoc *doc = (CDiffDirDoc *) GetDocument();
  bool *pSelected = new bool[doc->ItemCount()];
  ::ZeroMemory(pSelected, doc->ItemCount() * sizeof (bool));

	//Initialize the selected array
  int nItem = -1;
  int nIdx  = 0;
  while ((nItem = m_diffList.GetNextItem(nItem, LVNI_SELECTED)) != -1)
  {
    nIdx = m_diffList.GetItemData(nItem);
    if (nIdx < doc->ItemCount())
      pSelected[nIdx] = true;
    else
      ASSERT(false);
  }

  dlg.m_pSelected = pSelected;

  UINT nResult = dlg.DoModal();
  delete [] pSelected;

  if (nResult == IDOK)
  {
    //Start the copy operation
    CShellFileOp op;
    BOOL bSuccess, bAPICalled = FALSE, bAborted = FALSE;
    int  nAPIReturn = 0;

    op.SetOperationFlags ( FO_COPY, this, FOF_FILESONLY | FOF_NOCONFIRMMKDIR );

    if (dlg.m_strFrom.GetSize() != dlg.m_strTo.GetSize())
    {
      ASSERT(false);
      return;
    }

    for (int i = 0; i < dlg.m_strFrom.GetSize(); i++ )
    {
      op.AddSourceFile ( dlg.m_strFrom[i] );
      op.AddDestFile( dlg.m_strTo[i]);
    }

    bSuccess = op.Go ( &bAPICalled, &nAPIReturn, &bAborted );

  }
}

void CDiffDirView::OnToolsSelectFiles() 
{
	CSelectFilesDlg dlg;

  dlg.SetDiffList(&m_diffList);

  dlg.DoModal();
}

bool CDiffDirView::ExpandVar(const CString& strVar, CStringArray& strArray)
{
  ECmdVar      eVar;

  if (!CVarMgr::Lookup(strVar, eVar))
  {
    ASSERT(false);
    return false;
  }

  return ExpandVar(eVar, strArray);
}

bool CDiffDirView::ExpandVar(ECmdVar eVar, CStringArray& strArray, bool* pAllExistent)
{
	CDiffDirDoc *doc = (CDiffDirDoc *) GetDocument();

  if (pAllExistent)
    *pAllExistent = true;

  switch (eVar)
  {
    case var_RDir:
      strArray.SetSize(1);
      strArray[0] = doc->m_strDir2;
      return true;
    case var_LDir:
      strArray.SetSize(1);
      strArray[0] = doc->m_strDir1;
      return true;
    case var_LFilePath:
    case var_RFilePath:
    case var_LFileDir:
    case var_RFileDir:
    case var_LFileName:
    case var_RFileName:
    case var_Comment:
      {
        int nItem = -1;
        int nCount = 0;
        while ((nItem = m_diffList.GetNextItem(nItem, LVNI_SELECTED)) != -1)
        {
          nCount++;
        }
        strArray.SetSize(nCount);
        nItem = -1;
        int i = 0;
        CString strDir;
    
        if (eVar == var_LFilePath || eVar == var_LFileDir || eVar == var_LFileName)
          strDir = doc->m_strDir1;
        else if (eVar == var_RFilePath || eVar == var_RFileDir || eVar == var_RFileName)
          strDir = doc->m_strDir2;

        if (!strDir.IsEmpty() && strDir[strDir.GetLength() -1] != TCHAR('\\'))
          strDir += TCHAR('\\');

        EDiffType eDiffType = diff_Equal;
        while ((nItem = m_diffList.GetNextItem(nItem, LVNI_SELECTED)) != -1)
        {
          eDiffType = doc->GetRecAt(m_diffList.GetItemData(nItem))->m_eDiffType;
          {
            if (eVar == var_Comment)
              strArray[i] = m_diffList.GetItemText(nItem, COL_COMMENT);
            else
              strArray[i] = strDir + doc->GetFileName(m_diffList.GetItemData(nItem));

            if (eVar == var_LFileName || eVar == var_RFileName)
              GetFileName(strArray[i]);
            if (eVar == var_LFileDir || eVar == var_RFileDir)
              GetFileDir(strArray[i]);

            if (pAllExistent && *pAllExistent)
            {
              if (eVar == var_LFileName || eVar == var_LFileDir || eVar == var_LFilePath)
              {
                *pAllExistent = eDiffType != diff_ROnly;
              }
              else if (eVar == var_RFileName || eVar == var_RFileDir || eVar == var_RFilePath)
              {
                *pAllExistent = eDiffType != diff_LOnly;
              }
            }

            i++;
          }
        }
        return true;
      }
    default:
      ASSERT(false);
  }

  return false;
}

bool CDiffDirView::CanExpandVar(const CString& strVar)
{
  ECmdVar      eVar;

  if (!CVarMgr::Lookup(strVar, eVar))
  {
    TRACE("Invalid variable!\n");
    return false;
  }

  switch (eVar)
  {
    case var_RDir:
    case var_LDir:
    case var_LFilePath:
    case var_RFilePath:
    case var_LFileDir:
    case var_RFileDir:
    case var_LFileName:
    case var_RFileName:
    case var_Comment:
      return true;
  }

  return false;
}

CDiffDirDoc * CDiffDirView::GetDiffDirDoc()
{
  return (CDiffDirDoc *) GetDocument();
}

int CDiffDirFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CDiffDirFrame::DeleteAccelMgr()
{
  if (m_pAccelMgr)
  {
    m_pAccelMgr->Write();
    delete m_pAccelMgr;
  }
}

void CDiffDirView::Update()
{
  if (GetOptions().HasChanged_CmpDirOption())
  {
    if (GetOptions().HasChanged_Option(OCD_FilChangeNotify))
    {
      GetDiffDirDoc()->WatchDirectories();  
    }
    GetDiffDirDoc()->UpdateHide();
   	OnUpdate(NULL, NULL, NULL);
  }
}

bool CDiffDirView::IsColHidden(int nCol)
{
  switch (nCol)
  {
    case COL_RDATE:
    case COL_LDATE:
      return !GetOptions().GetOption(OCD_FilDate);

    case COL_RSIZE:
    case COL_LSIZE:
      return !GetOptions().GetOption(OCD_FilSize);
    case COL_COMMENT:
      return !GetOptions().GetOption(OCD_Comment);
  }
  return false;
}

void CDiffDirView::StoreColWidth()
{
  CRect rcClient;

  if (!m_pColSizes)
    return;

  if (m_diffList.m_hWnd == NULL)
    return;

  m_diffList.GetClientRect(rcClient);
  for (UINT i = 0; i < NOF_COLS; i++)
  {
    m_pColSizes[i] = (UINT) max(0, (m_diffList.GetColumnWidth(i) * MYPERCENT_DBL / rcClient.Width()));
  }
}

bool CDiffDirView::IsVisibleVScroll()
{
  CRect rcWindow;
  CRect rcClient;
  m_diffList.GetWindowRect(rcWindow);  
  m_diffList.GetClientRect(rcClient);  
  m_diffList.ClientToScreen(rcClient);
  return rcWindow.right - rcClient.right > GetSystemMetrics(SM_CXVSCROLL);
}

void CDiffDirView::OnContextMenu(CWnd* pWnd, CPoint point)
{
  /*
  CStringArray strArray;

  if (ExpandVar(var_LFilePath, strArray))
    TrackItemIDContextMenu((LPCTSTR) strArray[0], 0, &point, GetSafeHwnd());
  */

	PopupMenu Menu(IDR_DIRDIFF_POPUP,TRUE);
  Menu.TrackMenu(this, point, (UpdateMenuFkt) OnUpdatePopup);
}

void CDiffDirView::OnUpdateCommand_R(CCmdUI *pCmdUI)
{
  CStringArray strArray;
  bool         bAllExistent = true;

  pCmdUI->Enable(ExpandVar(var_RFilePath, strArray, &bAllExistent) && bAllExistent && strArray.GetSize() > 0);
}


void CDiffDirView::OnUpdateCommand_L(CCmdUI *pCmdUI)
{
  CStringArray strArray;
  bool         bAllExistent = true;

  pCmdUI->Enable(ExpandVar(var_LFilePath, strArray, &bAllExistent) && bAllExistent && strArray.GetSize() > 0);
}


void CDiffDirView::OnProperties_L()
{
  ExecuteShellCmd(szProperties, false, true);
}

void CDiffDirView::OnProperties_R()
{
  ExecuteShellCmd(szProperties, true, true);
}

void CDiffDirView::ExecuteShellCmd(LPCTSTR pszCmd, bool bRight, bool bRelative)
{
  CStringArray strArray;
  ECmdVar eVar = var_LFilePath;
  BOOL    bResult = true;

  if (bRight)
    eVar = var_RFilePath;

  if (ExpandVar(eVar, strArray))
  {
    if (stricmp(pszCmd, szOpen) == NULL)
    {
      for (int i = 0; i < strArray.GetSize(); i++)
      {
        ShellExecute(NULL, pszCmd, strArray[i], NULL,  NULL, SW_SHOW); 
      }
    }
    else if (stricmp(pszCmd, szDelete) == NULL)
    {
      CShellFileOp op;
      BOOL bOpStarted = false;
      int  nReturn    = TRUE;;
      for (int i = 0; i < strArray.GetSize(); i++)
      {
        op.AddSourceFile(strArray[i]);
      }
      op.SetOperationFlags(FO_DELETE, this, FOF_ALLOWUNDO);
      if (!op.Go(&bOpStarted, &nReturn))
      {
        int XXX = 1;
      }
    }
    else if (stricmp(pszCmd, szCopy) == 0)
    {
      bResult = ExecuteContextCommand(pszCmd, strArray, GetSafeHwnd());
      bResult = CopyToClipboard(strArray, GetSafeHwnd());
    }
    else if (stricmp(pszCmd, szProperties) == 0)
    {
      bResult = ExecuteContextCommand(pszCmd, strArray, GetSafeHwnd());
    }
  }
}

void CDiffDirView::OnCopy_L()
{
  ExecuteShellCmd(szCopy, false, false);
}

void CDiffDirView::OnCopy_R()
{
  ExecuteShellCmd(szCopy, true, false);  
}

void CDiffDirView::OnDelete_L()
{
  ExecuteShellCmd(szDelete, false, true);  
}

void CDiffDirView::OnDelete_R()
{
  ExecuteShellCmd(szDelete, true, true);  
}

void CDiffDirView::OnOpen_L()
{
  ExecuteShellCmd(szOpen, false, false);  
}

void CDiffDirView::OnOpen_R()
{
  ExecuteShellCmd(szOpen, true, false);  
}

void CDiffDirView::OnUpdateOneDir_R(CCmdUI *pCmdUI)
{
  CStringArray strArray;
  CString strDir;
  bool    bAllExistent = true;
  int nIdx;

  if (ExpandVar(var_RFilePath, strArray, &bAllExistent))
  {
    if (!bAllExistent)
    {
      pCmdUI->Enable(FALSE);
      return;
    }

    for (int i = 0; i < strArray.GetSize(); i++)
    {
      nIdx = strArray[i].ReverseFind('\\');
      if (nIdx == -1)
      {
        pCmdUI->Enable(FALSE);
        return;
      }
      
      if (strDir.IsEmpty())
        strDir = strArray[i].Left(nIdx);
      else if (strDir != strArray[i].Left(nIdx))
      {
        pCmdUI->Enable(FALSE);
        return;
      }
    }
     
  }
  pCmdUI->Enable(TRUE);
}


void CDiffDirView::OnUpdateOneDir_L(CCmdUI *pCmdUI)
{
  CStringArray strArray;
  CString strDir;
  int     nIdx = 0;
  bool    bAllExistent = true;

  if (ExpandVar(var_LFilePath, strArray, &bAllExistent))
  {
    if (!bAllExistent)
    {
      pCmdUI->Enable(FALSE);
      return;
    }

    for (int i = 0; i < strArray.GetSize(); i++)
    {
      nIdx = strArray[i].ReverseFind('\\');
      if (nIdx == -1)
      {
        pCmdUI->Enable(FALSE);
        return;
      }
      
      if (strDir.IsEmpty())
        strDir = strArray[i].Left(nIdx);
      else if (strDir != strArray[i].Left(nIdx))
      {
        pCmdUI->Enable(FALSE);
        return;
      }
    }
     
  }
  pCmdUI->Enable(TRUE);
}

void CDiffDirView::OnUpdatePopup(CMenu *pMenu)
{
  if (pMenu)
    GetUserToolMgr().UpdateUserToolMenu(pMenu);
}

void CDiffDirView::Draw()
{
  LOGFONT logfont;
  GetOptions().GetPrnFont(VIEW_DiffDir, &logfont, m_pDC);
  CSmxPrnFormView::SetFont(&logfont);

  COLORREF crLeft = GetSysColor(COLOR_WINDOWTEXT);
  COLORREF crRight = GetSysColor(COLOR_WINDOWTEXT);

  if (GetOptions().GetOption(OCF_PrintWithColors))
  {
    crLeft  = GetOptions().GetLeftPrnColor(VIEW_DiffDir);
    crRight = GetOptions().GetRightPrnColor(VIEW_DiffDir);
  }

  PrintText("FCU Directory Compare  ");
  PrintCurrentDate();
  PrintEOL();
  
  PrintText("Left directory:");
  int nY = m_nY;
  int nX = m_nX;
  PrintEOL();
  PrintText("Right directory:");
  nX = max(m_nX, nX);
  nX += 200;
  MoveTo(nX, nY);
  PrintText(GetDiffDirDoc()->m_strDir1);
  PrintEOL();
  MoveTo(nX, m_nY);
  PrintText(GetDiffDirDoc()->m_strDir2);
  PrintEOL();
  PrintEOL();

  CSmxPrnTable PrnTable(this);

  int  nCount = 0;
  UINT pColWidth[NOF_COLS];
  for (int i = 0; i < NOF_COLS; i++)
  {
    if(m_pColSizes[i] > 1)
    {
      pColWidth[nCount++] = (UINT) (m_pColSizes[i] * m_rcPrint.Width() / MYPERCENT_DBL);
    }


  }

  
  PrnTable.SetColumns(nCount, nCount - 1, pColWidth);
  //Print Header

  int nCol = 0;

  bool bColRight[NOF_COLS];
  LVCOLUMN lvc;
  memset (&lvc, 0, sizeof(LVCOLUMN));
	lvc.mask = LVCF_FMT;
  for (i = 0; i < NOF_COLS; i++)
  {
    m_diffList.GetColumn(i, &lvc);
    if (lvc.fmt & LVCFMT_RIGHT)
      bColRight[i] = true;
    else
      bColRight[i] = false;
  }

  for (i = 0; i < NOF_COLS; i++)
  {
    if(m_pColSizes[i] > 1)
    {
      if (bColRight[i])
        PrnTable.PrintText(0, nCol, aColHeaders[i], DT_RIGHT);
      else
        PrnTable.PrintText(0, nCol, aColHeaders[i]);
      nCol++;
    }
  }

  PrnTable.PrintFrame(FRM_TOP);
  PrnTable.PrintHLine(0);

  int nRowsPerPage = m_diffList.GetItemCount();
  int nFirstPrintRow = 0;

  if (m_bCalcNofPages)
  {
    m_aFirstRowsInPage.RemoveAll();
    m_aFirstRowsInPage.Add(0);
  }
  else
  {
    nFirstPrintRow = m_aFirstRowsInPage[m_nCurPage - 1];
    if (m_nCurPage == m_nNofPages)
      nRowsPerPage = m_diffList.GetItemCount() - m_aFirstRowsInPage[m_nCurPage - 1];
    else
      nRowsPerPage = m_aFirstRowsInPage[m_nCurPage] - m_aFirstRowsInPage[m_nCurPage - 1];
  }

  int nPrintRow = 1;

	CDiffDirDoc *pDoc = (CDiffDirDoc *) GetDocument();
  EDiffType eDiffType = diff_Equal;
  for (int nRow = nFirstPrintRow; nRow  < nFirstPrintRow + nRowsPerPage; nRow++)
  {
    eDiffType = pDoc->GetRecAt(m_diffList.GetItemData(nRow))->m_eDiffType;
    switch (eDiffType)
    {
      case diff_LNewer:
      case diff_LOnly:
        SetTextColor(crLeft);
        break;
      case diff_RNewer:
      case diff_ROnly:
        SetTextColor(crRight);
        break;
      default:
        SetTextColor(RGB(0, 0, 0));
        break;
    }

    nCol = 0;
    for (i = 0; i < NOF_COLS; i++)
    {
      if(m_pColSizes[i] > 1)
      {
        if (bColRight[i])
          PrnTable.PrintText(nPrintRow, nCol, m_diffList.GetItemText(nRow, i), DT_RIGHT);
        else
          PrnTable.PrintText(nPrintRow, nCol, m_diffList.GetItemText(nRow, i));
        nCol++;
      }
    }

    nPrintRow++;
    if (PrnTable.GetRowBottom(nPrintRow)  > int (m_rcClient.bottom - 2 * PrnTable.GetDefaultRowHeight()))
    {
      int x = PrnTable.GetRowBottom(nPrintRow) + m_nPaperTopMargin;
      int y = GetPaperHeight() - m_nPaperBottomMargin - 2 * PrnTable.GetDefaultRowHeight();
      if (m_bCalcNofPages)
      {
        nRow--;
        for (int i = 1; i < nPrintRow; i++)
        {
          PrnTable.SetRowHeight(i, PrnTable.GetDefaultRowHeight());
        }
        nPrintRow = 1;
        if (nRow < m_diffList.GetItemCount())
        {
          m_nNofPages++;
          m_aFirstRowsInPage.Add(nRow);
        }
      }
    }
  }

  SetPrintRect(NULL);
  PrintEOL();
  CString str;
  str.Format(_T("Page %d/%d"), m_nCurPage, m_nNofPages);
  MoveTo(m_nX, GetBottomPrintPos() - PrnTable.GetDefaultRowHeight());
  PrintText(str, DT_RIGHT);

}

void CDiffDirView::OnViewRefresh() 
{
  GetDiffDirDoc()->UpdateHide();
  OnUpdate(NULL, NULL, NULL);
  //((CDiffDirDoc *) GetDocument())->Refresh(false);
}

void CDiffDirView::UpdateRecord(int nIdx)
{
  int nPrevRow = -1;
  CDiffRec *pRec;
  int nSearchIdx;

  for (int nRow = 0; nRow < m_diffList.GetItemCount(); nRow++)
  {
    nSearchIdx = m_diffList.GetItemData(nRow);

    if (nSearchIdx < nIdx)
      nPrevRow = nRow;
    if (nSearchIdx == nIdx)
    {
      //Found the row, just need to refresh it
  		pRec = ((CDiffDirDoc *) GetDocument())->GetRecAt(nIdx);
      
      UpdateRec(pRec, nRow);
      return;
    }
  }

  //Could not find that record, if it's not hidden, show it after the prevRow
  pRec = ((CDiffDirDoc *) GetDocument())->GetRecAt(nIdx);
  if (!pRec->GetHide())
  {
    InsertRec(pRec, nPrevRow + 1, nIdx);
  }
}

void CDiffDirView::UpdateRec(CDiffRec *pRec, int nRow, bool bUpdateNow)
{
	int iSubItem = 0;	
  int i = m_diffList.GetItemData(nRow);

	CDiffDirDoc *doc = (CDiffDirDoc *) GetDocument();

  m_diffList.SetItemText(nRow, ++iSubItem, doc->GetFileName(i));

	CString status = doc->GetStatus(i);
	if(status.Left(5) == "right")
		m_diffList.SetItemTextColor(nRow, COXGridList::blue);
	else if (status.Left(4) == "left")
		m_diffList.SetItemTextColor(nRow, COXGridList::red);
  else
		m_diffList.SetItemTextColor(nRow, COXGridList::black);

  m_diffList.SetItemText(nRow, ++iSubItem, status);
	m_diffList.SetItemText(nRow, ++iSubItem, doc->GetLeftDate(i));
	m_diffList.SetItemText(nRow, ++iSubItem, doc->GetLeftSize(i));
	m_diffList.SetItemText(nRow, ++iSubItem, doc->GetRightDate(i));
	m_diffList.SetItemText(nRow, ++iSubItem, doc->GetRightSize(i));

  if (bUpdateNow)
  {
    CRect rcItem;
    m_diffList.GetItemRect(nRow, rcItem, LVIR_BOUNDS);
    m_diffList.InvalidateRect(rcItem);
  }

}

void CDiffDirView::InsertRec(CDiffRec *pRec, int nRow, int nIdx)
{
	LV_ITEM lvi;
	lvi.mask       = LVIF_TEXT;
	lvi.cchTextMax = 0;
	lvi.lParam     = 0;
	
  lvi.iItem      = nRow;
	lvi.iSubItem   = 0;
	lvi.iImage	   = 0;
		
	int nCount = 1;
  CString strCount;
  if (nRow > 0)
  {
  	strCount = m_diffList.GetItemText(nRow - 1, 0);
    nCount = atoi(strCount) + 1;
  }

  strCount.Format(_T("%i "), nCount);
  lvi.pszText = (LPTSTR) strCount.GetBuffer(1);
  strCount.ReleaseBuffer();
  m_diffList.InsertItem(&lvi);
  m_diffList.SetItemData(nRow, nIdx);

  UpdateRec(pRec, nRow, true);

  //Need to update the count.
  int nSearchIdx;
  int nModCount;

  for (int nRowX = 0; nRowX < m_diffList.GetItemCount(); nRowX++)
  {
    nSearchIdx = m_diffList.GetItemData(nRowX);
  	strCount = m_diffList.GetItemText(nRowX, 0);
    nModCount = atoi(strCount);
    if (nRowX != nRow && nModCount >= nCount)
    {
      nModCount++;
      strCount.Format(_T("%i "), nModCount);
      m_diffList.SetItemText(nRowX, 0, strCount);
    }
  }
}

void CDiffDirView::OnPageSetup()
{
	CSmxPgSetupDlg dlg(0, this);
  dlg.m_eHeader = SWC_hidden;
  dlg.m_eFooter = SWC_hidden;
  dlg.m_strRegKey = GetOptions().GetSectionPath(szDiffDirPgSetup);

  AfxGetApp()->DoPrintDialog(&dlg);
}

void CDiffDirView::SetPageOrientation(bool bLandscape)
{
  ((CFCUApp *) AfxGetApp())->SetPageOrientation(bLandscape);
}

LPCTSTR CDiffDirView::GetPageSetupSection()
{
  return GetOptions().GetSectionPath(szDiffDirPgSetup);  
}

void CDiffDirView::OnEditComment() 
{
  if (m_diffList.GetCurFocus() >= 0)
    m_diffList.EditLabel(m_diffList.GetCurFocus(), -1);  	
}

void CDiffDirView::OnViewCompareagain() 
{
  ((CDiffDirDoc *) GetDocument())->Refresh(false);
	OnUpdate(NULL, NULL, NULL);
}
