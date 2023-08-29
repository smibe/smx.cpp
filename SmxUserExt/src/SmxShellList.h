
#ifndef __SMXSHELLLIST_H__
#define __SMXSHELLLIST_H__

#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmxShell.h"
#include "SmxListCtrl.h"

// class forwards
class CSMXShellTree;

// CSMXShellList is a CSMXListCtrl derived class which is used to display
// system files and folders, typically associated with a CSMXShellTree object
class CSMXShellList : public CSMXListCtrl
{
	DECLARE_DYNAMIC(CSMXShellList)

public:

	// Default constructor
	//
	CSMXShellList();

	// Virtual destructor
	//
	virtual ~CSMXShellList();

public:
	virtual void BuildDefaultColumns();

	// this member function rebuilds the current directory list.
	//
	virtual void RefreshList();

	// call this member function to associate a CSMXShellTree object with
	// this control
	//
	virtual void AssociateTree(CSMXShellTree *pTreeCtrl);

	// this member function is called to initialize the system image list
	// used by the list control
	//
	virtual BOOL InitSystemImageLists();

	// this member function is called to initialize the list control items
	//
	virtual BOOL InitListViewItems(
		// Pointer to TreeView item data
		LPTVITEMDATA lptvid,
		// Pointer to parent shell folder
		LPSHELLFOLDER lpsf);

	// this member function is called to actually populate the list control
	// items
	//
	virtual BOOL PopulateListView(LPTVITEMDATA lptvid, LPSHELLFOLDER lpsf);

	// this member function is called to insert a single item into the
	// list control
	//
	virtual int InsertListItem(CString strFileName, BOOL bParse=TRUE);

	// Callback routine for sorting list
	//
	static int CALLBACK ListViewCompareProc(LPARAM, LPARAM, LPARAM);

	// override this member function in your derived class to
	// peform custom sort routines.
	//
	virtual bool SortList(
		int nCol,
		bool bAscending );

  //-------- Overrides ---------------------
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXShellList)
	//}}AFX_VIRTUAL

protected:
	//----------- Generated message map functions ---------------
	//{{AFX_MSG(CSMXShellList)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg long OnShellNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:

	CSMXShell		m_shell;			// used for shell operations
	CSMXShellTree*	m_pTreeCtrl;		// points to a CSMXShellTree that is associated with this control
	BOOL			m_bBuildingList;
	BOOL			m_bShowHidden;


};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

inline void CSMXShellList::AssociateTree(CSMXShellTree *pTreeCtrl)
	{ ASSERT( pTreeCtrl ); m_pTreeCtrl = pTreeCtrl; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _WIN32_IE >= 0x0400
#endif // __SMXSHELLLIST_H__
