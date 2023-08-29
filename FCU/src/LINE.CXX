/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: LINE.CXX,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/LINE.CXX,v 1.2 2002/11/22 17:27:13 GeH Exp $
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

/****************************** Module Header *******************************
* Module Name: LINE.C
*
* Functions that handle lines of text to be output.
*
* Functions:
*
* line_reset()
* line_gettext()
* line_gettabbedlength()
* line_getlink()
* line_getlinenr()
* line_compare()
* line_link()
* line_isblank()
* line_get_first()
* line_get_last()
* line_get_prev()
* line_get_next()
*
* Comments:
*
* LINE is a data type representing a string of ascii text along with 
* a line number.
*
* A LINE can compare itself to another line, and maintain a link if the
* lines are similar. 
*
* Comparisons between lines take note of the global option flag
* ignore_blanks, defined elsewhere. If this is true, we ignore
* differences in spaces and tabs when comparing lines, and when
* generating hashcodes.
*
* Links and are only generated once. To clear the link call line_reset.
*
* Lines can be allocated on a list. If a null list handle is passed, the
* line will be allocated using gmem_get() from the hHeap defined and
* initialised elsewhere.
*
****************************************************************************/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "FCUOptions.h"
#include "list.h"
#include "line.h"

#define ignore_blanks (GetOptions().GetIgnoreBlanks())
#define ignore_eol    (GetOptions().GetIgnoreEOL())
#define ignore_case   (GetOptions().GetIgnoreCase())

/******
struct fileline {

        UINT flags;     // see below 

        LPSTR text;     //* null-terminated copy of line text 
        DWORD hash;     //* hashcode for line 
        LINE link;      //* handle for linked line 
        UINT linenr;    //* line number (any arbitrary value) 
};
****/

/* flag values (or-ed) */
#define LF_DISCARD      1       /* if true, alloced from gmem heap */
#define LF_HASHVALID    2       /* if true, hashcode need not be recalced */



/***************************************************************************
 * Function: line_reset
 *
 * Purpose:
 *
 * Clears the link and force recalc of the hash code.
 *
 ***************************************************************************/
void
line_reset(PLINEDESC line)
{
        if (line == NULL) {
                return;
        }

        line->link = NULL;

        line->flags &= ~LF_HASHVALID;
}


/***************************************************************************
 * Function: line_gettext
 *
 * Purpose:
 *
 * Returns a pointer to the line text
 *
 ***************************************************************************/
LPSTR
line_gettext(PLINEDESC line)
{
        if (line == NULL) {
                return(NULL);
        }

        return (line->pText);
}

/***************************************************************************
 * Function: line_gettabbedlength
 *
 * Purpose:
 *
 * Returns the length of line in characters, expanding tabs. 
 *
 ***************************************************************************/
int
line_gettabbedlength(PLINEDESC line, int tabstops)
{
        int length;
        LPSTR chp;

        if (line == NULL) {
                return(0);
        }

        for (length = 0, chp = line->pText; *chp != '\0'; chp++) {
                if (*chp == '\t') {
                         length = (length + tabstops) / tabstops * tabstops;
                } else {
                        length++;
                }
        }
        return(length);
}


/***************************************************************************
 * Function: line_gethashcode
 *
 * Purpose:
 *
 * Returns the hashcode for this line 
 *
 ***************************************************************************/
DWORD
line_gethashcode(PLINEDESC line)
{
        if (line == NULL) {
                return(0);
        }

        if (! (line->flags & LF_HASHVALID)) {


                /* hashcode needs to be recalced */
                line->hash = hash_string(line->pText, ignore_blanks, ignore_eol, ignore_case);
                line->flags |= LF_HASHVALID;
        }
        return (line->hash);
}

/***************************************************************************
 * Function: line_getlink
 *
 * Purpose:
 *
 * Returns the handle for the line that is linked to this line (the
 * result of a successful line_link() operation). This line is
 * identical in text to the linked line (allowing for ignore_blanks).
 *
 ***************************************************************************/
PLINEDESC
line_getlink(PLINEDESC line)
{
        if (line == NULL) {
                return(NULL);
        }

        return((PLINEDESC) line->link);
}

/***************************************************************************
 * Function: line_getlinenr
 *
 * Purpose:
 *
 * Returns the line number associated with this line 
 *
 ***************************************************************************/
UINT
line_getlinenr(PLINEDESC line)
{
        if (line == NULL) {
                return(0);
        }

        return(line->iLineNo);
}

/***************************************************************************
 * Function: line_compare
 *
 * Purpose:
 *
 * Compares two lines and returns TRUE if they are the same.
 *
 ***************************************************************************/
BOOL
line_compare(PLINEDESC line1, PLINEDESC line2)
{
  LPSTR p1, p2;

  /* Assert: At least one of them is not null ??? */

  if ((line1 == NULL) || (line2 == NULL)) 
	{
    /* null line handles do not compare */
    return(FALSE);
  }

  /* check that the hashcodes match */
  if (line_gethashcode(line1) != line_gethashcode(line2)) 
	{
    return(FALSE);
  }

  /* hashcodes match - are the lines really the same ? */
  /* note that this is coupled to gutils\utils.c in definition of blank */
  p1 = line_gettext(line1);
  p2 = line_gettext(line2);

// Japanese friendy
  do 
	{
		if (ignore_blanks) 
		{
			while ( (*p1 == ' ') || (*p1 == '\t')) 
			{
				p1 = CharNext(p1);
			}
			while ( (*p2 == ' ') || (*p2 == '\t')) 
			{
				p2 = CharNext(p2);
			}
		}

    if (ignore_eol) 
		{
      while ( (*p1 == 0x0a) || (*p1 == 0x0d)) 
			{
        p1 = CharNext(p1);
      }
      while ( (*p2 == 0x0a) || (*p2 == 0x0d)) 
			{
        p2 = CharNext(p2);
      }
    }


    if (IsDBCSLeadByte(*p1) && *(p1+1) != '\0'
    &&  IsDBCSLeadByte(*p2) && *(p2+1) != '\0') 
		{
      if (*p1 != *p2 || *(p1+1) != *(p2+1)) 
			{
        return(FALSE);
      }
      p1 += 2;
      p2 += 2;
    } 
		else 
		{
      if (ignore_case)
      {
        if (toupper(*p1) != toupper(*p2)) 
			  {
          return(FALSE);
        }
      }
      else
      {
        if (*p1 != *p2) 
			  {
          return(FALSE);
        }
      }
      p1++;
      p2++;
    }
  } while ( (*p1 != '\0') && (*p2 != '\0'));

  return(TRUE);
}

/***************************************************************************
 * Function: line_link
 *
 * Purpose:
 *
 * Attempts to link two lines and returns TRUE if succesful.
 *
 * This will fail if either line is NULL, or already linked, or if
 * they differ.
 *
 ***************************************************************************/
BOOL
line_link(PLINEDESC line1, PLINEDESC line2)
{
        if ( (line1 == NULL) || (line2 == NULL)) {
                return(FALSE);
        }

        if ( (line1->link != NULL) || (line2->link != NULL)) {
                return(FALSE);
        }

        if (line_compare(line1, line2)) {
                line1->link = line2;
                line2->link = line1;
                return(TRUE);
        } else {
                return(FALSE);
        }
}


/***************************************************************************
 * Function: line_isblank
 *
 * Purpose:
 *
 * Returns TRUE iff line is blank.  NULL => return FALSE 
 *
 ***************************************************************************/
BOOL line_isblank(PLINEDESC line)
{
        if (line==NULL)
			return TRUE;
		
        LPCTSTR string = line->pText;

		while ( (*string == ' ') || (*string == '\t')) {
                string = CharNext(string);
        }

        /* having skipped all the blanks, do we see the end delimiter? */
        return (*string == '\0' || *string == '\r' || *string == '\n');
}

PLINEDESC line_get_first(PLINEDESC lpSpot)
{
   PLINEDESC lp;
	ASSERT(lpSpot);

   lp = lpSpot;

   if (!lp)
      return NULL;

	while (lp->pPrev)
	{
		lp = lp->pPrev;
	}

   return lp;	
}

PLINEDESC line_get_last(PLINEDESC lpSpot)
{
	PLINEDESC lp;
	ASSERT(lpSpot);

   lp = lpSpot;

   if (!lp)
      return NULL;

   while (lp->pNext)
      lp = lp->pNext;

   return lp;
}

PLINEDESC line_get_prev(PLINEDESC line)
{
	return(line->pPrev);
}

PLINEDESC line_get_next(PLINEDESC line)
{
	return(line->pNext);
}

/***************************************************************************
 * Function: hash_string
 *
 * Purpose:
 *
 * Generate a hashcode for a null-terminated ascii string.
 *
 * If bIgnoreBlanks is set, then ignore all spaces and tabs in calculating
 * the hashcode.
 *
 * Multiply each character by a function of its position and sum these.
 * The function chosen is to multiply the position by successive
 * powers of a large number.
 * The large multiple ensures that anagrams generate different hash
 * codes.
 */
DWORD APIENTRY
hash_string(LPSTR string, BOOL bIgnoreBlanks, BOOL bIgnoreEOL, BOOL bIgnoreCase)
{
#define LARGENUMBER     6293815

  DWORD sum = 0;
  DWORD multiple = LARGENUMBER;
  int index = 1;

  while (*string != '\0') 
	{

    if (bIgnoreBlanks) 
		{
      while ( (*string == ' ') || (*string == '\t')) 
			{
              string = CharNext(string);
      }
    }

    if (bIgnoreEOL) 
		{
      while ( (*string == 0x0a) || (*string == 0x0d)) 
			{
              string = CharNext(string);
      }
    }

    if (bIgnoreCase)
    {
      sum += multiple * index++ * (toupper(*string++));
    }
    else
    {
      sum += multiple * index++ * (*string++);
    }
    multiple *= LARGENUMBER;
  }
  return(sum);
}

// MCN
PLINEDESC line_iterate(PLINEDESC line, int iLines)
{
	PLINEDESC line1 = line;

	for (int iLine = 1; iLine < iLines; iLine ++) {

	   if (line1 == NULL)
		   break;

	   line1 = line_get_next(line1);
   }

   return line1;
}