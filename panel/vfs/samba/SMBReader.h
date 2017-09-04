#ifndef __SFTPREADER_H__
#define __SFTPREADER_H_

#include "define.h"
#include "reader.h"

using namespace MLSUTIL;

namespace MLS
{

class SMBReader:public Reader
{
protected:
	string		_sInitFile;
	string		_sViewPath;
	string		_sHome;

	void*		_pContext;
	void*		_pDirent;
	int			_nDir;
	bool		_bNoAuth;
	string		_sGroupName;
	
protected:
	void		SMBShareRead( const string strType, MLS::File& tFile );
	void		SMBFileRead( MLS::File& tFile, const struct stat* pStatBuf );

public:
	SMBReader();
	~SMBReader();

	void	Destroy();

	///	\brief	초기화 함수
	bool 	Init(const string& sInitFile = "");

	string GetViewPath() const;
	string GetRealPath(const string& sPath) const;
	
	///	\brief	초기화 함수
	///	\param	path	읽을 directory 명
	///	\return	???	bool 	Read(const string& sPath) { return false; }

	bool 	Read(const string& sPath);

	///	\brief	현재 파일정보를 얻음
	///	\param	r	반환값
	///	\return	정보 얻기 성공 여부
	bool 	GetInfo(MLS::File& tFile);

	///	\brief	다음 원소로 위치를 이동
	///	\return	위치 이동 성공 여부
	bool 	Next();

	bool 	isChkFile(const File& tFile);

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

	// 여러개의 파일 복사, 이동, 삭제 부분은 Select 를 이용해서 사용한다.
	// 모든 파일은 /tmp에 복사 하거나 이동하고 나서 다른곳으로 복사한다.
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
		return false;
	}
	
	///	\brief	파일 삭제
	///	\return	삭제 여부
	bool	Remove(MLS::Selection& tSelection, bool bMsgShow = true, bool bIgnore = false);

	///	\brief	파일을 보기 위한..
	///	\param	tFileOriginal	볼 원본 파일
	///	\param	pFileChange		Target 파일
	///	\return	압축 파일을 tmp 에 복사 해놓고 파일 위치 리턴
	bool	View(const File* tFileOriginal, File* pFileChange);

	///	\brief	파일 붙여넣기 할때
	///	\return	false  리턴
	bool	Paste(Selection& tSelection);

	bool	Rename(File* pFile, const string& sRename = "");
	bool	Mkdir(const string& sFullPathName = "");

	string	GetInitType() 		{ return _sInitTypeName; }
	void	EncodeChk(vector<File*>& tFileList, bool bEncChk);
};

};

#endif
