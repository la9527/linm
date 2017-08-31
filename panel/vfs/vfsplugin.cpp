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

#include "vfsplugin.h"

#include "dirreader.h"
#include "arcreader.h"
#include "ftpreader.h"		// ftp plugin

#ifdef __LINM_SFTP_USE__
#include "SFtpReader.h"		// sftp plugin
#endif

#ifdef __LINM_SAMBA_USE__
#include "SMBReader.h"		// samba plugin
#endif

using namespace MLS;

void PluginLoader(ReaderCtl *pReaderCtl) {
    DirReader *pDirReader = new DirReader;
    pReaderCtl->Insert(dynamic_cast<Reader *>(pDirReader));

    ArcReader *pArcReader = new ArcReader;
    pReaderCtl->Insert(dynamic_cast<Reader *>(pArcReader));

    FtpReader *pFtpReader = new FtpReader;
    pReaderCtl->Insert(dynamic_cast<Reader *>(pFtpReader));

#ifdef __LINM_SFTP_USE__
    SFtpReader*	pSFtpReader = new SFtpReader;
    pReaderCtl->Insert(dynamic_cast<Reader*>(pSFtpReader));
#endif

#ifdef __LINM_SAMBA_USE__
    SMBReader*	pSMBReader = new SMBReader;
    pReaderCtl->Insert(dynamic_cast<Reader*>(pSMBReader));
#endif

}
