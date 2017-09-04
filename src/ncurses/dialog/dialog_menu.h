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

#ifndef __MENU_H__
#define __MENU_H__

#include "define.h"
#include "dialog.h"
#include "keycfgload.h"

namespace MLS {

///	\brief	MenuItem class
class MenuItem:public Position
{
	string	_sName;		///< screen output menu item name.
	string	_sCmd;		///< command execute of menu item.
	string	_sCtlKey;

	bool	_bEnable;
	bool	_bCur;		/// is selected.
	
	ColorEntry	_tMenuColor;
	ColorEntry	_tMenuAColor;
	
public:
	
	///	\brief	instructor.
	///	\param	n		MenuItem name
	///	\param	c		MenuItem command.
	MenuItem(	const string &n, 
				const string &c, 
				bool bEn = true,
				bool bCur = false) 
		: _sName(n), _sCmd(c), _bEnable(bEn), _bCur(bCur)
	{}
	
	// setter	===================================
	///	\brief	name setting function
	///	\param	n		set item name
	void SetName(const string &n) { _sName = n;}

	///	\brief	command setting function
	///	\param	n		set command
	void SetCmd(const string &n) { _sCmd = n;}	
	// ============================================
	
	// getter	===================================
	///	\brief	get menu item name 
	///	\return	return the menu item name.
	const string &GetName() {return _sName;}

	///	\brief	get menu command
	///	\return	return the command.
	const string &GetCmd() {return _sCmd;}
	// ============================================

	void	SetCtlKey(const string& sCtlKey)	{ _sCtlKey = sCtlKey; }
	const string &GetCtlKey()	{ return _sCtlKey; }

	///	\brief  enable command.
	///	\param	bEn		if enable, return true.
	void	SetEnable(bool bEn) { _bEnable = bEn; }
	
	///	\brief	get enable
	///	\return	Enable
	bool	GetEnable() { return _bEnable; }
	
	void	SetCur(bool bCur)	{ _bCur = bCur; }
	bool	GetCur()	{ return _bCur; }

	void	Color(const ColorEntry& tMenuColor, const ColorEntry& tMenuAColor)
	{
		_tMenuColor = tMenuColor;
		_tMenuAColor = tMenuAColor;
	}	

	void	Draw();
};

// menu category.
///	\brief	Menu Category class.
class MenuCategory:public Form
{
	string _sName;		///< menu name
	
	int _nIndex;     		///< current position in menu category.
	
	vector<MenuItem> 	_vItem; ///< sub menu.
	ColorEntry			_tMenuColor;
	ColorEntry			_tMenuAColor;
	ViewType			_eViewType;

public:
	///	\brief	constructor
	///	\param	p		MenuCategory name
	///	\param	l		MenuCategory location
	MenuCategory(const string &p, const ViewType eViewType)
		: _sName(p) , _nIndex(0) 
	{
		_bNoOutRefresh = true; // when Refresh, do not doupdate() call.
		_bNoViewUpdate = true; // do not update.
		_bNotDrawBox = true; // do not draw box.
		_eViewType = eViewType;
	}

	~MenuCategory()
	{
	}

	void	MenuColor(const ColorEntry& tMenuColor, const ColorEntry& tMenuAColor)
	{
		_tMenuColor = tMenuColor;
		_tMenuAColor = tMenuAColor;
	}	

	///	\brief	menu category get name function
	///	\return	return name
	const string &GetName() const {return _sName;}
	
	///	\brief	Add to menu item function.
	///	\param	n		MenuItem name
	///	\param	c		MenuItem Command
	///	\param	k		MenuItem KeyInfo
	///	\param	b		is menu item Enable (default: true)
	void AddItem(	const string &n, 
					const string &c, 
					bool b = true);

	///	\brief	menu item get function
	///	\return	return the menu item command.
	string 	GetCmd()  { return _vItem[_nIndex].GetCmd(); }

	///	\brief	get menu item command function.
	///	\return	return the menu item command.
	bool	GetEnable() { return _vItem[_nIndex].GetEnable(); }
	int  	GetItemSize() { return _vItem.size(); }

	void	SetDisable(const std::string& sStr);
	void 	SetCursor(const int nIdx);
	void	SetTypeName(const ViewType e) { _eViewType = e; }

	void 	DrawFirst();
	void 	Draw();
	
	void 	CursorUp();
	void 	CursorDown();
	bool	MouseEvent(int Y, int X, mmask_t bstate);
};

///	\brief	Menu class
class Menu:public Form
{
protected:
	int _nNum;									///< currently selected index the menu item.
	bool						_bESC_Exit;		///< Is ESC Exit.
	std::vector<MenuCategory> 	_vItem; 		///< menu categoryes (ex. File, Menu)
	std::vector<std::string>	_vCmdDisable; 	/// be disable the command list.
	ColorEntry					_tColorMenu;
	ColorEntry					_tColorMenuA;
	
	bool	MouseEvent(int Y, int X, mmask_t bstate);

	// color setting of _tColorMenu, _tColorMenuA.
	virtual void MenuColorSetting() = 0;

	// When Reflash, another screen is need to refresh() call. for not broken text.
	virtual void MenuRefresh() {}
	
	// virtual function for add each menu.
	virtual void MenuAddCatecory() = 0;

public:
	Menu();

	void 	AddItem(MenuCategory& p);
	string	GetCurCmd();
	
	///	\brief	Add to be disable the command list.
	///	\param	vCmd	set to disable menu list (command)
	void 	SetDefault(void)	{ _vCmdDisable.clear(); }
	void 	SetDisable(std::vector<std::string> vCmd) { _vCmdDisable = vCmd; }

	void	Create();	
	void	DrawFirst();
	void 	Draw();
	void 	Refresh(bool bNoOutRefresh = false);

	void	Execute(KeyInfo& tKeyInfo);
};

};

#endif
