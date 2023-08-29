/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: DiffSectElt.cpp,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/DiffSectElt.cpp,v 1.1 2002/09/20 09:34:46 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "DiffSectElt.h"
#include "State.h"
#include "Util.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDiffSectElt, CObject)

CDiffSectElt::CDiffSectElt()
{
	memset(&m_right,     0, sizeof(DIFF_SECT));
	memset(&m_left,      0, sizeof(DIFF_SECT));
	memset(&m_compLeft,  0, sizeof(DIFF_SECT));
	memset(&m_compRight, 0, sizeof(DIFF_SECT));
	m_pAfter = NULL;
}

CDiffSectElt::~CDiffSectElt()
{

}


void CDiffSectElt::SetLLines(PLINEDESC pStart, PLINEDESC pEnd)
{
	ASSERT(pStart == NULL || pEnd == NULL);
	m_left.start = pStart;
	m_left.end = pEnd;
}

void CDiffSectElt::SetCLLines(PLINEDESC pStart, PLINEDESC pEnd)
{
	m_compLeft.start = pStart;
	m_compLeft.end = pEnd;
}

void CDiffSectElt::SetCRLines(PLINEDESC pStart, PLINEDESC pEnd)
{
	m_compRight.start = pStart;
	m_compRight.end = pEnd;
}

void CDiffSectElt::SetRLines(PLINEDESC pStart, PLINEDESC pEnd)
{
	ASSERT(pStart == NULL || pEnd == NULL);
	ASSERT(pStart || pEnd);
	m_right.start = pStart;
	m_right.end = pEnd;
}

CDiffSectElt::CDiffSectElt(SECTION left, SECTION right, 
						   SECTION lComp, SECTION rComp)
{
	m_right.start = section_getfirstline(right);
	m_right.end   = section_getlastline (right);
	m_right.state = rComp ? section_getstate(rComp) : 0;

	m_left.start = section_getfirstline(left);
	m_left.end   = section_getlastline (left);
	m_left.state = lComp ? section_getstate(lComp) : 0;

	m_compRight.start = section_getfirstline(rComp);
	m_compRight.end   = section_getlastline (rComp);
	m_compRight.state = rComp ? section_getstate(rComp) : 0;

	m_compLeft.start = section_getfirstline(lComp);
	m_compLeft.end   = section_getlastline (lComp);
	m_compLeft.state = lComp ? section_getstate(lComp) : 0;

	m_pAfter = NULL;
}


void CDiffSectElt::SetCREmpty()
{
	m_compRight.start = 0;
	m_compRight.end   = 0;
}

void CDiffSectElt::SetCLEmpty()
{
	m_compLeft.start  = 0;
	m_compLeft.end    = 0;
}

PLINEDESC CDiffSectElt::GetCStart() 
{ 
	if(m_compLeft.start && m_compLeft.end) 
		return m_compLeft.start;
	else if(m_compRight.start && m_compRight.end)
		return m_compRight.start;
	else
		return m_compLeft.start;
};

PLINEDESC CDiffSectElt::GetCEnd() 
{ 
	if(m_compRight.start && m_compRight.end)
		return m_compRight.end;
	else if(m_compLeft.start && m_compLeft.end) 
		return m_compLeft.end;
	else
		return m_compRight.end;
};

CString DIFF_SECT::GetStatus()
{
  CString str;
  switch (state)
  {
    case STATE_LEFTONLY:
      return GetResourceString(IDS_LeftOnly);
    case STATE_RIGHTONLY:
      return GetResourceString(IDS_RightOnly);
    default:
      return GetResourceString(IDS_Identical);
  }
}
