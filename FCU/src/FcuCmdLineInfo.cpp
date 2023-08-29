/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FcuCmdLineInfo.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FcuCmdLineInfo.cpp,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "fcu.h"
#include "Path.h"
#include "Dir.h"
#include "Util.h"
#include "fcucmdlineinfo.h"
#include "fcuOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFCUCmdLineInfo::CFCUCmdLineInfo()
{
  Reset();
}

void CFCUCmdLineInfo::Reset()
{
  m_cLastOption = 0;
	m_bCmpFile    = false;
	m_bCmpDir     = false;
	m_bInvalid    = false;
  m_bCVSMode    = false;
  m_nShowDlg    = -1;
	
  m_strLeftParam.Empty();
	m_strRightParam.Empty();
  m_strExcludeMask.Empty();
  m_strFileMask.Empty();
}

CFCUCmdLineInfo::~CFCUCmdLineInfo()
{
}

void CFCUCmdLineInfo::ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast )
{
	CString strParam = lpszParam;

  if (m_bInvalid)
    return;

	if (!bFlag && m_cLastOption == 0)
	{
		//Call without options, try to find out what we have got
		if (!m_strRightParam.IsEmpty())
    {
       m_bInvalid = true;
       m_strError = _T("Too many parameters!");
       return;
    }

    CString str = GetFileNameFromPath(lpszParam);
    bool bFileMask = str.Find('*') != -1 || str.Find('?') != -1;
    if (bFileMask && !m_strFileMask.IsEmpty())
    {
      if (str != m_strFileMask)
      {
        m_strError = "Sorry, can't handle different file masks!";
        m_bInvalid = true;
        return;
      }
    }
    
    if (bFileMask && !m_bCmpFile)
    {
      m_bCmpDir = true;

      if (m_strLeftParam.IsEmpty())
        m_strLeftParam = GetDirFromPath(lpszParam);
      else
        m_strRightParam = GetDirFromPath(lpszParam);

      m_strFileMask = str;
      return;
    }

		COXDirSpec dirSpec(lpszParam);
		if (bFileMask || dirSpec.Exists())
		{
			//This is a directory
			if (m_bCmpFile)
      {
        // We have a file already as first parameter, compare that file
        // with a file with the same name in this directory.
        ASSERT(!m_strLeftParam.IsEmpty());
  			m_strRightParam = lpszParam;
        GetFileDir(m_strRightParam);
        m_strRightParam += GetFileNameFromPath(m_strLeftParam);
        return;
      }
      m_bCmpDir = true;

      if (m_strLeftParam.IsEmpty())
        m_strLeftParam = lpszParam;
      else
        m_strRightParam = lpszParam;
			
      return;
		}

		COXPathSpec pathSpec(lpszParam);
		if (pathSpec.Exists())
		{
			//This is a file

      if (m_bCmpDir)
      {
        // We have a file directory as first parameter, compare this file
        // with a file with the same name in that directory.
        ASSERT(!m_strLeftParam.IsEmpty());
  			m_strRightParam = lpszParam;
        AddBackslashToDir(m_strLeftParam);
        m_strLeftParam += GetFileNameFromPath(m_strRightParam);
        m_bCmpDir  = false;
        m_bCmpFile = true;
        return;
      }

      m_bCmpFile = true;
      if (m_strLeftParam.IsEmpty())
  			m_strLeftParam = lpszParam;
      else
        m_strRightParam = lpszParam;

      return;
		}
	}

  if (bFlag)
  {
	  if (m_cLastOption != 0)
    {
      m_strError.Format("Missing argument for option '%s'.", lpszParam);
      m_bInvalid = true;
      return;
    }

    bool bEnableOption = true;
    if (*lpszParam == 'n')
    {
      if (lpszParam[1] != 'o' || lpszParam[2] == '\0')
      {
        m_strError.Format("Invalid flag: %s!",  lpszParam);
        m_bInvalid = true;
        return;
      }

      lpszParam += 2;
      bEnableOption = false;
    }

    char c = *lpszParam;
    if (*lpszParam == '\0' || lpszParam[1] != '\0')
    {
      if (*lpszParam == 'S')
      {
        const char *pChar = lpszParam;
        while (*(++pChar) && !m_bInvalid)
        {
          switch(*pChar)
          {
            case 'l':
    	        SetOption(bEnableOption, OCF_ShowLeft);
              break;
            case 'r':
    	        SetOption(bEnableOption, OCF_ShowRight);
              break;
            case 'c':
    	        SetOption(bEnableOption, OCF_ShowComposed);
              break;
            case 'D':
    	        SetOption(bEnableOption, OCD_Different);
              break;
            case 'I':
    	        SetOption(bEnableOption, OCD_Identical);
              break;
            case 'L':
    	        SetOption(bEnableOption, OCD_LeftOnly);
              break;
            case 'R':
    	        SetOption(bEnableOption, OCD_RightOnly);
              break;
            default:
              m_bInvalid = true;
          }
        }
        if (!m_bInvalid)
          return;
      }
      m_strError.Format("Invalid flag: '%s'!",  lpszParam);
      m_bInvalid = true;
      return;
    }
      
    switch (c)
    {
      case 'i':
    	  SetOption(bEnableOption, OMSC_IgnoreCase);
        return;
      case 'w':
    	  SetOption(bEnableOption, OMSC_IgnoreBlanks);
        return;
      case 'B':
	      SetOption(bEnableOption, OMSC_IgnoreEOL);
        return;
      case 'r':
	      SetOption(bEnableOption, OCD_Subdir);
        return;
      case 'l':
        m_nShowDlg = bEnableOption ? 1 : 0;
        return;
      case 'x':
      case 'm':
        if (bLast)
        {
          m_strError.Format("Missing argument for '%s'!",  lpszParam);
          m_bInvalid = true;
          return;
        }
        m_cLastOption = c;
        return;
      default:
        m_strError.Format("Unknown option %s!");
        m_bInvalid = true;
        return;
    }
  }
  else
  {
    //It's not a flag
    switch (m_cLastOption)
    {
      case 'x':
        m_strExcludeMask = lpszParam;
        break;
      case 'm':
        m_strFileMask = lpszParam;
        break;
      default:
        m_strError.Format("Unknown parameter %s!", lpszParam);
        m_bInvalid = true;
        return;
    }
    m_cLastOption = 0;
    return;
  }
	
  // No other parameters allowed.
	m_bInvalid = true;

	CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast); 
}

bool CFCUCmdLineInfo::IsInvalid()
{
	return m_bInvalid;
}

bool CFCUCmdLineInfo::ParseCmdLine(CString strCmdLine)
{
  CString strToken;
  CString str   = strCmdLine;

  while (!str.IsEmpty())
  {
    strToken = str.SpanExcluding(_T(" \t"));
    str = str.Mid(strToken.GetLength());
    str.TrimLeft();

    if (strToken.IsEmpty())
    {
      ASSERT(false);
    }
    else
    {
      if (strToken[0] == '-' || strToken[0] == '/')
      {
        strToken = strToken.Mid(1);
        ParseParam(strToken, true, str.IsEmpty());
      }
      else
      {
        ParseParam(strToken, false, str.IsEmpty());
      }
    }
  }

  if (m_nShowDlg > 0 || (!m_strLeftParam.IsEmpty() && m_strRightParam.IsEmpty()))
  {
    if (m_bCmpFile)
      SetOption(true, OCF_ShowCmpFile);
    else
      SetOption(true, OCD_ShowCmpDir);
  }

  return true;
}

CFCUCmdLineInfo& GetCmdLineInfo()
{
  static CFCUCmdLineInfo TheCmdLineInfo;
  return TheCmdLineInfo;
}
