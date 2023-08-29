// XMLDoc.h: interface for the CXMLDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLDOC_H__0DB29513_2138_41EF_8B15_2C6E21F4410E__INCLUDED_)
#define AFX_XMLDOC_H__0DB29513_2138_41EF_8B15_2C6E21F4410E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXMLElement;
class CXMLAttribute;
class CXMLElementList;
class CParseFile;

typedef CList<CXMLAttribute *, CXMLAttribute *> CXMLAttributeList;

class CXMLElement
{
public:
  CXMLElement(CXMLElement *pParent = NULL);
  virtual ~CXMLElement();

public:
  CXMLElement *GetParent() { return m_pParent; }
  CXMLElement *GetFirst(); 
  CXMLElement *GetLast();

  CXMLElement *GetNext(LPCTSTR pszTag = NULL);
  CXMLElement *GetChild(LPCTSTR pszTag = NULL);
   int         GetChildCount(LPCTSTR pszTag = NULL);

  void AddChild(CXMLElement *pElt);

  void SetTag(LPCTSTR pszTag)   { m_strTag = pszTag; }
  void SetText(LPCTSTR pszText) { m_strText = pszText; }
  
  CString GetCompleteText();
  CString GetTag()  { return m_strTag; }
  CString GetText() { return m_strText; }
protected:
  CString           m_strTag;
  CString           m_strText;
  CXMLElement      *m_pNext;
  CXMLElement      *m_pPrev;
  CXMLElement      *m_pParent;
  CXMLElement      *m_pChild;

  CXMLAttributeList m_aAttributes;
};


class CXMLDoc  
{
public:
	CXMLDoc();
	virtual ~CXMLDoc();

  bool Load(LPCTSTR pszFileName);
  bool LoadXML(LPCTSTR pszText);
  bool Save(LPCTSTR pszFileName);

  void Delete(CXMLElement *pRemove);

  CXMLElement *GetRoot();
  LPCSTR GetError() { return m_strError; }

protected:
  bool Parse(CParseFile *pParse, CXMLElement *pParent);

protected:
  CXMLElement *m_pRoot;
  CString      m_strError;
};

#endif // !defined(AFX_XMLDOC_H__0DB29513_2138_41EF_8B15_2C6E21F4410E__INCLUDED_)
