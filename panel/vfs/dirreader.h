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

#ifndef __DIRREADER_H__
#define __DIRREADER_H__

#include "define.h"
#include "strutil.h"
#include "mlsdialog.h"
#include "selection.h"
#include "reader.h"

namespace MLS
{

class DirReader:public Reader
{
protected:
	DIR*		_pHandle;	///< DIR Handle
	dirent*		_pEntry;	///< dirent struct
	struct stat _StatBuf;	///< file stat

protected:
	string	GetUserName(uid_t		tUidNum);
	string	GetGroupName(gid_t		tGidNum);

public:
	DirReader();
	~DirReader();
	
	bool	Init(const string& sInitFile = "") 
	{ 
		_sInitTypeName = "file://"; _bConnected = true; 
		return true;
	}
	void	Destroy();

	string	GetViewPath() const { return _sCurPath; }
	string  GetRealPath(const string& str) const;

	bool Read(const string &path);
	bool Next();
	bool GetInfo(File &);
	
	bool isRoot();

	bool isChkFile(const File& tFile)
	{
		if (access(tFile.sFullName.c_str(), R_OK) == -1)
		{
			MsgBox(_("Error"), strerror(errno));
			return false;
		}
		return true;
	}

	bool	Rename(File* pFile, const string& sRename = "")
	{
		if (pFile == NULL) 
		{
			LOG_WRITE("Rename pFile is NULL !!!");
			return false;
		}

		string sRenameName = pFile->sName;
		if (sRenameName == "..") return false;

		if (sRename == "")
		{
			if (InputBox(_("Rename"), sRenameName) == ERROR) 
				return false;
		}
		else
			sRenameName = sRename;
		
		sRenameName = _sCurPath + sRenameName;
		int err = rename(pFile->sFullName.c_str(), sRenameName.c_str());
		if (err == -1 && errno == EXDEV)
		{
			MsgBox(	_("Error"), strerror(errno));
			return false;
		}
		pFile->sFullName = sRenameName;
		return true;
	}

	bool	Mkdir(const string& sFullPathName = "")
	{
		string sMkdirName;
		if (sFullPathName == "")
		{
			if (InputBox(_("Make Directory"), sMkdirName) == ERROR) 
				return false;
		}
		else
			sMkdirName = sFullPathName;
		
		sMkdirName = _sCurPath + sMkdirName;
		if (mkdir(sMkdirName.c_str(), 0755)==-1)
		{
			MsgBox(	_("Error"), strerror(errno));
			return false;
		}
		return true;
	}

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
					Selection*	pSelection = NULL);

	///	\brief	파일 삭제
	///	\return	삭제 여부
	bool	Remove(Selection&	tSelection, bool bMsgShow = true, bool bIgnore = false);
	
	///	\brief	파일 붙여넣기 할때 file일 경우는 Copy를 그냥  이용한다.
	///	\return	false  리턴
	bool	Paste(Selection& tSelection)
	{
		return false;
	}
};

};

#endif
