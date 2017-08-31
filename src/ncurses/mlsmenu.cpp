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

#include "mlsmenu.h"

using namespace MLS;

void MlsMenu::MenuColorSetting() {
	_tColorMenu = g_tColorCfg.GetColorEntry("Func");
	_tColorMenuA = g_tColorCfg.GetColorEntry("FuncA");
}

void MlsMenu::MenuAddCatecory() {
	MenuCategory mls("LinM", PANEL);
		mls.AddItem(_("About"), "Cmd_About");
		mls.AddItem(_("Help"), "Cmd_Help");
		mls.AddItem("", "");
		mls.AddItem(_("Quit LinM"), "Cmd_Quit");
	AddItem(mls);

	MenuCategory run(_("Run"), PANEL);
		run.AddItem(_("Run"), "Cmd_Enter");
		run.AddItem(_("Run(select)"), "Cmd_Execute");
		run.AddItem("", "");
		run.AddItem(_("Console Mode"), "Cmd_ConsoleMode");
		run.AddItem(_("Shell command"), "Cmd_Shell");
		run.AddItem("", "");
		run.AddItem(_("View console"), "Cmd_ESC");
	AddItem(run);

	MenuCategory files(_("File"), PANEL);
		files.AddItem(_("New"), "Cmd_NewFile");
		files.AddItem(_("Touch File"), "Cmd_TouchFile");
		files.AddItem("", "");
		files.AddItem(_("ClipCut"), "Cmd_ClipCut");
		files.AddItem(_("ClipCopy"), "Cmd_ClipCopy");
		files.AddItem(_("ClipPaste"), "Cmd_ClipPaste");
		files.AddItem("", "");
		files.AddItem(_("Find"), "Cmd_FileFind");
		files.AddItem(_("Diff"), "Cmd_Diff");
		files.AddItem("", "");
		files.AddItem(_("Copy"), "Cmd_Copy");
		files.AddItem(_("Move"), "Cmd_Move");
		files.AddItem(_("Rename"), "Cmd_Rename");
		files.AddItem(_("Delete"), "Cmd_Remove");
		files.AddItem(_("Chmod"), "Cmd_Chmod");
		files.AddItem(_("Chown"), "Cmd_Chown");
		files.AddItem("", "");
		files.AddItem(_("Size info"), "Cmd_SizeInfo");
		files.AddItem("", "");
		files.AddItem(_("Select all"), "Cmd_SelectAll");
		files.AddItem(_("Invert select"), "Cmd_SelectInvert");
	AddItem(files);

	MenuCategory dire(_("Directory"), PANEL);
		dire.AddItem("Mcd", "Cmd_MCD");
		dire.AddItem("Qcd", "Cmd_QCD");
		dire.AddItem("", "");
		dire.AddItem(_("Mount List"), "Cmd_MountList");
		dire.AddItem(_("SyncDir"), "Cmd_SyncDirectory");
		dire.AddItem("", "");
		dire.AddItem(_("Mkdir"), "Cmd_Mkdir");
		dire.AddItem(_("To parent"), "Cmd_GoParent");
		dire.AddItem(_("To root"), "Cmd_GoRoot");
		dire.AddItem(_("To home"), "Cmd_GoHome");
		dire.AddItem("", "");
		dire.AddItem(_("Back"), "Cmd_Back");
		dire.AddItem(_("Forward"), "Cmd_Forward");
		//dire.AddItem("", "");
		//dire.AddItem(_("Properties"), "<properties>", false);
	AddItem(dire);

	MenuCategory util(_("Util"), PANEL);
		util.AddItem(_("Remote Connect"), "Cmd_RemoteConnProps");
		util.AddItem(_("Quick Connect"), "Cmd_RemoteConnect");
		util.AddItem(_("Disconnect"), "Cmd_RemoteClose");
		util.AddItem("","");
		util.AddItem(_("Extract"), "Cmd_Extract");
		util.AddItem("", "");
		util.AddItem(_("Compress (tar.gz)"), "Cmd_TargzComp");
		util.AddItem(_("Compress (tar.bz2)"), "Cmd_Tarbz2Comp");
		util.AddItem(_("Compress (zip)"), "Cmd_ZipComp");
	AddItem(util);

	MenuCategory view(_("View"), PANEL);
		view.AddItem(_("Refresh"), "Cmd_Refresh");
		view.AddItem("", "");
		view.AddItem(_("Column AUTO"), "Cmd_ColumnAuto");
		view.AddItem("", "");
		view.AddItem(_("Column 1"), "Cmd_Column1");
		view.AddItem(_("Column 2"), "Cmd_Column2");
		view.AddItem(_("Column 3"), "Cmd_Column3");
		view.AddItem(_("Column 4"), "Cmd_Column4");
		util.AddItem("", "");
		util.AddItem(_("ViewClip"), "Cmd_ViewClip");
		view.AddItem("", "");
		if ( _pPanel->GetHiddenView()  )
			view.AddItem(_("Hide hidden file"), "Cmd_HiddenFileView");
		else
			view.AddItem(_("Show hidden file"), "Cmd_HiddenFileView");

		if ( _pPanel->GetViewOwner() )
			view.AddItem(_("Hide owner"), "Cmd_FileOwnerView");
		else
			view.AddItem(_("Show owner"), "Cmd_FileOwnerView");

		view.AddItem(_("Sort Change"), "Cmd_SortChange");
		view.AddItem(_("Sort Asc/Descend"), "Cmd_SortAscDescend");
		view.AddItem("", "");
		view.AddItem(_("Split"),  "Cmd_Split");
		view.AddItem(_("Next window"), "Cmd_NextWindow");
		view.AddItem("", "");
		view.AddItem(_("SplitSync On/Off"), "Cmd_SplitViewSync");
	AddItem(view);

	MenuCategory op(_("Option"), PANEL);
		op.AddItem(_("Configure"), "Cmd_SettingChange");
		op.AddItem("", "");
		op.AddItem(_("Linecode Change"), "Cmd_BoxCodeChange");
		op.AddItem(_("Locale Change"), "Cmd_LangChange");
		op.AddItem("", "");
		op.AddItem(_("Edit Config"), "Cmd_DefaultCfgFileChg");
		op.AddItem(_("Edit Keybind"), "Cmd_KeyCfgFileChg");
		op.AddItem(_("Edit Colorset"), "Cmd_ColorCfgFileChg");
	AddItem(op);
}

void McdMenu::MenuColorSetting() {
	_tColorMenu = g_tColorCfg.GetColorEntry("Func");
	_tColorMenuA = g_tColorCfg.GetColorEntry("FuncA");
}

void McdMenu::MenuAddCatecory() {
	MenuCategory mcd("Mcd", MCD);
		mcd.AddItem(_("About"), "Cmd_About");
		mcd.AddItem(_("Help"), "Cmd_Help");
		mcd.AddItem("", "");
		mcd.AddItem(_("Quit Mcd"), "Cmd_Quit");
	AddItem(mcd);

	MenuCategory dire(_("Directory"), MCD);
		dire.AddItem(_("Mkdir"), "Cmd_Mkdir");
		dire.AddItem(_("Rename"), "Cmd_Rename");
		dire.AddItem("", "");
		dire.AddItem(_("Mount List"), "Cmd_MountList");
		dire.AddItem(_("Size Info"), "Cmd_SizeInfo");
		dire.AddItem("", "");
		dire.AddItem(_("ClipCut"), "Cmd_ClipCut");
		dire.AddItem(_("ClipCopy"), "Cmd_ClipCopy");
		dire.AddItem(_("ClipPaste"), "Cmd_ClipPaste");
		dire.AddItem("", "");
		dire.AddItem(_("To parent"), "Cmd_GoParent");
		dire.AddItem(_("To root"), "Cmd_GoRoot");
		dire.AddItem(_("To home"), "Cmd_GoHome");
	AddItem(dire);

	MenuCategory util(_("Util"), MCD);
		util.AddItem(_("Quick Connect"), "Cmd_RemoteConnect");
		util.AddItem(_("Disconnect"), "Cmd_RemoteClose");
	AddItem(util);

	MenuCategory view(_("View"), MCD);
		view.AddItem(_("Refresh"), "Cmd_Refresh");
		view.AddItem("", "");
		if ( _pMcd->GetHiddenView() )
			view.AddItem(_("Hide hidden file"), "Cmd_HiddenFileView");
		else
			view.AddItem(_("Show hidden file"), "Cmd_HiddenFileView");
		view.AddItem(_("Sort Change"), "Cmd_SortChange");
		view.AddItem(_("Sort Asc/Descend"), "Cmd_SortAscDescend");
		view.AddItem("", "");
		view.AddItem(_("Split"),  "Cmd_Split");
		view.AddItem(_("Next window"), "Cmd_NextWindow");
		view.AddItem("", "");
		view.AddItem(_("SplitSync On/Off"), "Cmd_SplitViewSync");

	AddItem(view);

	MenuCategory op(_("Option"), MCD);
		op.AddItem(_("Linecode Change"), "Cmd_BoxCodeChange");
		op.AddItem("", "");
		op.AddItem(_("Korean Mode"), "Cmd_LangChange");
	AddItem(op);
}

void EditorMenu::MenuColorSetting() {
	_tColorMenu = g_tColorCfg.GetColorEntry("Func");
	_tColorMenuA = g_tColorCfg.GetColorEntry("FuncA");
}

void EditorMenu::MenuAddCatecory() {
	MenuCategory File(_("File"), EDITOR );
		File.AddItem(_("New"), "Cmd_FileNew");
		File.AddItem("", "");
		File.AddItem(_("Save"), "Cmd_Save");
		File.AddItem(_("Save As"), "Cmd_SaveAS");
		File.AddItem("", "");
		File.AddItem(_("Quit"), "Cmd_Quit");
	AddItem(File);

	MenuCategory Edit(_("Edit"), EDITOR );
		Edit.AddItem(_("Undo"), "Cmd_Undo");
		Edit.AddItem("", "");
		Edit.AddItem(_("Cut"), "Cmd_ClipCut");
		Edit.AddItem(_("Copy"), "Cmd_ClipCopy");
		Edit.AddItem(_("Paste"), "Cmd_ClipPaste");
		Edit.AddItem("", "");
		Edit.AddItem(_("Indent"), "Cmd_Tab");
		Edit.AddItem(_("Unindent"), "Cmd_Untab");
		Edit.AddItem("", "");
		if (_pEditor && _pEditor->GetIndentMode())
			Edit.AddItem(_("Indent Off"), "Cmd_IndentMode");
		else
			Edit.AddItem(_("Indent On"), "Cmd_IndentMode");
		Edit.AddItem("", "");
		Edit.AddItem(_("Select All"), "Cmd_SelectAll");
		Edit.AddItem("", "");
		Edit.AddItem(_("Select Mode"), "Cmd_SelectMode");
	AddItem(Edit);

	MenuCategory Find(_("Find"), EDITOR );
		Find.AddItem(_("Find"), "Cmd_Find");
		Find.AddItem("", "");
		Find.AddItem(_("Find Next"), "Cmd_FindNext");
		Find.AddItem(_("Find Previous"), "Cmd_FindPrevious");
        Find.AddItem("", "");
        Find.AddItem(_("Replace"), "Cmd_Replace");
	AddItem(Find);

	MenuCategory View(_("View"), EDITOR );
		View.AddItem(_("Refresh"), "Cmd_Refresh");
		View.AddItem("", "");
		View.AddItem(_("Split"),  "Cmd_Split");
		View.AddItem(_("Next window"), "Cmd_NextWindow");
		View.AddItem("", "");
		View.AddItem(_("Show line numbers"), "Cmd_LineNumberView");
		View.AddItem("", "");
		View.AddItem(_("Goto Line"), "Cmd_GotoLine");
		View.AddItem("", "");
		View.AddItem(_("Goto First"), "Cmd_GotoFirst");
		View.AddItem(_("Goto End"),  "Cmd_GotoEnd");
		View.AddItem("", "");
		View.AddItem(_("Vim Editor"),  "Cmd_VimView");
	AddItem(View);

	MenuCategory op(_("Option"), EDITOR );
		op.AddItem(_("Linecode Change"), "Cmd_BoxCodeChange");
		op.AddItem(_("Korean Mode"), "Cmd_LangChange");
		op.AddItem("", "");

		if ( _pEditor->_bFullScreen == true )
		{
			if ( _pEditor->_bMouseMode == false)
				op.AddItem(_("Mouse Mode On"), "Cmd_MouseUse");
			else
				op.AddItem(_("Mouse Mode Off"), "Cmd_MouseUse");
		}
		else
			op.AddItem(_("Mouse Mode Off"), "Cmd_MouseUse", false);
	AddItem(op);

	MenuCategory help(_("Help"), EDITOR );
		help.AddItem(_("About"), "Cmd_About");
		help.AddItem("", "");
		help.AddItem(_("Help"), "Cmd_Help");
	AddItem(help);
}
