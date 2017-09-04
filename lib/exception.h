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

/// @file	exception.h
/// @brief	exception class header file.
#ifndef	__Exception_h__
#define	__Exception_h__

#include "define.h"
#include "strutil.h"
#include <stdarg.h>

namespace	MLSUTIL
{
	/// @brief	Exception class for except
	class	Exception
	{
		public:
			Exception() { _nErrNo = 0; }
			Exception(string	pMsg);
			Exception(const char *fmt, ...);

			Exception(int nNum): _nErrNo(nNum) {}
			Exception(int nErrNo, char* 	pMsg	= NULL);
			Exception(int nErrNo, string	pMsg);
			Exception(int nErrNo, const char *fmt, ...);

			char*	GetInfo(void);
			int		GetErrNo() { return _nErrNo; }

			/// @brief	return to char* type of error information.
			operator	char*()	{return	(char*)_strError.c_str();}

			/// @brief	return to string type of error information.
			operator	string()	{return	_strError;}
			
			/// @brief	for cout. operator ex) cout << ExceptionClass << endl;
			friend 	ostream&	operator>>(ostream& output, Exception& tEx);

		protected:
			string	_strError;		///<	Error information of string type.
			string	_sExcepInfo;	///<	Error description of string type.
			int		_nErrNo;
	};
}

#endif
