/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SECTION.H,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/SECTION.H,v 1.2 2002/11/22 17:27:13 GeH Exp $
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
 * SECTION.H
 */

/* handle to a section */
typedef struct section FAR * SECTION;


SECTION section_new(PLINEDESC first, PLINEDESC last, LIST list);
void section_delete(SECTION section);
BOOL section_match(SECTION section1, SECTION section2);
PLINEDESC section_getfirstline(SECTION section);
PLINEDESC section_getlastline(SECTION section);
void section_setfirstline(SECTION section, PLINEDESC line);
void section_setlastline(SECTION section, PLINEDESC line);
SECTION section_getlink(SECTION section);
SECTION section_getcorrespond(SECTION section);
void section_setstate(SECTION section, int state);
int section_getstate(SECTION section);
int section_getlinecount(SECTION section);
int section_getleftbasenr(SECTION section);
int section_getrightbasenr(SECTION section);
LIST section_makelist(PLINEDESC linelist, BOOL left);
void section_deletelist(LIST sections);
LIST section_makecomposite(LIST secsleft, LIST secsright);
BOOL section_matchlists(LIST secsleft, LIST secsright);
void section_setlinestate(SECTION sect);
SECTION Unify(SECTION sect, int state); 

// MCN
int section_exists_line(SECTION sec, PLINEDESC line);
SECTION section_search_in_list(LIST list, PLINEDESC line);
SECTION section_get_right_from_comp(SECTION sec);
SECTION section_get_left_from_comp(SECTION sec);
SECTION section_get_comp_from_right(LIST list, SECTION sec);
SECTION section_get_comp_from_left(LIST list, SECTION sec);

