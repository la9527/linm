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
#include "SFtpReader.h"
#include "ftplib.h"
#include "selection.h"
#include "sortfile.h"

#include "libssh2.h"
#include "libssh2_sftp.h"

#include "mlscfgload.h"

#include "mlslocale.h"

using namespace MLS;
using namespace MLSUTIL;

SFtpReader::SFtpReader() 
{
	_sReaderName = "sftp";
	_sInitTypeName = "sftp://";
	
	_pSession = NULL;
	_pChannel = NULL;
    _pSessionSFtp = NULL;
	_pDir = NULL;
	_pFileAttr = NULL;
	_bConnected = false;
	_eEncode = US;
	_pDefaultFtpNet = NULL;
}

SFtpReader::~SFtpReader()
{
	Destroy();
}

int		SFtpReader::GetIpUserPw(const string& sStr, 
								string& sIP, 
								string& sUser, 
								string& sPasswd,
								string& sDir,
								string& sKeyPublic,
								string& sKeyPrivate)
{
	string sStrTmpDir = sStr;
	if (sStrTmpDir != "")
	{
		// la9527:1234@kldp.net:20?prikeypath:pubkeypath?/path
		string::size_type p1 = sStrTmpDir.find("?");

		if (p1 != string::npos)
		{
			string sKeyData = sStrTmpDir.substr(p1+1);
			
			printf("KeyData [%s]\n", sKeyData.c_str() );

			string::size_type p3 = sKeyData.find(":");			
			string::size_type p4 = sKeyData.find("?");
			
			if (p3 != string::npos)
			{
				sKeyPublic = sKeyData.substr(0, p3);
				printf("KeyPublic  [%s]\n", sKeyPublic.c_str() );
				if ( p4 == string::npos )
					sKeyPrivate = sKeyData.substr(p3+1);
				else
					sKeyPrivate = sKeyData.substr(p3+1, p4 - p3 - 1);
					
				printf("KeyPrivate  [%s]\n", sKeyPrivate.c_str() );
				sStrTmpDir = sStrTmpDir.substr( 0, p1 ) + sStrTmpDir.substr( p1 + sKeyPublic.size() + sKeyPrivate.size() + 3 );
				printf("sStrTmpDir  [%s]\n", sStrTmpDir.c_str() );
			}
			else
			{
				if ( p4 == string::npos )
					sKeyPrivate = sKeyData;
				else
					sKeyPrivate = sKeyData.substr( 0, p4 );
								
				sStrTmpDir = sStrTmpDir.substr(0, p1);
			}
		}
		
		string::size_type p = sStrTmpDir.rfind("@");
		
		if (p == string::npos ) // '@' not found.
		{
			string::size_type p2 = sStrTmpDir.find("/");
			
			if (p2 != string::npos)
			{
				sIP = sStrTmpDir.substr(0, p2);
				sDir = sStrTmpDir.substr(p2+1);
			}
			else
				sIP = sStrTmpDir;
		}
		else
		{
			{ // get IP, Directory
				string	sIP_Dir = sStrTmpDir.substr(p+1);
				string::size_type p2 = sIP_Dir.find("/");
				string sStrTmp = sStrTmpDir;
		
				if (p2 != string::npos)
				{
					sIP = sIP_Dir.substr(0, p2);
					sDir = sIP_Dir.substr(p2);
				}
				else
					sIP = sIP_Dir;
			}
			
			{ // get User, Password
				string				sTmp = sStrTmpDir.substr(0, p);
				string::size_type 	pPasswd = sTmp.find(":");
				
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
		}
	}
	return SUCCESS;
}

void	SFtpReader::GetMethods()
{
	const char* sKex = NULL, *sHostkey, *sCrypt_cs, *sCrypt_sc, *sMac_cs, *sMac_sc,
			*sComp_cs, *sComp_sc, *sLang_cs, *sLang_sc;

	LIBSSH2_SESSION* pSession = (LIBSSH2_SESSION*)_pSession;

    sKex = libssh2_session_methods(pSession, LIBSSH2_METHOD_KEX);
	sHostkey = libssh2_session_methods(pSession, LIBSSH2_METHOD_HOSTKEY);
    sCrypt_cs = libssh2_session_methods(pSession, LIBSSH2_METHOD_CRYPT_CS);
    sCrypt_sc = libssh2_session_methods(pSession, LIBSSH2_METHOD_CRYPT_SC);
    sMac_cs = libssh2_session_methods(pSession, LIBSSH2_METHOD_MAC_CS);
    sMac_sc = libssh2_session_methods(pSession, LIBSSH2_METHOD_MAC_SC);
    sComp_cs = libssh2_session_methods(pSession, LIBSSH2_METHOD_COMP_CS);
    sComp_sc = libssh2_session_methods(pSession, LIBSSH2_METHOD_COMP_SC);
    sLang_cs = libssh2_session_methods(pSession, LIBSSH2_METHOD_LANG_CS);
    sLang_sc = libssh2_session_methods(pSession, LIBSSH2_METHOD_LANG_SC);

	LOG_WRITE("Kex        :: [%s]",  sKex );
	LOG_WRITE("sHostkey   :: [%s]",  sHostkey);
	LOG_WRITE("sCrypt_cs  :: [%s]",  sCrypt_cs );
	LOG_WRITE("sCrypt_sc  :: [%s]",  sCrypt_sc );
	LOG_WRITE("sMac_cs    :: [%s]",  sMac_cs );
	LOG_WRITE("sMac_sc    :: [%s]",  sMac_sc );
	LOG_WRITE("sComp_cs   :: [%s]",  sComp_cs );
	LOG_WRITE("sComp_sc   :: [%s]",  sComp_sc );
	LOG_WRITE("sLang_cs   :: [%s]",  sLang_cs );
	LOG_WRITE("sLang_sc   :: [%s]",  sLang_sc );
}

bool	SFtpReader::SetMethod(int nMethod, const string& str)
{
	if (libssh2_session_method_pref( (LIBSSH2_SESSION*)_pSession, nMethod, (char*)str.c_str() ) == -1)
	{
		LOG_WRITE("SetMethod Error :: [%s]", GetLastErrMsg().c_str());
		return false;
	}

	LOG_WRITE( "libssh2_session_method_pref [%s]", (const char*)str.c_str() );
	return true;
}

int		SFtpReader::SessionStartup(const string& sIP)
{
	Destroy();

	// use Ftp Socket. (프로그램 보수를 쉽게 하기 위한 방법)
	if (SFtpConnect(sIP.c_str(), (netbuf**)&_pDefaultFtpNet) == 0)
		throw Exception("sftp connect fail !!! - %s", sIP.c_str());
	
	int	nSockNum = FtpGetSocketID( (netbuf*)_pDefaultFtpNet );

	_pSession = (void*)libssh2_session_init();

	// reference : http://libssh2.sourceforge.net/doc/#libssh2sessionmethodpref
	// 
	string 	sKex = "none", sHostKey = "none", sCrypt_cs = "none", sMac_cs = "none", sComp_cs = "none";

	sKex = g_tCfg.GetValue("SSH", "Method_Kex", 
						"diffie-hellman-group1-sha1,"
						"diffie-hellman-group14-sha1,"
						"diffie-hellman-group-exchange-sha1");
	sHostKey = g_tCfg.GetValue("SSH", "Method_HostKey", 
						"ssh-dss,ssh-rsa");
	sCrypt_cs = g_tCfg.GetValue("SSH", "Method_Crypto",
						"aes256-cbc,"
						"rijndael-cbc@lysator.liu.se,"
						"aes192-cbc,"
						"aes128-cbc,"
						"blowfish-cbc,"
						"arcfour,"
						"cast128-cbc,"
						"3des-cbc,"
						"none");
	sMac_cs = g_tCfg.GetValue("SSH", "Method_Mac", 
						"hmac-sha1,"
						"hmac-sha1-96,"
						"hmac-md5,"
						"hmac-md5-96,"
						"hmac-ripemd160,"
						"hmac-ripemd160@openssh.com,"
						"none");
	sComp_cs = g_tCfg.GetValue("SSH", "Method_Compress", 
						"zlib,none");

	SetMethod( LIBSSH2_METHOD_KEX, sKex);
	SetMethod( LIBSSH2_METHOD_HOSTKEY, sHostKey);
	SetMethod( LIBSSH2_METHOD_CRYPT_CS, sCrypt_cs);
	SetMethod( LIBSSH2_METHOD_MAC_CS, sMac_cs );
	SetMethod( LIBSSH2_METHOD_MAC_SC, sMac_cs );
	SetMethod( LIBSSH2_METHOD_COMP_CS, sComp_cs);

	if ( libssh2_session_startup((LIBSSH2_SESSION*)_pSession, nSockNum))
	{
		GetMethods();
		throw Exception("Failure establishing SSH session.");
	}
	return nSockNum;
}

bool SFtpReader::Init(const string& sInitFile)
{
	string 	sIP, sUser, sPasswd, sDir, sPrivateKey, sPublicKey;
	int		nKeyAuth = 1;
	
	// sInitFile : username:passwd@IP
	// example   : annonymous:123123@name.com
	string sConnectionInfo = sInitFile;
	
	while(1)
	{
		GetIpUserPw(sConnectionInfo, sIP, sUser, sPasswd, sDir, sPublicKey, sPrivateKey);
		if (sIP == "" && sUser == "" && sPasswd == "")
		{	
			if (InputBox(	_("Input sftp connect url (user:passwd@hostname/dir)"),
							sConnectionInfo) == ERROR) return false;
		}
		else
			break;
	}

	if ( sUser == "" )
	{
		String sMsg;
		sMsg.Append("Input sftp username - [%s]", sIP.c_str());
		if (InputBox(sMsg.c_str(), sUser) == ERROR) return false;
	}

	LOG_WRITE("sInitFile [%s]", sInitFile.c_str());
	LOG_WRITE("sPrivateKey [%s] sPublicKey [%s]", sPrivateKey.c_str(), sPublicKey.c_str());
	
	void*	pWait = NULL;

	try
	{
		pWait = MsgWaitBox(_("SFtp connect"), _("Please wait !!!"));
	
		string::size_type p1 = sIP.find(":");
	
		if (p1 == string::npos)
			sIP = sIP + ":22"; // 없으면 22포트로 기본 설정.
		
		int	nSockNum = SessionStartup(sIP);
		
		bool bFinger = g_tCfg.GetBool("SSH", "FingerView", false);

		if ( bFinger )
		{
			const char *fingerprint = libssh2_hostkey_hash((LIBSSH2_SESSION*)_pSession, LIBSSH2_HOSTKEY_HASH_MD5);	
			if ( fingerprint )
			{
				String	strMsg;
				for(int i = 0; i < 16; i++) {
					strMsg.Append("%02X", (unsigned char)fingerprint[i]);
					if (i < 15 ) 
						strMsg.Append(":");
				}
				
				MsgBox(_("RSA key Finger info"), strMsg.c_str());
			}
		}

		// key or password authentication check.
		if ( !sPasswd.size() && nKeyAuth == 1)
		{
			if (sPrivateKey.size() == 0 || sPublicKey.size() == 0)
			{
				sPrivateKey = g_tCfg.GetValue("SSH", "DefPrivateKey", "~/.ssh/id_rsa");
				sPublicKey = g_tCfg.GetValue("SSH", "DefPublicKey", "~/.ssh/id_rsa.pub");
			}
			
			if (sPrivateKey.substr(0, 1) == "~")
				sPrivateKey = g_tCfg.GetValue("Static", "Home") + sPrivateKey.substr(1);
	
			if (sPublicKey.substr(0, 1) == "~")
				sPublicKey = g_tCfg.GetValue("Static", "Home") + sPublicKey.substr(1);

			LOG_WRITE("%s :: %s :: %s ::", sPrivateKey.c_str(), sPublicKey.c_str(), sUser.c_str());
	
			/* Or by public key */
			if (libssh2_userauth_publickey_fromfile(	(LIBSSH2_SESSION*)_pSession, 
														sUser.c_str(), 
														sPublicKey.c_str(), 
														sPrivateKey.c_str(), 
														"pasphrase") == 0)
			{
				nKeyAuth = 2;
				LOG_WRITE("libssh2_userauth_publickey_fromfile SUCCESS");
			}
			else
				nSockNum = SessionStartup(sIP);
		}

		if ( sPasswd.size() || nKeyAuth != 2)
		{
			MsgWaitEnd(pWait);

			if (sPasswd.size() == 0)
			{
				String sMsg;
				sMsg.Append("Input sftp passwd - [%s@%s]", sUser.c_str(), sIP.c_str());
				if (InputBox(sMsg.c_str(), sPasswd, true) == ERROR) return false;	
			}

			pWait = MsgWaitBox(_("SFtp connect"), _("Please wait !!!"));

			/* We could authenticate via password */
			int nRt = libssh2_userauth_password(	(LIBSSH2_SESSION*)_pSession, 
													sUser.c_str(), sPasswd.c_str());
			if (nRt == -1)
			{
				LOG_WRITE("UserAuth Password error. [%s]", GetLastErrMsg().c_str());
				throw Exception(_("Authentication by password failed."));
			}
		}

		/* Request a shell */
		//if ((_pChannel = (void*)libssh2_channel_open_session((LIBSSH2_SESSION*)_pSession)) == NULL)
		//	throw Exception("Unable to open a channel session.");
		
		/**
		* Some environment variables may be set,
		* It's up to the server which ones it'll allow though
		*/
		//if (libssh2_channel_setenv((LIBSSH2_CHANNEL*)_pChannel, "FOO", "bar"))
		//	throw Exception("libssh2_channel_setenv");
	
		/**
		* Request a terminal with 'vanilla' terminal emulation
		* See /etc/termcap for more options
		*/
		//if (libssh2_channel_request_pty((LIBSSH2_CHANNEL*)_pChannel, "vanilla")) 
		//	throw Exception( "Failed requesting pty");
		
		//if ( libssh2_channel_exec( (LIBSSH2_CHANNEL*)_pChannel, "pwd") )
		//{
		//	libssh2_channel_set_blocking( (LIBSSH2_CHANNEL*)_pChannel, 1 );
		//}
		
		_pSessionSFtp=(void*)libssh2_sftp_init( (LIBSSH2_SESSION*)_pSession );
			
		if (_pSessionSFtp == NULL)
			throw Exception( _("Unable to open a sftp session.") );
		
		MsgWaitEnd(pWait);
	
		string sPath;
		if (sDir.size() == 0) 
			sPath = GetPwd();
		else
		{
			sPath = GetRealPath( sDir );
		}

		if (sPath == "")
		{
			MsgBox(_("Error"),  _("get current dir reading failure !!!"));
			Destroy();
			return false;
		}
		
		_sCurPath = _sHome = sPath;
		_sCurPath += "/";
		_sInitTypeName = "sftp://" + sUser + "@" + sIP;
		_sIP = sIP;
		_sUser = sUser;

		if (!_pFileAttr) 
			_pFileAttr = (void*)new LIBSSH2_SFTP_ATTRIBUTES;

		LOG_WRITE("SFtp Connected [%s] [%s]", _sInitTypeName.c_str(), _sCurPath.c_str());

		_bConnected = true;
		_eEncode = US;
		_sInitFile = sInitFile;
	}
	catch (Exception& ex)
	{
		String	sMsg; string	sErrMsg = GetLastErrMsg();
		sMsg.Append("%s", (char*)ex);
		if ( !sErrMsg.empty() )	sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
		MsgWaitEnd(pWait);
		Destroy();
	}
	return _bConnected;
}

void	SFtpReader::Destroy()
{
	if ( _pFileAttr )
	{
		delete (LIBSSH2_SFTP_ATTRIBUTES*)_pFileAttr;
		_pFileAttr = NULL;
	}

	if ( _pDir )
	{
		libssh2_sftp_close_handle((LIBSSH2_SFTP_HANDLE*)_pDir);
		_pDir = NULL;
	}

	if (_pSessionSFtp)
	{
		libssh2_sftp_shutdown( (LIBSSH2_SFTP*)_pSessionSFtp );
		_pSessionSFtp = NULL;
	}

	if (_pSession)
	{
		libssh2_session_disconnect((LIBSSH2_SESSION*)_pSession, "done.");
		libssh2_session_free((LIBSSH2_SESSION*)_pSession);
		_pSession = NULL;
	}

	_sInitTypeName = "";
	_sCurPath = "";
	
	if (_pDefaultFtpNet)
	{
		int	nSockNum = FtpGetSocketID( (netbuf*)_pDefaultFtpNet );
		close (nSockNum);
	}
	_pDefaultFtpNet = NULL;
	_bConnected = false;
}

int	SFtpReader::GetLastError( string* sErrInfo )
{
	char*	strError = NULL;
	int		nRt = 0, n = 0;
	
	if ( !_pSession ) return -1;
	
	nRt = libssh2_session_last_error( (LIBSSH2_SESSION*)_pSession, &strError, &n, 1024);
	
	if ( strError )
	{
		*sErrInfo = strError;
		free( strError );
	}
	return nRt;
}

string	SFtpReader::GetLastErrMsg()
{
	string	str;
	GetLastError( &str );
	return str;	
}

/// \brief	상대경로는 절대경로로 바꾼다.
/// \param	str		상대경로 path명
///	\return	절대경로명
string SFtpReader::GetRealPath(const string& str) const
{
	string sPath = str;

	if (sPath.size() == 0) sPath = GetPwd();

	LOG_WRITE("GetRealPath One :: [%s]", str.c_str());
	
	// ~ 인지 먼저 판단.
	if (sPath[0]=='~')
	{
		char	cRealPath[2048];
		if ( libssh2_sftp_realpath(	(LIBSSH2_SFTP*)_pSessionSFtp,
                          			(char*)str.c_str(),
                          			cRealPath, sizeof(cRealPath)) == -1)
		{
			sPath = GetPwd(); // 에러면 getpwd를 보낸다.
		}
		else
			sPath = cRealPath;
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
				LOG_WRITE("GetRealPath :: [%s] [%s]", _sCurPath.c_str(), sPath.c_str());
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

string	SFtpReader::GetPwd() const
{
	char		cRealPath[4096];
	
	try
	{
		const char* cPath = ".";
		int nRt = 0;
		if ( (nRt = libssh2_sftp_realpath(	(LIBSSH2_SFTP*)_pSessionSFtp,
                          			cPath,
                          			cRealPath, sizeof(cRealPath))) == -1)
		{
			throw Exception("libssh2_sftp_realpath fail.");
		}
		cRealPath[ nRt ] = 0;
		LOG_WRITE("Real Path GetPwd :: %s", cRealPath);
	}
	catch(Exception& ex)
	{
		String	sMsg; 
		sMsg.Append("%s", (char*)ex);		
		MsgBox(_("Error"), sMsg.c_str());
		return "";
	}
	return cRealPath;
}

bool	SFtpReader::GetInfo(File &tFile)
{
	try
	{
		bool	bLink = false;
		
		//if (libssh2_sftp_fstat( (LIBSSH2_SFTP_HANDLE*)_pDir, &tFileAttributes) == -1)
		//	throw Exception("file reading fail. (GetInfo)");
		LIBSSH2_SFTP_ATTRIBUTES* pFileAttr = (LIBSSH2_SFTP_ATTRIBUTES*)_pFileAttr;

		if ( !pFileAttr ) 
			throw Exception( "first to sftp Init() fucntion."); 
	
		tFile.sType = _sInitTypeName;
	
		if (_sTmpFileName == ".") return false;

		if (_sTmpFileName == ".." )
		{
			if ( _sCurPath == "/" ) return false;
			tFile.sFullName = GetRealPath("..");
		}
		else
			tFile.sFullName = _sCurPath + _sTmpFileName;	

		if (pFileAttr->flags & LIBSSH2_SFTP_ATTR_SIZE)
			tFile.uSize = pFileAttr->filesize;

		// . 파일이 link 파일이면
		if (pFileAttr->flags & LIBSSH2_SFTP_ATTR_PERMISSIONS &&
			S_ISLNK( pFileAttr->permissions ) )
		{
			char cLinkPath[FILENAME_MAX];
			int  nRt = 0;
			memset(&cLinkPath, 0, sizeof(cLinkPath));

			LOG_WRITE("Link File :: _sTmpFileName [%s]", _sTmpFileName.c_str());
			if ((nRt = libssh2_sftp_readlink(	(LIBSSH2_SFTP*)_pSessionSFtp, 
										tFile.sFullName.c_str(),
             							cLinkPath, FILENAME_MAX)) > 0 )
			{
				string sLinkPath = cLinkPath;
				if (cLinkPath[0] == '/')
				{
					// .. 링크 대상인 진짜 파일 정보를 얻는다.
					if (libssh2_sftp_lstat((LIBSSH2_SFTP*)_pSessionSFtp, cLinkPath, pFileAttr) == -1)
						tFile.sName = ChgCurLocale( _sTmpFileName );
					else
					{
						tFile.sLinkName = cLinkPath;
						tFile.sName = ChgCurLocale( _sTmpFileName );
					}
				}
				else
				{
					sLinkPath = _sCurPath + sLinkPath;
					if (libssh2_sftp_lstat((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sLinkPath.c_str(), pFileAttr) == -1)
						tFile.sName = ChgCurLocale( _sTmpFileName );
					else
					{
						tFile.sLinkName = cLinkPath;
						tFile.sName = ChgCurLocale( _sTmpFileName );
					}
				}
				bLink = true;	
			}
			else
				LOG_WRITE("File Reading Failure (Link 1) [%s]", _sTmpFileName.c_str());
		}
		else
		{
			tFile.sName = ChgCurLocale(_sTmpFileName);
		}

		if (pFileAttr->flags & LIBSSH2_SFTP_ATTR_PERMISSIONS)
		{
			char	cAttr[10];
			
			if ( bLink )
				cAttr[0]= 'l';
			else
			{
				if ( S_IFDIR & pFileAttr->permissions )
				{
					cAttr[0]= 'd';
					tFile.bDir = true;
					if (tFile.sFullName.substr(tFile.sFullName.size()-1, 1) != "/") 
						tFile.sFullName += '/';
				}
				else
					cAttr[0]= '-';
			}

			cAttr[1]= S_IRUSR & pFileAttr->permissions ? 'r' : '-';
			cAttr[2]= S_IWUSR & pFileAttr->permissions ? 'w' : '-';
			cAttr[3]= S_IXUSR & pFileAttr->permissions ? 'x' : '-';
			if (S_ISUID & pFileAttr->permissions) cAttr[3] = 'S';
			if (S_ISUID & pFileAttr->permissions &&
				S_IXUSR & pFileAttr->permissions) cAttr[3] = 's';
			cAttr[4]= S_IRGRP & pFileAttr->permissions ? 'r' : '-';
			cAttr[5]= S_IWGRP & pFileAttr->permissions ? 'w' : '-';
			cAttr[6]= S_IXGRP & pFileAttr->permissions ? 'x' : '-';
			if (S_ISGID & pFileAttr->permissions) cAttr[6] = 'S';
			if (S_ISGID & pFileAttr->permissions &&
				S_IXGRP & pFileAttr->permissions) cAttr[6] = 's';
			cAttr[7]= S_IROTH & pFileAttr->permissions ? 'r' : '-';
			cAttr[8]= S_IWOTH & pFileAttr->permissions ? 'w' : '-';
			cAttr[9]= S_IXOTH & pFileAttr->permissions ? 'x' : '-';
			if (S_ISVTX & pFileAttr->permissions) cAttr[9] = 'S';
			if (S_ISVTX & pFileAttr->permissions &&
				S_IXUSR & pFileAttr->permissions) cAttr[9] = 's';
			cAttr[10] = 0;
			tFile.sAttr = cAttr;
		}
	
		char cDate[10];
		// . 파일 생성 날짜를 알아냄
		if (pFileAttr->flags & LIBSSH2_SFTP_ATTR_ACMODTIME)
		{
			tFile.tCreateTime = (time_t)pFileAttr->mtime;
			strftime(cDate, 9, "%y-%m-%d", localtime( (time_t*)&pFileAttr->mtime) );
			tFile.sDate = cDate;

			// . 파일 생성 시간을 알아냄
			strftime(cDate, 6, "%H:%M", localtime( (time_t*)&pFileAttr->mtime) );
			tFile.sTime = cDate;
		}
		
		/*
		LOG_WRITE("%10s %6s %6s %10d %s", tFile.sAttr.c_str(), 
											tFile.sDate.c_str(), 
											tFile.sTime.c_str(), 
											tFile.uSize, 
											tFile.sName.c_str());
		*/
	}
	catch(Exception& ex)
	{
		String	sMsg; string sErrMsg = GetLastErrMsg();
		sMsg.Append("%s", (char*)ex);		
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}
	return true;
}

bool	SFtpReader::Next()
{
	char	cFileName[2047];
	
	memset(_pFileAttr, 0, sizeof(LIBSSH2_SFTP_ATTRIBUTES));
	int nRt = libssh2_sftp_readdir((LIBSSH2_SFTP_HANDLE*)_pDir, cFileName, 2047, (LIBSSH2_SFTP_ATTRIBUTES*)_pFileAttr);
	
	if ( nRt > 0 )
	{
		cFileName[nRt] = '\0';
		_sTmpFileName = cFileName;
		return true;
	}

	if ( nRt == -1 )
	{
		String	sMsg; string sErrMsg = GetLastErrMsg();
		sMsg.Append(_("sftp file reading failure."));
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
	}
	return false;
}

bool	SFtpReader::Read(const string& sDir)
{
	if (_pSessionSFtp == NULL) return false;

	try
	{
		if ( _pDir )
		{
			libssh2_sftp_close_handle((LIBSSH2_SFTP_HANDLE*)_pDir);
			_pDir = NULL;
		}

		string	sDirTmp = GetRealPath( sDir );
		
		LIBSSH2_SFTP_HANDLE* pDir;
		pDir = libssh2_sftp_opendir( (LIBSSH2_SFTP*)_pSessionSFtp, (char*)sDirTmp.c_str() );
		
		if ( !pDir )
			throw Exception("SFtp open dir failed.");

		_pDir = (void*)pDir;
		_sCurPath = sDirTmp;
		if (_sCurPath.substr(_sCurPath.size()-1, 1) != "/") 
			_sCurPath = _sCurPath + '/';

		LOG_WRITE("SFtp Read :: %s", _sCurPath.c_str());
		//cout << "SFtp Read :: " << _sCurPath << endl;
	}
	catch(Exception& ex)
	{
		String	sMsg; string	sErrMsg;
		int		nRt = GetLastError( &sErrMsg );
		sMsg.Append(_("sftp file reading failure."));
		if ( !sErrMsg.empty() ) sMsg.Append("%s", sErrMsg.c_str() );		
		MsgBox(_("Error"), sMsg.c_str());
		
		if ( nRt == LIBSSH2_ERROR_SOCKET_TIMEOUT)
		{
			string sTmp = _sCurPath;
			Destroy();
			Init(_sInitFile);
			_sCurPath = sTmp;
		}
		return false;
	}
	return true;
}

bool	SFtpReader::Rename(File* pFile, const string& sRename)
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

	if ( libssh2_sftp_rename_ex((LIBSSH2_SFTP*)_pSessionSFtp, 
								(char*)pFile->sFullName.c_str(), 
								pFile->sFullName.size(),
								(char*)sRenameName.c_str(),
								sRenameName.size(), 
								LIBSSH2_SFTP_RENAME_OVERWRITE |
								LIBSSH2_SFTP_RENAME_ATOMIC |
								LIBSSH2_SFTP_RENAME_NATIVE) == -1)
	{
		String	sMsg; string	sErrMsg;
		int		nRt = GetLastError( &sErrMsg );
		sMsg.Append(_("SFtp rename failure !!! [%s]"), sRename.c_str());
		if ( !sErrMsg.empty() ) sMsg.Append("%s", sErrMsg.c_str() );
		MsgBox(_("Error"), sMsg.c_str());
		
		if ( nRt == LIBSSH2_ERROR_SOCKET_TIMEOUT)
		{
			string sTmp = _sCurPath;
			Destroy();
			Init(_sInitFile);
			_sCurPath = sTmp;
		}
		return false;
	}
	return true;
}

bool	SFtpReader::Mkdir(const string& sFullPathName)
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

	if ( libssh2_sftp_mkdir( (LIBSSH2_SFTP*)_pSessionSFtp, (char*)sMkdirName.c_str(), 0755) == -1)
	{
		String	sMsg; string	sErrMsg;
		int		nRt = GetLastError( &sErrMsg );
		sMsg.Append(_("SFtp mkdir failure !!! [%s]"), sMkdirName.c_str());
		if ( !sErrMsg.empty() ) sMsg.Append("%s", sErrMsg.c_str() );
		MsgBox(_("Error"), sMsg.c_str());
		
		if ( nRt == LIBSSH2_ERROR_SOCKET_TIMEOUT)
		{
			string sTmp = _sCurPath;
			Destroy();
			Init(_sInitFile);
			_sCurPath = sTmp;
		}
		return false;
	}
	return true;
}


bool SFtpReader::Copy(	Selection& tSelection, 		// remote
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

	int	nBufSize = g_tCfg.GetValueNum("SSH", "DefBufSize", 8196);

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

		LIBSSH2_SFTP_ATTRIBUTES 	tAttr;
		if (libssh2_sftp_stat((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sSourceName.c_str(), &tAttr) == -1)
		{
			// 소스가 없다니.. 참
			String	sMsg; string sErrMsg = GetLastErrMsg();
			sMsg.Append(_("File access error (%s), continue? !!! "), pFile->sName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
			tProgress.Start();
			continue;
		}

		LOG_WRITE("SFtpReader Copy sTargetName 1 [%s] [%s] [%s] [%d]", 
					sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str(), tAttr.filesize);
		
		String	sCount, sCount2;
		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), sSize.c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr2(sCount.c_str());
		
		if (stat(sTargetName.c_str(), &tar_stat)!=-1)
		{
			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain_sftp_copy:
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
						uPsize += tAttr.filesize;
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
												sRename) < 0) goto askagain_sftp_copy;
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
						uPsize += tAttr.filesize;
						uViewCount++;
						continue;
					
					case -1:
					case 5: // cancel
						goto halt_sftp_copy;
							
					default:
						break;
				}
			}
		}
		
		if (pFile->bDir)
		{
			if (pSelection != NULL)
			{
				File	tTargetFile = *pFile;
				tTargetFile.sFullName = sTargetName;
				pSelection->Add(&tTargetFile);
			}

			if (mkdir(sTargetName.c_str(), tAttr.permissions)==-1) // directory make.
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
		// copy operation 
		
		uViewCount++;
		
		LIBSSH2_SFTP_HANDLE*		pFtpFileHandle = NULL;
		
		pFtpFileHandle = libssh2_sftp_open( (LIBSSH2_SFTP*)_pSessionSFtp, (char*)sSourceName.c_str(), 
												LIBSSH2_FXF_READ, 0644);

		if (pFtpFileHandle == NULL)
		{
			String	sMsg; string sErrMsg = GetLastErrMsg();
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
						libssh2_sftp_close_handle(pFtpFileHandle);
						remove( sTargetName.c_str() );
						goto halt_sftp_copy;
					}
					tProgress.Start();
				}	
				
				if (tAttr.filesize <= (libssh2_uint64_t)uCsize ) break;
						
				if ((uLastSize = libssh2_sftp_read(pFtpFileHandle, buf, sizeof(buf))) < 0)
				{
					String	sMsg; string sErrMsg  = GetLastErrMsg();
					sMsg.Append(_("remote file access error (%s), continue? !!! "), pFile->sFullName.c_str());
					if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
					tProgress.End();
					if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
					fclose(out);
					libssh2_sftp_close_handle(pFtpFileHandle);
					remove( sTargetName.c_str() );
					goto halt_sftp_copy;
				}
				
				fwrite(buf, 1, uLastSize, out);
	
				uCsize +=uLastSize;
				uPsize +=uLastSize;

				// libssh2_sftp_seek(pFtpFileHandle, uCsize);
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
			libssh2_sftp_close_handle(pFtpFileHandle);
			tProgress.End();
			if (YNBox(_("Error"), _("local file access error, continue?"), false) == false) 	break;
			tProgress.Start();
			continue;
		}

		fclose(out);

		chmod(sTargetName.c_str(), tAttr.permissions);
		libssh2_sftp_close_handle(pFtpFileHandle);
		
		if (pSelection != NULL)
		{
			File	tTargetFile = *pFile;
			tTargetFile.sFullName = sTargetName;
			pSelection->Add(&tTargetFile);
		}
	}
halt_sftp_copy:
	tProgress.End();
	if (pSelection)
		pSelection->SetSelectPath(sTargetPathTmp2);

	LOG_WRITE("DirReader Copy End");
	return true;
}

bool	SFtpReader::Paste(Selection& tSelection)
{
	string			sCurName, sTargetPathTmp, sTargetPathTmp2;
	string			sSourceName, sTargetName;
	vector<File*>	vFiles;
	struct stat 	src_stat;

	int		nBufSize = g_tCfg.GetValueNum("SSH", "DefBufSize", 8196);
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
			if ( libssh2_sftp_mkdir( (LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str(), 0755) == -1)
			{
				String	sMsg; string sErrMsg = GetLastErrMsg();
				sMsg.Append(_("SFtp Directory exists, continue !!! [%s]"), sTargetName.c_str());
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
		
		mode_t permission = src_stat.st_mode;

		// 파일 리스트가 디렉토리 라면
		if (pFile->bDir) continue; 

		String	sCount, sCount2;
		sCount.Printf("%s/%s", toregular(uViewCount).c_str(), sSize.c_str());

		tProgress.setLeftStr(pFile->sName.c_str());
		tProgress.setRightStr2(sCount.c_str());

		LIBSSH2_SFTP_ATTRIBUTES 	tAttrs;
		
		if (libssh2_sftp_stat((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str(), &tAttrs) != -1)
		{
			if (bSkipAll) continue;
			else if (bOverwriteAll == false)
			{// 파일이 있을 경우..
askagain_sftp_paste:
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
								if (InputBox(	_("Rename"), sRename) < 0) goto askagain_sftp_paste;
								tProgress.Start();
	
								sTargetName = sTargetPathTmp2 + sRename;
								
								if ( libssh2_sftp_rename_ex( 	(LIBSSH2_SFTP*)_pSessionSFtp, 
									(char*)pFile->sFullName.c_str(), 
									pFile->sFullName.size(),
									(char*)sTargetName.c_str(),
									sTargetName.size(), 
									0644) == -1)
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
						goto halt_sftp_paste;
							
					default:
						break;
				}
			}
		}
		
		uCsize=0;	
		// 실제 복사 오퍼레이션	
		
		uViewCount++;

		LIBSSH2_SFTP_HANDLE*		pFtpOutFileHandle = NULL;
		
		pFtpOutFileHandle = libssh2_sftp_open( (LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str(), 
												LIBSSH2_FXF_READ | LIBSSH2_FXF_WRITE | LIBSSH2_FXF_TRUNC | LIBSSH2_FXF_CREAT ,
												0644);

		if (pFtpOutFileHandle == NULL)
		{
			String	sMsg; string sErrMsg = GetLastErrMsg();
			sMsg.Append("File write access error (%s), continue? !!! ", sTargetName.c_str());
			if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
			tProgress.End();
			if (YNBox(_("Error"), sMsg.c_str(), false) == false) break;
			tProgress.Start();
			continue;
		}

		FILE*	fp = fopen(sSourceName.c_str(), "rb");
		
		LOG_WRITE("TEST1");

		if ( fp )
		{
			while( !feof(fp) )
			{
				if (tProgress.isExit())
				{
					tProgress.End();
					if (YNBox(_("Warning"), _("Do you want to stop copy operation?"), false)==true)
					{
						libssh2_sftp_close_handle(pFtpOutFileHandle);
						fclose(fp);
						libssh2_sftp_unlink((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str());
						goto halt_sftp_paste;
					}
					tProgress.Start();
				}				
				
				uLastSize = fread(buf, 1, sizeof(buf), fp);
				
				int nSize = 0, nSizePlus = 0;
				do
				{
					if (tProgress.isExit()) break;
						
					nSize = 0;
					if ((nSize = libssh2_sftp_write(pFtpOutFileHandle, buf+nSizePlus, uLastSize-nSizePlus)) == -1)
					{
						tProgress.End();
						String		sMsg; string sErrMsg = GetLastErrMsg();
						sMsg.Append(_("file writing failure. continue?"));
						if (!sErrMsg.empty()) sMsg.Append(" [%s]", sErrMsg.c_str()); 						
						if (YNBox(_("Error"), sMsg.c_str() , false)==false)
						{
							libssh2_sftp_close_handle(pFtpOutFileHandle);
							fclose(fp);
							libssh2_sftp_unlink((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str());
							goto halt_sftp_paste;
						}
						tProgress.Start();
						break;
					}
					
					uCsize += nSize; uPsize += nSize; nSizePlus += nSize;
				}
				while( nSizePlus < uLastSize );
				
				if (tProgress.isExit()) continue;
				
				//libssh2_sftp_seek(pFtpOutFileHandle, uCsize);
				
				sCount2.Printf("%s/%s", 	toregular(uPsize).c_str(),
											toregular(uFileSize).c_str());
				
				tProgress.setRightStr(sCount2.c_str());
				if (src_stat.st_size && uCsize && uPsize && uFileSize)
					tProgress.setCount(	uCsize * 100 / src_stat.st_size,
										uPsize * 100 / uFileSize );
			}
		}
		else
		{
			tProgress.End();
			if (YNBox(_("Error"), _("File access error, continue?"), false) == false) break;
			tProgress.Start();
			continue;
		}
		
		LIBSSH2_SFTP_ATTRIBUTES	tAttrOut;
		libssh2_sftp_fstat(pFtpOutFileHandle, &tAttrOut);
		tAttrOut.permissions = permission;
		libssh2_sftp_fsetstat(pFtpOutFileHandle, &tAttrOut);
		libssh2_sftp_close_handle(pFtpOutFileHandle);
		fclose(fp);
	}
halt_sftp_paste:
	tProgress.End();
	return true;
}

bool SFtpReader::Remove(MLS::Selection& tSelection, bool bMsgShow, bool bIgnore)
{
	CommonProgress tProgress(_("sftp delete file"), "", true);
	if (bMsgShow)
	{
		if (YNBox(_("Info"), _("sftp delete file ?"), false) == false) return 0;
		
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
		LOG_WRITE("sftp file remove [%s]", sTargetName.c_str());
		
		if (libssh2_sftp_unlink((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str())==-1)
		{
			String	sMsg; string sErrMsg = GetLastErrMsg();
			sMsg.Append(_("sftp file remove failure : %s : continue ? "), pFile->sName.c_str());
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
		LOG_WRITE("sftp dir remove [%s]", sTargetName.c_str());
		if (libssh2_sftp_rmdir((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sTargetName.c_str())==-1)
		{
			String	sMsg; string sErrMsg = GetLastErrMsg();
			sMsg.Append(_("sftp dir remove failure : %s : %s. continue ? "), pFile->sName.c_str());
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
bool  SFtpReader::View(const File* pFileOriginal, File* pFileChange)
{ 
	string	sSourceName, sTargetName;
	LIBSSH2_SFTP_ATTRIBUTES 	tAttr;

	int		nBufSize = g_tCfg.GetValueNum("SSH", "DefBufSize", 8196);
	char	buf[nBufSize];
	 
	// 파일 이름이 없을경우 continue 
	if (!pFileOriginal) return false;

	// 소스 이름과 타겟 이름 정하기	
	sSourceName = pFileOriginal->sFullName;
	sTargetName = _sTmpDir + pFileOriginal->sName;

	LOG_WRITE("SFtpReader::View [%s] [%s]", sSourceName.c_str(), sTargetName.c_str());

	if (libssh2_sftp_stat((LIBSSH2_SFTP*)_pSessionSFtp, (char*)sSourceName.c_str(), &tAttr) == -1)
	{
		String	sMsg; string sErrMsg = GetLastErrMsg();
		sMsg.Append(_("File access error (%s), continue? !!!"), sSourceName.c_str());
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}

	LOG_WRITE("SFtpReader View sTargetName 1 [%s] [%s] [%s] [%d]", 
				sSourceName.c_str(), sTargetName.c_str(), _sCurPath.c_str(), tAttr.filesize);
	
	ullong	uFileSize = pFileOriginal->uSize;
	string 	sRestSize;

	ullong	uLastSize = 0, uCsize = 0;

	CommonProgress tProgress(_("sftp view file"), "");

	tProgress.setLeftStr(pFileOriginal->sName.c_str());
	tProgress.Start();

	LIBSSH2_SFTP_HANDLE*		pFtpFileHandle = NULL;
	
	pFtpFileHandle = libssh2_sftp_open( (LIBSSH2_SFTP*)_pSessionSFtp, (char*)sSourceName.c_str(), 
											LIBSSH2_FXF_READ, 0644);

	if (pFtpFileHandle == NULL)
	{
		String	sMsg; string sErrMsg = GetLastErrMsg();
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
					libssh2_sftp_close_handle(pFtpFileHandle);
					remove( sTargetName.c_str() );
					return false;
				}
				tProgress.Start();
			}
			
			if (tAttr.filesize <= (libssh2_uint64_t)uCsize ) break;
			
			uLastSize = libssh2_sftp_read(pFtpFileHandle, buf, sizeof(buf));

			if ( uLastSize == -1 )
			{
				libssh2_sftp_close_handle(pFtpFileHandle);
				tProgress.End();

				String	sMsg; string sErrMsg = GetLastErrMsg();
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
							toregular(tAttr.filesize).c_str());

			tProgress.setRightStr(sCount.c_str());
			tProgress.setCount( uCsize * 100 / uFileSize );
		}
	}
	else
	{
		libssh2_sftp_close_handle(pFtpFileHandle);
		tProgress.End();
		MsgBox(_("Error"), _("local file access error !!!"));
		return false;
	}

	fclose(out);
	libssh2_sftp_close_handle(pFtpFileHandle);

	tProgress.End();
	*pFileChange = *pFileOriginal;
	pFileChange->sTmp2 = pFileOriginal->sFullName;
	pFileChange->sFullName = sTargetName;
	pFileChange->sName = pFileOriginal->sName;
	return true;
}

bool 	SFtpReader::isChkFile(const File& tFile)
{
	LIBSSH2_SFTP_ATTRIBUTES 	tAttr;
	if (libssh2_sftp_stat((LIBSSH2_SFTP*)_pSessionSFtp, (char*)tFile.sFullName.c_str(), &tAttr) == -1)
	{
		String	sMsg; string sErrMsg = GetLastErrMsg();
		sMsg.Append(_("File access error (%s), continue? !!!"), tFile.sFullName.c_str());
		if (!sErrMsg.empty()) sMsg.Append("[%s]", sErrMsg.c_str());
		MsgBox(_("Error"), sMsg.c_str());
		return false;
	}
	return true;
}

void	SFtpReader::EncodeChk(vector<File*>& tFileList, bool bEncChk)
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

string SFtpReader::GetViewPath( ) const
{
	return ChgCurLocale( _sViewPath );
}

