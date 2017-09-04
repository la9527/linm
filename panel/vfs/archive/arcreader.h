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

#ifndef __ARCREADER_H__
#define __ARCREADER_H__

#include "define.h"
#include "mlslog.h"
#include "strutil.h"
#include "mlsdialog.h"
#include "selection.h"
#include "reader.h"
#include "archive.h"

using namespace MLSUTIL;

namespace MLS
{

class ArcReader:public Reader
{
protected:
	Archive*		_pArchive;
	vector<File*>	_vFileList;
	//vector<string>	_vDeleteFile; ///< 압축된 파일 view 하고 나서 tmp 내용을 지울때 씀.

public:
	ArcReader()
	{
		_pArchive = NULL;
		_uCur = 0;
		_sReaderName = "archive";
		_bConnected = false;
	}

	~ArcReader()
	{
		Destroy();
	}
	
	bool	Init(const string& sInitFile = "");
	void	Destroy();
	
	string GetViewPath() const ;
	string GetRealPath(const string& str) const;
	
	bool Read(const string &sPath);
	bool Next();

	bool GetInfo(File &tFile);
	bool isRoot();
	bool isChkFile(const File& tFile) { return true; }
	
	bool	Rename(File* pFile, const string& sRename = "")
	{
		MsgBox(_("Error"), "Archive file can't rename !!!");
		return false;
	}

	bool	Mkdir(const string& sFullPathName = "")
	{
		MsgBox(_("Error"), "Archive file can't Mkdir !!!");
		return false;
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

	bool	View(const File* tFileOriginal, File* pFileChange);
	bool	Copy(Selection&	tSelection, const string& sTargetPath = "", Selection* pSelection = NULL);
	bool	Move(Selection&	tSelection, const string& sTargetPath = "", Selection* pSelection = NULL);
	bool	Remove(Selection&	tSelection, bool bMsgShow = true);
	bool	Paste(Selection& tSelection);
};

};

#endif
