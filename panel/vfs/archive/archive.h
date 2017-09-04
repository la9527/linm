/* *************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 *                                                                         *
 ************************************************************************* */ 

///	\file	archive.h
///	\brief	압축 파일 헤더
#ifndef __Archive_h__
#define __Archive_h__
 
#include "define.h"
#include "mlslog.h"
#include "mlslocale.h"
#include "strutil.h"
#include "file.h"
#include "mlsdialog.h"
#include "cmdshell.h"

namespace MLS
{

///	\brief	압축 화일 확장자
enum ZIPTYPE
{
	TAR_GZ = 1,			///< tar.gz
	TAR_BZ,			///< tar.bz
	TAR,			///< tar
	GZ,				///< gz
	BZ,				///< bz
	ZIP,			///< zip
	RPM,			///< rpm
	DEB,			///< deb
	ALZ,			///< unalz
	RAR,			///< rar
	ISO				///< iso
};

///	\brief	tar 옵션
enum
{
	TAR_APPEND,		///< tar append
	TAR_DELETE		///< tar delete
};

class Archive
{
	private:
		string			_sTmpDir;			///< tmp data
		string  		_sFullFilename;		///< fullpath zip filename
		string			_sFilename;			///< filename
		string			_sDir;				///< directory
		string			_sTarCommand;		///< tar name
		
		//string			_sCwd;			///< 작업 디렉토리
		
		vector<File*>	_tFileList;			///< 압축 파일에 있는 filelist
		vector<string>	_tErrorInfo;			///< error info
		int				_eZipType;			///< 압축 file 정보(tar.gz, tar...)
	
		string			DetectTarCommand() const;

	protected:
		void	FileListClear();
		string 	GetTarFileName(const string& sName);
		int		CommandExecute(const string& sCommand);
		int		Fullname_To_Filename(string* sFullFile, string* sFilename, bool* bDir);

		int		FileListCheck_DirInsert(void);
		int		LineFormatRead(vector<string>&	vLineToken, File* pFileInfo, int nFormat);

		int 	ReadLine_TarGZ( vector<string>&	vLineToken, File* pFileInfo );
		int 	ReadLine_RPM( vector<string>&	vLineToken, File* pFileInfo );
		int 	ReadLine_DEB( vector<string>&	vLineToken, File* pFileInfo );
		int 	ReadLine_ZIP( vector<string>&	vLineToken, File* pFileInfo );
		int 	ReadLine_ALZ( vector<string>&	vLineToken, File* pFileInfo );
		int 	ReadLine_ISO( vector<string>&	vLineToken, File* pFileInfo );
		int 	ReadLine_RAR( vector<string>&	vLineToken, File* pFileInfo );

	public:
		Archive(const string& sDirFilename);
		~Archive();

		int		FileListRead(void);
		int		GetZipType(const string& sFileName);
		
		int		GetDir_Files(const string& s_Dir, vector<File*> *pRe_Dir);
		int		GetDir_AllFiles(const string& s_Dir, vector<File*> *pRe_Dir);
		int		GetAll_Files(vector<File*>* rRe_File);

		int		FileControl(const string& sFullName, int nAppendDel, const string& sInsertTmpPath = "");
		int		Compress(vector<File*>& vFileList, int nAppendDel = TAR_APPEND, const string& sInsertTmpPath = "");
		int		Compress(vector<string>& vFilenameList, int nAppendDel = TAR_APPEND);
		int		Compress(const File* pFile, int nAppendDel = TAR_APPEND);
		int		Compress(const string& s_Dir = ".");
		
		int		Uncompress(const File* pFile, const string& sTargetDir = ".");
		int		Uncompress(vector<File*>& t_FileList, const string& sTargetDir = ".");
		int		Uncompress(vector<string>& t_FileList, const string& sTargetDir = ".");
		int		Uncompress(const string& s_Dir = ".");	

		ullong	GetAllDataSize(void);

		int		GetCurZipType() { return _eZipType; }

		///	\brief	에러 리스트가 있으면 돌려주기 위한 함수
		vector<string>		GetErrorList(void)	{ return _tErrorInfo; }
};

};

#endif
