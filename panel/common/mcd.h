///	\file	Mcd.h
///	\brief	Mcd.h
#ifndef MLSMcd_H__
#define MLSMcd_H__

#include "define.h"
#include "file.h"
#include "strutil.h"
#include "mlslocale.h"
#include "reader.h"
#include "mlsdialog.h"
#include "keycfgload.h"

namespace MLS {

class Dir
{
public:
	File	tFile;
	int  	nDepth,		///< directory 깊이
			nRow,		///< 위치
			nIndex;		///< index?
	Dir*	pParentDir;	///< parent directory
	bool	bCheck;		///< 현재 디렉토리를 체크 했는가?
	
	vector<Dir*> vNode;		///< node
	
	///	\brief	생성자
	///	\param	p		directory name
	///	\param	pa		parent directory
	///	\param	n		nlink :  number of hard links
	///	\param	l		linked : 링크 인가?
	Dir(File& tF, Dir *pa, bool Chk = false)
		: pParentDir(pa), bCheck(Chk)
	{
		tFile = tF;
		nDepth = pa ? pa->nDepth+1 : 0;
	}

	Dir(string sPath, Dir *pa, bool Chk = false)
		: pParentDir(pa), bCheck(Chk)
	{
		File	tFileTmp;
		string 	sTmp;

		if (sPath == "/" || sPath.size() == 0)
		{
			tFileTmp.sName = "/";
			tFileTmp.sFullName = "/";
		}
		else
		{
			tFileTmp.sFullName = sPath;
			
			if (tFileTmp.sFullName.substr(tFileTmp.sFullName.size()-1, 1) == "/") 
				sTmp = tFileTmp.sFullName.substr(0, tFileTmp.sFullName.size()-1);
			else
				sTmp = tFileTmp.sFullName;
			
			tFileTmp.sName = sTmp.substr(sTmp.rfind("/")+1);
		}
		tFileTmp.bLink = false;
		tFileTmp.bDir = true;
		
		tFile = tFileTmp;
		nDepth = pa ? pa->nDepth+1 : 0;
	}
	
	string Path()
	{
		return tFile.sFullName;
	}
};

///	\brief	Mcd class
class Mcd
{
protected:
	///	\brief	directory 정보 
	///	\brief	mcdsort
	class McdSort
	{
	public:
		///	\brief	operator() 함수
		///	\param	a	directory 정보
		///	\param	b	directory 정보
		///	\return	true :		a directory 명이 b directory 명보다 우선 정렬
		///			false:		b directory 명이 a directory 명보다 우선 정렬
		bool operator()(Dir *a, Dir *b)	{return a->tFile.sName < b->tFile.sName;}
	};
	
	bool            _bMcdSort;
	bool			_bHidden;
	string			_sCode;
	
	typedef vector<Dir*>			pDirContainer;	///< dir * 벡터 재정의
	typedef vector<Dir*>::iterator	pDirIterator;	///< dir * iterator 재정의
	
	pDirContainer	_pOrder;  		///< 순회 테이블
	pDirIterator	_pCur;			///< 현 위치 index
	
	Dir*			_pRoot;        	///< 루트 트리..  마지막 '/' 을 뺀 이름만을 기록
	int				_nSRow,			///< 스크롤 한 Row
					_nSCol;			///< 스크롤 한 Col

	bool			_bExit;			///< exit 여부
	string			_sStrSearch;    ///< 검색단어
	bool			_bSearch;		///< 검색여부
	int				_nSearchCnt;	///< Tab Counter
	
	int				_nCounter;		/// 현재 진행상황 전달
	Reader*			_pReader;
	
	// 순회테이블 재정리, _pCur 무효화
	void SetOrder();
	
	void Destroy(Dir *pDir, bool = true);
	
	// draw function
	virtual void	Refresh() 	{}
	virtual void	Draw() 		{}

	Dir*	GetDir_RowArea(int nFindRow, int nDepth)
	{
		vector<Dir*>	tTmpDirList_Over;
		vector<Dir*>	tTmpDirList_Under;

		Dir*	pNode = NULL;
		for (int i = 0; i < (int)_pOrder.size() ;i++)
		{
			pNode = _pOrder[i];
			if (pNode->nDepth == nDepth)
			{
				if (pNode->nRow <= nFindRow)
				{
					tTmpDirList_Under.push_back(pNode);
					LOG_WRITE("pNode Under [%d] [%d]", nFindRow, pNode->nRow);
				}
				if (pNode->nRow > nFindRow)
				{
					tTmpDirList_Over.push_back(pNode);
					LOG_WRITE("pNode Over [%d] [%d]", nFindRow, pNode->nRow);
				}
			}
		}

		Dir*	pDirOver = NULL;
		Dir*	pDirUnder = NULL;

		if (tTmpDirList_Under.size() != 0) 
			pDirUnder = tTmpDirList_Under[tTmpDirList_Under.size()-1];
		if (tTmpDirList_Over.size() != 0)
			pDirOver = tTmpDirList_Over[0];
		
		if (pDirOver && pDirUnder)
		{
			if (pDirOver->nRow - nFindRow < nFindRow - pDirUnder->nRow)
				return pDirUnder;
			else
				return pDirOver;
		}
		else if (pDirOver)	return pDirOver;
		else if (pDirUnder)	return pDirUnder;
		return NULL;
	}
	
public:
	Mcd(const string& sDir = "");
	virtual ~Mcd();

	// scan fuction
	bool  Load(const char* sFile);
	bool  Save(const char* sFile);

	void Destroy() { Destroy(_pRoot, false); _pRoot = NULL; _pReader = NULL; }
	
	void	SetCur(const std::string &sFullName);
	int		Search(const std::string &sStr, int nNextNum = 0);

	///	\brief	디렉토리 재검색 
	int  Rescan(int nDepth=0);

	///	\brief	디렉토리 검색 
	int  Scan(Dir *pDir, int nDepth=1);

	// 커서이동	
	void Key_Left();
	void Key_Right();
	void Key_Up();
	void Key_Down();

	virtual void Key_Home();
	virtual void Key_End();
	
	virtual void	Key_PageDown() {}
	virtual void	Key_PageUp() {}
	
	int		SubDirOneSearch();
	int		SubDirAllSearch();
	int		SubDirAnySearch(int nDepth = 2);
	int		SubDirClear();

	int		SearchProcess(KeyInfo&	tKeyInfo);
	
	int		GetCounter() { return _nCounter; }
	void	SetReader(Reader* pReader)	{ _pReader = pReader; }
	Reader*	GetReader()					{ return _pReader; }

	///	\brief	디렉토리 추가
	bool	AddDirectory(const string &);

	///	\brief	현재 위치한 정보를 얻어낸다.
	///	\return	현재 위치한 정보
	string	&GetCurName() const {return (*_pCur)->tFile.sFullName;}
	File	GetCurFile() 		{ return (*_pCur)->tFile;}
	File	GetParentFile()
	{ 
		Dir* pParentDir =  (*_pCur)->pParentDir;
		return pParentDir->tFile;
	}

	bool	GetHiddenView()	{ return _bHidden; }
	void	SetHiddenView(bool bView = true) { _bHidden = bView; }

	void	MountList();
};

}; // namespace

#endif //MLSmcd_H__
