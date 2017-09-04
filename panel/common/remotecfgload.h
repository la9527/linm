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

#ifndef __REMOTECFGLOAD_H__
#define __REMOTECFGLOAD_H__

#include "define.h"
#include "strutil.h"
#include "configure.h"

using namespace MLSUTIL;

namespace MLS
{

class RemoteConnEntry
{
public:
	RemoteConnEntry(const string& s1 = "",
					const string& s2 = "",
					const string& s3 = "",
					const string& s4 = "",
					const string& s5 = "",
					const string& s6 = "",
					const string& s7 = "")
		: sName(s1), sProtocol(s2), sAddress(s3), sPort(s4), sDirectory(s5), 
		  sUserName(s6), sPassword(s7), bKeyFile(false), bModify(false)
	{
	}
	
	void	Clear();
	string	GetURLFormat(bool bPasswd);
	
	string		sName;
	string		sProtocol;	
	string		sAddress;
	string		sPort;
	string		sDirectory;
	string		sUserName;
	string		sPassword;
	string		sPrivateKey;
	string		sPublicKey;

	bool		bKeyFile;
	bool		bModify;
};

class RemoteCfgLoad:public Configure
{
public:
	RemoteCfgLoad() : Configure()  {}
	~RemoteCfgLoad() {}

	vector<RemoteConnEntry>			_vConnInfo;
	
	void	Init();
	bool	GetRemoteConnEntry(const string& sName, RemoteConnEntry& tRemote);

protected:
	bool	Parsing(const string& section, const string& var, const string& val);
	bool	SaveParcing();
};

};

#endif

