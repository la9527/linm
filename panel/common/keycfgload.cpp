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

#include "keycfgload.h"
#include "mlslocale.h"

using namespace MLSUTIL;
using namespace MLS;

void	KeyCfgLoad::Init()
{
	_mapKeyCmd.clear();
	_mapRunCmd.clear();
	_mapKeyHelp.clear();

	_vKeyList.clear();	
	_tMemTypeInfo.Clear();	
}

bool	KeyCfgLoad::Parsing(const string& sSection, const string& sVar, const string& sVal)
{
	//LOG_WRITE("Parsing KeyCfgLoad sSection [%s] var [%s] val [%s]", sSection.c_str(), sVar.c_str(), sVal.c_str());

	if (sSection == "KeyCodeSetting")
	{
		KeyInfo		tKeyInfo;
	
		StringToken		tToken(sVal, " ,");
		string			sTmp, sTmp2;

		vector<int>		vKey;
		while(tToken.Next())
		{
			sTmp = tToken.Get();
			StringToken		tToken2(sTmp, " :");
	
			vKey.clear();
			while(tToken2.Next())
			{
				sTmp2 = tToken2.Get();
				if (sTmp2.size() == 3 && sTmp2[0] == '\'' && sTmp2[2] == '\'')
				{
					vKey.push_back((int)sTmp2[2]);
				}
				else
				{
					vKey.push_back(strtoint(sTmp2));
				}
			}

			tKeyInfo.vKeyInfo = vKey;
			tKeyInfo.sKeyName = sVar;
			
			_vKeyList.push_back(tKeyInfo);
		}
	}

	if (sSection.substr(0, 7) == "Command") // 실행 명령
	{
		StringToken		tToken(sVal, " ,");		
		ViewType		eType;

		if ( sSection == "Command_Panel" )
			eType = PANEL;
		else if ( sSection == "Command_Mcd" )
			eType = MCD;
		else if ( sSection == "Command_Editor" )
			eType = EDITOR;
		else
			eType = COMMON;
		
		while(tToken.Next())
		{
			TypeInfo& tType = _tMemTypeInfo.Get();
			tType.eType = eType;
			tType.sValue = sVar;
			_mapKeyCmd[ &tType ] = tToken.Get();
		}
	}

	if (sSection == "KeyRun") // 키 눌렀을 때 쉘 실행
	{
		_mapRunCmd[sVar] = getbetween(sVal, '[', ']');
	}

	if (sSection.substr(0, 4) == "Help")
	{
		ViewType		eType;

		if ( sSection == "Help_Panel" )
			eType = PANEL;
		else if ( sSection == "Help_Mcd" )
			eType = MCD;
		else if ( sSection == "Help_Editor" )
			eType = EDITOR;
		else
			eType = COMMON;

		TypeInfo& tType = _tMemTypeInfo.Get();
		tType.eType = eType;
		tType.sValue = sVar;

		// gettext style reading.
		_mapKeyHelp[ &tType ] = getbetween(sVal, '\"', '\"');
	}
	return false;
}

KeyCfgLoad &KeyCfgLoad::GetInstance()
{
	static KeyCfgLoad instance;
	return instance;
}

string	KeyCfgLoad::GetCommand(const TypeInfo& tType)
{
	map<TypeInfo*, string>::iterator i;
	string		sTmpCmd;

	for (i = _mapKeyCmd.begin(); i != _mapKeyCmd.end(); ++i)
	{
		//LOG_WRITE("GetCommand [%s] [%s]", i->second.c_str(), i->first->sValue.c_str());
		if (tType == *(i->first))
		{
			return i->first->sValue;
		}
	}
	return "";
}

string	KeyCfgLoad::GetCommand( const string& sKeyName, const ViewType eType )
{
	map<TypeInfo*, string>::iterator i;
	map<TypeInfo*, string>::iterator cur;

	string		sTmpCmd;

	for (i = _mapKeyCmd.begin(); i != _mapKeyCmd.end(); ++i)
	{
		//LOG_WRITE("GetCommand 2 [%s] [%s] [%s] ", i->second.c_str(), i->first->sValue.c_str(), sKeyName.c_str());
		if ( i->second == sKeyName )
		{	
			if (eType == i->first->eType)
				return i->first->sValue;
			
			if ( i->first->eType == COMMON )
				sTmpCmd = i->first->sValue;
		}
	}
	if (sTmpCmd.size() > 0 ) return sTmpCmd;
	return "";
}

string	KeyCfgLoad::GetCommand( const KeyInfo& tKeyInfo, const ViewType eType )
{
	string				sKeyName;
	vector<string>		vCmdList;
	bool				bFind = false;

	map<TypeInfo*, string>::iterator i;
	vector<KeyInfo>::iterator	n;

	for (n = _vKeyList.begin(); n != _vKeyList.end(); ++n)
	{
		if ( *n == (KeyInfo)tKeyInfo )
		{
			bFind = true;
			sKeyName = (*n).sKeyName;
			LOG_WRITE( "GetCommand Find... [%s]", sKeyName.c_str() );
			break;
		}
	}

	if (bFind == true)
	{
		string		sTmpCmd;
		for (i = _mapKeyCmd.begin(); i != _mapKeyCmd.end(); ++i)
		{
			if ( i->second == sKeyName )
			{
				//LOG_WRITE("tKeyInfo GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first->sValue.c_str(), sKeyName.c_str());
				if (eType == i->first->eType)
					return i->first->sValue;
				if (i->first->eType == COMMON )
					sTmpCmd = i->first->sValue;
			}
		}
		
		//LOG_WRITE("GetCommand !!! [%s]", sTmpCmd.c_str());
		return sTmpCmd;
	}	
	return "";
}

string	KeyCfgLoad::GetHelp(const TypeInfo& tType)
{
	map<TypeInfo*, string>::iterator i;
	string 		sTmpCmd;

	for (i = _mapKeyHelp.begin(); i != _mapKeyHelp.end(); ++i)
	{
		//LOG_WRITE("GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first.c_str(), sKeyName.c_str());
		if (tType == *(i->first))
			return i->second;
		if ( i->first->sValue == tType.sValue && i->first->eType == COMMON )
			sTmpCmd = i->second;
	}
	if (sTmpCmd.size() > 0 ) return sTmpCmd;
	return "";
}

string	KeyCfgLoad::GetHelp_Key(const string& sKeyName, const ViewType eType)
{
	string sCmd = GetCommand( sKeyName, eType );

	TypeInfo	tTypeInfo(sCmd, eType);
	string 		sTmpCmd;

	map<TypeInfo*, string>::iterator i;
	for (i = _mapKeyHelp.begin(); i != _mapKeyHelp.end(); ++i)
	{
		//LOG_WRITE("GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first.c_str(), sKeyName.c_str());
		if (tTypeInfo == *(i->first))
			return i->second;
		if ( i->first->sValue == sCmd && i->first->eType == COMMON )
			sTmpCmd = i->second;
	}
	if (sTmpCmd.size() > 0 ) return sTmpCmd;
	return "";
}

string	KeyCfgLoad::GetHelp(const string& sCmd, const ViewType eType)
{
	TypeInfo	tTypeInfo(sCmd, eType);
	string 		sTmpCmd;

	map<TypeInfo*, string>::iterator i;

	/*
	if ( sCmd.substr(0, 4) == "Cmd_" && e_nCurLang != US )
	{
		// convert to locale help message.
		string sLang;	
		if ( getenv("LANG") )
			sLang = getenv("LANG");
		
		StringToken		tToken(sLang, " .");
		tToken.Next();
		sLang = tToken.Get();

		tTypeInfo.sValue = sLang + sCmd.substr(3);

		for (i = _mapKeyHelp.begin(); i != _mapKeyHelp.end(); ++i)
		{
			//LOG_WRITE("GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first.c_str(), sKeyName.c_str());
			if (tTypeInfo == *(i->first))
				return ChgCurLocale(i->second);
			
			if ( i->first->sValue == tTypeInfo.sValue && i->first->eType == COMMON )
			{
				sTmpCmd = i->second;
			}
		}
		
		if (sTmpCmd.size() > 0 ) return ChgCurLocale(sTmpCmd);
		tTypeInfo.sValue = sCmd;
	}
	*/
	
	for (i = _mapKeyHelp.begin(); i != _mapKeyHelp.end(); ++i)
	{
		//LOG_WRITE("GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first.c_str(), sKeyName.c_str());
		if (tTypeInfo == *(i->first))
			return i->second;
		if ( i->first->sValue == sCmd && i->first->eType == COMMON )
			sTmpCmd = i->second;
	}
	
	if (sTmpCmd.size() > 0 ) return sTmpCmd;
	return "";
}

string	KeyCfgLoad::GetHelp(const KeyInfo& tKeyInfo, const ViewType eType)
{
	map<TypeInfo*, string>::iterator i;
	string		sKeyName, sTmpHelp;
	bool		bFind = false;

	vector<KeyInfo>::iterator	n;

	for (n = _vKeyList.begin(); n != _vKeyList.end(); ++n)
	{
		if ( *n == (KeyInfo)tKeyInfo )
		{
			bFind = true;
			sKeyName = (*n).sKeyName;
			break;
		}
	}

	if (bFind == true)
	{
		string		sTmpCmd;

		for (i = _mapKeyHelp.begin(); i != _mapKeyHelp.end(); ++i)
		{
			//LOG_WRITE("GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first.c_str(), sKeyName.c_str());
			if ( i->second == sKeyName )
			{
				if (eType == i->first->eType)
					return i->first->sValue;
				
				if (i->first->eType == COMMON )
					sTmpCmd =  i->first->sValue;
			}
		}
		return sTmpCmd;
	}

	return "";
}

string	KeyCfgLoad::CmdToRuncmd(const string& sCmd) 
{
	map<string, string>::iterator i;
	if ((i = _mapRunCmd.find(sCmd)) != _mapRunCmd.end()) return (*i).second;
	return "";
}

string	KeyCfgLoad::CmdToKeyName(const string& sCmd, const ViewType eType)
{
	map<TypeInfo*, string>::iterator i;

	string		sTmpCmd;
	for (i = _mapKeyCmd.begin(); i != _mapKeyCmd.end(); ++i)
	{
		if ( i->first->sValue == sCmd )
		{
			//LOG_WRITE("CmdToKeyName GetCommand [%s] [%s] [%s] ", i->second.c_str(), i->first->sValue.c_str(), sCmd.c_str());
			if (eType == i->first->eType)
				return i->second;

			if (i->first->eType == COMMON )
				sTmpCmd = i->second;
		}
	}
	return sTmpCmd;
}

string	KeyCfgLoad::GetRunCmd(const KeyInfo& tKeyInfo)
{
	// 현재 Panel에서 밖에 없기 때문에 이렇게 한다.
	return GetCommand(tKeyInfo, PANEL );
}
