/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SingleViewTemplate.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SingleViewTemplate.h,v 1.1 2002/09/20 09:37:08 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// SingleViewTemplate.h: interface for the CSingleViewTemplate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEVIEWTEMPLATE_H__D28CA919_ACF3_4279_9FA1_7EE338F7EEFD__INCLUDED_)
#define AFX_SINGLEVIEWTEMPLATE_H__D28CA919_ACF3_4279_9FA1_7EE338F7EEFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSingleViewTemplate : public CMultiDocTemplate  
{
public:
	CSingleViewTemplate(UINT nIDResource, CRuntimeClass* pDocClass, 
                                        CRuntimeClass* pFrameClass, 
                                        CRuntimeClass* pViewClass );
	virtual ~CSingleViewTemplate();

	virtual void       AddDocument(CDocument* pDoc);
	virtual void       RemoveDocument(CDocument* pDoc);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
  virtual void CloseAllDocuments( BOOL bEndSession );


  virtual CDocument* GetDocument() { return m_pDocument; }
  virtual CView*     GetView();
 
protected:
  CDocument *m_pDocument;      //the one and only view
};

#endif // !defined(AFX_SINGLEVIEWTEMPLATE_H__D28CA919_ACF3_4279_9FA1_7EE338F7EEFD__INCLUDED_)
