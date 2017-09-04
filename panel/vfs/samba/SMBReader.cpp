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

#include "exception.h"
#include "strutil.h"
#include "mlsdialog.h"
#include "ftplib.h"
#include "selection.h"
#include "sortfile.h"
#include "mlscfgload.h"
#include "mlslocale.h"
#include "libsmbclient.h"
#include "SMBReader.h"

using namespace MLS;
using namespace MLSUTIL;

static string		g_strUser;
static string		g_strPasswd;

static void	No_AuthDataFn(	const char * pServer,
							const char * pShare,
							char * pWorkgroup,
							int maxLenWorkgroup,
							char * pUsername,
							int maxLenUsername,
							char * pPassword,
							int maxLenPassword)
{
    return;
}

static void	AuthDataFn(	const char * pServer,
						const char * pShare,
						char * pWorkgroup,
						int maxLenWorkgroup,
						char * pUsername,
						int maxLenUsername,
						char * pPassword,
						int maxLenPassword)
{
	string	sWorkGroup, sUser, sPasswd;
	String sMsg;
	sMsg.Append(_("Samba Connect Input WorkGroup - [%s - %s]"), pServer, pShare);
	if (InputBox(sMsg.c_str(), sWorkGroup) == ERROR)
	{
		pWorkgroup = 0;
		pUsername = 0;
		pPassword = 0;
		return;
	}

	sprintf( pWorkgroup, "%s", sWorkGroup.c_str() );

	sMsg.clear();
	sMsg.Append(_("Samba Connect Input Username - [%s - %s] [%s]"), pServer, pShare, pWorkgroup);
	if (InputBox( sMsg.c_str(), sUser ) == ERROR)
	{
		pUsername = 0;
		pPassword = 0;
		return;
	}

	sprintf( pUsername, "%s", sUser.c_str() );

	sMsg.clear();
	sMsg.Append(_("Samba Connect Input Passwd - [%s - %s] [%s %s] "), 
				pServer, pShare, pWorkgroup, pUsername);
	if (InputBox(sMsg.c_str(), sPasswd, true) == ERROR)
	{
		pPassword = 0;
		return;
	}
	
	sprintf( pPassword, "%s", sPasswd.c_str() );
}

SMBReader::SMBReader()
{
	_sReaderName = "smb";
	_sInitTypeName = "smb:/";
	
	_nDir = -1;
	_pContext = 0;
	_pDirent = 0;

	_bConnected = false;
	_eEncode = US;
}

SMBReader::~SMBReader()
{
	Destroy();
}

bool	SMBReader::Init(const string& sInitFile)
{
	_bConnected = false;
	_bNoAuth = true;

	SMBCCTX * pContext = 0;

	LOG_WRITE("SMBReader::Init :: %s", sInitFile.c_str());

	Destroy();

	if ( !_pContext )
	{
		/* Allocate a new context */
		pContext = smbc_new_context();

		if ( !pContext )
		{
			throw Exception( "Could not allocate new smbc context" );
			return false;
		}

		_pContext = (void*)pContext;
	}

    /* Set mandatory options (is that a contradiction in terms?) */
	/*
	#ifndef DEBUG
	*/
    	pContext->debug = 0;
	/*
	#else
		pContext->debug = 1;
		smbc_option_set(context, "debug_stderr");
	#endif
	*/

    pContext->callbacks.auth_fn = (_bNoAuth ? No_AuthDataFn : AuthDataFn);

    /* Initialize the context using the previously specified options */
    if ( !smbc_init_context(pContext) )
	{
        smbc_free_context(pContext, 0);
		_pContext = 0;
		throw Exception( "Could not initialize smbc context " );
		return false;
    }

	/* Tell the compatibility layer to use this context */
    smbc_set_context( pContext );

	_bConnected = true;
	
	if ( sInitFile.size() >= 6 && sInitFile.substr( 0, 5 ) == "smb:/" )
	{
		_sHome = sInitFile.substr( 5 );
		_sInitFile = sInitFile;
	}
	else
	{
		_sHome = sInitFile;
		_sInitFile = "smb:/" + sInitFile;
	}
	
	_sInitTypeName = _sInitFile;
	_sViewPath = _sCurPath = _sHome;
	LOG_WRITE("SMBReader::Init Success :: %s", sInitFile.c_str());
	return true;
}

void	SMBReader::Destroy()
{
	_pDirent = 0;
	
	if ( _nDir > 0 )
	{
		smbc_closedir( _nDir );
		_nDir = -1;
	}

	if ( _pContext )
	{
		smbc_free_context((SMBCCTX*)_pContext, 0);
		_pContext = 0;
	}
}

string MLS::SMBReader::GetViewPath() const
{
	string sPath = _sViewPath;

	// workgroup remove.
	string::size_type f =  sPath.find("@workgroup");
	if ( f != string::npos )
	{
		string strTmp1 = sPath.substr( 0, f );
		string strTmp2 = sPath.substr( f+10 );

		string::size_type f1 = strTmp1.rfind("/");
		sPath = strTmp1.substr( 0, f1 ) + strTmp2;
		LOG_WRITE("MLS::SMBReader::GetViewPath [%s]", strTmp1.substr( 0, f1 ).c_str(), sPath.c_str() );
	}

	LOG_WRITE("MLS::SMBReader::GetViewPath [%s]", sPath.c_str() );
	return ChgCurLocale( sPath );
}

string MLS::SMBReader::GetRealPath( const string & str ) const
{
	string sPath = str;

	if (sPath.size() == 0) sPath = _sCurPath;

	// workgroup remove.
	string::size_type f =  sPath.find("@workgroup/");
	if ( f != string::npos )
	{
		string strTmp1 = sPath.substr( 0, f );
		string strTmp2 = sPath.substr( f+10 );

		string::size_type f1 = strTmp1.rfind("/");
		sPath = strTmp1.substr( 0, f1 ) + strTmp2;
	}
	
	// ~ 인지 먼저 판단.
	if (sPath[0]=='~')
	{
		sPath = _sHome;
	}
	else if (sPath[0]!='/') // .. /(root) dir이 아니면
	{
		// ... 현재 dir를 얻어야 한다면 현재 dir절대경로를 찾는다. 
		if (sPath == ".") sPath = _sCurPath;
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
	LOG_WRITE("SMBReader::GetRealPath :: [%s]", sPath.c_str());
	return sPath;
}

bool	SMBReader::Read( const string& sPath )
{
	if ( _nDir > 0 )
	{
		smbc_closedir( _nDir );
		_nDir = -1;
	}

	_pDirent = 0;

	string	strPath;
	bool	bAccess = false;

	strPath = GetRealPath(  sPath );

	LOG_WRITE("smbc_opendir :: [smb:/%s]", strPath.c_str() );

	void*	pWait = MsgWaitBox(_("Samba connect"), _("Please wait !!!"));

SmbAuthCheck:
	if ((_nDir = smbc_opendir( ("smb:/" + strPath).c_str() )) < 0)
	{
		MsgWaitEnd(pWait);

		string	strMsg;
		if ( errno == EACCES )
		{
			SMBCCTX* pContext = (SMBCCTX*)_pContext;
			if ( !bAccess )
			{
				pContext->callbacks.auth_fn = AuthDataFn;
				smbc_set_context( pContext );
				bAccess = true;
				goto SmbAuthCheck;
			}
			else
			{
				pContext->callbacks.auth_fn = No_AuthDataFn;
				smbc_set_context( pContext );
			}	
		}

		String	strMsg2( _("Could not open directory [smb:/%s] - %s"),
						strPath.c_str(), strerror( errno ) );
		MsgBox( _("Error"), strMsg2.c_str() );
		return false;
	}

	MsgWaitEnd(pWait);

	LOG_WRITE("smbc_opendir :: [smb:/%s] success !!!", strPath.c_str() );
	_sViewPath = _sCurPath = strPath;
	return true;
}

bool	SMBReader::Next()
{
	if ( _nDir < 0 ) 
		return false;

	_pDirent = (void*)smbc_readdir( _nDir );
	if ( !_pDirent )
	{
		if ( errno != EBADF )
			LOG_WRITE("SMBReader::Next :: smbc_readdir [%s]", strerror( errno ) );
		return false;
	}
	return true;
}

bool	SMBReader::GetInfo( MLS::File& tFile )
{
	struct smbc_dirent*	pDirent = (struct smbc_dirent*)_pDirent;

	string 		strTmp;

	if ( !pDirent ) return false;

	LOG_WRITE("GetInfo Name [%s]", pDirent->name );

	switch( pDirent->smbc_type )
	{
	case SMBC_LINK:
	case SMBC_FILE:
	case SMBC_DIR:
		// . 파일정보를 얻는다. 
		strTmp = _sCurPath + pDirent->name;
		struct		stat	tStat;
		if (smbc_stat( ("smb:/"+strTmp).c_str(), &tStat)==-1)
		{
			if (_bMsgShow) MsgBox(_("Error"), strerror(errno));
			return false;
		}
		SMBFileRead( tFile, &tStat );
		break;
	case SMBC_WORKGROUP:
		SMBShareRead( "WORKGROUP", tFile );
		tFile.sName = pDirent->name;
		tFile.sFullName = _sCurPath + pDirent->name + "@workgroup";
		break;
	case SMBC_SERVER:
		SMBShareRead( "SERVER", tFile );
		tFile.sFullName = _sCurPath + pDirent->name; // server name is workgroup delete
		tFile.sName = ChgCurLocale( pDirent->name );
		break;
	case SMBC_FILE_SHARE:
		SMBShareRead( "FILESHARE", tFile );
		tFile.sName = ChgCurLocale( pDirent->name );
		tFile.sFullName = _sCurPath + pDirent->name;
		break;
	case SMBC_PRINTER_SHARE:
		SMBShareRead( "PRTSHARE", tFile );
		tFile.sName = ChgCurLocale( pDirent->name );
		tFile.sFullName = _sCurPath + pDirent->name;
		break;
	case SMBC_COMMS_SHARE:
		SMBShareRead( "COMMSSHARE", tFile );
		tFile.sName = ChgCurLocale( pDirent->name );
		tFile.sFullName = _sCurPath + pDirent->name;
		break;
	case SMBC_IPC_SHARE:
		SMBShareRead( "IPCSHARE", tFile );
		tFile.sName = ChgCurLocale( pDirent->name );
		tFile.sFullName = _sCurPath + pDirent->name;
		break;
	}
	return true;
}

void	SMBReader::SMBShareRead( const string strType, MLS::File& tFile )
{
	tFile.bDir = true;
	tFile.bLink = false;
	tFile.sType = _sInitTypeName;
	tFile.uSize = 0;	
	tFile.sDate = strType;
	tFile.sTmp = strType;
	tFile.sAttr = "drwxr-xr-x";
	tFile.sOwner = "samba";
	tFile.sGroup = "samba";
}

void	SMBReader::SMBFileRead( MLS::File& tFile, const struct stat* pStatBuf )
{
	if ( !pStatBuf )
		throw Exception( "SMBReader::SMBFileRead pStatBuf is null !!!");

	struct smbc_dirent* pDirent = (struct smbc_dirent*)_pDirent;

	if ( !pDirent ) 
		throw Exception( "SMBReader::SMBFileRead pDirent is null !!!");

	// . 파일 크기를 알아냄
	tFile.sTmp = "FILE";
	tFile.uSize = pStatBuf->st_size;
	
	// 파일 이름 알아냄
	tFile.sType = _sInitTypeName;
	tFile.bLink = false;
	tFile.sName = ChgCurLocale( pDirent->name );
	
	if (tFile.sName == "." || tFile.sName == "..")
		tFile.sFullName = GetRealPath( pDirent->name );
	else
		tFile.sFullName = _sCurPath + pDirent->name;

	// . directory 여부를 알아냄
	if (S_ISDIR(pStatBuf->st_mode))
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

	char	cAttr[10];
	if (tFile.bLink)
		cAttr[0]= 'l';
	else 
		cAttr[0]= tFile.bDir ? 'd' : '-';
		
	cAttr[1]= S_IRUSR & pStatBuf->st_mode ? 'r' : '-';
	cAttr[2]= S_IWUSR & pStatBuf->st_mode ? 'w' : '-';
	cAttr[3]= S_IXUSR & pStatBuf->st_mode ? 'x' : '-';
	if (S_ISUID & pStatBuf->st_mode) cAttr[3] = 'S';
	if (S_ISUID & pStatBuf->st_mode &&
		S_IXUSR & pStatBuf->st_mode) cAttr[3] = 's';
	cAttr[4]= S_IRGRP & pStatBuf->st_mode ? 'r' : '-';
	cAttr[5]= S_IWGRP & pStatBuf->st_mode ? 'w' : '-';
	cAttr[6]= S_IXGRP & pStatBuf->st_mode ? 'x' : '-';
	if (S_ISGID & pStatBuf->st_mode) cAttr[6] = 'S';
	if (S_ISGID & pStatBuf->st_mode &&
		S_IXGRP & pStatBuf->st_mode) cAttr[6] = 's';
	cAttr[7]= S_IROTH & pStatBuf->st_mode ? 'r' : '-';
	cAttr[8]= S_IWOTH & pStatBuf->st_mode ? 'w' : '-';
	cAttr[9]= S_IXOTH & pStatBuf->st_mode ? 'x' : '-';
	if (S_ISVTX & pStatBuf->st_mode) cAttr[9] = 'S';
	if (S_ISVTX & pStatBuf->st_mode &&
		S_IXUSR & pStatBuf->st_mode) cAttr[9] = 's';
	cAttr[10] = 0;
	tFile.sAttr = cAttr;
	// ====================================================
	
	// . 파일 생성 날짜를 알아냄
	char cDate[20];
	memset(&cDate, 0, sizeof(cDate));
	strftime(cDate, 9, "%y-%m-%d", localtime(&pStatBuf->st_mtime));
	tFile.tCreateTime = pStatBuf->st_mtime;
	tFile.sDate = cDate;

	// . 파일 생성 시간을 알아냄
	memset(&cDate, 0, sizeof(cDate));
	strftime(cDate, 6, "%H:%M", localtime(&pStatBuf->st_mtime));
	tFile.sTime = cDate;

	tFile.sOwner = "samba";
	tFile.sGroup = "samba";
}
bool SMBReader::Copy(	Selection& tSelection, 		// remote
						const string& sTargetPath, 	// local
						Selection*	pSelection )	
{
	string			sCurName, sTargetPathTmp, sTargetPathTmp2;
	string			sSourceName, sTargetName;
	vector<File*>	vFiles;

	ullong	uFileSize = tSelection.CalcSize();
	uint	uSize = tSelection.GetSize();
	
	string 	sSize = toregular(uSize), sRestSize;

	bool	bSkipAll = false, bOverwriteAll = false;
	ullong	uLastSize = 0, uViewCount=1, uPsize=0, uCsize = 0;
	
	struct stat src_stat, tar_stat;

	int	nBufSize = 8196;

	char	buf[nBufSize];	// reading byte 

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

	tProgress.Start();

	string sSourcePath = tSelection.GetSelectPath();
	
	LOG_WRITE("Copy sTargetPath [%s] [%s] [%s]", sTargetPath.c_str(), _sCurPath.c_str(), sTargetPathTmp2.c_str());

	// 파일 복사
	for (int n=0; n<(int)vFiles.size(); n++)
	{
		File*	pFile = vFiles[n];
		bool	bOverwrite = false;
		
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		// 소스 이름과 타겟 이름 정하기	
		sSourceName = pFile->sFullName;
		sTargetName = 	sTargetPathTmp2 + 
						ChgCurLocale( pFile->sFullName.substr(sSourcePath.size()) );

		struct stat		sStat;
		if ( smbc_stat( ("smb:/"+sSourceName).c_str(), &sStat) < 0 )
		{
			// 소스가 없다니.. 참
			String	sMsg; string sErrMsg = strerror( errno );
			sMsg.Append(_("File access error (%s), continue? !!! "), pFile->sName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
			tProgress.Start();
			continue;
		}

		LOG_WRITE("SMBReader Copy sTargetName 1 [%s] [%s] [%s] [%d]", 
					sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str(), sStat.st_size);
		
		String	sCount, sCount2;
		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), sSize.c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr2(sCount.c_str());
		
		if (stat(sTargetName.c_str(), &tar_stat)!=-1)
		{
			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain_samba_copy:
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
						bOverwrite = true;
						break;
					
					case 1: // skip
						uPsize += sStat.st_size;
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
												sRename) < 0) goto askagain_samba_copy;
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
						uPsize += sStat.st_size;
						uViewCount++;
						continue;
					
					case -1:
					case 5: // cancel
						goto halt_samba_copy;
							
					default:
						break;
				}
			}
		}
		
		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir)
		{
			if (pSelection != NULL)
			{
				File	tTargetFile = *pFile;
				tTargetFile.sFullName = sTargetName;
				pSelection->Add(&tTargetFile);
			}

			if (mkdir(sTargetName.c_str(), sStat.st_mode)==-1) // 디렉토리를 만든다.
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
		
		uCsize=0;
		// 실제 복사 오퍼레이션	
		
		uViewCount++;
		
		int nFileHandle = smbc_open( ("smb:/"+sSourceName).c_str(), O_RDONLY, 0);

		if (nFileHandle < 0)
		{
			String	sMsg; string sErrMsg = strerror( errno );
			sMsg.Append(_("File access error (%s), continue? !!! "), pFile->sFullName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
			tProgress.Start();
			continue;
		}		

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
						smbc_close(nFileHandle);
						remove( sTargetName.c_str() );
						goto halt_samba_copy;
					}
					tProgress.Start();
				}	
				
				if (sStat.st_size <= uCsize ) break;
			
				if ((uLastSize = smbc_read(nFileHandle, buf, sizeof(buf))) == -1)
				{
					String	sMsg; string sErrMsg  = strerror( errno );
					sMsg.Append(_("remote file access error (%s), continue? !!! "), pFile->sFullName.c_str());
					if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
					tProgress.End();
					if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
					fclose(out);
					smbc_close(nFileHandle);
					remove( sTargetName.c_str() );
					goto halt_samba_copy;
				}
				
				fwrite(buf, 1, uLastSize, out);
	
				uCsize +=uLastSize;
				uPsize +=uLastSize;

				//smbc_lseek( pFtpFileHandle, uCsize);

				sCount2.Printf("%s/%s", toregular(uCsize).c_str(), 
										toregular(pFile->uSize).c_str());
				tProgress.setRightStr(sCount2.c_str());

				if (pFile->uSize && uCsize && uPsize && uFileSize)
				{
					tProgress.setCount(	(uCsize * 100) / pFile->uSize,
										(uPsize * 100) / uFileSize );
				}
			}
		}
		else
		{
			smbc_close(nFileHandle);
			tProgress.End();
			if (YNBox(_("Error"), _("local file access error, continue?"), false) == false) break;
			tProgress.Start();
			continue;
		}

		fclose(out);

		chmod(sTargetName.c_str(), sStat.st_mode);
		smbc_close(nFileHandle);
		
		if (pSelection != NULL)
		{
			File	tTargetFile = *pFile;
			tTargetFile.sFullName = sTargetName;
			pSelection->Add(&tTargetFile);
		}
	}
halt_samba_copy:
	tProgress.End();
	if (pSelection)
		pSelection->SetSelectPath(sTargetPathTmp2);
	return true;
}

bool	SMBReader::Paste(Selection& tSelection)
{
	string			sCurName, sTargetPathTmp, sTargetPathTmp2;
	string			sSourceName, sTargetName;
	vector<File*>	vFiles;
	struct stat 	src_stat;

	int		nBufSize = 8196;
	char	buf[nBufSize];

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

	EncodeChk(vFiles, false);

	tProgress.Start();

	// 파일 디렉토리 먼저 생성 복사
	for (int n=0; n<(int)vFiles.size(); n++)
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
			if ( smbc_mkdir( ("smb:/"+ sTargetName).c_str(), 0755) == -1)
			{
				String	sMsg; string sErrMsg = strerror( errno );
				sMsg.Append(_("Samba Directory exists, continue !!! [%s]"), sTargetName.c_str());
				if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
				tProgress.End();
				if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
				tProgress.Start();
				return false;
			}
			continue;
		}
	}

	for (int n=0; n<(int)vFiles.size(); n++)
	{
		File*	pFile = vFiles[n];
		
		// 파일 이름이 없을경우 continue
		if (!pFile) continue;

		// 소스 이름과 타겟 이름 정하기	
		sSourceName = pFile->sFullName;
		sTargetName = sTargetPathTmp2 + pFile->sFullName.substr(sSourcePath.size());
		sTargetName = KorCodeChg(sTargetName, _eEncode);

		LOG_WRITE("SMBReader::Paste [%s] [%s]", sSourceName.c_str(), sTargetName.c_str());

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

		LOG_WRITE("SMBReader Paste sTargetName 1 [%s] [%s] [%s]", sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str());
		
		mode_t permission = src_stat.st_mode;

		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir) continue; 

		String	sCount, sCount2;
		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), sSize.c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr2(sCount.c_str());

		struct stat		sStat;
		if ( smbc_stat(sTargetName.c_str(), &sStat) == 0 ) // success 
		{
			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain_samba_paste:
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
								if (InputBox(	_("Rename"), sRename) < 0) goto askagain_samba_paste;
								tProgress.Start();
	
								sTargetName = sTargetPathTmp2 + sRename;
								
								if ( smbc_rename(
										(char*)("smb:/"+pFile->sFullName).c_str(),
										(char*)("smb:/"+sTargetName).c_str() ) == -1)
								{
									tProgress.End();
									MsgBox(	_("Error"), _("File exists"));
									tProgress.Start();
									continue;
								}
								tProgress.End();
							}
							break;
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
						goto halt_samba_paste;
							
					default:
						break;
				}
			}
		}
		
		uCsize=0;	
		// 실제 복사 오퍼레이션	
		
		uViewCount++;

		int nFileHandle = smbc_open(("smb:/"+sTargetName).c_str(), 
									O_RDWR | O_CREAT | O_TRUNC, 0666);

		if (nFileHandle < 0)
		{
			String	sMsg; string sErrMsg = strerror( errno );
			sMsg.Append(_("File write access error (%s), continue? !!! "), sTargetName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
			tProgress.Start();
			continue;
		}

		FILE*	fp = fopen(sSourceName.c_str(), "rb");
		
		if ( fp )
		{
			while( !feof(fp) )
			{
				if (tProgress.isExit())
				{
					tProgress.End();
					if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
					{
						smbc_close(nFileHandle);
						fclose(fp);
						smbc_unlink( (char*)sTargetName.c_str() );
						goto halt_samba_paste;
					}
					tProgress.Start();
				}				
				
				uLastSize = fread(buf, 1, sizeof(buf), fp);
				
				int nSize = 0, nSizePlus = 0;
				do
				{
					if (tProgress.isExit()) break;
						
					nSize = 0;
					if ((nSize = smbc_write(nFileHandle, buf+nSizePlus, uLastSize-nSizePlus)) < 0)
					{
						tProgress.End();
						String		sMsg; string sErrMsg = strerror( errno );
						sMsg.Append(_("file writing failure. continue?"));
						if (!sErrMsg.empty()) sMsg.Append(" [%s]", sErrMsg.c_str()); 						
						if (YNBox(_("Error"), sMsg.c_str() , false)==false)
						{
							smbc_close(nFileHandle);
							fclose(fp);
							smbc_unlink( sTargetName.c_str() );
							goto halt_samba_paste;
						}
						tProgress.Start();
						break;
					}
					
					uCsize += nSize; uPsize += nSize; nSizePlus += nSize;
				}
				while( nSizePlus < uLastSize );
				
				if (tProgress.isExit()) continue;
				
				//smbc_lseek( nFileHandle, uCsize);

				sCount2.Printf("%s/%s", toregular(uPsize).c_str(),
										toregular(uFileSize).c_str());
				
				tProgress.setRightStr(sCount2.c_str());
				if (src_stat.st_size && uCsize && uPsize && uFileSize)
				{
					tProgress.setCount(	(uCsize * 100) / src_stat.st_size,
										(uPsize * 100) / uFileSize );
				}
			}
		}
		else
		{
			tProgress.End();
			if (YNBox(_("Error"), _("File access error, continue?"), false) == false) break;
			tProgress.Start();
			continue;
		}
		
		/*
		struct stat		tOutStat;
		smbc_stat(sTargetName, &tOutStat);
		tOutStat.permissions = permission;
		smbc_ssettat(sTargetName, &tOutStat);
		*/
		smbc_close(nFileHandle);
		fclose(fp);
	}
halt_samba_paste:
	tProgress.End();
	return true;
}

bool SMBReader::Remove(MLS::Selection& tSelection, bool bMsgShow, bool bIgnore)
{
	CommonProgress tProgress(_("samba delete file"), "", true);
	if (bMsgShow)
	{
		if (YNBox(_("Info"), _("samba delete file ?"), false) == false) return 0;
		
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

	for (int n=0; n < (int)vFile.size(); n++)
	{
		File*	pFile = vFile[n];
		sTargetName = pFile->sFullName;
		LOG_WRITE("samba file remove [%s]", sTargetName.c_str());
		
		if (smbc_unlink( ("smb:/"+sTargetName).c_str() ) < 0 )
		{
			String	sMsg; string sErrMsg = strerror( errno );
			sMsg.Append(_("samba file remove failure : %s : continue ? "), pFile->sName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sMsg.c_str(), false)==true)
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
			sStr.Printf("%s/%s", toregular(nCount).c_str(), toregular(uSize).c_str());
			tProgress.setRightStr(pFile->sName.c_str());
		
			if (nCount && uSize)
				tProgress.setCount( nCount * 100 / uSize );

			if (tProgress.isExit())
			{
				tProgress.End();
				if (YNBox(_("Error"), _("Do you want to stop remove operation?"), false)==true) 
				{
					return false;
				}
				tProgress.Start();
			}
		}
	}

	// 디렉토리는 나중에 삭제
	for (int n=0; n < (int)vDirs.size(); n++)
	{
		File*	pFile = vDirs[n];
		sTargetName = pFile->sFullName;
		LOG_WRITE("samba dir remove [%s]", sTargetName.c_str());
		if (smbc_rmdir( ("smb:/"+sTargetName).c_str() ) < 0 )
		{
			String	sMsg; string sErrMsg = strerror( errno );
			sMsg.Append(_("samba dir remove failure : %s : %s. continue ? "), pFile->sName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sStr.c_str(), false)==true)
			{
				tProgress.Start();
				continue;
			}
			return false;
			break;
		}

		nCount++;
		sStr.Printf("%s/%s", toregular(nCount).c_str(), toregular(uSize).c_str());
		
		if (bMsgShow)
		{
			tProgress.setRightStr(sTargetName.c_str());
			if (nCount && uSize)
				tProgress.setCount( nCount * 100 / uSize );
			
			if (tProgress.isExit())
			{
				tProgress.End();
				if (YNBox(_("Warning"), _("Do you want to stop remove operation?"), false)==true) break;
				tProgress.Start();
			}
		}
	}

	tProgress.End();
	return true;
}

///	\brief	파일을 보기 위한..
///	\param	tFile	볼 파일.
///	\return	파일을 tmp 에 복사 해놓고 파일 위치 리턴
bool  SMBReader::View(const File* pFileOriginal, File* pFileChange)
{ 
	string	sSourceName, sTargetName;
	
	int		nBufSize = 8196;
	char	buf[nBufSize];
	 
	// 파일 이름이 없을경우 continue 
	if (!pFileOriginal) return false;

	// 소스 이름과 타겟 이름 정하기	
	sSourceName = pFileOriginal->sFullName;
	sTargetName = _sTmpDir + pFileOriginal->sName;

	LOG_WRITE("SMBReader::View [%s] [%s]", sSourceName.c_str(), sTargetName.c_str());

	struct stat		sStat;
	if ( smbc_stat( ("smb:/"+sSourceName).c_str(), &sStat) < 0 ) // success 
	{
		String	sMsg; string sErrMsg = strerror( errno );
		sMsg.Append(_("File access error (%s), continue? !!!"), sSourceName.c_str());
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}

	LOG_WRITE("SMBReader View sTargetName 1 [%s] [%s] [%s] [%d]", 
				sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str(), sStat.st_size);
	
	ullong	uFileSize = pFileOriginal->uSize;
	string 	sRestSize;

	ullong	uLastSize = 0, uCsize = 0;

	CommonProgress tProgress(_("sftp view file"), "");

	tProgress.setLeftStr(pFileOriginal->sName.c_str());
	tProgress.Start();

	int nFileHandle = smbc_open( ("smb:/"+sSourceName).c_str(), O_RDONLY, 0);

	if (nFileHandle < 0)
	{
		String	sMsg; string sErrMsg = strerror( errno );
		sMsg.Append(_("File access error (%s), continue? !!!"), sSourceName.c_str());
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		tProgress.End();
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}

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
					smbc_close(nFileHandle);
					remove( sTargetName.c_str() );
					return false;
				}
				tProgress.Start();
			}
			
			if (sStat.st_size <= uCsize ) break;
			
			if ((uLastSize = smbc_read(nFileHandle, buf, sizeof(buf))) == -1)
			{
				smbc_close(nFileHandle);
				tProgress.End();

				String	sMsg; string sErrMsg = strerror( errno );
				sMsg.Append(_("File access error (%s). !!!"),
							 pFileOriginal->sFullName.c_str());

				if (!sErrMsg.empty()) 
					sMsg.Append("[%s]", sErrMsg.c_str());
				MsgBox(_("Error"), sMsg.c_str());
				return false;
			}
			fwrite(buf, 1, uLastSize, out);

			uCsize +=uLastSize;
			
			//libssh2_sftp_seek(pFtpFileHandle, uCsize);
			sCount.Printf(	"%s/%s", 
							toregular(uCsize).c_str(), 
							toregular(sStat.st_size).c_str());

			tProgress.setRightStr(sCount.c_str());
			tProgress.setCount( uCsize * 100 / uFileSize );
		}
	}
	else
	{
		smbc_close(nFileHandle);
		tProgress.End();
		MsgBox(_("Error"), _("local file access error !!!"));
		return false;
	}

	fclose(out);
	smbc_close(nFileHandle);

	tProgress.End();
	*pFileChange = *pFileOriginal;
	pFileChange->sTmp2 = pFileOriginal->sFullName;
	pFileChange->sFullName = sTargetName;
	pFileChange->sName = pFileOriginal->sName;
	return true;
}


bool	SMBReader::Rename(File* pFile, const string& sRename)
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

	LOG_WRITE("Rename - [%s] [%s]", pFile->sFullName.c_str(), sRenameName.c_str());

	if ( smbc_rename(	("smb:/"+pFile->sFullName).c_str(), 
						("smb:/"+sRenameName).c_str() ) < 0 )
	{
		String	sMsg; string	sErrMsg = strerror( errno );
		sMsg.Append(_("rename failure !!! [%s]"), sRename.c_str());
		if ( !sErrMsg.empty() ) sMsg.Append("%s", sErrMsg.c_str() );
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}
	return true;
}

bool	SMBReader::Mkdir(const string& sFullPathName)
{
	string sMkdirName;
	if (sFullPathName == "")
	{
		if (InputBox(_("Make Directory"), sMkdirName) == ERROR)
			return false;
	}
	else
		sMkdirName = sFullPathName;
	
	sMkdirName = _sCurPath + sMkdirName;

	if ( smbc_mkdir( ("smb:/"+sMkdirName).c_str(), 0755) < 0 )
	{
		String	sMsg; string	sErrMsg = strerror( errno );
		sMsg.Append(_("mkdir failure !!! [%s]"), sMkdirName.c_str());
		if ( !sErrMsg.empty() ) sMsg.Append("%s", sErrMsg.c_str() );
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}
	return true;
}

void	SMBReader::EncodeChk(vector<File*>& tFileList, bool bEncChk)
{
	ENCODING	eEncode = US;
	bool		bChgEncode = false;
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

bool MLS::SMBReader::isChkFile( const File & tFile )
{
	struct stat		sStat;
	if ( smbc_stat( ("smb:/"+tFile.sFullName).c_str(), &sStat) < 0 )
	{
		String	sMsg; string sErrMsg = strerror( errno );
		sMsg.Append(_("File access error (%s), continue? !!!"), tFile.sFullName.c_str());
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}
	return true;
}


