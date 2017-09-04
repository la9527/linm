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

#include "syntexcfgload.h"

using namespace MLS;
using namespace MLSUTIL;

SyntexExtLoad& SyntexExtLoad::GetInstance()
{
	static SyntexExtLoad tInstance;
	return tInstance;
}

void SyntexExtLoad::Init()
{
    _mapSyntexExtList.clear();
    _mapSyntexNameList.clear();

    //_mapSyntexExtList["default.lang"] = "";    
    _mapSyntexExtList["ada.lang"] = "ada";
    _mapSyntexExtList["asm.lang"] = "S";
    _mapSyntexExtList["awk.lang"] = "awk";
    _mapSyntexExtList["bat.lang"] = "bat";
    _mapSyntexExtList["bib.lang"] = "bib";
    _mapSyntexExtList["bison.lang"] = "bison";
    _mapSyntexExtList["c.lang"] = "c";
    _mapSyntexExtList["caml.lang"] = "calm";
    _mapSyntexExtList["changelog.lang"] = "changelog";
    _mapSyntexExtList["clipper.lang"] = "clipper";
    _mapSyntexExtList["cobol.lang"] = "cobol";
    _mapSyntexExtList["conf.lang"] = "conf;ini;cfg";
    _mapSyntexExtList["cpp.lang"] = "cpp;cc;h";
    _mapSyntexExtList["csharp.lang"] = "cs;csharp;";
    _mapSyntexExtList["css.lang"] = "css";
    _mapSyntexExtList["d.lang"] = "d";
    _mapSyntexExtList["desktop.lang"] = "desktop";
    _mapSyntexExtList["diff.lang"] = "diff;dif;";
    _mapSyntexExtList["erlang.lang"] = "erl";
    _mapSyntexExtList["errors.lang"] = "err";
    _mapSyntexExtList["flex.lang"] = "flex";
    _mapSyntexExtList["fortran.lang"] = "fr";
    _mapSyntexExtList["glsl.lang"] = "glsl;gls";
    _mapSyntexExtList["haskell.lang"] = "has;haskell";
    _mapSyntexExtList["haxe.lang"] = "hex;haxe";
    _mapSyntexExtList["html.lang"] = "html;htm;html4;html3";
    _mapSyntexExtList["java.lang"] = "java";
    _mapSyntexExtList["javascript.lang"] = "js";
    _mapSyntexExtList["key_string.lang"] = "";
    _mapSyntexExtList["langdef.lang"] = "";
    _mapSyntexExtList["latex.lang"] = "latex";
    _mapSyntexExtList["ldap.lang"] = "ldap";
    _mapSyntexExtList["log.lang"] = "log";
    _mapSyntexExtList["lsm.lang"] = "lsm";
    _mapSyntexExtList["lua.lang"] = "lua";
    _mapSyntexExtList["m4.lang"] = "m4";
    _mapSyntexExtList["makefile.lang"] = "makefile;mk";
    _mapSyntexExtList["manifest.lang"] = "manifest";
    _mapSyntexExtList["nohilite.lang"] = "nohilite";
    _mapSyntexExtList["number.lang"] = "num";
    _mapSyntexExtList["outlang.lang"] = "outlang";
    _mapSyntexExtList["oz.lang"] = "oz";
    _mapSyntexExtList["pascal.lang"] = "pascal";
    _mapSyntexExtList["pc.lang"] = "pc";
    _mapSyntexExtList["perl.lang"] = "pl;perl";
    _mapSyntexExtList["php.lang"] = "php;php3;php4;";
    _mapSyntexExtList["postscript.lang"] = "ps;post";
    _mapSyntexExtList["prolog.lang"] = "prolog";
    _mapSyntexExtList["properties.lang"] = "ini";
    _mapSyntexExtList["python.lang"] = "py;python;";
    _mapSyntexExtList["ruby.lang"] = "ru";
    _mapSyntexExtList["scala.lang"] = "";
    _mapSyntexExtList["sh.lang"] = "sh;bash;";
    _mapSyntexExtList["slang.lang"] = "slang";
    _mapSyntexExtList["sml.lang"] = "sml";
    _mapSyntexExtList["spec.lang"] = "spec";
    _mapSyntexExtList["sql.lang"] = "sql";
    _mapSyntexExtList["style.lang"] = "style";
    _mapSyntexExtList["symbols.lang"] = "symbols";
    _mapSyntexExtList["tcl.lang"] = "tcl";
    _mapSyntexExtList["texinfo.lang"] = "tex;texinfo";
    _mapSyntexExtList["todo.lang"] = "todo";
    _mapSyntexExtList["url.lang"] = "url";
    _mapSyntexExtList["vbscript.lang"] = "vb;vbscript";
    _mapSyntexExtList["xml.lang"] = "xml;xsl;xslt;";
    _mapSyntexExtList["xorg.lang"] = "xorg";
    
    _mapSyntexNameList["changelog.lang"] = "changelog";
    _mapSyntexNameList["url.lang"] = "url";
    _mapSyntexNameList["todo.lang"] = "todo";
    _mapSyntexNameList["xorg.lang"] = "xorg";
}

bool    SyntexExtLoad::getExtSyntex( const string& strExt, std::string& strRt )
{
    map<string, string>::iterator i;
    
    for (i = _mapSyntexExtList.begin(); i != _mapSyntexExtList.end(); ++i)
	{
	    if ( i->second.length() == 0 ) continue;
	    
	    StringToken st(i->second, ";");
		while(st.Next())
		{
		    if ( st.Get() == strExt )
            {
                strRt = i->first;
                return true;
            }
		}
	}
	return false;
}

bool    SyntexExtLoad::getNameSyntex( const string& strName, std::string& strRt )
{
    map<string, string>::iterator i;
    for (i = _mapSyntexNameList.begin(); i != _mapSyntexNameList.end(); ++i)
	{
	    if ( i->second.length() == 0 ) continue;
	    
	    StringToken st(i->second, ";");
		while(st.Next())
		{
		    if ( st.Get() == strName )
            {
                strRt = i->first;
                return true;
            }
		}
	}
	return false;
}

bool	SyntexExtLoad::Parsing(const string& section, const string& var, const string& val)
{
    if ( section == "Syntex" )
	{
		if (Tolower(var.substr(0, 4)) == "ext[")
		{
			StringToken st(getbetween(var, '[', ']'), "; ");
			while (st.Next())
			{
				_mapSyntexExtList[Tolower(st.Get())] = val;
			}
		}
		else if (Tolower(var.substr(0, 5)) == "name[")
		{
			StringToken st(getbetween(var, '[', ']'), "; ");
			while (st.Next())
			{
				_mapSyntexNameList[Tolower(st.Get())] = val;
			}
		}
	}
	return false;
}

