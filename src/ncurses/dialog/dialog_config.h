/******************************************************************************
 *   Copyright (C) 2007 by la9527                                             *
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

#ifndef __DIALOG_CONFIG_H__
#define __DIALOG_CONFIG_H__

#include "define.h"
#include "dialog.h"
#include "keycfgload.h"

using namespace std;

namespace MLS {

enum
{
	CHECKBOX = 0,
	INPUTBOX = 1
};

class ConfigOBJ
{
public:
	string		_id;
	string		_text;
	string		_value;
	int			_nViewType;
	void*		_pBox;
	
	ConfigOBJ(	const string& 	id,
				const string& 	text,
				const string& 	value,
				int				nViewType = 0 )
		: _id( id ), _text( text ), _value( value ), _nViewType( nViewType ), _pBox( 0 )
	{ }
};

class Dialog_Config:public Form
{
public:
	Dialog_Config();
	~Dialog_Config();

	void	SetViewConfig( const vector<ConfigOBJ>& vConfigOBJS );

	const vector<ConfigOBJ>&	getConfigOBJS();
	bool	GetOk()				{ return _bOk; }

protected:
	void	ButtonExec( Button* pButton, KeyInfo& tKeyInfo );
	void	ViewObjExec( int nViewType, Position* pPos, KeyInfo& tKeyInfo );
	void	SetPosition(Position* pPosition, int y, int x, int width, int nColNum);
	bool	MouseEventExe(Position* pPosition, int nNum, int Y, int X, mmask_t bstate);
	bool    MouseEvent(int Y, int X, mmask_t bstate);
    void    Draw();
    void    Execute(KeyInfo& tKeyInfo);

private:
	vector<ConfigOBJ>			_vConfigOBJS;

    Button          _tOk;
    Button          _tCancel;

    bool            _bOk;
    int             _nCurNum;
};

};

#endif
