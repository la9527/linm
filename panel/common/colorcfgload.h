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

#ifndef __COLORCFGLOAD_H__
#define __COLORCFGLOAD_H__

#include "define.h"
#include "strutil.h"
#include "configure.h"
#include "colorentry.h"

using namespace MLSUTIL;

namespace MLS
{

class ColorCfgLoad:public Configure
{
public:
	static ColorCfgLoad &GetInstance();

	ColorEntry					_DefaultColor;

	map<string, ColorEntry>		_mapColor;
	map<string, ColorEntry>		_mapName;
	map<string, ColorEntry>		_mapExt;
	map<int, ColorEntry> 		_mapMask;

	void		Init();
	ColorEntry	GetColorEntry(const string& sName);
	ColorEntry 	GetExtColorEntry(const string& sExt);
	ColorEntry 	GetNameColorEntry(const string& sName);
	ColorEntry 	GetMaskColorEntry(const int nMask);

protected:
	bool	Parsing(const string& section, const string& var, const string& val);

private:
	ColorCfgLoad() : Configure()  {}
	~ColorCfgLoad() {}
};

#define g_tColorCfg ColorCfgLoad::GetInstance()

};

#endif
