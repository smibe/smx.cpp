/*******************************************************************************
// Copyright (C), Gleason-Pfauter Maschinenfabrik GmbH, Ludwigsburg, Germany
// All rights reserved. No part of this document may be reproduced or disclosed
// without in any form the written permission of Gleason-Pfauter.
//------------------------------------------------------------------------------
//
// PROJECT   : SIN840D
// $Workfile:   ParseFile.h  $
//
// ABSTRACT:   Interface for the CParseFile class.
//
//------------------------------------------------------------------------------
//
// Modification History
// $Log:   Q:/pvcs.db/archives/UIPrototype/ParseFile.h-arc  $
* 
*    Rev 1.0   Oct 05 2000 16:41:48   GeH
* Initial revision.
//
*******************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_PARSEFILE_H__D8367257_946D_11D4_B093_00016301B91D__INCLUDED_)
#define AFX_PARSEFILE_H__D8367257_946D_11D4_B093_00016301B91D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef bool (*GetLineFuncPtr)(CString& str, DWORD& dwGetLine);

//***************************************************************************
// @class CParseFile |
//
//  Class used for parsing a file. Actually this is a simple lexical analysis 
//  of the given file. 
//
//  If the parser encounters an error, all following parsing function will
//  parse the file any more, this means that parsing stops if an error occured.
//
// Generally all Check-functions will not set the error state but if the requested 
// item is not found, an error string will be remembered which can be displayed
// by the calling function. You can use SetError() to set or clear the error 
// state of the parser.
// 
//***************************************************************************
class CParseFile  
{
public:
	CParseFile();                           //@cmember Constructor.
	virtual ~CParseFile();                  //@cmember Destructor.

  //@cmember,menum Type of the comment in the parsed file.
  enum EComment
  {
    com_CPlusPlus = 1,                    //@cmember C++ style comments (Rest of the line after // wil be ignored).
    com_SemiColon = 2                     //@cmember All characters after ';' will be ignored.
  };

  void    SetComment(EComment eComment) { m_eComment = eComment; };   //@cmember Set the comment type.
  bool    CheckVersion(WORD& nActVersion, WORD nMinVersion, 
                       WORD nMaxVersion, LPCTSTR pszVersion);         //@cmember Check if the actual version is between Min and MAx.
  bool    Open(LPCTSTR pszFileName);                                  //@cmember Open a file for parsing.
  void    Close();                                                    //@cmember Close the paRSED FILE:
  bool    CheckSection(LPCTSTR szSection);                            //@cmember Look for the requested section name in the file to be parsed.
  bool    CheckEOF();                                                 //@cmember Check if the file is finished.
  bool    CheckChar(TCHAR c);                                         //@cmember Check if the next character is c.
  bool    UngetChar();
  bool    GetIdent(CString& str);                                     //@cmember Look for an identifier.
  bool    CheckString(LPCTSTR lpszString);                            //@cmember Check the next string.
  bool    GetNumber(UINT& nNumber);                                   //@cmember Look for a number.
  bool    GetNumber(WORD& nNumber);                                   //@cmember Look for a number.
  bool    GetChar(TCHAR c);                                           //@cmember Look for the requested character.
  void    SkipWS();                                                   //@cmember Skip white spaces.
  void    SkipToEOL();                                                //@cmember Skip to end of line
  bool    CheckEOL();                                                 //@cmember Check if the next character is EOL.
  bool    CheckWS();                                                  //@cmember Check if the next character is a white space.
  bool    GetError() { return m_bError; };                            //@cmember Return true if an error occured.
  LPCTSTR GetErrorStr();                                              //@cmember Return the error string.
  void    SetErrorStr(LPCTSTR pszError) { m_strError = pszError; }    //@cmember Set the error string for the parser
  int     GetCol() { return m_pChar - m_pBuf +1; };                   //@cmember Return the current column number
  void    ResetError();                                               //@cmember Reset the error.
  bool    LineIsEmpty();                                              //@cmember Check if the line is empty
  void    SetError(bool bError);                                      //@cmember Set or clear the error state.
  void    SetGetLineFunc(GetLineFuncPtr pFunc, DWORD dwGetLine) 
  { 
    m_pGetLine  = pFunc; 
    m_dwGetLine = dwGetLine;
  }
  bool    GetXMLText(CString& str);
  CString GetFileName() { return m_strFileName; }

protected:
  bool    FillBufferIfEmpty();                                        //@cmember Fill the buffer for parsing.
  void    SetBuffer(TCHAR *pBuf);                                     //@cmember Set the buffer for parsing.

protected:
  int        m_nLine;                                                 //@cmember Current line number.
  CString    m_strLine;                                               //@cmember The current line.
  CStdioFile m_File;                                                  //@cmember The currently parsed file.
  CString    m_strFileName;                                           //@cmember The file name of the currently parsed.
  EComment   m_eComment;                                              //@cmember The type of comments in the parsed file.
  bool       m_bError;                                                //@cmember true if an error occured.
  CString    m_strError;                                              //@cmember The error string.
  TCHAR     *m_pBuf;                                                  //@cmember The buffer holding the string to be parsed.
  TCHAR     *m_pChar;                                                 //@cmember The current position in the buffer.
  GetLineFuncPtr m_pGetLine;
  DWORD          m_dwGetLine;
};

#endif // !defined(AFX_PARSEFILE_H__D8367257_946D_11D4_B093_00016301B91D__INCLUDED_)
