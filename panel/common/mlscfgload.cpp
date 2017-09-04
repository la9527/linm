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

#include "mlscfgload.h"

using namespace MLSUTIL;
using namespace MLS;

CfgLoader &CfgLoader::GetInstance()
{
	static CfgLoader conf;
	return conf;
}

void	CfgLoader::Init()
{
	_mapExtBind.clear();
	_mapNameBind.clear();
}

bool	CfgLoader::Parsing(const string& section, const string& var, const string& val)
{
	LOG_WRITE("Parcing Default Cfg [%s][%s][%s]", section.c_str(), var.c_str(), val.c_str());

	if ( section == "RunProgram" )
	{
		if (Tolower(var.substr(0, 4)) == "ext[")
		{
			StringToken st(getbetween(var, '[', ']'), "; ");
			while (st.Next())
			{
				_mapExtBind[Tolower(st.Get())] = val;
			}
		}
		else if (Tolower(var.substr(0, 5)) == "name[")
		{
			StringToken st(getbetween(var, '[', ']'), "; ");
			while (st.Next())
			{
				_mapNameBind[Tolower(st.Get())] = val;
			}
		}
	}
	return false;
}

Configurable::Configurable()
	: _Config( CfgLoader::GetInstance() )  { } 

Configurable::~Configurable() { }

