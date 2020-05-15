# LinM

## Introduction

  LinM is a visual file manager.
  
  It's a feature rich full-screen text mode application that allows you to copy, move and
  delete files and whole directory trees, search for files and run commands in the sub-shell.
   
  LinM is best features are its ability to FTP, SFTP, view tar and zip, alz files, and to poke into RPMs for specific files, and DEB package.   
  It's a clone of Mdir, the famous file manager from the MS-DOS age.
  LinM inherits the keyboard shortcuts and the screen layout from Mdir to maximize user-friendliness.
  
  LinM project has not been managed for a long time.
  We are inspired by a lot of encouragement. so it tries to restart the project. Thank you for encouragement.
  
  For bug reports, comments and questions, please email to la9527@daum.net

## Installation Guide

### System Requirements
  
   * ncurses (http://ftp.gnu.org/pub/gnu/ncurses/) (ver 5.3 or higher)
     (http://www.gnu.org/software/ncurses/ncurses.html)	

        > The way to find the 'ncurses' library is the first 'ncursesw', 
        the second is 'ncurses', the third is 'curses'.         

   * cmake (3.x higher)

## Building LinM

  * Supported Platforms
      
      - Linux
      - Apple macOS
      - FreeBSD
      - OpenBSD
      - Solaris
      - AIX      
      - Microsoft Windows
      
  * Build & Tested OS
      
      - Apple macOS Sierra 10.X
      - Ubuntu Linux

  * Make

      - make -f Makefile.cvs

## License

 LinM is distributed under the GNU Version 3.
 See ['LICENSE'] for the detail.

## Changes

v0.9.0 - 2017-08-24

    - build packaging is change from automake to cmake.
    - fix compile warning.
    - Fixed syntax highlighter in internal editor.

