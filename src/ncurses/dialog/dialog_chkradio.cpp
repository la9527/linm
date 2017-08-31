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

using namespace MLSUTIL;
using namespace MLS;

CheckRadioButton::CheckRadioButton() : Position() {
    x = 0;
    y = 0;
    height = 1;
    width = 20;
    fontcolor = COLOR_WHITE;
    backcolor = 0;
    _bCheckButton = false;
    _bCheck = false;
    _bRight = false;
}

CheckRadioButton::CheckRadioButton(const string &str, bool bCheckButton) : Position() {
    x = 0;
    y = 0;
    height = 1;
    width = 20;
    fontcolor = COLOR_WHITE;
    backcolor = 0;
    _sMsg = str;
    _bFocus = false;
    _bCheck = false;
    _bRight = false;
    _bCheckButton = bCheckButton;
}

CheckRadioButton::CheckRadioButton(int Y, int X, int W, bool bCheckButton) : Position() {
    y = Y;
    x = X;
    height = 1;
    width = W;
    fontcolor = COLOR_WHITE;
    backcolor = 0;
    _bCheck = false;
    _bCheckButton = bCheckButton;
}

void CheckRadioButton::SetSize(int X, int Y, int W) {
    x = X;
    y = Y;
    height = 1;
    width = W;
}

void CheckRadioButton::SetMsg(const string &sMsg) {
    _sMsg = sMsg;
}

string CheckRadioButton::GetMsg() {
    return _sMsg;
}

void CheckRadioButton::SetCheck(bool bCheck) {
    _bCheck = bCheck;
}

void CheckRadioButton::SetFocus(bool bFocus) {
    _bFocus = bFocus;
    Resize();
    Draw();
    // 현재 라인만 업데이트
    wredrawln(_pForm->GetWin(), y, 1);
}

void CheckRadioButton::SetRightBoxPos(bool bRight) {
    _bRight = bRight;
}

void CheckRadioButton::Draw() {
    WINDOW *pWin = _pForm->GetWin();

    if (_bFocus)
        wattron (pWin, COLOR(backcolor, fontcolor));
    else {
        wattron (pWin, COLOR(fontcolor, backcolor));
        wattron (pWin, A_BOLD);
    }
    wmove(pWin, y, x);
    whline(pWin, ' ', width);

    if (!_bRight) {
        if (_bCheck) {
            if (_bCheckButton)
                mvwprintw(pWin, y, x, "[v] %s", (char *) _sMsg.c_str());
            else
                mvwprintw(pWin, y, x, "[*] %s", (char *) _sMsg.c_str());
        } else
            mvwprintw(pWin, y, x, "[ ] %s", (char *) _sMsg.c_str());
    } else {
        mvwprintw(pWin, y, x, (char *) _sMsg.c_str());

        if (_bCheck) {
            if (_bCheckButton)
                mvwprintw(pWin, y, x + width - 3, "[v]");
            else
                mvwprintw(pWin, y, x + width - 3, "[*]");
        } else
            mvwprintw(pWin, y, x + width - 3, "[ ]");
    }

    wattroff(pWin, A_BOLD);
    wnoutrefresh(pWin);
}
