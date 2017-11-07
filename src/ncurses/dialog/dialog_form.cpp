#include "dialog.h"

using namespace MLS;

Form::Form(WINDOW* pParentWin): _pParentWin(pParentWin), y(10), x(20), height(10), width(40)
{
	fontcolor = COLOR_WHITE;
	backcolor = 0;
	_pWin = NULL;
	_posX = CURRENT; _posY = CURRENT;
	_bNotDrawBox = false;
	_bNoOutRefresh = false;
	_bNoViewUpdate = false;
	_bExit = false;
}

void	Form::SetWin(WINDOW* pWin)
{
	if (!_pWin)
		_pWin = pWin;
	else
	{
		werase(_pWin);
		wrefresh(_pWin);
		//wclear(_pWin);
		delwin(_pWin);
		_pWin = pWin;
	}
}

void	Form::SetParentWin(WINDOW* pParent)
{
	_pParentWin = pParent;
}

void 	Form::FormResize()
{
	if (_posY == TOP)
		y = 0;
	else if (_posY == MIDDLE)
		y = (LINES / 2) - (height / 2);
	else if (_posY == BOTTOM)
		y = LINES - height;
	
	if (_posX == LEFT)
		x = 0;
	else if (_posX == MIDDLE)
		x = (COLS / 2) - (width/2);
	else if (_posX == RIGHT)
		x = COLS - width;
	
	if (y > LINES) y = LINES;
	if (x > COLS) x = COLS;

	if (y+height > LINES) height = LINES - y;
	if (x+width > COLS) width = COLS - x;
}

void 	Form::Popup()
{
	touchwin(_pWin);
	if (_bNoOutRefresh)
		wnoutrefresh(_pWin);
	else
		wrefresh(_pWin);
}

void 	Form::Hide()
{
	if (_pParentWin)
	{
		touchwin(_pParentWin);
		if (_bNoOutRefresh)
			wrefresh(_pParentWin);
		else
			wnoutrefresh(_pParentWin);
	}
	else
	{
		touchwin(stdscr);
		if (_bNoOutRefresh)
			wrefresh(stdscr);
		else
			wnoutrefresh(stdscr);
	}
}

void	Form::ScrClear()
{
	werase(_pWin);
	wrefresh(_pWin);
}

void Form::MouseProc()
{
	MEVENT event;
	if (getmouse(&event) != ERR)
	{
		if (_nBefMPosY == event.y && _nBefMPosX == event.x &&
			event.bstate & BUTTON1_CLICKED)
		{
			event.bstate = BUTTON1_DOUBLE_CLICKED;
			_nBefMPosY = -1; _nBefMPosX = -1;
		}
		else if (_nBefMPosY == event.y && _nBefMPosX == event.x &&
			event.bstate & BUTTON2_CLICKED)
		{
			event.bstate = BUTTON2_DOUBLE_CLICKED;
			_nBefMPosY = -1; _nBefMPosX = -1;
		}
		else if (_nBefMPosY == event.y && _nBefMPosX == event.x &&
			event.bstate & BUTTON3_CLICKED)
		{
			event.bstate = BUTTON3_DOUBLE_CLICKED;
			_nBefMPosY = -1; _nBefMPosX = -1;
		}
		else
		{
			_nBefMPosY = event.y; _nBefMPosX = event.x;
		}
		
		MouseEvent(event.y - y, event.x - x, event.bstate);
		_BefMmask = event.bstate;
	}
}

void	Form::Show()
{
	Resize();
	FormResize();
	
	if (_pWin == NULL)
	{
		LOG("new win");
	
		_pWin = newwin(y, x, height, width);

		if (_pWin == NULL)
			_pWin = newwin(0, 0, 0, 0);
		
		if (_pWin == NULL)
			LOG("_pWin is NULL !!!!");

		LOG("new win end");
	}

	wresize(_pWin, height, width);
	mvwin(_pWin, y, x);
	
	//LOG("y [%d] x [%d] height [%d] width [%d], COLS [%d] LINES [%d]", y, x, height, width, COLS, LINES);

	DrawFirst();
	if (!_bNotDrawBox) DrawBox();
	Draw();
	
	if (_bNoOutRefresh)
		wnoutrefresh(_pWin);
	else
	{
		// Unuse 'wnoutrefresh' for cursor position.		
		doupdate();
	}

	DrawEtc();
}

void	Form::Close()
{
	if (_pWin)
	{
		//wclear(_pWin);
		werase(_pWin);
		wrefresh(_pWin);
		delwin(_pWin);
		_pWin = NULL;
	}
}

void	Form::Do()
{
	string 		sKey;
	KeyReader	tKeyReader;
	KeyInfo		tKeyInfo;

	for(;;)
	{
		Show();

		tKeyInfo = tKeyReader.Read();

		switch((int)tKeyInfo)
		{
			case KEY_MOUSE:
				MouseProc();
				if (_bExit == true) break;
				continue;
			case KEY_RESIZE:
			case KEY_REFRESH:
				Refresh(_bNoOutRefresh);
				continue;
		}
		
		if ((int)tKeyInfo == -1)
		{
			if (_nKeyErr > 100) break;	
			_nKeyErr++;
			continue;	
		}
		_nKeyErr = 0;

		Execute(tKeyInfo);
		if (_bExit == true) break;
	}

	Close();
}

void Form::Refresh(bool bNoOutRefresh)
{
	_bNoOutRefresh = bNoOutRefresh;
	Show();
}

void Form::DrawBox()
{
	wbkgd(_pWin, COLOR(COLOR_WHITE, backcolor));
	wattron(_pWin, A_BOLD);
	wborder(_pWin, VLINE, VLINE, HLINE, HLINE, ULCORNER, URCORNER, LLCORNER, LRCORNER);
	wattroff(_pWin, A_BOLD);

	if (_sTitle.size() != 0)
	{
		// output title
		setcol(COLOR_BLACK, fontcolor, _pWin);
		wmove(_pWin, 1, 1);
		whline(_pWin, ' ', width-2);
		mvwprintw(_pWin, 1, (width - scrstrlen(_sTitle))/2, "%s", _sTitle.c_str());
		LOG("Title [%s]", _sTitle.c_str());
	}

	wnoutrefresh(_pWin);
}
