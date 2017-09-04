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

/// @file	exception.cpp
/// @brief	exception.cpp

#include "exception.h"

namespace MLSUTIL
{

/// @brief	Exception Constructor
/// @param	file	the error filename.
/// @param	lineno	line number
/// @param	pMsg	error mesage string.
Exception::Exception(string	pMsg)
{
	_nErrNo = 0;
	_strError		+= pMsg;
}

/// @brief	Exception constructor
/// @param	file	error file name
/// @param	lineno	line number
/// @param	fmt		error message string.
Exception::Exception(const char *fmt, ...)
{
	_nErrNo = 0;

	va_list argptr;
	char buf_data[256];
	char* data = buf_data;
	int data_size = sizeof(buf_data);
	int ret;

	va_start(argptr, fmt);
	ret = vsnprintf(data, data_size, fmt, argptr);
	++ret;
	va_end(argptr);

	if(ret > (int)sizeof(buf_data))
	{
		data = (char*)malloc(ret);
		if (data == NULL)
		{
			cerr << "Buffer over flow" << endl;
			return;
		}
		data_size = ret;
		va_start(argptr, fmt);
		vsnprintf(data, data_size, fmt, argptr);
		va_end(argptr);
	}

	_strError	= data;
	if(data != buf_data)
	{
		if (data != NULL) free(data);
	}
}

Exception::Exception(int nErrNo, string	pMsg)
{
	_strError += pMsg;
	_nErrNo = nErrNo;
}

Exception::Exception(int nErrNo, const char *fmt, ...)
{
	va_list argptr;
	char buf_data[256];
	char* data = buf_data;
	int data_size = sizeof(buf_data);
	int ret;

	va_start(argptr, fmt);
	ret = vsnprintf(data, data_size, fmt, argptr);
	++ret;
	va_end(argptr);

	if(ret > (int)sizeof(buf_data))
	{
		data = (char*)malloc(ret);
		if (data == NULL)
		{
			cerr << "Buffer over flow" << endl;
			return;
		}
		data_size = ret;
		va_start(argptr, fmt);
		vsnprintf(data, data_size, fmt, argptr);
		va_end(argptr);
	}

	_strError	= data;
	if(data != buf_data)
	{
		if (data != NULL) free(data);
	}
	_nErrNo = nErrNo;
}

/// @brief	return the error contents.
/// @return return the error string
char*	Exception::GetInfo(void)
{
	return (char*)_strError.c_str();
}


/// @brief	if cout of the data, it's not used the char*.
ostream&	operator>>(ostream& output, Exception& tEx)
{
	return output << (char*)tEx;
}

};
