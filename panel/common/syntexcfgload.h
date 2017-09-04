/******************************************************************************
 *   Copyright (C) 2010 by la9527                                             *
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

#ifndef __SYNTEXCFGLOAD_H__
#define __SYNTEXCFGLOAD_H__

#include "define.h"
#include "strutil.h"
#include "configure.h"
#include "mpool.h"

using namespace MLSUTIL;

namespace MLS
{

class SyntexExtLoad:public Configure
{
public:
	static SyntexExtLoad &GetInstance();
    
    void    Init();

    bool    getExtSyntex( const string& strExt, std::string& strRt );
    bool    getNameSyntex( const string& strName, std::string& strRt );
   
protected:
    bool	Parsing(const string& section, const string& var, const string& val);
    
    map<string, string >     _mapSyntexExtList;
    map<string, string >     _mapSyntexNameList;
    
private:
    SyntexExtLoad(): Configure() { }
};

#define g_tSyntexExtCfg SyntexExtLoad::GetInstance()

};

#endif // __SYNTEXCFGLOAD_H__


