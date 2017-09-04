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

#include "colorcfgload.h"

using namespace MLSUTIL;

namespace MLS
{
	ColorCfgLoad &ColorCfgLoad::GetInstance()
	{
		static ColorCfgLoad instance;
		return instance;
	}

	void	ColorCfgLoad::Init()
	{
		_mapColor.clear();
		_mapName.clear();
		_mapExt.clear();
		_mapMask.clear();

		_DefaultColor = ColorEntry(7,0);
		_mapColor[Tolower("Default")] 	= ColorEntry(7,0);   // 색지정안된 파일 기본색상
		_mapColor[Tolower("MCD")]     	= ColorEntry(15,4);
		_mapColor[Tolower("MCDLine")] 	= ColorEntry(15,4);
		_mapColor[Tolower("MCDstat")] 	= ColorEntry(1,15);
		_mapColor[Tolower("Dir")]     	= ColorEntry(9,0);
		_mapColor[Tolower("Drive")]   	= ColorEntry(3,0);   // 메인창 드라이브 색
		_mapColor[Tolower("Line")]    	= ColorEntry(6,0);   // 메인창 선 색
		_mapColor[Tolower("Func")]    	= ColorEntry(15,6);  // 펑션키 보통
		_mapColor[Tolower("FuncA")]   	= ColorEntry(11,6);  // 펑션키 강조
		_mapColor[Tolower("Stat")]    	= ColorEntry(0,7);   // 상태선 보통
		_mapColor[Tolower("StatA")]   	= ColorEntry(15,7);  // 상태선 강조
		_mapColor[Tolower("Help")]    	= ColorEntry(11,6);  // 도움말
		_mapColor[Tolower("QCD")]     	= ColorEntry(15,4);  // 빨리찾기 메인	
		_mapColor[Tolower("Dialog")]  	= ColorEntry(15,12); // 대화상자 색
		_mapColor[Tolower("SelectBox")]	= ColorEntry(11,6); // 대화상자 색
		_mapColor[Tolower("Back")]    	= ColorEntry(0,0);
		_mapColor[Tolower("Edit")]		= ColorEntry(7,4);	 // 에디터 화면
		_mapColor[Tolower("EditInfo")]	= ColorEntry(2,4);	// 에디터 정보 보통
		_mapColor[Tolower("EditInfoA")] = ColorEntry(10,4);	// 에디터 정보 강조
	}

	ColorEntry 	ColorCfgLoad::GetColorEntry(const string& sName)
	{
		ColorEntry	tColorEntry = ColorEntry(7,0);   // 색지정안된 파일 기본색상

		map<string, ColorEntry>::iterator i = _mapColor.find(Tolower(sName));
		if (i != _mapColor.end())
		{
			tColorEntry = i->second;
		}
		return tColorEntry;
	}

	ColorEntry 	ColorCfgLoad::GetExtColorEntry(const string& sExt)
	{
		ColorEntry	tColorEntry = ColorEntry(7,0);   // 색지정안된 파일 기본색상

		map<string, ColorEntry>::iterator i = _mapExt.find(Tolower(sExt));
		if (i != _mapExt.end())
		{
			tColorEntry = i->second;
		}
		return tColorEntry;
	}

	ColorEntry 	ColorCfgLoad::GetNameColorEntry(const string& sName)
	{
		ColorEntry	tColorEntry = ColorEntry(7,0);   // 색지정안된 파일 기본색상

		map<string, ColorEntry>::iterator i = _mapName.find(Tolower(sName));
		if (i != _mapName.end())
		{
			tColorEntry = i->second;
		}
		return tColorEntry;
	}

	ColorEntry 	ColorCfgLoad::GetMaskColorEntry(const int nMask)
	{
		ColorEntry	tColorEntry = ColorEntry(7,0);   // 색지정안된 파일 기본색상

		map<int, ColorEntry>::iterator i = _mapMask.find(nMask);
		if (i != _mapMask.end())
		{
			tColorEntry = i->second;
		}
		return tColorEntry;
	}

	bool	ColorCfgLoad::Parsing(const string& section, const string& var, const string& val)
	{
		int		fcol = 0, bcol = 0;
		string::size_type p = val.find(',');

		LOG_WRITE("Parsing sSection [%s] var [%s] val [%s]", section.c_str(), var.c_str(), val.c_str());

		try
		{
			if (p != string::npos)
			{
				fcol = strtoint(val.substr(0, p).c_str());
				bcol = strtoint(val.substr(p+1).c_str());
			}
			else 
			{
				fcol = strtoint(val.c_str());
				bcol = GetColorEntry("Default").back;
			}
		}
		catch(Exception& ex)
		{
			LOG_WRITE("Color File Loading Exception : %s", (char*)ex);
			return false;
		}
		
		ColorEntry cur(fcol, bcol);
	
		if (var.substr(0, 4) == "mask")		
		{
			int num = strtol(getbetween(var, '[', ']').c_str(), NULL, 8);
			_mapMask[num] = cur;
		}
		else if(var.substr(0, 4) == "name")
		{
			StringToken st(getbetween(var, '[', ']'), ";");
			while(st.Next())
			{
				if (st.Get().empty()) continue;
				_mapName[Tolower(st.Get())] = cur;
				LOG_WRITE("_mapName [%s [%d:%d]]", Tolower(st.Get()).c_str(), cur.font, cur.back);	
			}
		}
		else if (var.substr(0, 3) == "ext")
		{
			StringToken st(getbetween(var, '[', ']'), ";");
		
			while(st.Next())
			{
				if (st.Get().empty()) continue;
				_mapExt[st.Get()] = cur;
				LOG_WRITE("_mapExt [%s [%d:%d]]", st.Get().c_str(), cur.font, cur.back);
			}
		}	
		else
		{
			_mapColor[Tolower(var)] = cur;
			LOG_WRITE("_mapColor [%s [%d:%d]]", var.c_str(), cur.font, cur.back);
			if (Tolower(var) == "default") 
				_DefaultColor = cur;
		}
		return false;
	}
};
