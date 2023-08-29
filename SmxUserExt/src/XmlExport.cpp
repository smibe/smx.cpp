// XmlExport.cpp: implementation of the CXmlExport class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#define SMX_STRCODE
#include "XmlExport.h"
#undef SMX_STRCODE

TCHAR szIndent[] = _T("  ");

struct SXmlChar
{
  TCHAR   c;
  LPCTSTR szChar;
  BYTE    bDTD;
};

SXmlChar XmlChars[] = 
{
  { '{', "rbo", 1 },
  { '}', "rbc", 1 },
  { '/', "slsh", 1 },
  { '<', "lt", 0 },
  { '>', "gt", 0 },
  { '&', "amp", 0 },
  { '\'', "apos", 0 },
  { '"', "quot", 0 },
};

CXmlEntity::CXmlEntity(CXmlExport *pExp, LPCTSTR pszTag)
{
  m_pExp = pExp;
  m_strTag = pszTag;
}

CXmlEntity::~CXmlEntity()
{
  if (m_pExp)
    m_pExp->WriteEndTag(this);
}

void CXmlEntity::AddAttribute(LPCTSTR pszAttrib, LPCTSTR pszAttribVal)
{
  m_strAttrib.Add(pszAttrib);
  m_strAttribValue.Add(pszAttribVal);
}

void CXmlEntity::AddAttribute(LPCTSTR pszAttrib, CString strAttribVal)
{
  m_strAttrib.Add(pszAttrib);
  m_strAttribValue.Add(strAttribVal);
}

void CXmlEntity::SetText(LPCTSTR pszText)
{
  //Count < and > characters
  const TCHAR *p = pszText;
  int nCount = 0;
  int nLen = 0;
  bool bFound = false;
  while(*p)
  {
    bFound = false;
    if (*p <= 127 && !_istalnum(*p))
    {
      for (int i = 0; i < sizeof(XmlChars) / sizeof(SXmlChar); i++)
      {
        if (*p == XmlChars[i].c)
        {
          nCount += strlen(XmlChars[i].szChar) + 1;
          bFound = true;
          break;
        }
      }
    }
    
    if (bFound)
      break;

    if (*p > 127)
    {
      nCount += 4;
    }

    nLen++;
    *p++;
  }
  
  TCHAR *s = m_strText.GetBuffer(nLen + nCount + 1);
  p = pszText;
  while (*p)
  {
    bFound = false;
    if (*p <= 127 && !_istalnum(*p))
    {
      for (int i = 0; i < sizeof(XmlChars) / sizeof(SXmlChar); i++)
      {
        if (*p == XmlChars[i].c)
        {
          *s++ = '&';
          _tcscpy(s, XmlChars[i].szChar);
          s += strlen(XmlChars[i].szChar);
          *s++ = ';';
          bFound = true;
          break;
        }
      }
    }
    
    if (bFound)
      break;

    if (*p > 127)
    {
      _stprintf(s, _T("&#x%x;"), (int) *p);
      s += 5;
    }
    else if (*p != 0x0d && *p != 0x0a)
    {
      *s++ = *p;
    }
    p++;
  }
  *s = 0;
  m_strText.ReleaseBuffer();
  
}

void CXmlEntity::SetText(CString strText)
{
  m_strText = strText;
}

void CXmlEntity::SetValue(int nValue)
{
  m_strText.Format(_T("%d"), nValue);
}


CXmlExport::CXmlExport(LPCTSTR pszFileName)
{
  m_File.SetFilePath(pszFileName);
  m_nIndent = 0;
}

CXmlExport::~CXmlExport()
{

}

bool CXmlExport::Open(LPCTSTR pszRoot)
{
  if (!m_File.Open(m_File.GetFilePath(), CFile::modeWrite | CFile::modeCreate))
    return false;

  m_File.WriteString(_T("<?xml version=\"1.0\"?>\n"));
  CString str;
  str.Format(_T("<!DOCTYPE %s [\n"), pszRoot);
  m_File.WriteString(str);

  for (int i = 0; i < sizeof(XmlChars) / sizeof(SXmlChar); i++)
  {
    if (XmlChars[i].bDTD)
    {
      str.Format(_T("<!ENTITY  %s \"%c\">\n"), XmlChars[i].szChar, XmlChars[i].c);
      m_File.WriteString(str);
    }
  }
  m_File.WriteString(_T("]>\n"));

  return true;
}
void CXmlExport::Write(LPCTSTR pszTag, LPCTSTR pszValue)
{
  CXmlEntity Entity(this, pszTag);
  Entity.SetText(pszValue);
  Write(Entity);
}

void CXmlExport::Write(LPCTSTR pszTag, int nValue)
{
  CXmlEntity Entity(this, pszTag);
  Entity.SetValue(nValue);
  Write(Entity);
}

void CXmlExport::Write(const CXmlEntity& Entity)
{
  Write(&Entity);
}

void CXmlExport::Write( const CXmlEntity *pEntity)
{
  m_str = m_strIndent;
  m_str += '<';
  m_str += pEntity->m_strTag;
  for (int i = 0; i < pEntity->m_strAttrib.GetSize(); i++)
  {
    m_str += ' ';
    m_str += pEntity->m_strAttrib[i];
    m_str += "=\"";
    m_str += pEntity->m_strAttribValue[i];
    m_str += "\"";
  }

  if (pEntity->m_strText.IsEmpty())
  {
    m_str += ">\n";
    m_strIndent += szIndent;
  }
  else
  {
    m_str += ">";
    m_str += pEntity->m_strText;
  }

  WriteString();
}

void CXmlExport::WriteEndTag(const CXmlEntity *pEntity)
{
  if (pEntity->m_strText.IsEmpty())
  {
    m_strIndent = m_strIndent.Left(m_strIndent.GetLength() - (sizeof(szIndent) / sizeof(TCHAR) - 1));
    m_str = m_strIndent;
  }
  m_str += _T("</");
  m_str += pEntity->m_strTag;
  m_str += ">\n";
  WriteString();
}

void CXmlExport::Close()
{
  m_File.Close();
}

void CXmlExport::WriteString()
{
  m_File.WriteString(m_str);
  m_str.Empty();
}

void CXmlExport::WriteDate(CTime Time)
{
  CXmlEntity Date(this, szDate);
  Write(Date);

  Write(szDay, Time.GetDay());
  Write(szMonth, Time.GetMonth());
  Write(szYear, Time.GetYear());
}

