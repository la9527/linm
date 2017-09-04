#include "selection.h"

#include "qt_dialog.h"
#include "qt_panel.h"
#include "qt_mcd.h"
#include "qt_command.h"
#include "qt_mainwindow.h"

using namespace MLSUTIL;

PanelCmd::PanelCmd(QObject * parent )
	: QObject ( parent ) 
{
	_pPanel = 0;
	_pMain = 0;
}

void	PanelCmd::SetMain( Qt_MainWindow* pMain )
{
	_pMain = pMain;
}

void	PanelCmd::Empty()
{
	qDebug("PanelCmd::Cmd_Empty()");
}

void	PanelCmd::GetMainPanel()
{
	if ( _pMain )
		_pPanel = _pMain->GetFocusPanel();
}

void	PanelCmd::Refresh()
{
	_pPanel = _pMain->GetFocusPanel();
	_pMain->Refresh();
	_pPanel->setFocus();
}

void	PanelCmd::Left()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Left();
}

void	PanelCmd::Right()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Right();
}

void	PanelCmd::Up()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Up();
}

void	PanelCmd::Down()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Down();
}

void	PanelCmd::PgDn()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_PageDown();
}

void	PanelCmd::PgUp()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_PageUp();
}

void	PanelCmd::End()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_End();
}

void	PanelCmd::Home()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Home();
}

void	PanelCmd::Enter()
{
	_pPanel = _pMain->GetFocusPanel();

	const string& strBefInitType = _pPanel->GetReader()->GetInitType();

	_pPanel->MLS::Panel::Key_Enter();

	Qt_Mcd* pMcd = _pMain->GetFocusMcd();

	if ( strBefInitType != _pPanel->GetReader()->GetInitType() )
		pMcd->InitMcd( _pPanel->GetReader(), "/" );

	pMcd->setDir( _pPanel->GetPath() );
	_pPanel->TabLabelChg();
}

void	PanelCmd::Select()
{
	_pPanel = _pMain->GetFocusPanel();

	_pPanel->MLS::Panel::Key_Select();
}

bool	PanelCmd::Quit()
{
	_pPanel = _pMain->GetFocusPanel();

	if ( _pPanel->GetReader()->GetReaderName() == "sftp" || 
		 _pPanel->GetReader()->GetReaderName() == "ftp")
	{
		RemoteClose();
		return false;
	}

	_pMain->close();
	return true;
}

void	PanelCmd::Split()
{
	_pMain->Split();
}

void PanelCmd::RemoteConnect()
{
	_pPanel = _pMain->GetFocusPanel();

	//if (g_tMainFrame.GetActiveViewType() != PANEL) return;

	if (_pPanel->GetReader()->GetReaderName() != "file" )
	{
		MsgBox(_("Error"), _("current remote connected."));
		return; 
	}

	string sConnectionInfo;

	sConnectionInfo = MLS::g_tCfg.GetValue("Default", "LastRemoteConnected");

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

	if (_pPanel->GetReader()->GetReaderName() == "ftp" ||
		_pPanel->GetReader()->GetReaderName() == "sftp")
	{
		MLS::g_tCfg.SetValue(	"Default", 
							"LastRemoteConnected", 
							_pPanel->GetReader()->GetInitType());
	}

	Refresh();
}

void PanelCmd::RemoteClose()
{
	_pPanel = _pMain->GetFocusPanel();

	//if (g_tMainFrame.GetActiveViewType() != PANEL) return;

	if (MLS::g_tCfg.GetBool("Default", "AskRemoteExit") == true)
	{
		bool bYN = YNBox(_("Remote Close"), _("Do you really want to remote connection close ?"), true);
		if (bYN == false) return;
	}

	_pPanel->PluginClose("file");
	Refresh();
}

void PanelCmd::Archive()
{
	_pPanel = _pMain->GetFocusPanel();

	MLS::File* pFile = _pPanel->GetCurFile();

	qDebug("ArchiveFileView [%s]", pFile->sFullName.c_str());

	if (pFile->sType.substr(0, 7) == "archive")
	{
		MsgBox(_("Error"), "Archive file view failure !!!");
	}
	else
	{
		if (_pPanel->GetReader()->GetReaderName() != "file")
			_pPanel->PluginOpen(MLS::g_tCfg.GetValue("Static", "TmpDir") + pFile->sName, "archive");
		else
			_pPanel->PluginOpen(pFile->sFullName, "archive");
	}
}

///	@brief	명령어를 실행한다.
///	@param	cmd		명령어및 인자
///	@param	bPause	명령어를 실행시키고 종료시 잠시 정지시킬지 여부
/// @param	bFork	Fork할 것인지?
///	@return	명령어 실행 결과
int 	PanelCmd::Run( const std::string &cmd, bool bPause, bool bBackground)
{
	_pPanel = _pMain->GetFocusPanel();

	// command 창에서 exit, quit 를 치면 종료하게 한다.
	if (cmd == "exit" || cmd == "quit") 
	{
		Quit();
		return 0;
	}

	int status;

	if (_pPanel->GetReader()->GetReaderName() == "file")
	{
		chdir(_pPanel->GetReader()->GetPath().c_str());
	}

	if (bBackground)
	{
		//cout << "linm $ " << cmd.c_str() << endl;
		string sCmd = cmd + " > /dev/null 2>&1 &";
		status = system(sCmd.c_str());
	}
	else
	{
		//cout << "linm $ " << cmd.c_str() << endl;		
		string sConsoleName = MLS::g_tCfg.GetValue("Qt_Setting", "UseConsole", "konsole");	
		sConsoleName = sConsoleName + " " + cmd;
		status = system( sConsoleName.c_str() );
	}

	if (bPause)
	{
		
	}

	//CursesInit();
	Refresh();
	return 0;
}

///	@brief	모든 커맨드 메시지를 관리
///	@param	p	실행 시킬 명령어
///	@return		명령어 실행 결과
int 	PanelCmd::ParseAndRun(const string &p, bool Pause)
{
	bool	bFlag  = false,
		 	bPause = Pause,
		 	bParam = false,
		 	bMcd   = false ,
		 	bConfirm = false,
			bBackground = false,
			bRootExe = false;

	_pPanel = _pMain->GetFocusPanel();
			
	// cd 구현
	if (p.substr(0, 2) == "cd")
	{
		string sStr;
		if (p == "cd" || p == "cd ")
			sStr = _pPanel->GetReader()->GetRealPath("~");
		else
			sStr = _pPanel->GetReader()->GetRealPath(p.substr(3));
		sStr = Replace(sStr, "\\", "");
		if (sStr != "")	
		{
			if (_pPanel->Read(sStr) == true) 
			{
				return 1;
			}
		}
		return 0;
	}
	
	string 	arg, tmp_name;

	MLS::File	tFile;
	
	// 파일을 복사 해서 보여주기 위한.
	{
		MLS::File* 	pFile = _pPanel->GetCurFile();
		if (_pPanel->GetReader()->View(pFile, &tFile) == false)
		{
			//MsgBox(	_("Error"), _("file reading failure !!!!"));
			return 0;
		}
	}
	
	// 파싱
	for (string::const_iterator i = p.begin(); i!= p.end(); i++)
	{
		if (*i == '%')
		{
			bFlag = true;
			continue;
		}
		
		if (bFlag)
		{
			switch(*i)
			{
			// 파일을 리턴해줌
			case '1':
			case 'F':
				arg += addslash(tFile.sFullName);
				break;
				
			case 'N':
			{
				if (tFile.sName[0]=='.') return 0;
				
				string::size_type p = tFile.sName.rfind('.');
				
				if (p != string::npos)
					arg += addslash(tFile.sFullName.substr(0, p-1));
				else
					arg += addslash(tFile.sFullName);
				break;
			}
			
			case 'E':
				if (tFile.Ext().empty()) return 0;
				arg += addslash(tFile.Ext());
				break;
				
			case 'W':
				bPause = true;
				break;
				
			case '%':
				arg += '%';
				break;
				
			case 'S':
			{
				MLS::Selection tSelection;
				_pPanel->GetSelection(tSelection);
				
				if (tSelection.GetSize() == 0) return 0;				
				bool first = true;				
			
				for (int t = 0; t< (int)tSelection.GetSize(); t++)
				{
					if (first) first= false;
					else arg += ' ';
					arg += addslash(tSelection[t]->sFullName);
				}
				break;
			}
			
			case 's':
			{
				MLS::Selection tSelection;
				_pPanel->GetSelection(tSelection);
				
				if (!tSelection.GetSize() == 0)
				{
					bool first = true;					
				
					for (int t = 0; t< (int)tSelection.GetSize(); t++)
					{
						if (first) first= false;
						else arg += ' ';
					
						arg += addslash(tSelection[t]->sFullName);
					}
				}
				else 
				{
					if (tFile.sName == "..") return 0;
					arg += addslash(tFile.sFullName);
				}
				break;
			}
			
			case 'd':			
				arg += addslash(_pPanel->GetPath());
				break;		
			
			case 'D':
				bMcd = true;
				break;
			
			case 'P':
				bParam= true;
				break;

			case 'R': // root 계정 실행시
				bRootExe = true;
				break;
				
			case 'H':
				arg += addslash(_pPanel->GetReader()->GetRealPath("~"));
				break;
				
			case 'Q':
				bConfirm = true;
				break;
				
			case 'B':
				bBackground = true;
				break;
				
			default:
				break;
			}
			
			bFlag = false;
			continue;
		}
		else arg += *i;
	}
	
	if (bMcd)
	{
		/*
		Mcd mcd(_tPanel);
		mcd.setCur(_tPanel->GetCurrentPath());
		if (!mcd.proc())
            return 0;
		arg += addslash(mcd.getCur());
		*/
	}
	
	if (bParam)	if (InputBox(_("Edit Parameter"), arg)<0) return 0;
		
	if (bConfirm)
		if (YNBox(_("Question"), _("Do you want to run this operation?"), false) != true) return 0;

	if (bRootExe)
		arg = "su - --command=\"" + arg + "\"";
	
	// 실행
	int nRt = Run(arg, bPause, bBackground);
	return 0;
}

void PanelCmd::NextWindow()
{
	if ( _pMain->isSplit() )
		_pMain->NextFocus();
}
