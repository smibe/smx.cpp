#if !defined(AFX_SMXCOMBOBOX_H__468F75B7_F45B_4CB5_9DC3_51F318B07B6A__INCLUDED_)
#define AFX_SMXCOMBOBOX_H__468F75B7_F45B_4CB5_9DC3_51F318B07B6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SMXComboBox.h : header file
//

struct CB_INIT;
#include "SMXComboEdit.h"

class CSMXComboBox : public CComboBox
{
public:
	CSMXComboBox();
	virtual ~CSMXComboBox();

  void Initialize(CB_INIT *pCbInit);
  int  SetCurSelItemWithData(int nData);
  int  GetCurSelItemData();

  void Set3DFrame(bool b3DFrame);
  virtual void EndEdit() {};

  // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMXComboBox)
	//}}AFX_VIRTUAL


protected:
	//{{AFX_MSG(CSMXComboBox)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
    CSMXComboEdit m_edit;  // subclassed edit control
protected:
  bool   m_b3DFrame;
  int    m_nShrinkHeight;
  CRect  m_btnRect;        //@cmember CRext where the button will be drawn (for m_b3D == FALSE)
  bool   m_bClicked;       //@cmember TRUE if the combo box button is clicked
  bool   m_bLBMoved;       //@cmember TRUE if the listbox already moved.
  bool   m_bReadOnly;      //@cmember true if the control is set readonly.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMXCOMBOBOX_H__468F75B7_F45B_4CB5_9DC3_51F318B07B6A__INCLUDED_)
