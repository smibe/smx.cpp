/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: Crypt.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/Crypt.h,v 1.1 2002/09/20 09:37:07 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef __CRYPT_H___2867E822_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_
#define __CRYPT_H___2867E822_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_



class CCrypt
{
public:
  CCrypt() { srand( (unsigned) GetTickCount() ); }
  ~CCrypt() {};

  void EncryptDigits(CString& str, const CString& strPassword, int nLength);
  void DecryptDigits(CString& str, const CString& strPassword);

protected:
  bool StringToNumber(UINT *pOut, int nSize, const CString& strIn);
  bool NumberToString(CString& strOut, UINT *pIn, int nSize);
  int GetNofLoops(int nSize);

protected:

};

#endif // __CRYPT_H___2867E822_E4B9_11D1_9E30_204C4F4F5020__INCLUDED_
