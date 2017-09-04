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

#ifndef __KEYREADER_H__
#define __KEYREADER_H__

#include "config.h"

#ifdef HAVE_NCURSES_H
        #include <ncurses.h>
#else
        #include <curses.h>
#endif

#include "define.h"
#include "strutil.h"
#include "keycfgload.h"

#define		KEY_TAB		9
#define 	KEY_BS		263
#define		KEY_ESC		1000027

#define		KEY_CLEFT	1000001
#define		KEY_CRIGHT	1000002
#define		KEY_CUP		1000003
#define		KEY_CDOWN	1000004

#define		KEY_SUP		1000010
#define		KEY_SDOWN	1000020

using namespace MLSUTIL;

namespace MLS
{

class KeyReader
{
	public:
		KeyReader()
		{
			KeyInfoReload();
		}

		~KeyReader() {}

		void	KeyInfoReload()
		{
			_vKeyList = g_tKeyCfg.GetKeyInfo();
		}

		KeyInfo		Read(WINDOW*	pWin = stdscr, bool bDelay = false);
		int 		NameToDef(KeyInfo& tKeyInfo);

		KeyInfo KeynameToDefine(const string&	sKeyName);
		
	private:
		vector<KeyInfo>		_vKeyList;
};

};

#endif
