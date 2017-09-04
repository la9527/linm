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

#ifndef __PANEL_H__
#define __PANEL_H__

#include "define.h"
#include "mpool.h"
#include "file.h"
#include "selection.h"
#include "reader.h"
#include "readerctl.h"
#include "vfsplugin.h"
#include "keycfgload.h"
#include "mlscfgload.h"
#include "history.h"

// color 변경 사용 방안 : !01#is@

using namespace MLSUTIL;

namespace MLS
{

/// @brief  정렬 방식.
enum SortMethod
{
	SORT_NONE,          ///< 무 정렬.
	SORT_NAME,          ///< 파일이름 순으로 정렬.
	SORT_EXT,           ///< 확장자 순으로 정렬.
	SORT_SIZE,          ///< 파일 크기 순으로 정렬.
	SORT_TIME,          ///< 파일 시간 순으로 정렬.
	SORT_COLOR,		    ///< 색깔 순으로 정렬.
	SORT_END_			///< 유효하지 않은 정렬 값
};

class Panel
{
protected:
 	vector<File*>	_vDirFiles;
	MemoryPool<File> _tMemFile;
	Reader*			_pReader;
	ReaderCtl		_tCtlReader;
	int				_nSortMethod;	///< sort 방식
	bool			_bDirSort;		///< 디렉토리 sort 

	string			_sReaderType;
	string			_sCurPath;		///< 현재 디렉토리 위치
	string			_sFileCurPath;  ///< 플러그인에 들어 가기전 원래 위치
	string			_sBefReaderType;///< 플러그인 들어가기 전 Reader Type

	string			_sStrSearch;

	HistoryMap		_tHistoryMap;

	bool			_bRoot;			///< root directory 인가?
	bool			_bShowHidden; 	///< Show/Hidden

	uint 			_uCur;			///< 커서 위치(file index값)
	
	int				_nCurFile;

protected:
	virtual void	Refresh() 	= 0;
	virtual void	Draw() 		= 0;

	/// @brief		파일위치 설정.
	void SetCur(const int nCur)
	{
		_uCur = nCur;

		if ( _nCol != 0 && _nRow != 0 )
			_nPage = _uCur / (_nCol * _nRow);
	}

public:
	uint _uDir,			///< 디렉토리 개수
		 _uFile,		///< 파일 개수
		 _uDirSize;		///< 현재 디렉토리 안의 파일크기 들의 합

	uint _uSelNum,		///< 선택된 파일 개수
		 _uSelSize;		///< 선택된 파일 사이즈

	int  _nCol,			///< 컬럼 사이즈
		 _nRow,			///< 라인 사이즈
		 _nPage;		///< 페이지 위치

	bool	_bChange;	///< 전체 바뀜여부
	bool	_bSearch;	///< 검색 여부
	int		_nSort;		///< 현재 소트 위치

	bool	_bDirSortAscend;///< 디렉토리 정렬 순서
	bool	_bFileSortAscend;///<파일 정렬 순서

public:
	Panel()
	{	
		_sReaderType = "file";
		_pReader = NULL;
		_bShowHidden = false;
		_nSortMethod = SORT_COLOR;

		_bDirSortAscend = true;
		_bFileSortAscend = true;

		_bDirSort = true;
		_bShowHidden = false;
		_nSortMethod = SORT_COLOR;

		PluginLoader(&_tCtlReader);
	}

	virtual ~Panel()	{ }

	void	Init();
	void	ConfigureLoad();

	File*		GetCurFile() 
	{ 
		uint n = _vDirFiles.size();
		if (n > _uCur && n != 0)
			return _vDirFiles[_uCur];
		return NULL;
	}

	File*		GetNextFile()
	{
		uint n = _vDirFiles.size();
		if (n > _uCur+1 && n != 0)
			return _vDirFiles[_uCur+1];
		if ( n >= _uCur+1 && n != 0)
			return _vDirFiles[n-1];
		return NULL;
	}

	const string		GetPath() const
	{
		return _pReader->GetPath();
	}

	string		GetPathView() const;

	Reader*		GetReader()	{ return _pReader; }
	Reader*		GetReader(const string& sPathOrType);
	ColorEntry  GetColor(File* pFile);
	void 		Sort(void);

	int 		GetSortMethod() { return _nSortMethod; }
	void		SetSortMethod(int nSortMethod) { _nSortMethod = nSortMethod; }
	bool		GetHiddenView()	{ return _bShowHidden; }
	void		SetHiddenView(bool bView);
	void		SetHiddenView();

	bool		PluginOpen(const string& sReaderInit, const string& sType = "file")
	{
		_sFileCurPath = _sCurPath;
		_sBefReaderType = _sReaderType;
		_sReaderType = sType;
		Reader* 	pReader = _tCtlReader.Get(sType);
		
		if ( pReader == NULL ) 
			pReader = _tCtlReader.Get("file");	
		
		if ( pReader->Init(sReaderInit) == false )
		{
			PluginClose();
			return false;
		}
		if ( Read("") == false )
		{
			PluginClose();
			return false;
		}
		return true;
	}

	bool		PluginClose(const string& sType = "")
	{
		_sCurPath = _sFileCurPath;
		if (sType == "")
			_sReaderType = _sBefReaderType;
		else
			_sReaderType = sType;
		_pReader->Destroy();

		return Read(_sCurPath);
	}

	virtual void	ReadEnd() {}

	bool	Read(const string& sPath);
	
	void	Key_Left();
	void	Key_Right();
	void	Key_Up();
	void	Key_Down();
	void	Key_PageUp();
	void	Key_PageDown();
	void	Key_Home();
	void	Key_End();
	void	Key_Enter();

	void	Key_Select();

	void 	Select(File &f);
	void 	Deselect(File &f);
	void	ToggleSelect();

	// select
	int		GetSelection(Selection& tSelection);
	
	void 	SelectAll()
	{
		for (int t=0; t< (int)_vDirFiles.size(); t++)
			Select(*_vDirFiles[t]);
	}

	void 	SelectInvert() 
	{
		for (int t=0; t < (int)_vDirFiles.size(); t++)
			if (_vDirFiles[t]->bSelected) 
				Deselect(*_vDirFiles[t]);
			else 
				Select(*_vDirFiles[t]);
	}
	
	bool	SearchProcess(KeyInfo&	tKeyInfo);
	bool	SearchExactFile(const string & fileName, uint & fileIndex, bool bFullName = false) const;
	bool	SearchMatchingFile(const string &str, uint & d_index, int s_index = 0);
	bool	SetCurFileName(const string& sFileName);

	void	SelectExecute();

	virtual void	ParseAndRun(const string &sCmd, bool bPause = false) = 0;
	virtual void	Execute(const string& sCmd) = 0;

	void	MountList();
	void	Back();
	void	Forward();
};

};

#endif
