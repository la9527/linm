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

#ifndef __MLS_MENU_H__
#define __MLS_MENU_H__

#include "define.h"
#include "dialog_menu.h"
#include "mainframe.h"

namespace MLS {

    class MlsMenu : public Menu {
    protected:
        void MenuColorSetting();

        void MenuAddCatecory();

        // Reflash 할때 기타 화면을 Reflesh 꼭 넣어야 제대로 화면 나옴.
        void MenuRefresh() {
            g_tMainFrame.Refresh(true);
        }

    protected:
        NCurses_Panel *_pPanel;

    public:
        MlsMenu() : Menu() {}

        NCurses_Panel *SetPanel(NCurses_Panel *p) {
            _pPanel = p;
            return _pPanel;
        }
    };

    class McdMenu : public Menu {
    protected:
        void MenuColorSetting();

        void MenuAddCatecory();

        // Reflash 할때 기타 화면을 Reflesh 꼭 넣어야 제대로 화면 나옴.
        void MenuRefresh() {
            g_tMainFrame.Refresh(true);
        }

    protected:
        NCurses_Mcd *_pMcd;

    public:
        McdMenu() : Menu() { _pMcd = NULL; }

        NCurses_Mcd *SetMcd(NCurses_Mcd *p) {
            _pMcd = p;
            return _pMcd;
        }
    };

    class EditorMenu : public Menu {
    private:
        NCurses_Editor *_pEditor;

    protected:
        void MenuColorSetting();

        void MenuAddCatecory();

        // Reflash 할때 기타 화면을 Reflesh 꼭 넣어야 제대로 화면 나옴.
        void MenuRefresh() {
            g_tMainFrame.Refresh(true);
        }

    public:
        NCurses_Editor *SetEditor(NCurses_Editor *p) {
            _pEditor = p;
            return _pEditor;
        }

        EditorMenu() : Menu() { _pEditor = NULL; }
    };

};

#endif

