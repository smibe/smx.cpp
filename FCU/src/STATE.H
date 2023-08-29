/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: STATE.H,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/STATE.H,v 1.2 2002/11/22 17:27:13 GeH Exp $
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
 * STATE.H
 *
 * Definition of the results of comparisons for files and for lines
 * within files.
 *
 * These need to be globally declared so that the UI code in windiff.c can
 * map states to the colour scheme (to correctly highlight changed lines).
 *
 * They apply to files (compitem_getstate() ) and to sections in the
 * composite list (section_getstate). All lines within a section have the
 * same state. The UI code will use the view_getstate() function to find the
 * state for a given line on the screen.
 *
 */

enum ELineState
{
    STATE_SAME        = 1,       //@emem Lines are the same
    STATE_LEFTONLY    = 2,       //@emem Lineine only in left file 
    STATE_RIGHTONLY   = 3,       //@emem Line only in right file
    STATE_MOVEDLEFT   = 4,       //@emem This is the left file version
    STATE_MOVEDRIGHT  = 5,       //@emem This is the right file version

    // GeH: After Operations with sections (only for composed section list)
    STATE_DELLEFTBEFORE		= 6,	 //@emem Left section was deleted before
    STATE_DELRIGHTBEFORE	= 7,   //@emem Right section wad deleted before
    STATE_DELLEFTAFTER		= 8,   //@emem Left section was deleted after.
    STATE_DELRIGHTAFTER		= 9,   //@emem Right section was deleted after.
    STATE_FOLDED      		= 10,  //@emem This is a folded line

    STATE_MARKED          = 99,  //@emem When creating the composite, marks the line as handled.
};
