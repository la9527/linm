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

#ifndef	 	__STRLINETOKEN_H__
#define		__STRLINETOKEN_H__

#include "define.h"
#include "strutil.h"

namespace MLSUTIL
{

class StrLineToken
{
	vector<wstring>	_vStr;
	wstring			_SrcString;
	
	int		_width;
	uint	_curline;
	
	public:
		#ifdef __CYGWIN_C__
		StrLineToken(const wstring& sLineStr = "", int nViewWidth=80)
		#else
		StrLineToken(const wstring& sLineStr = L"", int nViewWidth=80)
		#endif
		{
			SetWString(sLineStr, nViewWidth);
		}

		void	SetWString(wstring sLineWStr, int nViewWidth)
		{
			string			sLineStr;
			string			sViewStr;
			wstring			sConvStr;
			bool			bNextLine = false;
			int				nKrStrSize;

			_vStr.clear();
			
			_SrcString = sLineWStr;
			_curline = 0;

			if (sLineWStr.size() == 0) return;
			sLineStr = wstrtostr(sLineWStr);

			do
			{
				nKrStrSize = scrstrlen(sLineStr);
				if (nKrStrSize <= nViewWidth)
				{
					sViewStr = scrstrncpy(sLineStr, 0, nKrStrSize);
					bNextLine = true;
				}
				else
				{
					sViewStr = scrstrncpy(sLineStr, 0, nViewWidth);
					sLineStr = scrstrncpy(sLineStr, nViewWidth, nKrStrSize-nViewWidth);
					bNextLine = false;
				}

				sConvStr = strtowstr(sViewStr);
				_vStr.push_back(sConvStr);
			}
			while(bNextLine == false);
		}
		
		bool	Set_LineData(const wstring& sWStr, uint nLine)
		{
			if (_vStr.size() <= nLine)
				_vStr.push_back(sWStr);
			else
				_vStr[nLine] = sWStr;
			return true;
		}

		wstring GetWString()
		{
			#ifdef __CYGWIN_C__
				wstring sStr;
			#else
				wstring sStr = L"";
			#endif
			for (uint nSize = 0;nSize < _vStr.size(); nSize++)
			{
				sStr = sStr + _vStr[nSize];
			}
			return sStr;
		}

		uint			LineSize()						{ return _vStr.size(); }
		
		wstring	Get()
		{
			if (_vStr.size() == 0 || _vStr.size() <= _curline) 
			{	
				#ifdef __CYGWIN_C__
					return "";
				#else
					return L"";
				#endif
			}
			return _vStr[_curline];
		}

		wstring	GetLineStr(uint nLineNum)
		{
			if (_vStr.size() == 0 || _vStr.size() <= nLineNum) 
			{
				#ifdef __CYGWIN_C__
					return "";
				#else
					return L"";
				#endif
			}
			return _vStr[nLineNum];
		}
		
		uint			GetCurNum()						{ return _curline; }

		bool Next()
		{
			if (_vStr.size() > _curline)
			{
				_curline++;
				return true;
			}
			return false;
		}

		bool NextChk()
		{
			if (_vStr.size() > _curline)
			{
				return true;
			}
			return false;
		}
};

};

#endif
