/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: Crypt.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/Crypt.cpp,v 1.2 2002/11/22 17:08:23 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include <stdafx.h>
#include "Crypt.h"

const int nBase = 37;
const int nSteps = 3;
const int aInsertPos[] = {0, 3, 5};

void CCrypt::EncryptDigits(CString& str, const CString& strPassword, int nLength)
{
  int nSize    = strPassword.GetLength();

  ASSERT(nLength > str.GetLength());
  //sort some characters into the string
  TCHAR c;
  int   nPos;
  int   nLoops = nLength - str.GetLength() - nSteps;
  for (int r = 0; r < nLoops; r++)
  {
    c = (TCHAR) ((rand() % 26) + TCHAR('A'));
    nPos = rand() % (str.GetLength() + 1);

    //insert the character
    str = str.Left(nPos) + c + str.Mid(nPos);
  }

  int nOutSize = str.GetLength() + nSteps;
  UINT *pPasswd = new UINT[strPassword.GetLength()];
  UINT *pBuf    = new UINT[nOutSize];

  VERIFY( StringToNumber(pPasswd, strPassword.GetLength(), strPassword) );
  VERIFY( StringToNumber(pBuf,    str.GetLength(), str) );

  for (int nStepIdx = 0; nStepIdx < nSteps; nStepIdx++)
  {
    int n = rand() % 26;

    int i = 0;
    int nIdx = 0;

    for (int k = 0; k < n; k++)
    {
      for (i = 0; i < nOutSize - nSteps + nStepIdx; i++)
      {
        pBuf[i] = (pPasswd[(nIdx + i) % nSize] + pBuf[i]) % nBase;
      }
      nIdx++;
  
      if (nIdx >= nSize)
        nIdx = 0;
    }
  
    //insert the number of executed loops.
    for (i = nOutSize - nSteps + nStepIdx; i > aInsertPos[nStepIdx]; i--)
    {
      pBuf[i] = pBuf[i-1];
    }
    
    pBuf[aInsertPos[nStepIdx]] = n;

  }

  VERIFY( NumberToString(str, pBuf, nOutSize) );

  delete [] pBuf;
  delete [] pPasswd;
}

void CCrypt::DecryptDigits(CString& str, const CString& strPassword)
{
  int nSize    = strPassword.GetLength();
  int nOutSize = str.GetLength();
  int i = 0;

  UINT *pPasswd = new UINT[strPassword.GetLength()];
  UINT *pBuf    = new UINT[str.GetLength()];

  VERIFY( StringToNumber(pPasswd, strPassword.GetLength(), strPassword) );
  VERIFY( StringToNumber(pBuf,    str.GetLength(), str) );

  for (int j = 0; j < nSize; j++)
  {
    pPasswd[j] = nBase - pPasswd[j];
  }

  for (int nStepIdx = nSteps - 1; nStepIdx >= 0; nStepIdx--)
  {
    int n = pBuf[aInsertPos[nStepIdx]];

    //remove the number of executed loops.
    for (i = aInsertPos[nStepIdx]; i < nOutSize - nSteps + nStepIdx; i++)
    {
      pBuf[i] = pBuf[i+1];
    }

    int nIdx = (n - 1) % nSize;
  
    for (int k = 0; k < n; k++)
    {
      for (i = 0; i < nOutSize - nSteps + nStepIdx; i++)
      {
        pBuf[i] = (pPasswd[(nIdx + i) % nSize] + pBuf[i]) % nBase;
      }
      nIdx--;
  
      if (nIdx < 0)
        nIdx = nSize - 1;
    }
  }

  VERIFY( NumberToString(str, pBuf, nOutSize - nSteps) );

  //Throw all nondigits away
  CString strTemp = str;
  str.Empty();
  for (i = 0; i < strTemp.GetLength(); i++)
  {
    if (TCHAR('0') <= strTemp[i] && strTemp[i] <= TCHAR('9'))
      str += strTemp[i];
  }

  delete [] pBuf;
  delete [] pPasswd;
}

bool CCrypt::StringToNumber(UINT *pOut, int nSize, const CString& strIn)
{
  int cChar = 0;
  for (int i = 0; i < nSize; i++)
  {
    cChar = toupper((int) strIn[i]);

    if (TCHAR('0') <= cChar && cChar <= TCHAR('9'))
      pOut[i] = cChar - TCHAR('0');
    else if (TCHAR('A') <= cChar && cChar <= TCHAR('Z'))
      pOut[i] = 10 + cChar - TCHAR('A');
    else if (TCHAR('#') == cChar)
      pOut[i] = nBase - 1;
    else
    {
      return false;
    }
  }
  return true;
}

bool CCrypt::NumberToString(CString& strOut, UINT *pIn, int nSize)
{
  int nChar = 0;
  LPTSTR pOut = strOut.GetBuffer(nSize + 1);
  pOut[nSize] = 0;

  for (int i = 0; i < nSize; i++)
  {
    nChar = pIn[i];

    if (0 <= nChar && nChar <= 9)
      pOut[i] = (TCHAR) nChar + TCHAR('0');
    else if (10 <= nChar && nChar <= 35)
      pOut[i] = (TCHAR) nChar + TCHAR('A') - 10;
    else if (nChar == 36)
      pOut[i] = TCHAR('#');
    else
    {
      strOut.ReleaseBuffer();
      return false;
    }
  }

  strOut.ReleaseBuffer();
  return true;
}


int CCrypt::GetNofLoops(int nSize)
{
  //return 1;
  return 2 * nSize + nSize / 2 + nSize % 5;
}
