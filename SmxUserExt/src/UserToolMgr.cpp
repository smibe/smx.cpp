/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: UserToolMgr.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/UserToolMgr.cpp,v 1.2 2002/11/22 17:20:01 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc
// UserToolMgr.cpp: implementation of the CUserToolMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Util.h"
#include "UserToolMgr.h"
#include "AcceleratorManager.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BYTE CUserToolMgr::m_nVersion = 1;
BYTE CSmxUserTool::m_nVersion = 2;

LPCTSTR szIniFileFormat  = _T("FcuUsr.utl");
LPCTSTR szIniFileFormat1 = _T("FcuUsr%d.utl");

bool IsInternalCommand(CString& strCmd);

CSmxUserTool::CSmxUserTool()
{
  m_dwFlags        = 0;
}

CSmxUserTool::CSmxUserTool(LPCTSTR pszCommand, LPCTSTR pszParameter, LPCTSTR pszStartDir, LPCTSTR pszMenuString, LPCTSTR pszDescription)
{
  m_strCommand     = pszCommand;
  m_strStartDir    = pszStartDir;
  m_strMenuString  = pszMenuString;
  m_strDescription = pszDescription;
  m_strParameter   = pszParameter;
  m_dwFlags        = 0;
}

CSmxUserTool::~CSmxUserTool()
{
}

bool CSmxUserTool::Load(CArchive& ar)
{
  BYTE nVersion = 0;
  ar >>  nVersion;

  if (nVersion >  m_nVersion)
    return false;

  ar >>  m_strCommand;
  ar >>  m_strStartDir;
  ar >>  m_strMenuString;
  ar >>  m_strDescription;
  ar >>  m_strParameter;
  ar >>  m_dwFlags;

  if (nVersion >= 2)
    ar >> m_strBatchFileName;

  return true;
}

bool CSmxUserTool::Store(CArchive& ar)
{
  ar <<  m_nVersion;
  ar <<  m_strCommand;
  ar <<  m_strStartDir;
  ar <<  m_strMenuString;
  ar <<  m_strDescription;
  ar <<  m_strParameter;
  ar <<  m_dwFlags;
  ar <<  m_strBatchFileName;

  return true;
}

CUserToolMgr::CUserToolMgr()
{
  m_nFirstCmdID = 0;
  m_nLastCmdID  = 0;
}

CUserToolMgr::~CUserToolMgr()
{
  for (int i = 0; i <= m_UserTools.GetUpperBound(); i++)
  {
    if (m_UserTools[i])
    {
      delete m_UserTools[i];
      m_UserTools[i] = NULL;
    }
  }
}

UINT CUserToolMgr::AddUserTool(LPCTSTR pszCommand, LPCTSTR pszParameter, LPCTSTR pszStartDir, LPCTSTR pszMenuString, LPCTSTR pszDescription)
{
  UINT nNextTool = m_UserTools.GetUpperBound() + 1;
  CSmxUserTool *pUserTool = new CSmxUserTool(pszCommand, pszParameter, pszStartDir, pszMenuString, pszDescription);
  m_UserTools.Add(pUserTool);

  return m_nFirstCmdID;
}

void CUserToolMgr::SetUserCmdID(UINT nFirstCmdID, UINT nLastCmdID)
{
  ASSERT(nFirstCmdID > 0 && nFirstCmdID < nLastCmdID);
  m_nFirstCmdID = nFirstCmdID;
  m_nLastCmdID  = nLastCmdID;
}

void CUserToolMgr::UpdateUserToolMenu(CMenu *pMenu)
{
  //Remove all Tool entries
  UINT nCount = pMenu->GetMenuItemCount();
  UINT nCmdID;
  for (UINT i = 0; i < pMenu->GetMenuItemCount(); i++)
  {
    nCmdID = pMenu->GetMenuItemID(i);

    if (nCmdID >= m_nFirstCmdID && nCmdID <= m_nLastCmdID)
    {
      pMenu->DeleteMenu(i, MF_BYPOSITION);
      i--;
    }
  }

  //Now add all tool menu entries
  for (int nIdx = 0; nIdx <= m_UserTools.GetUpperBound(); nIdx++)
  {
    nCmdID = m_nFirstCmdID + nIdx;
    pMenu->InsertMenu(nIdx, MF_BYPOSITION, nCmdID, m_UserTools[nIdx]->m_strMenuString);
  }
  
  if (nIdx > 0 && nIdx < (int)pMenu->GetMenuItemCount() && pMenu->GetMenuItemID(nIdx) != 0)
    pMenu->InsertMenu(nIdx, MF_BYPOSITION, 0);
}

bool CUserToolMgr::Load(HKEY hRegKey, LPCTSTR szRegKey)
{
  bool bResult = true;
  m_hRegKey   = hRegKey;
  m_strRegKey = szRegKey;

	CRegistry reg;
	reg.Open(m_hRegKey, m_strRegKey);
	if (! reg.VerifyValue  ("UserTools")) 
    return false;

  CString strUserToolFile;
  BOOL bRet = reg.Read("UserTools", strUserToolFile);
	reg.Close();
	if (bRet != TRUE) 
    return false;

  GetUserToolsPath(strUserToolFile);

  //Now load the user tools from file
  CFile File;
  if (!File.Open(strUserToolFile, CFile::modeRead))
  {
    CString str;
    str.Format("Can't load the user tools from file %s!", strUserToolFile);
    AfxMessageBox(str);
    return false;
  }

  CArchive ar(&File, CArchive::load);

  bResult = Load(ar);

  ar.Close();
  File.Close();
  
  return bResult;
}

void CUserToolMgr::Clear()
{
  for (int i = 0; i <= m_UserTools.GetUpperBound(); i++)
  {
    if (m_UserTools[i])
    {
      delete m_UserTools[i];
      m_UserTools[i] = NULL;
    }
  }

  m_UserTools.RemoveAll();
}

bool CUserToolMgr::Load(CArchive& ar)
{
  BYTE nVersion = 0;
  ar >> nVersion;

  if (nVersion >  m_nVersion)
    return false;

  Clear();

  int nNofValues = 0;
  ar >> nNofValues;
  m_UserTools.SetSize(nNofValues);
  CSmxUserTool *pUserTool;
  for (int i = 0; i < nNofValues; i++)
  {
     pUserTool = new CSmxUserTool;
     pUserTool->Load(ar);
     m_UserTools.SetAt(i, pUserTool);
  }
  return true;
}

bool CUserToolMgr::Store(CArchive& ar)
{
  ar << m_nVersion;

  int nNofValues = m_UserTools.GetUpperBound() + 1;
  ar << nNofValues;

  CSmxUserTool *pUserTool;
  for (int i = 0; i < nNofValues; i++)
  {
    pUserTool =  m_UserTools[i];
    pUserTool->Store(ar);
  }
  return true;
}

void CUserToolMgr::SetRegKey(HKEY hRegKey, LPCTSTR szRegKey)
{
  m_hRegKey   = hRegKey;
  m_strRegKey = szRegKey;
}

bool CUserToolMgr::Store()
{
  bool bResult = true;
  if (m_hRegKey == NULL || m_strRegKey.IsEmpty())
  {
    ASSERT(false);
    return (false);
  }

  CString strFileName;
	CRegistry reg;
  CFile File;
	reg.Open(m_hRegKey, m_strRegKey);
	if (! reg.VerifyValue  ("UserTools"))
  {
    //Make sure the directory exists
    GetUserToolsPath(strFileName);
    if (!CreateDirectory(strFileName, NULL))
    {
      if (GetLastError() != ERROR_ALREADY_EXISTS)
      {
        DWORD dwError = GetLastError();
        CString msg;
        msg.Format("Cant create the user tools directory %s!", strFileName);
        AfxMessageBox(msg);
        return false;
      }
    }

    //Create the temporary file
    bool bFileExists = true;
    int  i = 0;
    while (bFileExists)
    {
      if (i == 0)
        strFileName.Format(szIniFileFormat, i);
      else
        strFileName.Format(szIniFileFormat1, i);

      GetUserToolsPath(strFileName);
      
      if (!File.Open(strFileName, CFile::modeRead))
        break;
      File.Close();
      i++;
    }

    CString strTemp;
    if (i == 0)
      strTemp.Format(szIniFileFormat, i);
    else
      strTemp.Format(szIniFileFormat1, i);

    BOOL bRet = reg.Write("UserTools", strTemp);
    reg.Close();
	  if (bRet != TRUE) 
    {
      return false;
    }
  }
  else
  {
    BOOL bRet = reg.Read("UserTools", strFileName);
    reg.Close();
	  if (bRet != TRUE) 
    {
      return false;
    }
    GetUserToolsPath(strFileName);
  }

  //Now store the user tools to the file
  if (!File.Open(strFileName, CFile::modeCreate | CFile::modeWrite))
  {
    CString str;
    str.Format("Can't store the user tools to the file %s!", strFileName);
    AfxMessageBox(str);
    return false;
  }

  CArchive ar(&File, CArchive::store);

  bResult = Store(ar);

  ar.Close();
  File.Close();

  return bResult;
}

void CUserToolMgr::SetCallbackFct(CWnd *pWnd, EXPAND_VAR_FKT pExpandVarFkt, CAN_EXPAND_VAR_FKT pCanExpandVarFkt)
{
  m_pWnd             = pWnd;
  m_pExpandVarFkt    = pExpandVarFkt;
  m_pCanExpandVarFkt = pCanExpandVarFkt;

}

bool CUserToolMgr::Execute(UINT nCmdID)
{
  if (nCmdID >= m_nFirstCmdID && nCmdID <= m_nFirstCmdID + m_UserTools.GetUpperBound())
  {
    CSmxUserTool *pUserTool = m_UserTools[nCmdID - m_nFirstCmdID]; 
    CString strStartDir = pUserTool->m_strStartDir;
    CString strParameter = pUserTool->m_strParameter;
    CString strParam;
    CString strExecDir;
    CStringArray strArrayDir;
    CStringArray strArrayParam;

    if (Eval(strStartDir, strArrayDir, true) && Eval(strParameter, strArrayParam, true))
    {
      if (!pUserTool->m_strCommand.IsEmpty())
      {
        CString strCommand = pUserTool->m_strCommand;
        int i       = 0;
        int nDirIdx = 0;

        HINSTANCE hInst = NULL;
        
        if (pUserTool->m_dwFlags & UT_BATCH)
        {
          CStdioFile File;
          CString str;

          if (!File.Open(pUserTool->m_strBatchFileName, CFile::modeWrite | CFile::modeCreate))
          {
            str.Format("Could not open the batch file %s!", pUserTool->m_strBatchFileName);
            AfxMessageBox(str);
            return false;
          }

          str.Format("rem Batch file for command[%s]\n", pUserTool->m_strMenuString);
          File.WriteString(str);
          if (strArrayDir.GetSize() == 1)
          {
            str.Format("cd %s\n", strArrayDir[0]);
            File.WriteString(str);
          }

          if (pUserTool->m_dwFlags & UT_MULTIPLEEXE)
          {
            for (int i = 0; i < strArrayParam.GetSize(); i++)
            {
              if (strArrayDir.GetSize()  == strArrayParam.GetSize() && strArrayDir.GetSize() > 1)
              {
                str.Format("cd %s\n", strArrayDir[i]);
                File.WriteString(str);
              }

              str.Format("%s %s\n", pUserTool->m_strCommand, strArrayParam[i]);
              File.WriteString(str);
            }
          }
          else
          {
            CString strParam;
            ASSERT(strArrayDir.GetSize() == 1);

            for (i = 0; i < strArrayParam.GetSize(); i++)
            {
              if (i > 0)
                strParam += " ";
            
              strParam += strArrayParam[i];
            }
            str.Format("%s %s\n", pUserTool->m_strCommand, strParam);
            File.WriteString(str);
          }
          File.Close();

          if (pUserTool->m_dwFlags & UT_BATCHNOEXECUTE)
            return true;
        }

        if (pUserTool->m_dwFlags & UT_BATCH && !(pUserTool->m_dwFlags & UT_BATCHNOEXECUTE))
        {
          //execute batch file
          strParam = strArrayParam[0];
          strExecDir = strArrayDir[0];
          hInst  = Run(pUserTool->m_strBatchFileName, strArrayDir[0], strArrayParam[0], SW_SHOW, false);
        }
        else if (pUserTool->m_dwFlags & UT_MULTIPLEEXE)
        {
          //Execute multiple commands
          for (i = 0; i < strArrayParam.GetSize(); i++)
          {
            if (strArrayDir.GetSize() == strArrayParam.GetSize())
              nDirIdx = i;

            strParam = strArrayParam[i];
            strExecDir = strArrayDir[nDirIdx];
            hInst  = Run(pUserTool->m_strCommand, strArrayDir[nDirIdx], strArrayParam[i], SW_SHOW, false);
            if ((UINT)hInst <= 32)
              break;
          }
        }
        else 
        {
          ASSERT(strArrayDir.GetSize() == 1);

          for (i = 0; i < strArrayParam.GetSize(); i++)
          {
            if (i > 0)
              strParam += " ";
            
            strParam += strArrayParam[i];
          }
        
          strExecDir = strArrayDir[0];
          hInst  = Run(pUserTool->m_strCommand, strArrayDir[0], strParam, SW_SHOW, false);
        }


        if ((int) hInst < 32)
        {
          CString strMessage;
          CString strCmd = pUserTool->m_strCommand;

          if (pUserTool->m_dwFlags & UT_BATCH && !(pUserTool->m_dwFlags & UT_BATCHNOEXECUTE))
            strCmd = pUserTool->m_strBatchFileName;
          
          if (strExecDir.IsEmpty())
          {
            strMessage.Format("%s\nCommand: %s\nParameter: %s", 
                               GetShellErrorString((DWORD)hInst),
                               strCmd,
                               strParam);
          }
          else
          {
            strMessage.Format("%s\nCommand: %s\nParameter: %s\nInitial directory: %s", 
                               GetShellErrorString((DWORD)hInst),
                               strCmd, 
                               strParam,
                               strExecDir);
          }

          AfxMessageBox(strMessage);
        }
      }
    }
  }
  else
  {
    ASSERT(false);
  }

  return true;
}

HINSTANCE CUserToolMgr::Run(LPCTSTR pszCmd, LPCTSTR pszDir, LPCTSTR pszParam, UINT nShow, bool bCloseWindow)
{
	bool bResult = true;
  CString strExeFile = pszCmd;
  bool bInternalCommand = IsInternalCommand(strExeFile);
  HINSTANCE hInst = NULL;

  if (!bInternalCommand)
  {
    hInst =  FindExecutable(pszCmd, pszDir, strExeFile.GetBuffer(MAX_PATH));
    strExeFile.ReleaseBuffer();
    if ((UINT) hInst <= 32)
    {
      return hInst;
    }
  }

  CString strCmd;
  CString strParam;
  DWORD dwExeType = SHGetFileInfo(strExeFile, NULL, NULL, 0, SHGFI_EXETYPE);
  
  if (IsInternalCommand(strExeFile) || (!strncmp((LPCTSTR) &dwExeType, "PE", 2) && HIWORD(dwExeType) == 0))
  {
    strCmd = "cmd.exe";
    strParam = "/c ";
    strParam += pszCmd;
    strParam += " "; 
    strParam += pszParam;

    if (!bCloseWindow)
      strParam +=" & pause";
  }
  else
  {
    strCmd = pszCmd;
    strParam = pszParam;
  }
      
  hInst = ShellExecute(NULL, "open", strCmd, strParam,  pszDir, nShow); 

  return hInst;
}



bool CUserToolMgr::CanExecute(UINT nCmdID)
{
  if (nCmdID >= m_nFirstCmdID && nCmdID <= m_nFirstCmdID + m_UserTools.GetUpperBound())
  {
    CSmxUserTool *pUserTool = m_UserTools[nCmdID - m_nFirstCmdID]; 
    CString strStartDir = pUserTool->m_strStartDir;
    CString strParameter = pUserTool->m_strParameter;
    return   CanEval(strStartDir) && CanEval(strParameter);
  }

  return false;
}

void CUserToolMgr::SplitString(CString& strString, CStringArray& strSplitArray)
{
  CString strToDo = strString;
  int nIdx = -1;

  for (nIdx = -1; nIdx < strToDo.GetLength(); )
  {
    if (strToDo.IsEmpty())
      break;

    nIdx = strToDo.Find(TCHAR('$'));
    if (nIdx == -1)
    {
      strSplitArray.Add(strToDo);
      break;
    }
    else
    {
      if (nIdx > 0)
        strSplitArray.Add(strToDo.Left(nIdx));

      strToDo = strToDo.Mid(nIdx);
      nIdx = 1;
      TCHAR c;
      while (nIdx < strToDo.GetLength())
      {
        c = strToDo[nIdx];
        //Skip all characters between [a-z] and [A-Z]
        if (!((c >= TCHAR('a') && c <= TCHAR('z')) || (c >= TCHAR('A') && c <= TCHAR('Z'))))
          break;

        nIdx++;
      }

      if (nIdx < strToDo.GetLength())
      {
        strSplitArray.Add(strToDo.Left(nIdx));
        strToDo = strToDo.Mid(nIdx);
      }
      else
      {
        strSplitArray.Add(strToDo);
        break;
      }
    }
  }
}

bool CUserToolMgr::Eval(CString& strString, CStringArray& strArray, bool bArray)
{
  CStringArray strSplitArray;
  int     i = 0;

  SplitString(strString, strSplitArray);
  for (i = 0; i < strSplitArray.GetSize(); i++)
  {
    if (strSplitArray[i].GetAt(0) == TCHAR('$'))
    {
      CStringArray strEval;
      if (m_pWnd && m_pExpandVarFkt)
      {
        if (!(m_pWnd->*m_pExpandVarFkt)(strSplitArray[i], strEval))
          return false;
      }

      if (strEval.GetSize() == NULL)
        continue;

      if (bArray)
      {
        if (strArray.GetSize() > 1)
        {
          if (strArray.GetSize() != strEval.GetSize() && strEval.GetSize() > 1)
          {
            //Cant do this
            AfxMessageBox("Cant evalueate the parameters: the parameter numbers are incompatibel!");
            return false;
          }
        }
        else 
        {
          CString str;
          if (strArray.GetSize() == 1)
            str = strArray[0];
          
          if (strEval.GetSize() > 1)
            strArray.SetSize(strEval.GetSize());

          if (!str.IsEmpty())
          {
            for (int k = 1; k < strArray.GetSize(); k++)
            {
              strArray[k] = str;
            }
          }
        }

        if (strArray.GetSize() > 1)
        {
          if (strEval.GetSize() > 1)
          {
            ASSERT( strEval.GetSize() == strArray.GetSize());
            for (int k = 0; k < strArray.GetSize(); k++)
            {
              strArray[k] += strEval[k];
            }
          }
          else
          {
            ASSERT( strEval.GetSize() == 1);
            for (int k = 0; k < strArray.GetSize(); k++)
            {
              strArray[k] += strEval[0];
            }
          }
        }
        else
        {
          if ( strArray.GetSize() == 0)
            strArray.SetSize(1);

          ASSERT( strEval.GetSize() == 1 && strArray.GetSize() == 1);
          strArray[0] += strEval[0];
        }
      }
      else
      {
        //Do not create an array
        if ( strArray.GetSize() == 0)
          strArray.SetSize(1);

        ASSERT( strArray.GetSize() == 1);

        for (int k = 0; k < strEval.GetSize(); k++)
        {
          strArray[0] += strEval[0];
          if (k > 0)
            strArray[0] += TCHAR(' ');
        }
      }
    }
    else
    {
      //Nothing to evaluate, just add the string
      if (strArray.GetSize() == 0)
          strArray.SetSize(1);

      for (int k = 0; k < strArray.GetSize(); k++)
      {
        strArray[k] += strSplitArray[i];
      }
    }
  }

  if (strArray.GetSize() == 0)
  {
    strArray.SetSize(1);
  }

  return true;
}

bool CUserToolMgr::CanEval(CString& strString)
{
  CStringArray strSplitArray;
  bool bCanExpand = true;
  int     i = 0;

  SplitString(strString, strSplitArray);
  for (i = 0; i < strSplitArray.GetSize(); i++)
  {
    if (strSplitArray[i].GetAt(0) == TCHAR('$'))
    {
      CStringArray strEval;
      if (m_pWnd && m_pExpandVarFkt)
      {
        if (!(m_pWnd->*m_pCanExpandVarFkt)(strSplitArray[i]))
          return false;
      }
    }
  }
  
  return bCanExpand;
}

void CUserToolMgr::UpdateAccelMgr(CAcceleratorManager *pAccelMgr)
{
  POSITION pos = pAccelMgr->GetAccelTableMap().GetStartPosition();

  CCmdAccelOb *pCmdAccelOb = NULL;
	UINT         nCommandID  = 0;
  while (pos)
  {
    pAccelMgr->GetAccelTableMap().GetNextAssoc(pos, nCommandID, pCmdAccelOb); 
    if (nCommandID >= m_nFirstCmdID && nCommandID <= m_nLastCmdID)
    {
      //This is a user tool command
      if ((int) nCommandID > m_nFirstCmdID + m_UserTools.GetUpperBound())
      {
        pAccelMgr->DeleteEntry(nCommandID);
      }
    }
  }

  //Add all user tool commands
  for (int i = 0; i <= m_UserTools.GetUpperBound(); i++)
  {
    nCommandID = m_nFirstCmdID + i;
    if (!pAccelMgr->GetAccelTableMap().Lookup(nCommandID, pCmdAccelOb))
    {
      pCmdAccelOb = pAccelMgr->CreateEntry(nCommandID);
      pCmdAccelOb->SetCommandString(m_UserTools[i]->m_strMenuString);
    }
    else
    {
      pCmdAccelOb->SetCommandString(m_UserTools[i]->m_strMenuString);
    }
  }

  if (m_UserTools.GetUpperBound() < 0)
    pAccelMgr->SetUserCmdIDs(0, 0);
  else
    pAccelMgr->SetUserCmdIDs(m_nFirstCmdID, m_nFirstCmdID + m_UserTools.GetUpperBound());
}

void CUserToolMgr::GetUserToolsPath(CString& strFileName)
{
  CString      str  = strFileName;
  LPITEMIDLIST pidl = NULL;

  SHGetSpecialFolderLocation(AfxGetMainWnd()->GetSafeHwnd(), CSIDL_APPDATA, &pidl);
  SHGetPathFromIDList(pidl, strFileName.GetBuffer(MAX_PATH));
  strFileName.ReleaseBuffer();

  if (strFileName.IsEmpty())
  {
    //Seems like the shell could not find the Applikation Data directory
    GetModuleFileName(AfxGetInstanceHandle(), strFileName.GetBuffer(MAX_PATH), MAX_PATH);
    strFileName.ReleaseBuffer();

    int nIdx = strFileName.ReverseFind(TCHAR('\\'));
    if (nIdx != -1)
    {
      strFileName = strFileName.Left(nIdx + 1);
    }
  }

  if (strFileName.GetAt(strFileName.GetLength() - 1) != TCHAR('\\'))
    strFileName += TCHAR('\\');

  //strFileName += _T("UserTools\\");
  strFileName += str;
}


CSmxUserTool *CUserToolMgr::GetUserTool(UINT nCmdID)
{
  if (nCmdID - m_nFirstCmdID >= 0 && nCmdID <= m_UserTools.GetUpperBound() - m_nFirstCmdID)
    return m_UserTools[nCmdID - m_nFirstCmdID];

  return NULL;
}
