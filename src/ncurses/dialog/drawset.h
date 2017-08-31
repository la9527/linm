/******************************************************************************
 *   Copyright (C) 2005 by la9527                                             *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      * 
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*
 ******************************************************************************/

#ifndef __DRAWSET_H__
#define __DRAWSET_H__

#include "config.h"

#ifdef HAVE_NCURSES_H

#include <ncurses.h>

#else
#include <curses.h>
#endif

#include "define.h"
#include "colorcfgload.h"

namespace MLS {

    extern chtype HLINE;
    extern chtype VLINE;
    extern chtype ULCORNER;
    extern chtype LLCORNER;
    extern chtype URCORNER;
    extern chtype LRCORNER;
    extern chtype LTEE;
    extern chtype RTEE;
    extern chtype BTEE;
    extern chtype TTEE;

/// @brief	Line Code
    enum LINECODE {
        AUTOLINE, CHARLINE, ACSLINE
    };

    extern LINECODE e_nBoxLineCode;

///	\brief	color 지정 및 속성 지정 함수
///	\param	font	font color
///	\param	back	background color
///	\param	win		ncurses window
    void setcol(int font, int back, WINDOW *win = stdscr);

///	\brief	color 지정 및 속성 지정 함수
///	\param	p		ColorEntry
///	\param	win		ncurses window
    inline void setcol(ColorEntry &p, WINDOW *win = stdscr) {
        setcol(p.font, p.back, win);
    }

///	\brief	color 지정 및 속성 지정 함수(반전)
///	\param	p		ColorEntry
///	\param	win		ncurses window
    inline void setrcol(ColorEntry &p, WINDOW *win = stdscr) {
        setcol(p.back, p.font, win);
    }

    void Set_BoxLine(LINECODE nCode);

    void CursesInit(bool bTransparency = false);

    void CursesDestroy();

    void CursesRestart(bool bTransparency = false);

    void MouseInit();

    void MouseDestroy();

    void ScreenClear();

    int Signal_Blocking();

    void Signal_ResizeBlocking();

    void Signal_ResizeUnblocking();

};

#endif
