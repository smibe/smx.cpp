/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: OXDirDlg.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/OXDirDlg.cpp,v 1.1 2002/10/12 07:20:54 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
//            Class Implementation : COXDirectoryDialog
// ==========================================================================

// Source file :oxdirdlg.cpp

// Source : R.Mortelmans & F.Melendez
// Creation Date :         28th April 1995
// Last Modification : 10th June 1996

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"             // standard MFC include
#include <stdlib.h>
#include <dlgs.h>
#include <direct.h>
#include <limits.h>             // For SHRT_MAX
#include "oxdirdlg.h"           // class specification
#include "oxdirres.h"           // resource IDs
#include "path.h"               // To get current dir, create a dir, etc.

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Definition of static members


// Data members -------------------------------------------------------------
// protected:
        // CString m_sTitle;
        // --- The title text of the dialog

        // CString m_sFullDir;
        // --- The specified directory (may not yet exist);

        // CString m_sExistingDir;
        // --- That part of the specified directory that does exist

        // BOOL m_bNewDirAllowed
        // --- Whether the specification of a non-existing directory is allowed

        // BOOL m_bAfterInit
        // --- Whether the initializations in OnInitDialog executed already or not

        // BOOL m_bExplorerOn95
        // --- Whether the class is used in a Windows95 environment and with EXPLORER look

// private:

// Member functions ---------------------------------------------------------
// public:

COXDirectoryDialog::COXDirectoryDialog (LPCTSTR pszDefaultDir /* = NULL */ ,
                                        LPCTSTR pszTitle /* = NULL */ ,
                                        BOOL bNewDirAllowed /* = FALSE */ ,
                                        DWORD dwFlags   /* = OFN_HIDEREADONLY */
                                        , CWnd * pParentWnd /* = NULL */ )
:
m_sTitle (pszTitle),
m_sFullDir (pszDefaultDir),
m_sExistingDir (), m_bNewDirAllowed (bNewDirAllowed), m_bAfterInit (FALSE),
#ifdef WIN32
  m_bExplorerOn95 ((dwFlags & OFN_EXPLORER) && IsWin95 ()),
CFileDialog (TRUE, NULL, NULL,
             ((dwFlags & OFN_EXPLORER)
              && IsWin95 ())? dwFlags : dwFlags | OFN_ENABLETEMPLATE, NULL,
             pParentWnd)
#else
  m_bExplorerOn95 (FALSE),
CFileDialog (TRUE, NULL, NULL, dwFlags | OFN_ENABLETEMPLATE, NULL, pParentWnd)
#endif
{
  //{{AFX_DATA_INIT(COXDirectoryDialog)
  //}}AFX_DATA_INIT

#ifdef _DEBUG
  if (!m_bExplorerOn95)
    // ... The new directory dialog resource must be accessable
#ifdef _AFXDLL
    ASSERT (AfxFindResourceHandle (MAKEINTRESOURCE (FILEOPENDUMMY), RT_DIALOG)
            != NULL);
#else
    ASSERT (AfxGetResourceHandle () != NULL);
#endif
#endif

  // ... If no directory is specified, use the current default dir
  if (m_sFullDir.IsEmpty ())
  {
    COXDirSpec currentDir;
    VERIFY (currentDir.DoGetCurrentDir ());
    m_sFullDir = currentDir.GetDirectory ();
  }

  // It is possible that some parts of the specified directory do not yet exist
  // (E.g. When specifying C:\ONO\TWO\THREE, it is possible that
  //  C:\ONE\TWO exists, but C:\ONO\TWO\THREE does not)
  // We will now extract the existing part
  COXDirSpec realDir;
  if (!realDir.SetDirectory (m_sFullDir)
      || realDir.GetDirectory ().IsEmpty ())
  {
    TRACE (_T
           ("COXDirectoryDialog::COXDirectoryDialog ; Illegal or no directrory (%s) specified, using default dir\n"),
           pszDefaultDir == NULL ? _T ("") : pszDefaultDir);
    VERIFY (realDir.DoGetCurrentDir ());
    m_sFullDir = m_sExistingDir = realDir.GetDirectory ();
  }
  else
  {
    realDir.MakeLargestExisting ();
    m_sExistingDir = realDir.GetDirectory ();
  }

  m_ofn.lpstrInitialDir = m_sExistingDir;
  m_ofn.lpstrTitle = m_sTitle;

#ifdef WIN32
  if (!m_bExplorerOn95)
  {
    // MFC CFileDialog automatically chooses EXPLORER look when in Win95
    // even when the user did not pass it in the parameter dwFlags
    // so eliminate it again
    m_ofn.Flags &= ~OFN_EXPLORER;
    m_ofn.hInstance = AfxGetInstanceHandle ();
    m_ofn.lpTemplateName = MAKEINTRESOURCE (FILEOPENDUMMY);
    ASSERT (!(m_ofn.Flags & OFN_EXPLORER));
  }
#else
  m_ofn.hInstance = AfxGetResourceHandle ();
  m_ofn.lpTemplateName = MAKEINTRESOURCE (FILEOPENDUMMY);
#endif
}

#ifdef WIN32
BOOL COXDirectoryDialog::IsWin95 ()
  // --- In  : 
  // --- Out : 
  // --- Returns :
  // --- Effect : Determine the environment the app is executing in.
{
  BOOL
    bWin95 =
    FALSE;
  OSVERSIONINFO
    info;

  info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
  GetVersionEx (&info);
  if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ||
      (info.dwPlatformId == VER_PLATFORM_WIN32_NT
       && info.dwMajorVersion >= 4))
    bWin95 = TRUE;

  return bWin95;
}

CString COXDirectoryDialog::GetFolderPath () const
  // --- In  : 
  // --- Out : 
  // --- Returns :
  // --- Effect : When in EXPLORER look ask the dialog what is the currently
  //                              selected directory
{
  ASSERT (::IsWindow (m_hWnd));
  ASSERT (m_bExplorerOn95);

  CString
    strResult;
  if (GetParent ()->
      SendMessage (CDM_GETFOLDERPATH, (WPARAM) MAX_PATH,
                   (LPARAM) strResult.GetBuffer (MAX_PATH)) < 0)
    strResult.Empty ();
  else
    strResult.ReleaseBuffer ();

  return strResult;
}
#endif

BOOL COXDirectoryDialog::OnInitDialog ()
{
  CFileDialog::OnInitDialog ();

  CenterWindow ();

  if (!m_bExplorerOn95)
  {
    // .... The directory edit control must exist within this dialog
    ASSERT (GetDlgItem (IDC_DIRECTORY) != NULL);

    // ... Use lower case to show a dir
    CString
    sDirectory (m_sFullDir);
    //sDirectory.MakeLower();
    GetDlgItem (IDC_DIRECTORY)->SetWindowText (sDirectory);

    // Let's hide these windows so the user cannot tab to them.  Note that in
    // the private template (in cddemo.dlg) the coordinates for these guys are
    // *outside* the coordinates of the dlg window itself.  Without the following
    // ShowWindow()'s you would not see them, but could still tab to them.
    GetDlgItem (stc2)->ShowWindow (SW_HIDE);
    GetDlgItem (stc3)->ShowWindow (SW_HIDE);
    GetDlgItem (edt1)->ShowWindow (SW_HIDE);
    GetDlgItem (lst1)->ShowWindow (SW_HIDE);
    GetDlgItem (cmb1)->ShowWindow (SW_HIDE);

    // We must put something in this field, even though it is hidden.  This is
    // because if this field is empty, or has something like "*.txt" in it,
    // and the user hits OK, the dlg will NOT close.  We'll jam something in
    // there (like "Junk") so when the user hits OK, the dlg terminates.
    // Note that we'll deal with the "Junk" during return processing (see below)
    SetDlgItemText (edt1, _T ("Junk"));

    // The directory text shown in the static control above the directory
    // listbox, will always fit.  When the directory text becomes to large
    // full stops are automatically inserted (like "C:\..\DIR1\DIR2")
    // Because we want to use this contents to show in an edit control, 
    // no full stops should be inserted.  
    // This can be done by making the static control large enough 
    // so full stops will never be inserted
    // Because it will always be invisible, this poses no problem

    CWnd *
      pDirStatic;
    CRect
      staticRect;
    pDirStatic = GetDlgItem (stc1);
    ASSERT (pDirStatic != NULL);
    // ... Get the present size
    pDirStatic->GetClientRect (staticRect);
    // ... Make as large as possible
    staticRect.left = 0;
    staticRect.right = SHRT_MAX;
    pDirStatic->MoveWindow (staticRect);
    // ... Hide window
    pDirStatic->ShowWindow (SW_HIDE);

    // Now set the focus to the directory edit control and 
    // select that part of the directory that does not yet exist on disk
    GetDlgItem (IDC_DIRECTORY)->SetFocus ();
    // Determine tha part of the full directory, that is also in the
    // existing directory
    //m_sFullDir.MakeLower();
    //m_sExistingDir.MakeLower();
    LPCTSTR
      pszFullDir =
      m_sFullDir;
    LPCTSTR
      pszExistingDir =
      m_sExistingDir;
    // ... While neither has reached the end of the string and both
    //     characters are the same, continue searching
    while ((*pszFullDir == *pszExistingDir) && (*pszFullDir != _T ('\0')))
    {
      pszFullDir++;
      pszExistingDir++;
    }
    // ... Select thet apret that does not yet exist on disk
    ((CEdit *) GetDlgItem (IDC_DIRECTORY))->SetSel (pszFullDir -
                                                    (LPCTSTR) m_sFullDir,
                                                    INT_MAX);
  }
#ifdef WIN32
  else
  {
    GetParent ()->SendMessage (CDM_HIDECONTROL, (WPARAM) stc3, (LPARAM) 0);
    GetParent ()->SendMessage (CDM_HIDECONTROL, (WPARAM) edt1, (LPARAM) 0);
    GetParent ()->SendMessage (CDM_HIDECONTROL, (WPARAM) stc2, (LPARAM) 0);
    GetParent ()->SendMessage (CDM_HIDECONTROL, (WPARAM) cmb1, (LPARAM) 0);
  }
#endif

  m_bAfterInit = TRUE;
  // Focus already explicitely set
  return FALSE;
}

CString COXDirectoryDialog::GetDirectory ()
{
  return m_sFullDir;
}

// protected:
void
COXDirectoryDialog::OnLBSelChangedNotify (UINT nIDBox, UINT iCurSel,
                                          UINT nCode)
{
  ASSERT (!m_bExplorerOn95);
  if (m_bExplorerOn95)
    return;

  CFileDialog::OnLBSelChangedNotify (nIDBox, iCurSel, nCode);
  // When the selection in the directory listbox changes,
  // adjust the diretcory in the edit control
  if (m_bAfterInit && ((nIDBox == lst2) && (nCode == CD_LBSELCHANGE) ||
                       (nIDBox == cmb2) && (nCode == CD_LBSELCHANGE)))
  {
    // Tranfer the directory spec from the hidden static control
    // to the visible edit control
    CString sDir;
    GetDlgItem (stc1)->GetWindowText (sDir);
    VERIFY (GetCaseSensitive (sDir));

    // ... The string should not contain double full stops within the
    //     directory spec (this would mean that the hidden static control
    //     is to small!)
    ASSERT (sDir.Find (_T ("..")) == -1);
    GetDlgItem (IDC_DIRECTORY)->SetWindowText (sDir);
  }
}

#ifdef WIN32
void
COXDirectoryDialog::OnFolderChange ()
        // Notification for Windows 95 and Windows NT 4.0
{
  ASSERT (m_bExplorerOn95);
  if (!m_bExplorerOn95)
    return;

  COXPathSpec sPath;
  m_sFullDir = GetFolderPath ();
  sPath.SetDirectory (m_sFullDir);
  sPath.SetFileName (_T ("junk.xxx"));
  GetParent ()->SendMessage (CDM_SETCONTROLTEXT, (WPARAM) edt1,
                             (LPARAM) (LPCTSTR) sPath.GetPath ());
}
#endif

void
COXDirectoryDialog::OnOK ()
        // --- In  : 
        // --- Out : 
        // --- Returns :
        // --- Effect : Called when the user clicks the OK button 
        //                              (the button with an ID of IDOK).
{
  // .... The directory edit control must exist within this dialog
  ASSERT (GetDlgItem (IDC_DIRECTORY) != NULL);
  GetDlgItem (IDC_DIRECTORY)->GetWindowText (m_sFullDir);

  BOOL bOK = TRUE;

  COXDirSpec fullDir;
  if (!fullDir.SetDirectory (m_sFullDir))
  {
    TRACE (_T ("COXDirectoryDialog::OnOK : Invalid dir : %s\n"),
           (LPCTSTR) m_sFullDir);
    CString sPrompt;
    AfxFormatString1 (sPrompt, IDS_INVALID_DIR, (LPCTSTR) m_sFullDir);
    AfxMessageBox (sPrompt, MB_ICONEXCLAMATION, IDS_INVALID_DIR);
    // ... Invalid dir specified
    bOK = FALSE;
  }
  else
  {
    if (!fullDir.Exists () && m_bNewDirAllowed)
    {
      CString sPrompt;
      AfxFormatString1 (sPrompt, IDS_CREATE_NEW_DIR, (LPCTSTR) m_sFullDir);
      if (AfxMessageBox
          (sPrompt, MB_ICONINFORMATION | MB_YESNO,
           IDS_CREATE_NEW_DIR) == IDYES)
      {
        if (fullDir.DoMakeNew ())
        {
          m_sFullDir = m_sExistingDir = fullDir.GetDirectory ();
        }
        else
        {
          AfxMessageBox (IDS_FAILED_CREATE_DIR, MB_ICONEXCLAMATION,
                         IDS_FAILED_CREATE_DIR);
          // .... Failed to create dir
          bOK = FALSE;
        }
      }
      else
      {
        // ... User did not allow to create a new directory
        bOK = FALSE;
      }
    }
    else
    {
      if (!fullDir.Exists () && !m_bNewDirAllowed)
      {
        AfxMessageBox (IDS_NON_EXISTING_DIR, MB_ICONEXCLAMATION,
                       IDS_NON_EXISTING_DIR);
        // .... Directory does not exist and not allowed to create
        bOK = FALSE;
      }
    }
  }

  // ... If everything went OK, call base class implementation
  if (bOK)
    CFileDialog::OnOK ();
  else
    // ... Set focus on invalid entry
    GetDlgItem (IDC_DIRECTORY)->SetFocus ();
}

void
COXDirectoryDialog::OnCancel ()
        // --- In  : 
        // --- Out : 
        // --- Returns :
        // --- Effect : Called when the user clicks the CANCEL button 
        //                              (the button with an ID of IDCANCEL).
{
  // ... Clear the result dir
  m_sFullDir.Empty ();
  // ... Call base class implementation
  CFileDialog::OnCancel ();
}

// private:

// Message handlers ---------------------------------------------------------

BEGIN_MESSAGE_MAP (COXDirectoryDialog, CFileDialog)
  //{{AFX_MSG_MAP(COXDirectoryDialog)
  //}}AFX_MSG_MAP
  END_MESSAGE_MAP ()
     bool COXDirectoryDialog::GetCaseSensitive (CString & sDir)
{
  CListBox *pDirLBBox = (CListBox *) GetDlgItem (lst2);
  CString sTempDir = sDir;
  CString sNewDir;
  CString sSubDir;
  int idx;
  int nIndex;

  if (sTempDir[sTempDir.GetLength () - 1] == '\\')
    sTempDir = sTempDir.Mid (0, sTempDir.GetLength () - 1);

  idx = sTempDir.Find (':');
  if (idx != -1)
  {
    sNewDir = sTempDir.Left (2);
    sTempDir = sTempDir.Mid (idx + 1);
  }

  if (!sTempDir.IsEmpty () && sTempDir.GetAt (0) == '\\')
  {
    sTempDir = sTempDir.Mid (1);
    sNewDir = sNewDir + '\\';
  }

  while (!sTempDir.IsEmpty ())
  {
    idx = sTempDir.Find ('\\');

    if (idx != -1)
    {
      sSubDir = sTempDir.Left (idx);
      sTempDir = sTempDir.Mid (idx + 1);
    }
    else
    {
      sSubDir = sTempDir;
      sTempDir.Empty ();
    }

    if (sSubDir.IsEmpty ())
      break;

    nIndex = pDirLBBox->FindStringExact (0, sSubDir);

    if (nIndex == LB_ERR)
      return false;

    pDirLBBox->GetText (nIndex, sSubDir);
    ASSERT (!sSubDir.IsEmpty ());
    sNewDir = sNewDir + sSubDir + '\\';
  }

  sDir = sNewDir;
  return true;
}

// ==========================================================================
