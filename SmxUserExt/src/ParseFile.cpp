/*******************************************************************************
// Copyright (C), Gleason-Pfauter Maschinenfabrik GmbH, Ludwigsburg, Germany
// All rights reserved. No part of this document may be reproduced or disclosed
// without in any form the written permission of Gleason-Pfauter.
//------------------------------------------------------------------------------
//
// PROJECT   : SIN840D
// $Workfile:   ParseFile.cpp  $
//
// ABSTRACT:  Implementation of the CParseFile class.
// 
//------------------------------------------------------------------------------
//
// Modification History
// $Log:   Q:/pvcs.db/archives/UIPrototype/ParseFile.cpp-arc  $
// 
//    Rev 1.2   09 Oct 2001 10:44:50   GeH
// Changes for using UNICODE character encoding 
// 
//    Rev 1.1   Oct 10 2000 14:14:30   top
// Using now GPMVERIFY instead of VERIFY and ASSERT
// 
//    Rev 1.0   Oct 05 2000 16:41:36   GeH
// Initial revision.
//
*******************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "SmxRes.h"
#include "ParseFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//***************************************************************************
// Function         CParseFile::CParseFile
// mfunc            Constructor
//***************************************************************************
CParseFile::CParseFile()
{
  m_bError   = false;
  m_pBuf     = NULL;
  m_pChar    = NULL;
  m_eComment = com_SemiColon;
  m_nLine    = 0;
  m_pGetLine = 0;
}

//***************************************************************************
// Function         CParseFile::~CParseFile
// @mfunc           Destructor
//***************************************************************************
CParseFile::~CParseFile()
{

}

//***************************************************************************
// Function         CParseFile::Open
// @mfunc           Open the given file for parsing.
// @parm            The filename of the file to be parsed.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CParseFile::Open(LPCTSTR pszFileName)
{
  m_strFileName = pszFileName;

  if (!m_strFileName.IsEmpty())
    return m_File.Open(m_strFileName, CFile::modeRead | CFile::typeText) != 0;

  return false;
}

//***************************************************************************
// Function         CParseFile::Close
// @mfunc           Close a previously opened file.
//***************************************************************************
void CParseFile::Close()
{
  m_File.Close();
}

//***************************************************************************
// Function         CParseFile::FillBufferIfEmpty
// @mfunc           Read the next line if the buffer for parsing is empty.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CParseFile::FillBufferIfEmpty()
{
  while (LineIsEmpty())
  {
    if (m_pGetLine)
    {
      if (!(*m_pGetLine)(m_strLine, m_dwGetLine))
        return false;
    }
    else  if  (!m_File.ReadString(m_strLine))
    {
      return false;
    }
    m_nLine++;
    m_pBuf = m_strLine.GetBuffer(0);
    m_pChar = m_pBuf;
  }

  return true;
}

//***************************************************************************
// Function         CParseFile::CheckChar
// @mfunc           Check if the next nonblank character is the character c.
// @parm            Character to be checked.
// @rdesc           true if the next character is 'c', otherwise false.
//***************************************************************************
bool CParseFile::CheckChar(TCHAR c)
{
  if  (!FillBufferIfEmpty())
    return false;

  if (m_bError || !m_pBuf || !m_pChar)
    return false;

  SkipWS();
  
  if (*m_pChar == c)
  {
    m_pChar++;
    return true;
  }
  else
  {
    m_strError.Format(_T("Character '%c' expected!"), c);
    return false;
  }
}

bool CParseFile::UngetChar()
{
  if (m_pChar == m_pBuf)
    return false;

  m_pChar--;
  return true;
}

//***************************************************************************
// Function         CParseFile::GetIdent
// @mfunc           Look for an identifier.
// @parm            OUT: the found identifier.
// @rdesc           true if an identifier could be parsed.
//***************************************************************************
bool CParseFile::GetIdent(CString& str)
{
  TCHAR *pChar;

  if  (!FillBufferIfEmpty())
    return false;

  if (m_bError || !m_pBuf || !m_pChar)
    return false;

  SkipWS();

  pChar = m_pChar;

  if (!_istalpha(*pChar))
  {
    m_strError.LoadString(IDS_ParseErr_Identifier);
    m_bError = true;
    return false;
  }

  while (_istalnum(*pChar) || *pChar == '_')
    pChar++;

  TCHAR c = *pChar;
  *pChar = 0;
  str = m_pChar;
  *pChar = c;
  m_pChar = pChar;

  return true;
}

//***************************************************************************
// Function         CParseFile::CheckString
// @mfunc           Check if the next string is the string szString.
// @parm            The string to be looked for.
// @rdesc           true if the given string was found.
//***************************************************************************
bool CParseFile::CheckString(LPCTSTR lpszString)
{
  if  (!FillBufferIfEmpty())
    return false;

  if (m_bError || !m_pBuf || !m_pChar)
    return false;

  TCHAR *pChar = m_pChar;

  SkipWS();

  if (_tcsncmp(m_pChar, lpszString,_tcslen(lpszString)))
  {
    CString strFormat;
    strFormat.LoadString(IDS_ParseErr_StringNotFound_1);
    m_strError.Format(strFormat, lpszString);
    return false;
  }
  
  pChar += _tcslen(lpszString);

  m_pChar = pChar;
  return true;
}

//***************************************************************************
// Function         CParseFile::GetNumber
// @mfunc           Look for a number.
// @parm            OUT: the parsed number.
// @rdesc           true if a number could be parsed.
//***************************************************************************
bool CParseFile::GetNumber(UINT& nNumber)
{
  if  (!FillBufferIfEmpty())
    return false;

  if (m_bError || !m_pBuf || !m_pChar)
    return false;

  bool  bResult = true;
  TCHAR *pChar;   
  SkipWS();

  pChar = m_pChar;
  
  while (_istdigit(*pChar))
  {
    pChar++;
  }

  if (pChar == m_pChar)
  {
    //Could not find a valid character.
    m_strError.LoadString(IDS_ParseErr_NoDigit);
    m_bError = true;
    nNumber  = 0;
    bResult  = false;
  }
  else
  {
    TCHAR c  = *pChar;
    *pChar  = TCHAR('\0');
    nNumber = _ttoi(m_pChar);
    *pChar  = c;
    m_pChar = pChar;
  }

  return bResult;
}

//***************************************************************************
// Function         CParseFile::GetNumber
// @mfunc           Look for a number.
// @parm            OUT: the parsed number.
// @rdesc           true if a number could be parsed, otherwise false.
// @comm Exception  
//***************************************************************************
bool CParseFile::GetNumber(WORD& nNumber)
{
  UINT nValue = 0;
  if  (GetNumber(nValue))
  {
    nNumber = (WORD) nValue;
    return true;
  }
  
  return false;
}


//***************************************************************************
// Function         CParseFile::SkipWS
// @mfunc           Skip white spaces.
//***************************************************************************
void CParseFile::SkipWS()
{
  if (m_bError || !m_pBuf || !m_pChar)
    return;

  while (_istspace(*m_pChar) || *m_pChar == TCHAR('\t'))
    m_pChar++;

  //Check if the rest of the line is a comment
  if (m_eComment == com_SemiColon && *m_pChar == ';')
    SkipToEOL();

  if (m_eComment == com_CPlusPlus && *m_pChar == '\\' && *(m_pChar+1) == '\\')
    SkipToEOL();
}

//***************************************************************************
// Function         CParseFile::CheckEOL
// @mfunc           Check if the string is finished.
// @rdesc           true if the string id finished (does contain only 
//                  white spaces after the current position.
//***************************************************************************
bool CParseFile::CheckEOL()
{
  if (m_bError)
    return false;

  SkipWS();

  if (*m_pChar == TCHAR('\0'))
    return true;
  else
  {
    m_strError.LoadString(IDS_ParseErr_ExpectingEOL);
    m_bError   = true;
    return false;
  }
}

//***************************************************************************
// Function         CParseFile::CheckWS
// @mfunc           Check if the next character is a white space.
// @rdesc           true if the next character is a white space.
//***************************************************************************
bool CParseFile::CheckWS()
{
  if (m_bError || !m_pBuf || !m_pChar)
    return false;

  if ( isspace(*m_pChar) || *m_pChar == TCHAR('\t') || *m_pChar == TCHAR('\0'))
    return true;
  else
  {
    m_strError.LoadString(IDS_ParseErr_ExpectingWS);
    return false;
  }
}


//***************************************************************************
// Function         CParseFile::ResetError
// @mfunc           Reset the parsing error.
//***************************************************************************
void CParseFile::ResetError()
{
  m_bError = false;
  m_strError.Empty();
}

//***************************************************************************
// Function         CParseFile::SetBuffer
// @mfunc           Set the buffer to be parsed.
// @parm            The buffer to be parsed.
//***************************************************************************
void CParseFile::SetBuffer(TCHAR *pBuf) 
{ 
  m_pBuf = pBuf; 
  m_pChar = pBuf; 
}

//***************************************************************************
// Function         CParseFile::LineIsEmpty
// @mfunc           Check if the line is empty.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CParseFile::LineIsEmpty()
{
  SkipWS();

  if (!m_pChar || *m_pChar == TCHAR('\0'))
    return true;
  else
    return false;
}

//***************************************************************************
// Function         CParseFile::CheckEOF
// @mfunc           Check if EOF is reached.
// @rdesc           true if the next character is EOF, otherwise false.
//***************************************************************************
bool CParseFile::CheckEOF()
{
  if (!FillBufferIfEmpty())
    return true;

  return false;
}

//***************************************************************************
// Function         CParseFile::CheckSection
// @mfunc           Find the section with the given section name. The parsed 
//                  file is searched only from the current position!!
// @parm            The section name.
// @rdesc           true if the section was found, otherwise false.
//***************************************************************************
bool CParseFile::CheckSection(LPCTSTR pszSection)
{
  CString strSection;

  if (m_bError)
    return false;

  strSection.Format(_T("[%s]"), pszSection);
  while (!CheckEOF())
  {
    if (CheckString(strSection) && CheckEOL())
      return true;

    SkipToEOL();
  }

  return false;
}

//***************************************************************************
// Function         CParseFile::SkipToEOL
// @mfunc           Skip the rest of this line.
//***************************************************************************
void CParseFile::SkipToEOL()
{
  m_pChar += _tcslen(m_pChar);     //Skip to end of line
}

//***************************************************************************
// Function         CParseFile::GetErrorStr
// @mfunc           Return a string which describes the last occured error.
// @rdesc           The last occured error string.
//***************************************************************************
LPCTSTR CParseFile::GetErrorStr() 
{ 
  CString str = m_strError;
  m_strError.Format(_T("(%d, %d) %s"), m_nLine, GetCol(), str); 
  return m_strError;
}

//***************************************************************************
// Function         CParseFile::SetError
// @mfunc           Set/clear the error string of the parser.
// @parm            true if the error state should be set.
//***************************************************************************
void CParseFile::SetError(bool bError)
{
  m_bError = bError;
}

//***************************************************************************
// Function         CParseFile::CheckVersion
// @mfunc           Check if the actual version number is between Min and Max version.
//                  The version is an entry like _T("Version = 3") in the file to be parsed.
// @parm            OUT: The version number.
// @parm            Minimum version supported.
// @parm            Maximum supported version.
// @parm            String identifying the version, usually "Version".
// @rdesc           true if the version number is between min and max.
//***************************************************************************
bool CParseFile::CheckVersion(WORD& nActVersion, WORD nMinVersion, WORD nMaxVersion, LPCTSTR pszVersion)
{
  if (!CheckString(pszVersion) || !CheckChar(TCHAR('=')))
  {
    SetError(true);
    return false;
  }

  if (GetNumber(nActVersion))
  {
    if (!(nActVersion >= nMinVersion && nActVersion <= nMaxVersion))
    {
      CString strFormat;
      strFormat.LoadString(IDS_ParseErr_InvalidVersionNumber);
      m_strError.Format(strFormat, nActVersion);
      m_bError = true;
    }
  }
  else
  {
    m_strError.LoadString(IDS_ParseErr_CouldNotReadVersionNumber);
    m_bError = true;    
  }

  return !m_bError;
}

//***************************************************************************
// Function         CParseFile::GetChar
// @mfunc           Skip the next character if it is the requested one, otherwise
//                  set the error state and return false;
// @parm            The next expected character.
// @rdesc           true if successful, otherwise false.
//***************************************************************************
bool CParseFile::GetChar(TCHAR c)
{
  if (m_bError)
    return false;

  if (!CheckChar(c))
    SetError(true);

  return !m_bError;
}

bool CParseFile::GetXMLText(CString& str)
{
  TCHAR *pStart;
  TCHAR cTemp;
  str.Empty();
  while (true)
  {
    if  (!FillBufferIfEmpty())
      return false;

    if (m_bError || !m_pBuf || !m_pChar)
      return false;

    pStart = m_pChar;
    while (*m_pChar  && *m_pChar != TCHAR('<'))
      m_pChar++;
      
    cTemp = *m_pChar;
    *m_pChar = 0;
    str += pStart;
    *m_pChar = cTemp;

    if (*m_pChar == TCHAR('<'))
      break;
  }
  return true;
}