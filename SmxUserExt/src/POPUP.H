/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: POPUP.H,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/POPUP.H,v 1.2 2002/11/22 17:16:04 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

//@module popup.hxx - <c PopupMenu> definition | Definition of generic 
// popup menu engine
#ifndef _POPUP_H
#define _POPUP_H

//Callback function for updating the menu
typedef void (CWnd::*UpdateMenuFkt) (CMenu *);

const int MAX_POPUP = 20;
//@class
//
// The PopupMenu class is a generic engine for pop-up menus activated by right-
// mouse button clicks. The pop-up menu is defined as a "normal" menu with the
// resource editor.
class PopupMenu : public CMenu
{
private:
   CMenu *m_menu[MAX_POPUP];
   int    m_menuCount;
   CWnd  *m_pWin;
   BOOL   m_showAll;
   CMenu *m_template;
   UINT  *m_skipItems;
   
public:
   //@cmember Construct popup menu from a menu resource
   PopupMenu(UINT menuResID, BOOL showAll = TRUE, UINT *exclude = NULL);
   //@cmember Display and track popup menu 
   void TrackMenu(CWnd *pWin, CPoint mousePos, UpdateMenuFkt pUpdMenu = NULL);
   ~PopupMenu();

private:
   //@cmember Recursive pop-up menu builder 
   CMenu *MakeMenu(CMenu* pMenuTemplate);
   BOOL KeepItem(UINT id);
};

#endif
