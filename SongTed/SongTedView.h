// SongTedView.cpp : implementation of the CSongTedView class
//
#if !defined(AFX_SONGTEDVIEW_H__01E2B596_EF7D_4A42_A601_CF65E807386D__INCLUDED_)
#define AFX_SONGTEDVIEW_H__01E2B596_EF7D_4A42_A601_CF65E807386D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "SongTed.h"
#include <atlconv.h>

#include "Util.h"
#include "SmxRes.h"
#include "SongTedDoc.h"
#include "SmxPgSetupDlg.h"
#include "XMLDoc.h"
#include "SelectTagDlg.h"
#include "SongTedOptions.h"
#include "OEdit.h"
#include "SongTedCtrl.h"
#include "SmxPrnFormview.h"


class CSongTedView : public CSmxPrnFormView
{
public:
	DECLARE_DYNCREATE(CSongTedView)
	DECLARE_MESSAGE_MAP()


	CSongTedView();

	void DoDataExchange(CDataExchange* pDX);

	CSongTedView::~CSongTedView();

	BOOL CSongTedView::PreCreateWindow(CREATESTRUCT& cs);

	void CSongTedView::OnDraw(CDC* pDC);
	void CSongTedView::OnEditInsertChords();
	void CSongTedView::OnEditInsertTag();

#ifdef _DEBUG
	void CSongTedView::AssertValid() const;
	void CSongTedView::Dump(CDumpContext& dc) const;
	CSongTedDoc* CSongTedView::GetDocument();
#endif //_DEBUG

	void CSongTedView::OnFilePrint();
	void CSongTedView::OnSize(UINT nType, int cx, int cy);
	void CSongTedView::OnInitialUpdate();
	void CSongTedView::Draw();
	void CSongTedView::OnPageSetup();
	bool CSongTedView::GetText(CString& str, bool bSelection = false);
	void CSongTedView::OnEditAuthor();
	BOOL CSongTedView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	void CSongTedView::PrintStave(CXMLElement* pElt, int nCordY, int nCount, bool& bNewPageOrColumn);
	void CSongTedView::OnEditInsertChord();
	void CSongTedView::OnEditCreateMaster();
	BOOL CSongTedView::OnEraseBkgnd(CDC* pDC);
	void CSongTedView::OnPreview();
	void CSongTedView::OnViewOptions();
	
	LPCTSTR GetPageSetupSection() override { return "SongTedPageSetup"; }

protected:
	CSongTedCtrl m_SongTed;
	CXMLDoc* m_pXMLDoc;
	bool m_bPreview;
	CXMLDoc* m_pMasterStave;
	CArray<CXMLElement*> m_aPages;
	int m_nNofColumns;
	int m_nBetweenCols;
	int m_nStartY;
	int m_nFirstCol;

};


#endif // !defined(AFX_SONGTEDVIEW_H__01E2B596_EF7D_4A42_A601_CF65E807386D__INCLUDED_)
