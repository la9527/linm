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
#include "mlslog.h"
#include "strutil.h"

namespace MLSUTIL
{
	MlsLog	g_Log;
	
	void	MlsLog::Write(const string& sStr)
	{
		#ifdef __DEBUGMODE__
		if (!_bLog) return;
		ofstream	fp(_LogFile.c_str(), ios::out|ios::app);

		if (fp.is_open())
		{
			fp.write(sStr.c_str(), sStr.size());
			fp.write("\n", 1);
		}
		#endif
	}
	
	void	MlsLog::Write(const char* fmt, ...)
	{
		#ifdef __DEBUGMODE__
		if (!_bLog) return;

/* coded by revival / 2006/05/06 */
		va_list argptr;
		int ret;
		FILE *fp = fopen(_LogFile.c_str(), "a");
		va_start(argptr, fmt);
		vfprintf(fp, fmt, argptr);
		fprintf(fp, "\n");
		fclose(fp);
		va_end(argptr);
/* end of revival */
		#endif
	}

/* coded by revival / 2006/05/06 */
#ifdef __DEBUGMODE__
// FIXME: need more info
	void	MlsLog::WriteLog(const char *strDebugFile, const int nDebugLine)
	{
		if (!_bLog) return;

		FILE *fp = fopen(_LogFile.c_str(), "a");
		fprintf(fp, "%-15s:%-3d] ", strDebugFile, nDebugLine);
		fclose(fp);
	}
#endif
/* end of revival */
};
