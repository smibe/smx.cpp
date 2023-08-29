/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: Util.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/Util.cpp,v 1.2 2002/11/22 17:21:29 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include <float.h>
#include <math.h>
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LPCTSTR GetResourceString(UINT nStrID)
{
  static CString str;
  VERIFY( str.LoadString(nStrID) );
  return str;
}

void GetFileName(CString& strFilePath)
{
  int nIdx = strFilePath.ReverseFind(TCHAR('\\'));
  if (nIdx != -1)
  {
    strFilePath = strFilePath.Mid(nIdx+1);
  }
}

void GetFileDir(CString& strFilePath)
{
  int nIdx = strFilePath.ReverseFind(TCHAR('\\'));
  if (nIdx != -1)
  {
    strFilePath = strFilePath.Left(nIdx);
  }
}

bool IsRelativePath(CString& str)
{
	return !((!str.IsEmpty() && str[0] == '\\') || (str.GetLength() > 1 && str[1] == ':'));
}

CString GetDirFromPath(const CString& strPath)
{
  int nIdx = strPath.ReverseFind('\\');

  if (nIdx != -1)
    return strPath.Left(nIdx);
  else
    return CString();
}

CString GetFileNameFromPath(const CString& strPath)
{
  int nIdx = strPath.ReverseFind('\\');

  if (nIdx != -1)
    return strPath.Mid(nIdx + 1);
  else
    return strPath;
}

void AddBackslashToDir(CString& strDir)
{
	if (strDir.IsEmpty())
    return;

  if(strDir[strDir.GetLength() - 1] != '\\')
		strDir += '\\';
}

bool FileExists(LPCTSTR pszFilePath)
{
  CFile File;
  if (File.Open(pszFilePath, CFile::modeRead))
  {
    File.Close();
    return true;
  }
  return false;
}

void CatStrings(CString& strLine, CStringArray& strStringArray, TCHAR cSep)
{
	strLine.Empty();
	for (int i=0; i < strStringArray.GetSize(); i++)
	{
		if (i > 0)
			strLine += cSep;

		strLine += strStringArray[i];
	}
}

void SplitString(CString& strLine, CStringArray& strStringArray, TCHAR cSep)
{
	TCHAR *pChar = strLine.GetBuffer(strLine.GetLength());
	TCHAR *pSubStr = pChar;
	int nCount = 0;
	int i      = 0;

	while (*pChar)
	{
		if (*pChar == cSep)
			nCount++;

		pChar++;
	}

	strStringArray.SetSize(nCount+1);
	pChar = pSubStr;

	while (*pChar)
	{
		if (*pChar == cSep)
		{
			*pChar = TCHAR('\0');
			strStringArray[i++] = pSubStr;
			*pChar = cSep;
			pSubStr = pChar + 1;
		}
		pChar++;
	}
	strStringArray[i] = pSubStr;
}

double String2Double(LPCTSTR pszValue)
{
  CString str = pszValue;

  if (!str.IsEmpty ())
  {
		TCHAR cGrpSy = '.';
    TCHAR buf[8];
    if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, buf, 8))
		{
			cGrpSy = *buf;
		}

    str.Remove(cGrpSy);

    // Check if the string starts with "+." or "-."; if yes, change it to "+0." resp "-0." otherwise 
	  // the string can not be converted.
    if (str.GetLength() >= 2 && str[1] == TCHAR('.') && (str[0] == TCHAR('+') || str[0] == TCHAR('-')))
    {
      CString strTemp = TCHAR('0') + str.Mid(1);
      str = strTemp + str.Mid(1);
    }
  }
  
  return atof(str);
}

TCHAR GetDeciSymbol()
{
	TCHAR cDeciSy = '.';
  TCHAR buf[8];

	if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buf, 8))
	{
		cDeciSy = *buf;
	}
  
  return cDeciSy;
}


TCHAR GetGrpSymbol()
{
	TCHAR cGrpSy = '.';
  TCHAR buf[8];

  if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, buf, 8))
	{
		cGrpSy = *buf;
	}
  
  return cGrpSy;
}

int GetGrpNumber()
{
	int nGrpNumber = 3;
  TCHAR buf[16];
	//Get the number of digits in group.
	if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, buf, 16))
	{
			LPTSTR c = buf;
			while(*c != TCHAR('\0')) 
			{
				if(*c== TCHAR(';')) 
				{
					*c = 0;
					break;
				}
				c++;
			}

			nGrpNumber = (short) atoi((const char*) buf);
	}
  return nGrpNumber;
}

void InsertGrpSymbols(CString& str)
{
  TCHAR cGrpSy = GetGrpSymbol();
  int   nGrpNumber = GetGrpNumber();

  CString strInt = str;
  int nIdx = str.ReverseFind(GetDeciSymbol());
  if (nIdx != -1)
    strInt = str.Left(nIdx);

  if(strInt.IsEmpty())
    return;

  int nIntLen    = strInt.GetLength();      //length of input string without grpSy
  int nGrpIntLen = strInt.GetLength();      //length of input string without grpSy and +/- characters
  
  if (strInt[0] == TCHAR('-') || strInt[0] == TCHAR('+'))
    nGrpIntLen--;

  int nGrp;
  if(nGrpNumber >0)
  {
    nGrp = nGrpIntLen / nGrpNumber;      //number of group symbols to add
    if(nGrpIntLen % nGrpNumber == 0)
      nGrp--;
  }
  else 
  {
    nGrp = 0;
    return;
  }


  ASSERT(nIntLen + nGrp > 0);
  USHORT *buf = new USHORT[nIntLen + nGrp + 3];

  //insert the grouping symbol
  int i=strInt.GetLength()-1;         //index in strLen
  int n = nIntLen + nGrp -1;
  int cnt = 0;
  buf[n+1] = '\0';

  for( ; n >= 0; n--)
  {
    if(cnt == nGrpNumber && nGrp > 0) 
    {
      buf[n] = cGrpSy;
      nGrp--;
      cnt = 0;
    }
    else
    {
      buf[n] = strInt[i--];
      cnt++;
    }
  }
  strInt = buf;
  
  if (nIdx != -1)
    str = strInt + str.Mid(nIdx);
  else
    str = strInt;
  delete [] buf;

}

CString Double2String(double dbValue, int nPrecision, int nGrpNumber)
{
  CString str;
  str.Format("%.*lf", nPrecision, dbValue);
  
  if (nGrpNumber != 0)
    InsertGrpSymbols(str);
  return str;
}


double Round(double dbValue, int nNofDecDigits)
{
  double dbTemp = dbValue * pow(10, nNofDecDigits);
  double dbRest = dbTemp - (int) dbTemp;
  dbTemp = (int) dbTemp;
  
  if (dbRest >= 0.5)
    dbTemp += 1;

  dbTemp /= pow(10, nNofDecDigits);
  return dbTemp;
}

LPCTSTR GetShellErrorString(DWORD dwError)
{
  ASSERT(dwError <= 32);

  switch (dwError)
  {
    case 0:
      return _T("The system is out of memory or resources.");
    case ERROR_BAD_FORMAT:
      return _T("The .exe file is invalid (non-Win32 .exe or error in .exe image).");
    case SE_ERR_ACCESSDENIED:
      return _T("The operating system denied access to the specified file.  ");
    case SE_ERR_ASSOCINCOMPLETE:
      return _T("The file name association is incomplete or invalid. ");
    case SE_ERR_DDEBUSY:
      return _T("The DDE transaction could not be completed because other DDE transactions were being processed. ");
    case SE_ERR_DDEFAIL:
      return _T("The DDE transaction failed. ");
    case SE_ERR_DDETIMEOUT:
      return _T("The DDE transaction could not be completed because the request timed out. ");
    case SE_ERR_DLLNOTFOUND:
      return _T("The specified dynamic-link library was not found.  ");
    case SE_ERR_FNF:
      return _T("The specified file was not found.  ");
    case SE_ERR_NOASSOC:
      return _T("There is no application associated with the given file name extension. ");
    case SE_ERR_OOM:
      return _T("There was not enough memory to complete the operation. ");
    case SE_ERR_PNF:
      return _T("The specified path was not found. ");
    case SE_ERR_SHARE:
      return _T("A sharing violation occurred. ");
    default:
      ;
  }
 
  return _T("An unknown error occured.");
}

