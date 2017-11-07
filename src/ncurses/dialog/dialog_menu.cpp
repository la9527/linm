#include "dialog_menu.h"

using namespace MLS;

void	MenuItem::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();

	if (_sName.empty())
	{
		// if menu name is empty, change to line.
		setcol(0, _tMenuColor.back, pWin);
		mvwhline(pWin, y, x+1, HLINE, width-2);
	}
	else
	{
		if (_bCur == false)
			setcol(_tMenuColor, pWin);
		else
			setcol(7, 0,  pWin);

		mvwhline(pWin, y, x+1, ' ', width-2);
		if (_bEnable == false)  setcol(8, _tMenuColor.back, pWin);
		mvwprintw(pWin, y, x+2, "%s", _sName.c_str());
		
		if (!_sCtlKey.empty())
		{
			// get Ctrl, Alt information
			if (_bCur == false)
			{
				if (_bEnable == false)
					setcol(8, _tMenuColor.back, pWin);
				else
					setcol(_tMenuAColor, pWin);
			}
			else
				setcol(7, 0,  pWin);
			mvwprintw(pWin, y, x+width-2-scrstrlen(_sCtlKey), "%s", _sCtlKey.c_str());
		}
	}
	//wnoutrefresh(pWin);
}

void MenuCategory::AddItem(	const string &n, 
					const string &c, 
					bool b)
{
	MenuItem	tMenuItem(n, c, b);
	LOG("Menu AddItem Cmd [%s] Key [%s] [%d]", c.c_str(), g_tKeyCfg.CmdToKeyName( c, _eViewType ).c_str(), _eViewType);
	tMenuItem.SetCtlKey( g_tKeyCfg.CmdToKeyName( c, _eViewType ) );
	tMenuItem.x = 0; tMenuItem.y = _vItem.size()+1;
	tMenuItem.height = 1; tMenuItem.width = width;
	_vItem.push_back(tMenuItem);
}

void MenuCategory::SetDisable(const std::string& sStr)
{
	for(int c=0; c < (int)_vItem.size(); c++)
	{
		if (_vItem[c].GetCmd() == sStr) _vItem[c].SetEnable(false);
	}
}

void MenuCategory::SetCursor(const int nIdx)
{
	if (nIdx < 0 || (int)_vItem.size() <= nIdx) return;
	if (_vItem[nIdx].GetName().empty()) return;
	_nIndex = nIdx;
}

void MenuCategory::DrawFirst()
{
	werase(_pWin);
	wbkgd(_pWin, COLOR(_tMenuColor.font, _tMenuColor.back));
	
	setcol(15, _tMenuColor.back, _pWin);
	mvwaddch (_pWin, 0, 0, ULCORNER);
	mvwaddch (_pWin, 0, width-1, URCORNER);
	mvwaddch (_pWin, height-1, 0, LLCORNER);
	mvwaddch (_pWin, height-1, width-1, LRCORNER);
	mvwhline(_pWin, 0, 1, HLINE, width-2);
	mvwvline(_pWin, 1, 0, VLINE, height-2);
	mvwhline(_pWin, height-1, 1, HLINE, width-2);
	mvwvline(_pWin, 1, width-1, VLINE, height-2);
}

void MenuCategory::Draw()
{
	for (int n = 0; n < (int)_vItem.size(); n++)
	{
		MenuItem tMenuItem = _vItem[n];
		tMenuItem.SetForm((Form*)this);
		tMenuItem.Color(_tMenuColor, _tMenuAColor);
		if (_nIndex == n)
			tMenuItem.SetCur(true);
		else
			tMenuItem.SetCur(false);
		tMenuItem.Show();
	}
}

void MenuCategory::CursorUp()
{
	if (_vItem.empty()) return;

	_nIndex = (_nIndex + _vItem.size() - 1) % _vItem.size();

	// if null string, to next line. if menu all string is empty, danger of infinite loop.
	if (_vItem[_nIndex].GetName().empty()) CursorUp();
	if (_vItem[_nIndex].GetEnable() == false) CursorUp();
}

void MenuCategory::CursorDown()
{
	if (_vItem.empty()) return;

	_nIndex = (_nIndex + 1) % _vItem.size();

	// if null string, to next line. if menu all string is empty, danger of infinite loop.
	if (_vItem[_nIndex].GetName().empty()) CursorDown();
	if (_vItem[_nIndex].GetEnable() == false) CursorDown();
}

bool MenuCategory::MouseEvent(int Y, int X, mmask_t bstate)
{
	int nY = Y - y; int nX = X - x;
	for (int n = 0; n<(int)_vItem.size(); n++)
	{
		if (_vItem[n].AreaChk(nY, nX))
		{
			_nIndex = n;
			return true;
		}
	}
	return false;
}

bool	Menu::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (Y == 0)
	{
		for (int t = 0; t < (int)_vItem.size(); t++)
		{
			int width = scrstrlen(_vItem[t].GetName());
			if (_vItem[t].x <= X && X <= _vItem[t].x + width)
			{
				_nNum = t;
				MenuRefresh();
				return true;
			}
		}
	}
	else
	{
		if (_vItem[_nNum].MouseEvent(Y, X, bstate) == false)
		{
			// if exist other things select, simliar ESC.
			_bESC_Exit = true;
			_bExit = true;
		}
		else
		{
			if (bstate & BUTTON1_DOUBLE_CLICKED)
			{
				_bExit = true;
			}
		}
	}
	return false;
}

Menu::Menu()
{ 
	_nNum = 0; 
	//_bNoOutRefresh = true; // when Refresh, not doupdate
	//_bNoViewUpdate = true; // not update.
	_bNotDrawBox = true; // not draw box.
	_bESC_Exit = false;
	_bExit = false;
	x = 0; y = 0;
	height = 1; width = COLS-1;
}

///	\brief	Add to menuCategory
///	\param	p	MenuCategory
void Menu::AddItem(MenuCategory& p)
{
	// if not things menu, position X is 4.
	if (_vItem.empty())
		p.x = 4;
	// if exist menu, set position refer to privous menu position and chars number.
	else 
	{
		p.x = _vItem.back().x + scrstrlen(_vItem.back().GetName()) + 2;
	}

	p.y = 1;
	p.width = 26;
	p.height = p.GetItemSize() + 2;

	p.MenuColor(_tColorMenu, _tColorMenuA);
	
	// Add to menu data.
	_vItem.push_back(p);
}

string	Menu::GetCurCmd()
{
	if (_bESC_Exit == false) { return _vItem[_nNum].GetCmd(); }
	return "";
}

void	Menu::Create()
{
	_vItem.clear();

	MenuColorSetting();
	MenuAddCatecory();

	// change to disable data.
	for(int c=0; c<(int)_vCmdDisable.size(); c++)
		for(int b=0; b<(int)_vItem.size(); b++)
			_vItem[b].SetDisable(_vCmdDisable[c]);

	_bESC_Exit = false;
	_bExit  = false;
}

void Menu::DrawFirst()
{
	x = 0; y = 0;
	height = 1; width = COLS-1;

	// draw main menu.
	// ex. "Mls  Run  File  Directory  Archive  Option" menu.
	setcol(_tColorMenu, _pWin);
	wmove(_pWin, y, x);
	whline(_pWin,' ', width);

	for (int t=0; t<(int)_vItem.size(); t++)	
	{
		mvwprintw(_pWin, y, _vItem[t].x, " %s ", _vItem[t].GetName().c_str());
		LOG("MENU :: %-11s", _vItem[t].GetName().c_str());
	}

	// main menu are first char is yellow. (for division)
	setcol(_tColorMenuA, _pWin);
	for (int t=0; t<(int)_vItem.size(); t++)
	{
		mvwprintw(_pWin, y, _vItem[t].x+1,
				(char*)scrstrncpy(_vItem[t].GetName(), 0, 1).c_str());
	}

	// if selected menu, write to string on black background.
	setcol(7, 0, _pWin);
	mvwprintw(_pWin, y, _vItem[_nNum].x, " %s ", _vItem[_nNum].GetName().c_str());

	// if selected menu, first char is yellow. (for division)
	setcol(_tColorMenuA.font, 0, _pWin);
	mvwprintw(_pWin, y, _vItem[_nNum].x+1,
			(char*)scrstrncpy(_vItem[_nNum].GetName(), 0, 1).c_str());

	wnoutrefresh(_pWin);
}

void Menu::Draw()
{
	if (_vItem[_nNum].GetEnable() == false) _vItem[_nNum].CursorDown();
	_vItem[_nNum].Show();
}

void Menu::Refresh(bool bNoOutRefresh)
{
	werase(_pWin);
	_bNoOutRefresh = bNoOutRefresh;
	MenuRefresh();
	Show();
}

#include "mlsdialog.h"

void	Menu::Execute(KeyInfo& tKeyInfo)
{
	bool bRefesh = false;

	switch((int)tKeyInfo)
	{
		case KEY_DOWN:
		{
			_vItem[_nNum].CursorDown();
			break;
		}
		case KEY_UP:
		{
			_vItem[_nNum].CursorUp();
			break;
		}
		// if down the left key, move to left menu.
		case KEY_LEFT:
		{
			_vItem[_nNum].Close(); // before used the menu window is close.
			_nNum = (_nNum+ _vItem.size() -1) % _vItem.size();
			if (_vItem[_nNum].GetEnable() == false) _vItem[_nNum].CursorDown();
			bRefesh = true;
			break;
		}

		// if down the right key, move to right menu.
		case KEY_RIGHT:
		{
			_vItem[_nNum].Close(); // before used the menu window is close.
			_nNum = (_nNum + 1) % _vItem.size();
			if (_vItem[_nNum].GetEnable() == false) _vItem[_nNum].CursorDown();
			bRefesh = true;
			break;
		}

		case KEY_ENTER:
		{
			_bExit = true;
			break;
		}

		case KEY_ESC:
		{
			_bESC_Exit = true;
			_bExit = true;
			break;
		}
	}

	if (bRefesh) 
	{
		MenuRefresh();
		curs_set(0);
	}
}
