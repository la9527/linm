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

#ifndef __READERCTL_H__
#define __READERCTL_H__

#include "define.h"
#include "reader.h"

namespace MLS
{

class	ReaderCtl
{
	vector<Reader*>		_vReader;

public:
	ReaderCtl() {}
	~ReaderCtl()
	{
		for (int n = 0; n < (int)_vReader.size(); n++)
		{
			Reader*	pReader = _vReader[n];
			delete pReader;
		}
	}

	bool	Insert(Reader* pReader)
	{
		for (int n = 0; n < (int)_vReader.size(); n++)
		{
			Reader*	pReaderBef = _vReader[n];
			if (pReaderBef == pReader)	return false;
		}

		_vReader.push_back(pReader);
		return true;
	}

	void	Delete(Reader* pReader)
	{
		for (int n = 0; n < (int)_vReader.size(); n++)
		{
			if(_vReader[n] == pReader)
			{
				_vReader.erase(_vReader.begin() + n);
				break;
			}
		}
	}

	Reader*	Get(const string& sType)
	{
		for (int n = 0; n < (int)_vReader.size(); n++)
		{
			if(_vReader[n]->GetReaderName() == sType)
			{
				return _vReader[n];
			}
		}
		return NULL;
	}

	static ReaderCtl &GetInstance()
	{
		static ReaderCtl	_tReaderCtl;
		return _tReaderCtl;
	}
};

#define	g_tReaderCtl	ReaderCtl::GetInstance()

};

#endif
