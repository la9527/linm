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

#ifndef  __MLSLOG_H__
#define  __MLSLOG_H__

#include "define.h"

/* coded by revival / 2006/05/06 */
#ifdef __DEBUGMODE__
#define LOG_WRITE(x...) g_Log.WriteLog(__FILE__, __LINE__), g_Log.Write(x)
#else
#define LOG_WRITE(x...)
#endif
/* end of revival */

#include <stdarg.h>
#include <string>

using namespace std;

namespace MLSUTIL
{

class MlsLog
{
	string	_LogFile;
	bool	_bLog;

public:
	MlsLog()	{ _bLog = false; }

	void	SetFile(const string& sFile) {  _LogFile = sFile; _bLog = true; }

	void	Write(const string& sStr);
	void	Write(const char* fmt, ...);
	#ifdef __DEBUGMODE__	
	void    WriteLog(const char *strDebugFile, const int nDebugLine);
	#endif	
};

extern MlsLog	g_Log;

};

#endif
