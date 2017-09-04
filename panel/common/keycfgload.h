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

#ifndef __KEYCFGLOAD_H__
#define __KEYCFGLOAD_H__

#include "define.h"
#include "strutil.h"
#include "configure.h"
#include "mpool.h"

using namespace MLSUTIL;

namespace MLS
{

class	KeyInfo
{
public:
	vector<int>		vKeyInfo;
	string			sKeyName;

	int				size() const	{	return (int)vKeyInfo.size(); }

	///	\brief	operator == 재정의 함수
	/// \param	t	비교할 KeyInfo
	///	\return	비교 대상과 같은가
	bool operator == (KeyInfo &t)
	{
		return vKeyInfo == t.vKeyInfo;
	}

	///	\brief	operator == 재정의 함수
	/// \param	t	비교할 KeyInfo
	///	\return	비교 대상과 같은가
	bool operator == (const KeyInfo &t)
	{
		return vKeyInfo == t.vKeyInfo;
	}

	bool operator == (int nKey)
	{
		if (vKeyInfo.size() == 0) return false;
		if ((int)vKeyInfo[0] != nKey)
			return false;
		return true;
	}

	KeyInfo&	operator= (int nKey)
	{
		vKeyInfo.clear();
		vKeyInfo.push_back(nKey);
		return *this;
	}
	
	int operator [] (int nKey)
	{
		if ((int)vKeyInfo.size() <= nKey) return ERROR;
		return vKeyInfo[nKey];
	}

	/// @brief	key code 로 돌려준다.
	operator	int()
	{
		if (vKeyInfo.size() < 1) return ERROR;
		return	vKeyInfo[0];
	}

	/// @brief	한글입력 부분 코드의 경우는 string으로 돌려준다.
	operator	string()
	{
		string 	sStr;
		for (int n = 0; n < (int)vKeyInfo.size(); n++)
		{
			if (n == 0 && vKeyInfo[0] == 8) continue; // Konsole (kde console) program patch
			if (n == 0 && vKeyInfo[0] == 263) continue; // Konsole (kde console) program patch
			if (vKeyInfo[n] != ERROR)
				sStr = sStr + (char)vKeyInfo[n];
		}
		return	sStr;
	}

	KeyInfo(	int key1, int key2 = ERROR, int key3  = ERROR,
				int key4 = ERROR, int key5 = ERROR)
	{
		vKeyInfo.clear();
		vKeyInfo.push_back(key1);
		if ( key2 != ERROR )
			vKeyInfo.push_back(key2);
		if ( key3 != ERROR )
			vKeyInfo.push_back(key3);
		if ( key4 != ERROR )
			vKeyInfo.push_back(key4);
		if ( key5 != ERROR )
			vKeyInfo.push_back(key5);
	}

	KeyInfo(vector<int>	vKey)
	{
		vKeyInfo = vKey;
	}

	KeyInfo()
	{
		vKeyInfo.clear();
	}	
};

class TypeInfo
{
public:
	TypeInfo()	{}
	TypeInfo(string V, ViewType T): sValue(V), eType(T) {}
	TypeInfo(ViewType T, string V): sValue(V), eType(T) {}
	TypeInfo(string V): sValue(V), eType( COMMON ) {}
	
	bool	empty()
	{
		if (sValue.size() == 0) return true;
		return false;
	}

	bool operator == (TypeInfo& tTypeInfo)
	{
		if (eType == tTypeInfo.eType && sValue == tTypeInfo.sValue)
			return true;
		return false;
	}
	
	bool operator == (const TypeInfo& tTypeInfo)  const
	{
		if (eType == tTypeInfo.eType && sValue == tTypeInfo.sValue)
			return true;
		return false;
	}

	bool operator < (const TypeInfo& tTypeInfo)  const
	{
		return tTypeInfo.sValue < tTypeInfo.sValue;
	}

public:
	string		sValue;
	ViewType	eType;
};

class KeyCfgLoad:public Configure
{
public:
	KeyCfgLoad(): Configure()  {}
	~KeyCfgLoad() 
	{
	}

	static KeyCfgLoad &GetInstance();

	vector<KeyInfo>		GetKeyInfo()
	{
		return _vKeyList;
	}

	void	Init();

	string	GetCommand(const TypeInfo& tType);
	string	GetCommand( const string& sKeyName, const ViewType eType );
	string	GetCommand(const KeyInfo& tKeyInfo, const ViewType eType);
	string	GetHelp(const TypeInfo& tType);

	string	GetHelp_Key(const string& sKeyName, const ViewType eType);
	string	GetHelp(const string& sCmd, const ViewType eType);
	string	GetHelp(const KeyInfo& tKeyInfo, const ViewType eType);
	string	CmdToRuncmd(const string& sCmd);
	string	CmdToKeyName(const string& sCmd, const ViewType eType);
	
	string	GetRunCmd(const KeyInfo& tKeyInfo);
	
	// [KeyCodeSetting] LEFT   = 27:91:68,260      키코드 세팅
	// [Command]        Cmd_Left = LEFT            실행
	// [KeyRun]         Cmd_Editor = [vim]         커맨드 실행
    // [Help]           Cmd_Left = [LEFT]          도움말
	bool	Parsing(const string& section,
					const string& var,
					const string& val);

public:
	// map< key command, help >
	map<TypeInfo*, string>	_mapKeyHelp;

protected:
	// map< key command, key Name  >
	map<TypeInfo*, string>	_mapKeyCmd;
	// map< key command, run cmd >
	map<string, string>		_mapRunCmd;

	vector<KeyInfo>			_vKeyList;
	MemoryPool<TypeInfo>		_tMemTypeInfo;
};

#define g_tKeyCfg KeyCfgLoad::GetInstance()

};

#endif
