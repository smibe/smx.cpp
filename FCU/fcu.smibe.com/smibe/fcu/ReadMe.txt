Readme file for FCU, File Compare Utility

Last updated:     23 November 2001.
Current Version:  1.3.0

0. Description
----------------
FCU is a file compare utility for comparing text files. The compared
files are displayed in three windows: left file window, right file window
and an extra window were the composed file will be displayed.

Different sections are colored with red (left file) and yellow (right file)
background. Anyhow the color for the background can be changed.
User friendly commands allow a quick insertion and deletion of the lines in
the different sections.

In addition it is possible to compare directories including subdirectories,
as well specify an file mask for the files to be compared. The different files
are then displayed in a list control. A simple double click on the line containing
the file will open a 'Compare File' view where the files can be examinated or
merged.


1. Installation
----------------
FCU is distributed as a self installing exe file created with MakeNSIS.
Options and Settings will be saved in the registry.


2. Uninstall
-----------
A uninstall option is supplied. Use the Windows uninstall functionality
(Add/Remove Programs in the control Panel) to uninstall FCU.


3. Files contained in the ZIP archive
--------------------------------------
readme.txt       - this file
licence.txt      - license file
fcu.chm          - help file
FCU.EXE          - executable


4. Changes from V1.09 to V1.2
-----------------------------
* Bug fix: 
  color settings for different sections not saved correctly.

* Command line parameters:
  It is now possible to start fcu with command line parameters
  fcu -f <file1> <file2>  - compare files (file1 with file2)
  fcu -d <dir1> <dir2>    - compare directories (dir1 with dir2)

* Interrupt executions:
  Pressing Ctrl+Break interrupts a still running file or directory compare.

5. Changes from V1.2 to V1.3
-----------------------------
* Bug fix:
  Crash if the compare file windows was minimized and FCU restarted with file compare.

* Edit in the compare file window
  It is now possible to edit the text displayed in the compare file window. Anyhow this 
  feature should not be used to delete or insert the text from the left/right file. You
  should better use the provided commands for that purpose.

* It is not necessary to specify the -f or -d flags when uning command line parameters.
  FCU will check if the first parameter is a file or directory and automatically
  starts the corresponding compare. (Usefull for WinCvs)

* The 'compare file' respective 'compare directory' dialogs are not shoen anymore if
  FCU was started with command line parameters. Anyhow if you still want to have a chance
  to modify the parameters entered in the command line, check the corresponding options
  in the option dialog. FCU will show then the compare dialogs if it was started
  with command line parameters. (Usefull for WinCvs)

* The 'Save As' option when saving the composed file should work now.

* A deleted section is marked by a thin colored line. Sometimes this line would overwrite
  pixels from the text displayed (the '_' character is not shown anymore). This is fixed now!!

* If one of the directories to be compared does not exist a message box is displayed and no
  comparing will be done!

* If the composed file exists and is read only a message box is displayed and the compare 
  file view will not be displayed.

* Added option to ignore the EOL characters (<CR>, <LF>) when comparing, to allow comparing
  of Unix and DOS/Windows files.
  The edit controls used by FCU try to find out what format your ASCII file has (Unix format 
  were lines end with <LF> respective Dos/Windows format were lines end with <CR><LF>). The 
  format of the first line in the file is used as the file format. After that
  all lines are converted to the respective found format. Unfortunately when comparing lines
  the EOL characters were compared as well => files in UNIX format are completly different
  from files in Dos/Windows format. To compare them please check the 'ignore EOL' flag in the
  options dialog.
  NOTE: when merging files the EOL characters are not changed, this means when merging Unix 
  files and dos files this could result in mixed Unix/Dos file format!!

* Bug fix: when using directories as command line parameters the path of the current directory 
  was added to the second directory which resulted in an invalid directory 
  (ex.: "d:\temp\dir\d:\temp\dir1"). This is fixed now.


V1.3f
-----
* Added option for ignoring case. This option is useful when comparing case insensitive files
  (like VisualBasic files).

* Added options for setting font and tab size for the displayed files. THis affects only the 
  edit controls were the files are displayed.

* Bug fix: The filename sorting algorithm was case sensitive. This had sometimes a strange effect:
  the files in different directories woul not be compared, instead they are shown as left only
  and right only.

* Bug fix: When scrolling horizontally the marked text wouldn't be displayed correctly. This
  is fixed now. Anyhow there are still some minor display problems when using block marking.

V1.3h
-----
* Customizable toolbars: 
  For each view the toolbar can now be customized for your own needs.
  Menu : Tools/Customize/Toolbar

* Customizable accelerator keys: 
  Customizing the keyboard affects only the current view.
  That means if you change a key binding in one view it will not affect the oterr views.
  A view means 'Compare file view', 'Compare directory view' or 'Main view'.
  Menu : Tools/Customize/Keyboard. 
  Note: In the compare file view there are key bindings which are used by the edit controls
  (like Ctrl+F for 'Find'). You can't change this key bindings!

* Customizable user tools
  This is a really complex feature. In V1.3h it is not tested extensively, therefore take care
  if you use 'User Tools'. 
  I have added them mainly for calling an extern editor and for 
  getting from or checking files into a version control system.
  There are a number of internal variables which are expanded when the tool is executed.
  In the compare directory view the parameters can expand into a list of values. (Example:
  the $LFilePath expands to a list of left selected files). You can then execute the command
  once for each parameter from the list or once with a list of parameters.
  As well it is possible to create a batch file with optional possibility to execute it.
  For testing purpose it is usefull to just create a batch file and check the contents of
  that file!! 
  I have no description yet for the parameter variables. Just try them out and you will 
  see how they expand.
  Menu: Tools/Customize/User Tools
  NOTE: The user tools will be stored in a file with the extension '.utl' in your 
        "Application Data" directory.

* Copy files
  Displays a dialog for copying files. At the first glance you get probably confused with it
  but I think once you get used to that dialog it's really usefull for syncronizing directories.
  The files are copied using the Windows Shell!

* Added a very simple installation procedure using MakeNSIS (Copyright 1999-2001 Nullsoft, Inc.).

* Added option to use only one instance of FCU if started with command line parameters.
  This option is handy if FCU is used with WinCvs.

* Added option to enable wrapping when the last or first difference is reached. 
  A corresponding message is displayed in the status bar.

* Added handling for WM_MOUSWHEEL.

V1.3.0 (Release version)
------------------------
* Added registration password
* Bug Fix: Ignore the case for subdirectory names.
* Enlarge the edit control for entering comments in the compare directry view.
* Save the composed file always whis dos styl EOL (<CR><LF>).

V1.3.0 (Release version)
------------------------
* Bug fix: Crash on Windows 2000 when opening the compare file view.

V1.4.0
------------------------
* Added context menus in the compare directory view
* Added command line parameters
* Added printing
* Added syncroneous scrolling
* Added line folding
* Added XML export
* Changed help file type to HTML-help
* Many more, you will be surprized


FCU (Version 1.4) is not freeware. Please check the help file for licensing informations.

For problems, sugestions updates please contact:
   fcu@smibe.com
   http://fcu.smibe.com
