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

#include "ncurses_panel.h"
#include "mainframe.h"

using namespace MLSUTIL;
using namespace MLS;

// attr, date, time, filename, size 
void	Dialog_FileBox::Draw1(WINDOW* pWin)
{
	if (_bFocus)
		setrcol(_pFile->tColor, pWin);
	else
		setcol(g_tColorCfg._DefaultColor, pWin);

	mvwhline(pWin, y, x, ' ', width);

	String	sMsg;
	sMsg.AppendBlank(_pFile->sAttr, 11);

	if ( !_bShowFileOwner )
	{
		sMsg.AppendBlank(_pFile->sDate, 9);
		sMsg.AppendBlank(_pFile->sTime, 5);
	}
	else
	{
		if (_pFile->sOwner.size() != 0)
		{
			sMsg.AppendBlank(_pFile->sOwner, 7);
			sMsg.AppendBlank(_pFile->sGroup, 7);
		}
		else
		{
			sMsg.AppendBlank(14, "------ ------");
		}
	}
	mvwprintw(pWin, y, x, "%s", sMsg.c_str());

	if (_pFile->bSelected)
	{
		if (_bFocus)
			setcol(COLOR_WHITE+8, _pFile->tColor.font, pWin);
		else 
			setcol(COLOR_WHITE+8, _pFile->tColor.back, pWin);
		mvwaddch(pWin, y, x+sMsg.size(), '*');
	}
	else
	{
		mvwaddch(pWin, y, x+sMsg.size(), ' ');
	}

	if (_bFocus) 
		setrcol(_pFile->tColor, pWin);
	else 
		setcol(_pFile->tColor, pWin);

	int n = width - sMsg.size() - 12;

	string	sName;
	if (_pFile->bLink)
		sName = _pFile->sName + " -> " + _pFile->sLinkName;
	else
		sName = _pFile->sName;
		
#ifdef __APPLE__
	sName = MLSUTIL::CodeConvert( sName, "UTF-8", "UTF-8-MAC" );
#endif

	if (scrstrlen(sName) >= n)
		mvwprintw(pWin, y, x+sMsg.size()+1, "%s~", scrstrncpy(sName, 0, n-1).c_str());
	else 
		mvwprintw(pWin, y, x+sMsg.size()+1, "%s", sName.c_str());

	if (_pFile->bDir)
	{
		mvwprintw(pWin, y, x+(width-10), "[ SubDir ]");
	}
	else
	{
		if (_pFile->uSize >= 1000000000)
		{
			mvwprintw(pWin, y, x + (width-10), "%9.2f", (float)_pFile->uSize/1073741824);
			if (_bFocus)
				setcol(11, _pFile->tColor.font, pWin);
			else 
				setcol(11, _pFile->tColor.back, pWin);
				
			mvwaddch(pWin, y, x + (width-1), 'G');
		}
		else if (_pFile->uSize >= 10000000)
		{
			mvwprintw(pWin, y, x + (width-10), "%9.2f", (float)(_pFile->uSize/1048576));

			if (_bFocus)
				setcol(11, _pFile->tColor.font, pWin);
			else 
				setcol(11, _pFile->tColor.back, pWin);
			mvwaddch(pWin, y, x + (width-1), 'M');
		}
		else 
			mvwprintw (pWin, y, x + (width-10), "%10s", toregular(_pFile->uSize).c_str());
	}
}

// filename, size 
void	Dialog_FileBox::Draw2(WINDOW* pWin)
{
	if (_bFocus)
		setrcol(_pFile->tColor, pWin);
	else
		setcol(g_tColorCfg._DefaultColor, pWin);

	mvwhline(pWin, y, x, ' ', width);

	if (_pFile->bSelected)
	{
		if (_bFocus)
			setcol(COLOR_WHITE+8, _pFile->tColor.font, pWin);
		else 
			setcol(COLOR_WHITE+8, _pFile->tColor.back, pWin);
		mvwaddch(pWin, y, x, '*');
	}

	if (_bFocus) 
		setrcol(_pFile->tColor, pWin);
	else 
		setcol(_pFile->tColor, pWin);

	int 	n = width - 13;
	string	sName;

	if (_pFile->bLink)
		sName = _pFile->sName + " -> " + _pFile->sLinkName;
	else
		sName = _pFile->sName;
		
#ifdef __APPLE__
	sName = MLSUTIL::CodeConvert( sName, "UTF-8", "UTF-8-MAC" );
#endif

	if (scrstrlen(sName) >= n)
		mvwprintw(pWin, y, x+1, "%s~", scrstrncpy(sName, 0, n-1).c_str());
	else 
		mvwprintw(pWin, y, x+1, "%s", sName.c_str());

	if (_pFile->bDir)
	{
		mvwprintw(pWin, y, x+(width-10), "[ SubDir ]");
	}
	else
	{
		if (_pFile->uSize >= 1000000000)
		{
			mvwprintw(pWin, y, x + (width-10), "%9.2f", (float)_pFile->uSize/1073741824);
			if (_bFocus)
				setcol(11, _pFile->tColor.font, pWin);
			else 
				setcol(11, _pFile->tColor.back, pWin);
				
			mvwaddch(pWin, y, x + (width-1), 'G');
		}
		else if (_pFile->uSize >= 10000000)
		{
			mvwprintw(pWin, y, x + (width-10), "%9.2f", (float)_pFile->uSize/1048576);

			if (_bFocus)
				setcol(11, _pFile->tColor.font, pWin);
			else 
				setcol(11, _pFile->tColor.back, pWin);
			mvwaddch(pWin, y, x + (width-1), 'M');
		}
		else 
			mvwprintw (pWin, y, x + (width-10), "%10s", toregular(_pFile->uSize).c_str());
	}
}

// filename only.
void	Dialog_FileBox::Draw3(WINDOW* pWin)
{
	if (_bFocus)
		setrcol(_pFile->tColor, pWin);
	else
		setcol(g_tColorCfg._DefaultColor, pWin);

	mvwhline(pWin, y, x, ' ', width);

	if (_pFile->bSelected)
	{
		if (_bFocus)
			setcol(COLOR_WHITE+8, _pFile->tColor.font, pWin);
		else 
			setcol(COLOR_WHITE+8, _pFile->tColor.back, pWin);
		mvwaddch(pWin, y, x, '*');
	}

	if (_bFocus)
		setrcol(_pFile->tColor, pWin);
	else 
		setcol(_pFile->tColor, pWin);

	int 	n = width - 1;
	string	sName;

	if (_pFile->bLink)
		sName = _pFile->sName + " -> " + _pFile->sLinkName;
	else
		sName = _pFile->sName;

#ifdef __APPLE__
	sName = MLSUTIL::CodeConvert( sName, "UTF-8", "UTF-8-MAC" );
#endif

	if (scrstrlen(sName) >= n)
		mvwprintw(pWin, y, x+1, "%s~", scrstrncpy(sName, 0, n-1).c_str());
	else 
		mvwprintw(pWin, y, x+1, "%s", sName.c_str());
}

void	Dialog_FileBox::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();

	if (!_pFile) throw Exception("pFile is NULL !!!");
	
	if (_nDrawType == 0) // Auto Type
	{
		if (width > 50)
			Draw1(pWin);
		else if (width > 30)
			Draw2(pWin);
		else
			Draw3(pWin);
	}
	else if (_nDrawType == 1) Draw1(pWin);
	else if (_nDrawType == 2) Draw2(pWin);
	else if (_nDrawType >= 3) Draw3(pWin);
	else
		throw Exception("DrawType Error !!1 %d", _nDrawType);
}


// _nCol, _nRow getsize 
void	NCurses_Panel::InitDraw()
{
	int nSize = _vDirFiles.size();
	const int nMaxlen = 12;

	// Auto Column Mode.
	// Specify the number of files according to column -> Select Mode.
	if (_nViewColumn == 0 || _nViewColumn > 6)
	{
		if (nSize <= height) _nCol = 1;
		else if (nSize <= height*2) _nCol = 2;
		else if (nSize <= height*3) _nCol = 3;
		else if (nSize <= height*4) _nCol = 4;
		else if (nSize <= height*5) _nCol = 5;
		else _nCol = 6;

		// posible max column count :  maxcol = _cols / 12 (minimum 12 letters)
		// minimum size is 12 letters.
		if (_nCol > (width) / nMaxlen) _nCol = (width) / nMaxlen;
	}
	else
	{
		_nCol = _nViewColumn;
	}

	if (!_bViewRowFixed && (nSize + _nCol - 1) / _nCol <= (height-3) )
	{
		_nRow = (nSize + _nCol - 1 ) / _nCol;
	}
	else
	{
		if (!_bViewRowFixed)
			_nRow = height-3;
		else
			_nRow = height-2;
	}
	
	int nNewSize = _nCol*_nRow;
	if (_nBefMemSize != nNewSize)
	{
		_tMemPoolFileBox.Clear();
		_vDrawFileList.clear();
		
		for (int n = 0; n < _nCol*_nRow; n++)
		{
			Dialog_FileBox& tFileBox = _tMemPoolFileBox.Get();
			tFileBox = Dialog_FileBox();
			_vDrawFileList.push_back(&tFileBox);
		}
	}
	_nBefMemSize = _vDrawFileList.size();
}

void	NCurses_Panel::DrawFirst()
{
	if (_nBefPage != _nPage )
	{
		_nBefPage = _nPage;
		_bChange = true;
	}

	if (_bChange)
	{
		werase(_pWin);
		wbkgd(_pWin, COLOR(g_tColorCfg._DefaultColor.font, g_tColorCfg._DefaultColor.back));
		
		setcol(_tLineColor, _pWin);
		mvwaddch (_pWin, 0, 0, ULCORNER);
		mvwaddch (_pWin, 0, width-1, URCORNER);
		mvwaddch (_pWin, _nRow+1, 0, LLCORNER);
		mvwaddch (_pWin, _nRow+1, width-1, LRCORNER);
		mvwhline(_pWin, 0, 1, HLINE, width-2);
		mvwvline(_pWin, 1, 0, VLINE, _nRow);
		mvwhline(_pWin, _nRow+1, 1, HLINE, width-2);
		mvwvline(_pWin, 1, width-1, VLINE, _nRow);
	}
}

void	NCurses_Panel::LineDraw()
{
	// Line draw and split line.
	if (_bChange)
	{
		setcol(_tLineColor, _pWin);
		for (int i=1; i<_nCol; i++)
			for (int j=1; j<_nRow+1; j++)
				mvwaddch (_pWin, j, i * (_nItemWidth + 2), VLINE);
	}

	// Find Section.
	setcol(g_tColorCfg._DefaultColor);
	if (!_sStrSearch.empty())
		mvwprintw(_pWin, 0, width-22, "[%-20s]", _sStrSearch.c_str());
	//mvwprintw(_pWin, 0, 0, "[%d %d %d %d %d]", _uCur, _nPage, _nBefPage, _nCol, _nRow);

	wnoutrefresh(_pWin);
}

void NCurses_Panel::Draw()
{
	InitDraw();

	int				nNum = 0;
	Dialog_FileBox* pFileBox = NULL;
	File*	pFile = NULL;

	if ( _nCol != 0 && _nRow != 0 )
		_nPage = _uCur / (_nCol * _nRow);

	int nCur = (_nCol*_nRow)*_nPage;
	LOG_WRITE("_uCur [%d] _nPage [%d]", _uCur, _nPage );
	_nItemWidth = (width / _nCol) - 2;

	for (int nCol = 0; nCol < _nCol; nCol++)
	{
		for (int nRow = 1; nRow < _nRow+1; nRow++)
		{
			pFileBox = _vDrawFileList[nNum];
			if (nCur < (int)_vDirFiles.size())
			{
				pFile = _vDirFiles[nCur];
				if (_bChange)
				{
					pFileBox->SetForm((Form*)this);
					pFileBox->height = 1;
					pFileBox->width = _nItemWidth;
					pFileBox->x = 1+(nCol * (pFileBox->width+2));
					pFileBox->y = nRow;
					if (nCur == (int)_uCur && _bFocus)
					{
						LOG_WRITE("Cur Select [%s]", pFile->sName.c_str());
						pFileBox->SetFile(pFile, true, nCur, _bShowFileOwner);
					}
					else
						pFileBox->SetFile(pFile, false, nCur, _bShowFileOwner);
					pFileBox->Show();
				}
				else
				{
					pFileBox->SetForm((Form*)this);
					if (nCur == (int)_uCur)
					{
						if (_bFocus)
							pFileBox->SetFile(pFile, true, nCur, _bShowFileOwner);
						else
							pFileBox->SetFile(pFile, false, nCur, _bShowFileOwner);
					}
					if (nCur == (int)_uBefCur)
					{
						pFileBox->SetFile(pFile, false, nCur, _bShowFileOwner);
					}
					if (nCur == (int)_uCur || nCur == (int)_uBefCur)
						if (_uCur != _uBefCur)
							pFileBox->Show();
				}
			}
			else
				pFileBox->SetFile(NULL, false, -1, _bShowFileOwner);
			nCur++; nNum++;
		}
	}
	_uBefCur = _uCur;
	LineDraw();
}

void	NCurses_Panel::ParseAndRun(const string &sCmd, bool bPause)
{
	(g_tMainFrame.GetCommand())->ParseAndRun(sCmd, bPause);
}

void	NCurses_Panel::Execute(const string& sCmd)
{
	(g_tMainFrame.GetCommand())->Execute(sCmd);
}
