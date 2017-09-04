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

#ifndef __FTPREADER_H__
#define __FTPREADER_H__

#include "define.h"
#include "strutil.h"
#include "mlsdialog.h"
#include "selection.h"
#include "reader.h"

#include "ftplib.h"

using namespace MLSUTIL;

namespace MLS
{

class FtpReader:public Reader
{
private:
	string			_sIP;
	string			_sUser;
	string			_sHome;		///< 홈디렉토리
	string			_sInitFile;

protected:
	netbuf* 		_pDefaultFtpNet; // ftp Net
	vector<File*>	_vFileLists;
	
	string	GetPwd() const;
	int		GetIpUserPw(const string& sStr, string& sIP, string& sUser, string& sPasswd, string& sDir);
	int		LineFormatRead(vector<string>&	vLineToken, File* pFileInfo);

public:
	FtpReader();
	~FtpReader();
	
	bool	Init(const string& sInitFile = "");
	void	Destroy();

	string	GetViewPath() const 					{ return _sInitTypeName + _sCurPath; }
	string 	GetRealPath(const string& str) const;
	
	bool Read(const string &sPath);
	bool Next();
	bool GetInfo(File &tFile);
	bool isRoot()
	{
		return false;
	}

	bool isChkFile(const File& tFile)
	{		
		return true;
	}

	bool	View(const File* pFileOriginal, File* pFileChange);

	bool	Rename(File* pFile, const string& sRename = "");
	bool	Mkdir(const string& sFullPathName = "");
	bool	Copy(File& tFile, const string& sTargetPath, File* pFile = NULL)
	{
		return false;
	}
	
	bool	Move(File& tFile, const string& sTargetPath, File* pFile = NULL)
	{
		return false;
	}

	bool	Remove(File& tFile)
	{
		return false;
	}

	bool 	Paste(File& tFile)
	{
		return false;
	}	

	///	\brief	파일 복사
	///	\return	복사 여부	
	bool	Copy(	Selection& tSelection, 
					const string& sTargetPath = "",
					Selection*	pSelection = NULL);
	
	///	\brief	파일 이동
	///	\return	이동 여부
	bool	Move(	Selection& tSelection,
					const string& sTargetPath = "",
					Selection*	pSelection = NULL)
	{
		MsgBox(_("Error"), _("ftp file move fail !!!"));
		return false;
	}

	///	\brief	파일 삭제
	///	\return	삭제 여부
	bool	Remove(MLS::Selection& tSelection, bool bMsgShow = true, bool bIgnore = false);
	
	///	\brief	파일 붙여넣기 할때 file일 경우는 Copy를 그냥  이용한다.
	///	\return	false  리턴
	bool	Paste(Selection& tSelection);

	void	EncodeChk(vector<File*>& tFileList, bool bEncChk);
};

};

#endif
