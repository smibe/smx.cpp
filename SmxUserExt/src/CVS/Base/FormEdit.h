#if !defined(AFX_FORMEDIT_H__7110EA13_2134_11D5_B2A2_AC95D4000000__INCLUDED_)
#define AFX_FORMEDIT_H__7110EA13_2134_11D5_B2A2_AC95D4000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormEdit.h : header file
//

class CFormEdit : public CEdit
{
public:
	CFormEdit();
	virtual ~CFormEdit();

  //---------------- Overrides ------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormEdit)
	//}}AFX_VIRTUAL

  void      SetIntNumber(short nIntNo);                                   //@cmember Set the number of digits before decimal.
  void      SetDeciNumber(short nDeciNo);                                 //@cmember Set the number of digits after decimal.
  void      SetGrpNumber(short nGrpNo) ;                                  //@cmember Set the number of digits after decimal.
	
	double GetValue();
	int    GetIntValue();
  LPCTSTR GetText() { return m_strText; }
	
	void   SetValue(double dbValue);
	void   SetIntValue(int nValue);
	void   EnableGrpSymbol(bool bEnable);

  void    DoubleToString(CString& str, double dVal);
	bool    StringToDouble(double& dbValue, LPCTSTR strValue);

	//void   ConnectDBObject(CDBObject *pObject, SetDoubleFunc *pSetDoubleFunc);
	//void   ConnectDBObject(CDBObject *pObject, SetIntFunc *pSetIntFunc);

protected:
  void    FormatInput();                                      //@cmember Format the input string.                    
  bool    IsValidChar(int nChar);                             //@cmember Checks if the character is valid for the respective datatype                               
  void    ReplaceSymbol(TCHAR nFrom, TCHAR nTo);              //@cmember Replace nFrom with nTo in the member m_strText                                             
  CString RestoreSymbols();                                   //@cmember Deletes the grpSy and replaces deciSy in m_strText with '.' (used for converting text to float)
  bool    MaxLenReached();                                    //@cmember Returns TRUE if no more input is possible (length of string)                      
  void    EnterChar(UINT nChar, UINT nRepCnt, UINT nFlags);   //@cmember Enter a character in the edit box                                                        
  void    Overwrite(TCHAR nChar);                             //@cmember Overwrite one character to the right with cChar                             
  bool    IsOverwritePos();                                   //@cmember Check if the next entered character should overwrite the character to the right
  bool    CheckClipboardData();                               //@cmember Check if the clipboard data is OK.
  bool    IsDeletionAllowed ();                               //@cmember Check if deletion at this position is being allowed

	void    RestoreSymbols(CString& str);
	void    InsertGrpSy(CString& strInt);
	void    FormatString(CString& strValue);
	short   GetIntNumber(const CString strText);
	short   GetDeciNumber(const CString strText);

	//-------------- Generated message map functions ------------------
	//{{AFX_MSG(CFormEdit)
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);    //@cmember WM_KEYDOWN message handler.
  afx_msg void OnKillFocus(CWnd* pNewWnd);                          //@cmember WM_KILLFOCUS message handler.
  afx_msg void OnSetFocus(CWnd* pOldWnd);                           //@cmember WM_SETFOCUS message handler.
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);       //@cmember WM_CHAR message handler.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
protected:
	double dbValue;
	int    nValue;

	//DB Binding
	//CDBObject     *pObject;
	//SetDoubleFunc *pSetDoubleFunc;
	//SetIntFunc    *pSetIntFunc;

  CString    m_strText;         //@cmember holds the value displayed in the edit box
  CString    m_strOldText;      //@cmember Remembers the string in the moment of getting the focus (used for restoring the old value)

  short  m_nIntNo;                 //@cmember Number of digits before decimal
  short  m_nDeciNo;                //@cmember Number of digits after decimal
  short  m_nGrpNo;                 //@cmember Number of digits in group
          
  static TCHAR  sm_cDeciSy;        //@cmember Decimal symbol
  static TCHAR  sm_cGrpSy;         //@cmember Digit grouping symbol
  static short  sm_nGrpNo;         //@cmember Number of digits in group
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMEDIT_H__7110EA13_2134_11D5_B2A2_AC95D4000000__INCLUDED_)
