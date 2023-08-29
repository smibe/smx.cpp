#if !defined(AFX_SMXDIRTREE_H__3A062429_54DA_4E83_8D8A_944CEAA829B7__INCLUDED_)
#define AFX_SMXDIRTREE_H__3A062429_54DA_4E83_8D8A_944CEAA829B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

class CSmxDirTree : public CTreeCtrl
{
public:
	CSmxDirTree();
	virtual ~CSmxDirTree();

  bool Load(HTREEITEM hItem, LPCTSTR pszDir);



  //--------------- Overrides --------------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmxDirTree)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CSmxDirTree)
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
  CString m_strDir;
  DWORD   m_dwDisplayFlags;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMXDIRTREE_H__3A062429_54DA_4E83_8D8A_944CEAA829B7__INCLUDED_)
