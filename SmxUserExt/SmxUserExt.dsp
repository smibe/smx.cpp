# Microsoft Developer Studio Project File - Name="SmxUserExt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SmxUserExt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SmxUserExt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SmxUserExt.mak" CFG="SmxUserExt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SmxUserExt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SmxUserExt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SmxUserExt - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib\Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\SmxUserExt\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Release\SmxUserExt.lib"

!ELSEIF  "$(CFG)" == "SmxUserExt - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\SmxUserExt\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Debug\SmxUserExt.lib"

!ENDIF 

# Begin Target

# Name "SmxUserExt - Win32 Release"
# Name "SmxUserExt - Win32 Debug"
# Begin Source File

SOURCE=.\src\AccelDlgHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AccelDlgHelper.h
# End Source File
# Begin Source File

SOURCE=.\src\AcceleratorManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AcceleratorManager.h
# End Source File
# Begin Source File

SOURCE=.\src\AccelListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AccelListBox.h
# End Source File
# Begin Source File

SOURCE=.\src\AccelMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AccelMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AccelMgrReg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CJFlatHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CJFlatHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\CJSortClass.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CJSortClass.h
# End Source File
# Begin Source File

SOURCE=.\src\CmdAccelOb.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CmdAccelOb.h
# End Source File
# Begin Source File

SOURCE=.\src\res\combo_bt.bmp
# End Source File
# Begin Source File

SOURCE=.\src\COPYTREE.CPP
# End Source File
# Begin Source File

SOURCE=.\src\COPYTREE.h
# End Source File
# Begin Source File

SOURCE=.\src\CPYSTDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\src\CPYSTDLG.h
# End Source File
# Begin Source File

SOURCE=.\src\Crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Crypt.h
# End Source File
# Begin Source File

SOURCE=.\src\CShellFileOp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CShellFileOp.h
# End Source File
# Begin Source File

SOURCE=.\src\DelayedDirectoryChangeHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DelayedDirectoryChangeHandler.h
# End Source File
# Begin Source File

SOURCE=.\src\Dir.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DIR.h
# End Source File
# Begin Source File

SOURCE=.\src\DirChangeHdl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DirChangeHdl.h
# End Source File
# Begin Source File

SOURCE=.\src\DirectoryChanges.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DirectoryChanges.h
# End Source File
# Begin Source File

SOURCE=.\src\DIRLIST.CPP
# End Source File
# Begin Source File

SOURCE=.\src\DIRLIST.h
# End Source File
# Begin Source File

SOURCE=.\src\DlgDataEx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DlgDataEx.h
# End Source File
# Begin Source File

SOURCE=.\src\File.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FILE.h
# End Source File
# Begin Source File

SOURCE=.\src\FileDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FileDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\src\FILELIST.CPP
# End Source File
# Begin Source File

SOURCE=.\src\FILELIST.h
# End Source File
# Begin Source File

SOURCE=.\src\FILELMT.h
# End Source File
# Begin Source File

SOURCE=.\src\FilePathCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FilePathCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\FormEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FormEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\KeyboardEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KeyboardEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\OXDirDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OXDirDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\OXDirDlg.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\OXDirRes.h
# End Source File
# Begin Source File

SOURCE=.\src\OXDockCtrlBarRes.h
# End Source File
# Begin Source File

SOURCE=.\src\OXDockCtrlBars.RC
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\OXGridEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OXGridEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\OXGridList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OXGridList.h
# End Source File
# Begin Source File

SOURCE=.\src\OXGridList.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\OXGridListRes.h
# End Source File
# Begin Source File

SOURCE=.\src\OXRectTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OXRectTracker.h
# End Source File
# Begin Source File

SOURCE=.\src\ParseFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ParseFile.h
# End Source File
# Begin Source File

SOURCE=.\src\PATH.CPP
# End Source File
# Begin Source File

SOURCE=.\src\PATH.h
# End Source File
# Begin Source File

SOURCE=.\src\PATHITER.CPP
# End Source File
# Begin Source File

SOURCE=.\src\PATHITER.h
# End Source File
# Begin Source File

SOURCE=.\src\POPUP.CPP
# End Source File
# Begin Source File

SOURCE=.\src\POPUP.H
# End Source File
# Begin Source File

SOURCE=.\src\PxShlAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PxShlAPI.h
# End Source File
# Begin Source File

SOURCE=.\src\regex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\regex.h
# End Source File
# Begin Source File

SOURCE=.\src\Registry.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Registry.h
# End Source File
# Begin Source File

SOURCE=.\src\RETURNS.h
# End Source File
# Begin Source File

SOURCE=.\src\SelectVarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SelectVarDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ShellToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ShellToolBar.h
# End Source File
# Begin Source File

SOURCE=.\src\ShExecCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ShExecCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\SingleViewTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SingleViewTemplate.h
# End Source File
# Begin Source File

SOURCE=.\src\SInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SInstance.h
# End Source File
# Begin Source File

SOURCE=.\src\SMXComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SMXComboBox.h
# End Source File
# Begin Source File

SOURCE=.\src\SMXComboEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SMXComboEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxDialog.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxFileList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxFileList.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SMXListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxObjWrap.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxPgSetupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxPgSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxPrnFormView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxPrnFormView.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxPrnTable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxPrnTable.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxPrnView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxPrnview.h
# End Source File
# Begin Source File

SOURCE=.\src\SMXPropertyPg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SMXPropertyPg.h
# End Source File
# Begin Source File

SOURCE=.\src\SMXRes.h
# End Source File
# Begin Source File

SOURCE=.\src\SMXRes.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\SMXSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SMXSheet.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxShell.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxShell.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxShellList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxShellList.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxShellTree.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxShellTree.h
# End Source File
# Begin Source File

SOURCE=.\src\SmxTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SmxTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\SMXUtil.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\TitleTip.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TitleTip.h
# End Source File
# Begin Source File

SOURCE=.\src\ToolbarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ToolbarEx.h
# End Source File
# Begin Source File

SOURCE=.\src\UserToolMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UserToolMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\UserToolsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UserToolsDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Util.h
# End Source File
# Begin Source File

SOURCE=.\src\VarMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\VarMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\WaitDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\WaitDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\XMLDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\XMLDoc.h
# End Source File
# Begin Source File

SOURCE=.\src\XmlExport.cpp
# End Source File
# Begin Source File

SOURCE=.\src\XmlExport.h
# End Source File
# End Target
# End Project
