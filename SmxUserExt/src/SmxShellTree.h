
#ifndef __SMCSHELLTREE_H__
#define __SMCSHELLTREE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SMXShell.h"
#include "SMXTreeCtrl.h"

// class forwards
class CSMXShellList;

// CSMXShellTree is a CSMXTreeCtrl derived class which is used to display
// system files and folders, typically associated with a CSMXShellList object
class CSMXShellTree : public CSMXTreeCtrl
{
	DECLARE_DYNAMIC(CSMXShellTree)

public:
	
	// Default constructor
	//
	CSMXShellTree();

	// Virtual destructor
	//
	virtual ~CSMXShellTree();

public:

	CSMXShell		m_shell;		// used for shell operations
	CSMXShellList*	m_pListCtrl;	// CSMXShellList that is associated with this control 
	CComboBox*		m_pComboBox;	// CComboBox that is associated with this control see AssociateCombo(...)
	bool			m_bRefresh;		// true when the control is to be refreshed
	bool			m_bOutaHere;	// true when the control is destroyed
	HTREEITEM		m_htiCurrent;	// currently selected tree item

	// enumeration that holds the type of object folder or drive
	//
	enum FindAttribs { type_drive, type_folder };

public:

	// this member function obtains a handle to the system image 
	// list and attaches it to the tree control. DO NOT DELETE the imagelist
	//
	virtual BOOL InitSystemImageLists();

	// this member function fills a branch of the TreeView control. Given the
	// shell folder, enumerate the subitems of this folder,
	// and add the appropriate items to the tree.
	//
	// This function enumerates the items in the folder identifed by lpsf.
	// Note that since we are filling the left hand pane, we will only add
	// items that are folders and/or have sub-folders.  We *could* put all
	// items in here if we wanted, but that's not the intent.
	//
	virtual void FillTreeView(
		// pointer to shell folder that we want to enumerate items 
		LPSHELLFOLDER lpsf,
		// fully qualified item id list to the item that we are enumerating
		// items for.  In other words, this is the PIDL to the item
		// identified by the lpsf parameter.
		LPITEMIDLIST lpifq,
		// parent node
		HTREEITEM hParent);

	// this member function processes the File.Fill/RefreshTree command
	//
	virtual void OnFill(
		// true when refreshing tree
		bool bRefresh=false);

	// this member function populates tree based upon path. this method was
	// written by Takehiko Mizoguti [mizoguti@m2.sys.to.casio.co.jp]
	//
	virtual void PopulateTree(
		// path to populate
		LPCTSTR lpszPath);

	// this member function will "tunnel" the tree to find the specified path.
	// this will only work when the OnFill(...) method is used to populate the tree
	// written by Selom Ofori [sofori@chat.carleton.ca]
	//
	virtual void TunnelTree(
		// path to find
		CString szFindPath);

	// this member function is used to associate a CComboBox object with the
	// control. whenever the path changes, the combo is updated.
	//
	virtual void AssociateCombo(
		// combo box to associate
		CComboBox *pComboBox);

	// this member function is used to associate a CSMXShellList object with the
	// control. whenever the path changes, the list is updated.
	//
	virtual void AssociateList(
		// list control to associate
		CSMXShellList *pListCtrl);

	// this member function retrieves the path of a folder item, does not have 
	// to be selected. Pass a CString object that will hold the folder path. 
	// If the path is not in the filesystem(eg MyComputer) 
	// or none is selected it returns FALSE.
	//
	virtual BOOL GetFolderItemPath(
		// tree item to get path for
		HTREEITEM hItem,
		// reference to a CString object to contain folder path
		CString &strFolderPath);

	// this member function retrieves the path of a folder item, does not have 
	// to be selected. Pass a CString object that will hold the folder path. 
	// If the path is not in the filesystem(eg MyComputer) 
	// or none is selected it returns FALSE.
	// written by Selom Ofori [sofori@chat.carleton.ca]
	//
	virtual BOOL GetSelectedFolderPath(
		// reference to a CString object to contain folder path
		CString &strFolderPath);

  HTREEITEM AddItem(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, LPITEMIDLIST  lpi, HTREEITEM hParent, HTREEITEM hPrev);
  void PopulateTree(LPSHELLFOLDER pFolder, LPITEMIDLIST pIDL);

	// this member function searchs the tree for a specified folder
	// written by Selom Ofori [sofori@chat.carleton.ca]
	//
	virtual BOOL SearchTree(
		// tree item to begin search from
		HTREEITEM treeNode,
		// string to search for
		CString szSearchName,
		// looking for folder or drive
		FindAttribs attr);

	// Callback routine for sorting tree
	//
	static int CALLBACK TreeViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM lparamSort);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXShellTree)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CSMXShellTree)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

inline void CSMXShellTree::AssociateList(CSMXShellList *pListCtrl)
{ ASSERT( pListCtrl != NULL ); m_pListCtrl = pListCtrl; }

inline void CSMXShellTree::AssociateCombo(CComboBox *pComboBox)
	{ ASSERT( pComboBox != NULL ); 	m_pComboBox = pComboBox; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __SMCSHELLTREE_H__
