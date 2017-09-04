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

#ifndef __READER_H__
#define __READER_H__

#include "define.h"
#include "file.h"
#include "mlslocale.h"

namespace MLS
{

class Selection;
class Reader
{
	public:
		Reader() {}
		virtual ~Reader() {}

		///	\brief	초기화 함수
		virtual bool Init(const string& sInitFile = "") = 0;
		
		///	\brief	제거 함수
		virtual void Destroy() = 0;

		virtual string GetViewPath() const = 0;
		virtual string GetRealPath(const string& sPath) const = 0;
		
		///	\brief	초기화 함수
		///	\param	path	읽을 directory 명
		///	\return	???
		virtual bool Read(const string& sPath) = 0;

		///	\brief	현재 파일정보를 얻음
		///	\param	r	반환값
		///	\return	정보 얻기 성공 여부
		virtual bool GetInfo(MLS::File& tFile) = 0;

		///	\brief	'/' directory인가?
		///	\return	true	:	'/' diretory\n
		///			false	:	'/' directory가 아니다
		virtual bool isRoot() { return false; }

		///	\brief	다음 원소로 위치를 이동
		///	\return	위치 이동 성공 여부
		virtual bool 	Next() = 0;
		virtual uint	Size() 		{ return _vFiles.size(); }

		virtual bool isChkFile(const File& tFile) = 0;

		virtual bool	Copy(File& /*tFile*/, const string& /*sTargetPath*/, File* /*pFile*/ = NULL)
		{
			return false;
		}
		
		virtual bool	Move(File& /*tFile*/, const string& /*sTargetPath*/, File* /*pFile*/ = NULL)
		{
			return false;
		}
	
		virtual bool	Remove(File& /*tFile*/)
		{
			return false;
		}
	
		virtual bool 	Paste(File& /*tFile*/)
		{
			return false;
		}
	
		// 여러개의 파일 복사, 이동, 삭제 부분은 Select 를 이용해서 사용한다.
		// 모든 파일은 /tmp에 복사 하거나 이동하고 나서 다른곳으로 복사한다.
		///	\brief	파일 복사
		///	\return	복사 여부
		virtual bool	Copy(	Selection& /*tSelection*/, 
								const string& /*sTargetPath*/ = "", 
								Selection*	/*pSelection*/ = NULL)
		{
			return false;
		}
		
		///	\brief	파일 이동
		///	\return	이동 여부
		virtual bool	Move(	Selection& /*tSelection*/, 
								const string& /*sTargetPath*/ = "", 
								Selection*	/*pSelection*/ = NULL)
		{ 
			return false;
		}
		
		///	\brief	파일 삭제
		///	\return	삭제 여부
		virtual bool	Remove(MLS::Selection& /*tSelection*/, bool /*bMsgShow*/ = true, bool /*bIgnore*/ = false)
		{
			return false;
		}

		///	\brief	파일을 보기 위한..
		///	\param	tFileOriginal	볼 원본 파일
		///	\param	pFileChange		Target 파일
		///	\param	pTmpDir			임시 파일 디렉토리 위치
		///	\return	압축 파일을 tmp 에 복사 해놓고 파일 위치 리턴
		virtual bool	View(const File* tFileOriginal, File* pFileChange)
		{
			*pFileChange = *tFileOriginal;
			return true;
		}

		///	\brief	파일 붙여넣기 할때
		///	\return	false  리턴
		virtual bool	Paste(Selection& /*tSelection*/)
		{
			return false;
		}

		virtual bool	Rename(File* /*pFile*/, const string& /*sRename*/ = "")
		{
			return false;
		}
	
		virtual bool	Mkdir(const string& /*sFullPathName*/ = "")
		{
			return false;
		}

		virtual string	GetPath()						 	{ return _sCurPath; }
		virtual	string	GetReaderName() 				 	{ return _sReaderName; }
		virtual void	SetErrMsgShow(bool bFlag = true) 	{ _bMsgShow = bFlag; }
		virtual vector<MLS::File*>&	GetFiles() 				{ return _vFiles; }
		
		virtual uint	GetFilesSize()
		{
			uint	uSize = 0;
			for (uint n = 0; n < _vFiles.size(); n++)
			{
				uSize = uSize + _vFiles[n]->uSize;
			}
			return uSize;
		}
		
		virtual string	GetInitType() 						{ return _sInitTypeName; }
		virtual bool	GetConnected()						{ return _bConnected; }
		virtual void	SetTmpDir(const string& sTmpDir) 	{ _sTmpDir = sTmpDir; }
		
	protected:
		vector<MLS::File*>	_vFiles;
		
		uint			_uCur;
		string			_sCurPath;
		
		string			_sReaderName;
		string			_sInitTypeName;

		bool			_bMsgShow;
		bool			_bConnected;	///< Connected
		string			_sTmpDir;
		ENCODING		_eEncode;		///< Encoding
};

};

#endif
