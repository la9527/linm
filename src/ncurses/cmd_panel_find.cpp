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

using namespace MLS;
using namespace MLSUTIL;

void	CmdPanelImp::FileFind()
{
	string			sFindStr, sFullName, sCurPath;
	vector<string>	vFindName;
	vector<File*>	vFindFiles;

	if (InputBox(_("input search filename."),
				 sFindStr) == ERROR) return;

	sCurPath = _pPanel->GetPath();

	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [ESC]"));

	Selection& tMainSelection = *(g_tMainFrame.GetSelection());
	tMainSelection.Clear();
	_pPanel->SelectAll();
	_pPanel->GetSelection(tMainSelection);
	tMainSelection.ExpandDir(_pPanel->GetReader(), true );

	MsgWaitEnd(pWait);

	for ( uint n = 0; n < tMainSelection.GetSize(); n++ )
	{
		File* pFile = tMainSelection[n];

		if( sFindStr == "*" || pFile->sName.find( sFindStr ) != string::npos)
		{
			vFindFiles.push_back( pFile );

			sFullName = pFile->sFullName;
			sFullName = sFullName.substr( sCurPath.size() );
			vFindName.push_back( sFullName );
		}
	}

	_pPanel->SelectInvert();

	if ( vFindName.size() == 0 )
	{
		Refresh();
		MsgBox(_("Warning"), _("File Not Found."));
		return;
	}

	int nCur = -1;

	{
		String	sView;
		sView.Printf(_("Find List - %d"), vFindName.size());
		Curses_TextBox	tTextBox(sView);
	
		int	nHeightSize = vFindName.size();
	
		if ( nHeightSize > LINES-6 && LINES-6 > 0 ) 
			nHeightSize = LINES-6;
		else
			nHeightSize = vFindName.size() + 6;
	
		int nMaxWidth = 40;
		for (uint n = 0; n < vFindName.size(); n++)
		{
			int nSize = scrstrlen( vFindName[n] );
			if ( nSize > nMaxWidth) nMaxWidth = nSize;
		}

		tTextBox.backcolor = COLOR_BLUE;
		tTextBox.fontcolor = COLOR_WHITE;
		
		if ( nMaxWidth+4 > COLS - 8 ) nMaxWidth = COLS-8-4;
		tTextBox.setCurShow( true );
		tTextBox.Size(nHeightSize, nMaxWidth+4);
		tTextBox.setText( vFindName );
		tTextBox.Move(MIDDLE, MIDDLE);
		tTextBox.Do();

		nCur = tTextBox.GetCur();
	}

	if ( nCur != -1 )
	{
		File* 	pFile = vFindFiles[ nCur ];
		string 	sPath = pFile->sFullName;
		
		if ( !pFile->bDir )
			sPath = sPath.substr(0, pFile->sFullName.size() - pFile->sName.size());

		_pPanel->Read( sPath );

		if ( !pFile->bDir )
			_pPanel->SetCurFileName( pFile->sFullName );

		Refresh();
	}
}

