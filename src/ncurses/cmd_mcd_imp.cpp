#include "cmd_mcd_imp.h"
#include "drawset.h"
#include "mainframe.h"
#include "selection.h"
#include "mlsmenu.h"
#include "archive.h"

using namespace MLS;
using namespace MLSUTIL;

void CmdMcdImp::UpdateConfig()
{
}

void CmdMcdImp::SaveConfig()
{
}

void	CmdMcdImp::SplitViewSync()
{
	g_tMainFrame.SetScreenSync(!g_tMainFrame.GetScreenSync());

	if (g_tMainFrame.GetScreenSync())
	{
		NCurses_Panel* pPanel = g_tMainFrame.GetPanel(1);
		NCurses_Mcd* pMcd = g_tMainFrame.GetMcd(0);
		
		void*	pWait = MsgWaitBox(	_("Wait"),
									_("Please wait !!! - Cancel Key [ESC]"));

		if ( _pMcd->GetReader() && _pMcd->GetReader()->GetReaderName() != "archive" )
		{
			string sSaveName = 	g_tCfg.GetValue("Static", "CfgHome") + "McdDirSave/" +
								_pMcd->GetReader()->GetInitType();
	
			// make the 'McdDirSave' directory in home directory. also saved the MCD tree infomation.
			mkdir((g_tCfg.GetValue("Static", "CfgHome") + "McdDirSave").c_str(), 0755);
			
			_pMcd->Save( sSaveName.c_str() );
			
		}
		pMcd->Destroy();
		pMcd->SetReader(pPanel->GetReader());
		McdLoad();
		MsgWaitEnd(pWait);
		ViewSync();
	}
}

void	CmdMcdImp::ViewSync()
{
	int nActive = g_tMainFrame.GetActiveNum();

	NCurses_Panel* pPanel = g_tMainFrame.GetPanel(1);
	NCurses_Mcd* pMcd = g_tMainFrame.GetMcd(0);

	pPanel->Read(pMcd->GetCurName());
	pMcd->_bFocus = true;
	pPanel->_bFocus = false;
	g_tMainFrame.Refresh(false);
}

void CmdMcdImp::Key_Enter()
{
	if (g_tMainFrame.GetScreenSync())
	{
		NCurses_Panel* pPanel = g_tMainFrame.GetPanel(1);
		NCurses_Mcd* pMcd = g_tMainFrame.GetMcd(0);
		
		pPanel->Read(pMcd->GetCurName());
		g_tMainFrame.Refresh(false);
	}
	else
	{
		McdSave();

		_pPanel->Read(_pMcd->GetCurName());
		g_tMainFrame.SetActiveViewType(PANEL);
		
		// when mcd copy in panel.
		switch( g_tMainFrame.GetMcdCopyMode() )
		{
			case CLIP_NONE:
				break;
			case CLIP_COPY:
			case CLIP_CUT:
				LOG("Key_Enter CLIP_COPY or CLIP_CUT");
				ClipPaste();
				g_tMainFrame.McdClipCopyClear();
				break;
		}

		switch( g_tMainFrame.GetMcdExeMode().eMcdExeMode )
		{
			case ARCHIVE_COPY:
			{
				string sName = g_tMainFrame.GetMcdExeMode().sData;
				Archive		tArchive( sName );
				
				string sView = _("Please wait !!! - Uncompress file [") + sName + "]";
				void*  pWin = MsgWaitBox( 	_("Wait"), sView);
				
				tArchive.Uncompress(_pMcd->GetCurName());
	
				MsgWaitEnd(pWin);
				g_tMainFrame.SetMcdExeMode(MCD_EXEMODE_NONE);
				break;
			}

			case EXECUTE_QCD:
			{
				string sNum = g_tMainFrame.GetMcdExeMode().sData;
				_Config.SetValue("QCD", "QcdF" + sNum, _pMcd->GetCurName() );
				g_tMainFrame.SetMcdExeMode(MCD_EXEMODE_NONE);
				g_tMainFrame.GetCommand()->Execute( "Cmd_QCD" );
				break;
			}

			default:
				break;
		}

		Refresh();
	}
}

void CmdMcdImp::Key_Left()
{ 
	_pMcd->Key_Left(); 
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_Right() 		
{ 
	_pMcd->Key_Right(); 
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_Up()
{ 
	_pMcd->Key_Up(); 
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_Down()
{ 
	_pMcd->Key_Down();
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_Home()
{ 
	_pMcd->Key_Home();
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_End() 		
{ 
	_pMcd->Key_End();
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_PageUp()
{
	_pMcd->Key_PageUp(); 
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Key_PageDown()
{
	_pMcd->Key_PageDown(); 
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::Menu()
{
	McdMenu		tMcdMenu;
	vector<string>	vDisableList;

	const string&	sReaderName = _pMcd->GetReader()->GetReaderName();

	if (sReaderName == "file")
	{
		vDisableList.push_back("Cmd_RemoteClose");
	}
	else if (sReaderName == "ftp" || sReaderName == "sftp")
	{
		vDisableList.push_back("Cmd_RemoteConnect");
	}
	else
	{
		vDisableList.push_back("Cmd_RemoteConnect");
		vDisableList.push_back("Cmd_RemoteClose");
	}

	tMcdMenu.SetDisable(vDisableList);
	tMcdMenu.SetMcd( _pMcd );
	tMcdMenu.Create();
	tMcdMenu.Do();

	g_tMainFrame.Refresh(false);
	if (tMcdMenu.GetCurCmd().size() != 0)
	{
		LOG("Menu Run [%s]", tMcdMenu.GetCurCmd().c_str());McdSave();
		g_tMainFrame.GetCommand()->Execute(tMcdMenu.GetCurCmd());
	}
}

void CmdMcdImp::Quit()
{
	McdSave();

	if (!g_tMainFrame.GetScreenSync())
	{
		g_tMainFrame.SetActiveViewType(PANEL);
		Refresh();
	}

	switch( g_tMainFrame.GetMcdCopyMode() )
	{
		case CLIP_NONE:
			break;
		case CLIP_COPY:
		case CLIP_CUT:
			g_tMainFrame.McdClipCopyClear();
			Refresh();
			break;
	}
	
	g_tMainFrame.SetMcdExeMode(MCD_EXEMODE_NONE);
}

void CmdMcdImp::Split()
{
	g_tMainFrame.Split();
}

void CmdMcdImp::NextWindow()
{
	g_tMainFrame.NextWindow();
}

void CmdMcdImp::ClearRefresh()
{
	ScreenClear();
	Refresh();
}

void CmdMcdImp::Refresh()
{
	string sFileName1, sFileName2;
	if (g_tMainFrame.GetPanel(0)->GetCurFile() != NULL)
		 sFileName1 = g_tMainFrame.GetPanel(0)->GetCurFile()->sFullName;
	if (g_tMainFrame.GetPanel(1)->GetCurFile() != NULL)
		 sFileName2 = g_tMainFrame.GetPanel(1)->GetCurFile()->sFullName;
	
	g_tMainFrame.GetPanel(0)->Read(g_tMainFrame.GetPanel(0)->GetPath());
	g_tMainFrame.GetPanel(1)->Read(g_tMainFrame.GetPanel(1)->GetPath());

	if ( !sFileName1.empty() )
		g_tMainFrame.GetPanel(0)->SetCurFileName( sFileName1 );
	if ( !sFileName2.empty() )
		g_tMainFrame.GetPanel(1)->SetCurFileName( sFileName2 );
	g_tMainFrame.Refresh();
}

void CmdMcdImp::RemoteConnect()
{
	if (g_tMainFrame.GetActiveViewType() == EDITOR) return;

	if (_pPanel->GetReader()->GetReaderName() == "sftp" ||
		_pPanel->GetReader()->GetReaderName() == "ftp") 
	{
		MsgBox(_("Error"), _("current remote connected."));
		return; 
	}

	string sConnectionInfo;
	if (InputBox(	_("Input sftp(ftp) connect url (sftp://user:pswd@hostname)"),
							sConnectionInfo) == ERROR) return;

	if (sConnectionInfo.size() > 6)
	{
		if (sConnectionInfo.substr(0, 6) == "ftp://")
		{
			sConnectionInfo = sConnectionInfo.substr(6);
			_pPanel->PluginOpen(sConnectionInfo, "ftp");
		}
		else if (sConnectionInfo.substr(0, 7) == "sftp://")
		{
			sConnectionInfo = sConnectionInfo.substr(7);
			_pPanel->PluginOpen(sConnectionInfo, "sftp");
		}
		else
		{
			_pPanel->PluginOpen(sConnectionInfo, "sftp");
		}
	}
	else
		MsgBox(_("Error"), _("input url invalid format."));

	if (_pPanel->GetReader()->GetConnected() == true)
	{
		McdSave();
		_pMcd->Destroy();
		_pMcd->SetReader(_pPanel->GetReader());
		McdLoad();
	}
	Refresh();
}

void CmdMcdImp::RemoteClose()
{
	if (g_tMainFrame.GetActiveViewType() == EDITOR) return;

	if (g_tCfg.GetBool("Default", "AskRemoteExit") == true)
	{
		bool bYN = YNBox(_("Remote Close"), _("Do you really want to remote connection close ?"), true);
		if (bYN == false) return;
	}

	_pPanel->PluginClose("file");

	McdSave();
	_pMcd->Destroy();
	_pMcd->SetReader(_pPanel->GetReader());
	McdLoad();
	Refresh();
}

void CmdMcdImp::Remove()
{
	File 	tFile = _pMcd->GetCurFile();

	if (tFile.sFullName == "/")
	{
		MsgBox(_("Error"), _("remove not root directory. !!!"));
		return;
	}

	Key_Left();
	
	string sBefPath = _pMcd->GetCurFile().sFullName;

	Selection& tMainSelection = *(g_tMainFrame.GetSelection());
	tMainSelection.Clear();
	tMainSelection.Add( &tFile );
	
	tMainSelection.SetSelectPath(sBefPath);
	tMainSelection.ExpandDir(_pPanel->GetReader(), false);
	_pPanel->Read(sBefPath);
	_pPanel->GetReader()->Remove(tMainSelection);

	_pMcd->SubDirAnySearch(2);
	_pMcd->SetCur( _pPanel->GetPath() );
	if (g_tMainFrame.GetScreenSync()) ViewSync();
	
	LOG("Remove :: Cur Set ---- [%s]", _pPanel->GetPath().c_str());
	Refresh();
}

void CmdMcdImp::ClipCopy()
{
	Selection& tMainSelection = *(g_tMainFrame.GetSelection());

	File 	tFile = _pMcd->GetCurFile();
	tMainSelection.Clear();
	tMainSelection.Add(&tFile);
	tMainSelection.SetSelectPath(_pMcd->GetParentFile().sFullName);
	tMainSelection.SetReader(_pPanel->GetReader());

	if (tMainSelection.GetSize() == 0)
		g_tMainFrame.GetClip()->Set(CLIP_NONE);
	else
		g_tMainFrame.GetClip()->Set(CLIP_COPY);
	
	LOG("Mcd ClipCopy [%s] [%s]",
				_pMcd->GetParentFile().sFullName.c_str(),
				tFile.sFullName.c_str());
}

void CmdMcdImp::ClipCut()
{
	Selection& tMainSelection = *(g_tMainFrame.GetSelection());

	File 	tFile = _pMcd->GetCurFile();
	tMainSelection.Clear();
	tMainSelection.Add(&tFile);
	tMainSelection.SetSelectPath(_pMcd->GetParentFile().sFullName);
	tMainSelection.SetReader(_pPanel->GetReader());

	if (tMainSelection.GetSize() == 0)
		g_tMainFrame.GetClip()->Set(CLIP_NONE);
	else
		g_tMainFrame.GetClip()->Set(CLIP_CUT);

	LOG("Mcd ClipCut [%s] [%s]",
				_pMcd->GetParentFile().sFullName.c_str(),
				tFile.sFullName.c_str());
}

void CmdMcdImp::ClipPaste()
{
	// again be input..
	if (g_tMainFrame.GetActiveViewType() == EDITOR) return;

	string sBefPath = _pMcd->GetCurFile().sFullName;
	_pPanel->Read(_pMcd->GetCurFile().sFullName);

	ClipState state = g_tMainFrame.GetClip()->GetState();

	if (state == CLIP_NONE) 
	{
		LOG("CmdMcdImp None !!!");
		return;
	}

	if (state == CLIP_COPY)
	{
		LOG("CmdMcdImp Copy !!!");
		CopyPaste();
	}
	else if (state == CLIP_CUT)
	{
		LOG("CmdMcdImp Cut !!!");
		CutPaste();
	}

	_pMcd->Rescan(2);
	_pMcd->AddDirectory( _pPanel->GetPath() );
	_pMcd->SetCur( _pPanel->GetPath() );
	
	if (g_tMainFrame.GetScreenSync()) ViewSync();

	LOG("Cur Set ---- [%s]", sBefPath.c_str());
}

void	CmdMcdImp::CopyPaste()
{
	Selection& _tMainSelection = *(g_tMainFrame.GetSelection());

	if (_tMainSelection.GetSize() <= 0) return;
	
	Selection	tSelectionCopy;

	LOG("ClipPaste Copy Start 1 - [%d] [%s] [%s] !!!",
						_tMainSelection.GetSize(),
						_tMainSelection[0]->sType.c_str(), 
						_pPanel->GetReader()->GetInitType().c_str());

	// if Source or Target is file, to accept.
	if (_tMainSelection[0]->sType == "file://" && 
		_pPanel->GetReader()->GetInitType() == "file://")
	{
		_tMainSelection.ExpandDir(_tMainSelection.GetReader(), false);
		_pPanel->GetReader()->Copy(_tMainSelection);
	}
	else
	{
		bool	bUseGlobalReaderCtl = false;

		// Because panel window are two panel. reads the type to matched in panel of mainframe.
		Reader* pReader = g_tMainFrame.GetPanel(0)->GetReader(_tMainSelection[0]->sType);
		if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType)
		{
			pReader = g_tMainFrame.GetPanel(1)->GetReader(_tMainSelection[0]->sType);
			
			if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType)
			{
				// if not found, to create additional.
				string::size_type p = _tMainSelection[0]->sType.find("://");
				string sInitName;

				if (p == string::npos)
					sInitName = _tMainSelection[0]->sType;
				else
					sInitName = _tMainSelection[0]->sType.substr(0, p);

				// get g_tReaderCtl.
				pReader = g_tReaderCtl.Get(sInitName);
				if (pReader == NULL)
				{
					MsgBox(_("Error"), _("plugin not found."));
					return;
				}

				LOG("g_tReaderCtl USE [%p] [%s]", pReader, sInitName.c_str());

				bUseGlobalReaderCtl = true;	
				if (p == string::npos)
					sInitName = _tMainSelection[0]->sType;
				else
					sInitName = _tMainSelection[0]->sType.substr(p+3);

				pReader->Init(sInitName);
				if (_pPanel->GetReader()->GetConnected() == false) return;
			}
		}

		LOG("File Copy !!!");

		if (_pPanel->GetReader()->GetConnected() == false)
		{
			_pPanel->GetReader()->Init(_tMainSelection[0]->sType);
			if (_pPanel->GetReader()->GetConnected() == false) return;
		}
		
		_tMainSelection.ExpandDir(pReader, false);

		// copy to temp directory and then, copy from temp directory to copy directoy.
		if (pReader->Copy(_tMainSelection, g_tCfg.GetValue("Static", "TmpDir"), &tSelectionCopy))
		{
			LOG("File Copy Complite !!!");
		
			if (_pPanel->GetReader()->GetInitType() == "file://")
			{
				LOG("File Copy Move !!!");

				// if init type is 'file', move to files.
				_pPanel->GetReader()->Move(tSelectionCopy);
			}
			else
			{	
				LOG("File Copy TmpDir Remove !!!");

				// if init type is not 'file', copy from the temp directory to the paste directoy and then,
				// temp directoy is remove.				
				if (_pPanel->GetReader()->Paste(tSelectionCopy))
					if ((pReader=_pPanel->GetReader("file://")) != NULL)
						pReader->Remove(tSelectionCopy, false);
			}
		}

		if (bUseGlobalReaderCtl == true)
			pReader->Destroy();
	}
}

void 	CmdMcdImp::CutPaste()
{
	Selection& _tMainSelection = *(g_tMainFrame.GetSelection());

	if (_tMainSelection.GetSize() <= 0) return;
	
	LOG("ClipPaste CLIP_CUT Start - [%d] [%s] [%s] !!!",
						_tMainSelection.GetSize(),
						_tMainSelection[0]->sType.c_str(),
						_pPanel->GetReader()->GetInitType().c_str());

	if (_tMainSelection[0]->sType == "file://" && _pPanel->GetReader()->GetInitType() == "file://")
	{
		_tMainSelection.ExpandDir(_tMainSelection.GetReader(), false);

		// if sType is file, just move file.
		_pPanel->GetReader()->Move(_tMainSelection);
	}
	else
	{
		bool	bUseGlobalReaderCtl = false;

		// Because panel window are two panel. reads the type to matched in panel of mainframe.
		Reader* pReader = g_tMainFrame.GetPanel(0)->GetReader(_tMainSelection[0]->sType);
		if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType)
		{
			pReader = g_tMainFrame.GetPanel(1)->GetReader(_tMainSelection[0]->sType);
			if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType)
			{
				// if not found, to create additional.
				string::size_type p = _tMainSelection[0]->sType.find("://");
				string sInitName;

				if (p != string::npos)
					sInitName = _tMainSelection[0]->sType;
				else
					sInitName = _tMainSelection[0]->sType.substr(0, p);

				// get g_tReaderCtl.
				pReader = g_tReaderCtl.Get(sInitName);
				if (pReader == NULL)
				{
					MsgBox(_("Error"), _("plugin not found."));
					return;
				}

				bUseGlobalReaderCtl = true;
				if (p == string::npos)
					sInitName = _tMainSelection[0]->sType;
				else
					sInitName = _tMainSelection[0]->sType.substr(p+3);

				pReader->Init(sInitName);
				if (pReader->GetConnected() == false) return;
			}
		}

		if (pReader->GetConnected() == false)
		{
			pReader->Init(_tMainSelection[0]->sType);
			if (pReader->GetConnected() == false) return;
		}

		_tMainSelection.ExpandDir(pReader, false);

		Selection	tSelectionMove;

		// copy to temp directory and then, copy from temp directory to copy directoy.
		if (pReader->Copy(_tMainSelection, g_tCfg.GetValue("Static", "TmpDir"), &tSelectionMove))
		{
			Reader*		pCurReader = _pPanel->GetReader();
			if (pCurReader->GetInitType() == "file://")
			{
				// if init type is 'file', move to files.
				if (pCurReader->Move(tSelectionMove))
					pCurReader->Remove(_tMainSelection, false); // move and than, original source is remove.
			}
			else
			{	
				// if init type is not 'file', copy from the temp directory to the paste directoy and then,
				// temp directoy is remove.
				if (pCurReader->Paste(tSelectionMove))
					if ((pReader = _pPanel->GetReader("file://")) != NULL)
					{
						pReader->Remove(tSelectionMove, false);	// temp directory remove.
						pCurReader->Remove(_tMainSelection, false); // original source remove.
					}
			}
		}

		if (bUseGlobalReaderCtl == true)
			pReader->Destroy();
	}
}

void	CmdMcdImp::Key_ESC()
{
	if (g_tMainFrame.GetScreenSync())
	{
		g_tMainFrame.SetScreenSync(false);
		return;
	}

	g_tMainFrame.SetActiveViewType(PANEL);
	Refresh();
}

void	CmdMcdImp::Rename()
{
	File 	tFile;
	string sBefPath = _pPanel->GetPath();
	tFile = _pMcd->GetCurFile();
	_pPanel->Read(_pMcd->GetCurName());
	_pPanel->Read("..");
	_pPanel->GetReader()->Rename(&tFile);
	_pMcd->SetCur( _pPanel->GetPath() );
	_pMcd->SubDirAnySearch(2);
	_pMcd->AddDirectory( tFile.sFullName );
	_pMcd->SetCur( tFile.sFullName );
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void	CmdMcdImp::Mkdir()
{
	string sBefPath = _pPanel->GetPath();
	_pPanel->Read(_pMcd->GetCurName());
	_pPanel->GetReader()->Mkdir(); 
	_pPanel->Read(sBefPath);
	_pMcd->SubDirAnySearch(2);
	_pMcd->AddDirectory( sBefPath );
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::GoRoot()
{ 
	_pPanel->Read("/");
	_pMcd->SetCur("/");
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::GoHome() 
{ 
	_pPanel->Read("~");
	string sPath =_pPanel->GetPath();
	_pMcd->SetCur(sPath);
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::GoParent()
{ 
	_pPanel->Read("..");
	_pMcd->Key_Left();
	string sPath =_pPanel->GetPath();
	_pMcd->SetCur(sPath);
	if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdMcdImp::HiddenFileView()
{
	_pMcd->SetHiddenView(!_pMcd->GetHiddenView());
}

void CmdMcdImp::AllScan()
{
	if ( YNBox(	_("All Rescan"), 
				_("Do you really want to directory all rescan ?"), 
				false) == false ) return;

	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [ESC]"));
	string sBefPath = _pMcd->GetCurFile().sFullName;

	_pMcd->Rescan();
	_pMcd->AddDirectory( sBefPath );
	_pMcd->SetCur( sBefPath );
	MsgWaitEnd(pWait);
}

void CmdMcdImp::Scan()
{
	void*	pWait = MsgWaitBox(	_("Wait"),
								_("Please wait !!! - Cancel Key [ESC]"));

	string sBefPath = _pMcd->GetCurFile().sFullName;

	_pMcd->Rescan(2);
	_pMcd->AddDirectory( sBefPath );
	_pMcd->SetCur( sBefPath );

	MsgWaitEnd(pWait);
}

void	CmdMcdImp::MountList()
{
	_pMcd->MountList();
	Refresh();
}

void 	CmdMcdImp::SizeInfo()
{
	Selection tSelection;

	File	tFile = _pMcd->GetCurFile();
	tSelection.Add(&tFile);
	
	void*  pWin = MsgWaitBox( 	_("Wait"),
								_("Please wait !!! - Cancel Key [ESC]"));

	tSelection.ExpandDir(_pPanel->GetReader(), true);

	MsgWaitEnd(pWin);
	
	String sMsg;
	sMsg.Append(_("  Selected files     %10s  /  %10s byte  "), 
				toregular(tSelection.GetSize()).c_str(),
				toregular(tSelection.CalcSize()).c_str());

	MsgBox(	_("Size info"), sMsg.c_str());
	Refresh();
}

void	CmdMcdImp::McdSave()
{
	if ( _pMcd->GetReader() && _pMcd->GetReader()->GetReaderName() != "archive" )
	{
		string sPath = _pMcd->GetReader()->GetInitType();
		sPath = Replace(sPath, "/", "\\");

		string sSaveName = 	g_tCfg.GetValue("Static", "CfgHome") + "McdDirSave/" +
							 sPath;

		// make 'McdDirSave' directory in '~/.linm'. and saved the MCD tree information.
		mkdir((g_tCfg.GetValue("Static", "CfgHome") + "McdDirSave").c_str(), 0755);
		
		_pMcd->Save( sSaveName.c_str() );
	}
}

void	CmdMcdImp::McdLoad()
{
	if ( _pPanel->GetReader() && _pPanel->GetReader()->GetReaderName() != "archive" )
	{
		string sPath = _pMcd->GetReader()->GetInitType();
		sPath = Replace(sPath, "/", "\\");
		string sLoadName = 	g_tCfg.GetValue("Static", "CfgHome") + "McdDirSave/" +
							 sPath;

		if ( !_pMcd->Load( sLoadName.c_str() ) )
			_pMcd->Rescan(2);
	}
	else
	{
		_pMcd->Rescan(2);		
	}
	_pMcd->AddDirectory(_pPanel->GetReader()->GetPath());
	_pMcd->SetCur(_pPanel->GetReader()->GetPath());
}
