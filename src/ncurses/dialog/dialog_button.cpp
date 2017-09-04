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

using namespace MLS;

Button::Button(const string& str, int W )  : Position()
{
	x = 0; y = 0; height = 1; width = W;
	fontcolor = COLOR_WHITE;
	backcolor = 0;
	_sMsg = str;
	_MsgPosition = MIDDLE;
	_bFocus = false;
}

Button::Button(int Y, int X, int W )
{
	x = X; y = Y; height = 1; width = W;
	fontcolor = COLOR_WHITE;
	backcolor = 0;
	_MsgPosition = MIDDLE;
	_bFocus = false;
}

Button::Button()
{
	x = 0; y = 0; height = 1; width = 15;
	fontcolor = COLOR_WHITE;
	backcolor = 0;
	_MsgPosition = MIDDLE;
	_bFocus = false;
}

void 	Button::SetSize(int Y, int X, int W)
{
	x = X; y = Y; height = 1; width = W;
}

void 	Button::SetTxtAlign(POSITION	posX)
{
	_MsgPosition = posX;
}

void 	Button::SetMsg(const string& sMsg)
{
	_sMsg = sMsg;
}

void	Button::SetFocus(bool bFocus)
{
	_bFocus = bFocus;
	// current line update.
	Resize();
	Draw();
	wredrawln(_pForm->GetWin(), y, 1);
}

void	Button::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();

	if (_bFocus == true)
		wattron (pWin, COLOR(backcolor, fontcolor));
	else
	{
		wattron (pWin, COLOR(fontcolor, backcolor));
		wattron (pWin, A_BOLD);
	}

	string sMsg;

	if (_MsgPosition == RIGHT)
	{
		int nSize = width-scrstrlen(_sMsg);
		if (nSize > 0 )	sMsg.append(nSize, ' ');
		sMsg = _sMsg;
	}
	else if (_MsgPosition == MIDDLE)
		sMsg = strmid(width, _sMsg);
	else // LEFT
	{
		int nSize = width-scrstrlen(_sMsg);
		sMsg = _sMsg;
		if (nSize > 0 )	sMsg.append(nSize, ' ');
	}

	LOG_WRITE("[%s] [%d]", sMsg.c_str(), width);
	mvwprintw(pWin, y, x, "%s", (char*)sMsg.c_str());
	wattroff(pWin, A_BOLD);

	wnoutrefresh(pWin);
}
