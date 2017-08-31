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

/// @file   command.h
/// @brief 	Command class header file
/// @author	mls programer
/// @date   2004-

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "define.h"
#include "file.h"
#include "keycfgload.h"
#include "ncurses_panel.h"
#include "ncurses_mcd.h"
#include "ncurses_editor.h"

namespace MLS {

    class CmdPanelImp;

    class CmdMcdImp;

    class CmdEditorImp;

/// @brief command Execute class
    class Command {
    protected:
        typedef void (CmdPanelImp::* PanelFun)();

        typedef void (CmdMcdImp::* McdFun)();

        typedef void (CmdEditorImp::* EditorFun)();

        typedef std::map<TypeInfo *, PanelFun> PanelMap;
        typedef std::map<TypeInfo *, McdFun> McdMap;
        typedef std::map<TypeInfo *, EditorFun> EditorMap;

        CmdPanelImp *_pPanelImp;
        CmdMcdImp *_pMcdImp;
        CmdEditorImp *_pEditorImp;

        PanelMap _mapPanelFunc;
        McdMap _mapMcdFunc;
        EditorMap _mapEditorFunc;

    public:
        Command();

        ~Command();

        // 하위클래스에서 실행 내용 추가
        void Init();

        void SetPanel(NCurses_Panel *);

        void SetMcd(NCurses_Mcd *);

        void SetEditor(NCurses_Editor *);

        NCurses_Panel *GetPanel();

        // 커맨드 메시지와 실행명령을 처리한다.
        int Execute(const std::string &sCmd);

        // str을 파싱한 다음 Run으로 실행한다
        int ParseAndRun(const std::string &sCmd, bool bPause);

        // 쉘에 의해서 실행 되는 커맨드
        int Run(const std::string &str, bool bPause = false);
    };

}; // namespace

#endif
