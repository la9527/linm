#include "ftpreader.h"
#include "cmdshell.h"
#include "sortfile.h"
#include "mlslocale.h"

using namespace MLSUTIL;
using namespace MLS;

FtpReader::FtpReader()
{
	_pDefaultFtpNet = NULL;
	_sReaderName = "ftp";
	_bConnected = false;
	_eEncode = AUTO;
}

FtpReader::~FtpReader()
{
}

int		FtpReader::GetIpUserPw(const string& sStr, 
								string& sIP, 
								string& sUser, 
								string& sPasswd,
								string& sDir)
{
	string sStrTmpDir = sStr;
	if (sStrTmpDir != "")
	{
		// la9527:1234@kldp.net:20
		string::size_type p2 = sStrTmpDir.find("/");
		string sStrTmp = sStrTmpDir;
		
		if (p2 != string::npos)
		{
			sStrTmp = sStrTmpDir.substr(0, p2);
			sDir = sStrTmpDir.substr(p2);
		}
		
		string::size_type p = sStrTmp.rfind("@");

		if (p != string::npos) 
		{
			string				sTmp = sStrTmp.substr(0, p);
			string::size_type 	pPasswd = sTmp.find(":");
			
			sIP = sStrTmp.substr(p+1);
			
			if (pPasswd != string::npos)
			{
				sUser = sTmp.substr(0, pPasswd);
				sPasswd = sTmp.substr(pPasswd+1, p-pPasswd-1);
			}
			else
			{
				sUser = sTmp.substr(0, p);
			}
		}
		else
		{
			sIP = sStrTmpDir;
		}
	}
	return SUCCESS;
}

int		FtpReader::LineFormatRead(vector<string>&	vLineToken, File* pFileInfo)
{
	// -rw-------   1 root  root       4223 Aug 14  2004 .test
	// -rwxr-xr-x   1 test  users       116 Jan  3  2005 Test
	// drwxr-xr-x   2 test  users      4096 May  4 11:50 Test1
	pFileInfo->sAttr = vLineToken[0];

	if(vLineToken.size() > 11 || vLineToken.size() < 9) return ERROR;
	
	if (pFileInfo->sAttr[0] == 'd' || pFileInfo->sAttr[0] == '-')
	{
		pFileInfo->sType = _sInitTypeName;
		pFileInfo->uSize = atoll(vLineToken[4].c_str());
		pFileInfo->sOwner = vLineToken[2];
		pFileInfo->sGroup = vLineToken[3];
		pFileInfo->sDate = vLineToken[5] + " " + vLineToken[6];
		pFileInfo->sTime = vLineToken[7];
		if (pFileInfo->sAttr[0] == 'd')
			pFileInfo->bDir = true;
		else
			pFileInfo->bDir = false;

		pFileInfo->sFullName = _sCurPath + vLineToken[8];
		
		if ( pFileInfo->bDir )
			pFileInfo->sFullName += "/";
		pFileInfo->sName = ChgCurLocale( vLineToken[8] );
				
		// Link Setting	
		if (pFileInfo->sAttr[0] == 'l')
			if (vLineToken.size() == 11)
			{
				pFileInfo->sLinkName = vLineToken[10];
			}
	}
	else
		return ERROR;
	return SUCCESS;
}

bool	FtpReader::Init(const string& sInitFile)
{
	string 	sIP, sUser, sPasswd, sDir;
	// sInitFile : username:passwd@IP
	// example   : annonymous:123123@name.com
	string sConnectionInfo = sInitFile;

	while(1)
	{
		GetIpUserPw(sConnectionInfo, sIP, sUser, sPasswd, sDir);
		if (sIP == "" && sUser == "" && sPasswd == "")
		{	
			if (InputBox(	_("Input ftp connect url (user:pswd@hostname/dir)"),
							sConnectionInfo) == ERROR) return false;
		}
		else
			break;
	}
	
	if (sUser == "" && sPasswd == "")
	{
		String sMsg;
		sMsg.Append("Ftp Connect Input username - [%s]", sIP.c_str());
		if (InputBox(sMsg.c_str(), sUser) == ERROR) return false;

		sMsg.clear();
		sMsg.Append("Ftp Connect Input passwd - [%s@%s]", sUser.c_str(), sIP.c_str());
		if (InputBox(sMsg.c_str(), sPasswd, true) == ERROR) return false;
	}
	else if (sPasswd == "")
	{
		String sMsg;
		sMsg.Append("Ftp Connect Input passwd - [%s@%s]", sUser.c_str(), sIP.c_str());
		if (InputBox(sMsg.c_str(), sPasswd, true) == ERROR) return false;
	}

	void*	pWait = MsgWaitBox(_("Ftp connect"), _("Please wait !!!"));

	if (FtpConnect(sIP.c_str(), &_pDefaultFtpNet) == 0)
	{
		String	sConnErrMsg;
		sConnErrMsg.Append("ftp connect fail !!! - %s", sIP.c_str());
		MsgWaitEnd(pWait);
		MsgBox(_("Error"), sConnErrMsg);
		return false;
	}

	if (FtpLogin( sUser.c_str(), sPasswd.c_str(), _pDefaultFtpNet) == 0)
	{
		String	sLoginErrMsg;
		sLoginErrMsg.Append("ftp login fail !!! - %s", sUser.c_str());
		MsgWaitEnd(pWait);
		MsgBox(_("Error"), sLoginErrMsg);
		Destroy();
		return false;
	}

	MsgWaitEnd(pWait);

	string sPath = GetPwd();
	if (sPath == "")
	{
		MsgBox(_("Error"), "get current dir reading failure !!!");
		Destroy();
		return false;
	}
	
	if (sDir.size() == 0)
		_sCurPath = _sHome = sPath;
	else
		_sCurPath = GetRealPath( sDir );
	_sInitTypeName = "ftp://" + sUser + "@" + sIP;
	_sIP = sIP; 
	_sUser = sUser;

	LOG_WRITE("Ftp Connected [%s] [%s]", _sInitTypeName.c_str(), _sCurPath.c_str());

	if (Read(_sCurPath) == false)
	{
		Destroy();
		return false;
	}
	_bConnected = true;
	_sInitFile = sInitFile;
	return true;
}

void	FtpReader::Destroy()
{		
	_sInitTypeName = "";
	_sCurPath = "";
	FtpQuit(_pDefaultFtpNet);
	_pDefaultFtpNet = NULL;

	_sIP = ""; _sUser = "";
	_sHome = "";
	_bConnected = false;
}

string	FtpReader::GetPwd() const
{
	string 	sPathTmp;
	char	cPath[1024];
	memset(&cPath, 0, 1024);

	if (FtpPwd(cPath, 1024, _pDefaultFtpNet) == 0)
	{
		return sPathTmp;
	}
	sPathTmp = sPathTmp + cPath + "/";
	return sPathTmp;
}

/// \brief	상대경로는 절대경로로 바꾼다.
/// \param	str		상대경로 path명
///	\return	절대경로명
string FtpReader::GetRealPath(const string& str) const
{
	string sPath = str;
	
	// ~ 인지 먼저 판단.
	if (sPath[0]=='~')
	{			
		if (sPath == "~" || sPath[1] == '/')
		{
			// . home 디렉토리 지정
			string sTmp1 = sPath.substr(1);
			string sTmp2;
			if (sTmp1.find('/') != string::npos)
				sTmp2 = sTmp1.substr(sTmp1.find('/')+1);
			sPath = _sHome + sTmp2;
		}
		else
		{	
			sPath = str;
		}
	}
	else if (sPath[0]!='/') // .. /(root) dir이 아니면
	{
		// ... 현재 dir를 얻어야 한다면 현재 dir절대경로를 찾는다. 
		if (sPath == ".") sPath = _sCurPath.empty() ? GetPwd() : _sCurPath;
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
	if (sPath.substr(sPath.length()-1, 1) != "/") sPath += '/';
	return sPath;
}

bool	FtpReader::Read(const string &sPath)
{
	if (_pDefaultFtpNet == NULL) return false;

	string	sPathReal;
	if (sPath == "")
		sPathReal = _sCurPath;
	else
		sPathReal = GetRealPath(sPath);

	int nRt = 0;

	if ((nRt = FtpChdir(sPathReal.c_str(), _pDefaultFtpNet)) <= 0)
	{
		String sMsgStr;
		sMsgStr.Append("access error - %s", sPathReal.c_str());
		MsgBox(_("Error"), sMsgStr.c_str());
		if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
		{
			Destroy();
			Init(_sInitFile);
		}
		return false;
	}
	
	string sListFp = "/tmp/" + _sUser + "@" + _sIP;
	LOG_WRITE("Read List File [%s] [%s]", sListFp.c_str(), sPathReal.c_str());

	if ((nRt = FtpDir(sListFp.c_str(), sPathReal.c_str(), _pDefaultFtpNet)) == 0)
	{
		String sMsgStr;
		sMsgStr.Append("access error - %s", sPathReal.c_str());
		MsgBox(_("Error"), sMsgStr.c_str());
		if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
		{
			Destroy();
			Init(_sInitFile);
		}
		return false;
	}

	string sBefPath = _sCurPath;
	_sCurPath = sPathReal;
	
	vector<LineArgData> vLineArgData;
	if (CmdShell::CmdExeArg(sListFp, 0, vLineArgData, true) == ERROR)
	{
		_sCurPath = sBefPath;
		return false;
	}

	string sDelFile = "rm -rf /tmp/" + addslash(sListFp) + " > /dev/null 2> /dev/null";
	system( sDelFile.c_str() );

	LOG_WRITE("CmdExeArg vLineArgData size [%d]", vLineArgData.size());

	vector<File*>	tTmpList;

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
		LOG_WRITE("Data [%s]", sLine.c_str());
		if (LineFormatRead(vLineArgData[n], pFileInfo) == SUCCESS)
			tTmpList.push_back(pFileInfo);
		else
		{
			delete pFileInfo; pFileInfo = NULL;
		}
	}

	for (uint n = 0; n < _vFileLists.size(); n++)
	{
		File* pFile = _vFileLists[n];
		delete pFile;
	}
	_vFileLists.clear();

	if (sPathReal != "/")
	{
		File*	pFile = new File;
		pFile->sType = _sInitTypeName;
		pFile->sAttr = "drwxr--r--";
		pFile->uSize = 0;
		pFile->bDir = true;
		pFile->sDate = "--------";
		pFile->sTime = "--:--";
		pFile->sFullName =  GetRealPath("..");
		pFile->sName = "..";
		tTmpList.push_back(pFile);
	}
	_vFileLists = tTmpList;
	_uCur = 0;
	return true;
}

bool	FtpReader::Next()
{
	if (_uCur < _vFileLists.size())
	{
		_uCur++;
		return true;
	}
	return false;
}

bool FtpReader::GetInfo(File &tFile)
{
	File*	pFile = NULL;

	// . 현재 선택된 파일이 유효하면
	if (_uCur-1 < _vFileLists.size())
	{
		// .. filelist에서 파일 정보를 가져옴
		pFile = _vFileLists[_uCur-1];
		tFile.Clear();

		if (pFile->sName == "." ) return false;
		tFile = *pFile;
	}
	else 
		return false;
	return true;
}

bool	FtpReader::Rename(File* pFile, const string& sRename)
{
	if (pFile == NULL) 
	{
		LOG_WRITE("Rename pFile is NULL !!!");
		return false;
	}

	string sRenameName = pFile->sName;
	if (sRenameName == "..") return false;

	if (sRename == "")
	{
		if (InputBox(_("Rename"), sRenameName) == ERROR) 
			return false;
	}
	else
		sRenameName = sRename;
	
	sRenameName = _sCurPath + sRenameName;

	int nRt = 0;
	if ((nRt = FtpRename(pFile->sFullName.c_str(), sRenameName.c_str(), _pDefaultFtpNet)) == 0)
	{
		String sMsg;
		sMsg.Append("Rename failure !!! - %s %s", 
					pFile->sName.c_str(), FtpLastResponse(_pDefaultFtpNet) );
		MsgBox(	_("Error"), sMsg.c_str());
		if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
		{
			Destroy();
			Init(_sInitFile);
		}
		return false;
	}
	return true;
}

bool	FtpReader::Mkdir(const string& sFullPathName)
{
	string sMkdirName;
	int	nRt = 0;

	if (sFullPathName == "")
	{
		if (InputBox(_("Make Directory"), sMkdirName) == ERROR)
			return false;
	}
	else
		sMkdirName = sFullPathName;
	
	sMkdirName = _sCurPath + sMkdirName;
	if ((nRt = FtpMkdir(sMkdirName.c_str(), _pDefaultFtpNet)) == 0)
	{
		String sMsg;
		sMsg.Append("Mkdir failure !!! - %s %s", sMkdirName.c_str(),  FtpLastResponse(_pDefaultFtpNet));
		MsgBox(	_("Error"), sMsg.c_str());
		if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
		{
			Destroy();
			Init(_sInitFile);
		}
		return false;
	}
	return true;
}

///	\brief	파일을 보기 위한..
///	\param	tFile	볼 파일.
///	\return	파일을 tmp 에 복사 해놓고 파일 위치 리턴
bool  FtpReader::View(const File* pFileOriginal, File* pFileChange)
{ 
	string		sSourceName, sTargetName;
	char		buf[8192];
	netbuf*		pFtpNet;
	int			nRt = 0;

	// 파일 이름이 없을경우 continue
	if (!pFileOriginal) return false;

	// 소스 이름과 타겟 이름 정하기	
	sSourceName = pFileOriginal->sFullName;
	sTargetName = _sTmpDir + ChgCurLocale(pFileOriginal->sName);

	LOG_WRITE("SFtpReader::View [%s] [%s]", sSourceName.c_str(), sTargetName.c_str());
	
	if ((nRt = FtpAccess(	sSourceName.c_str(),
					FTPLIB_FILE_READ, 
					FTPLIB_IMAGE, 
					_pDefaultFtpNet, 
					&pFtpNet)) <= 0)
	{
		String sMsg(_("File access error (%s) !!!"), pFileOriginal->sName.c_str());
		if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
		{
			sMsg.Append("- %s",  FtpLastResponse(_pDefaultFtpNet));
			MsgBox(_("Error"), sMsg.c_str());
			Destroy();
			Init(_sInitFile);
		}
		else
		{
			MsgBox(_("Error"), sMsg.c_str());
		}
		return false;
	}

	ullong	uFileSize = pFileOriginal->uSize;
	string 	sRestSize;

	ullong	uLastSize = 0, uCsize = 0;

	CommonProgress tProgress(_("ftp view file"), "");

	tProgress.setLeftStr(pFileOriginal->sName.c_str());
	tProgress.Start();

	FILE*	out = fopen(sTargetName.c_str(), "wb");

	String	sCount;
	
	if ( out )
	{
		for(;;)
		{
			if (tProgress.isExit())
			{
				tProgress.End();
				if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
				{
					fclose(out);
					FtpClose(pFtpNet);
					remove( sTargetName.c_str() );
					return false;
				}
				tProgress.Start();
			}
			
			if (pFileOriginal->uSize <= uCsize ) break;
	
    		uLastSize = FtpRead(buf, sizeof(buf), pFtpNet);
			if (uLastSize == 0) break;

			fwrite(buf, 1, uLastSize, out);

			uCsize +=uLastSize;
			
			sCount.Printf(	"%s/%s", 
							toregular(uCsize).c_str(), 
							toregular(pFileOriginal->uSize).c_str());

			tProgress.setRightStr(sCount.c_str());
			tProgress.setCount( uCsize * 100 / uFileSize );
		}
	}
	else
	{
		tProgress.End();
		MsgBox(_("Error"), _("local file access error !!!"));
		FtpClose(pFtpNet);
		return false;
	}

	fclose(out);
	FtpClose(pFtpNet);

	tProgress.End();
	*pFileChange = *pFileOriginal;
	pFileChange->sFullName = sTargetName;
	pFileChange->sTmp2 = pFileOriginal->sFullName;
	pFileChange->sName = pFileOriginal->sName;	
	return true;
}

///	\brief	파일 복사
///	\return	복사 여부	
bool	FtpReader::Copy(	Selection& tSelection, 
							const string& sTargetPath,
							Selection*	pSelection)
{
	string			sCurName, sTargetPathTmp, sTargetPathTmp2;
	string			sSourceName, sTargetName;
	vector<File*>	vFiles;

	ullong	uFileSize = tSelection.CalcSize();
	uint	uSize = tSelection.GetSize();
	
	string 	sSize = toregular(uSize), sRestSize;

	bool	bSkipAll = false, bOverwriteAll = false;
	ullong	uLastSize = 0, uViewCount=1, uPsize=0, uCsize = 0;
	
	struct stat src_stat, tar_stat, dir_stat;

	char		buf[8192];	// reading byte 
	netbuf*		pFtpNet;

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
	
	LOG_WRITE("Copy sTargetPath [%s] [%s] [%s]", sTargetPath.c_str(), _sCurPath.c_str(), sTargetPathTmp2.c_str());
	tProgress.Start();

	// 파일 복사
	for (uint n=0; n<vFiles.size(); n++)
	{
		File*	pFile = vFiles[n];
		
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		// 소스 이름과 타겟 이름 정하기	
		sSourceName = pFile->sFullName;
		sTargetName = 	sTargetPathTmp2 + 
						ChgCurLocale( pFile->sFullName.substr(sSourcePath.size()) );

		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir)
		{
			if (pSelection != NULL)
			{
				File	tTargetFile = *pFile;
				tTargetFile.sFullName = sTargetName;
				pSelection->Add(&tTargetFile);
			}

			if (stat(sTargetName.c_str(), &dir_stat) != -1)
			{
				tProgress.End();
				if (YNBox(_("Error"), _("Directory exists, continue?"), false) == false) break;
				tProgress.Start();
				continue;
			}
			else if (mkdir(sTargetName.c_str(), 0755)==-1) // 디렉토리를 만든다.
			{// 실패하면
				tProgress.End();
				MsgBox(_("Error"), _("Directory Making error"));
				tProgress.Start();
				break;
			}
			continue;
		}

		if (!FtpAccess(	sSourceName.c_str(), 
						FTPLIB_FILE_READ, 
						FTPLIB_IMAGE, 
						_pDefaultFtpNet, 
						&pFtpNet))
		{
			tProgress.End();
			if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
			{
				String sMsg("%s",  FtpLastResponse(_pDefaultFtpNet));
				MsgBox(_("Error"), sMsg.c_str());
				Destroy();
				Init(_sInitFile);
				return false;
			}
			
			String sMsg(_("File access error (%s), continue? !!!"), pFile->sName.c_str());
			if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
			tProgress.Start();
			continue;
		}

		LOG_WRITE("FtpReader Copy sTargetName 1 [%s] [%s] [%s] [%d]", 
					sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str(), pFile->uSize);
		
		String	sCount, sCount2;
		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), sSize.c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr2(sCount.c_str());
		
		if (stat(sTargetName.c_str(), &tar_stat)!=-1)
		{
			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain_ftp_copy:
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

				LOG_WRITE("Selection [%d]", nSelect);
				
				switch(nSelect)
				{
					case 0:	// overwrite
						LOG_WRITE("OverWrite");
						break;
					
					case 1: // skip
						uPsize += pFile->uSize;
						uViewCount++;
						FtpClose(pFtpNet);
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
												sRename) < 0) goto askagain_ftp_copy;
								tProgress.Start();
								sTargetName = sTargetPathTmp2 + sRename;
															
								if (lstat(sTargetName.c_str(), &statbuf) != -1)
								{
									tProgress.End();
									MsgBox(	_("Error"), _("File exists"));
									tProgress.Start();
									continue;
								}
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
						uPsize += pFile->uSize;
						uViewCount++;
						FtpClose(pFtpNet);
						continue;
					
					case -1:
					case 5: // cancel
						FtpClose(pFtpNet);
						goto halt_ftp_copy;
							
					default:
						break;
				}
			}
		}
		
		uCsize=0;
		// 실제 복사 오퍼레이션	
		
		uViewCount++;
		
		FILE*	out = fopen(sTargetName.c_str(), "wb");
		
		if ( out )
		{
			for(;;)
			{
				if (tProgress.isExit())
				{
					tProgress.End();
					if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
					{
						fclose(out);
						FtpClose(pFtpNet);
						remove(sTargetName.c_str());
						goto halt_ftp_copy;
					}
					tProgress.Start();
				}	
				
				if (pFile->uSize <= uCsize ) break;
			
				uLastSize = FtpRead(buf, sizeof(buf), pFtpNet);
				if (uLastSize == 0) break;
				
				fwrite(buf, 1, uLastSize, out);
	
				uCsize +=uLastSize;
				uPsize +=uLastSize;

				sCount2.Printf("%s/%s", toregular(uCsize).c_str(), 
										toregular(pFile->uSize).c_str());
				tProgress.setRightStr(sCount2.c_str());

				if ( pFile->uSize && uCsize && uPsize && uFileSize )
					tProgress.setCount(	uCsize * 100 / pFile->uSize,
										uPsize * 100 / uFileSize );
			}
		}
		else
		{
			tProgress.End();
			if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
			{
				String sMsg("%s",  FtpLastResponse(_pDefaultFtpNet));
				MsgBox(_("Error"), sMsg.c_str());
				Destroy();
				Init(_sInitFile);
				return false;
			}
			if (YNBox(_("Error"), _("local file access error, continue?"), false) == false) break;
			tProgress.Start();
			continue;
		}

		fclose(out);

		chmod(sTargetName.c_str(), 0755);
		FtpClose(pFtpNet);
		
		if (pSelection != NULL)
		{
			File	tTargetFile = *pFile;
			tTargetFile.sFullName = sTargetName;
			pSelection->Add(&tTargetFile);
		}
	}

halt_ftp_copy:
	tProgress.End();
	if (pSelection)
		pSelection->SetSelectPath(sTargetPathTmp2);

	LOG_WRITE("FtpReader Copy End");
	return true;
}

///	\brief	파일 삭제
///	\return	삭제 여부
bool	FtpReader::Remove(Selection&	tSelection, bool bMsgShow, bool bIgnore)
{
	CommonProgress tProgress(_("Delete"), "");

	if (bMsgShow)
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
	vector<File*>	vFile, vDirs;
	
	// 파일 이동
	for (int t=0; t< (int)vFiles.size(); t++)
	{
		File*	pFile = vFiles[t];
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;
		if (!pFile->bDir)
			vFile.push_back(pFile);
		else
			vDirs.push_back(pFile);
	}

	String	sStr;

	int 	nCount = 0;
	if (bMsgShow)
	{
		sStr.Printf("<%d/%d>", nCount, uSize);
		tProgress.setRightStr(sStr.c_str());
		if (nCount && uSize)
			tProgress.setCount((int)((long)(nCount/uSize)/100.0));
	}
	
	// 파일 긴것부터 삭제. 
	vector<File *>::iterator   	begin = vFile.begin(),	end = vFile.end();
	sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());
	begin = vDirs.begin();	end = vDirs.end();
	sort(begin, end, sort_dir_adaptor<sort_fullname_length, sort_fullname_length>());

	for (uint n=0; n < vFile.size(); n++)
	{
		File*	pFile = vFile[n];
		sTargetName = pFile->sFullName;
		LOG_WRITE("File Remove [%s]", sTargetName.c_str());

		if (FtpDelete(sTargetName.c_str(), _pDefaultFtpNet) == 0)
		{
			tProgress.End();
			if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
			{
				MsgBox(_("Error"),  FtpLastResponse(_pDefaultFtpNet));
				Destroy();
				Init(_sInitFile);
				return false;
			}

			String	sStr;
			sStr.Append(_("ftp file remove failure : %s : continue ? "), pFile->sName.c_str());
			if (YNBox(_("Error"), sStr.c_str(), false)==true)
			{
				tProgress.Start();
				continue;
			}
			return false;
			break;
		}
		nCount++;
		if (bMsgShow)
		{
			sStr.Printf("<%d/%d>", nCount, uSize);
			tProgress.setRightStr(pFile->sName.c_str());
			if (nCount && uSize)
				tProgress.setCount( nCount * 100 / uSize );
		}
	}

	for (uint n=0; n < vDirs.size(); n++)
	{
		File*	pFile = vDirs[n];
		sTargetName = pFile->sFullName;
		LOG_WRITE("Dir Remove [%s]", sTargetName.c_str());
		if (FtpRmdir(sTargetName.c_str(), _pDefaultFtpNet) == 0)
		{
			tProgress.End();
			if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
			{
				MsgBox(_("Error"),  FtpLastResponse(_pDefaultFtpNet));
				Destroy();
				Init(_sInitFile);
				return false;
			}

			String	sStr;
			sStr.Append(_("ftp dir remove failure %s : continue ?"), 
					sTargetName.c_str());
			
			if (YNBox(_("Error"), sStr.c_str(), false)==true)
			{
				tProgress.Start();
				continue;
			}
			return false;
			break;
		}

		nCount++;
		if (bMsgShow)
		{
			sStr.Printf("<%d/%d>", nCount, uSize);
			tProgress.setRightStr(sTargetName.c_str());
			if (nCount && uSize)
			{
				tProgress.setCount((int)(((double)nCount/(double)uSize)*100.0));
			}
		}
	}
	tProgress.End();
	return true;
}

bool	FtpReader::Paste(Selection& tSelection)
{
	string			sCurName, sTargetPathTmp, sTargetPathTmp2;
	string			sSourceName, sTargetName;
	vector<File*>	vFiles;
	struct stat 	src_stat;

	char	buf[8192];

	ullong	uFileSize = tSelection.CalcSize();
	uint	uSize = tSelection.GetSize();
	
	string 	sSize = toregular(uSize), sRestSize;

	bool	bSkipAll = false, bOverwriteAll = false;
	ullong	uLastSize = 0, uViewCount=1, uPsize=0, uCsize = 0;

	vFiles = tSelection.GetData();
	sTargetPathTmp2 = _sCurPath;
	
	CommonProgress	tProgress(	_("Copy"), (_("To ") + sTargetPathTmp2).c_str(), true);

	string sSourcePath = tSelection.GetSelectPath();
	
	LOG_WRITE("Copy sTargetPath [%s] [%s]", _sCurPath.c_str(), sTargetPathTmp2.c_str());

	if ( vFiles.size() == 0 ) return false;

	EncodeChk(vFiles, false);
	tProgress.Start();

	// 파일 디렉토리 먼저 생성 복사
	for (uint n=0; n<vFiles.size(); n++)
	{
		File*	pFile = vFiles[n];
		
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;
		// 소스 이름과 타겟 이름 정하기	
		sSourceName = pFile->sFullName;
		sTargetName = sTargetPathTmp2 + pFile->sFullName.substr(sSourcePath.size());
		sTargetName = KorCodeChg(sTargetName, _eEncode);
		
		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir)
		{
			if (FtpMkdir(sTargetName.c_str(), _pDefaultFtpNet) == 0)
			{
				// 이미 디렉토리가 있다면
				tProgress.End();
				if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
				{
					MsgBox(_("Error"),  FtpLastResponse(_pDefaultFtpNet));
					Destroy();
					Init(_sInitFile);
					return false;
				}
				if (YNBox(_("Error"), _("Directory exists, continue?"), false) == false) break;
				tProgress.Start();
				continue;
			}			
			continue;
		}
	}

	for (uint n=0; n<vFiles.size(); n++)
	{
		File*	pFile = vFiles[n];
		
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		// 소스 이름과 타겟 이름 정하기	
		sSourceName = pFile->sFullName;
		sTargetName = sTargetPathTmp2 + pFile->sFullName.substr(sSourcePath.size());
		sTargetName = KorCodeChg(sTargetName, _eEncode);

		LOG_WRITE("SFtpReader::Paste [%s] [%s]", sSourceName.c_str(), sTargetName.c_str());

		if (stat(sSourceName.c_str(), &src_stat)==-1)
		{
			// 소스가 없다니.. 참
			String	sStr;
			sStr.Append(_("Source is not found : %s"), sSourceName.c_str());
			tProgress.End();
			MsgBox(_("Error"), sStr);
			tProgress.Start();
			break;
		}

		LOG_WRITE("SFtpReader Paste sTargetName 1 [%s] [%s] [%s]", sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str());
		
		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir) continue;

		String	sCount, sCount2;
		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), sSize.c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr2(sCount.c_str());

		int 	nSize;
		if (FtpSize(sTargetName.c_str(), &nSize, FTPLIB_IMAGE, _pDefaultFtpNet) == 0) // fail
		{
			if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
			{
				MsgBox(_("Error"),  FtpLastResponse(_pDefaultFtpNet));
				Destroy();
				Init(_sInitFile);
				return false;
			}

			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain_ftp_paste:
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

				LOG_WRITE("Selection [%d]", nSelect);
				
				switch(nSelect)
				{
					case 0:	// overwrite
						LOG_WRITE("OverWrite");
						break;
					
					case 1: // skip
						uPsize += src_stat.st_size;
						uViewCount++;
						continue;
						
					case 2: // rename 
					{
						if ( !pFile->bDir )
						{	
							string sRename = pFile->sName;
							
							while(1)
							{
								tProgress.End();
								if (InputBox(	_("Rename"), sRename) < 0) goto askagain_ftp_paste;
								tProgress.Start();
								sTargetName = sTargetPathTmp2 + sRename;
	
								if (FtpRename(	pFile->sFullName.c_str(), 
												sTargetName.c_str(), 
												_pDefaultFtpNet) == 0)
								{
									String sMsg;
									sMsg.Append("Rename failure !!! - %s", pFile->sName.c_str());
									tProgress.End();
									MsgBox(	_("Error"), sMsg.c_str());
									tProgress.Start();
									continue;
								}
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
						goto halt_ftp_paste;
							
					default:
						break;
				}
			}
		}
		
		uCsize=0;	
		// 실제 복사 오퍼레이션	
		
		uViewCount++;

		FILE*	fp = fopen(sSourceName.c_str(), "rb");

		if ( fp )
		{
			netbuf*		pFtpNet;
			if (!FtpAccess(	sTargetName.c_str(), 
							FTPLIB_FILE_WRITE, 
							FTPLIB_IMAGE, 
							_pDefaultFtpNet, 
							&pFtpNet))
			{
				if (FtpGetSocketID( _pDefaultFtpNet ) == 0)
				{
					MsgBox(_("Error"),  FtpLastResponse(_pDefaultFtpNet));
					Destroy();
					Init(_sInitFile);
					return false;
				}

				String sMsg(_("File access error (%s), continue? !!!"), pFile->sName.c_str());
				tProgress.End();
				MsgBox(_("Error"), sMsg.c_str());
				tProgress.Start();
				return false;
			}

			while( !feof(fp) )
			{
				if (tProgress.isExit())
				{
					tProgress.End();
					if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
					{
						FtpClose(pFtpNet);
						fclose(fp);
						FtpDelete(sTargetName.c_str(), _pDefaultFtpNet);
						goto halt_ftp_paste;
					}
					tProgress.Start();
				}				
				
				uLastSize = fread(buf, 1, sizeof(buf), fp);
				if (FtpWrite(buf, uLastSize, pFtpNet) != uLastSize)
				{
					tProgress.End();
					if (YNBox(_("Error"), _("file writing failure. continue?"), false)==true)
					{
						FtpClose(pFtpNet);
						fclose(fp);
						goto halt_ftp_paste;
					}
					tProgress.Start();
					break;
				}

				uCsize +=uLastSize;
				uPsize +=uLastSize;

				sCount2.Printf("%s/%s", 	toregular(uPsize).c_str(),
											toregular(uFileSize).c_str());
				
				tProgress.setRightStr(sCount2.c_str());
				if (src_stat.st_size && uCsize && uPsize && uFileSize)
					tProgress.setCount(	uCsize * 100 / src_stat.st_size,
										uPsize * 100 / uFileSize );
			}

			FtpClose(pFtpNet);
		}
		else
		{
			tProgress.End();
			if (YNBox(_("Error"), _("File access error, continue?"), false) == false) break;
			tProgress.Start();
			continue;
		}

		fclose(fp);
	}
halt_ftp_paste:
	tProgress.End();
	return true;
}

void	FtpReader::EncodeChk(vector<File*>& tFileList, bool bEncChk)
{
	ENCODING	eEncode = US;
	bool 		bChgEncode = false;
	if ( tFileList.size() == 0 ) bChgEncode = true;

	for ( int n = 0; n < (int)tFileList.size(); n++ )
	{
		isKorCode(tFileList[n]->sFullName,	&eEncode);
		if ( eEncode != US )
		{
			bChgEncode = true;
			break;
		}
	}
	
	if ( bChgEncode || bEncChk )
	{
		vector<string>	vStr;
		vStr.push_back(_("LocalEncode"));
		vStr.push_back(_("EUC-KR"));
		vStr.push_back(_("UTF-8"));
		vStr.push_back(_("Cancel"));
				
		int nSelect = SelectBox(_("Remote filename encode select"), vStr, 0);
		if (nSelect == -1) return;
		switch( nSelect )
		{
			case 1:
				_eEncode = KO_EUCKR;
				break;
			case 2:
				_eEncode = KO_UTF8;
				break;
		}
	}
}
