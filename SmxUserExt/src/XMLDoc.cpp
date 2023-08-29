// XMLDoc.cpp: implementation of the CXMLDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "XMLDoc.h"
#include "ParseFile.h"

bool GetLine(CString& str, DWORD& dwGetLine)
{
  LPCTSTR pszBuffer = (LPCTSTR ) dwGetLine;
  TCHAR *pChar = const_cast<LPSTR>(pszBuffer);
  TCHAR cTemp;

  while (*pChar != 0xd && *pChar != 0xa)
    *pChar++;

  cTemp = *pChar;
  *pChar = 0;
  str = pszBuffer;
  *pChar = cTemp;
  
  //Skip EOL
  while (*pChar == 0xd || *pChar == 0xa)
    pChar++;

  dwGetLine = (DWORD) pChar;
  return *pszBuffer != 0;
}

CXMLElement::CXMLElement(CXMLElement* pParent)
{
  m_pParent = pParent;
  m_pNext = NULL;
  m_pPrev = NULL;
  m_pChild = NULL;
}

CXMLElement::~CXMLElement()
{
  if (m_pNext)
    m_pNext->m_pPrev = m_pPrev;

  if (m_pPrev)
    m_pPrev->m_pNext = m_pNext;

  if (m_pParent && m_pParent->m_pChild == this)
    m_pParent->m_pChild = m_pNext;
}

CXMLElement *CXMLElement::GetFirst()
{
  CXMLElement *pElt = this;
  while (pElt->m_pPrev)
    pElt = pElt->m_pPrev;

  return pElt;
}

CXMLElement *CXMLElement::GetLast()
{
  CXMLElement *pElt = this;
  while (pElt->m_pNext)
    pElt = pElt->m_pNext;

  return pElt;
}

CXMLElement *CXMLElement::GetNext(LPCTSTR pszTag)
{
  if (!pszTag || *pszTag == 0)
    return m_pNext;

  CXMLElement *pElt = this;
  while (pElt->m_pNext)
  {
    pElt = pElt->m_pNext;
    if (pElt->m_strTag == pszTag)
      return pElt;
  }
  return NULL;
}

CXMLElement *CXMLElement::GetChild(LPCTSTR pszTag)
{
  if (this == NULL)
    return NULL;

  if (!pszTag || *pszTag == 0)
    return m_pChild;

  CXMLElement *pElt = m_pChild;
  while (pElt)
  {
    if (pElt->m_strTag == pszTag)
      return pElt;

    pElt = pElt->m_pNext;
  }
  return NULL;
}

int CXMLElement::GetChildCount(LPCTSTR pszTag)
{
  int nCount = 0;

  CXMLElement *pElt = m_pChild;
  while (pElt)
  {
    pElt = pElt->m_pNext;
    nCount++;
  }
  return nCount;
}

void CXMLElement::AddChild(CXMLElement *pElt)
{
  pElt->m_pParent = this;

  if (!m_pChild)
  {
    m_pChild = pElt;
    return;
  }

  CXMLElement *pLast = m_pChild->GetLast();
  pLast->m_pNext = pElt;
  pElt->m_pPrev = pLast;
}


CXMLDoc::CXMLDoc()
{
  m_pRoot        = NULL;
}

CXMLDoc::~CXMLDoc()
{
  Delete(m_pRoot);   
}

bool CXMLDoc::Load(LPCTSTR pszFileName)
{
  return true;
}

bool CXMLDoc::LoadXML(LPCTSTR pszText)
{
  CParseFile ParseFile;
  ParseFile.SetGetLineFunc(GetLine, (DWORD) pszText);

  Delete(m_pRoot);
  m_pRoot = NULL;

  if (!ParseFile.CheckChar('<'))
  {
    m_strError = ParseFile.GetErrorStr();
    Delete(m_pRoot);
    m_pRoot = NULL;
    return false;
  }

  if (!Parse(&ParseFile, NULL))
  {
    m_strError = ParseFile.GetErrorStr();
    Delete(m_pRoot);
    m_pRoot = NULL;
    return false;
  }
  return true;
}

bool CXMLDoc::Parse(CParseFile *pParse, CXMLElement *pParent)
{
  CString strIdent;
  CString strText;

  pParse->GetIdent(strIdent);
  pParse->GetChar('>');
  pParse->GetXMLText(strText);

  
  CXMLElement *pElt = new CXMLElement();
  pElt->SetTag(strIdent);

  if (pParent)
    pParent->AddChild(pElt);
  else
    m_pRoot = pElt;

  if (!strText.IsEmpty())
  {
    CXMLElement *pText = new CXMLElement(pElt);
    pElt->AddChild(pText);
    pText->SetText(strText);
  }

  pParse->GetChar('<');
  while (!pParse->GetError() && !pParse->CheckChar('/'))
  {
    Parse(pParse, pElt);
    if (pParse->CheckEOF())
      return false;
    
    if (pParse->GetError())
      return false;

    pParse->GetXMLText(strText);

    if (!strText.IsEmpty())
    {
      CXMLElement *pText = new CXMLElement(pElt);
      pElt->AddChild(pText);
      pText->SetText(strText);
    }
    pParse->GetChar('<');
  }

  pParse->CheckString(strIdent);
  pParse->GetChar('>');
  //pElt->SetText(strText);
  
  return !pParse->GetError();
}

bool CXMLDoc::Save(LPCTSTR pszFileName)
{
  return true;
}

CXMLElement *CXMLDoc::GetRoot()
{
  return m_pRoot;
}

void CXMLDoc::Delete(CXMLElement *pRemove)
{
  if (pRemove == 0)
    return;

  if (pRemove->GetChild() == NULL)
  {
    delete pRemove;
    pRemove = NULL;
    return;
  }

  CXMLElement *pElt  = pRemove->GetChild();
  CXMLElement *pNext = NULL;
  do
  {
    pNext = pElt->GetNext();
    Delete(pElt);
    pElt = pNext;
  }while (pElt);

  delete pRemove;
  pRemove = NULL;
}


CString CXMLElement::GetCompleteText()
{
  CString str;
  CXMLElement *pNext = GetChild();
  while (pNext)
  {
    str += pNext->GetCompleteText();
    pNext = pNext->GetNext();
  }
  str += m_strText;
  return str;
}
