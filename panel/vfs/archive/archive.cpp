/* *************************************************************************
 *   Copyright (C) 2004 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 *                                                                         *
 ************************************************************************* */

///	\file	archive.cpp
///	\brief	archive.cpp

#include "archive.h"

using namespace MLSUTIL;
using namespace MLS;

///	\brief	생성자
///	\param	sDir_Filename	Full file name(directory 포함)
///	\param	isZip			zip 파일 여부
Archive::Archive(const string& sDirFilename)
: _sTarCommand(DetectTarCommand()), _sFullFilename(sDirFilename)
{
	string::size_type tSize = sDirFilename.find("/");
	if (tSize != string::npos)
	{
		// . directory name 설정
		_sDir = sDirFilename.substr(0, sDirFilename.find_last_of("/"));
		// . file name  설정
		_sFilename = sDirFilename.substr(	sDirFilename.find_last_of("/") + 1,
						sDirFilename.length() - sDirFilename.find_last_of("/") - 1);
	}
	else
	{
		_sDir = "./";
		_sFilename = sDirFilename;
	}

	_eZipType = GetZipType(_sFilename);
}

///	\brief	소멸자
Archive::~Archive()
{
	FileListClear();
}

void	Archive::FileListClear()
{
	File*	pFile = NULL;
	uint	nCount;
	
	for (nCount = 0; nCount < _tFileList.size(); nCount++)
	{
		pFile = _tFileList[nCount];
		if (pFile) delete pFile;
	}
	_tFileList.clear();
}

///	\brief	파일이 있는지 확인
///	\param	sFilename	filename
///	\return	파일이 있는지 여부
inline bool IsFileCreate(const string& sFilename)
{
	if (access(sFilename.c_str(), R_OK) == -1)
		return true;
	return false;
}

//	\brief	Command 명령 실행. 
///	\param	sCommand	실행할 명령어
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::CommandExecute(const string& sCommand)
{
	char	cLine[1024];
	FILE*	pfFile = NULL;
	_tErrorInfo.clear();

	LOG_WRITE("Command Execute [%s]", sCommand.c_str());
	
	string sCmd = sCommand;
	sCmd.append(" 2> /dev/null");
	pfFile = popen(sCmd.c_str(), "r");
	
	if (pfFile) 
	{
		rewind(pfFile);
		// 줄단위로 데이터 읽음.
		while (fgets(cLine, sizeof(cLine), pfFile))
		{
			LOG_WRITE("Command Error :: [%s]", cLine);
		}
		pclose(pfFile);
	}
	else
	{
		MsgBox("Error", _("File open error !!!"));
		return ERROR;
	}
	return SUCCESS;
}

///	\brief	 확장자를 제외한 기본파일 이름 반환 (Test.tar.gz -> Test.tar)
///	\return	변환된 파일 이름
string Archive::GetTarFileName(const string& sName)
{
	string sFilename = "";
	string::size_type tSize;
	
	tSize = sName.rfind(".tar");
	if (tSize != string::npos)
	{
		sFilename = sName.substr(0, tSize+4);
		return sFilename;
	}
	
	tSize = sName.rfind(".tgz");
	if (tSize != string::npos) 
	{
		sFilename = sName.substr(0, tSize);
		sFilename = sName+".tar";
		return sFilename;
	}
		
	tSize = _sFilename.rfind(".tgz");
	if (tSize != string::npos) 
	{
		sFilename = sName.substr(0, tSize);
		sFilename = sName+".tar";
		return sFilename;
	}	
	return sName;
}

///	\brief	확장자 체크 
///	\param	sExt	체크할 파일 확장자
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
inline int	FileExtCheck(const string& sFileName, const char* sExt)
{
	string 		strName = Tolower( sFileName );
	string::size_type tStringSize = strName.rfind(Tolower(sExt));
	if (tStringSize == string::npos)
		return ERROR;
	return SUCCESS;
}

///	\brief	압축 파일 구분.(_eZipType 에 저장)
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int	Archive::GetZipType(const string& sFileName)
{
	if (GetTarFileName(sFileName) == "") return ERROR;

	if (FileExtCheck(sFileName, ".tar.gz") == SUCCESS 
	 || FileExtCheck(sFileName, ".tgz")    == SUCCESS 
	 || FileExtCheck(sFileName, ".tar.z")  == SUCCESS)
	{
		return TAR_GZ;
	}
	else if (FileExtCheck(sFileName, ".tar.bz2") == SUCCESS 
          || FileExtCheck(sFileName, ".tbz")     == SUCCESS
		  || FileExtCheck(sFileName, ".tar.bz")  == SUCCESS)
	{
		return TAR_BZ;
	}
	else if (FileExtCheck(sFileName, ".tar") == SUCCESS)
		return TAR;
	else if (FileExtCheck(sFileName, ".gz") == SUCCESS)
		return GZ;
	else if (FileExtCheck(sFileName, ".bz2") == SUCCESS 
          || FileExtCheck(sFileName, ".bz") == SUCCESS)
		return BZ;
	else if (FileExtCheck(sFileName, ".zip") == SUCCESS
		  || FileExtCheck(sFileName, ".jar") == SUCCESS)
		return ZIP;
	else if (FileExtCheck(sFileName, ".rpm") == SUCCESS)
		return RPM;
	else if (FileExtCheck(sFileName, ".deb") == SUCCESS)
		return DEB;
	else if (FileExtCheck(sFileName, ".alz") == SUCCESS)
		return ALZ;
	else if (FileExtCheck(sFileName, ".iso") == SUCCESS)
		return ISO;
	else if (FileExtCheck(sFileName, ".rar") == SUCCESS)
		return RAR;
	else 
		return ERROR;
	return SUCCESS;
}

///	\brief	파일 전체 경로에서 디렉토리와 파일이름을 빼낸다.
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Fullname_To_Filename(string* sFullFile, string* sFilename, bool* bDir)
{
	string				sTmp;
	string::size_type	tStringSize;
	string::size_type	tStringSize_tmp_1;
	
	tStringSize_tmp_1 = 0;
	sTmp = "";
	
	if (*sFullFile == "") return ERROR;

	for(;;)
	{
		tStringSize_tmp_1 = sFullFile->find("/./", tStringSize_tmp_1);
		if (tStringSize_tmp_1 != string::npos)
		{
			sFullFile->erase(tStringSize_tmp_1, 2);
		}
		else 
			break;
	}

	// 앞에 '.'로 시작되는 파일이면 '.'을 떼어낸다.
	if (sFullFile->substr(0, 1) == ".") sFullFile->erase(0, 1);
	
	// 앞에 '/'로 시작되는 파일이면 '/'를 떼어낸다.
	if (sFullFile->size() != 1)
		if (sFullFile->substr(0, 1) == "/") sFullFile->erase(0, 1);
	
	// 마지막에 '/' 가 있는지 찾는다. (디렉토리인지 확인)
	tStringSize = sFullFile->find("/", sFullFile->length() - 1);
	
	// Fullname에서 name 구분.
	if (tStringSize != string::npos)
	{
		sTmp = sFullFile->substr(0, sFullFile->length() - 1);
		
		tStringSize_tmp_1 = sTmp.rfind("/");
					
		if (tStringSize_tmp_1 == string::npos)
		{
			*sFilename = *sFullFile;
		}
		else
		{
			*sFilename = sFullFile->substr(	tStringSize_tmp_1 + 1, sFullFile->length() - tStringSize_tmp_1);
		}
		*bDir = true;
	}
	else
	{
		sTmp = *sFullFile;
		tStringSize_tmp_1 = sTmp.rfind("/");
		*sFilename = sFullFile->substr(	tStringSize_tmp_1 + 1, sFullFile->length() - tStringSize_tmp_1);
		*bDir = false;
	}
	return SUCCESS;
}

int Archive::ReadLine_TarGZ( vector<string>&	vLineToken, File* pFileInfo )
{
	string	sFullFilename, sFilename;
	bool	bDir = false;
	// It is not same date and time format every OS.
	// So if you get information from text output, user maybe
	// get incorrect screen.
	//
	// Consider process tar header. It is not real solution.
	// cf) http://www.gnu.org/software/tar/manual/html_node/tar_134.html

	LOG_WRITE("LineFormatRead - TARGZ");
	
	// 파일 정보 채움.
	#ifdef __FreeBSD__
	if (vLineToken.size() >= 7)
	{ 
		pFileInfo->sDate = vLineToken[3] + vLineToken[4];
		pFileInfo->sTime = vLineToken[5].substr(0, 5);
		for (int n = 0; n < vLineToken.size()-7; n++)
			if (n == 0)
				sFullFilename = vLineToken[7];
			else
				sFullFilename = sFullFilename + " " + vLineToken[7+n];
	#else
	if (vLineToken.size() >= 6)
	{ 
		LOG_WRITE("LineFormatRead - TARGZ");
		pFileInfo->sDate = vLineToken[3].substr(2, 8);
		pFileInfo->sTime = vLineToken[4].substr(0, 5);

		string sOwner = vLineToken[1];
		if (sOwner.find("/") != string::npos)
		{
			pFileInfo->sOwner = sOwner.substr(0, sOwner.find("/") - 1);
			pFileInfo->sGroup = sOwner.substr( sOwner.find("/") + 1);
		}

		for (uint n = 0; n < (uint)vLineToken.size()-5; n++)
			if (n == 0)
				sFullFilename = vLineToken[5];
			else
				sFullFilename = sFullFilename + " " + vLineToken[5+n];
	#endif
		pFileInfo->sAttr = vLineToken[0];
		pFileInfo->uSize = atoll(vLineToken[2].c_str());

		if (pFileInfo->sAttr[0] == 'd' || bDir == true)
			pFileInfo->bDir = true;
		else
			pFileInfo->bDir = false;
		
		// Tmp는 나중에 압축 풀때 사용.
		pFileInfo->sTmp = sFullFilename; 

		// '/./' 제거
		Fullname_To_Filename(&sFullFilename, &sFilename, &bDir);
		
		// 디렉토리 이름 끝에 / 가 들어가면 지운다.
		if (pFileInfo->bDir == true)
			if (sFilename.substr(sFilename.length()-1, 1) == "/")
				sFilename.erase(sFilename.length()-1, 1);
		
		// 디렉토리 중에 끝에 '/' 가 없으면 만들어준다.
		if (pFileInfo->bDir == true)
			if (sFullFilename.substr(sFullFilename.length()-1, 1) != "/")
				sFullFilename = sFullFilename + "/";
			
		pFileInfo->sFullName = sFullFilename;
		pFileInfo->sName = sFilename;

		// Link setting
		if (pFileInfo->sAttr[0] == 'l')
		{
			#ifdef __FreeBSD__
			if (vLineToken.size() == 10)
				pFileInfo->sLinkName = vLineToken[9];
			#else
			if (vLineToken.size() == 8)
				pFileInfo->sLinkName = vLineToken[7];
			#endif
		}
	}
	else
		return ERROR;
	return SUCCESS;
}

int Archive::ReadLine_RPM( vector<string>&	vLineToken, File* pFileInfo )
{
	string	sFullFilename, sFilename;
	bool	bDir = false;

	if (vLineToken.size() >= 9)
	{
		// 파일 정보 채움.
		pFileInfo->sAttr = vLineToken[0];
		pFileInfo->uSize = atoll(vLineToken[4].c_str());
		
		pFileInfo->sDate = vLineToken[5] + " " + vLineToken[6];
		pFileInfo->sTime = vLineToken[7].substr(0,5);
	
		if (pFileInfo->sAttr[0] == 'd')
			pFileInfo->bDir = true;
		else
			pFileInfo->bDir = false;

		pFileInfo->sOwner = vLineToken[2];
		pFileInfo->sGroup = vLineToken[3];

		for (uint n = 0; n < (uint)vLineToken.size()-8; n++)
			if (n == 0)
				sFullFilename = vLineToken[8];
			else
				sFullFilename = sFullFilename + " " + vLineToken[8+n];
			
		pFileInfo->sFullName = sFullFilename;
		pFileInfo->sTmp = sFullFilename;
		
		// '/./' 제거
		Fullname_To_Filename(&sFullFilename, &sFilename, &bDir);
		
		if (pFileInfo->bDir == true)
			if (sFilename.substr(sFilename.length()-1, 1) == "/")
				sFilename.erase(sFilename.length()-1, 1);
		
		// 디렉토리 중에 끝에 '/' 가 없으면 만들어준다.
		if (pFileInfo->bDir == true)
			if (sFullFilename.substr(sFullFilename.length()-1, 1) != "/")
				sFullFilename = sFullFilename + "/";

		pFileInfo->sFullName = sFullFilename;
		pFileInfo->sName = sFilename;

		// Link setting
		if (pFileInfo->sAttr[0] == 'l')
			if (vLineToken.size() == 11)
				pFileInfo->sLinkName = vLineToken[10];
	}
	else
		return ERROR;
	return SUCCESS;
}

int Archive::ReadLine_DEB( vector<string>&	vLineToken, File* pFileInfo )
{
	string	sFullFilename, sFilename;
	bool	bDir = false;

	if (vLineToken.size() >= 6)
	{
		// lrwxrwxrwx root/root         0 2005-03-03 20:00:59 ./usr/lib/libXft.so.2 -> libXft.so.2.1.1
		// 파일 정보 채움.
		pFileInfo->sAttr = vLineToken[0];
		pFileInfo->uSize = atoll(vLineToken[2].c_str());
		pFileInfo->sDate = vLineToken[3].substr(2, 8);
		pFileInfo->sTime = vLineToken[4].substr(0, 5);
		
		if (pFileInfo->sAttr[0] == 'd')
			pFileInfo->bDir = true;
		else
			pFileInfo->bDir = false;

		string sOwner = vLineToken[1];
		if (sOwner.find("/") != string::npos)
		{
			pFileInfo->sOwner = sOwner.substr(0, sOwner.find("/") - 1);
			pFileInfo->sGroup = sOwner.substr( sOwner.find("/") + 1);
		}

		for (uint n = 0; n < (uint)vLineToken.size()-5; n++)
			if (n == 0)
				sFullFilename = vLineToken[5+n];
			else
				sFullFilename = sFullFilename + " " + vLineToken[5+n];
			
		pFileInfo->sFullName = sFullFilename;
		pFileInfo->sTmp = sFullFilename;

		// '/./' 제거
		Fullname_To_Filename(&sFullFilename, &sFilename, &bDir);

		// Root 디토리가 있으면 지운다.
		if (sFilename == "/") return ERROR;
		
		if (pFileInfo->bDir == true)
			if (sFilename.substr(sFilename.length()-1, 1) == "/")
				sFilename.erase(sFilename.length()-1, 1);

		pFileInfo->sName = sFilename;
		
		// 디렉토리 중에 끝에 '/' 가 없으면 만들어준다.
		if (pFileInfo->bDir == true)
			if (sFullFilename.substr(sFullFilename.length()-1, 1) != "/")
				sFullFilename = sFullFilename + "/";
		pFileInfo->sFullName = sFullFilename;
		pFileInfo->sName = sFilename;

		// Link Setting	
		if (pFileInfo->sAttr[0] == 'l')
			if (vLineToken.size() == 8)
				pFileInfo->sLinkName = vLineToken[7];
	}
	else
	{
		return ERROR;
	}
	return SUCCESS;
}

int Archive::ReadLine_ZIP( vector<string>&	vLineToken, File* pFileInfo )
{
	string	sFullFilename, sFilename;
	bool	bDir = false;

	if (vLineToken.size() >= 4)
	{
		if (vLineToken[1] == "Date" && vLineToken[2] == "Time") return ERROR;
		if (vLineToken[1] == "----" && vLineToken[2] == "----") return ERROR;

		// 파일 정보 채움.
		pFileInfo->uSize = atoll(vLineToken[0].c_str());
		pFileInfo->sDate = vLineToken[1].substr(0, 8);
		pFileInfo->sTime = vLineToken[1].substr(0, 5);
		
		// '/./' 제거
		for (uint n = 0; n < (uint)vLineToken.size()-3; n++)
			if (n == 0)
				sFullFilename = vLineToken[3];
			else
				sFullFilename = sFullFilename + " " + vLineToken[3+n];

		pFileInfo->sTmp = sFullFilename;
			
		Fullname_To_Filename(&sFullFilename, &sFilename, &bDir);
		
		pFileInfo->bDir = bDir;
		if (pFileInfo->bDir == true)
			if (sFilename.substr(sFilename.length()-1, 1) == "/")
				sFilename.erase(sFilename.length()-1, 1);

		pFileInfo->sName = sFilename;
		pFileInfo->sFullName = sFullFilename;
		
		// zip은 내용을 알수 없음. 나중에 수정할수 있음 수정.
		if (pFileInfo->bDir == true)
			pFileInfo->sAttr = "drwxr-xr-x";
		else
			pFileInfo->sAttr = "-rwxr-xr-x";
	}
	else
	{
		return ERROR;
	}
	return SUCCESS;
}

int Archive::ReadLine_ALZ( vector<string>&	vLineToken, File* pFileInfo )
{
	string	sFullFilename, sFilename;
	bool	bDir = false;
	
	if (vLineToken.size() >= 6)
	{
		if (vLineToken[0].substr(0, 9) == "Copyright") return ERROR;
		if (Tolower(vLineToken[0]) == "date" &&
			Tolower(vLineToken[1]) == "time") return ERROR;
		if (vLineToken[0].substr(0, 4) == "----" &&
			vLineToken[1].substr(0, 4) == "----") return ERROR;

		// 파일 정보 채움.
		if (vLineToken[0].size() > 8)
		{
			vLineToken[0] = Replace(vLineToken[0], ":", "-");
			pFileInfo->sDate = vLineToken[0].substr(2, 8);
		}
		else
		{
			pFileInfo->sDate = vLineToken[0].substr(0, 8);
		}
		pFileInfo->sTime = vLineToken[1].substr(0, 5);
		
		string sFileSize = vLineToken[3];
		for (uint n = 0; n < sFileSize.size(); n++)
			if (isdigit(sFileSize[n]) == 0) return ERROR;
		
		pFileInfo->uSize = atoll(sFileSize.c_str());
		
		for (uint n = 0; n < (uint)vLineToken.size()-5; n++)
			if (n == 0)
				sFullFilename = vLineToken[5];
			else
				sFullFilename = sFullFilename + " " + vLineToken[5+n];

		pFileInfo->sFullName = sFullFilename;
		pFileInfo->sTmp = sFullFilename;
		
		// \ -> /
		sFullFilename = Replace(sFullFilename, "\\", "/");
		Fullname_To_Filename(&sFullFilename, &sFilename, &bDir);

		pFileInfo->bDir = bDir;
		if (pFileInfo->bDir == true)
			if (sFilename.substr(sFilename.length()-1, 1) == "/")
				sFilename.erase(sFilename.length()-1, 1);

		pFileInfo->sName = sFilename;

		// AD_H 형태로 나오는데 디렉토리만 우선 구성.
		if (pFileInfo->bDir == true)
			pFileInfo->sAttr = "drwxr-xr-x";
		else
			pFileInfo->sAttr = "-rwxr-xr-x";
	}
	else
		return ERROR;
	return SUCCESS;
}

int Archive::ReadLine_ISO( vector<string>&	vLineToken, File* pFileInfo )
{
	//	Directory listing of /I386/
	//  d---------   0    0    0          254440 Aug 13 2004 [     27 02]  Test
	//  ----------   0    0    0             843 Aug 29 2001 [   1974 00]  12520437.CP_

	if ( vLineToken.size() < 4 ) return ERROR;
	if ( vLineToken[0] == "isoinfo:" ) return ERROR;

	if ( vLineToken[0] == "Directory" )
	{
		string sTmp = vLineToken[3];
		if ( vLineToken.size() > 4 )
			for ( int n = 4; n < vLineToken.size(); n++ )
				sTmp = sTmp + " " + vLineToken[n];
		_sTmpDir = sTmp;
		return ERROR;
	}
	
	if ( vLineToken.size() < 10 ) return ERROR;
	
	if ( vLineToken[0] == "d---------" )
	{
		pFileInfo->sAttr = "drw-r--r--";
		pFileInfo->bDir = true;
	}
	else if ( vLineToken[0] == "----------" )
	{
		pFileInfo->sAttr = "-rw-r--r--";
	}

	pFileInfo->sOwner = "root";
	pFileInfo->sGroup = "root";
	pFileInfo->uSize = atoll(vLineToken[4].c_str());

	pFileInfo->sDate = vLineToken[5] + " " + vLineToken[7];
	
	pFileInfo->sName = vLineToken[ vLineToken.size() - 1 ];

	if ( !pFileInfo->sName.size() ) return ERROR;

	pFileInfo->sFullName = _sTmpDir + pFileInfo->sName;
	pFileInfo->sTmp = pFileInfo->sFullName;

	if ( pFileInfo->sFullName.substr( 0, 1 ) == "/" ) 
		pFileInfo->sFullName = pFileInfo->sFullName.substr( 1, pFileInfo->sFullName.size()-1 );

	if ( pFileInfo->sName == "." ) return ERROR;
	if ( pFileInfo->sName == "..") return ERROR;

	// / 넣어주지 않으면 이중으로 생김. (tar.gz 과 호환성 맞춤 )
	if ( pFileInfo->bDir ) return ERROR;

	LOG_WRITE("[Name] [%s]", pFileInfo->sFullName.c_str());
	return SUCCESS;
}

int Archive::ReadLine_RAR( vector<string>&	vLineToken, File* pFileInfo )
{
	string	sFullFilename, sFilename;
	bool	bDir = false;

	if ( vLineToken.size() == 0 ) return ERROR;

	// Pathname/Comment
	// Size   Packed Ratio  Date   Time     Attr      CRC   Meth Ver
	
	if ( vLineToken.size() != 9 )
	{
		static bool bStart = false;

		if ( vLineToken[0].substr( 0, 15 ) == "---------------" )
			bStart = true;
		else
		{
			if ( bStart == true )
			{
				string sTmp = vLineToken[0];
				for ( int n = 1; n < vLineToken.size(); n++ )
					sTmp = sTmp + " " + vLineToken[n];
				_sTmpDir = sTmp;
			}
		}
		return ERROR;
	}
	
	if ( vLineToken[0] == "Size" ) return ERROR;
	
	pFileInfo->sDate = vLineToken[3];
	pFileInfo->sTime = vLineToken[4];
	pFileInfo->sAttr = vLineToken[5];

	sFullFilename = _sTmpDir;
	
	Fullname_To_Filename(&sFullFilename, &sFilename, &bDir);
	
	pFileInfo->sFullName = sFullFilename;
	pFileInfo->sTmp = sFullFilename;
	pFileInfo->sName = sFilename;

	pFileInfo->sOwner = "root";
	pFileInfo->sGroup = "root";
	pFileInfo->uSize = atoll(vLineToken[0].c_str());

	pFileInfo->bDir = false;
	if ( pFileInfo->sAttr.size() == 7 && pFileInfo->sAttr[1] == 'D' )
		pFileInfo->bDir = true;
	if ( pFileInfo->sAttr.size() == 10 && pFileInfo->sAttr[0] == 'd' )
		pFileInfo->bDir = true;

	// / 넣어주지 않으면 이중으로 생김. (tar.gz 과 호환성 맞춤 )
	if ( pFileInfo->bDir ) 
		pFileInfo->sFullName = pFileInfo->sFullName + "/";

	LOG_WRITE( "[%s]", pFileInfo->sAttr.c_str() );
	return SUCCESS;
}

///	\brief	압축 확장자 별로 파일리스트를 읽는다.
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::LineFormatRead(vector<string>&	vLineToken, File* pFileInfo, int nFormat)
{
	string	sFullFilename;
	string	sFilename;
	string	sDate;
	bool	bDir = false;

	pFileInfo->sType = "archive";
	
	switch(nFormat)
	{
		case	GZ:
			if (vLineToken.size() >= 4)
			{ 
				if (vLineToken[0] == "compressed" && vLineToken[1] == "uncompressed") return ERROR;
				pFileInfo->sAttr = "-rw-r--r--";
				pFileInfo->uSize = atoll(vLineToken[1].c_str());
				pFileInfo->bDir = false;
				pFileInfo->sDate = "--------";
				pFileInfo->sTime = "--:--";
				pFileInfo->sName = _sFilename.substr(0, _sFilename.rfind("."));
				pFileInfo->sTmp = pFileInfo->sFullName = _sFilename.substr(0, _sFilename.rfind("."));
			}
			break;
		case	TAR_GZ:
		case	TAR:
		case	TAR_BZ:
			if ( ReadLine_TarGZ( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;
		
		case RPM:
			if ( ReadLine_RPM( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;

		case DEB:
			if ( ReadLine_DEB( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;
			
		case ZIP:
			if ( ReadLine_ZIP( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;
		case ALZ:
			if ( ReadLine_ALZ( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;
		case ISO:
			if ( ReadLine_ISO( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;
		case RAR:
			if ( ReadLine_RAR( vLineToken, pFileInfo ) == ERROR ) return ERROR;
			break;
	}
	return SUCCESS;
}

///	\brief	압축 화일의 정보 읽음.
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::FileListRead(void)
{
	string	sCommand;
	
	if (_sFilename.length() == 0) {
		MsgBox("Error", "Error :: filename is NULL");
		return ERROR;
	}

	LOG_WRITE("FileListRead [%s]", _sFilename.c_str());
	
	// 파일 확장자 체크
	if ((_eZipType = GetZipType(_sFilename)) == ERROR) 
	{
		LOG_WRITE("GetZipType Error !!!");
		return ERROR;
	}
	
	if (_eZipType == BZ)
	{
		// .gz, .bz 파일은 하나 밖에 없기 때문에 그 화일 이름을 그대로 사용한다.
		{
			File*	pFile = new File;
			pFile->sAttr = "dr--r--r--";
			pFile->uSize = 0;
			pFile->bDir = true;
			pFile->sDate = "--------";
			pFile->sTime = "--:--";
			pFile->sTmp = pFile->sFullName = "Exit";
			pFile->sName = "..";
			_tFileList.push_back(pFile);
		}

		{
			File*	pFile = new File;
			pFile->sAttr = "-rw-r--r--";
			pFile->uSize = 0;
			pFile->bDir = false;
			pFile->sDate = "--------";
			pFile->sTime = "--:--";
			pFile->sName = ChgCurLocale( _sFilename.substr(0, _sFilename.rfind(".")) );
			//pFile->sTmp = pFile->sFullName = _sFullFilename.substr(0, _sFullFilename.rfind("."));
			pFile->sTmp = pFile->sFullName = _sFilename.substr(0, _sFilename.rfind("."));
			_tFileList.push_back(pFile);
		}
		LOG_WRITE("BZ SUCCESS !!! [%s]", _sFilename.c_str());
		return SUCCESS;
	}

	LOG_WRITE("FileListRead 2 [%s]", _sFilename.c_str());

	// . 압축 파일에 해당하는 명령어를 알아낸다.
	switch(_eZipType)
	{
		case 	GZ:
			sCommand = "gunzip -l " + addslash(_sFullFilename);
			break;
		case	TAR_GZ:
			sCommand = _sTarCommand + " tvfz " + addslash(_sFullFilename);
			break;
		case	TAR_BZ:
			sCommand = _sTarCommand + " tvfj " + addslash(_sFullFilename);
			break;
		case	TAR:
			sCommand = _sTarCommand + " tvf " + addslash(_sFullFilename);
			break;
		case	ZIP:
			sCommand = "unzip -l " + addslash(_sFullFilename);
			break;
		case	RPM:
			sCommand = "rpm2cpio " + addslash(_sFullFilename) + " | cpio -tv ";
			break;
		case	DEB:
			sCommand = "dpkg-deb -c " + addslash(_sFullFilename);
			break;
		case	ALZ:
			sCommand = "export LANG=c; unalz -l " + addslash(_sFullFilename);
			break;
		case	ISO:
			sCommand = "isoinfo -l -i " + addslash(_sFullFilename);
			break;
		case	RAR:
			sCommand = "rar v " + addslash(_sFullFilename);
			break;
		default:
			return ERROR;
	}
	
	LOG_WRITE("COMMAND Data :: [%s]", sCommand.c_str());
	sCommand = sCommand + " 2> /dev/null";
	
	// -rw-r--r-- root/root       730 2004-01-12 23:15:53 project/smstest/smstest.kdevelop.pcs
	//
	// In FreeBSD bsdtar (c locale)
	// -rw-rw-r--  0 500    500      3827 Nov 27 15:13 mls-0.4.5/README
	//
	// In FreeBSD gnu tar (c locale)
	// -rw-rw-r-- 500/500        3827 Nov 27 15:13 2004 mls-0.4.5/README

	if (access(_sFullFilename.c_str(), R_OK) == -1)
	{
		LOG_WRITE("Error", strerror(errno));
		return ERROR;
	}

	_tErrorInfo.clear();
	FileListClear();

	vector<LineArgData> vLineArgData;

	if (CmdShell::CmdExeArg(sCommand, 0, vLineArgData) == ERROR) return ERROR;

	LOG_WRITE("CmdExeArg vLineArgData size [%d] [%d]", vLineArgData.size(), _eZipType);

	File*	pFileInfo = NULL;

	for (uint n = 0; n < vLineArgData.size(); n++)
	{
		pFileInfo = new File;

		string	sLine;
		for (uint c = 0; c < vLineArgData[n].size(); c++)
		{
			vector<string>&	tStr = vLineArgData[n];
			sLine = sLine + "[" + tStr[c] + "]";
		}
		//LOG_WRITE("Cmd %s", sLine.c_str());

		if (LineFormatRead(vLineArgData[n], pFileInfo, _eZipType) == SUCCESS)
		{
			LOG_WRITE( "List %s", sLine.c_str() );
			pFileInfo->sName = ChgCurLocale( pFileInfo->sName );
			_tFileList.push_back(pFileInfo);
		}
		else
		{
			_tErrorInfo.push_back(sLine);
			delete pFileInfo; pFileInfo = NULL;
		}
	}

	if (FileListCheck_DirInsert() == ERROR) return ERROR;
	
	// 파일이 없는 것이다.
	if (_tFileList.size() == 1) return ERROR;
	return SUCCESS;
}

///	\brief	압축 화일에서 없는 디렉토리를 찾아 넣어준다.
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::FileListCheck_DirInsert(void)
{
	File*				pFile  = NULL;
	uint				nCount = 0, nCount_2 = 0;
	
	string				sFullName;
	string				sTmpFullName, sTmpFilename;
	bool 				bTmpDir;
	
	vector<string>		vDir;
	vector<string>		vNullDir;
	
	string::size_type	tSizeType;
	
	// 디렉토리를 우선 찾아서 vDir 넣는다.
	for (nCount = 0; nCount < _tFileList.size(); nCount++)
	{
		pFile = _tFileList[nCount];	
		if (pFile->bDir == true)
		{
			vDir.push_back(pFile->sFullName);
		}
	}
	
	// 파일리스트 중 디렉토리가 없는 것을 찾아 디렉토리를 만들어준다.
	for (nCount = 0; nCount < _tFileList.size(); nCount++)
	{
		pFile = _tFileList[nCount];
		
		// 디렉토리는 검사하지 않는다.
		if (pFile->bDir == true) continue;
		
		sFullName = pFile->sFullName;
		
		tSizeType = sFullName.rfind("/");
		
		// 디렉토리가 없는 것은 검사하지 않음.
		if (tSizeType == string::npos) continue;
		
		do
		{
			sFullName = sFullName.substr(0, tSizeType+1);
			
			if (vDir.size() != 0)
			{
				for (nCount_2 = 0; nCount_2 < vDir.size(); nCount_2++)
				{
					if (vDir[nCount_2] == sFullName) break;
					
					if (nCount_2 == vDir.size() - 1)
					{
						vNullDir.push_back(sFullName);
						vDir.push_back(sFullName);
						break;
					}
				}
			}
			else
			{
				vNullDir.push_back(sFullName);
				vDir.push_back(sFullName);
			}
			
			tSizeType = sFullName.rfind("/", tSizeType-1);
			if (tSizeType == 0) break;
		}
		while(tSizeType != string::npos);
	}
	
	for (nCount_2 = 0; nCount_2 < (uint)vNullDir.size(); nCount_2++)
	{
		pFile = new File;
		pFile->sAttr = "drwxr-xr-x";
		pFile->uSize = 0;
		pFile->bDir = true;
		pFile->sDate = "--------";
		pFile->sTime = "--:--";
		sTmpFullName = vNullDir[nCount_2];
		Fullname_To_Filename(&sTmpFullName, &sTmpFilename, &bTmpDir);

		if ( sTmpFilename.size() > 2 )
			if ( sTmpFilename.substr( sTmpFilename.size()-1, 1 ) == "/" )
				sTmpFilename = sTmpFilename.substr( 0, sTmpFilename.size()-1 );

		pFile->sName = ChgCurLocale( sTmpFilename );
		pFile->sFullName = sTmpFullName;
		pFile->sTmp = sTmpFullName;
		_tFileList.push_back(pFile);
		LOG_WRITE("NULL Dir [%s] [%s] [%s]", vNullDir[nCount_2].c_str(), sTmpFullName.c_str(), sTmpFilename.c_str());
	}

	// 종료를 위한 내용 추가
	pFile = new File;
	pFile->sAttr = "dr--r--r--";
	pFile->uSize = 0;
	pFile->bDir = true;
	pFile->sDate = "--------";
	pFile->sTime = "--:--";
	pFile->sName = "..";
	pFile->sTmp = pFile->sFullName = "Exit";
	_tFileList.push_back(pFile);
	return SUCCESS;
}

///	\brief	디렉토리 별로 파일 정보를 읽어 옴, Return Data :: pRe_Dir
///	\param	s_Dir	directory name
///	\param	pRe_Dir	파일정보 반환값
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::GetDir_Files(const string& s_Dir, vector<File*>* pRe_Dir)
{
	File*	pFile = NULL;
	uint	nCount = 0;
	string::size_type	tSizeType;
	string::size_type	tSizeType_tmp_1;
	string::size_type	tSizeType_tmp_2;
	
	vector<File*>	tDir_Files;
	tDir_Files.clear();
	
	if (s_Dir.length() == 0 || s_Dir == "/")
	{
		for (nCount = 0; nCount < (uint)_tFileList.size(); nCount++)
		{
			pFile = _tFileList[nCount];

			LOG_WRITE("LIST :: [%s]", pFile->sFullName.c_str());
			
			tSizeType = pFile->sFullName.find("/");
			
			if (tSizeType == string::npos)
			{
				tDir_Files.push_back(pFile);
				continue;
			}
			
			tSizeType_tmp_1 = pFile->sFullName.find("/", tSizeType + s_Dir.length() + 1);
			
			if (tSizeType_tmp_1 == string::npos)
			{
				if (pFile->sFullName.substr(pFile->sFullName.length() - 1) == "/")
				{
					tDir_Files.push_back(pFile);
				}
			}
		}
	}
	else
	{
		for (nCount = 0; nCount < (uint)_tFileList.size(); nCount++)
		{
			pFile = _tFileList[nCount];

			tSizeType = pFile->sFullName.find(s_Dir);
			if (tSizeType == string::npos) 	continue;
			
            tSizeType_tmp_1 = pFile->sFullName.find("/", s_Dir.length() + tSizeType);
			
			// 파일 넣기
			if (tSizeType_tmp_1 == string::npos)
			{
				tDir_Files.push_back(pFile);
				continue;
			}
			
			// 디렉토리 넣기
			if (tSizeType_tmp_1 == pFile->sFullName.length() - 1)
			{
				tDir_Files.push_back(pFile);
			}
		}
	}
	
	if (tDir_Files.size() == 0) 
	{
		LOG_WRITE("tDir_files size :: 0 :: ERROR :: %d", _tFileList.size());
		return ERROR;
	}
	
	pRe_Dir->clear();
	*pRe_Dir = tDir_Files;
	return SUCCESS;
}

///	\brief	디렉토리 별로 파일 정보를 읽어 옴, Return Data :: pRe_Dir
///	\param	s_Dir	directory name
///	\param	pRe_Dir	파일정보 반환값
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int
Archive::GetDir_AllFiles(const string& s_Dir, vector<File*>* pRe_Dir)
{	
	File*	pFile = NULL;
	string::size_type	tSizeType;
	pRe_Dir->clear();
	
	for (uint nCount = 0; nCount < (uint)_tFileList.size(); nCount++)
	{
		pFile = _tFileList[nCount];
		tSizeType = pFile->sFullName.find(s_Dir);
		if (tSizeType == string::npos) 	continue;
		pRe_Dir->push_back(pFile);
	}
	return SUCCESS;
}

///	\brief	전체 정보 읽어옴.
///	\param	rRe_File	반환할 파일정보
///	\return	SUCCESS	:	성공
int Archive::GetAll_Files(vector<File*>* rRe_File)
{
	*rRe_File = _tFileList;
	return SUCCESS;
}

///	\brief	압축 풀었을때의 전체 파일 사이즈를 알아낸다.
///	\return	압축 풀었을때의 전체 파일 사이즈
ullong	Archive::GetAllDataSize(void)
{
	ullong	uTotal = 0;
	
	File*	pFile = NULL;
	uint	nCount;
	string::size_type	tSizeType;
	
	for (uint nCount = 0; nCount < (uint)_tFileList.size(); nCount++)
	{
		pFile = _tFileList[nCount];
		uTotal = uTotal + pFile->uSize;
	}
	return uTotal;
}

///	\brief	 화일하나에 대해 Append나 delete를 한다.
///
///	 Append : true, Delete : false
///
///	\param	sFullName	full file name
///	\param	nAppendDel	TAR_DELETE 	:	파일 지움\n
///						TAR_APPEND	:	파일 추가
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int	Archive::FileControl(const string& sFullName, int nAppendDel, const string& sInsertTmpPath)
{
	string	sCommand;
	
	if (_eZipType == ZIP)
	{
		if (IsFileCreate(_sFullFilename) == true)
		{
			if (nAppendDel == TAR_DELETE)
			{
				MsgBox("Error", _sFullFilename +"' file not found... :: zip file delete");
				return ERROR;
			}
		} else {
			if (nAppendDel == TAR_DELETE)
			{
				sCommand = "cd "+_sDir+"; zip -q -Fd " + addslash(_sFullFilename) + " " + sFullName;
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				return SUCCESS;
			}
		}

		if (sInsertTmpPath == "")
			sCommand = "cd "+_sDir+"; zip -q -Fr " + addslash(_sFullFilename) + " " + sFullName;
		else
			sCommand = "cd "+sInsertTmpPath+"; zip -q -Fr " + addslash(_sFullFilename) + " " + addslash(sFullName);
		if (CommandExecute(sCommand) == ERROR) return ERROR;
		return SUCCESS;
	}

	string	sTarFilename = GetTarFileName(_sFullFilename);
	LOG_WRITE("TARFileNAME [%s]", sTarFilename.c_str());
	
	// 우선 파일이 있는지 확인. 없으면 만듦. :: 있으면 true
	if (IsFileCreate(sTarFilename) == true)
	{
		if (nAppendDel == TAR_DELETE)
		{
			MsgBox("Error", "ERROR : '"+ sTarFilename +"' file not found... :: TAR_DELETE");
			return ERROR;
		}

		sCommand = "cd "+_sDir+"; " + _sTarCommand + " cf " + addslash(sTarFilename) + " " + sFullName;
		if (CommandExecute(sCommand) == ERROR) return ERROR;
	}
	else
	{
		switch(nAppendDel)
		{
			case TAR_APPEND:
			{
				if (sInsertTmpPath == "")
					sCommand = "cd "+_sDir+"; " + _sTarCommand + " rf " + addslash(sTarFilename) + " " + sFullName;
				else
					sCommand = "cd "+sInsertTmpPath+"; " + _sTarCommand + " rf " + addslash(sTarFilename) + " " + addslash(sFullName);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
			}
				
			case TAR_DELETE:
				sCommand = "cd "+_sDir+"; " + _sTarCommand + " --delete --file " +
							addslash(sTarFilename) + " " + sFullName;
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
		}
	}
	return SUCCESS;
}

///	\brief	파일을 압축한다.
///
/// File들은 name으로만 판별.
///
///	\param	t_FileList	압축할 파일 리스트 
///	\param	nAppendDel :: TAR_APPEND, TAR_DELETE 가 있음. 
///	\param	nAppendDel	TAR_DELETE 	:	파일 지움\n
///						TAR_APPEND	:	파일 추가
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Compress(vector<File*>& vFileList, int nAppendDel, const string& sInsertTmpPath)
{
	string	sCommand;
	//SetCounter(20);

	if ((_eZipType = GetZipType(_sFilename)) == ERROR) return ERROR;
	
	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다.
		switch(_eZipType)
		{
			case	TAR_GZ:
				sCommand = "cd "+_sDir+"; " + "gunzip " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
			case 	TAR_BZ:
				sCommand = "cd "+_sDir+"; " + "bunzip2 " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
		}
	}
	//SetCounter(50);	
	string::size_type	tSizeType;
	File*	pFile = NULL;
	string 	sName;
	
	for (uint nCount = 0; nCount < (uint)vFileList.size(); nCount++)
	{
		pFile = vFileList[nCount];
		
		if (pFile->sFullName.size() > sInsertTmpPath.size() && 
			pFile->sFullName.substr(0, sInsertTmpPath.size()) == sInsertTmpPath )
			sName = pFile->sFullName.substr( sInsertTmpPath.size() );
		else
			sName = pFile->sFullName;
			
		if (FileControl(sName, nAppendDel, sInsertTmpPath) == ERROR) return ERROR;
	}	
	//SetCounter(70);	
	switch(_eZipType)
	{
		case	TAR_GZ:
			sCommand = "cd " + _sDir + "; " + "gzip " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
		case 	TAR_BZ:
			sCommand = "cd " +_sDir + "; " + "bzip2 " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
	}
	//SetCounter(90);	
	return SUCCESS;
}

///	\brief	File이 아닌 파일이름을 string으로
///	\param	t_FileList	압축할 파일 리스트 
///	\param	nAppendDel	TAR_DELETE 	:	파일 지움\n
///						TAR_APPEND	:	파일 추가
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Compress(vector<string>& vFileNameList, int nAppendDel)
{
	string	sCommand;
	//SetCounter(20);
	
	if ((_eZipType = GetZipType(_sFilename)) == ERROR) return ERROR;
	
	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다. 
		switch(_eZipType)
		{
			case	TAR_GZ:
				sCommand = "cd "+_sDir+"; " + "gunzip " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
			case 	TAR_BZ:
				sCommand = "cd "+_sDir+"; " + "bunzip2 " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
		}
	}

	//SetCounter(50);
	string::size_type	tSizeType;
	string	sFullFileName;
	//SetCounter(70);	
	for (uint nCount = 0; nCount < (uint)vFileNameList.size(); nCount++)
	{
		sFullFileName = vFileNameList[nCount];
		if (FileControl(sFullFileName, nAppendDel) == ERROR) return ERROR;
	}

	switch(_eZipType)
	{
		case	TAR_GZ:
			sCommand = "cd "+_sDir+"; " + "gzip " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
		case 	TAR_BZ:
			sCommand = "cd "+_sDir+"; " + "bzip2 " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
	}
	//SetCounter(90);	
	return SUCCESS;
}

  
///	\brief	파일 하나를 압축한다.
///	\param	pFile		압축할 파일명
///	\param	nAppendDel	TAR_DELETE 	:	파일 지움\n
///						TAR_APPEND	:	파일 추가
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Compress(const File* pFile, int nAppendDel)
{
	string	sCommand;
	
	//SetCounter(20);
	
	if ((_eZipType = GetZipType(_sFilename)) == ERROR) return ERROR;

	string sInTarget = "cd " + _sDir + "; ";
	
	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다.
		switch(_eZipType)
		{
			case	TAR_GZ:
				sCommand = sInTarget + "gunzip " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
			case 	TAR_BZ:
				sCommand = sInTarget + "bunzip2 " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
		}
	}

	//SetCounter(50);
	if (FileControl(pFile->sFullName, nAppendDel) == ERROR) return ERROR;
	//SetCounter(70);
	
	switch(_eZipType)
	{
		case	TAR_GZ:
			sCommand = sInTarget + "gzip " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
		case 	TAR_BZ:
			sCommand = sInTarget + "bzip2 " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
	}
	//SetCounter(90);
	return SUCCESS;
}

///	\brief	directory를 압축한다.
///	\param	s_Dir	압축할 디렉토리
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Compress(const string& sTargetDir)
{
	string	sCommand;

	if (access(sTargetDir.c_str(), R_OK) == -1)
    {
        throw Exception(strerror(errno));
		return ERROR;
    }

	string sInTarget = "cd "+_sDir+"; ";
	
	//SetCounter(20);
	
	if ((_eZipType = GetZipType(_sFilename)) == ERROR) return ERROR;
	
	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다. 
		switch(_eZipType)
		{
			case	TAR_GZ:
				sCommand = sInTarget + "gunzip -c " + _sDir + addslash(_sFullFilename) 
							+ " > " + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
			case 	TAR_BZ:
				sCommand = sInTarget + "bunzip2 " + _sDir + addslash(_sFullFilename);
				if (CommandExecute(sCommand) == ERROR) return ERROR;
				break;
		}
		
		sCommand = sInTarget + _sTarCommand + " rf " + 
					addslash(GetTarFileName(_sFullFilename)) + " " + sTargetDir; // Append Directory
	}
	else
	{
		sCommand = sInTarget + _sTarCommand + " cf " + 
					addslash(GetTarFileName(_sFullFilename)) + " " + sTargetDir; // create
	}
	
	//SetCounter(50);
	if (CommandExecute(sCommand) == ERROR) return ERROR;
	
	//SetCounter(70);
	
	switch(_eZipType)
	{
		case	TAR_GZ:
			sCommand = sInTarget + "gzip " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
		case 	TAR_BZ:
			sCommand = sInTarget + "bzip2 " + addslash(GetTarFileName(_sFullFilename));
			if (CommandExecute(sCommand) == ERROR) return ERROR;
			break;
	}
	
	//SetCounter(90);
	return SUCCESS;
}

///	\brief	압축을 푼다.
///	\param	s_Dir	압축풀 디렉토리
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Uncompress(const string& sTargetDir)
{
	string	sCommand;
	string	sZip;
	
	if (access(sTargetDir.c_str(), W_OK) == -1)
    {
        MsgBox(_("Error"), strerror(errno));
		return ERROR;
    }

	//SetCounter(20);
	if ((_eZipType = GetZipType(_sFullFilename)) == ERROR) return ERROR;

	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다. 
		//SetCounter(50);

		string sInTarget = "cd "+sTargetDir+"; ";
		
		switch(_eZipType)
		{
			case BZ:
				sCommand = sInTarget + "bunzip2 -c " + addslash(_sFullFilename) + " > "
							+ _sFilename.substr(0, _sFilename.rfind("."));
				break;
			case GZ:
				sCommand = sInTarget + "gunzip -c " + addslash(_sFullFilename) + " > "
							+ _sFilename.substr(0, _sFilename.rfind("."));
				break;
			case TAR:
				sCommand = sInTarget + _sTarCommand + " xf " + addslash(_sFullFilename);
							+ " -C " + sTargetDir; // extract Directory
				break;
			case TAR_BZ:
				sCommand = sInTarget + _sTarCommand + " xfj " + addslash(_sFullFilename);
							+ " -C " + sTargetDir; // extract Directory
				 break;
			case TAR_GZ:
				sCommand = sInTarget + _sTarCommand + " xfz " + addslash(_sFullFilename)
							+ " -C " + sTargetDir; // extract Directory
				break;
			case ZIP:
				sCommand = sInTarget + "unzip -o -qq" + sZip + " " + addslash(_sFullFilename)
							+ " -d " + sTargetDir; // extract Directory
				break;
			case RPM:
				sCommand = sInTarget + "rpm2cpio " + addslash(_sFullFilename) +
							" | cpio -iumd --quiet ";  // extract file
				break;
			case DEB:
					//dpkg-deb --fsys-tarfile $archive | tar xOf - $filename ./$filename
				sCommand = sInTarget + "dpkg-deb --fsys-tarfile " + addslash(_sFullFilename) +
								" | tar xf - -C " + sTargetDir;  // extract file
				break;
			case ALZ:
				sCommand = sInTarget + "unalz " + addslash(_sFullFilename); // extract file
				break;
			case RAR:
				sCommand = sInTarget +  "unrar x -y " + addslash(_sFullFilename);
				break;
		}

		//SetCounter(70);
		if (CommandExecute(sCommand) == ERROR) return ERROR;
	}
	else
	{
		MsgBox(_("Error"), _sFullFilename + "file can't not found");
		return ERROR;
	}
	
	//SetCounter(90);
	return SUCCESS;
}

///	\brief	해당되는 파일의 압축을 푼다.
///	\param	t_FileList	압축풀 파일 리스트 
///	\param	sTargetDir	압풀을 폴 장소(Directory)
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Uncompress(const File* pFile, const string& sTargetDir)
{
	string	sCommand = "";
	string	sZip = "";
	
	if (access(sTargetDir.c_str(), W_OK) == -1)
    {
        MsgBox(_("Error"), strerror(errno));
		return ERROR;
    }

	//SetCounter(20);
	
	if ((_eZipType = GetZipType(_sFullFilename)) == ERROR) return ERROR;
	
	if (pFile == NULL) return ERROR;

	string	sFileName = addslash(pFile->sTmp);
	string	sZipFileName = addslash(_sFullFilename);

	LOG_WRITE("Uncompress [%s] [%s] [%s]", _sFullFilename.c_str(), sTargetDir.c_str(), sFileName.c_str());
	
	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다.
		//SetCounter(50);
		
		string::size_type	tSizeType;
		File*	pFile;

		string sInTarget = "cd "+sTargetDir+"; ";
		
		switch(_eZipType)
		{
			case BZ:
				sCommand = sInTarget + "bunzip2 -c " + sZipFileName + " > "
							+ _sFilename.substr(0, _sFilename.rfind("."));
				break;
			case GZ:
				sCommand = sInTarget + "gunzip -c " + sZipFileName + " > "
							+ _sFilename.substr(0, _sFilename.rfind("."));
				break;
			case TAR:
				sCommand = 	sInTarget +  _sTarCommand + " xf " + sZipFileName +
						" -C " + sTargetDir + " " + sFileName; // extract file
				break;
			case TAR_BZ:
				sCommand = 	sInTarget +  _sTarCommand + " xfj " + sZipFileName +
						" -C " + sTargetDir + " " + sFileName; // extract file
				break;
			case TAR_GZ:
			{
				sCommand = 	sInTarget +  _sTarCommand + " xfz " + sZipFileName +
						" -C " + sTargetDir + " " + sFileName; // extract file
				break;
			}
			case ZIP:
				sCommand = 	sInTarget +  "unzip -o -qq " + sZipFileName +
						" " + sFileName + " -d " + sTargetDir;  // extract file
				break;
			case RPM:
				sCommand =  sInTarget +  "rpm2cpio " + sZipFileName +
						" | cpio -iumd --quiet " + sFileName;  // extract file
				break;
			case DEB:
				//dpkg-deb --fsys-tarfile $archive | tar xOf - $filename ./$filename
				sCommand =  sInTarget +  "dpkg-deb --fsys-tarfile " + sZipFileName + 
						" | tar xf - -C " + sTargetDir + " " + sFileName;  // extract file
				break;
			case ALZ:
				sCommand = 	sInTarget +  "unalz " + sZipFileName +
							" \"" + sFileName + "\"";  // extract file
				break;
			case ISO:
				sCommand = sInTarget +  "isoinfo -x \"" + sFileName + "\" -i " + sZipFileName + 
							" > " + addslash(pFile->sName);  // extract file
				break;
			case RAR:
				sCommand = sInTarget +  "unrar x -y " + sZipFileName + " " + sFileName;
				break;
		}

		// 에러무시
		if (CommandExecute(sCommand) == ERROR) return ERROR;
		
		//SetCounter(70);
	}
	else
	{
		MsgBox(_("Error"), sZipFileName + " file can't not found");
		return ERROR;
	}
	//SetCounter(90);
	return SUCCESS;
}

///	\brief	해당되는 파일의 압축을 푼다.
///	\param	t_FileList	압축풀 파일 리스트 
///	\param	sTargetDir	압풀을 폴 장소(Directory)
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Uncompress(vector<MLS::File*>& t_FileList, const string& sTargetDir)
{
	string	sCommand = "";
	string	sZip = "";

	if (access(sTargetDir.c_str(), W_OK) == -1)
    {
        MsgBox(_("Error"), strerror(errno));
		return ERROR;
    }

	//SetCounter(20);
	
	if ((_eZipType = GetZipType(_sFullFilename)) == ERROR) return ERROR;
	
	LOG_WRITE("Uncompress [%s]", sTargetDir.c_str());

	string	sFileName;
	string	sZipFileName = addslash(_sFullFilename);
	
	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다.
		//SetCounter(50);
		
		string::size_type	tSizeType;
		File*	pFile;

		string sInTarget = "cd "+addslash(sTargetDir)+"; ";
		
		for (uint nCount = 0; nCount < (uint)t_FileList.size(); nCount++)
		{
			pFile = t_FileList[nCount];
			sFileName = addslash(pFile->sTmp);
			
			switch(_eZipType)
			{
				case BZ:
					sCommand = sInTarget + "bunzip2 -c " + sZipFileName + " > "
								+ _sFilename.substr(0, _sFilename.rfind("."));
					break;
				case GZ:
					sCommand = sInTarget + "gunzip -c " + sZipFileName + " > "
								+ _sFilename.substr(0, _sFilename.rfind("."));
					break;
				case TAR:
					sCommand = 	sInTarget +  _sTarCommand + " xf " + sZipFileName +
							" -C " + addslash(sTargetDir) + " " + sFileName; // extract file
					break;
				case TAR_BZ:
					sCommand = 	sInTarget +  _sTarCommand + " xfj " + sZipFileName +
							" -C " + addslash(sTargetDir) + " " + sFileName; // extract file
					break;
				case TAR_GZ:
					sCommand = 	sInTarget +  _sTarCommand + " xfz " + sZipFileName +
							" -C " + addslash(sTargetDir) + " " + sFileName; // extract file
					break;
				case ZIP:
					sCommand = 	sInTarget +  "unzip -o -qq " + sZipFileName +
							" " + sFileName + " -d " + addslash(sTargetDir);  // extract file
					break;
				case RPM:
					sCommand =  sInTarget +  "rpm2cpio " + sZipFileName +
							" | cpio -iumd --quiet " + sFileName;  // extract file
					break;
				case DEB:
					//dpkg-deb --fsys-tarfile $archive | tar xOf - $filename ./$filename
					sCommand =  sInTarget +  "dpkg-deb --fsys-tarfile " + sZipFileName + 
							" | tar xf - -C " + addslash(sTargetDir) + " " + sFileName;  // extract file
					break;
				case ALZ:
					sCommand = 	sInTarget +  "unalz " + sZipFileName +
								" \"" + sFileName + "\"";  // extract file
					break;
				case ISO:
					sCommand = sInTarget +  "isoinfo -x \"" + sFileName + "\" -i " + sZipFileName + 
								" > " + addslash(pFile->sName);  // extract file
					break;
				case RAR:
					sCommand = sInTarget +  "unrar x -y " + sZipFileName + " " + sFileName;
					break;
			}

			// 에러무시
			if (CommandExecute(sCommand) == ERROR) continue;
		}
		
		//SetCounter(70);
	}
	else
	{
		MsgBox(_("Error"), sZipFileName + "file can't not found");
		return ERROR;
	}
	//SetCounter(90);
	return SUCCESS;
}

///	\brief	압축을 푼다.
///	\param	t_FileList	File list (String 방식)
///	\param	sTargetDir	압풀을 폴 장소(Directory)
///	\return	SUCCESS	:	성공\n
///			ERROR	:	실패
int Archive::Uncompress(vector<string>& _tFileList, const string& sTargetDir)
{
	string	sCommand;
	string	sZip;
	
	if (access(sTargetDir.c_str(), W_OK) == -1)
    {
		MsgBox(_("Error"), strerror(errno));
		return ERROR;
    }
	
	//SetCounter(20);

	if ((_eZipType = GetZipType(_sFullFilename)) == ERROR) return ERROR;

	// 우선 파일이 있는지 확인. 있으면 압축을 푼다.
	if (IsFileCreate(_sFullFilename) == false)
	{
		// tar.gz 파일은 압축을 풀고 Append나 Delete를 해야 한다. 
		//SetCounter(50);
		
		string::size_type	tSizeType;
		string	sFullFileName;

		string sInTarget = "cd "+addslash(sTargetDir)+"; ";
		
		for (uint nCount = 0; nCount < (uint)_tFileList.size(); nCount++)
		{
			sFullFileName = _tFileList[nCount];

			switch(_eZipType)
			{
				case BZ:
					sCommand = sInTarget + "bunzip2 -c " + addslash(_sFullFilename) + " > "
								+ addslash( _sFilename.substr(0, _sFilename.rfind(".")) );
					break;
				case GZ:
					sCommand = sInTarget + "gunzip -c " + addslash(_sFullFilename) + " > "
								+ addslash( _sFilename.substr(0, _sFilename.rfind(".")) );
					break;
				case TAR:
					sCommand = sInTarget + _sTarCommand + " xfj " + addslash(_sFullFilename) +
								" -C " + addslash(sTargetDir) + " " + addslash( sFullFileName ); // extract file
					break;
				case TAR_BZ:
					sCommand = sInTarget + _sTarCommand + " xfj " + addslash(_sFullFilename) +
								" -C " + addslash(sTargetDir) + " " + addslash(sFullFileName); // extract file
					break;
				case TAR_GZ:
					sCommand = sInTarget + _sTarCommand + " xfz " + addslash(_sFullFilename) +
								" -C " + addslash(sTargetDir) + " " + addslash(sFullFileName); // extract file
					break;
				case ZIP:
					sCommand = sInTarget + "unzip -o -qq " + addslash(_sFullFilename) +
								" " + addslash(sFullFileName) + " -d " + addslash(sTargetDir);  // extract file
					break;
				case RPM:
					sCommand = sInTarget + "rpm2cpio " + addslash(_sFullFilename) +
								" | cpio -iumd --quiet " + addslash(sFullFileName);  // extract file
					break;
				case DEB:
					//dpkg-deb --fsys-tarfile $archive | tar xOf - $filename ./$filename
					sCommand = sInTarget + "dpkg-deb --fsys-tarfile " + addslash(_sFullFilename) +
								" | tar xf - -C " + addslash(sTargetDir) + " " + addslash(sFullFileName);  // extract file
					break;
				case ALZ:
					sCommand = sInTarget + "unalz " + addslash(_sFullFilename) +
								" \"" + sFullFileName + "\"";  // extract file
					break;
				case ISO:
					sCommand = sInTarget +  "isoinfo -x \"" + sFullFileName + "\" -i " + addslash(_sFullFilename) + 
								" > " + addslash(sFullFileName);  // extract file
					break;
				case RAR:
					sCommand = sInTarget +  "unrar x -y " + addslash(_sFullFilename) + " " + addslash(sFullFileName);
					break;
				default:
					LOG_WRITE("ERROR FILEINFO");
					break;
			}
			
			// 에러무시
			if (CommandExecute(sCommand) == ERROR) continue;
		}
		
		//SetCounter(70);
	}
	else
	{
		MsgBox(_("Error"), _sFullFilename + "file can't not found");
		return ERROR;
	}
	
	//SetCounter(90);
	return SUCCESS;
}

string Archive::DetectTarCommand() const
{
	// In FreeBSD, there is bsd tar(bsdtar) and gnu tar(gtar).
	// Default is bsdtar.
	// Need consider m_TarDir ?

	#ifndef TARNAME
		FILE* const tarVersionCommand = popen("tar --version", "r");
		if(tarVersionCommand != NULL) {
	
			char tarVersion[7];
			memset(tarVersion, 0, 7);
	
			rewind(tarVersionCommand);
			if(fgets(tarVersion, sizeof(tarVersion), tarVersionCommand) != NULL) {
	
				if(strncmp(tarVersion, "bsdtar", sizeof(tarVersion)) == 0) {
					pclose(tarVersionCommand);
					// Need check existance of gtar ?
					return "gtar";
				}
	
			}
			pclose(tarVersionCommand);
	
			return "tar";
	
		} else
			return "tar";
	#else
		return TARNAME;
	#endif
}

