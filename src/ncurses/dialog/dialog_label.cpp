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

#include "dialog.h"
#include "strlinetoken.h"

using namespace MLSUTIL;
using namespace MLS;

Label::Label(const string& sMsg)
{
	x = 0; y = 0; height = 1; width = scrstrlen(sMsg)+2;
	fontcolor = COLOR_WHITE;
	backcolor = 0;
	_sMsg = sMsg;
	_TextPos = LEFT;
	_ObjPosY = CURRENT; _ObjPosX = CURRENT;
}

Label::Label(POSITION Y, POSITION X, const string& sMsg)
	: _sMsg(sMsg)
{
	height = 1;
	width = scrstrlen(sMsg);
	_ObjPosY = Y; _ObjPosX = X;
}

Label::Label(int Y, POSITION X, const string& sMsg)
	: _sMsg(sMsg)
{
	height = 1;
	width = scrstrlen(sMsg);
	_ObjPosX = X; y = Y;
}

Label::Label(POSITION  Y, int X, const string& sMsg)
	: _sMsg(sMsg)
{
	height = 1;
	width = scrstrlen(sMsg);
	_ObjPosY = Y; x = X;
}

Label::Label(int  Y, int X, const string& sMsg)
	: _sMsg(sMsg)
{
	height = 1;
	width = scrstrlen(sMsg);
	y = Y; x = X;
	_ObjPosY = CURRENT; _ObjPosX = CURRENT;
}

Label::Label()
{
	x = 0; y = 0; height = 1; width = 10;
	fontcolor = COLOR_WHITE;
	backcolor = 0;
	_TextPos = LEFT;
	_ObjPosY = CURRENT; _ObjPosX = CURRENT;
}

void Label::SetSize(int nHeight, int nWidth, POSITION textPos)
{
	height = nHeight; width = nWidth;
	_TextPos = textPos;
}

void Label::SetMsg(const string& sMsg)
{
	_sMsg = sMsg;
	width = scrstrlen(sMsg);
}

void Label::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();

	if (_pForm->width <= width)
	{
		width = _pForm->width-4;
		Resize();
	}

	LOG_WRITE("Label Draw width [%d] [%d]", width, _pForm->width);

	StrLineToken	tLineToken(strtowstr(_sMsg), width);

	if ((int)tLineToken.LineSize() > height)
	{
		height = tLineToken.LineSize();
		Resize();
	}	

	wattron (pWin, COLOR(fontcolor, backcolor));
	wattron (pWin, A_BOLD);

	for (int n = 0; n < height; n++)
		mvwhline(pWin, y+n, x, ' ', width);

	string 	sViewStr;
	int		viewX = 0;

	for (int n = 0; n < (int)tLineToken.LineSize(); n++)
	{
		sViewStr = wstrtostr(tLineToken.GetLineStr((uint)n));

		if (_TextPos == LEFT)
			viewX = 0;
		else if (_TextPos == MIDDLE)
			viewX = (width / 2) - (scrstrlen(sViewStr) / 2);
		else if (_TextPos == RIGHT)
		{
			viewX = width - scrstrlen(sViewStr);
			if (viewX < 0) viewX = 0;
		}
		else
			viewX = 0;

		mvwprintw(pWin, y+n, x+viewX, (char*)sViewStr.c_str());
		LOG_WRITE("y [%d] viewX [%d] [%s]", y+n, viewX, sViewStr.c_str());
	}
	wattroff(pWin, A_BOLD);
	wnoutrefresh(pWin);
}
