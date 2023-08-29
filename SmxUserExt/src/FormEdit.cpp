// FormEdit.cpp : implementation file
//

#include "stdafx.h"
#include <errno.h>
#include <locale.h>
#include <float.h>
#include "FormEdit.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UNDEF_VAL -1

TCHAR  CFormEdit::sm_cDeciSy = TCHAR('\0');
TCHAR  CFormEdit::sm_cGrpSy  = TCHAR('\0');
short  CFormEdit::sm_nGrpNo  = UNDEF_VAL;

namespace
{
  const TCHAR gcPlus = TCHAR ('+');
  const TCHAR gcMinus = TCHAR ('-');
}

CFormEdit::CFormEdit()
{
	if (sm_cDeciSy == TCHAR('\0'))
	{
		//Get default values from the Registry
		TCHAR buf[64];
		//Get the decimal symbol
		if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, buf, 64))
		{
			sm_cDeciSy = *buf;
		}
		else
		{
			sm_cDeciSy = TCHAR(',');
		}

		//Get the grouping symbol
		if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, buf, 64))
		{
			sm_cGrpSy = *buf;
		}
		else
		{
			sm_cGrpSy = '.';
		}

		//Get the number of digits in group.
		if(GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, buf, 64))
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

				m_nGrpNo = (short) atoi((const char*) buf);
		}
		else
		{
			m_nGrpNo = 3;
		}
	}

	dbValue = 0.0;
	nValue  = 0;
	m_nIntNo  = UNDEF_VAL;
	m_nDeciNo = 2;
	m_nGrpNo  = sm_nGrpNo;
  m_dbConvFact = 1.0;
}

CFormEdit::~CFormEdit()
{
}


BEGIN_MESSAGE_MAP(CFormEdit, CEdit)
	//{{AFX_MSG_MAP(CFormEdit)
  ON_WM_KEYDOWN()
  ON_WM_KILLFOCUS()
  ON_WM_SETFOCUS()
  ON_WM_CHAR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CFormEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  
  switch(nChar) 
  {
    case VK_ESCAPE:
      SetWindowText(m_strOldText);
      m_strText = m_strOldText;
      SetSel(0,0);        //don't select any text
      return;

    case VK_DELETE:
      if (!IsDeletionAllowed ())
      {
        return;
      }
      break;
    case VK_INSERT:
      if(GetKeyState(VK_SHIFT) < 0)
      {
        //The user wants to insert text, check if the text is valid
        if(!CheckClipboardData())
          return;
      }

    default:
      ;
  }
  
  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
  GetWindowText(m_strText);
}

void CFormEdit::OnKillFocus(CWnd* pNewWnd) 
{
  FormatInput();
	CEdit::OnKillFocus(pNewWnd);
}

void CFormEdit::OnSetFocus(CWnd* pOldWnd) 
{
  CEdit::OnSetFocus(pOldWnd);

  GetWindowText(m_strText);
  GetWindowText(m_strOldText);
}

void CFormEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  if(!IsValidChar(nChar))
    return;

	if (m_nDeciNo == 0)
	{
    if(MaxLenReached())
      return;
    EnterChar(nChar, nRepCnt, nFlags);
	}
	else
	{
    if(_istdigit(nChar) || nChar == VK_BACK || nChar == gcPlus || nChar == gcMinus)
    {
      if(_istdigit(nChar))
      {
        if(MaxLenReached())
          return;
      }
      EnterChar(nChar, nRepCnt, nFlags);
      return;
    }

    if (nChar == static_cast<UINT>(sm_cDeciSy))
    {
      //find decimal symbol and jump cursor
      int idx = m_strText.Find(sm_cDeciSy);
      if(idx == -1) 
      {
        //let user enter the symbol
        EnterChar(nChar, nRepCnt, nFlags);
      }
      else
      {
        SetSel(idx+1, idx+1);
      }
    }
    else
    {
      //All possibilities handled
      VERIFY (FALSE);
    }
	}

  //store the text
  GetWindowText(m_strText);
}

bool CFormEdit::IsValidChar(int nChar)
{
  if(nChar == VK_BACK)      //backspace
    return true;

  if (_istdigit(nChar))
  {
    return true;
  }

  if ((nChar == gcPlus) || (nChar == gcMinus))
  {
    // only allowed if at the first position in the edit control
    int nStart, nEnd;
    GetSel(nStart, nEnd);

    if (nStart != 0)
      return false;

    //Check if a + or - is the next character
    if (m_strText.GetLength() > nEnd  && (m_strText[nEnd] == gcPlus || m_strText[nEnd] == gcMinus))
      return false;

    return true;
  }

	// for float also decimal separator allowed
  if (m_nDeciNo > 0)
  {
    return (nChar == sm_cDeciSy);
  }
  else
  {
    return false;
  }
}

bool CFormEdit::MaxLenReached()
{
  bool bMaxLen = false;
  int nStart, nEnd;
  int idx;

  GetSel(nStart, nEnd);
  if(nStart != nEnd)
    return false;        //replace more caracters with one possible

  if (m_nDeciNo == 0)
	{
    if(m_nIntNo != -1 && m_strText.GetLength() >= m_nIntNo && m_strText.GetLength() == nStart)
      bMaxLen = true;
	}
	else
	{
    idx = m_strText.Find(sm_cDeciSy);
    if(idx != -1)
    {
      //we have got a decimal point check cursor position
      if (nStart == idx && m_nIntNo != -1 && GetIntNumber(m_strText) >= m_nIntNo)
      {
        bMaxLen = true;
      }
      else if(nStart > idx && m_nDeciNo != -1 && GetDeciNumber(m_strText) >= m_nDeciNo 
              && m_strText.GetLength() == nStart)
      {
        bMaxLen = true;
      }
    }
    else
    {
      //no decimal point, check number of digits before point
      if (m_nIntNo != -1 && GetIntNumber(m_strText) >= m_nIntNo && m_strText.GetLength() == nStart)
        bMaxLen = true;
    }
	}
	return bMaxLen;
}

short CFormEdit::GetIntNumber(const CString strText)
{
  //there is no validation check done here!!
  short len = 0;
  for(int i=0; i<strText.GetLength(); i++)
  {
    //GPMVERIFY(IsValidChar(strText[i]));
    TCHAR cChar = strText[i];
    if(cChar == TCHAR('\0'))
      break;
    if(cChar == sm_cDeciSy )
      break;
    if(cChar == sm_cGrpSy || cChar == gcPlus || cChar == gcMinus)
      continue;

    len++;
  }
  return (len);

}

short CFormEdit::GetDeciNumber(const CString strText)
{
  short len = 0;
  int idx = strText.Find(sm_cDeciSy);
  if(idx != -1)
  {
    len = (short) (strText.GetLength() -idx -1);
  }

  return len;
}

bool CFormEdit::IsOverwritePos()
{
  int nStart, nEnd;
  GetSel(nStart, nEnd);

  if(nStart != nEnd)
    return false;
  
  if(nStart < m_strText.GetLength() && m_strText[nStart] == sm_cGrpSy)
  {
    nStart++;               //if the cursor is on the grouping symbol, skip this one
    SetSel(nStart, nStart); 
  }
  
  if (m_nDeciNo == 0)
	{
		return (nStart != m_strText.GetLength());
	}
	else
	{
    int idx = m_strText.Find(sm_cDeciSy);
    if(idx == -1)
    {
      return (nStart != m_strText.GetLength());
    }
    else
    {
      return (nStart != idx && nStart != m_strText.GetLength());
    }
	}

	return true;
}

void CFormEdit::Overwrite(TCHAR cChar)
{
  int nStart, nEnd;
  GetSel(nStart, nEnd);

  ASSERT(nStart == nEnd);

  m_strText.SetAt(nStart, cChar);
  SetWindowText(m_strText);
  SetSel(nStart+1, nStart+1);
  
}

void CFormEdit::EnterChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if(IsOverwritePos() && nChar != VK_BACK && nChar != VK_DELETE && nChar != gcMinus && nChar != gcPlus)
    Overwrite((TCHAR)nChar);
  else
  {
    // check if a decimal separator has been selected
    int nStartChar, nEndChar, nDecimalSymbolPos;
    bool bMultipleSelection = false;
    bool bSelectionContainsDecimalSymbol = false;
    GetSel (nStartChar, nEndChar);
    
    bMultipleSelection = (nStartChar != nEndChar);
    nDecimalSymbolPos = m_strText.Find (sm_cDeciSy);
    // check if the decimal symbol is being selected
    bSelectionContainsDecimalSymbol = (nDecimalSymbolPos >= nStartChar) && (nDecimalSymbolPos < nEndChar);
    
    if (nDecimalSymbolPos != -1)
    {
      CString strReplaceChar;
      if (nChar == VK_BACK)
      {
        if (bMultipleSelection)
        {
          if (bSelectionContainsDecimalSymbol)
          {
            strReplaceChar = sm_cDeciSy;
            ReplaceSel (strReplaceChar);      
            return;
          }
        }
        else
        {
          if (nStartChar > 0 && m_strText[nStartChar-1] == sm_cDeciSy) 
          {
            // do nothing, we are behind a separator
            return; 
          }
        }
      }
      else if (!_istcntrl (nChar))
      {
        // decimal separator will be ignored before, so we have not to take them into account
        if (bSelectionContainsDecimalSymbol && bMultipleSelection)
        {
          if (m_strText[nStartChar] == sm_cDeciSy)
          {
            strReplaceChar = sm_cDeciSy;

            strReplaceChar += (TCHAR) nChar;
            ReplaceSel (strReplaceChar);      
            return;
          }
          else
          {
            CString strText;

            strReplaceChar = (TCHAR) nChar;
            strReplaceChar += sm_cDeciSy;

            ReplaceSel (strReplaceChar);      
            
            // set the caret before the decimal point
            GetWindowText (strText);
            nDecimalSymbolPos = strText.Find (sm_cDeciSy);
            if (nDecimalSymbolPos != -1)
            {
              SetSel (nDecimalSymbolPos, nDecimalSymbolPos, TRUE);
            }
            return;
          }
        }

      }
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
  }
}

bool CFormEdit::IsDeletionAllowed ()
{
  bool bRet = true;
  if (m_nDeciNo != 0)
  {
    // check if a decimal separator has been selected
    int nStartChar, nEndChar, nDecimalSymbolPos;
    bool bMultipleSelection = false;
    bool bSelectionContainsDecimalSymbol = false;
    GetSel (nStartChar, nEndChar);
  
    bMultipleSelection = (nStartChar != nEndChar);
    nDecimalSymbolPos = m_strText.Find (sm_cDeciSy);
    // check if the decimal symbol is being selected
    bSelectionContainsDecimalSymbol = (nDecimalSymbolPos >= nStartChar) && (nDecimalSymbolPos < nEndChar);
    
    if (nDecimalSymbolPos != -1)
    {
      if (bMultipleSelection)
      {
        if (bSelectionContainsDecimalSymbol)
        {
          CString strReplaceChar = sm_cDeciSy;
          ReplaceSel (strReplaceChar);      
          bRet = false;
        }
      }
      else
      {
        if ((m_strText.GetLength() > nStartChar)  && m_strText[nStartChar] == sm_cDeciSy) 
        {
          // do nothing, we are behind a separator
          bRet = false;
        }
      }
    }
  }

  return bRet;
}

void CFormEdit::ReplaceSymbol(TCHAR nFrom, TCHAR nTo)
{
  
  if(nFrom == nTo)
    return;
  for(int i=0; i< m_strText.GetLength(); i++)
  {
    if(m_strText[i] == nFrom)
      m_strText.SetAt(i, nTo);
  }
}

void CFormEdit::FormatInput()
{
  bool bResult = true;
  bool bDefault = false;
  if(!::IsWindow(m_hWnd))
      return;

  CString strVal;
  GetWindowText(strVal);
  double dbVal = 0.0;

  if(m_nDeciNo != 0)
  {
    if(strVal.IsEmpty() || (dbVal = String2Double(strVal)) != DBL_MAX)
    {
       DoubleToString(strVal, dbVal);
    }
  }

  if(bResult)
  {
	  SetWindowText(strVal);
  }
}


bool CFormEdit::CheckClipboardData()
{
  HGLOBAL handle = 0;
  bool    bResult = true;

  if(!OpenClipboard())
  {
    CWnd *pWnd = GetClipboardOwner();
    return false;
  }

#ifdef _UNICODE
  handle = GetClipboardData(CF_UNICODETEXT);
#else
  handle = GetClipboardData(CF_TEXT);

  if (handle == 0)
    handle = GetClipboardData(CF_OEMTEXT);
#endif

  // if CF_TEXT clipboard format found
  if (handle)
  {
    LPTSTR psz = (LPTSTR) GlobalLock(handle);
    DWORD size = GlobalSize(handle);

    for(DWORD i=0; i < size && *psz;  psz++, i++)
    {
      if(!IsValidChar(*psz))
      {
        bResult = false;
        goto cleanup;
      }
    }
  }

cleanup:
  CloseClipboard();
  return bResult;
}

void CFormEdit::DoubleToString(CString& str, double dVal)
{
  if(m_nDeciNo == UNDEF_VAL)
    str = Double2String(dVal, 6, m_nGrpNo);
  else 
  {
    str = Double2String(dVal, m_nDeciNo, m_nGrpNo);
  }
}

void CFormEdit::FormatString(CString& strValue)
{
  CString strInt, strDeci;
  CString str =  strValue;
  
  //split the text into integer and decimal part
  int idx = str.Find(_T('.'));
  if(idx == -1) 
  {
    strInt = str;
    if(strInt == "")
    {
      strInt = _T('0');     //default
    }

    strDeci = sm_cDeciSy;
  }
  else
  {
    strInt = str.Left(idx);
    strDeci = str.Mid(idx);
    strDeci.SetAt(0, sm_cDeciSy);
  }
  
  //No decimal part ? delete the decimal symbol
  if(m_nDeciNo == 0)
    strDeci = _T("");

  if(strInt == _T(""))
    strInt = _T('0');       //default value

  InsertGrpSy(strInt);

  strValue = strInt + strDeci;
}

void CFormEdit::InsertGrpSy(CString& strInt)
{
  if(strInt == "")
    return;

  int nIntLen    = strInt.GetLength();      //length of input string without grpSy
  int nGrpIntLen = strInt.GetLength();      //length of input string without grpSy and +/- characters
  
  if (strInt[0] == TCHAR('-') || strInt[0] == TCHAR('+'))
    nGrpIntLen--;

  int nGrp;
  if(m_nGrpNo >0)
  {
    nGrp = nGrpIntLen / m_nGrpNo;      //number of group symbols to add
    if(nGrpIntLen % m_nGrpNo == 0)
      nGrp--;
  }
  else 
  {
    nGrp = 0;
    return;
  }


  ASSERT(nIntLen + nGrp > 0);
  WCHAR *buf = (WCHAR*) calloc(nIntLen + nGrp + 3, sizeof(WCHAR));

  //insert the grouping symbol
  int i=strInt.GetLength()-1;         //index in strLen
  int n = nIntLen + nGrp -1;
  int cnt = 0;
  buf[n+1] = '\0';

  for( ; n >= 0; n--)
  {
    if(cnt == m_nGrpNo && nGrp > 0) 
    {
      buf[n] = sm_cGrpSy;
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
  free( buf);
}

bool CFormEdit::StringToDouble(double& dbValue, LPCTSTR strValue)
{
  bool    bResult = true;
  ASSERT(false);

  CString str     = strValue;

  
  if (!str.IsEmpty ())
  {
    //RestoreSymbols(str);

    // Check if the string starts with "+." or "-."; if yes, change it to "+0." resp "-0." otherwise 
	 // the string can not be converted.
    if (str.GetLength() >= 2 && str[1] == TCHAR('.') && (str[0] == TCHAR('+') || str[0] == TCHAR('-')))
    {
      CString strTemp = TCHAR('0') + str.Mid(1);
      str = strTemp + str.Mid(1);
    }

    // convert the string into a double value
    dbValue = _tcstod((LPCTSTR) str, NULL);

    // check for incrorrect conversion
    if (errno == ERANGE)
    {  
      dbValue = 0.0;
			return false;
    }  
  }
  return bResult;
}

void CFormEdit::RestoreSymbols(CString& str)
{
  int   j = 0;
  int   n = str.GetLength();
  TCHAR c = 0;

  for(int i = 0; i < n; i++)
  {
    c = str[i];
    if(c != sm_cGrpSy) 
    {
      if(c == sm_cDeciSy)
        str.SetAt(j++, TCHAR('.'));
      else
        str.SetAt(j++, c);
    }
  }
  str = str.Left(j);
  return;
}

double CFormEdit::GetValue()
{
	dbValue = 0.0;
	if (m_hWnd)
    GetWindowText(m_strText);
	dbValue = String2Double(m_strText);
  dbValue *= m_dbConvFact;
	return dbValue;
}

int CFormEdit::GetIntValue()
{
	dbValue = 0.0;
	GetWindowText(m_strText);
	dbValue = String2Double(m_strText);
  dbValue *= m_dbConvFact;
	return (int) (dbValue + SGN(dbValue) * 0.5);
}

void CFormEdit::SetValue(double dbValue)
{
  dbValue /= m_dbConvFact;
	DoubleToString(m_strText, dbValue);
	if (m_hWnd)
		SetWindowText(m_strText);
}

void CFormEdit::SetIntNumber(short nIntNo)
{ 
  ASSERT(nIntNo >= -1); 
  m_nIntNo = nIntNo;
}

void CFormEdit::SetDeciNumber(short nDeciNo) 
{
  if(nDeciNo < -1)
  {
    nDeciNo = 0;
  }
  m_nDeciNo = nDeciNo;
}

void CFormEdit::EnableGrpSymbol(bool bEnable) 
{
	if (bEnable)
		m_nGrpNo = sm_nGrpNo;
	else
		m_nGrpNo = 0;
}

void CFormEdit::SetGrpNumber(short nGrpNo)
{
  m_nGrpNo = nGrpNo;
}

void CFormEdit::PreSubclassWindow() 
{
	CEdit::PreSubclassWindow();
  PostMessage(WM_SETTEXT, 0, ((LPARAM)((LPCTSTR) m_strText)));
  //SetWindowText(m_strText);	
}

void CFormEdit::OnDestroy() 
{
	CEdit::OnDestroy();
	
	GetWindowText(m_strText);
	
}
