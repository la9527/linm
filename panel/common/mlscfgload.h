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

#ifndef __CFGLOADER_H__
#define __CFGLOADER_H__

#include "define.h"
#include "strutil.h"
#include "configure.h"

using namespace MLSUTIL;

namespace MLS
{

class CfgLoader:public Configure
{
public:
	static CfgLoader &GetInstance();

	map<string, string>		_mapExtBind;
	map<string, string>		_mapNameBind;

	void		Init();

	string	GetExtBind(const string& sExt)
	{
		map<string, string>::iterator i;
		if ((i = _mapExtBind.find(Tolower(sExt))) != _mapExtBind.end()) return (*i).second;
		return "";
	}

	string	GetNameBind(const string& sName)
	{
		map<string, string>::iterator i;
		if ((i = _mapNameBind.find(Tolower(sName))) != _mapNameBind.end()) return (*i).second;
		return "";
	}

protected:
	bool	Parsing(const string& section, const string& var, const string& val);

private:
	CfgLoader() : Configure()  {}
	~CfgLoader() {}
};

class Configurable
{
protected:
	CfgLoader &_Config;

public:
	Configurable();
	virtual ~Configurable();
	virtual void UpdateConfig() = 0;
	virtual void SaveConfig() = 0;
};

#define g_tCfg 	CfgLoader::GetInstance()

};

#endif
