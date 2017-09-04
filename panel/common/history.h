///	\file	history.h
///	\brief	history.h
#ifndef __HISTORY_H__
#define __HISTORY_H__

#include <string>
#include <vector>
#include <map>

namespace MLS {
	
///	\brief	명령어 history 를 처리하는 class
class History
{
protected:
	std::vector<std::string> _vPaths;	///< paths
	
	int 					_nMax;		///< history 저장 용량
	int 					_nCur;		///< history 정보
	
	bool 					_bAdd;		///< ??
	std::string				_sFileTypeName;
	
public:
	History(const std::string& str);
	virtual ~History() {}

	const std::string& GetFileTypeName() { return _sFileTypeName; }

	void 	AddEntry(const std::string & sPath);
	
	const std::string &GetPrev();
	const std::string &GetNext();
	
	bool 	isFront();
	bool 	isBack();
};

class HistoryMap
{
protected:
	std::vector<History>		_vHistory;
	
public:
	HistoryMap();
	~HistoryMap();

	void 	AddEntry(const std::string& sName, const std::string & sPath);

	const std::string &GetPrev(const std::string& sName);
	const std::string &GetNext(const std::string& sName);
	
	bool 	isFront(const std::string& sName);
	bool 	isBack(const std::string& sName);
};

}; // namepsace

#endif
