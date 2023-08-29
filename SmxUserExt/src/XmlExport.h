// XmlExport.h: interface for the CXmlExport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLEXPORT_H__8FCB6E9A_F506_442E_B099_859B0E246C17__INCLUDED_)
#define AFX_XMLEXPORT_H__8FCB6E9A_F506_442E_B099_859B0E246C17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef SMX_STRCODE
#define TAG(__x) LPCTSTR sz##__x = _T(#__x);
#else
#define TAG(__x) extern LPCTSTR sz##__x;
#endif


TAG(Date);
TAG(Day);
TAG(Month);
TAG(Year);


class CXmlExport;

class CXmlEntity
{
public:
  CXmlEntity(CXmlExport *pExp, LPCTSTR pszTag);
  ~CXmlEntity();

  void AddAttribute(LPCTSTR pszAttrib, LPCTSTR pszAttribVal);
  void AddAttribute(LPCTSTR pszAttrib, CString strAttribVal);
  void SetText(LPCTSTR pszText);
  void SetText(CString strText);
  void SetValue(int nValue);

public:
  CXmlExport *m_pExp;
  CString m_strTag;
  CString m_strText;
  CStringArray m_strAttrib;
  CStringArray m_strAttribValue;
};

class CXmlExport  
{
public:
	CXmlExport(LPCTSTR pszFileName);
	virtual ~CXmlExport();

  bool Open(LPCTSTR pszRoot);
  void Write(const CXmlEntity *pEntity);
  void Write(const CXmlEntity& Entity);
  void Write(LPCTSTR pszTag, LPCTSTR pszValue);
  void Write(LPCTSTR pszTag, int nValue);
  void WriteEndTag(const CXmlEntity *pEntity);
  void WriteDate(CTime Time);
  void Close();

protected:
  void WriteString();

public:
  CString m_str;
  CString m_strIndent;
  CStdioFile m_File;
  int m_nIndent;
};

#endif // !defined(AFX_XMLEXPORT_H__8FCB6E9A_F506_442E_B099_859B0E246C17__INCLUDED_)
