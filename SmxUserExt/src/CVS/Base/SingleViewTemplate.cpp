/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SingleViewTemplate.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/SingleViewTemplate.cpp,v 1.2 2002/11/22 17:25:35 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// SingleViewTemplate.cpp: implementation of the CSingleViewTemplate class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SingleViewTemplate.h"

CSingleViewTemplate::CSingleViewTemplate(UINT nIDResource, CRuntimeClass* pDocClass, 
                                                           CRuntimeClass* pFrameClass, 
                                                           CRuntimeClass* pViewClass )
: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
  m_pDocument = NULL;
}

CSingleViewTemplate::~CSingleViewTemplate()
{

}

void CSingleViewTemplate::AddDocument(CDocument* pDoc)
{
  ASSERT(m_pDocument == NULL);
  CMultiDocTemplate::AddDocument(pDoc);
}

void CSingleViewTemplate::RemoveDocument(CDocument* pDoc)
{
  CMultiDocTemplate::RemoveDocument(pDoc);
  m_pDocument = NULL;
}

CDocument* CSingleViewTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible)
{
  CloseAllDocuments(FALSE);
  m_pDocument = CMultiDocTemplate::OpenDocumentFile(lpszPathName, bMakeVisible);
  return m_pDocument;
}

CView* CSingleViewTemplate::GetView()
{
  if (!GetDocument())
    return NULL;

  POSITION pos = GetDocument()->GetFirstViewPosition();
  CView *pView = NULL;

  while (pos != NULL)
  {
    pView = GetDocument()->GetNextView(pos);
    if (pView->IsKindOf(m_pViewClass))
      return pView;
  }   
  
  return NULL;
}

void CSingleViewTemplate::CloseAllDocuments( BOOL bEndSession )
{
  if (GetDocument())
  {
    GetDocument()->OnCloseDocument();
    m_pDocument = NULL;
  }

  CMultiDocTemplate::CloseAllDocuments(bEndSession);
}
