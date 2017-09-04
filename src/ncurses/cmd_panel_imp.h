#ifndef __MLS_CMD_PANEL_IMP_H__
#define __MLS_CMD_PANEL_IMP_H__

#include "cmd_common_imp.h"

#include "command.h"
#include "selection.h"

// PluginLoader.
#include "vfsplugin.h"
#include "readerctl.h"

namespace MLS {

class CmdPanelImp:public CmdCommonImp, public Configurable
{
private:
	vector<NCurses_Panel*>	_pCurPanel;

public:
	CmdPanelImp() : CmdCommonImp()
	{
		UpdateConfig();

		// This reader is for copy & paste
		// only one load this hear.
		PluginLoader(&g_tReaderCtl);
	}

	~CmdPanelImp() 					{ SaveConfig(); }

	// IConfigurable
	void UpdateConfig();
	void SaveConfig();

	int  	Run(const std::string &, bool = false, bool = false);
	int		ParseAndRun(const string &p, bool = false);

	void	GoRoot();
	void	GoHome();
	void	GoParent();

	void	Key_Enter();
	void	Key_Left();
	void	Key_Right();
	void	Key_Up();
	void	Key_Down();
	void	Key_Home();
	void	Key_End();
	void	Key_PageUp();
	void	Key_PageDown();
	void	Key_Ins()			{ ClipPaste(); }
	void	Key_Del()			{ Remove(); }
	void	Key_BS()			{ GoParent(); }
	void	Key_Select()		{ _pPanel->Key_Select();	}

	void	SelectAll()		{ _pPanel->SelectAll();		}
	void	SelectInvert() 	{ _pPanel->SelectInvert();	}

	void	Key_ESC();
	void	Mcd();

	void	ClearRefresh();
	void	Refresh();

	void	ArchiveFileView();
	void	ArchiveFileMake();

	void	RemoteConnProps();
	void	RemoteConnect();
	void	RemoteClose();

	void	Editor();
	void	ExtEditor();
	void	Menu();
	void	EditorChoice(bool bReadOnly, const string& sEditorCmd = "", File* pFile = NULL);

	void	Quit();	
	void	Split();
	void	NextWindow();
	void	SplitViewSync();
	void	ViewSync();

	void	Remove();
	void	ClipCopy();
	void	ClipPaste();
	void	ClipCut();

	void	Rename();
	void	Mkdir();

	void	Shell();

	void	McdSubDirOneSearch()	{	_pMcd->SubDirOneSearch();	}
	void	McdSubDirAllSearch()	{	_pMcd->SubDirAllSearch();	}
	void	McdSubDirClear() 		{	_pMcd->SubDirClear(); }

	void	HiddenFileView();
	void	FileOwnerView();

	// sort
    void    SortChange();
    void    SortAscDescend();
    void	SortNone();
    void	SortName();
	void	SortExt();
	void	SortSize();
	void	SortTime();
	void	SortColor();
	void	ColumnAuto();
	void	Column1();
	void	Column2();
	void	Column3();
	void	Column4();

	void	SettingChange();

	void	DefaultCfgFileChg();
	void	ColorCfgFileChg();
	void	KeyCfgFileChg();

	void	Copy();
	void	Move();
	void	MountList();
	void	SizeInfo();

	void	Forward();
	void	Back();

	void	NewFile();
	void	TouchFile();
	
	void	FileCompress(int nType, const string& sName = "");
	void	Extract();

	void	TargzCompress() 	{ FileCompress(0); }
	void	Tarbz2Compress()   { FileCompress(1); }
	void	ZipCompress()    	{ FileCompress(2); }

	void	Execute();
	void	QCD();
	void	Chmod();
	void	ViewClip();

	void	SyncDirectory();
	
	void	ConsoleMode();
	void	Chown();
	void	Chgrp();

	void	Diff();
	void	FileFind();

protected:
	void	CutPaste();
	void	CopyPaste();
}; // class CommandImp

}; // namespace

#endif
