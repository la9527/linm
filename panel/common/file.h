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

///	\file	file.h
///	\brief	file.h
#ifndef __FILE_H__
#define __FILE_H__

#include "define.h"
#include "colorentry.h"

using namespace MLSUTIL;

namespace MLS {

// type 은  string으로 결정한다.
// file://
// ftp://
// sftp://
// network://
// sftp://test:test@test.com/home/test
// tar:/home/TEST/mls_0.6.7-3.tar.gz/mls-0.6.7/src

///	\brief	File class
class File
{
	public:
		string		sType;		///< 파일 type               (file://), (ftp://test:test@test.com)
		string 		sName;		///< 화면에 보여줄 file name (test.tmp)
		string		sLinkName;	///< Link 명
		string 		sFullName;	///< Full File Name          (/home/test.tmp)
		string		sDate;		///< 날짜
		string		sTime;		///< 시간
		string		sAttr;		///< 파일 속성 drwxrwxrwx , 각 시스템 마다 호환성 있게 string 으로 결정.
		string		sOwner;		///< 관리자
		string		sGroup;		///< 그룹
		string		sTmp;		///< Tmp.	tar.gz 의 파일 위치
		string		sTmp2;		///< File	tmp 복사 위치
		string		sTmp3;		///< other  Temp
		
		time_t		tCreateTime; ///< 생성시간

		ullong 		uSize;		///< 파일 size
		bool 		bDir;		///< directory 여부
		bool		bLink;		///< Link 여부
		bool 		bSelected;	///< 선택 여부

		ColorEntry	tColor;		///< 화면에 출력될 색상
		//void*		pTmp1;		///< Tmp1
		//void*		pTmp2;		///< Tmp2
		int			nY,			///< mouse Y
					nX;			///< mouse X
		
		/// \brief	파일 확장자를 알아내는 함수
		///	\return	파일의 확장자
		inline string Ext()
		{
			string::size_type p = sName.rfind(".");
			if (p == string::npos || p==0)
				return "";
			return sName.substr(p+1);
		}

		inline bool	isExecute()
		{
			return sAttr[3] == 'x' ? true : false;
		}
		
		///	\brief	생성자
		File()
		{
			uSize = 0;
			bSelected = false;
			bDir = false;
			bLink = false;
			tColor = ColorEntry();
		}
		
		///	\brief	File 정보 초기화
		inline void Clear(void)
		{
			sType = ""; sName = ""; sFullName = "";
			sDate = ""; sTime = ""; sAttr = "";
			uSize = 0;
			bSelected = false;
			bDir = false;
			tColor = ColorEntry();
		}

		/*
		File& operator= (File tFile)
		{
			sType = tFile.sType;
			sName = tFile.sName;
			sFullName = tFile.sFullName;
			sDate = tFile.sDate;
			sTime = tFile.sTime;
			sAttr = tFile.sAttr;
			sOwner = tFile.sOwner;
			sGroup = tFile.sGroup;
			sTmp = tFile.sTmp;	
			tCreateTime = tFile.tCreateTime;
			uSize = tFile.uSize;
			bDir = tFile.bDir;
			bLink = tFile.bLink;
			bSelected = tFile.bSelected;
			tColor = tFile.tColor;
			return *this;
		}
		*/
};

}; // namespace

#endif
