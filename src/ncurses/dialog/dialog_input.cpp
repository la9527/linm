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

Input::Input(const string &str, bool bFirstCursorEnd) : Position() {
    y = 0;
    x = 0;
    width = 30;
    height = 1;
    fontcolor = COLOR_WHITE;
    backcolor = 0;
    _bPasswdType = false;
    _bEnter = false;
    _bExit = false;
    _bMouse = false;
    _nFirst = 0;
    _nEnd = 0;
    _nCur = 0;
    SetStr(str, bFirstCursorEnd);
}

void Input::SetFocus(bool bFocus) {
    if (bFocus) {
        Resize();
        Draw();
        curs_set(1); // 커서를 보이게 한다.
    }
}

int Input::DataInput(KeyInfo &tKeyInfoBef) {
    Resize();

    curs_set(1); // 커서를 보이게 한다.
    KeyReader tKeyReader;
    bool bExit = false, bMouse = false;
    bool bTAB = false, bEnter = false, bKeyUp = false, bKeyDown = false;

    _nFirst = 0;
    _nEnd = 0;
    if (_bFirstCursorEnd)
        _nCur = _sWStr.size();
    else
        _nCur = 0;

    if (_bFirstCursorEnd && (int) _sWStr.size() > width - 6)
        _nFirst = _sWStr.size() - width + 3;
    else
        _nFirst = 0;

    if (_nFirst < 0) _nFirst = 0;

    int nBefKey = (int) tKeyInfoBef;

    for (;;) {
        KeyInfo tKeyInfo;

        Draw();
        wredrawln(_pForm->GetWin(), y, 1);

        if (nBefKey != ERR)
            tKeyInfo = tKeyInfoBef;
        else
            tKeyInfo = tKeyReader.Read(_pForm->GetWin());

        nBefKey = ERR;

        LOG_WRITE("InputBox Key [%d]", (int) tKeyInfo);

        switch ((int) tKeyInfo) {
            case KEY_MOUSE:
                bMouse = true;
                break;

            case KEY_UP:
                bKeyUp = true;
                break;
            case KEY_LEFT: {
                _nCur--;
                if (_nCur < 0) _nCur = 0;
                if (_nCur - _nFirst < 3) {
                    _nFirst = _nCur - 3;
                    if (_nFirst < 0) _nFirst = 0;
                }
                continue;
            }

            case KEY_DOWN:
                bKeyDown = true;
                break;
            case KEY_RIGHT: {
                _nCur++;
                if (_nCur > (int) _sWStr.size()) _nCur = (int) _sWStr.size();
                if (_nCur > _nEnd - 3) {
                    if (_nFirst < (int) _sWStr.size() - 5)
                        _nFirst = _nFirst + 1;
                }
                continue;
            }
            case KEY_PPAGE:
            case KEY_HOME:
                _nCur = 0;
                _nFirst = 0;
                continue;
            case KEY_NPAGE:
            case KEY_END:
                _nCur = _sWStr.size();
                if ((int) _sWStr.size() > width - 6)
                    _nFirst = (int) _sWStr.size() - width + 3;
                else
                    _nFirst = 0;
                if (_nFirst < 0) _nFirst = 0;
                continue;
            case KEY_DC:
                if (!_sWStr.empty()) _sWStr.erase(_nCur, 1);
                continue;
            case 127:
            case KEY_BS:
                if (!_sWStr.empty() && _nCur) {
                    _sWStr.erase(_nCur - 1, 1);
                    _nCur--;
                    if (_nCur - _nFirst > 4) {
                        _nFirst = _nFirst - 1;
                        if (_nFirst < 0) _nFirst = 0;
                    }
                }
                continue;
            case KEY_RESIZE:
            case KEY_REFRESH:
                _pForm->Refresh();
                continue;
            case KEY_TAB:
                bTAB = true;
                break;
            case 13:
            case KEY_ENTER:
                bEnter = true;
                break;
            case KEY_ESC:
                bExit = true;
                break;
            case 8: // OntheSpot Patch
                if (strtowstr((string) tKeyInfo).size() != 0) {
                    if (!_sWStr.empty() && _nCur) _sWStr.erase(_nCur - 1, 1);
                    wstring wstr = strtowstr((string) tKeyInfo);
                    if ((int) _sWStr.size() > _nCur)
                        _sWStr = _sWStr.substr(0, _nCur - 1) + wstr + _sWStr.substr(_nCur - 1);
                    else
                        _sWStr = _sWStr + wstr;
                    LOG_WRITE("Input Box write [%s] [%s]", ((string) tKeyInfo).c_str(), wstrtostr(_sWStr).c_str());
                } else {
                    if (!_sWStr.empty() && _nCur) {
                        _sWStr.erase(_nCur - 1, 1);
                        _nCur--;
                        if (_nCur - _nFirst > 4) {
                            _nFirst = _nFirst - 1;
                            if (_nFirst < 0) _nFirst = 0;
                        }
                    }
                }
                break;
            case 27:
            case ERR:
                continue;
            default: {
                if ((int) tKeyInfo > 27)
                    if (strtowstr((string) tKeyInfo).size() != 0) {
                        wstring wstr = strtowstr((string) tKeyInfo);
                        if ((int) _sWStr.size() > _nCur)
                            _sWStr = _sWStr.substr(0, _nCur) + wstr + _sWStr.substr(_nCur);
                        else
                            _sWStr = _sWStr + wstr;
                        _nCur = _nCur + (int) wstr.size();
                        if (_nCur >= _nEnd) _nFirst = _nFirst + (int) wstr.size();
                    }
                continue;
            }
        }
        if (bEnter || bMouse || bExit || bTAB || bKeyDown || bKeyUp) break;
    }
    curs_set(0); // 커서를 보이지 않게 한다.

    if (bMouse) return KEY_MOUSE;
    if (bEnter) return KEY_ENTER;
    if (bExit) return KEY_ESC;
    if (bTAB) return KEY_TAB;
    if (bKeyDown) return KEY_DOWN;
    if (bKeyUp) return KEY_UP;
    return SUCCESS;
}

void Input::SetClear() {
#ifdef __CYGWIN_C__
    _sWStr = "";
#else
    _sWStr = L"";
#endif
    Draw();
}

void Input::Draw() {
    WINDOW *pWin = _pForm->GetWin();

    wattron (pWin, COLOR(fontcolor, backcolor));
    wattron (pWin, A_BOLD);
    wmove(pWin, y, x);
    whline(pWin, ' ', width);

    // 끝에 한칸이 나오는 경우를 자른다.
    int W1 = width;
    if (wstrlen(_sWStr.substr(_nFirst, W1)) > width) {
        while (1) {
            if (wstrlen(_sWStr.substr(_nFirst, W1)) <= width) break;
            W1--;
        }
    }
    _nEnd = _nFirst + W1;
    /****************************/

    string sViewStr = wstrtostr(_sWStr.substr(_nFirst, W1));

    if (_bPasswdType) {
        string sPasswdStr;
        for (int n = 0; n < (int) sViewStr.size(); n++)
            sPasswdStr = sPasswdStr + "*";
        sViewStr = sPasswdStr;
    }

    wprintw(pWin, "%s", sViewStr.c_str());
    wattroff(pWin, A_BOLD);

    // 커서가 있는 곳으로 이동한다.
    int nViewcur = wstrlen(_sWStr.substr(_nFirst, _nCur - _nFirst));
    wmove(pWin, y, x + nViewcur);

    wnoutrefresh(pWin);
}

