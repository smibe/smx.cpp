/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: POPUP.CPP,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/POPUP.CPP,v 1.2 2002/11/22 17:16:04 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

#include "stdafx.h"
#include "popup.h"

//@module popup.cxx - <c PopupMenu> implementation | 
//Implementation of generic engine for pop-up menus


///////////////////////////////////////////////////////////////////////////
// Small helper class - catches ON_UPDATE_COMMAND_UI calls to pop-up menu
// items and helps determine if/how the item should be displayed

class PopupCmdUI : public CCmdUI
{
public:
      UINT m_enable;
      UINT m_check;

      PopupCmdUI(UINT id, int index, CMenu *popup) : CCmdUI()
      {
         m_nID = id;
         m_nIndex = index;    
         m_pMenu = popup;
         m_pOther = NULL;
         m_enable = TRUE;
         m_check = 0;
      }
      virtual void Enable(BOOL b) 
      {
         if(!b) m_enable = FALSE;
      }
      virtual void SetCheck(BOOL b) 
      {
         m_enable = TRUE;
         if(b)
            m_check = b;
      }
};


///////////////////////////////////////////////////////////////////////////
// @mfunc Construct a popup menu based on a template. The menu template
// is drawn with the resource editor, just like any other menu. 

PopupMenu::PopupMenu(

UINT menuID,   //@parm Resource ID for the menu template
BOOL all,      //@parm FALSE to hide disabled items when popup is shown
UINT *exclude) //@parm Optional list of items to exclude from popup
{
   m_showAll   = all;
   m_menuCount = 0;
   m_skipItems = exclude;
   m_template  = new CMenu;
   m_template->LoadMenu(menuID);
}

///////////////////////////////////////////////////////////////////////////
// @mfunc Build a popup submenus from a template. If a submenu is detected,
// this function calls itself recursively and builds a tree of up to
// MAX_POPUP submenus. During construction the owner window's 
// ON_UPDATE_COMMAND_UI message handlers are consulted to determine
// how/if the items in the menus should be displayed.

CMenu *PopupMenu::MakeMenu(

CMenu *sourceMenu) //@parm Ref to menu that is the template for the popup
{
   char itemtext[80+1];

   if(m_menuCount == MAX_POPUP) 
      return NULL;

   CMenu *popup = new CMenu;
   popup->CreatePopupMenu();
   m_menu[m_menuCount++] = popup;
   int sepIndex=0;
   int index = 0;
   
   for(int i=0,j=sourceMenu->GetMenuItemCount();j;i++,--j)
   {
      int id = sourceMenu->GetMenuItemID(i);
      sourceMenu->GetMenuString(i,itemtext,80,MF_BYPOSITION);
      if(id == 0) 
      {
         if(index != sepIndex)
         {
            popup->AppendMenu(MF_SEPARATOR);
            sepIndex = ++index;
         }
      }
      else if(id == -1)
      {
         CMenu *submenu = MakeMenu(sourceMenu->GetSubMenu(i));
         if(submenu && (m_showAll || submenu->GetMenuItemCount() > 0))
         {
            popup->AppendMenu(MF_POPUP,(UINT)submenu->m_hMenu, itemtext);
            index++;
         }
      }
      else if(KeepItem(id))
      {
         UINT flags=0;
         PopupCmdUI cui(id, index, popup);
         m_pWin->OnCmdMsg(id, CN_UPDATE_COMMAND_UI, &cui, NULL);
         if(m_showAll) 
            flags = sourceMenu->GetMenuState(i, MF_BYPOSITION);
         if(m_showAll || cui.m_enable)
         {
            flags |= cui.m_enable ? MF_ENABLED : MF_DISABLED;
            flags |= cui.m_check ? MF_CHECKED : MF_UNCHECKED;
            popup->AppendMenu(flags,id,itemtext);
            index++;
         }
      }     
   }
   return popup;
}


//////////////////////////////////////////////////////////////////////////
// @mfunc On-the fly build and display the context-sensitive popup-menu

void PopupMenu::TrackMenu(

CWnd *pWin,              //@parm The window to show the menu on
CPoint point,            //@parm Client coordinate for menu position
UpdateMenuFkt pUpdMenu)
{
   m_pWin = pWin;
   CMenu *popup = MakeMenu(m_template);
   CRect rect;
   m_pWin->GetWindowRect(rect);
   //point.Offset(rect.TopLeft());

   if (pUpdMenu)
     (m_pWin->*pUpdMenu)(popup);


   if(popup)
      popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                point.x+1,point.y+1,AfxGetMainWnd());
   while(m_menuCount)
   {
      // MR 2210  m_menu[--m_menuCount]->Detach();
      m_menuCount--; // MR 2210
      delete m_menu[m_menuCount];
   }
}



BOOL PopupMenu::KeepItem(UINT id)
{
   if(m_skipItems==NULL) 
      return TRUE;

   for(int i=0; m_skipItems[i]; i++)
      if(id == m_skipItems[i])
        return FALSE;

   return TRUE;
}


PopupMenu::~PopupMenu()
{
//MR 2210   m_template->Detach();
   delete m_template;
}



