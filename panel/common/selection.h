#ifndef __SELECTION_H__
#define __SELECTION_H__

#include "define.h"
#include "file.h"

namespace MLS {

class Reader;

/// 선택
class Selection
{
	vector<File*> 	_vFile;
	string			_sSelPath;
	bool			_bUseExpandDir;
	Reader*			_pReader;
	
public:
    Selection() { _bUseExpandDir = true; }
	~Selection()
	{
		Clear();
	}

	void				SetSelectPath(const string& sPath)	{ _sSelPath = sPath; }
	string				GetSelectPath()	{ return _sSelPath; }

	void 				Add(File*	pFile)
	{
		File* pFileNew = new File;
		*pFileNew = *pFile;
		_vFile.push_back(pFileNew);
	}

	void 				Clear()
	{ 
		File* 	pFile = NULL;
		for (int n = 0; n < (int)_vFile.size(); n++)
		{
			pFile = _vFile[n];
			if (pFile) delete pFile; pFile = NULL;
		}	
		_vFile.clear();
		_bUseExpandDir = true;
	}

	Reader*				GetReader()						{ return _pReader; }
	void				SetReader(Reader* pReader)		{ _pReader = pReader; }

	vector<File*>&		GetData() 		 				{ return _vFile; }
	File*		 		operator[](int index) const 	{ return _vFile[index]; }
	
	/// File 개수를 얻는다
	uint  				GetSize() const					{ return _vFile.size();	}
	
	/// 데이터의 크기를 계산한다 ( 파일마운트라고 가정함 )
	ullong  			CalcSize(Reader* pReader = NULL);
	
	/// 데이터 내용을 디렉토리에 대하여 확장시킨다
	int 				ExpandDir(Reader* pReader, bool bCtlBreak = false);
};

/// 클립 보드
class ClipBoard
{	
	//Selection vData;
	ClipState nState;
	
public:	
    ClipBoard();
    ~ClipBoard();
	
	void Set(ClipState);
	ClipState  GetState();	
};

};

#endif
