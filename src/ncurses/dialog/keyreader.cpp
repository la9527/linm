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

#include "keyreader.h"

using namespace MLS;

KeyInfo		KeyReader::Read(WINDOW*	pWin, bool bDelay)
{
	int nKey1 = ERR, nKey2 = ERR;

	if (!bDelay)
		nodelay(pWin, FALSE);
	else
		nodelay(pWin, TRUE);

	this->KeyInfoReload();
	
	vector<int>		vKey;
	nKey1 = wgetch(pWin);

	vKey.push_back(nKey1);

	// 8, 263 is koran hangul the first syllable.
	if ((nKey1 == 8 || nKey1 == 27 || nKey1 > 0x80) 
		&& nKey1 != KEY_MOUSE && nKey1 != ERR && nKey1 != KEY_RESIZE)
	{
		nodelay(pWin, TRUE);
		for (;;)
		{
			nKey2 = wgetch(pWin);
			if (nKey2 == ERR || nKey2 == nKey1 ) break;
			if (nKey2 == 8 )
			{
				ungetch(8);
				break; // konsole scim korean input fix.
			}
			vKey.push_back(nKey2);
		}
		nodelay(pWin, FALSE);
	}

	String sView;
	for (int n = 0; n < (int)vKey.size(); n++)
	{
		sView.Append("Key%d [%d] ", n, vKey[n]);
	}
	LOG(sView.c_str());

	KeyInfo		tKeyInfo(vKey);
	if (vKey.size() == 1 && vKey[0] == 27)
	{
		tKeyInfo = (int)KEY_ESC;
	}

	int nKey = NameToDef(tKeyInfo); // get key number.
	if (nKey != ERR) 
	{
		tKeyInfo = (int)nKey;
	}

	//LOG("Output KeyNum [%d] KeyListSize [%d]", nKey, vKey.size());
	return tKeyInfo;
}

int KeyReader::NameToDef(KeyInfo& tKeyInfo)
{
	string sKeyName;
	for (int n = 0; n < (int)_vKeyList.size(); n++)
	{
		KeyInfo		tKeyInfo2 = _vKeyList[n];
		if (tKeyInfo2 == tKeyInfo)
		{
			sKeyName = _vKeyList[n].sKeyName;
			tKeyInfo.sKeyName = sKeyName;
			return (int)KeynameToDefine(sKeyName);
		}
	}

	return (int)ERR;
}

KeyInfo KeyReader::KeynameToDefine(const string& sKeyName)
{
	int		nKeyInfo = ERR;

	string		sUppKeyName;
	sUppKeyName = Toupper(sKeyName);

	if (sUppKeyName == "F1") 		nKeyInfo = (int)KEY_F(1);
	if (sUppKeyName == "F2") 		nKeyInfo = (int)KEY_F(2);
	if (sUppKeyName == "F3") 		nKeyInfo = (int)KEY_F(3);
	if (sUppKeyName == "F4") 		nKeyInfo = (int)KEY_F(4);
	if (sUppKeyName == "F5")		nKeyInfo = (int)KEY_F(5);
	if (sUppKeyName == "F6")		nKeyInfo = (int)KEY_F(6);
	if (sUppKeyName == "F7")		nKeyInfo = (int)KEY_F(7);
	if (sUppKeyName == "F8")		nKeyInfo = (int)KEY_F(8);
	if (sUppKeyName == "F9")		nKeyInfo = (int)KEY_F(9);
	if (sUppKeyName == "F10")		nKeyInfo = (int)KEY_F(10);
	if (sUppKeyName == "F11")		nKeyInfo = (int)KEY_F(11);
	if (sUppKeyName == "F12")		nKeyInfo = (int)KEY_F(12);

	if (sUppKeyName == "DOWN") 		nKeyInfo = (int)KEY_DOWN;
	if (sUppKeyName == "UP") 		nKeyInfo = (int)KEY_UP;
	if (sUppKeyName == "LEFT") 		nKeyInfo = (int)KEY_LEFT;
	if (sUppKeyName == "RIGHT") 	nKeyInfo = (int)KEY_RIGHT;

	// Shift + allow
	if (sUppKeyName == "SLEFT")		nKeyInfo = (int)KEY_SLEFT; 
	if (sUppKeyName == "SRIGHT")	nKeyInfo = (int)KEY_SRIGHT;
	if (sUppKeyName == "SUP")		nKeyInfo = (int)KEY_SUP;
	if (sUppKeyName == "SDOWN")		nKeyInfo = (int)KEY_SDOWN;

	// Ctrl + allow
	if (sUppKeyName == "CLEFT")		nKeyInfo = (int)KEY_CLEFT;
	if (sUppKeyName == "CRIGHT")	nKeyInfo = (int)KEY_CRIGHT;
	if (sUppKeyName == "CUP")		nKeyInfo = (int)KEY_CUP;
	if (sUppKeyName == "CDOWN")		nKeyInfo = (int)KEY_CDOWN;

	if (sUppKeyName == "HOME") 		nKeyInfo = (int)KEY_HOME;
	if (sUppKeyName == "END") 		nKeyInfo = (int)KEY_END;
	if (sUppKeyName == "INSERT") 	nKeyInfo = (int)KEY_IC;
	if (sUppKeyName == "DEL") 		nKeyInfo = (int)KEY_DC;
	if (sUppKeyName == "PGUP")		nKeyInfo = (int)KEY_PPAGE;
	if (sUppKeyName == "PGDN")		nKeyInfo = (int)KEY_NPAGE;

	if (sUppKeyName == "BS") 		nKeyInfo = (int)KEY_BACKSPACE;
	if (sUppKeyName == "ENTER")		nKeyInfo = (int)KEY_ENTER;
	if (sUppKeyName == "REFRESH")	nKeyInfo = (int)KEY_REFRESH;
	if (sUppKeyName == "EXIT")		nKeyInfo = (int)KEY_EXIT;
	if (sUppKeyName == "TAB")		nKeyInfo = (int)KEY_TAB;
	if (sUppKeyName == "BTAB")		nKeyInfo = (int)KEY_BTAB;
	if (sUppKeyName == "ESC")		nKeyInfo = (int)KEY_ESC;
	if (sUppKeyName == "ENTER")		nKeyInfo = (int)KEY_ENTER;
	return nKeyInfo;
}
