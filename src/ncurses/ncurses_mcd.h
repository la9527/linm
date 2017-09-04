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

#ifndef __NCURSES_MCD_H__
#define __NCURSES_MCD_H__

#include "define.h"
#include "mcd.h"
#include "strutil.h"
#include "dialog.h"
#include "mlsdialog.h"
#include "colorcfgload.h"
#include "mpool.h"

using namespace MLSUTIL;

namespace MLS
{

class McdDirButton:public Position
{
private:
	Dir*	_pNode;
	bool	_bSelect;
	bool	_bShowCheck;

protected:
	ColorEntry	_tMCDColor;
	ColorEntry	_tLineColor;

protected:
	void	Draw();	

public:
	McdDirButton()
	{
		_pNode = NULL; _bSelect = false; _bShowCheck = false;
		ColorSet();
	}

	void	ColorSet()
	{
		_tMCDColor 	= g_tColorCfg.GetColorEntry("MCD");
		_tLineColor = g_tColorCfg.GetColorEntry("MCDLine");
	}

	void	SetDir(Dir* pDir, bool bSel = false) { _pNode = pDir; _bSelect = bSel; _bShowCheck = true; }
	bool	GetShowCheck() { return _bShowCheck; }
	Dir*	GetNode() { return _pNode; }
};

class NCurses_Mcd:public Mcd, public Form
{
private:
	vector<McdDirButton*>		_vDirButtonList;
	MemoryPool<McdDirButton>		_tMemPoolDirButton;
	int							_nBefMemSize;
	int							_nRowSize;

protected:
	ColorEntry	_tMCDColor;
	ColorEntry	_tMCDLine;
	ColorEntry	_tMCDHighLight;

protected:
	void	DrawInit();
	void 	Draw();

	void	Refresh()
	{
		werase(_pWin);
		Draw();
	}

public:
	bool	_bFocus;

	NCurses_Mcd(const string& sDir = ""):Mcd(sDir)
	{
		_nRowSize = 0;
		_nBefMemSize = 0;
		_bNoOutRefresh = true; // When Refresh funtion, not doupdate() call.
		_bNoViewUpdate = true; // Does not update.
		_bNotDrawBox = true; // Does not draw the box.
		_bFocus = false;
		Init();
	}

	void 	Init();

	bool	MouseEvent(int Y, int X, mmask_t bstate);
	void	Key_PageDown();
	void	Key_PageUp();
};

};

#endif
