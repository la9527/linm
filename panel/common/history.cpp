///	\file	history.cpp
///	\brief	명령어 history 정보 소스
#include "history.h"

#include "define.h"

using namespace std;
using namespace MLS;

static string nullstr;		///<	history 파일안에서만 존재하는 null변수

///	\brief	생성자
History::History(const string& sName)
{
	_nCur = -1;
	_bAdd  = true;
	_sFileTypeName = sName;
}

//             c   
// 0 1 2 3 4 5 6 7 8 
// a b c d e f g h

///	\brief	명령어 history를 추가한다.
///	\param	path	??
void History::AddEntry(const string &sPath)
{
	if (!_bAdd)
	{
		_bAdd = true;
		return;
	}
	_nCur++;
	
	// . history용량이 다 찾으면 하나 제거
	if ((int)_vPaths.size() > _nCur)
		_vPaths.erase(_vPaths.begin()+_nCur, _vPaths.end());
	
	// . history에 추가
	_vPaths.push_back(sPath);
}

///	\brief	바로 하나전 history를 알아낸다.
///	\return	바로 하나전 history
const string &History::GetPrev()
{
	if (_nCur == -1) return nullstr;
	if (!isFront()) _nCur--;	
	
	_bAdd = false;
	
	return _vPaths[_nCur];
}

///	\brief	다음 명령 history로 간다.
///	\return	다음 명령 history
const string &History::GetNext()
{
	if (_nCur==-1) return nullstr;
	if (!isBack()) _nCur++;
	
	_bAdd = false;
	
	return _vPaths[_nCur];
}

///	\brief	현재 history위치가 가장 처음인지 알아내는 함수
///	\return	true	:	현재 위치가 가장 처음임.\n
///			false	:	현재 위치가 처음이 아님.
bool History::isFront()
{
	return (_nCur == 0);
}

///	\brief	현재 history위치가 가장 마지막인지 알아내는 함수
///	\return	true	:	현재 위치가 가장 마지막임.\n
///			false	:	현재 위치가 마지막이 아님.
bool History::isBack()
{
	return (_nCur == (int)_vPaths.size()-1);
}

HistoryMap::HistoryMap()
{	
}

HistoryMap::~HistoryMap()
{
}

void 	HistoryMap::AddEntry(const std::string& sName, const std::string & sPath)
{
	for (uint n = 0; n < _vHistory.size(); n++)
	{
		if (_vHistory[n].GetFileTypeName() == sName)
		{
			_vHistory[n].AddEntry(sPath);
			return;
		}	
	}

	History		tHistory( sName );
	tHistory.AddEntry( sPath );
	_vHistory.push_back(tHistory);
}	

const std::string &HistoryMap::GetPrev(const std::string& sName)
{
	for (uint n = 0; n < _vHistory.size(); n++)
		if (_vHistory[n].GetFileTypeName() == sName)
			return _vHistory[n].GetPrev();

	return nullstr;
}

const std::string &HistoryMap::GetNext(const std::string& sName)
{
	for (uint n = 0; n < _vHistory.size(); n++)
		if (_vHistory[n].GetFileTypeName() == sName)
			return _vHistory[n].GetNext();

	return nullstr;
}

bool 	HistoryMap::isFront(const std::string& sName)
{
	for (uint n = 0; n < _vHistory.size(); n++)
		if (_vHistory[n].GetFileTypeName() == sName)
			return _vHistory[n].isFront();

	return false;
}

bool 	HistoryMap::isBack(const std::string& sName)
{
	for (uint n = 0; n < _vHistory.size(); n++)
		if (_vHistory[n].GetFileTypeName() == sName)
			return _vHistory[n].isBack();

	return false;
}
