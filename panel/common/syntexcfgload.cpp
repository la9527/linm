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

SyntaxExtLoad& SyntaxExtLoad::GetInstance()
{
	static SyntaxExtLoad tInstance;
	return tInstance;
}

void SyntaxExtLoad::Init()
{
    _mapSyntaxExtList.clear();
    _mapSyntaxNameList.clear();

    //_mapSyntaxExtList["default.lang"] = "";
    _mapSyntaxExtList["ada.lang"] = "ada";
    _mapSyntaxExtList["asm.lang"] = "S";
    _mapSyntaxExtList["awk.lang"] = "awk";
    _mapSyntaxExtList["bat.lang"] = "bat";
    _mapSyntaxExtList["bib.lang"] = "bib";
    _mapSyntaxExtList["bison.lang"] = "bison";
    _mapSyntaxExtList["c.lang"] = "c";
    _mapSyntaxExtList["caml.lang"] = "calm";
    _mapSyntaxExtList["changelog.lang"] = "changelog";
    _mapSyntaxExtList["clipper.lang"] = "clipper";
    _mapSyntaxExtList["cobol.lang"] = "cobol";
    _mapSyntaxExtList["conf.lang"] = "conf;ini;cfg";
    _mapSyntaxExtList["cpp.lang"] = "cpp;cc;h";
    _mapSyntaxExtList["csharp.lang"] = "cs;csharp;";
    _mapSyntaxExtList["css.lang"] = "css";
    _mapSyntaxExtList["d.lang"] = "d";
    _mapSyntaxExtList["desktop.lang"] = "desktop";
    _mapSyntaxExtList["diff.lang"] = "diff;dif;";
    _mapSyntaxExtList["erlang.lang"] = "erl";
    _mapSyntaxExtList["errors.lang"] = "err";
    _mapSyntaxExtList["flex.lang"] = "flex";
    _mapSyntaxExtList["fortran.lang"] = "fr";
    _mapSyntaxExtList["glsl.lang"] = "glsl;gls";
    _mapSyntaxExtList["haskell.lang"] = "has;haskell";
    _mapSyntaxExtList["haxe.lang"] = "hex;haxe";
    _mapSyntaxExtList["html.lang"] = "html;htm;html4;html3";
    _mapSyntaxExtList["java.lang"] = "java";
    _mapSyntaxExtList["javascript.lang"] = "js";
    _mapSyntaxExtList["key_string.lang"] = "";
    _mapSyntaxExtList["langdef.lang"] = "";
    _mapSyntaxExtList["latex.lang"] = "latex";
    _mapSyntaxExtList["ldap.lang"] = "ldap";
    _mapSyntaxExtList["log.lang"] = "log";
    _mapSyntaxExtList["lsm.lang"] = "lsm";
    _mapSyntaxExtList["lua.lang"] = "lua";
    _mapSyntaxExtList["m4.lang"] = "m4";
    _mapSyntaxExtList["makefile.lang"] = "makefile;mk";
    _mapSyntaxExtList["manifest.lang"] = "manifest";
    _mapSyntaxExtList["nohilite.lang"] = "nohilite";
    _mapSyntaxExtList["number.lang"] = "num";
    _mapSyntaxExtList["outlang.lang"] = "outlang";
    _mapSyntaxExtList["oz.lang"] = "oz";
    _mapSyntaxExtList["pascal.lang"] = "pascal";
    _mapSyntaxExtList["pc.lang"] = "pc";
    _mapSyntaxExtList["perl.lang"] = "pl;perl";
    _mapSyntaxExtList["php.lang"] = "php;php3;php4;";
    _mapSyntaxExtList["postscript.lang"] = "ps;post";
    _mapSyntaxExtList["prolog.lang"] = "prolog";
    _mapSyntaxExtList["properties.lang"] = "ini";
    _mapSyntaxExtList["python.lang"] = "py;python;";
    _mapSyntaxExtList["ruby.lang"] = "ru";
    _mapSyntaxExtList["scala.lang"] = "";
    _mapSyntaxExtList["sh.lang"] = "sh;bash;";
    _mapSyntaxExtList["slang.lang"] = "slang";
    _mapSyntaxExtList["sml.lang"] = "sml";
    _mapSyntaxExtList["spec.lang"] = "spec";
    _mapSyntaxExtList["sql.lang"] = "sql";
    _mapSyntaxExtList["style.lang"] = "style";
    _mapSyntaxExtList["symbols.lang"] = "symbols";
    _mapSyntaxExtList["tcl.lang"] = "tcl";
    _mapSyntaxExtList["texinfo.lang"] = "tex;texinfo";
    _mapSyntaxExtList["todo.lang"] = "todo";
    _mapSyntaxExtList["url.lang"] = "url";
    _mapSyntaxExtList["vbscript.lang"] = "vb;vbscript";
    _mapSyntaxExtList["xml.lang"] = "xml;xsl;xslt;";
    _mapSyntaxExtList["xorg.lang"] = "xorg";
    
    _mapSyntaxNameList["changelog.lang"] = "changelog";
    _mapSyntaxNameList["url.lang"] = "url";
    _mapSyntaxNameList["todo.lang"] = "todo";
    _mapSyntaxNameList["xorg.lang"] = "xorg";
}

bool    SyntaxExtLoad::getExtSyntax( const string& strExt, std::string& strRt )
{
    map<string, string>::iterator i;
    
    for (i = _mapSyntaxExtList.begin(); i != _mapSyntaxExtList.end(); ++i)
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

bool    SyntaxExtLoad::getNameSyntax( const string& strName, std::string& strRt )
{
    map<string, string>::iterator i;
    for (i = _mapSyntaxNameList.begin(); i != _mapSyntaxNameList.end(); ++i)
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

bool	SyntaxExtLoad::Parsing(const string& section, const string& var, const string& val)
{
    if ( section == "Syntax" )
	{
		if (Tolower(var.substr(0, 4)) == "ext[")
		{
			StringToken st(getbetween(var, '[', ']'), "; ");
			while (st.Next())
			{
				_mapSyntaxExtList[Tolower(st.Get())] = val;
			}
		}
		else if (Tolower(var.substr(0, 5)) == "name[")
		{
			StringToken st(getbetween(var, '[', ']'), "; ");
			while (st.Next())
			{
				_mapSyntaxNameList[Tolower(st.Get())] = val;
			}
		}
	}
	return false;
}

