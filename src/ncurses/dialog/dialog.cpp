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

void Position::SetForm(Form* pForm)
{
	_pForm = pForm;
}

void Position::Resize()
{
	if (_ObjPosY == TOP)
		y = 0;
	else if (_ObjPosY == MIDDLE)
		y = (_pForm->height / 2) - (height / 2);
	else if (_ObjPosY == BOTTOM)
		y = _pForm->height - height;
	
	if (_ObjPosX == LEFT)
		x = 0;
	else if (_ObjPosX == MIDDLE)
		x = (_pForm->width / 2) - (width / 2);
	else if (_ObjPosX == RIGHT)
		x = _pForm->width - width;

	if (_pForm->width < x) x = _pForm->width;
	if (_pForm->height < y) y = _pForm->height;
	
	if (_pForm->width < x+width) width = _pForm->width-x;
	if (_pForm->height < y+height) height = _pForm->height-y;

	/*
	LOG_WRITE("Position y [%d] x [%d] h [%d] w [%d] Form-H [%d] Form-W [%d]", 
				y, x, height, width, _pForm->height, _pForm->width);
	*/
}

