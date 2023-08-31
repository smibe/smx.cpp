/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffFrm.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffFrm.cpp,v 1.1 2002/09/20 09:34:47 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "fildifffrm.h"
#include "fildiffview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace
{
UINT gl_AccelGroups[] = 
{ 
  IDS_File, 1, 
    ID_CMP_DIR, ID_CMP_FILE, ID_FILE_SAVECOMPOSED, ID_FILE_PRINT, ID_FILE_PRINT_PREVIEW,
    ID_FILE_PAGE_SETUP, ID_APP_EXIT, ID_FILE_CLOSE,
    ID_BREAK,
    0,
  IDS_Edit,
    ID_EDIT_CUT,
    ID_EDIT_COPY,
    ID_EDIT_PASTE,
    ID_EDIT_UNDO,
    ID_EDIT_FIND,
    ID_EDIT_REPLACE,
    ID_EDIT_INSERTRIGHT,
    ID_EDIT_INSERTLEFT,
    ID_EDIT_INSERTBOTH,
    ID_EDIT_DELETEDIFF,
    ID_EDIT_RESTORELEFT,
    ID_EDIT_RESTORERIGHT,
    ID_EDIT_CMDMODE,
    ID_BOOKMARKNEXT,
    ID_BOOKMARKSET,
    ID_EDIT_FINDNEXT,
    ID_EDIT_REDO,
    ID_BREAK,
    0,

  IDS_View, 1, 
    ID_VIEW_REFRESH,
    ID_VIEW_NEXTDIFF,
    ID_VIEW_PREVDIFF,
    ID_VIEW_LINEDIFF,
    ID_VIEW_TOOLBAR,
    ID_VIEW_STATUS_BAR,
    ID_VIEW_OPTIONS,
    ID_VIEW_NEXT_DIFF_FROM_CURSOR,
    0,
  IDS_Tools, 1, 
    ID_TOOLS_COPYFILES, ID_TOOLS_CUSTOMIZE_TOOLBAR, 
    ID_TOOLS_CUSTOMIZE_KEYBOARD, ID_TOOLS_CUSTOMIZE_USERTOOLS,
    0,
  IDS_Windows, 1,
    ID_WINDOWS_LEFTFILE,
    ID_WINDOWS_RIGHTFILE,
    ID_WINDOWS_COMPFILE,
    ID_WINDOWS_NEXT,
    ID_NEXT_PANE,
    ID_PREV_PANE,
    0,
};
}


CAcceleratorManager* CFilDiffFrm::m_pAccelMgr = NULL;

extern LPCTSTR szInitPath;

IMPLEMENT_DYNCREATE(CFilDiffFrm, CFcuMDIChildWndEx)

CFilDiffFrm::CFilDiffFrm()
{
}

CFilDiffFrm::~CFilDiffFrm()
{
}


BEGIN_MESSAGE_MAP(CFilDiffFrm, CFcuMDIChildWndEx)
	//{{AFX_MSG_MAP(CFilDiffFrm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CFilDiffFrm::IsButtonInitialyHidden(UINT nCmdID)
{
  switch (nCmdID)
  {
    case ID_CMP_FILE:
    case ID_FILE_SAVECOMPOSED:
    case ID_VIEW_NEXTDIFF:
    case ID_VIEW_PREVDIFF:
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

CAcceleratorManager *CFilDiffFrm::GetAccelMgr()
{
  if (m_pAccelMgr == NULL)
  {
    m_pAccelMgr = new CAcceleratorManager;
    m_pAccelMgr->m_strView = "File Compare";

    m_pAccelMgr->SetAccelGroups(gl_AccelGroups, sizeof(gl_AccelGroups) / sizeof(UINT) );
      
    if (!m_pAccelMgr->Load(HKEY_CURRENT_USER, szInitPath, _T("FilDiffAccel")))
    {
      m_pAccelMgr->CreateDefaultTable(IDR_DIFF_FIL);
    }
    else
    {
      m_pAccelMgr->SetAccelTableID(IDR_DIFF_FIL);
    }

    GetUserToolMgr().UpdateAccelMgr(m_pAccelMgr);
    m_pAccelMgr->InitAccelGroups();
  }

  return m_pAccelMgr;
}

void CFilDiffFrm::DeleteAccelMgr()
{
  if (m_pAccelMgr)
  {
    m_pAccelMgr->Write();
    delete m_pAccelMgr;
  }
}
