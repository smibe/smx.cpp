/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DlgDataEx.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/DlgDataEx.cpp,v 1.2 2002/11/22 17:11:25 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "StdAfx.h"
#include "DlgDataEx.h"

//Forward function declarations
bool InsertString(CStringArray& strArray, CString str);

namespace
{
  LPCTSTR szCBEntry = _T("CBEntry");
  LPCTSTR szEmpty = _T("<<>>");
}

void DDX_CheckFlag(CDataExchange* pDX, int nIDC, DWORD& dwFlags, DWORD dwMask)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		if (::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) == 0)
      dwFlags &= ~dwMask;
    else
      dwFlags |= dwMask;
	}
	else
	{
		::SendMessage(hWndCtrl, BM_SETCHECK, (dwFlags & dwMask) == dwMask, 0L);
	}
}

void DDX_Check(CDataExchange* pDX, int nIDC, bool& bValue)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		bValue = ::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) != 0;
	}
	else
	{
		::SendMessage(hWndCtrl, BM_SETCHECK, (WPARAM)bValue, 0L);
	}
}

void DDX_RadioFlag(CDataExchange* pDX, int nIDC, DWORD& dwFlags, DWORD dwMask)
{
  HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		if (::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) == 0)
      dwFlags &= ~dwMask;
    else
      dwFlags |= dwMask;
	}
	else
	{
		::SendMessage(hWndCtrl, BM_SETCHECK, (dwFlags & dwMask) == dwMask, 0L);
	}
}

void DDX_CBStrings(CDataExchange *pDX, int nIDC, LPCTSTR pszSection, int nMaxStrings, bool bEmptyAllowed)
{
  CComboBox *pBox = (CComboBox *) CWnd::FromHandle(pDX->PrepareCtrl(nIDC));
  CString str;
  bool    bChange = false;
  int     i       = 0;

  CStringArray strArray;

  if (pDX->m_bSaveAndValidate)
  {
    strArray.SetSize(pBox->GetCount());
    //First get the strings from the combo box
    for (i=0; i < min(pBox->GetCount(), nMaxStrings); i++)
    {
      pBox->GetLBText(i, strArray[i]);
    }

    pBox->GetWindowText(str);
    
    //Now insert the selected string
    if (bEmptyAllowed || !str.IsEmpty())
      bChange = InsertString(strArray, str);
  }

  CString strKey;
  for (i=0; i < nMaxStrings; i++) 
	{
		strKey.Format(_T("%s%d"), szCBEntry, i);
 
    if (pDX->m_bSaveAndValidate)
    {
      //Store the strings into registry
      if (i < strArray.GetSize() && (bEmptyAllowed || !strArray[i].IsEmpty()))
        AfxGetApp()->WriteProfileString(pszSection, strKey, strArray[i]); 
    }
    else
    {
      str = AfxGetApp()->GetProfileString(pszSection, strKey, szEmpty); 

      if (str != szEmpty)
      {
        if (bEmptyAllowed || !str.IsEmpty())
          pBox->AddString(str);
      }
      else
      {
        str = AfxGetApp()->GetProfileString(pszSection, strKey); 
        if (!str.IsEmpty())
          pBox->AddString(str);
      }
    }
  }

  if (!bChange)
    pBox->SetCurSel(0);
}

CString DDX_GetLastCBEntry(LPCTSTR pszSection)
{
  CString strKey;
  CString strEntry;
  strKey.Format(_T("%s0"), szCBEntry);
  strEntry = AfxGetApp()->GetProfileString(pszSection, strKey); 
  return strEntry;
}

bool InsertString(CStringArray& strArray, CString str)
{
	int i;
	for(i=0; i< strArray.GetSize(); i++)
	{
		if(str == strArray[i])
		{
			if(i==0)
				return false;	//string is present and is first

			//string is in array but not first -> switch
			strArray[i] = strArray[0];
			strArray[0] = str;
			return true;
		}
	}

	//string not found, check if there is an empty string,
	//otherwise insert first
	/*
  for(i=0; i<strArray.GetSize(); i++)
	{
		if(strArray[i] == "")
		{
			if(i==0) {
				strArray[0] = str;
				return true;
			} else {
				strArray[i] = strArray[0];
				strArray[0] = str;
				return true;
			}
		}
	}
  */

  strArray.SetSize(strArray.GetSize() + 1);
	for(i=strArray.GetSize()-1; i>0; i--)
	{
		strArray[i] = strArray[i-1];
	}
	strArray[0] = str;
	return true;
}

