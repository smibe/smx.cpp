/*******************************************************************************
// PROJECT   : FCU - File Compare Utility
// $RCSfile: SECTION.cxx,v $
// 
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/FCU/src/SECTION.cxx,v 1.2 2002/11/22 17:27:13 GeH Exp $
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
* Module Name: SECTION.C
*
* Manages sections of lines, and lists of sections.
*
* Functions:
*
* section_new()
* section_delete()
* section_match()
* section_getfirstline()
* section_setfirstline()
* section_getlastline()
* section_setlastline()
* section_getlink()
* section_getcorrespond()
* section_getstate()
* section_setstate()
* section_getlinecount()
* section_getleftbasenr()
* section_setleftbasenr()
* section_getrightbasenr()
* section_setrightbasenr()
* FindEndOfUnmatched()
* NextNonIngnorable()
* FinEndOfMatched()
* section_makelist()
* section_deletelist()
* FindFirstWithLink()
* section_matchlists()
* section_takesection()
* section_makecomposite()
* AbsorbAnyBlanks()
* section_makectree()
* section_expandanchor()
*
* Comments:
*
* A section is a data type that represents a contiguous block of lines
* of the same state (all unmatched, or all matched to a contiguous block of
* lines). A section can link up matching lines within the section.
*
* Section list functions can make and match lists of sections from lists of
* lines, and create a composite list by combining sections from two lists
* to create a list that 'best represents' the similarities and differences
* between the two lists of lines.
* 
* Assumptions: the lines passed in are on a list (can be traversed with
* List_Next() etc. Line numbering using the section_get*basenr()
* functions work only if lines are numbered sequentially in ascending order.
*
****************************************************************************/
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "tree.h"
#include "state.h"
#include "list.h"
#include "line.h"
#include "section.h"

#include "FCUoptions.h"

#define ignore_blanks  (GetOptions().GetIgnoreBlanks())

class CSectHndl;

/*
 * a section handle (SECTION) is a pointer to one of these structures
 */
struct section {
        PLINEDESC first;             /* first line in section */
        PLINEDESC last;              /* last line in section */

        BOOL bDiscard;          /* true if not alloc-ed on list */

        SECTION link;           /* we match this section */
        SECTION correspond;     /* we correspond to this section, but
                                 * don't match it
                                 */

        int state;              /* compare state for section */

        int leftbase;           /* nr in original left list of first line*/
        int rightbase;          /* nr in original right list of first line*/
};

/* --- function prototypes ------------------------------------------*/

TREE section_makectree(SECTION sec);
BOOL section_expandanchor(SECTION sec1, PLINEDESC line1, SECTION sec2, PLINEDESC line2);
static int GetUnmovedState(int state);



/***************************************************************************
 * Function: section_new
 *
 * Purpose:
 *
 * Makes a new section, given handles to a first and last line.
 *
 * A section must be at least one line long. The lines passed in must be
 * on a list in order.
 *
 * If the list parameter is non-null, we will allocate the section struct
 * on the list. otherwise we will alloc it from gmem_get(hHeap). We remember
 * this in the bDiscard flag for section_delete, so that we only
 * hand back to gmem_free memory that we got.
 **************************************************************************/
SECTION
section_new(PLINEDESC first, PLINEDESC last, LIST list)
{
        SECTION sec;

        /* alloc the sec and remember where we alloc-ed it */
        if (list) {
                sec = (SECTION) List_NewLast(list, sizeof(struct section));
                sec->bDiscard = FALSE;
        } else {
                sec = (SECTION) malloc(sizeof(struct section));
                sec->bDiscard = TRUE;
        }

        sec->first = first;
        sec->last = last;
        sec->link = NULL;
        sec->correspond = NULL;
        sec->state = 0;
        sec->leftbase = 1;
        sec->rightbase = 1;

        return(sec);
}

/***************************************************************************
 * Function: section_delete
 *
 * Purpose:
 *
 * Discard a section. Free all associated memory (not the line list).
 * Free up the section itself if it was not alloc-ed on a list.
 */
void
section_delete(SECTION section)
{
        if (section->bDiscard) {
                free(section);
        }
}

/***************************************************************************
 * Function: section_match
 *
 * Purpose:
 *
 * Match up two sections: match all lines that
 * are unique and identical between the two sections.
 *
 * We use a tree of line handles, keyed by the line hash code. We use a
 * ctree, which keeps a count for multiple identical keys. This allows
 * us to rapidly find lines that are unique within this section.
 * We build two of these trees (one for each line list). For each line
 * that is unique in both trees, we attempt to link the lines.
 *
 * We also attempt to link the first and last line of the section.
 *
 * For each line we successfully link, we spread up and down from
 * this anchor point attempting to link lines.
 *
 * We return true if we linked any lines
 *
 * This routine may be called more than once on the same list of lines.
 * In matching lines we want to find unique, *unmatched* lines: so we only
 * insert lines into the ctree if they are currently unlinked.
 */
BOOL
section_match(SECTION sec1, SECTION sec2)
{
        TREE ctleft, ctright;
        PLINEDESC line, line2;
        BOOL bLinked = FALSE;


        if ((sec1 == NULL) || (sec2 == NULL)) {
                return(FALSE);
        }

        if ((sec1->first == NULL) || (sec2->first == NULL)) {
                return(FALSE);
        }
        /* ASSERT if first is non-null, so is last */

        /* attempt to link the first line of each file, and
         * if matched, expand as long as we keep matching
         */
        bLinked |= section_expandanchor(sec1, sec1->first, sec2, sec2->first);

        /* attempt to link the last lines of each file and
         * expand upwards
         */
        bLinked |= section_expandanchor(sec1, sec1->last, sec2, sec2->last);


        /* build a tree of lines, indexed by the line hashcode.
         * a ctree will hold only the first value of any given key, but
         * it will keep track of the number of items inserted on this key.
         * thus we can keep count of the number of times this line
         * (or at least this hashcode) appears.
         */
        ctleft = section_makectree(sec1);
        ctright = section_makectree(sec2);

        /* for each unlinked line in one list (doesnt matter which), find if
         * appears once only in each list. if so, link, and expand
         * the link to link lines before and after the matching line
         * as long as thecontinue to match.
         */
        for (line = sec1->first; line != NULL; line = line_get_next(line)) {

                if ((line_getlink(line) == NULL) &&
                   (ctree_getcount(ctleft, line_gethashcode(line)) == 1) &&
                   (ctree_getcount(ctright, line_gethashcode(line)) == 1)) {

                        /* line appears exactly once in each list */
                        line2 = * ((PLINEDESC FAR *)ctree_find(ctright,
                                        line_gethashcode(line)));
                        bLinked |= section_expandanchor(sec1, line, sec2, line2);
                }               

                if (line == sec1->last) {
                        break;
                }
        }

        /* delete the ctrees */
        ctree_delete(ctleft);
        ctree_delete(ctright);

        return(bLinked);
} /* section_match */

/***************************************************************************
 * Function: section_getfirstline
 *
 * Purpose:
 *
 * Gets a handle to the first line in this section
 */
PLINEDESC
section_getfirstline(SECTION section)
{
        if (section == NULL) {
                return(NULL);
        }
        return(section->first);
}

void
section_setfirstline(SECTION section, PLINEDESC line)
{
        if (section == NULL) {
                return;
        }
        section->first = line;
}
/***************************************************************************
 * Function: section_getlastline
 *
 * Purpose:
 *
 * Returns a handle to the last line in a section
 */
PLINEDESC
section_getlastline(SECTION section)
{
        if (section == NULL) {
                return(NULL);
        }
        return(section->last);
}


void
section_setlastline(SECTION section, PLINEDESC line)
{
        if (section == NULL) {
                return;
        }
        section->last = line;
}


/***************************************************************************
 * Function: section_getlink
 *
 * Purpose:
 *
 * Returns a handle to the linked section, if any. A linked section
 * is a section whose lines all match the lines in this section
 */
SECTION
section_getlink(SECTION section)
{
        if (section == NULL) {
                return NULL;
        }
        return(section->link);
}

/***************************************************************************
 * Function: section_getcorrespond
 *
 * Purpose:
 *
 * Returns a handle to the corresponding section (a section which
 * corresponds in position to this one, but whose lines do not match).
 */
SECTION
section_getcorrespond(SECTION section)
{
        if (section == NULL) {
                return(NULL);
        }
        return(section->correspond);    
}
/***************************************************************************
 * Function: section_getstate
 *
 * Purpose:
 *
 * Gets the state for this section */
int
section_getstate(SECTION section)
{
        if (section == NULL) 
                return(0);
        return(section->state);
}

/***************************************************************************
 * Function: section_setstate
 *
 * Purpose:
 *
 * Sets the state for this section */
void
section_setstate(SECTION section, int state)
{
        section->state = state;
}

/***************************************************************************
 * Function: section_getlinecount
 *
 * Purpose:
 *
 * Returns the number of lines in the section. Here we assume that
 * lines in the section are number sequentially in ascending order, and we
 * simply look at the first and last line numbers.
 */
int
section_getlinecount(SECTION section)
{
        return(line_getlinenr(section->last) -
                        line_getlinenr(section->first)) + 1;
}

/*
 * -- base line numbers --
 *
 * These functions only apply to sections in the composite list. When creating
 * a composite section, we record the line number of the first line in each
 * of the two sections we built it from. Thus we can calculate the
 * line number of any line in the section in either file it appeared in,
 * by adding the index of the line within the section to the base line
 * number.
 */
int
section_getleftbasenr(SECTION section)
{
        return(section->leftbase);
}

void
section_setleftbasenr(SECTION section, int base)
{
        section->leftbase = base;
}

int
section_getrightbasenr(SECTION section)
{
        return(section->rightbase);
}

void
section_setrightbasenr(SECTION section, int base)
{
        section->rightbase = base;
}


/* --- section list functions -------------------------------------*/

/* Theory of handling blank lines:
|   
|  If ignore_blanks is FALSE then a blank is just another character.
|  If it is TRUE then we will normally include unmatched blanks in whatever
|  section is surrounding them.  It would be nice if we could arrange to
|  never have a section that is only unmatched blanks, but (at least at
|  the start of the file) it can happen.
|
|  Note that there are two DIFFERENT blank handling techniques:
|  In the first phase of the comparison when we are just trying to match up
|  lines, we skip over blank lines both forwards and backwards from an anchor.
|  When we are making real sections for display we only go forwards.
|  This results in a possible anomaly at the top of the whole file where
|  there could be some blanks which do not match and which can only possibly
|  be described as the start of a section.
|  For this reason, we label the sections with their state as early as possible
|  and go by that rather than by the presence or absence of link fields.
|  (It takes some scanning to find a link.  The first line in the section
|  could be a blank).
*/

/***************************************************************************
 * Function: FindEndOfUnmatched
 *
 * Purpose:
 *
 * Returns a PLINEDESC which is the last line in an unmatched section
 * containing (probably starting with) Line.
 * Note that it does not necessarily make progress.
 *
 * As noted above, even if blank lines are being ignored, we don't
 * mind tagging them onto the end of an already unmatching section.
 * This means we carry on until we find the first real link
 */
PLINEDESC FindEndOfUnmatched(PLINEDESC line)
{
        PLINEDESC next;

        for (; ; )
        {       next = line_get_next(line);
                if (next==NULL) return line;
                if (line_getlink(next)!=NULL) return line;
                line = next;
        }
} /* FindEndOfUnmatched */


/***************************************************************************
 * Function: NextNonIgnorable
 *
 * Purpose:
 *
 * An ignorable line is a blank line with no link and ignore_blanks set
 *
 * Given that line is initially not NULL and not ignorable:
 * If line is the last line in the list then return NULL
 * Else If ignore_blanks is FALSE then return the next line after line
 * else return next line which has a link or which is non-blank.
 * If there is no such line then return the last line in the list.
 *
 * Note that this does always make progress (at the cost of
 * sometimes returning NULL).
 */
PLINEDESC NextNonIgnorable(PLINEDESC line)
{       PLINEDESC next;

        next = line_get_next(line);
        if (next==NULL) return NULL;
        for (; ; ) {
                line = next;
                if (  line_getlink(line)!=NULL) return line;
                if (! ignore_blanks)            return line;
                if (! line_isblank(line))       return line;
                next = line_get_next(line);
                if (next==NULL) return line;
        }
} /* NextNonIgnorable */


/***************************************************************************
 * Function: FindEndOfMatched
 *
 * Purpose:
 *
 * Given that line is either linked or an ignorable blank:
 * Return a PLINEDESC which is the last line in a matched section
 * containing (probably starting with) line.
 * This could mean returning the line we were given.
 *
 * If the lines linked to are not consecutive then the section ends.
 * If blanks are being ignored, then any blank line is deemed
 * to match (even if it doesn't match).  In this case we need the
 * links of the lines before and after the blanks to be consecutive
 * in order to carry on.  There could be blank lines on either or both
 * ends of the links.
 */
PLINEDESC FindEndOfMatched(PLINEDESC line)
{
        PLINEDESC next;              /* next non-ignored or linked line */
        PLINEDESC nextlink;          /* next in other file */

        /* The basic algorithm is to set up next and nextlink to point to
           candidate lines.  Examine them.  If they are good then step
           on to them, else return the line one before.
           There are confusion factors associated with the beginning and
           end of the file.
        */

        /* ASSERT( line is either an ignorable blank or else is linked) */

        /* As a section (at least at the start of the file) might start
           with an ignored non-linked blank line, first step over any such
        */
        if( line_getlink(line)==NULL && line_isblank(line) ) {
                next = NextNonIgnorable(line);

                /* There are unfortunately 6 cases to deal with
                   * marks where next will be. * against eof means next==NULL
                   blank(s) refer to ignorable unlinked blanks.
                          A         B        C        D        E        F
                   line-> xxxxx     xxxxx    xxxxx    xxxxx    xxxxx    xxxxx
                         *unlinked  blanks  *linked   blanks  *eof     *blanks
                                   *unlinked         *linked            eof

                   next could be:
                
                      null - case E => return line
                      unlinked ignorable blank - case F => return that blank line
                      unlinked other - cases A,B return prev(that unlinked line)
                      linked - cases C,D continue from that linked line
                */
                if (next==NULL) return line;
                if (line_getlink(next)==NULL) {
                        if (ignore_blanks && line_isblank(next)) {
                                return next;
                        }
                        return line_get_prev(next);
                }

                line = next;
        }

        /* we have stepped over inital blanks and now do have a link */

        for ( ; ; ) {

                next = NextNonIgnorable(line);
                /* Same 6 cases - basically same again */
                if (next==NULL) return line;
                if (line_getlink(next)==NULL) {
                        if (ignore_blanks && line_isblank(next)) {
                                return next;
                        }
                        return line_get_prev(next);
                }

                nextlink = NextNonIgnorable(line_getlink(line));

                /* WEAK LOOP INVARIANT
                   line is linked.
                   next is the next non-ignorable line in this list after line.
                   nextlink is the next non-ignorable line after link(line)
                                        in the other list (could be NULL etc).
                */
                if (line_getlink(next) != nextlink) return line_get_prev(next);

                line = next;
        }
        return line;
} /* FindEndOfMatched */


/***************************************************************************
 * Function: section_makelist
 *
 * Purpose:
 *
 * Make a list of sections by traversing a list of lines. Consecutive
 * linked lines that are linked to consecutive lines are put in a single
 * section. Blocks of unlinked lines are placed in a section.
 * If ignore_blanks is set then we first try to link them as normal.
 * but if they won't link then we just skip over them and keep them
 * in the same section.
 *
 * Left must be set TRUE iff the list of lines is a left hand section.
 * Returns a handle to a list of sections
 */
LIST
section_makelist(PLINEDESC linelist, BOOL left)
{
        PLINEDESC line1, line2;
        LIST sections;
        BOOL matched;
        SECTION sect;

        /* make an empty list of sections */
        sections = List_Create();

        /* for each line in the list */

        Line_TRAVERSE(linelist, line1) {

                /* is it linked ? */

                if( line_getlink(line1) != NULL
                  || ( ignore_blanks && line_isblank(line1))
                  ) {
                        line2 = FindEndOfMatched(line1);
                        matched = TRUE;
                } else {
                        line2 = FindEndOfUnmatched(line1);
                        matched = FALSE;
                }

                /* create the section and add to list */
                sect = section_new(line1, line2, sections);
                sect->state = (matched ? STATE_SAME
                                       : left ? STATE_LEFTONLY
                                              : STATE_RIGHTONLY
                              );

                /* advance to end of section (no-op if 1 line section) */
                line1 = line2;
        }

        return(sections);
} /* section_makelist */



/***************************************************************************
 * Function: section_deletelist
 *
 * Purpose:
 *
 * Delete a list of sections
 *
 * Sections have no dangling pointers, so all we do is delete the list
 */
void    
section_deletelist(LIST sections)
{
        List_Destroy(&sections);
}

/***************************************************************************
 * Function: FindFirstWithLink
 *
 * Purpose:
 *
 * Return the first line in the range first..last
 * which has a link.  Return last if none of them have a link.
 * List_Next must lead from first to last eventually.
 * It is legit for last to be NULL.
 */
PLINEDESC FindFirstWithLink(PLINEDESC first, PLINEDESC last)
{       
        /* The strategy of including blanks on the ENDS of sections rather
           than the start of new sections will mean that this function
           usually strikes gold immediately.  A file with a leading
           blank section is its raison d'etre.
        */
        while (line_getlink(first)==NULL && first!=last)
                first = line_get_next(first);

        if (line_getlink(first)==NULL) {
        }
        return first;
} /* FindFirstWithLink */


/***************************************************************************
 * Function: section_matchlists
 *
 * Purpose:
 *
 * Match up two lists of sections. Establish links between sections
 * that match, and establish 'correspondence' between sections that
 * are in the same place, but don't match.
 *
 * For each pair of corresponding sections, we also call section_match
 * to try and link up more lines.
 *
 * We return TRUE if we made any more links between lines, or false
 * otherwise.
 *
 */
BOOL
section_matchlists(LIST secsleft, LIST secsright)
{
        BOOL bLinked = FALSE;
        SECTION sec1, sec2;

        /* match up linked sections - We know whether a section is
           supposed to link from its state, but we don't know what section
           it links to.  Also we can have sections which are defined to
           be matching but actually contain nothing but ignorable
           blank lines
        */
        
        /*  for each linked section try to find the section  linked to it. */
		for(  sec1= (SECTION) List_First(secsleft)
                   ;  sec1!=NULL          
                   ;  sec1 =  (SECTION) List_Next((LPVOID) sec1)
                   )
        {
                if (sec1->state==STATE_SAME) {
                        PLINEDESC FirstWithLink = FindFirstWithLink(sec1->first, sec1->last);
                        Sect_TRAVERSE( secsright, sec2) {
                                if ( sec2->state==STATE_SAME
                                   && line_getlink(FirstWithLink)
                                        == FindFirstWithLink(sec2->first, sec2->last)) {
                                            break;
                                }
                        }
                        /* sec2 could be NULL if sec1 is all allowable blanks */
                        if (sec2!=NULL) {
                                sec1->link = sec2;
                                sec2->link = sec1;
                        }
                }
        }

        /* go through all unmatched sections. Note that we need to complete
         * the link-up of matching sections before this, since we need
         * all the links in place for this to work.
         */

        Sect_TRAVERSE( secsleft, sec1) {
                SECTION secTemp;

                if (sec1->state == STATE_SAME) {
                        /* skip the linked sections */
                        continue;
                }

                /* check that the previous and next sections, if
                 * they exist, are linked. this should not fail since
                 * two consecutive unlinked sections should be made into
                 * one section
                 */
                secTemp = (SECTION) List_Prev(sec1);
                if (secTemp && secTemp->state!= STATE_SAME) {
                        continue;
                }
                secTemp = (SECTION) List_Next(sec1);
                if (secTemp && secTemp->state!= STATE_SAME) {
                        continue;
                }

                /* find the section that corresponds to this - that is, the
                 * section following the section linked to our previous section.
                 * we could be at beginning or end of list.
                 */
                if (List_Prev(sec1) != NULL) {
                        SECTION secOther;
                        secOther = section_getlink((SECTION) List_Prev(sec1));
                        if (secOther==NULL)
                                continue;

                        sec2 = (SECTION) List_Next(secOther);

                        /* check this section is not linked */
                        if ((sec2 == NULL) || (section_getlink(sec2) != NULL)) {
                                continue;
                        }
                        
                        /* check that the section after these are linked
                         * to each other (or both are at end of list).
                         */
                        if (List_Next(sec1) != NULL) {

                                if (section_getlink((SECTION) List_Next(sec1)) !=
                                    List_Next(sec2)) {
                                        continue;
                                }
                        } else {
                                if (List_Next(sec2) == NULL) {
                                        continue;
                                }
                        }

                } else if (List_Next(sec1) != NULL) {
                        SECTION secOther;
                        secOther = section_getlink((SECTION)List_Next(sec1));
                        if (secOther==NULL)
                                continue;

                        sec2 = (SECTION) List_Prev(secOther);

                        /* check this section is not linked */
                        if ((sec2 == NULL) || (section_getlink(sec2) != NULL)) {
                                continue;
                        }
                        
                        /* check that the section before these are linked
                         * to each other (or both are at start of list).
                         */
                        if ((SECTION) List_Prev(sec1) != NULL) {

                                if (section_getlink((SECTION) List_Prev(sec1)) !=
                                    (SECTION) List_Prev(sec2)) {
                                        continue;
                                }
                        } else {
                                if (List_Prev(sec2) == NULL) {
                                        continue;
                                }
                        }
                } else {
                        /* there must be at most one section in each
                         * file, and they are unmatched. make these correspond.
                         */
                        sec2 = (SECTION) List_First(secsright);
                }


                /* make the correspondence links
                 */
                if ((sec1 != NULL) && (sec2 != NULL)) {
                        sec1->correspond = sec2;
                        sec2->correspond = sec1;
                }

                /* attempt to link up lines */
                if (section_match(sec1, sec2)) {
                        bLinked = TRUE;
                }
        }

        return(bLinked);
} /* section_matchlists */

/***************************************************************************
 * Function: section_takesection
 *
 * Purpose:
 *
 * Add a section to the composite list. Called from make_composites
 * to copy a section, add it to the composite list and set the state,
 * leftbase and rightbase.   Note that the state could be STATE_SAME
 * with a NULL section on the left.  May NOT call with STATE_SAME and
 * a NULL right section!
 *
 */
void
section_takesection(LIST compo, SECTION left, SECTION right, int state)
{
        SECTION newsec;
        SECTION sec{};

        /* select which section is being output, and change the state
         * to indicate it has been output
         */
        switch(state) {
        case STATE_SAME:
                /* both the same. we mark both as output, and
                 * take the right one.  It is possible that the
                 * left one could be NULL (an ignorable blank section)
                 */
				if (left!=NULL) 
        {
          left->state = state;
					section_setlinestate(left);
					left->state = STATE_MARKED;
				}
				
        if (right!= NULL)
        {
          right->state = state;
				  section_setlinestate(right);
          right->state = STATE_MARKED;
        }

        if (GetOptions().GetOption(OCF_InsLeftIdentSect))
          sec = left;
        else
          sec = right;

        break;

        case STATE_LEFTONLY:
        case STATE_MOVEDLEFT:
                sec = left;
				left->state = state;
				section_setlinestate(left);
                left->state = STATE_MARKED;
                break;

        case STATE_RIGHTONLY:
        case STATE_MOVEDRIGHT:
                sec = right;
				right->state = state;
				section_setlinestate(right);
                right->state = STATE_MARKED;
                break;
        }


        /* create a new section on the list */
        newsec = section_new(sec->first, sec->last, compo);

		//GeH: link the new section to the corresponding one
		//Take care: it's a one way link!!!
		newsec->link = sec;

        newsec->state = state;


        if (left != NULL) {
                newsec->leftbase = line_getlinenr(left->first);
        } else {
                newsec->leftbase = 0;
        }

        if (right != NULL) {
                newsec->rightbase = line_getlinenr(right->first);
        } else {
                newsec->rightbase = 0;
        }

} /* section_takesection */

/***************************************************************************
 * Function: section_makecomposite
 *
 * Purpose:
 *
 * Make a composite list of sections by traversing a list of sections.
 *
 * Return a handle to a list of sections.
 *
 * During this, set state, leftbase and rightbase for sections.
 *
 * Comments:
 *
 * This function creates a list that corresponds to the 'best' view
 * of the differences between the two lists. We place sections from the
 * two lists into one composite list. Sections that match each other are only
 * inserted once (from the right list). Sections that match, but in different
 * positions in the two lists are inserted twice, once in each position, with
 * status to indicate this. Unmatched sections are inserted in the correct
 * position.
 *
 * - Take sections from the left list until the section is linked to one not
 *   already taken.
 * - Then take sections from right until we find a section linked to one not
 *   already taken.
 * - If the two sections waiting are linked to each other, take them both
 *   (once- we take the right one and advance past both).
 *
 * - Now we have to decide which to take in place and which to declare
 *   'moved'. Consider the case where the only change is that the first line
 *   has been moved to the end. We should take the first line (as a move),
 *   then the bulk of the file (SAME) then the last line (as a move). Hence,
 *   in difficult cases, we take the smaller section first, to ensure that
 *   the larger section is taken as SAME.
 *
 *   To indicate which section has been output, we set the state field
 *   to STATE_MARKED once we have taken it.   States in left and right
 *   lists are of no further interest once we have built the composite.
 *
 *   Up to this point we have worked off the STATE of a section.  By now
 *   all the section links are in place, so we can use them too.
 */
LIST
section_makecomposite(LIST secsleft, LIST secsright)
{
        SECTION left, right;
        LIST compo;

        /* make an empty list for the composite */
        compo = List_Create();

        left = (SECTION) List_First(secsleft);
        right = (SECTION) List_First(secsright);

        while ( (left != NULL) || (right != NULL)) {

                if (left == NULL) {
                        /* no more in left list - take right section */
                        /* is it moved or just unmatched ? */
                        if (right->link == NULL) {
                                section_takesection(compo, NULL, right, STATE_RIGHTONLY);
                                right = (SECTION) List_Next(right);
                        } else {
                                //section_takesection(compo, right->link, right, STATE_MOVEDRIGHT);
                                right = (SECTION) Unify(right, STATE_MOVEDRIGHT);
                        }
                } else if (right == NULL) {
                        /* right list empty - must be left next */

                        /* is it moved or just unmatched ? */
                        if (left->link == NULL) {
                                section_takesection(compo, left, NULL, STATE_LEFTONLY);
                                left = (SECTION) List_Next(left);
                        } else {
                                //section_takesection(compo, left, left->link, STATE_MOVEDLEFT);
                                left = (SECTION) Unify(left, STATE_MOVEDLEFT);
                        }

                } else if (left->state == STATE_LEFTONLY) {
                        /* unlinked section on left */
                        section_takesection(compo, left, NULL, STATE_LEFTONLY);
                        left = (SECTION) List_Next(left);

                } else if (left->link==NULL) {
                        /* This is an ignorable blank section on the left.
                         * We take ir only if the option is set.
                         */
                        if (GetOptions().GetOption(OCF_InsLeftIdentSect))
                        {
                          section_takesection(compo, left, NULL, left->state);
                        }
                        left = (SECTION) List_Next(left);

                } else if (left->link->state==STATE_MARKED) {
                        /* left is linked to section that is already taken*/
                        //section_takesection(compo, left, left->link, STATE_MOVEDLEFT);
                        left = (SECTION) Unify(left, STATE_MOVEDLEFT);

                } else  if (right->link == NULL) {
                        /* take unlinked section on right
                         * Either unmatched or ignorable blanks
                         */
                        if (right->state != STATE_SAME || !GetOptions().GetOption(OCF_InsLeftIdentSect))
                        {
                          section_takesection(compo, NULL, right, right->state);
                        }
                        right = (SECTION) List_Next(right);
                
                } else if (right->link->state==STATE_MARKED) {
                        /* right is linked to section that's already taken */
                        //section_takesection(compo, right->link, right, STATE_MOVEDRIGHT);
                        right = (SECTION) Unify(right, STATE_MOVEDRIGHT);
                
                } else if (left->link == right) {
                        /* sections match */
                        section_takesection(compo, left, right, STATE_SAME);
                        right = (SECTION) List_Next(right);
                        left = (SECTION) List_Next(left);
                } else {
                        /* both sections linked to forward sections
                         * decide first based on size of sections
                         * - smallest first as a move so that largest
                         * is an unchanged.
                         */
                        if (section_getlinecount(right) > section_getlinecount(left)) {
                                //section_takesection(compo, left, left->link, STATE_MOVEDLEFT);
                                left = (SECTION) Unify(left, STATE_MOVEDLEFT);
                        } else {
                                //section_takesection(compo, right->link, right, STATE_MOVEDRIGHT);
                                right = (SECTION) Unify(right, STATE_MOVEDRIGHT);
                        }
                }
        }

		//GeH: now it could be that we have unified some sections,
		//so just rescan the compo sections
        SECTION ss;
		Sect_TRAVERSE( compo, ss) {
			ASSERT(ss->link);
			section_setlinestate(ss);
			ss->link->state = ss->state;
			section_setlinestate(ss->link);
			ss->first = ss->link->first;
			ss->last = ss->link->last;
		}


        return(compo);
} /* section_makecomposite */

typedef PLINEDESC (APIENTRY * MOVEPROC)(PLINEDESC);

/***************************************************************************
 * Function: AbsorbAnyBlanks
 *
 * Purpose:
 *
 * Update PLINE by making it point to the first non-blank
 * at-or-after from but not after limit.
 * If they are all blank then make it point to limit
 * If from is non-blank then leave it alone.
 * Return TRUE iff PLINE was updated.
 * It is legit for limit to be NULL (meaning end of file).
 */
BOOL AbsorbAnyBlanks(PLINEDESC * from, PLINEDESC limit, BOOL forward)
{       BOOL progress = FALSE;

        while ( (from!=NULL)
              && (line_isblank(*from))
              && (*from!=limit)
              ) {
                if(forward)
					*from = (*from)->pNext;
				else
					*from = (*from)->pPrev;
                progress = TRUE;
        }
        return progress;
} /* AbsorbAnyBlanks */


/***************************************************************************
 * Function: section_expandanchor
 *
 * Purpose:
 *
 * Given an anchor point (two lines that we think should match),
 * try to link them, and the lines above and below them for as long
 * as the lines can be linked (are the same, are unlinked).
 *
 * Return TRUE if we make any links.
 *
 */
BOOL
section_expandanchor(SECTION sec1, PLINEDESC line1, SECTION sec2, PLINEDESC line2)
{
        /* when a line is matched we set bChanges.  If we notice some
         * blank lines, but do NOT link any new non-blank lines, we
         * do NOT set bChanges.  (If we did it would cause a closed
         * loop as they would get noticed again next time.  line_link
         * only returns TRUE if it is a NEW link).
         * At this stage we are only interested in making links, not in
         * the size of the section that results (that fun comes later).
         * therefore trailing blanks at the end of a section are not
         * interesting and we don't look for them.
         */
        BOOL bChanges = FALSE;
        PLINEDESC left, right;

        /* We handle the section limits by using a sentinel which is one
         * past the end of the section.  (If the section ends at the end
         * of the list then the sentinel is NULL).
         */
        PLINEDESC leftend, rightend;
        leftend = line_get_next(sec1->last);
        rightend = line_get_next(sec2->last);

        /* null lines shall not match */
        if ((line1 == NULL) || (line2 == NULL)) {
                return(FALSE);
        }

        /* check all lines forward until fail to link (because null,
         * not matching, or already linked).
         * include the passed in anchor point since this has not
         * yet been linked.
         * If blanks are ignorable then skip over any number of whole
         * blank lines.
         */
        left = line1;
        right = line2;
        for (; ; ) {
                if (line_link(left, right) ) {

                        bChanges = TRUE;
                        left = line_get_next(left);
                        right = line_get_next(right);
                        if (left==leftend || right==rightend) break;
                }
                else if (ignore_blanks){
                        /* even though no match, maybe an ignorable blank? */

                        BOOL moved = FALSE;
                        moved |= AbsorbAnyBlanks(&left, leftend, TRUE);
                        moved |= AbsorbAnyBlanks(&right, rightend, TRUE);
                        if (!moved) break; /* it didn't match and we didn't move on */
                        if (left==leftend || right==rightend) break;
                }
                else break;
        }

        /* check all matches going backwards from anchor point
           but only if it was a real anchor  (could have been
           end-of-section/end-of-file and non-matching).
        */
        if (line_getlink(line1)==NULL) return bChanges;

        left = line_get_prev(line1);
        right = line_get_prev(line2);
        if (left==NULL || right==NULL) return bChanges;

        leftend = line_get_prev(sec1->first);
        rightend = line_get_prev(sec2->first);

        for (; ; ) {
                if (line_link(left, right)) {

                        bChanges = TRUE;
                        left = line_get_prev(left);
                        right = line_get_prev(right);
                        if (left == leftend || right == rightend) break;

                }
                else if (ignore_blanks){
                        /* even though no match, maybe an ignorable blank? */

                        BOOL moved = FALSE;
                        moved |= AbsorbAnyBlanks(&left, leftend, FALSE);
                        moved |= AbsorbAnyBlanks(&right, rightend, FALSE);
                        if (!moved) break; /* it didn't match and we didn't move on */
                        if (left==leftend || right==rightend) break;

                }
                else break;
        }

        return(bChanges);
}


/***************************************************************************
 * Function: section_makectree
 *
 * Purpose:
 *
 * Build a ctree from the lines in the section given
f *
 * Remember that we are only interested in the lines that are
 * not already linked.
 *
 * The value we store in the tree is the handle of the line. the key
 * is the line hash code
 */
TREE
section_makectree(SECTION sec)
{
        TREE tree;
        PLINEDESC line;

        /* make an empty tree */
        tree = ctree_create(NULL);

        for (line = sec->first; line != NULL; line = line_get_next(line)) {
                if (line_getlink(line) == NULL) {
                        ctree_update(tree, line_gethashcode(line),
                                        &line, sizeof(PLINEDESC));
                }
                if (line == sec->last) {
                        break;
                }
        }
        return(tree);
}


/***************************************************************************
 * Function: section_setlinestate
 *
 * Purpose: sets the state for the lines (used for drawing)
 *
 */
void section_setlinestate(SECTION sec)
{
	PLINEDESC line;

    if(sec == NULL)
		return;

	for (line = sec->first; line != NULL; line = line_get_next(line)) 
	{
		//if(line->state == 0)
		line->state = sec->state;
	
		if (line == sec->last) {
				break;
		}
	}
}
	
static int GetUnmovedState(int state)
{
	switch(state) {
		case STATE_MOVEDLEFT:
			return STATE_LEFTONLY;
		case STATE_MOVEDRIGHT:
			return STATE_RIGHTONLY;
		default:
			;
	}
	return(state);
}

SECTION Unify(SECTION sect, int state) 
{
	SECTION sectPrev = (SECTION) List_Prev(sect);
	SECTION sectNext = (SECTION) List_Next(sect);
	if(sectPrev == NULL || sectPrev->first->state == STATE_SAME) {
		//This is first section and it's moved
		//Mark it as left/right-only
		sect->state = GetUnmovedState(state);
		sect->link = NULL;
		return(sect);
	}
	ASSERT(sectPrev->first->state != STATE_SAME);

	if(sectNext && sectNext->state != STATE_SAME) {

		//Unify all tree sections
		sectPrev->last = sectNext->last;
		if(!sectPrev->link)
			sectPrev->link = sectNext->link;
		List_Delete(sect);
		List_Delete(sectNext);
		return((SECTION) List_Next(sectPrev));
	}

	//Just concaternate this section and the previous one
	sectPrev->last = sect->last;
	List_Delete(sect);
	return((SECTION) List_Next(sectPrev));
}

// MCN
int section_exists_line(SECTION sec, PLINEDESC line)
{
    if (sec == NULL || line == NULL) 
		return 0;


	PLINEDESC line1 = section_getfirstline(sec);
	PLINEDESC line2 = section_getlastline(sec);
	ASSERT (line1 != NULL && line2 != NULL);

	if (line_getlinenr(line1) > line_getlinenr(line) ||
	    line_getlinenr(line2) < line_getlinenr(line))
		return 0;
	

	int posLine = 1;

	while (line1 != NULL)
	{
		if (line == line1)
			return posLine;

		if (line1 == line2)
			return 0;

		posLine ++;

		line1 = line_get_next(line1);
	}

	return 0;
}

// MCN
SECTION section_search_in_list(LIST list, PLINEDESC line)
{
    if (list == NULL || line == NULL) 
		return 0;

	SECTION sec;

	for(sec = (SECTION)List_First(list);  
	    sec != NULL;
		sec = (SECTION)List_Next((LPVOID)sec))	
	{		
		if (section_exists_line(sec, line) > 0)
			break;
	}

	return sec;
}

// MCN
SECTION section_get_right_from_comp(SECTION sec)
{
    if (sec == NULL) 
		return NULL;

	if (section_getstate(sec) == STATE_MOVEDLEFT || 
	    section_getstate(sec) == STATE_LEFTONLY)
		return NULL;

	return section_getlink(sec);
}

// MCN
SECTION section_get_left_from_comp(SECTION sec)
{
    if (sec == NULL) 
		return NULL;

	if (section_getstate(sec) == STATE_MOVEDLEFT || 
	    section_getstate(sec) == STATE_LEFTONLY)
		return section_getlink(sec);

	return section_getlink(section_getlink(sec));
}

// MCN
SECTION section_get_comp_from_right(LIST list, SECTION sec)
{
    if (list == NULL || sec == NULL) 
		return 0;

	SECTION sec1;

	for(sec1 = (SECTION)List_First(list);  
	    sec1 != NULL;
		sec1 = (SECTION)List_Next((LPVOID)sec1))	
	{		
		if (section_getlink(sec1) == sec)
			break;
	}

	return sec1;
}

// MCN
SECTION section_get_comp_from_left(LIST list, SECTION sec)
{
    if (list == NULL || sec == NULL) 
		return 0;

	SECTION sec1;

	for(sec1 = (SECTION)List_First(list);  
	    sec1 != NULL;
		sec1 = (SECTION)List_Next((LPVOID)sec1))	
	{		
		if (((section_getstate(sec1) == STATE_MOVEDLEFT || 
			  section_getstate(sec1) == STATE_LEFTONLY) &&			
			  section_getlink(sec1)== sec) ||
			  section_getlink(section_getlink(sec1)) == sec)
			break;
	}

	return sec1;
}