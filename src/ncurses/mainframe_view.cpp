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

#include "mainframe_view.h"
#include <deque>

using namespace MLSUTIL;
using namespace MLS;

void	DrawTop::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();
	ColorEntry tColorFuncA = g_tColorCfg.GetColorEntry("FuncA");
	ColorEntry tColorFunc  = g_tColorCfg.GetColorEntry("Func");

	setcol(tColorFunc, pWin);
	wmove(pWin, y, x);
	whline(pWin, ' ', width);
	
	int nStartFuncNum = 2;
	int nEndFuncNum = 9;

	if (width > 100)
	{
		nStartFuncNum = 1;
		nEndFuncNum = 12;
	}
	
	// Funtion panel draw
	for (int i=0; i< nEndFuncNum-nStartFuncNum+1; i++)
	{
		int pos = GetColumnIndex(width, nEndFuncNum-nStartFuncNum+1, i);

		setcol(tColorFuncA, pWin);

		String	sStr;
		if (i == 0)
		{
			sStr.Append("F%d", i+nStartFuncNum);
			mvwprintw(pWin, y, x+pos, "%s", sStr.c_str());
		}
		else
		{
			sStr.Append("%d", i+nStartFuncNum);
			mvwprintw(pWin, y, x+pos+1, "%s", sStr.c_str());
			sStr.clear();
			sStr.Append("F%d", i+nStartFuncNum);
		}

		setcol(tColorFunc, pWin);
		
		string	sCmd = g_tKeyCfg.GetCommand( sStr.c_str(), _eViewType );
		sCmd = g_tKeyCfg.GetHelp( sCmd, _eViewType );
		if ( sCmd.size() > 0)
			mvwprintw(pWin, y, x+sStr.size()+pos, "%s", _(sCmd.c_str()));
	}
	
	setcol(COLOR_BLACK, tColorFunc.back, pWin);
	for (int i=1; i< nEndFuncNum-nStartFuncNum+1;i++)
	{
		int pos = GetColumnIndex(width, nEndFuncNum-nStartFuncNum+1, i);
		mvwaddch(pWin, y, x+pos, VLINE);
	}
	wnoutrefresh(pWin);
}


int DrawTop::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (bstate & BUTTON1_DOUBLE_CLICKED) 
	{
		WINDOW*	pWin = _pForm->GetWin();

		int nStartFuncNum = 2;
		int nEndFuncNum = 9;

		if (width > 100)
		{
			nStartFuncNum = 1;
			nEndFuncNum = 12;
		}

		// Funtion panel draw
		for (int i=0; i< nEndFuncNum-nStartFuncNum+1; i++)
		{
			int index = GetIndex(width, nEndFuncNum-nStartFuncNum+1, X);
			return index+nStartFuncNum;
		}
	}
	return -1;
}


void	DrawPath::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();
	if (pWin == NULL) return;

	////////////////// . PATH Draw
	ColorEntry tColorStat  = g_tColorCfg.GetColorEntry("Stat");
	setcol(tColorStat, pWin);
	wmove(pWin, y, x); 	
	whline(pWin, ' ', width);

	string		sStr = _sStr;
	if (scrstrlen(sStr) > width)
	{
		sStr = scrstrncpy(sStr, scrstrlen(sStr)-width+5, width-5);
		sStr = "..." + sStr;
	}
	
	mvwprintw(pWin, y, x, "%s", sStr.c_str());
	wnoutrefresh(pWin);
}

void	Hint::SetHintData(std::vector<HintData>&	vHints)
{
	vHints.clear();

	vHints.push_back( HintData( _("Quit"), 		g_tKeyCfg.CmdToKeyName("Cmd_Quit", _eViewType)));
	vHints.push_back( HintData( _("Menu"), 		g_tKeyCfg.CmdToKeyName("Cmd_Menu", _eViewType)));
	vHints.push_back( HintData( _("Split"), 	g_tKeyCfg.CmdToKeyName("Cmd_Split", _eViewType)));
	vHints.push_back( HintData( _("Shell"), 	g_tKeyCfg.CmdToKeyName("Cmd_Shell", _eViewType)));
	vHints.push_back( HintData( _("MCD"),  		g_tKeyCfg.CmdToKeyName("Cmd_MCD", _eViewType)));
	vHints.push_back( HintData( _("QCD") ,		g_tKeyCfg.CmdToKeyName("Cmd_QCD", _eViewType)));	
	vHints.push_back( HintData( _("Help"), 		g_tKeyCfg.CmdToKeyName("Cmd_Help", _eViewType)));
	
	switch( _eViewType )
	{
		case PANEL:
			vHints.push_back( HintData( _("ClipCopy"), 	g_tKeyCfg.CmdToKeyName("Cmd_ClipCopy", _eViewType)));
			vHints.push_back( HintData( _("ClipCut"), 	g_tKeyCfg.CmdToKeyName("Cmd_ClipCut", _eViewType)));
			vHints.push_back( HintData( _("ClipPaste"), g_tKeyCfg.CmdToKeyName("Cmd_ClipPaste", _eViewType)));
			vHints.push_back( HintData( _("Select"),
										g_tKeyCfg.CmdToKeyName("Cmd_Select", _eViewType)));
			vHints.push_back( HintData( _("RemoteConnect"),
										g_tKeyCfg.CmdToKeyName("Cmd_RemoteConnProps", _eViewType)));
			break;

		case MCD:
			vHints.push_back( HintData( _("SubDirOneSearch"),
										g_tKeyCfg.CmdToKeyName("Cmd_Mcd_SubDirOneSearch", _eViewType)));
			vHints.push_back( HintData( _("SubDirAllSearch"),
										g_tKeyCfg.CmdToKeyName("Cmd_Mcd_SubDirAllSearch", _eViewType)));
			vHints.push_back( HintData( _("SubDirClear"),
										g_tKeyCfg.CmdToKeyName("Cmd_Mcd_SubDirClear", _eViewType)));
			break;

		case EDITOR:
			vHints.push_back( HintData( _("ClipCopy"), 	g_tKeyCfg.CmdToKeyName("Cmd_ClipCopy", _eViewType)));
			vHints.push_back( HintData( _("ClipCut"), 	g_tKeyCfg.CmdToKeyName("Cmd_ClipCut", _eViewType)));
			vHints.push_back( HintData( _("ClipPaste"), g_tKeyCfg.CmdToKeyName("Cmd_ClipPaste", _eViewType)));
			vHints.push_back( HintData( _("SelectMode"),
										g_tKeyCfg.CmdToKeyName("Cmd_SelectMode", _eViewType)));
			break;
		default:
			break;
	}
}

void	Hint::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();
	
	//////////////////. Hint Info
	std::vector<HintData> 	vHints;
	std::vector<HintData>::iterator	 i;
	
	setcol(g_tColorCfg._DefaultColor, pWin);
	wmove(pWin, y, x);
	whline(pWin, ' ', width);
	wnoutrefresh(pWin);	

	if ( _eClip == CLIP_NONE )
	{
		switch ( _eMcdExeMode )
		{
			case MCD_EXEMODE_NONE:
			{
				int		pos = 0;
				SetHintData( vHints );
				mvwprintw(pWin, y, 0, _("Hint: "));
				for (i = vHints.begin(); i!=vHints.end(); i++)
				{
					if (i->sValue != "" && width-15 > pos+6 )
					{
						setcol(COLOR_CYAN, g_tColorCfg._DefaultColor.back, pWin);
						mvwprintw(pWin, y, 6+pos+x, "%s", i->sValue.c_str());
						pos = pos + scrstrlen(i->sValue);
						setcol(g_tColorCfg._DefaultColor, pWin);
						mvwprintw(pWin, y, 6+pos+x, "-");
						setcol(g_tColorCfg._DefaultColor, pWin);
						pos = pos + 1;					
						mvwprintw(pWin, y, 6+pos+x, "%s ", i->sData.c_str());
						pos = pos + scrstrlen(i->sData)+1;
					}
				}
				break;
			}

			case ARCHIVE_COPY:
			{
				setcol(COLOR_CYAN, g_tColorCfg._DefaultColor.back, pWin);
				mvwprintw(pWin, y, 0, _("Select the directory where you want to extract these files."));
				break;
			}

			default:
				break;
		}
	}
	else if (_eClip == CLIP_COPY)
	{
		setcol(COLOR_CYAN, g_tColorCfg._DefaultColor.back, pWin);
		mvwprintw(pWin, y, 0, _("Select the directory where you want to copy these files."));
	}
	else if (_eClip == CLIP_CUT)
	{
		setcol(COLOR_CYAN, g_tColorCfg._DefaultColor.back, pWin);
		mvwprintw(pWin, y, 0, _("Select the directory where you want to move these files."));
	}
	
	wnoutrefresh(pWin);
}

void	StatusInfo::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();

	if (pWin == NULL || _pPanel == NULL) return;

	//  FileManager Status
	ColorEntry tColorStatA = g_tColorCfg.GetColorEntry("StatA");
	ColorEntry tColorStat = g_tColorCfg.GetColorEntry("Stat");
	ColorEntry tColorDir = g_tColorCfg.GetColorEntry("Dir");

	setcol(tColorStatA, pWin);
	wmove(pWin, y, x);
	whline(pWin, ' ', width);

	setcol(tColorStatA, pWin);
	mvwprintw(pWin, y, x+0, "%5d ", _pPanel->_uFile);
	mvwprintw(pWin, y, x+10, "%5d ", _pPanel->_uDir);
	mvwprintw(pWin, y, x+20, "%15.15s ", toregular(_pPanel->_uDirSize).c_str());

	int nSort = _pPanel->GetSortMethod();
	char cSort = ' ';
	switch(nSort)
	{
	case SORT_NAME:
		cSort = 'N';
		break;
	case SORT_EXT:
		cSort = 'E';
		break;
	case SORT_SIZE:
		cSort = 'S';
		break;
	case SORT_TIME:
		cSort = 'T';
		break;
	case SORT_COLOR:
		cSort = 'C';
		break;
	default:
		cSort = ' ';
		break;
	}
	mvwprintw(pWin, y, width-2, "%c", cSort);
	if (nSort != SORT_NONE)
	{
		mvwprintw(pWin, y, width-1, "%c", _pPanel->_bFileSortAscend ? '+' : '-');
	}

	if (_pPanel->_uSelNum)
	{
		mvwprintw(pWin, y, x+40, "%5d", _pPanel->_uSelNum);
		mvwprintw(pWin, y, x+55, " %15.15s ", toregular(_pPanel->_uSelSize).c_str());
	}

	setcol(tColorStat, pWin);
	mvwprintw(pWin, y, x+6,	_("File"));
	mvwprintw(pWin, y, x+16, _("Dir"));
	mvwprintw(pWin, y, x+36, _("Byte"));

	if (_pPanel->_uSelNum)
	{
		mvwprintw(pWin, y, x+45, _(" Selected ("));
		mvwprintw(pWin, y, x+72, _("Bytes)"));
	}
	
	wnoutrefresh(pWin);
	//wrefresh(pWin);
}

void	DirInfo::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();
	if (pWin == NULL || _pPanel == NULL) return;

	File* pFile = _pPanel->GetCurFile();

	ColorEntry tColorFunc = g_tColorCfg.GetColorEntry("Func");
	setcol(tColorFunc, pWin);

	wmove(pWin, y, x);
	whline(pWin, ' ', width);
	
	vector<int>		vGubun;

	String	sView;
	sView.Append("%-11.11s ", pFile->sAttr.c_str());
	vGubun.push_back( sView.size() ); 
	sView.Append(" ");
	if (pFile->sOwner.size() != 0)
	{
		sView.Append(" %s %s ", pFile->sOwner.c_str(), pFile->sGroup.c_str());
		vGubun.push_back( sView.size() ); sView.Append(" ");
	}
	if ( pFile->uSize )
		sView.Append(" %10s ", toregular(pFile->uSize).c_str());
	else
		sView.Append("          0 ");
	vGubun.push_back( sView.size() ); 
	sView.Append(" ");
	sView.Append(" %s %s ", pFile->sDate.c_str(), pFile->sTime.c_str());
	vGubun.push_back( sView.size() ); 
	sView.Append(" ");

	string	sName = pFile->sName;
	
	if ( pFile->bLink )
		sName = sName + " -> " + pFile->sLinkName;

#ifdef __APPLE__
	sName = MLSUTIL::CodeConvert( sName, "UTF-8", "UTF-8-MAC" );
#endif

	sView.Append(" %s", sName.substr(0, width-38).c_str());

	mvwprintw(pWin, y, x, "%s", sView.c_str());
	
	setcol(COLOR_BLACK, tColorFunc.back, pWin);
	for( uint n = 0; n < vGubun.size(); n++)
	{
		mvwaddch(pWin, y, x+vGubun[n], VLINE);
	}

	wnoutrefresh(pWin);
	//wrefresh(pWin);
}

deque<wstring> g_tCmdHistory;

int 	ShellCmd::DataInput(KeyInfo& tKeyInfoBef)
{
	KeyReader	tKeyReader;
	bool	bExit = false;
	bool	bMouse = false;
	bool	bTAB = false;
	bool	bEnter = false;

	_nFirst = 0; _nEnd = 0;
	_nStart = 0;
	
	#ifdef __CYGWIN_C__
	_sWStr = "";
	#else
	_sWStr = L"";
	#endif
	_nCur = 0;
	_nTabIndex = 0;

	int nBefKey = (int)tKeyInfoBef;

	int nHistoryNum = g_tCmdHistory.size();
	
	for(;;)
	{
		KeyInfo		tKeyInfo;

		Draw();
		wredrawln(_pForm->GetWin(), y, 1);

		if (nBefKey != ERR)
			tKeyInfo = tKeyInfoBef;
		else
			tKeyInfo = tKeyReader.Read(_pForm->GetWin());

		nBefKey = ERR;

		LOG("InputBox Key [%d] [%s]", (int)tKeyInfo, ((string)tKeyInfo).c_str());

		if ((int)tKeyInfo != KEY_TAB)
			_nTabIndex = 0;

		switch((int)tKeyInfo)
		{
			case KEY_MOUSE:
				bMouse = true;
				break;

			case KEY_UP:
			{
				if (g_tCmdHistory.empty()) continue;
				
				nHistoryNum--;
				if (nHistoryNum < 0) nHistoryNum = g_tCmdHistory.size()-1;
				
				_sWStr = g_tCmdHistory[nHistoryNum];
				_nCur = _sWStr.size();
				if (_sWStr.size() > _nCmdWidth - 6)
					_nFirst = _sWStr.size() - _nCmdWidth+3;
				else
					_nFirst = 0;
				if (_nFirst < 0) _nFirst = 0;
				continue;
			}
					
			case KEY_DOWN:
			{
				if (g_tCmdHistory.empty()) continue;
				
				nHistoryNum++;
				if (nHistoryNum >= g_tCmdHistory.size()) nHistoryNum = 0;
				
				_sWStr = g_tCmdHistory[nHistoryNum];
				_nCur = _sWStr.size();
				if (_sWStr.size() > _nCmdWidth - 6)
					_nFirst = _sWStr.size() - _nCmdWidth+3;
				else
					_nFirst = 0;
				if (_nFirst < 0) _nFirst = 0;
				continue;
			}

			case KEY_LEFT:
			{
				_nCur--;
				if (_nCur < 0) _nCur = 0;
				if (_nCur-_nFirst < 3)
				{
					_nFirst = _nCur - 3;
					if (_nFirst < 0) _nFirst = 0;
				}
				continue;
			}

			case KEY_RIGHT:
			{
				_nCur++;
				if (_nCur > _sWStr.size()) _nCur = _sWStr.size();
				if (_nCur > _nEnd-3)
				{
					if (_nFirst < _sWStr.size()-5)
						_nFirst = _nFirst + 1;
				}
				continue;
			}
			case KEY_PPAGE:
			case KEY_HOME:
				_nCur = 0;
				_nFirst = 0;
				continue;
			case KEY_NPAGE:
			case KEY_END:
				_nCur = _sWStr.size();
				if (_sWStr.size() > _nCmdWidth - 6)
					_nFirst = _sWStr.size() - _nCmdWidth+3;
				else
					_nFirst = 0;
				if (_nFirst < 0) _nFirst = 0;
				continue;
			case KEY_DC:
				if (!_sWStr.empty()) _sWStr.erase(_nCur, 1);
				continue;
			case 127:
			case KEY_BS:
				if (!_sWStr.empty() && _nCur)
				{
					_sWStr.erase(_nCur-1, 1);
					_nCur--;
					if (_nCur-_nFirst > 4)
					{
						_nFirst=_nFirst-1;
						if (_nFirst < 0) _nFirst = 0;
					}
				}
				continue;
			case KEY_RESIZE:
			case KEY_REFRESH:
				_pForm->Refresh();
				continue;
			case KEY_TAB:
			{
				if (!_nTabIndex)
				{
					wstring::size_type loc;
					# ifdef __CYGWIN_C__
					if ((loc=_sWStr.rfind(' ', _nCur-1)) != wstring::npos)
					#else
					if ((loc=_sWStr.rfind(L' ', _nCur-1)) != wstring::npos)
					#endif
						_nStart = loc + 1;
					else
						_nStart = 0;
	
					// if CD(change dir) command, add the directory only.
					# ifdef __CYGWIN_C__
					if (_sWStr.substr(0, 2) == "cd")
					#else
					if (_sWStr.substr(0, 2) == L"cd")
					#endif
						_vCom_entry = PathComplete(wstrtostr(_sWStr.substr(_nStart, _nCur-_nStart)), false);
					else
						_vCom_entry = PathComplete(wstrtostr(_sWStr.substr(_nStart, _nCur-_nStart)), true);
				}
				
				if (_vCom_entry.empty()) break;
	
				wstring rf;
				if (g_tCfg.GetBool("Default", "TabFileRotate", false))
					rf = strtowstr(addslash(_vCom_entry[_nTabIndex % _vCom_entry.size()]));
				else
					rf = strtowstr(addslash(Com_entry(_vCom_entry, _nStart)));

				wstring::size_type nCut;
				int nCutCur = 0;
				# ifdef __CYGWIN_C__
				nCut = _sWStr.rfind("/");
				# else
				nCut = _sWStr.rfind(L"/");
				#endif
				if (nCut != wstring::npos)
					nCutCur = nCut+1;
				else
					nCutCur = _nStart;
		
				LOG("CutCur [%d %d] [%s]", nCutCur, _nCur, wstrtostr(_sWStr).c_str());
				
				_sWStr.erase(nCutCur, _nCur-nCutCur);
				
				# ifdef __CYGWIN_C__
				if (_vCom_entry.size() == 1 && _sWStr.substr(0, 2) == "cd")
				{
					rf = rf + "/";
					_nTabIndex = 0;
				}
				#else
				if (_vCom_entry.size() == 1 && _sWStr.substr(0, 2) == L"cd")
				{
					rf = rf + L"/";
					_nTabIndex = 0;
				}
				#endif
				else
				{
					_nTabIndex++;
				}
					
				_sWStr.insert(nCutCur, rf);
				_nCur = nCutCur + rf.size();				
			}	
			break;
			case 13:
			case KEY_ENTER: 
				bEnter = true;
				while(g_tCmdHistory.size() >= 50) g_tCmdHistory.pop_front();
				g_tCmdHistory.push_back(_sWStr);
				break;
			case KEY_ESC:
				bExit = true;
				break;
			case 8: // OntheSpot Patch (Ctrl+H)
				if (strtowstr((string)tKeyInfo).size() != 0)
				{
					LOG("[%d]", _nCur );
					if (!_sWStr.empty() && _nCur) _sWStr.erase(_nCur-1, 1);
					wstring wstr = strtowstr((string)tKeyInfo);
					if (_sWStr.size() > _nCur)
						_sWStr = _sWStr.substr(0, _nCur-1) + wstr + _sWStr.substr(_nCur-1);
					else
						_sWStr = _sWStr + strtowstr((string)tKeyInfo);
					LOG("Input Write [%s] [%s]", ((string)tKeyInfo).c_str(), wstrtostr(_sWStr).c_str());
				}
				else
				{
					// Cygwin num 8 : BS
					if (!_sWStr.empty() && _nCur)
					{
						_sWStr.erase(_nCur-1, 1);
						_nCur--;
						if (_nCur-_nFirst > 4)
						{
							_nFirst=_nFirst-1;
							if (_nFirst < 0) _nFirst = 0;
						}	
					}
				}
				break;
			case 27:
			case ERR:
				continue;
			case 15: // CTRL_O
				return 15;
				break;
			default:
			{
				if (strtowstr((string)tKeyInfo).size() != 0)
				{
					wstring wstr = strtowstr((string)tKeyInfo);
					if (_sWStr.size() > _nCur)
						_sWStr = _sWStr.substr(0, _nCur) + wstr + _sWStr.substr(_nCur);
					else
						_sWStr = _sWStr + wstr;
					_nCur = _nCur + wstr.size();
					if (_nCur >= _nEnd) _nFirst=_nFirst+wstr.size();
				}
				continue;
			}
		}
		if (bEnter || bMouse || bExit) break;
	}
	
	if (bMouse)	return KEY_MOUSE;
	if (bEnter)	return KEY_ENTER;
	if (bExit)	return KEY_ESC;
	return SUCCESS;
}

string	ShellCmd::GetPrompt()
{
	string  sPrompt, sLogin, sHostName;

	char	cHostName[100];
	memset(&cHostName, 0, sizeof(cHostName));
	if (gethostname((char*)&cHostName, sizeof(cHostName)) == -1)
		sPrompt = "$ ";
	else
	{
		struct passwd* pw = getpwuid(getuid());
		if ( pw )
			sLogin = pw->pw_name;
	}
	if (strlen(cHostName) == 0 || sLogin.size() == 0)
		sPrompt = "$ ";
	sHostName = cHostName;
	if (sHostName.find(".") != string::npos)
		sHostName = sHostName.substr(0, sHostName.find("."));

	if (sPrompt != "$ ")
	{
		sPrompt = "";
		string sPath = _pPanel->GetPath();
		Reader* pReader = _pPanel->GetReader("file");
		if (sPath == pReader->GetRealPath("~")) sPath = "~";
		if (sPath != "/" && sPath.substr(sPath.size()-1, 1) == "/")
			sPath = sPath.substr(0, sPath.size()-1);
		if (scrstrlen(sPath) > 40)
			sPath = "..." + scrstrncpy(sPath, scrstrlen(sPath)- 37, 37);
		sPrompt = sPrompt + sLogin + "@" + sHostName  + ":" + sPath;
		if (sLogin == "root")
			sPrompt = sPrompt + "# ";
		else
			sPrompt = sPrompt + "$ ";
	}
	return sPrompt;
}

void	ShellCmd::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();
	if (pWin == NULL || _pPanel == NULL) return;
	
	setcol(g_tColorCfg._DefaultColor, pWin);
	wmove(pWin, y, x);
	whline(pWin, ' ', width);

	string sPrompt = GetPrompt();
	_nCmdWidth = width - scrstrlen(sPrompt);

	// if the end charator is broken. cut the end line.
	int W1 = _nCmdWidth;
	if (wstrlen(_sWStr.substr(_nFirst, W1)) > _nCmdWidth)
	{
		while(1) {
			if (wstrlen(_sWStr.substr(_nFirst, W1)) <= _nCmdWidth) break;
			W1--;
		}
	}
	_nEnd = _nFirst + W1;
	/****************************/	
	
	string	sViewStr = wstrtostr(_sWStr.substr(_nFirst, W1));
	wprintw(pWin, "%s%s", sPrompt.c_str(), sViewStr.c_str());
	wattroff(pWin, A_BOLD);

	// go to the cursor.
	int nViewcur = wstrlen(_sWStr.substr(_nFirst, _nCur-_nFirst));
	wmove(pWin, y, x+nViewcur+scrstrlen(sPrompt));
	wnoutrefresh(pWin);
	//wrefresh(pWin);
	curs_set(1);
}

string	ShellCmd::Com_entry(vector<string>& vStr, int cur)
{
	string  sName, sName2;
	if (vStr.size() == 0) return "";
	if (vStr.size() == 1) return vStr[0];
	sName = vStr[0];
	for (int nLength = cur; nLength < sName.size(); nLength++)
	{
		for (int nCount = 1; nCount < vStr.size(); nCount++)
		{
			sName2 = vStr[nCount];
			if (nLength > vStr[nCount].size()) continue;
			if (sName[nLength] != sName2[nLength])
			{
				return sName.substr(0, nLength);
			}
		}
	}
	return sName;
}
	
vector<string> ShellCmd::PathComplete(const string &path, bool bPathsearch)
{
	//          l
	// 012345678901
	// workspace/l
	// /usr/src/red
	vector<string> vEntries;
	vEntries.clear();
	
	string::size_type loc;
	string sName = path;
	string sDir, sDirName;

	Reader*	pReader = _pPanel->GetReader("file");
	if (pReader == NULL) return vEntries;

	string	sBefPath = pReader->GetPath();
	
	if ((loc = sName.rfind('/')) == string::npos)
	{
		sDirName = sBefPath;
	}
	else
	{
		if (sName[0] == '/')
			sDirName = sName.substr(0, loc+1);
		else
			sDirName = sBefPath + sName.substr(0, loc+1);
		sName = sName.substr(loc+1);
	}

	if (pReader->Read(sDirName) == false) return vEntries;
	bool bBreak = false;
	
	while(pReader->Next())
	{
		MLS::File	tFile;
		if (!pReader->GetInfo(tFile)) continue;
		if (tFile.sName == "." || tFile.sName == "..")	continue;
		if (!strncmp(sName.c_str(), tFile.sName.c_str(), sName.length()))
		{
			if (bPathsearch == true)
				vEntries.push_back(tFile.sName);
			else
				if (tFile.bDir)
					vEntries.push_back(tFile.sName);
		}
	}
	pReader->Read(sBefPath);
	return vEntries;
}
