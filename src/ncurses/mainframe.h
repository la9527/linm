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

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#include "define.h"
#include "dialog.h"
#include "reader.h"
#include "mlsdialog.h"
#include "drawset.h"
#include "ncurses_panel.h"
#include "ncurses_mcd.h"
#include "ncurses_editor.h"
#include "keycfgload.h"
#include "colorcfgload.h"
#include "command.h"
#include "mainframe_view.h"
#include "selection.h"

namespace MLS
{

class	McdExecuteMode
{
public:
	McdExeMode	eMcdExeMode;
	string		sData;

	McdExecuteMode()
	{
		eMcdExeMode = MCD_EXEMODE_NONE;
	}

	void	operator = (McdExecuteMode& tExeMode)
	{
		eMcdExeMode = tExeMode.eMcdExeMode;
		sData = tExeMode.sData;
	}
};

class MainFrame:public Form, public Configurable
{
private:
	DrawTop			_tDrawTop;
	DrawPath		_tDrawPath[2];
	Hint			_tHint;
	DirInfo			_tDirInfo;
	StatusInfo		_tStatusInfo[2];
	ShellCmd			_tShell;

protected:
	bool			_bSplit;
	bool			_bViewType;
	bool			_bShell;
	bool			_bAlwaysRedraw;
	bool			_bHintView;
	int				_nActive;
	string			_sLastPath;
	string			_sLineCodeChange;	
	ViewType		_eViewType[2];
	bool			_bTitleChange;	

	bool			_bScrSync;	// screen sync
	ViewType		_eViewTypeSyncBef[2];
	
	NCurses_Panel	_tPanel[2];
	NCurses_Mcd		_tMcd[2];
	NCurses_Editor	_tEditor[2];
	Command			_tCommand;

	Selection		_tSelection;
	ClipBoard		_tClipboard;

protected:
	void	UpdateConfig();
	
	void	Init();
	void	DrawInit();
	void	DrawStatus();
	void	Draw();
	void	Execute(KeyInfo& tKeyInfo);
	bool	MouseEvent(int Y, int X, mmask_t bstate);

public:
	MainFrame():Form()
	{
		_bSplit = false; _bViewType = false;
		_bNotDrawBox = true;	// not draw a box.
		_nActive = 0;
		_bScrSync = false;
		_eMcdClipCopy = CLIP_NONE;
		_bTitleChange = true;
		x = 0; y = 0;
		width = COLS; height = LINES;
		_bHintView = true;

		UpdateConfig();
		Init();
		Refresh();
	}

	~MainFrame()
	{
		LOG_WRITE("MainFrame Success Exit ...");
	}

	void	SaveConfig();
	void	Split();
	void	NextWindow();	
	void 	Refresh(bool bNoOutRefresh = false);

	Command* 		GetCommand()	{ return &_tCommand; }

	int		GetActiveNum()			{ return _nActive; }

	NCurses_Panel*	GetPanel(int nNum)
	{ 
		if (nNum < 0 && nNum > 1) return NULL;
		return &_tPanel[nNum];
	}

	NCurses_Mcd*	GetMcd(int nNum)
	{ 
		if (nNum < 0 && nNum > 1) return NULL;
		return &_tMcd[nNum];
	}

	NCurses_Editor*	GetEditor(int nNum)
	{ 
		if (nNum < 0 && nNum > 1) return NULL;
		return &_tEditor[nNum];
	}

	ViewType	GetActiveViewType()
	{
		return _eViewType[_nActive];
	}

	void	SetActiveViewType(ViewType	eFormViewType)
	{
		_eViewType[_nActive] = eFormViewType;
	}

	void	SetTitleChange( bool bTitleChange )		{ _bTitleChange = bTitleChange; }

	void	CmdShell(bool bExecute);

	int		GetScreenSync() 			{ return _bScrSync; }
	void	SetScreenSync(bool bSync)
	{
		if (bSync)
		{
			_eViewTypeSyncBef[0] = _eViewType[0];
			_eViewTypeSyncBef[1] = _eViewType[1];
			_eViewType[0] = MCD;
			_eViewType[1] = PANEL;
			_bSplit = true;
			_bViewType = false;
		}
		else
		{
			if (_bScrSync)
			{
				_eViewType[0] = PANEL;
				_eViewType[1] = PANEL;
				_tMcd[0]._bFocus = _tPanel[0]._bFocus = _nActive ? false : true;
				_tMcd[1]._bFocus = _tPanel[1]._bFocus = _nActive ? true : false;
				Refresh(false);
			}
		}
		_bScrSync = bSync;
	}

	void	LineCodeChange()
	{
		if (e_nBoxLineCode == ACSLINE)
			Set_BoxLine(CHARLINE);
		else
			Set_BoxLine(ACSLINE);
	}

	static MainFrame &GetInstance()
	{
		static MainFrame   tMainFrame;
		return tMainFrame;
	}

	void	Exit()	{ _bExit = true; }

	void	DoMcd();

	Selection*	GetSelection() 	{ return &_tSelection; }
	ClipBoard*  GetClip()		{ return &_tClipboard; }

	void	Reload();

// Mcd Copy
protected:
	ClipState		_eMcdClipCopy;
	McdExecuteMode	_tMcdExecuteMode;

public:
	ClipState		GetMcdCopyMode()	{ return _eMcdClipCopy; }
	McdExecuteMode&	GetMcdExeMode()		{ return _tMcdExecuteMode; }

	void	SetMcdExeMode(McdExeMode eMcdExeMode = MCD_EXEMODE_NONE, const string& sData = "")
	{ 
		_tMcdExecuteMode.eMcdExeMode = eMcdExeMode;
		_tMcdExecuteMode.sData = sData;
	}
	
	void	Copy();
	void	Move();
	void	McdClipCopyClear();
	void	SyncDirectory();
};

#define g_tMainFrame	MainFrame::GetInstance()

};

#endif
