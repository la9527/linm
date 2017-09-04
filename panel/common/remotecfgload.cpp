#include "remotecfgload.h"
#include "passencrypt.h"

using namespace MLSUTIL;
using namespace MLS;

void	RemoteConnEntry::Clear()
{
	sName = ""; sProtocol = ""; sAddress = ""; sPort = ""; sUserName = ""; 
	sPassword = ""; sPrivateKey = ""; sPublicKey = "";
	bModify = false;
}

string	RemoteConnEntry::GetURLFormat(bool bPasswd)
{
	string	sURLFormat;

	if (bPasswd && sPassword.size() > 0)
	{
		if (Tolower(sProtocol) == "sftp")
			sURLFormat = "sftp://" + sUserName + ":" + sPassword + "@" + sAddress + ":" + sPort + sDirectory;
		else if (Tolower(sProtocol) == "ftp")
			sURLFormat = "ftp://" + sUserName  + ":" + sPassword + "@" + sAddress + ":" + sPort + sDirectory;
		else if (Tolower(sProtocol) == "smb")
			sURLFormat = "smb://" + sAddress + sDirectory;
	}
	else
	{
		if (Tolower(sProtocol) == "sftp")
			sURLFormat = "sftp://" + sUserName + "@" + sAddress + ":" + sPort + sDirectory;
		else if (Tolower(sProtocol) == "ftp")
			sURLFormat = "ftp://" + sUserName  + "@" + sAddress + ":" + sPort + sDirectory;
		else if (Tolower(sProtocol) == "smb")
			sURLFormat = "smb://" + sAddress + sDirectory;
	}

	if (sURLFormat.size() && (sPublicKey.size() || sPrivateKey.size()))
	{
		sURLFormat = sURLFormat + "?" + sPublicKey + ":" + sPrivateKey;
	}
	return sURLFormat;
}

void	RemoteCfgLoad::Init()
{
	_vConnInfo.clear();
}

bool 	RemoteCfgLoad::GetRemoteConnEntry(const string& sName, RemoteConnEntry&		tConn)
{
	for (int n = 0; n < (int)_vConnInfo.size(); n++)
	{
		if (_vConnInfo[n].sName == sName)
		{
			tConn = _vConnInfo[n];
			return true;
		}
	}
	return false;
}

bool	RemoteCfgLoad::Parsing(const string& section, const string& var, const string& val)
{
	for (int n = 0; n < (int)_vConnInfo.size(); n++)
	{
		if (_vConnInfo[n].sName == section)
		{
			if (var == "Protocol")	_vConnInfo[n].sProtocol = val;
			if (var == "Address")	_vConnInfo[n].sAddress = val;
			if (var == "Port")		_vConnInfo[n].sPort = val;
			if (var == "Directory")	_vConnInfo[n].sDirectory = val;
			if (var == "UserName")	_vConnInfo[n].sUserName = val;
			if (var == "KeyFile")		_vConnInfo[n].bKeyFile = (Tolower(val) == "on") ? true : false;
			if (var == "Password")
			{
				string			sPasswd;
				if (val != "")
				{
					PasswdCrypt		tCrypt;
					tCrypt.Decrypt(val, _vConnInfo[n].sPassword);
				}
			}
			if (var == "PrivateKey")	_vConnInfo[n].sPrivateKey = val;
			if (var == "PublicKey")		_vConnInfo[n].sPublicKey = val;
			return false;
		}
	}

	RemoteConnEntry		tEntry;

	tEntry.sName = section;	
	if (var == "Protocol")		tEntry.sProtocol = val;
	if (var == "Address")		tEntry.sAddress = val;
	if (var == "Port")			tEntry.sPort = val;
	if (var == "Directory")		tEntry.sDirectory = val;
	if (var == "UserName")		tEntry.sUserName = val;
	if (var == "Password")	
	{
		string			sPasswd;
		if (val != "")
		{
			PasswdCrypt		tCrypt;
			tCrypt.Decrypt(val, tEntry.sPassword);
		}
	}
	if (var == "KeyFile")		tEntry.bKeyFile = (Tolower(val) == "on") ? true : false;
	if (var == "PrivateKey")	tEntry.sPrivateKey = val;
	if (var == "PublicKey")		tEntry.sPublicKey = val;
	
	_vConnInfo.push_back( tEntry );
	return false;
}


bool	RemoteCfgLoad::SaveParcing()
{
	string			sPasswd;
	PasswdCrypt		tCrypt;

	_EnvMap.clear();
	
	for (int n = 0; n < (int)_vConnInfo.size(); n++)
	{
		SetValue(_vConnInfo[n].sName, "Protocol", 	_vConnInfo[n].sProtocol, _vConnInfo[n].bModify);
		SetValue(_vConnInfo[n].sName, "Address",	_vConnInfo[n].sAddress, _vConnInfo[n].bModify);
		SetValue(_vConnInfo[n].sName, "Port", 		_vConnInfo[n].sPort, _vConnInfo[n].bModify);
		SetValue(_vConnInfo[n].sName, "Directory",	_vConnInfo[n].sDirectory, _vConnInfo[n].bModify);
		SetValue(_vConnInfo[n].sName, "UserName",	_vConnInfo[n].sUserName, _vConnInfo[n].bModify);
		SetValue(_vConnInfo[n].sName, "KeyFile",	_vConnInfo[n].bKeyFile ? "On" : "Off", _vConnInfo[n].bModify);
		if (_vConnInfo[n].bKeyFile)
		{
			SetValue(_vConnInfo[n].sName, "Password",	"", _vConnInfo[n].bModify);
			SetValue(_vConnInfo[n].sName, "PrivateKey",	_vConnInfo[n].sPrivateKey, _vConnInfo[n].bModify);
			SetValue(_vConnInfo[n].sName, "PublicKey",	_vConnInfo[n].sPublicKey, _vConnInfo[n].bModify);
		}
		else
		{
			sPasswd = "";
			tCrypt.Encrypt(_vConnInfo[n].sPassword, sPasswd);
			SetValue(_vConnInfo[n].sName, "Password",	sPasswd, _vConnInfo[n].bModify);
			SetValue(_vConnInfo[n].sName, "PrivateKey",	"", _vConnInfo[n].bModify);
			SetValue(_vConnInfo[n].sName, "PublicKey",	"", _vConnInfo[n].bModify);
		}
	}
	return true;
}
