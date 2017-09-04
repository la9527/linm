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

#ifndef __CMDSHELL_H__
#define	__CMDSHELL_H__

#include "define.h"
#include "strutil.h"
#include "exception.h"

namespace MLSUTIL
{
	typedef vector<string>	LineArgData;

	class CmdShell
	{
		public:

		/**
		*  실행 출력 데이터를 컬럼 단위의 내용으로 돌려준다.
		* @param sCmd 		실행내용
		* @param nArgNum 	컬럼 기준 사이즈
		* @param tLineArgData	vector형태의 실행 내용 이것을 리턴해준다.
		* @param bFile	파일 내용을 읽을때 사용
		* @return vector형태의 실행 내용
		*/
		static int	CmdExeArg(	const string& sCmd, 
								int nArgNum, 
								vector<LineArgData>& tLineArgData,
								bool bFile = false)
		{
			vector<string>		vLineStr;
		
			vector<string>		vTmp;
			vector<LineArgData>	vData;
		
			if (sCmd.size() == 0) return ERROR;
		
			if (bFile == false)
				vLineStr = CmdShell::CmdExecute(sCmd);
			else
				vLineStr = CmdShell::GetFileData(sCmd);
		
			StringToken			tToken;
			
			for (int n = 0; n < (int)vLineStr.size(); n++)
			{
				tToken.SetString(vLineStr[n]);
				vTmp.clear();
		
				while(tToken.Next())
				{
					if (tToken.Get().empty()) continue;
					vTmp.push_back(tToken.Get());
					if (nArgNum != 0 && (int)vTmp.size() > nArgNum) break;
				}
		
				if (nArgNum == 0 || (int)vTmp.size() == nArgNum)
					vData.push_back(vTmp);
			}
			
			tLineArgData = vData;
			return SUCCESS;
		}
	
		/**
		* 	shell 명령을 실행
		* @param sCmd 실행내용
		* @return vector형태의 실행 내용
		*/
		static vector<string> CmdExecute(const string& sCmd)
		{
			char 	cLine[1024];
			vector<string>	vPrtList;
			string 	sTmp = sCmd;
			if (sTmp.size() == 0) return vPrtList;
		
			sTmp.append(" 2> /dev/null");
			FILE*	pfFile = popen(sTmp.c_str(), "r");
			
			if (pfFile)
			{
				rewind(pfFile);
				// 줄단위로 데이터 읽음.
				while (fgets(cLine, sizeof(cLine), pfFile))
				{
					string sLine = cLine;
					sLine = Replace(sLine, "\r", "");
					sLine = Replace(sLine, "\n", "");
					vPrtList.push_back(sLine);
				}
				pclose(pfFile);
			}
			return vPrtList;
		}

		/**
		 * File 내용을 읽는다.
		 * @param sFileName 파일이름
		 * @return 내용데이터
		 */
		static vector<string> GetFileData(const string& sFileName)
		{
			char 	cLine[1024];
			vector<string>	vPrtList;
			
			if (sFileName.size() == 0) return vPrtList;
			FILE*	pfFile = fopen(sFileName.c_str(), "r");
			if (pfFile)
			{
				rewind(pfFile);
				// 줄단위로 데이터 읽음.
				while (fgets(cLine, sizeof(cLine), pfFile))
				{
					string sLine = cLine;
					sLine = Replace(sLine, "\r", "");
					sLine = Replace(sLine, "\n", "");
					vPrtList.push_back(sLine);
				}
				fclose(pfFile);
			}
			return vPrtList;
		}
	};
};

#endif

