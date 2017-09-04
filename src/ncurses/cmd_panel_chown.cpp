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

#include "cmd_panel_imp.h"
#include "panel.h"
#include "mainframe.h"
#include "selection.h"
#include "dialogbox.h"
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>

using namespace MLS;
using namespace MLSUTIL;

class UserInfo
{
public:
	string	sOwner;
	uid_t	uid;
};

class GroupInfo
{
public:
	string	sGroup;
	gid_t	gid;
};

class ChownBox: public Form
{
public:
	ChownBox(const string& sTitle)
	{
		SetTitle(sTitle);
		
		_tOwnerLabel = Label(_("[ Owner ]"));
		_tGroupLabel = Label(_("[ Group ]"));

		_tButtonOk = Button(_("Ok"), 12);
		_tButtonCancel = Button(_("Cancel"), 12);
		_tOwnerBox = TextBox(1, 1, 10, 20);
		_tGroupBox = TextBox(1, 1, 10, 20);

		fontcolor = g_tColorCfg.GetColorEntry("SelectBox").font;
		backcolor = g_tColorCfg.GetColorEntry("SelectBox").back;

		_tOwnerBox.fontcolor = _tGroupBox.fontcolor = g_tColorCfg.GetColorEntry("Edit").font;
		_tOwnerBox.backcolor = _tGroupBox.backcolor = g_tColorCfg.GetColorEntry("Edit").back;
		
		_tOwnerBox.ShowCursor( true );
		_tGroupBox.ShowCursor( false );
		_nSelect = 0;
		_bSearch = false;
		_bOk = false;
	}
	
	virtual ~ChownBox() { }
	
	void	setTitle(const string& sTitle) { _sTitle = sTitle; }
	void	setList(vector<string>& vOwnerList, vector<string>& vGrpList)
	{
		_tOwnerBox.SetStringList( vOwnerList );
		_tGroupBox.SetStringList( vGrpList );
		_vOwner = vOwnerList;
		_vGroup = vGrpList;
	}
	
	void	setListPosition( int nOwner, int nGroup )
	{
		LOG_WRITE("Set Position :: nOwner [%d] nGroup [%d]", nOwner, nGroup);
		_tOwnerBox.SetCur( nOwner );
		_tGroupBox.SetCur( nGroup );
	}
	
	bool	getList( int* nOwner, int* nGroup )
	{
		*nOwner = _tOwnerBox.GetCur();
		*nGroup = _tGroupBox.GetCur();
		return _bOk;	
	}

	bool 	isExit()	{ return _bExit; }
	
protected:
	void	Draw()
	{
		_tButtonOk.backcolor = backcolor; _tButtonOk.fontcolor = fontcolor;
		_tButtonCancel.backcolor = backcolor; _tButtonCancel.fontcolor = fontcolor;
		_tOwnerLabel.backcolor = backcolor; _tOwnerLabel.fontcolor = fontcolor;
		_tGroupLabel.backcolor = backcolor; _tGroupLabel.fontcolor = fontcolor;
		
		_tOwnerLabel.SetForm((Form*)this);
		_tGroupLabel.SetForm((Form*)this);
		_tButtonOk.SetForm((Form*)this);
		_tButtonCancel.SetForm((Form*)this);
		_tOwnerBox.SetForm((Form*)this);
		_tGroupBox.SetForm((Form*)this);
	
		_tOwnerBox.Move(4, 3);
		_tGroupBox.Move(4, _tOwnerBox.width + 6);

		_tOwnerLabel.Move( 3, 3 );
		_tGroupLabel.Move( 3, _tOwnerBox.width + 6 );
		
		_tButtonOk.Move(height - 2, (width / 2) - _tButtonOk.width - 2);
		_tButtonCancel.Move(height - 2, width / 2 + 2);
		_tButtonOk.SetTxtAlign(MIDDLE);
		_tButtonCancel.SetTxtAlign(MIDDLE);
		
		_tOwnerBox.ShowCursor( false );
		_tGroupBox.ShowCursor( false );
		_tButtonOk.SetFocus( false );
		_tButtonCancel.SetFocus( false );
		
		switch( _nSelect )
		{
			case 0:	_tOwnerBox.ShowCursor( true ); break;
			case 1: _tGroupBox.ShowCursor( true ); break;
			case 2: _tButtonOk.SetFocus( true ); break;
			case 3: _tButtonCancel.SetFocus( true ); break;
		}

		setcol(fontcolor, backcolor, GetWin());
		wmove(GetWin(), height - 3, 2);
		whline(GetWin(), HLINE, width-4);
		
		_tOwnerBox.Show();
		_tGroupBox.Show();
		_tOwnerLabel.Show();
		_tGroupLabel.Show();
		_tButtonOk.Show();
		_tButtonCancel.Show();
	}
	
	bool	MouseEvent(int Y, int X, mmask_t bstate)
	{
		if (_tButtonOk.AreaChk(Y, X))
		{
			_tButtonOk.SetFocus(true);
			_bOk = true;
			if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
			_nSelect = 2;
			return true;
		}
	
		if (_tButtonCancel.AreaChk(Y, X))
		{
			_tButtonCancel.SetFocus(true);
			_bOk = false;
			if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;			
			_nSelect = 3;
			return true;
		}
		return false;
	}

	void	SearchProcess(KeyInfo&	tKeyInfo)
	{
		int key = (int)tKeyInfo;

		LOG_WRITE("SearchProcess Key :: [%d]", key );
		if (32 < key && key <=126)
		{
			if ( _nSelect == 0 )
			{
				for ( uint n = 0; n < _vOwner.size(); n++)
				{
					string str = _vOwner[n];
					if ( str[0] == (char)key )
					{
						LOG_WRITE("SearchProcess Owner str :: [%s]", str.c_str() );
						_tOwnerBox.SetCur( n );
						return;
					}
				}
			}
			else if ( _nSelect == 1 )
			{
				for ( uint n = 0; n < _vGroup.size(); n++)
				{
					string str = _vGroup[n];
					if ( str[0] == (char)key )
					{
						LOG_WRITE("SearchProcess Group str :: [%s]", str.c_str() );
						_tGroupBox.SetCur( n );
						return;
					}
				}
			}
		}
	}
		
	void 	Execute(KeyInfo& tKeyInfo)
	{
		if ( _nSelect == 0 ) // Owner Box
		{
			switch ((int)tKeyInfo)
			{
				case KEY_LEFT:		_nSelect = 3; 			break;
				case KEY_TAB:
				case KEY_RIGHT:		_nSelect = 1; 			break;
				case KEY_DOWN:		_tOwnerBox.Down();		break;
				case KEY_UP:		_tOwnerBox.Up();		break;
				case KEY_PPAGE:		_tOwnerBox.PageUp();	break;
				case KEY_NPAGE:		_tOwnerBox.PageDown();	break;
				case KEY_ESC:
				case KEY_ENTER:
				case 13:
				case ' ':			_nSelect = 1;			break;
			}

			SearchProcess(tKeyInfo);
		}
		else if ( _nSelect == 1 ) // Group Box
		{
			switch ((int)tKeyInfo)
			{
				case KEY_LEFT:		_nSelect = 0; 			break;
				case KEY_TAB:
				case KEY_RIGHT:		_nSelect = 2; 			break;
				case KEY_DOWN:		_tGroupBox.Down();		break;
				case KEY_UP:		_tGroupBox.Up();		break;
				case KEY_PPAGE:		_tGroupBox.PageUp();	break;
				case KEY_NPAGE:		_tGroupBox.PageDown();	break;
				case KEY_ESC:
				case KEY_ENTER:
				case 13:
				case ' ':			_nSelect = 2;			break;
			}
			SearchProcess(tKeyInfo);
		}
		else if ( _nSelect == 2 )
		{
			switch ((int)tKeyInfo)
			{
				case KEY_LEFT:
				case KEY_UP:		_nSelect = 1; 	break;
				
				case KEY_TAB:
				case KEY_RIGHT:
				case KEY_DOWN:		_nSelect = 3;	break;
				case KEY_PPAGE:		_nSelect = 0;	break;
					
				case KEY_ESC:
				case KEY_NPAGE:		_nSelect = 3;	break;
											
				case KEY_ENTER:
				case 13:
				case ' ':
					_bOk 	= true;	_bExit	= true;
					break;
			}
		}
		else if ( _nSelect == 3 )
		{
			switch ((int)tKeyInfo)
			{
				case KEY_LEFT:
				case KEY_UP:		_nSelect = 2; 	break;
				case KEY_TAB:
				case KEY_RIGHT:
				case KEY_DOWN:		_nSelect = 0;	break;
				case KEY_PPAGE:		_nSelect = 0;	break;
				case KEY_NPAGE:		_nSelect = 3;	break;
				case KEY_ESC:
				case KEY_ENTER:
				case 13:
				case ' ':
					_bOk 	= false; _bExit	= true;
					break;
			}
		}
	}
	
private:
	Label		_tOwnerLabel;
	Label		_tGroupLabel;
	TextBox		_tOwnerBox;
	TextBox		_tGroupBox;
	
	Button		_tButtonOk;
	Button		_tButtonCancel;
	int			_nSelect;
	bool		_bOk;

	bool		_bSearch;
	string		_sStrSearch;

	vector<string>	_vOwner;
	vector<string>	_vGroup;
};

void	CmdPanelImp::Chown()
{
	if (_pPanel->GetReader()->GetReaderName() != "file" )
	{
		MsgBox(_("Error"), _("current connect mode or archive mode."));
		return; 
	}
	
	struct passwd* 		pwd;
	struct group* 		grp;
	
	vector<GroupInfo>	vGroupList;
	vector<UserInfo>	vUserList;
	
	vector<string>		vOwnerStr, vGroupStr, vViewStr;
	
	setpwent ();
    while ((pwd = getpwent ()))
    {
    	UserInfo	tUserInfo;
    	tUserInfo.sOwner = pwd->pw_name;
    	tUserInfo.uid = pwd->pw_uid;
    	vUserList.push_back( tUserInfo );
    	vOwnerStr.push_back( tUserInfo.sOwner );
    }
    endpwent ();
    
    setgrent ();
    while ((grp = getgrent ()))
    {
    	GroupInfo	tGroupInfo;
    	tGroupInfo.sGroup = grp->gr_name;
    	tGroupInfo.gid = grp->gr_gid;
    	vGroupList.push_back( tGroupInfo );
    	vGroupStr.push_back( tGroupInfo.sGroup );
    }
    endgrent ();
    
    int		nSelectOwner = 0, nSelectGroup = 0;
    
    Selection& tMainSelection = *(g_tMainFrame.GetSelection());
	tMainSelection.Clear();
	_pPanel->GetSelection(tMainSelection);
	
	for ( uint n = 0; n < tMainSelection.GetSize(); n++ )
	{
		if ( tMainSelection[n]->bDir )
		{
			if ( YNBox(_("Question"), _("subdirectory files change ?")) )
			{
				tMainSelection.ExpandDir(_pPanel->GetReader(), true);
				break;
			}
		}
	}
	
	if ( tMainSelection.GetSize() > 0 )
	{
		File*	pFile = tMainSelection[0];
		for ( int n = 0; n < vUserList.size(); n++ )
		{
			if (pFile->sOwner == vUserList[n].sOwner)
			{
				nSelectOwner = n; 
				break;
			}
		}	
		
		for ( int n = 0; n < vGroupList.size(); n++ )
		{
			if (pFile->sGroup == vGroupList[n].sGroup)
			{
				nSelectGroup = n; 
				break;
			}
		}
	}
	
	{
		ChownBox		tChownBox(_("Chown command"));
		
		tChownBox.Size(17, 50);
		tChownBox.Move(MIDDLE, MIDDLE);

		tChownBox.setList(vOwnerStr, vGroupStr);
		tChownBox.setListPosition( nSelectOwner, nSelectGroup );
		tChownBox.Do();

		if (!tChownBox.getList( &nSelectOwner, &nSelectGroup ))
		{
			Refresh();
			return;
		}
	}
	
	vViewStr.clear();
	
	for ( uint n = 0; n < tMainSelection.GetSize(); n++ )
	{
		if ( chown(	tMainSelection[n]->sFullName.c_str(), 
					vUserList[nSelectOwner].uid, 
					vGroupList[nSelectGroup].gid ) == -1 )
		{
			string    sStr = tMainSelection[n]->sFullName + " - " + strerror(errno);
			vViewStr.push_back(sStr);
		}
	}
	
	Refresh();

	if (vViewStr.size() != 0)
	{
		Curses_TextBox	tTextBox(_("Error Info"));
	
		int	nHeightSize = vViewStr.size();
	
		if ( nHeightSize > LINES-6 && LINES-6 > 0 ) 
			nHeightSize = LINES-6;
		else
			nHeightSize = vViewStr.size() + 6;
	
		int nMaxWidth = COLS-4;
		for (uint n = 0; n < vViewStr.size(); n++)
		{
			int nSize = scrstrlen( vViewStr[n] );
			if ( nSize > nMaxWidth) nMaxWidth = nSize;
		}

		tTextBox.backcolor = COLOR_RED;
		tTextBox.fontcolor = COLOR_WHITE;
		
		if ( nMaxWidth+4 > COLS - 8 ) nMaxWidth = COLS-8-4;
		tTextBox.setCurShow( true );
		tTextBox.Size(nHeightSize, nMaxWidth+4);
		tTextBox.setText( vViewStr );
		tTextBox.Move(MIDDLE, MIDDLE);
		tTextBox.Do();
	}
}

