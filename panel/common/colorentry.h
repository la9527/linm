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
#ifndef __COLORENTRY_H__
#define __COLORENTRY_H__

#include "define.h"

namespace MLS
{

#define COLOR(fg, bg) COLOR_PAIR(((fg)<<3)+(bg))	///< COLOR_PAIR 변환 define

/// \brief	color entry struct
class ColorEntry
{
public:
	int font,		///< font color
		back;		///< background color
	
	/// \brief	기본 생성자
	ColorEntry() : font(0), back(0) {}
	ColorEntry(int f, int b) : font(f), back(b) { }

	///	\brief	operator == 재정의 함수
	/// \param	t	비교할 ColorEntry
	///	\return	비교 대상과 같은가
	bool operator == (ColorEntry &t)
	{
		return font==t.font && back==t.back;
	}
	
	///	\brief	operator < 재정의 함수 
	///	\param	t	비교할 ColorEntry
	///	\return	비교 대상과 크기 비교된 결과
	bool operator < (ColorEntry &t)
	{
		return font*16+back > t.font*16+t.back;
	}
};

}

#endif
