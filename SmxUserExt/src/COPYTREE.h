/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: COPYTREE.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/COPYTREE.h,v 1.1 2002/10/12 07:20:53 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Class Specification : COXCopyTree
// ==========================================================================

// Header file : copytree.h

// Source : Periphere NV (Frank Melendez)
//			Based upon orginal Microsoft Unsupported Tools on MSDN CD	
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class (does not have any objects)
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation (constructor & Create())
//	NO	Has a message map
//	NO  Needs a resource (template)

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	This class allows the copying of the contents of one directory to another

// Remark:
//		

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////
#ifndef __COPYTREE_H__
#define __COPYTREE_H__

#include "path.h"
#include "dstrlist.h"

class COXCopyTree : public CObject
{
DECLARE_DYNAMIC(COXCopyTree)

// Data members -------------------------------------------------------------
public:
	
protected:
	COXDoubleStringList 	m_dsFiles;		// Files created during copy process			
	COXDoubleStringList 	m_dsDirs;		// Dirs created during copy process

	short 				m_nStatus;		// Status of the copy process
						
	COXDirSpec 			m_SourceDir;
	COXDirSpec 			m_DestDir;
	COXDirSpec 			m_BufferDir;
	
	COXPathSpec 			m_SourcePath;
	COXPathSpec 			m_DestPath;

	COXCopyStatusDialog* 	m_pCpyStatDlg;
						 
private:
	HCURSOR 			m_hCursor;
	
// Member functions ---------------------------------------------------------
public:
	COXCopyTree();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : Contructor of object
	//				It will initialize the internal state
   
 	BOOL DoCopyTree(COXDirSpec SourceDir, COXDirSpec DestDir, BOOL bOnlyContents = TRUE,
		BOOL bCleanUp = FALSE,COXCopyStatusDialog* pCpyStatDlg = NULL);
	// --- In  : SourceDir : the directory to copy the contents from
	//			 DestDir : the directory to copy the contents to
	//			 bOnlyContents : whether to copy the name of the source directory first 
	//			 pCpyStatDlg : pointer to Status dlg. MUST ONLY BE ALLOCATED(new) AND
	//						   NOT YET CREATED!!!!!
	// --- Out : 
	// --- Returns :  suceeded or not
	// --- Effect : copies the contents of one directory to another
 	
 	void WalkTree(WORD wLevel);
	// --- Input : wLevel : bookmark, when wLevel is greater than 0, then the
    //		       		current working directory is a subdirectory of the original
    //      	  		directory.  If wLevel is equal to 0, then the directory is the
    //         	   		original directory and the recursive calls stop
	// --- Out : 
	// --- Returns :
    // --- Effect : finds a subdirectory in the current working directory,
    //		        changes the current working directory to this subdirectory,
    //      		and recusively calls itself until there are no more
    //         		subdirectories
 	       
	void CleanUp();
	// --- In  : 
	// --- Out : 
	// --- Returns :
	// --- Effect : clean up the dirs and files double list and check for 
	//		  		required removal of files based on status.


	void SetHourGlass();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Set an hourglass cursor

	void SetArrow();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Set an arrow cursor

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	virtual ~COXCopyTree();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Destructor of object

protected:      

private:
                   
};

#endif
// ==========================================================================
