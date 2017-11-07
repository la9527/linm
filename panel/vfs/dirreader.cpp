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

///	\file	dirreader.cpp
///	\brief	dirreader.cpp
#include "dirreader.h"
#include "sortfile.h"
#include "mlslocale.h"

#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

using namespace MLSUTIL;
using namespace MLS;

///	\brief	생성자
DirReader::DirReader()
{
	_pHandle = NULL;
	_bMsgShow = false;
	_sReaderName = "file";
	_sInitTypeName = "file://";
}

DirReader::~DirReader()
{
	Destroy();
}

///	\brief	제거 함수
void DirReader::Destroy()
{
	// . directory 닫기
	if (_pHandle) closedir(_pHandle); _pHandle = NULL;
}


///	\brief	현재 directory가 '/'폴더인가를 조사
///	\return	true	:	'/' directory임\n
///			false	:	'/' directory가 아님.
bool DirReader::isRoot()
{
	if  (_sCurPath == "/") return true;
	return false;
}

string	GetCurrentPath(void)
{
	string 	sPath;
	
	// . 현재 dir name을 구한다.
	char *tmpdir  = getcwd(NULL, 0);
	if (tmpdir)
	{
		sPath = tmpdir;
		free(tmpdir);
			
		// . 끝에 '/'를 추가한다.
		if (sPath != "/") sPath += '/';
	}
	else
	{
		// 현재 디렉토리 읽기 오류면 home 디렉토리 지정
		struct passwd *pw = getpwuid(getuid()); // getuid이냐 geteuid냐...;
		sPath = sPath + pw->pw_dir + '/';
	}
	return sPath;
}

string	DirReader::GetUserName(uid_t		tUidNum)
{
	struct passwd *pw = getpwuid( tUidNum );
	string	sUserName;

	if ( pw )
	{
		sUserName = pw->pw_name;
	}
	else
		sUserName = itoa((int)tUidNum);

	
	return sUserName;
}

string	DirReader::GetGroupName(gid_t		tGidNum)
{
	struct group *grp = getgrgid( tGidNum );
	string	sGroupName;

	if ( grp )
		sGroupName = grp->gr_name;
	else
		sGroupName = itoa((int)tGidNum);
	return sGroupName;
}

/// \brief	상대경로는 절대경로로 바꾼다.
/// \param	str		상대경로 path명
///	\return	절대경로명
string DirReader::GetRealPath(const string& str) const
{
	string sPath = str;
	
	// ~ 인지 먼저 판단.
	if (sPath[0]=='~')
	{			
		if (sPath == "~" || sPath[1] == '/')
		{
			// . home 디렉토리 지정
			struct passwd *pw = getpwuid(getuid()); // getuid이냐 geteuid냐...;
			string sHome;
			if ( pw )
				sHome  = pw->pw_dir;
			sHome += '/';
			string sTmp1 = sPath.substr(1);
			string sTmp2 = "";
			if (sTmp1.find('/') == string::npos)
				sTmp2 = sTmp1.substr(sTmp1.find('/')+1);
			sPath = sHome + sTmp2;
		}
		else
		{	
			string sTmp1 = sPath.substr(1);
			string sTmp2, sTmp3;
			if (sTmp1.find('/') == string::npos)
				sTmp2 = sTmp1.substr(0, sTmp1.find('/'));
			else
				sTmp2 = sTmp1;
			if (sTmp1.find('/') == string::npos)
				sTmp3 = sTmp1.substr(sTmp1.find('/')+1);
			struct passwd *pw = getpwnam(sTmp2.c_str());
			if (pw == NULL) return "";
			string sHome = pw->pw_dir;
			sHome += '/';
			sPath = sHome + sTmp3;
		}
	}
	else if (sPath[0]!='/') // .. /(root) dir이 아니면
	{
		// ... 현재 dir를 얻어야 한다면 현재 dir절대경로를 찾는다. 
		if (sPath == ".") sPath = _sCurPath.empty() ? GetCurrentPath() : _sCurPath;
		// ... 상위 dir를 얻어야 한다면
		else if (sPath == "..")
		{
			// .... /(root)dir이 아니면 상위 절대경로 dir명을 구함
			if (_sCurPath != "/")
			{
				string::size_type p = _sCurPath.rfind('/', _sCurPath.size()-2);
				sPath = _sCurPath.substr(0, p+1);
			}			
			/// .... /(root)dir이면 절대경로에 / 를 넣음
			else sPath = _sCurPath;
		}
		else // ... 그외 dir은 path에 추가하여 절대경로를 구한다.
		{
			sPath = _sCurPath + sPath;
		}
	}
	if (sPath.substr(sPath.size()-1, 1) != "/") sPath += '/';
	return sPath;
}


///	\brief	Read 함수
///	\param	path	읽어들일 path 절대경로
///	\return	초기화 성공 여부
bool DirReader::Read(const string &sPath)
{
	string sRealPath;
	if (sPath == "") 
		sRealPath = GetRealPath("/");
	else
		sRealPath = GetRealPath(sPath);

	// access 에 R_OK 도 들어가야 제대로 Mls 가 작동 된다.
	if (access(sRealPath.c_str(), R_OK | X_OK) == -1)
	{
		if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
		return false;
	}

	DIR*		_pHandle2 = _pHandle;
	if ((_pHandle = opendir(sRealPath.c_str()))==NULL)
	{
		if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
		_pHandle = _pHandle2;
		return false;
	}
	if (_pHandle2) closedir(_pHandle2); _pHandle2 = NULL;

	if (chdir(sRealPath.c_str()) == -1)
	{
		if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
		return false;
	}
	
	_sCurPath = sRealPath;
	return true;
}

///	\brief	다음 파일이나 directory로 이동한다.
///	\return	true	:	성공적으로 이동했음.\n
///			false	:	이동하는것을 실패했음.
bool DirReader::Next()
{
	if (!_pHandle) return false;	
	if ((_pEntry = readdir(_pHandle)) == NULL) return false;
	return true;
}

///	\brief	현재 파일정보를 얻음
///	\param	file	파일정보 반환값
///	\return	현재 파일정보얻기 성공여부
bool DirReader::GetInfo(File &tFile)
{
	bool bLink = false;		// link 파일여부
	int  n;
	
	// . 파일정보를 얻는다. 
	if (lstat(_pEntry->d_name, &_StatBuf)==-1)
	{
		if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
		return false;
	}
	
	char	cLinkPath[FILENAME_MAX];
	memset(&cLinkPath, 0, sizeof(cLinkPath));

	// . 파일 크기를 알아냄
	tFile.uSize = _StatBuf.st_size;
	
	// . 파일이 link 파일이면
	if (S_ISLNK(_StatBuf.st_mode))
	{	
		if ((n = readlink(_pEntry->d_name, cLinkPath, FILENAME_MAX)) == -1)
		{
			if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
			return false;
		}

		cLinkPath[n] = '\0';
		
		// .. 링크 대상인 진짜 파일 정보를 얻는다. 
		// 링크 파일을 찾지 못하면 링크 정보로 사용.
		if (lstat(cLinkPath, &_StatBuf) == -1)
			if (lstat(_pEntry->d_name, &_StatBuf)==-1)
				if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
		
		bLink = true;
	}
	else
		bLink = false;

	// 파일 이름 알아냄
	tFile.sType = _sInitTypeName;
	tFile.bLink = bLink;

	if ( bLink )
		tFile.sLinkName = cLinkPath;
	
	tFile.sName = ChgCurLocale( _pEntry->d_name );
	LOG(" DirReader::GetInfo [%s] [%s]", tFile.sName.c_str(), _pEntry->d_name );
	
	if (tFile.sName == "." || tFile.sName == "..")
		tFile.sFullName = GetRealPath(_pEntry->d_name);
	else
		tFile.sFullName = _sCurPath + _pEntry->d_name;

	// . directory 여부를 알아냄
	if (S_ISDIR(_StatBuf.st_mode))
	{
		tFile.bDir = true;
		
		if (tFile.sFullName.substr(tFile.sFullName.size()-1, 1) != "/") 
			tFile.sFullName += '/';
	}
	else
		tFile.bDir = false;

	// . tFile attribute(drwxrwxrwx)를 설정함	===========
	// tFile S_ISUID, S_ISGID, S_ISVTX 추가
	// zip과 맞추기 위해서 '.' 에서 '-' 로 바꿈.

	char	cAttr[11];
	if (bLink)
		cAttr[0]= 'l';
	else 
		cAttr[0]= tFile.bDir ? 'd' : '-';
		
	cAttr[1]= S_IRUSR & _StatBuf.st_mode ? 'r' : '-';
	cAttr[2]= S_IWUSR & _StatBuf.st_mode ? 'w' : '-';
	cAttr[3]= S_IXUSR & _StatBuf.st_mode ? 'x' : '-';
	if (S_ISUID & _StatBuf.st_mode) cAttr[3] = 'S';
	if (S_ISUID & _StatBuf.st_mode &&
		S_IXUSR & _StatBuf.st_mode) cAttr[3] = 's';
	cAttr[4]= S_IRGRP & _StatBuf.st_mode ? 'r' : '-';
	cAttr[5]= S_IWGRP & _StatBuf.st_mode ? 'w' : '-';
	cAttr[6]= S_IXGRP & _StatBuf.st_mode ? 'x' : '-';
	if (S_ISGID & _StatBuf.st_mode) cAttr[6] = 'S';
	if (S_ISGID & _StatBuf.st_mode &&
		S_IXGRP & _StatBuf.st_mode) cAttr[6] = 's';
	cAttr[7]= S_IROTH & _StatBuf.st_mode ? 'r' : '-';
	cAttr[8]= S_IWOTH & _StatBuf.st_mode ? 'w' : '-';
	cAttr[9]= S_IXOTH & _StatBuf.st_mode ? 'x' : '-';
	if (S_ISVTX & _StatBuf.st_mode) cAttr[9] = 'S';
	if (S_ISVTX & _StatBuf.st_mode &&
		S_IXUSR & _StatBuf.st_mode) cAttr[9] = 's';
	cAttr[10] = 0;
	tFile.sAttr = cAttr;
	// ====================================================
	
	// . 파일 생성 날짜를 알아냄
	char cDate[20];
	memset(&cDate, 0, sizeof(cDate));
	strftime(cDate, 9, "%y-%m-%d", localtime(&_StatBuf.st_mtime));
	tFile.tCreateTime = _StatBuf.st_mtime;
	tFile.sDate = cDate;

	// . 파일 생성 시간을 알아냄
	memset(&cDate, 0, sizeof(cDate));
	strftime(cDate, 6, "%H:%M", localtime(&_StatBuf.st_mtime));
	tFile.sTime = cDate;

	tFile.sOwner = GetUserName( _StatBuf.st_uid );
	tFile.sGroup = GetGroupName( _StatBuf.st_gid );

	return true;
}

bool	DirReader::Copy(Selection&	tSelection, const string& sTargetPath, Selection*	pSelection)
{
	string			sCurName, sTargetPathTmp, sTargetPathTmp2;
	string			sSourceName, sTargetName;
	vector<File*>	vFiles;

	ullong	uFileSize = tSelection.CalcSize();
	
	string 	sSize = toregular(uFileSize), sRestSize;

	bool	bSkipAll = false, bOverwriteAll = false;
	bool	bReturn = true, bRenameChk = false;
	ullong	uLastSize = 0, uViewCount=1, uPsize=0, uCsize = 0;
	
	struct stat src_stat, tar_stat;

	char buf[65536];

	vFiles = tSelection.GetData();

	if (sTargetPath.size() == 0)
		sTargetPathTmp2 = _sCurPath;
	else
		sTargetPathTmp2 = sTargetPath;

	CommonProgress	tProgress(	_("Copy"), (_("To ") + sTargetPathTmp2).c_str(), true);
	
	if (pSelection != NULL)
	{
		pSelection->Clear();
		pSelection->SetSelectPath(sTargetPathTmp2);
	}

	string sSourcePath = tSelection.GetSelectPath();

	String	sCount, sCount2;
	
	LOG("Copy sTargetPath [%s] [%s] [%s]", sTargetPath.c_str(), _sCurPath.c_str(), sTargetPathTmp2.c_str());

	tProgress.Start();

	LOG("Test tProgress Exit code [%d]", tProgress.isExit() ? 1 : 0 );

	// 파일 복사
	for (int n=0; n<(int)vFiles.size(); n++)
	{
		File*	pFile = vFiles[n];
		bool	bOverwrite = false;
		
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		if (tProgress.isExit())
		{
			tProgress.End();
			if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
				goto halt;
			tProgress.Start();
		}

		// 소스 이름과 타겟 이름 정하기	
		sSourceName = pFile->sFullName;
		sTargetName = sTargetPathTmp2 + pFile->sFullName.substr(sSourcePath.size());
		sTargetName = ChgCurLocale( sTargetName ); // EUCKR, UTF8 상관 없이 파일 적용 복사

		if (stat(sSourceName.c_str(), &src_stat)==-1)
		{
			// 소스가 없다니.. 참
			tProgress.End();
			String	sStr;
			sStr.Append(_("Source is not found : %s"), sSourceName.c_str());
			MsgBox(_("Error"), sStr);
			break;
		}

		LOG("Copy sTargetName 1 [%s] [%s] [%s]", sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str());
		
		mode_t permission = src_stat.st_mode;

		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), toregular(vFiles.size()).c_str());
		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr(sCount.c_str());

		if (stat(sTargetName.c_str(), &tar_stat)!=-1)
		{
			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain:
				vector<string> q;
				q.push_back(_("Overwrite"));
				q.push_back(_("Skip"));
				q.push_back(_("Rename"));
				q.push_back(_("Overwrite All"));
				q.push_back(_("Skip All"));
				q.push_back(_("Cancel"));

				int 	nSelect = 0;
				tProgress.End();
				nSelect = SelectBox((_("File exists : ") + pFile->sName).c_str(), q, 0);
				tProgress.Start();

				LOG("Selection [%d]", nSelect);
				
				switch(nSelect)
				{
					case 0:	// overwrite
						LOG("OverWrite");
						bOverwrite = true;
						break;
					
					case 1: // skip
						uPsize += src_stat.st_size;
						uViewCount++;
						continue;
						
					case 2: // rename 
					{
						if ( !pFile->bDir )
						{
							struct stat statbuf;
							string sRename = pFile->sName;
							
							while(1)
							{
								tProgress.End();
								if (InputBox(	_("Rename"),
												sRename) < 0) goto askagain;
								tProgress.Start();
								sTargetName = sTargetPathTmp2 + sRename;
															
								if (lstat(sTargetName.c_str(), &statbuf) != -1)
								{
									tProgress.End();
									MsgBox(	_("Error"), _("File exists"));
									tProgress.Start();
									continue;
								}
								bRenameChk = true;
								break;
							}
						}
						else
						{
							tProgress.End();
							MsgBox(	_("Error"), _("Directory can't rename."));
							tProgress.Start();
						}
						break;
					}
					
					case 3: // overwriteall
						bOverwriteAll = true;
						break;
						
					case 4: // skipall
						bSkipAll = true;
						uPsize += src_stat.st_size;
						uViewCount++;
						continue;
					case -1:
					case 5: // cancel
						bReturn = false;
						goto halt;
							
					default:
						break;
				}
			}
		}

		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir && !pFile->bLink)
		{
			if (pSelection != NULL)
			{
				File	tTargetFile = *pFile;
				tTargetFile.sFullName = sTargetName;
				pSelection->Add(&tTargetFile);
			}

			if (mkdir(sTargetName.c_str(), src_stat.st_mode)==-1) // 디렉토리를 만든다.
			{	
				// 실패하면
				if ( !bOverwriteAll && !bSkipAll && !bOverwrite)
				{
					tProgress.End();
					MsgBox(_("Error"), _("Directory Making error"));
					break;
				}
			}
			continue;
		}

		if (pFile->bLink)
		{
			if (pSelection != NULL)
			{
				File	tTargetFile = *pFile;
				tTargetFile.sFullName = sTargetName;
				pSelection->Add(&tTargetFile);
			}

			string sLinkName;
			if (pFile->sLinkName.find("/") == string::npos)
			{
				sLinkName = pFile->sFullName.substr(0, 
							pFile->sFullName.size() - pFile->sName.size() - 1) 
							+ pFile->sLinkName;
			}
			else
				sLinkName = pFile->sLinkName;

			if (sTargetName.substr(sTargetName.size()-1, 1) == "/")
				sTargetName = sTargetName.substr(0, sTargetName.size()-1);

			if (symlink(  sLinkName.c_str(), sTargetName.c_str() ) == -1)
			{
				tProgress.End();
				String	sStr;
				sStr.Append(_("file link copy failure : %s (%s) : %s. continue ? "),
						sLinkName.c_str(),
						sTargetName.c_str(),
						strerror(errno));

				MsgBox(_("Error"), sStr.c_str());
				break;
			}
			continue;
		}

		uCsize=0;	
		// 실제 복사 오퍼레이션	
		
		uViewCount++;
		
		if (!bRenameChk && src_stat.st_ino == tar_stat.st_ino) continue;

		FILE*	fp = fopen(sSourceName.c_str(), "rb");
		FILE*	out = fopen(sTargetName.c_str(), "wb");
		
		if (fp && out)
		{
			while( !feof(fp) )
			{
				if (tProgress.isExit())
				{
					tProgress.End();
					if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
					{
						remove( sTargetName.c_str() );
						bReturn = false;
						goto halt;
					}
					tProgress.Start();
				}
				
				uLastSize = fread(buf, 1, sizeof(buf), fp);
				fwrite(buf, 1, uLastSize, out);

				uCsize +=uLastSize;
				uPsize +=uLastSize;
				
				sRestSize = toregular(uCsize);

				sCount2.Printf("%s/%s", sRestSize.c_str(), sSize.c_str());
				tProgress.setRightStr2(sCount2.c_str());
				
				if (src_stat.st_size && uCsize && uPsize && uFileSize)
					tProgress.setCount(uCsize * 100 / src_stat.st_size, uPsize * 100 / uFileSize);
			}
		}
		else
		{
			tProgress.End();
			if (YNBox(_("Error"), _("File access error, continue?"), false) == false) 
			{
				bReturn = false;
				break;
			}
			tProgress.Start();
			continue;
		}
		
		fclose(out);
		fclose(fp);
		
		chmod(sTargetName.c_str(), permission);

		if (pSelection != NULL)
		{
			File	tTargetFile = *pFile;
			tTargetFile.sFullName = sTargetName;
			pSelection->Add(&tTargetFile);
		}
	}
halt:
	tProgress.End();
	if (pSelection)
		pSelection->SetSelectPath(sTargetPathTmp2);

	LOG("DirReader Copy End");
	return bReturn;
}

bool	DirReader::Move(Selection&	tSelection, const string& sTargetPath, Selection* pSelection)
{
	// 1. 선택한 것을 하는 것인가..
	// 2. 그냥 한 파일만 이동하는 것인가
	
	bool	bOverwriteAll = false;
	bool	bFilesCopyRemove = false;
	
	ullong	uFileSize = tSelection.CalcSize();
	uint	uSize = tSelection.GetSize();

	string 	sSize = toregular(uFileSize), sRestSize;

	struct 	stat src_stat, tar_stat;
		
	//ostringstream count;
	ullong uViewCount=1, uPsize=0, uCsize;	

	string sCurName;
	string sSourceName;
	string sTargetName;
	string sTargetPathTmp;

	vector<File*>	vFiles;
	vFiles = tSelection.GetData();

	if (sTargetPath.size() == 0)
		sTargetPathTmp = _sCurPath;
	else
		sTargetPathTmp = sTargetPath;

	CommonProgress	tProgress(	_("Move"), (_("To ") + sTargetPathTmp).c_str(), true);
	
	tProgress.setLeftStr(_("Calculating file sizes..."));
	tProgress.Start();

	string sSourcePath = tSelection.GetSelectPath();

	if (pSelection != NULL)
	{
		pSelection->Clear();
		pSelection->SetSelectPath(sTargetPathTmp);
	}

	vector<File *>::iterator begin = vFiles.begin(), end = vFiles.end();
	sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());
	
	// 파일 이동
	for (int t=0; t< (int)vFiles.size(); t++)
	{
		File*	pFile = vFiles[t];

		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		// 하부 파일들은 이동하지 않는다.
		string sTmpName = pFile->sFullName.substr(sSourcePath.size());
		string::size_type p = sTmpName.find("/");
		if (p != string::npos) 
			if (p != sTmpName.length() - 1)
				continue;
		
		// 소스 이름과 타겟 이름 정하기	옮기기
		sCurName = pFile->sName;
		sSourceName = pFile->sFullName;
		sTargetName = sTargetPathTmp + pFile->sFullName.substr(sSourcePath.size());
		sTargetName = ChgCurLocale( sTargetName ); // EUCKR, UTF8 상관 없이 파일 적용 복사
		LOG("sSourceName [%d] [%s] [%s]", t, sSourceName.c_str(), sTargetName.c_str());

		if (stat(sSourceName.c_str(), &src_stat)==-1)
		{
			tProgress.End();
			// 소스가 없다니.. 참
			String	sStr;
			sStr.Append("%s is not found. continue ?", sSourceName.c_str());
			if (YNBox(_("Error"), sStr.c_str(), false)==true)
			{
				tProgress.Start();
				continue;
			}
			tProgress.End();
			break;
		}

		// 타겟이 이미 있는지 확인
		if (!bOverwriteAll && stat(sTargetName.c_str(), &tar_stat) != -1)
		{// 만약에 타겟이 있다면..
			vector<string> q;
			q.push_back(_("Overwrite"));
			q.push_back(_("Skip"));
			q.push_back(_("Overwrite All"));
			q.push_back(_("Cancel"));
			tProgress.End();
			int x = SelectBox((_("File exists : ") + sCurName).c_str(), q, 0);
			tProgress.Start();
			
			switch(x)
			{
				case -1:
				case 0:
					break;
				
				case 1:
					uCsize += src_stat.st_size;
					uViewCount++;
					continue;
				
				case 2:
					bOverwriteAll = true;
					break;
						
				case 3:
					goto halt;
						
				default:
					break;
			}
		}

		String	sCount;
		sCount.Append("%s/%s", toregular(uViewCount).c_str(), toregular(uSize).c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr(sCount.c_str());
		if (uPsize && uSize)
			tProgress.setCount( 0, uPsize * 100 / uSize );
		
		int err = rename(sSourceName.c_str(), sTargetName.c_str());

		if (err == -1 && errno == EXDEV)
		{
			bFilesCopyRemove = true;
			break;
		}
		else
		{
			uPsize += src_stat.st_size;
			if (uPsize && uSize)
				tProgress.setCount( 100 , uPsize * 100 / uSize );
		}
		
		if (pSelection != NULL)
		{
			File	tTargetFile = *pFile;
			tTargetFile.sFullName = sTargetName;
			pSelection->Add(&tTargetFile);
		}
	}

	if (bFilesCopyRemove)
	{
		tProgress.End();

		// EXDEV에 대해서 복사/삭제 오퍼레이션으로 전환.
		if ( Copy(tSelection, sTargetPath) )
			Remove(tSelection, true, true);
	}

halt:
	tProgress.End();
	if (pSelection) 
		pSelection->SetSelectPath(sTargetPathTmp);
	return true;
}

bool	DirReader::Remove(Selection&	tSelection, bool bMsgShow, bool bIgnore)
{
	CommonProgress tProgress(_("Delete"), "");

	if (bMsgShow && !bIgnore)
	{
		if (YNBox(_("Info"), _("Delete ?"), false) == false) return 0;
		
		tProgress.setLeftStr(_("Counting files..."));
	}

	tProgress.Start();

	uint	uSize = tSelection.GetSize();
	string	sTargetName;
	
	vector<string> dir_stack;	

	vector<File*>	vFiles;
	vFiles = tSelection.GetData();
	vector<File*>	vLink, vFile, vDirs;
	
	// 파일 이동
	for (int t=0; t< (int)vFiles.size(); t++)
	{
		File*	pFile = vFiles[t];
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		if (!pFile->bDir || pFile->bLink)
			vFile.push_back(pFile);
		else
			vDirs.push_back(pFile);
	}

	String	sStr;

	int 	nCount = 0;
	sStr.Printf("%s/%s", toregular(nCount).c_str(), toregular(uSize).c_str());

	if (bMsgShow)
	{
		tProgress.setRightStr(sStr.c_str());
		if (nCount && uSize)
			tProgress.setCount( nCount * 100 / uSize );
	}

	// 파일 긴것부터 삭제. 
	vector<File *>::iterator   	begin = vFile.begin(),	end = vFile.end();
	sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());
	begin = vDirs.begin();	end = vDirs.end();
	sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());

	int nRt = 0;

	for (int n=0; n < (int)vFile.size(); n++)
	{
		File*	pFile = vFile[n];
		sTargetName = pFile->sFullName;
		LOG("File Remove [%s]", sTargetName.c_str());

		if ( pFile->bLink )
		{
			if (sTargetName.substr(sTargetName.size()-1, 1) == "/")
				sTargetName = sTargetName.substr(0, sTargetName.size()-1);
			nRt = unlink(sTargetName.c_str());
		}
		else
			nRt = remove(sTargetName.c_str());
		
		if ( nRt == -1)
		{
			if (!bIgnore)
			{
				String	sStr;
				
				if (!pFile->bLink)
				{
					sStr.Append(_("file remove failure : %s : %s. continue ? "),
							pFile->sName.c_str(),
							strerror(errno));
				}
				else
				{
					sStr.Append(_("link file remove failure : %s : %s. continue ? "),
							pFile->sName.c_str(),
							strerror(errno));	
				}
	
				tProgress.End();
				if (YNBox(_("Error"), sStr.c_str(), false)==true)
					tProgress.Start();
				else
				{
					tProgress.End();
					return false;
				}
			}
		}
		nCount++;
		if (bMsgShow)
		{
			sStr.Printf("%s/%s", toregular(nCount).c_str(), toregular(uSize).c_str());
			tProgress.setRightStr(pFile->sName.c_str());
		
			if (nCount && uSize)
				tProgress.setCount( nCount * 100 / uSize );
		}
	}

	for (int n=0; n < (int)vDirs.size(); n++)
	{
		File*	pFile = vDirs[n];
		sTargetName = pFile->sFullName;
		LOG("Dir Remove [%s]", sTargetName.c_str());
		if (rmdir(sTargetName.c_str())==-1)
		{
			if (!bIgnore)
			{
				String	sStr;
				sStr.Append(_("dir remove failure : %s : %s. continue ? "),
						sTargetName.c_str(),
						strerror(errno));
				tProgress.End();
				if (YNBox(_("Error"), sStr.c_str(), false)==true)
					tProgress.Start();
				else
				{
					tProgress.End();
					return false;
				}
				break;
			}
		}

		nCount++;
		sStr.Printf("%s/%s", toregular(nCount).c_str(), toregular(uSize).c_str());
		if (bMsgShow)
		{
			tProgress.setRightStr(sTargetName.c_str());
			if (nCount && uSize)
				tProgress.setCount((int)(((double)nCount/(double)uSize)*100.0));
		}
	}
	tProgress.End();
	return true;
}

