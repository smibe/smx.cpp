/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: FilDiffview.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/FilDiffview.h,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#ifndef AFX_FILDIFFXML_H__8786B282_0502_11D2_9E5A_3068BF000000__INCLUDED_
#define AFX_FILDIFFXML_H__8786B282_0502_11D2_9E5A_3068BF000000__INCLUDED_

#ifdef SMX_STRCODE
#undef TAG
#define TAG(__x) LPCTSTR sz##__x = _T(#__x);
#else
#define TAG(__x) extern LPCTSTR sz##__x;
#endif


TAG(FileCompare);
TAG(LeftFile);
TAG(RightFile);
TAG(Name);
TAG(Size);

TAG(Options);
TAG(IgnoreBlanks);
TAG(IgnoreEOL);
TAG(IgnoreCase);
TAG(IdenticalSect);

TAG(Sections);
TAG(Section);
TAG(Status);
TAG(StartLineNo);
TAG(EndLineNo);

TAG(Lines);
TAG(Line);


#endif // AFX_FILDIFFXML_H__8786B282_0502_11D2_9E5A_3068BF000000__INCLUDED_
