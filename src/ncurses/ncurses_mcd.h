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

namespace MLS {

    class McdDirButton : public Position {
    private:
        Dir *_pNode;
        bool _bSelect;
        bool _bShowCheck;

    protected:
        ColorEntry _tMCDColor;
        ColorEntry _tLineColor;

    protected:
        void Draw() {
            WINDOW *pWin = _pForm->GetWin();
            if (_pNode == NULL) return;

            setcol(_tMCDColor, pWin);
            wmove(pWin, y, x);
            whline(pWin, ' ', width);

            if (_pNode->tFile.sFullName == "/") {
                if (_bSelect)
                    setrcol(_tMCDColor, pWin);
                else
                    setcol(_tMCDColor, pWin);
                wprintw(pWin, "/");
            } else {
                if (_bSelect) {
                    setrcol(_tMCDColor, pWin);
                    LOG_WRITE("Selected [%s]", _pNode->tFile.sName.c_str());
                    if (scrstrlen(_pNode->tFile.sName) <= 12)
                        wprintw(pWin, "%-12s", _pNode->tFile.sName.c_str());
                    else
                        wprintw(pWin, "%-11.11s~", _pNode->tFile.sName.c_str());
                } else {
                    LOG_WRITE("Mcd Data [%s]", _pNode->tFile.sName.c_str());

                    if (scrstrlen(_pNode->tFile.sName) <= 12) {
                        setcol(_tMCDColor, pWin);
                        wprintw(pWin, "%s ", _pNode->tFile.sName.c_str());

                        if (!_pNode->vNode.empty()) {
                            setcol(_tLineColor, pWin);
                            whline(pWin, HLINE, 12 - _pNode->tFile.sName.size());
                        }
                    } else {
                        setcol(_tMCDColor, pWin);
                        wprintw(pWin, "%-11.11s~", _pNode->tFile.sName.c_str());
                    }
                }
            }

            wnoutrefresh(pWin);
        }

    public:
        McdDirButton() {
            _pNode = NULL;
            _bSelect = false;
            _bShowCheck = false;
            ColorSet();
        }

        void ColorSet() {
            _tMCDColor = g_tColorCfg.GetColorEntry("MCD");
            _tLineColor = g_tColorCfg.GetColorEntry("MCDLine");
        }

        void SetDir(Dir *pDir, bool bSel = false) {
            _pNode = pDir;
            _bSelect = bSel;
            _bShowCheck = true;
        }

        bool GetShowCheck() { return _bShowCheck; }

        Dir *GetNode() { return _pNode; }
    };

    class NCurses_Mcd : public Mcd, public Form {
    private:
        vector<McdDirButton *> _vDirButtonList;
        MemoryPool<McdDirButton> _tMemPoolDirButton;
        int _nBefMemSize;
        int _nRowSize;

    protected:
        ColorEntry _tMCDColor;
        ColorEntry _tMCDLine;
        ColorEntry _tMCDHighLight;

    protected:
        void DrawInit();

        void Draw();

        void Refresh() {
            werase(_pWin);
            Draw();
        }

    public:
        bool _bFocus;

        NCurses_Mcd(const string &sDir = "") : Mcd(sDir) {
            _nRowSize = 0;
            _nBefMemSize = 0;
            _bNoOutRefresh = true; // Refresh 할때 doupdate 하지 않는다.
            _bNoViewUpdate = true; // 업데이트 하지 않는다.
            _bNotDrawBox = true; // 박스를 그리지 않는다.
            _bFocus = false;
            Init();
        }

        void Init();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Key_PageDown();

        void Key_PageUp();
    };

};

#endif
