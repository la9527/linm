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

#ifndef __NCURSES_PANEL_H__
#define __NCURSES_PANEL_H__

#include "define.h"
#include "mpool.h"
#include "dialog.h"
//#include "file.h"
#include "panel.h"

#include "mlscfgload.h"

using namespace MLSUTIL;

namespace MLS
{

class Dialog_FileBox:public Position
{
public:
	Dialog_FileBox()
	{
		_bFocus = false;
		_nDrawType = 0;
		_nNumber = -1;
		_bShowFileOwner = false;
	}

	void	SetFile(File* pFile, bool bFocus, int nNumber, bool bFileOwner)
	{
		_bFocus = bFocus;
		_nNumber = nNumber;
		_bShowFileOwner = bFileOwner;
		if (pFile)
		{
			_pFile = pFile;
			
			if (_pFile->bDir)
				_pFile->tColor = g_tColorCfg.GetColorEntry("Dir");
		}
	}

	void	SetDrawType(int nDrawType = 0) { _nDrawType = nDrawType; }

	File*	GetFile()	{ return _pFile; }
	int		GetNum()	{ return _nNumber; }

protected:
	// attr, date, time, filename, size 
	void	Draw1(WINDOW* pWin);
	
	// filename, size 
	void	Draw2(WINDOW* pWin);
	
	// filename 만
	void	Draw3(WINDOW* pWin);

	void	Draw();
	

private:
	bool		_bShowFileOwner;
	int			_nDrawType;
	File*		_pFile;
	bool		_bFocus;
	int			_nNumber;
};

class NCurses_Panel:public Panel, public Form
{
private:
	int							_nItemWidth;
	int							_nViewColumn;
	vector<Dialog_FileBox*>		_vDrawFileList;
	MemoryPool<Dialog_FileBox>	_tMemPoolFileBox;
	ColorEntry					_tLineColor;	

	int							_nBefPage; // for reflesh of screen.
	int							_nBefMemSize;	
	uint						_uBefCur;
	bool						_bViewRowFixed; // fixed rows.
	bool						_bShowFileOwner;	/// owner/group show.

protected:
	void	InitDraw();
	void	DrawFirst();
	void	LineDraw();
	void 	Draw();

	void	Refresh()
	{
		werase(_pWin);
		DrawFirst();
		Draw();
		LineDraw();
	}

	void	ParseAndRun(const string &sCmd, bool bPause = false);
	void	Execute(const string& sCmd);
	
public:
	bool				_bFocus;			/// Check this panel focus.
	
	NCurses_Panel():Panel()
	{
		Init();
	}

	~NCurses_Panel() {}

	void	Init()
	{
		_tLineColor = g_tColorCfg.GetColorEntry("Line");
		_nViewColumn = strtoint( g_tCfg.GetValue("Default", "Column") );
		_bNoOutRefresh = !g_tCfg.GetBool("Default", "AlwaysRedraw", true); // When Refresh() call, did not doupdate().
		_bNoViewUpdate = true; // Did not update.
		_bNotDrawBox = true; // Did not draw box.
		_nBefPage = 0;
		_nBefMemSize = 0;
		_bViewRowFixed = false;
		_bFocus = true;
		_bShowFileOwner = false;
		_bChange = true;
	}

	// 0 Auto, 1~5 
	void	SetViewRowFixed(bool bViewRowFixed)	{ _bViewRowFixed = bViewRowFixed; }
	void	SetViewColumn(int nViewColumn) { _nViewColumn = nViewColumn; }
	void	SetViewOwner(bool bViewOwner)  { _bShowFileOwner = bViewOwner; }
	bool	GetViewOwner(void)			   { return _bShowFileOwner; }

	bool	MouseEvent(int Y, int X, mmask_t bstate)
	{
		Dialog_FileBox* pFileBox = NULL;

		int nY = Y - y; int nX = X - x;
		for (int n = 0; n<(int)_vDrawFileList.size(); n++)
		{
			pFileBox = _vDrawFileList[n];
			if (pFileBox->GetNum() != -1)
			{
				if (pFileBox->AreaChk(nY, nX))
				{
					LOG_WRITE("MouseEvent Y [%d] X [%d]", Y, X);
					if (bstate & BUTTON1_CLICKED) 
					{
						int nCur = -1;
						if ((nCur = pFileBox->GetNum()) != -1)
						{
							_uCur = nCur;
						}
					}
					else if (bstate & BUTTON1_DOUBLE_CLICKED) 
					{
						int nCur = -1;
						if ((nCur = pFileBox->GetNum()) != -1)
						{
							_uCur = nCur;
							Key_Enter();
						}
					}
					else if (bstate & BUTTON3_CLICKED)
					{
						int nCur = -1;
						if ((nCur = pFileBox->GetNum()) != -1)
						{
							_uCur = nCur;
							Key_Select();
						}
					}
					#if ( NCURSES_MOUSE_VERSION > 1 )
					
					if ( bstate & BUTTON2_PRESSED )
						LOG_WRITE("BUTTON2_PRESSED !!!");
					if ( bstate & BUTTON2_RELEASED )
						LOG_WRITE("BUTTON2_RELEASED !!!");
					if ( bstate & BUTTON2_DOUBLE_CLICKED )
						LOG_WRITE("BUTTON2_DOUBLE_CLICKED !!!");
					if ( bstate & BUTTON2_TRIPLE_CLICKED )
						LOG_WRITE("BUTTON2_TRIPLE_CLICKED !!!");

					if ( bstate & BUTTON3_PRESSED )
						LOG_WRITE("BUTTON3_PRESSED !!!");
					if ( bstate & BUTTON3_RELEASED )
						LOG_WRITE("BUTTON3_RELEASED !!!");
					if ( bstate & BUTTON3_DOUBLE_CLICKED )
						LOG_WRITE("BUTTON3_DOUBLE_CLICKED !!!");
					if ( bstate & BUTTON3_TRIPLE_CLICKED )
						LOG_WRITE("BUTTON3_TRIPLE_CLICKED !!!");

					if ( bstate & BUTTON4_CLICKED & REPORT_MOUSE_POSITION)
						LOG_WRITE("BUTTON4_CLICKED !!!");
					if ( bstate & BUTTON4_PRESSED & REPORT_MOUSE_POSITION)
						LOG_WRITE("BUTTON4_PRESSED !!!");
					if ( bstate & BUTTON4_RELEASED & REPORT_MOUSE_POSITION)
						LOG_WRITE("BUTTON4_RELEASED !!!");
					if ( bstate & BUTTON4_DOUBLE_CLICKED & REPORT_MOUSE_POSITION)
						LOG_WRITE("BUTTON4_DOUBLE_CLICKED !!!");
					if ( bstate & BUTTON4_TRIPLE_CLICKED & REPORT_MOUSE_POSITION)
						LOG_WRITE("BUTTON4_TRIPLE_CLICKED !!!");
	
					if ( bstate & BUTTON5_CLICKED )
						LOG_WRITE("BUTTON5_CLICKED !!!");
					if ( bstate & BUTTON5_PRESSED )
						LOG_WRITE("BUTTON5_PRESSED !!!");
					if ( bstate & BUTTON5_RELEASED )
						LOG_WRITE("BUTTON5_RELEASED !!!");
					if ( bstate & BUTTON5_DOUBLE_CLICKED )
						LOG_WRITE("BUTTON5_DOUBLE_CLICKED !!!");
					if ( bstate & BUTTON5_TRIPLE_CLICKED )
						LOG_WRITE("BUTTON5_TRIPLE_CLICKED !!!");
					
					if (bstate & BUTTON4_TRIPLE_CLICKED)
					{
						LOG_WRITE("Wheel Up !!!");
						Key_Up();
					}
					
					if (bstate & BUTTON5_TRIPLE_CLICKED & BUTTON_CTRL )
					{
						LOG_WRITE("Wheel Down !!!");
						Key_Down();
					}
					
					LOG_WRITE("NCURSES_MOUSE_VERSION %x %x", bstate, BUTTON5_TRIPLE_CLICKED * BUTTON_CTRL);
					
					#endif
					break;
				}
			}
		}
		_bChange = true;
		return false;
	}
};

};

#endif
