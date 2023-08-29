/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: LINE.H,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/LINE.H,v 1.2 2002/11/22 17:27:13 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1996 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/*
 * LINE.H
 */

#define Line_TRAVERSE(lst, curs)  for(  curs=line_get_first(lst)            \
                                       ;  curs!=NULL                      \
                                       ;  curs = line_get_next(curs)  \
                                       )


void line_reset(PLINEDESC line);
BOOL line_compare(PLINEDESC line1, PLINEDESC line2);
BOOL line_link(PLINEDESC line1, PLINEDESC line2);
LPSTR line_gettext(PLINEDESC line);
int line_gettabbedlength(PLINEDESC line, int tabstops);
DWORD line_gethashcode(PLINEDESC line);
PLINEDESC line_getlink(PLINEDESC line);
UINT line_getlinenr(PLINEDESC line);
BOOL line_isblank(PLINEDESC line);
PLINEDESC line_get_first(PLINEDESC line);
PLINEDESC line_get_last(PLINEDESC line);
PLINEDESC line_get_prev(PLINEDESC line);
PLINEDESC line_get_next(PLINEDESC line);
DWORD APIENTRY hash_string(LPSTR string, BOOL bIgnoreBlanks, BOOL bIgnoreEOL, BOOL bIgnoreCase);

// MCN
PLINEDESC line_iterate(PLINEDESC line, int iLines);