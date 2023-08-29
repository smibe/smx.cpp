// FileDropTarget.h: interface for the CFileDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEDROPTARGET_H__8114CB2F_9CAE_4036_9614_EA6DF72E0152__INCLUDED_)
#define AFX_FILEDROPTARGET_H__8114CB2F_9CAE_4036_9614_EA6DF72E0152__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileDropTarget;

typedef DROPEFFECT (CWnd::*DragEnterFP)(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, CFileDropTarget* pDrop);
typedef void       (CWnd::*DragLeaveFP)(CFileDropTarget* pDrop);
typedef DROPEFFECT (CWnd::*DragOverFP)(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point, CFileDropTarget* pDrop);
typedef BOOL       (CWnd::*DropFP)(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point, CFileDropTarget* pDrop);

class CFileDropTarget : public COleDropTarget  
{
public:
	CFileDropTarget();
	virtual ~CFileDropTarget();

  virtual DROPEFFECT OnDragEnter( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
  virtual void OnDragLeave( CWnd* pWnd );
  virtual DROPEFFECT OnDragOver( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
  virtual BOOL OnDrop( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

  UINT GetPathNames(COleDataObject* pDataObject, CStringArray* pPathArray);

public:
  DragEnterFP m_pDragEnter;
  DragLeaveFP m_pDragLeave;
  DragOverFP  m_pDragOver;
  DropFP      m_pDrop;
  CWnd       *m_pObject;

  static UINT sm_cfFileName;
  static UINT sm_cfShellIdList;

};

#endif // !defined(AFX_FILEDROPTARGET_H__8114CB2F_9CAE_4036_9614_EA6DF72E0152__INCLUDED_)
