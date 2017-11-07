/******************************************************************************
 *   Copyright (C) 2005 by la9527                                             *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 3 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; If not, see <https://www.gnu.org/licenses/>.     *
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

namespace MLS
{

class CmdPanelImp;
class CmdMcdImp;
class CmdEditorImp;

/// @brief command Execute class
class Command
{
protected:
	typedef void (CmdPanelImp::* PanelFun)();
	typedef void (CmdMcdImp::* McdFun)();
	typedef void (CmdEditorImp::* EditorFun)();

	typedef std::map<TypeInfo*, PanelFun>	PanelMap;
	typedef std::map<TypeInfo*, McdFun>		McdMap;
	typedef std::map<TypeInfo*, EditorFun>	EditorMap;

	CmdPanelImp*		_pPanelImp;
	CmdMcdImp*			_pMcdImp;
	CmdEditorImp*		_pEditorImp;

	PanelMap 		_mapPanelFunc;
	McdMap 			_mapMcdFunc;
	EditorMap 		_mapEditorFunc;

public:
	Command();
	~Command();

	// should be add to code to function of child class.
	void Init();

	void SetPanel(NCurses_Panel*);
	void SetMcd(NCurses_Mcd*);
	void SetEditor(NCurses_Editor*);
	NCurses_Panel*	GetPanel();

	// Execute the command as command message.
	int  Execute(const std::string &sCmd);

	// the sCmd parse and execute as Run().
	int	 ParseAndRun(const std::string &sCmd, bool bPause);

	// Executed the command by shell
	int  Run(const std::string &str, bool bPause = false);
};

}; // namespace

#endif
