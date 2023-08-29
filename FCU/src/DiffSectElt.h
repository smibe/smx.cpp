/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffSectElt.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffSectElt.h,v 1.1 2002/09/20 09:34:46 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#if !defined(AFX_DIFFSECTELT_H__E72B1212_1706_11D2_8C4F_E0FB4F000000__INCLUDED_)
#define AFX_DIFFSECTELT_H__E72B1212_1706_11D2_8C4F_E0FB4F000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "list.h"
#include "struct.h"
#include "section.h"

struct DIFF_SECT 
{
	int state;				//State of this section		
	PLINEDESC start;		//First line
	PLINEDESC end;			//Last line
	int       movedIdx;		//Index of moved section
  
  CString GetStatus();
};

 
class CDiffSectElt : public CObject  
{
DECLARE_DYNCREATE(CDiffSectElt)

public:
	CDiffSectElt();
	CDiffSectElt(SECTION left, SECTION right, SECTION lComp, SECTION rComp);
	virtual ~CDiffSectElt();

  void SetCREmpty();
	void SetCLEmpty();
	
  bool IsCREmpty() { return(!(m_compRight.start && m_compRight.end)); };
	bool IsCLEmpty() { return(!(m_compLeft.start && m_compLeft.end)); };
	bool IsREmpty() { return(!(m_right.start && m_right.end)); };
	bool IsLEmpty() { return(!(m_left.start && m_left.end)); };
	
  void SetLLines(PLINEDESC pStart, PLINEDESC pEnd);
	void SetRLines(PLINEDESC pStart, PLINEDESC pEnd);
	void SetCRLines(PLINEDESC pStart, PLINEDESC pEnd);
	void SetCLLines(PLINEDESC pStart, PLINEDESC pEnd);
	
  //Member Access Functions
	PLINEDESC GetRStart()  { return m_right.start; }; 
	PLINEDESC GetREnd()    { return m_right.end; };
	PLINEDESC GetLStart()  { return m_left.start; };
	PLINEDESC GetLEnd()    { return m_left.end; };
	PLINEDESC GetCLStart() { return m_compLeft.start; };
	PLINEDESC GetCLEnd()   { return m_compLeft.end; };
	PLINEDESC GetCRStart() { return m_compRight.start; };
	PLINEDESC GetCREnd()   { return m_compRight.end; };
	PLINEDESC GetCStart();
  PLINEDESC GetCEnd();

public:
  DIFF_SECT m_right;
	DIFF_SECT m_left;
	DIFF_SECT m_compLeft;
	DIFF_SECT m_compRight;
	PLINEDESC m_pAfter;           // Line after which this section should be
																// inserted, NULL if previous section is not 
	                              // of STATE_SAME

  int  m_nIdx;                  // Actual index in the array.
};

#endif // !defined(AFX_DIFFSECTELT_H__E72B1212_1706_11D2_8C4F_E0FB4F000000__INCLUDED_)
