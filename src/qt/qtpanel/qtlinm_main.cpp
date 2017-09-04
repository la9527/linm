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

#include <fcntl.h>
#include <unistd.h>

#include <QApplication>
#include <QMessageBox>
#include <QTextCodec>
#include <QFont>

#ifdef 	LINM_KDE
	#include <kglobal.h>
	#include <kiconloader.h> // kdelibs4-dev
	#include <kapplication.h>
	#include <kcmdlineargs.h>
#endif

#include <qframe.h>

#include "define.h"
#include "strutil.h"
#include "mlslog.h"
#include "exception.h"
#include "panel.h"
#include "mlscfgload.h"
#include "colorcfgload.h"

#include "qt_dialog.h"
#include "qt_statusbar.h"
#include "qt_panel.h"
#include "qt_paneltooltip.h"
#include "qt_tabpanel.h"
#include "qt_mainwindow.h"

using namespace MLS;
using namespace MLSUTIL;

/*
void fontSelect()
{
	bool ok;
    QFont oldfont = tablefont;
    tablefont = QFontDialog::getFont(&ok, oldfont, this);

    if (ok)
        setFont(tablefont);
    else
        tablefont = oldfont;
}
*/

#define 	__LINM_CFGPATH__		"~/.linm"

namespace { // 처음 기본 세팅

vector<string> _vCfgFile, _vColFile, _vKeyFile;

}

/// @brief	mls 시작시 초기화 함수
///
///  config준비. 설정파일 읽기, 컬러셋 읽기
/// @return	성공여부 0일때 성공
bool Initialize()
{
	int t;
	const char *succMsg = "[\033[01;36m SUCCESS \033[0m]";
	const char *failMsg = "[\033[01;31m  FAIL   \033[0m]";
	const char *errMsg  = "[\033[01;31m  ERROR  \033[0m]";
	
	char*	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
	{
		String sMsg;
		sMsg.AppendBlank(60, "%s", strerror(errno));
		cout << sMsg.c_str();
		cout << errMsg << endl;
		return false;
	}
	if (access(cwd, R_OK | X_OK) == -1)
	{
		String sMsg;
		sMsg.AppendBlank(60, "%s", strerror(errno));
		cout << sMsg.c_str();
		cout << errMsg << endl;
		return false;
	}
	free(cwd);

	string sCfgDefaultPath, sCfgColorPath;
	
	sCfgDefaultPath = sCfgDefaultPath + __LINM_CFGPATH__ + "/default.cfg";
	sCfgColorPath = sCfgColorPath + __LINM_CFGPATH__ + "/colorset.cfg";

	{ // Config 준비..
		struct passwd *pw = getpwuid(getuid());
		{
			std::string home  = pw->pw_dir;
			home += '/';

			g_tCfg.SetStaticValue("Home", home);
		}

		// . config dir 지정
		g_tCfg.SetStaticValue("CfgHome", g_tCfg.GetValue("Static", "Home") + ".linm/");
		g_tCfg.SetStaticValue("TmpDir", g_tCfg.GetValue("Static", "Home") + ".linm/linm_tmpdir/");
		g_tColorCfg.Init();
		
		// 홈에 .linm를 만든다. mcd treeinfo를 저장하기 위해서도 필요
		mkdir((g_tCfg.GetValue("Static", "Home") + ".linm").c_str(), 0755);
		// 파일 복사에 필요한 tmp 디렉토리.
		mkdir((g_tCfg.GetValue("Static", "TmpDir")).c_str(), 0777);
	}

	{ // 설정 파일 읽기
		_vCfgFile.push_back( g_tCfg.GetValue("Static", "CfgHome") + "default.cfg" );
		_vCfgFile.push_back( sCfgDefaultPath );

		for (t=0; t<(int)_vCfgFile.size(); t++)
		{
			string cfgfile = _vCfgFile[t];

			if (g_tCfg.Load(cfgfile.c_str()))
			{
				g_tCfg.SetStaticValue("CfgFile", cfgfile);
#ifdef __DEBUGMODE__
				String sMsg;
				sMsg.AppendBlank(60, "Load configuration %s", cfgfile.c_str());
				cout << sMsg.c_str();
				cout << succMsg << endl;
#endif
				/*
				if (cfgfile == sCfgDefaultPath)
				{
					string sCmd = "cp " + sCfgDefaultPath + " " + g_tCfg.GetValue("Static", "Home") + ".linm";
					system(sCmd.c_str());
				}
				*/
				break;
			}
			else
			{
				String sMsg;
				sMsg.AppendBlank(60, "Load configuration %s", cfgfile.c_str());
				cout << sMsg.c_str();
				cout << failMsg << endl;
			}
		}

		if (t == (int)_vCfgFile.size())
		{
			String sMsg;
			sMsg.AppendBlank(60, "Load configuration ");
			cout << sMsg.c_str();
			cout << failMsg << endl;
			return false;
		}
	}
	
	{ // 컬러셋 읽기
		if ( g_tCfg.GetValue("Default", "ColorSetFile") != "")
			_vColFile.push_back( g_tCfg.GetValue("Static", "CfgHome") + g_tCfg.GetValue("Default", "ColorSetFile"));
		_vColFile.push_back( sCfgColorPath );
	
		for (t=0; t<(int)_vColFile.size(); t++)
		{
			string colfile = _vColFile[t];
			if (g_tColorCfg.Load(colfile.c_str()))
			{
				g_tCfg.SetStaticValue("ColFile", colfile);
#ifdef __DEBUGMODE__
				String sMsg;
				sMsg.AppendBlank(60, "Load colorset %s", colfile.c_str());
				cout << sMsg.c_str();
				cout << succMsg << endl;
#endif
				/*
				if (colfile == sCfgColorPath)
				{
					string sCmd = "cp " + sCfgColorPath + " " + g_tCfg.GetValue("Static", "Home") + ".linm";
					system(sCmd.c_str());
				}
				*/
				break;
			}
			else
			{
				String sMsg;
				sMsg.AppendBlank(60, "Load colorset %s", colfile.c_str());
				cout << failMsg << endl;
			}
		}

		if (t == (int)_vColFile.size())
		{
			String sMsg;
			sMsg.AppendBlank(60, "Load colorset");
			cout << sMsg.c_str();
			cout << failMsg << endl;
			return false;
		}
	}

	{ // Debuging Setting.
		string sLogFile;
	    sLogFile = ttyname(STDOUT_FILENO);

		qDebug("GetTty :: [%s]", sLogFile.c_str());
		g_Log.SetFile( sLogFile );

		int fd = 0;	
		if ((fd = open(sLogFile.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
		{
			printf("Log file open error : %s ", sLogFile.c_str());
			exit(0);
		}
		else
		{
			//dup2(fd, 2); // stderr에 리다이렉션한다.
			close(fd);
		}
	}
	return true;
}

/// @brief  Mls keybind파일, mls editor keybind 파일을 읽는다.
///
/// MainFrame 안에 KeyBind가 있기 때문에 따로 만듬.
/// @return keybind 파일 읽기 성공여부.
bool	Load_KeyFile()
{
	int t;
	const char *succMsg = "[\033[01;36m SUCCESS \033[0m]";
	const char *failMsg = "[\033[01;31m  FAIL   \033[0m]";
	string	sKeyCfgPath;
	sKeyCfgPath = sKeyCfgPath + __LINM_CFGPATH__ + "/qtkeyset.cfg";

	{// Key Binding file을 읽는다.
		if (g_tCfg.GetValue("Default", "QtKeySetFile") != "")
			_vKeyFile.push_back( g_tCfg.GetValue("Static", "CfgHome") + g_tCfg.GetValue("Default", "QtKeySetFile"));
		_vKeyFile.push_back( sKeyCfgPath );

		for (t=0; t<(int)_vKeyFile.size(); t++)
		{
			string keyfile = _vKeyFile[t];
			if ( g_tKeyCfg.Load(keyfile) )
			{
				g_tCfg.SetStaticValue("KeyFile", keyfile);
#ifdef __DEBUGMODE__
				String sMsg;
				sMsg.AppendBlank(60, "Load key settings... %s", keyfile.c_str());
				cout << sMsg.c_str();
				cout << succMsg << endl;
#endif
				if (keyfile == sKeyCfgPath)
				{
					string sCmd = "cp " + sKeyCfgPath + " " + g_tCfg.GetValue("Static", "Home") + ".linm";
					system(sCmd.c_str());
				}
				break;
			}
			else
			{
				String sMsg;
				sMsg.AppendBlank(60, "Load key settings... %s", keyfile.c_str());
				cout << sMsg.c_str();
				cout << failMsg << endl;
			}
		}

		if (t==(int)_vKeyFile.size())
		{
			String sMsg;
			sMsg.AppendBlank(60, "Load key settings... ");
			cout << sMsg.c_str();
			cout << failMsg << endl;
			return false;
		}
	}

#ifdef __DEBUGMODE__
	{
		String sMsg;
		sMsg.AppendBlank(60, "Mouse Wheel... ");
		cout << sMsg.c_str();

		#ifndef NCURSES_MOUSE_VERSION
			cout << failMsg << endl;
		#else
			cout << succMsg << endl;
		#endif
	}
#endif
	return true;
}


int main( int argc, char ** argv ) 
{
	#ifdef 	LINM_KDE
	KCmdLineArgs::init( argc, argv, "test", "Test" , ki18n("LinM") ,"0.8a" );
	KApplication app;
	#else
	QApplication app( argc, argv );
	#endif

	QFont       font = QApplication::font();
    font.setPointSize( 10 );
    font.setFamily(QString::fromUtf8("Arial"));
    QApplication::setFont( font );

	QApplication::setGlobalMouseTracking( FALSE );

	try
	{
		
		//app.setDefaultCodec( QTextCodec::codecForName("utf8") );
		QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF-8") );
		QTextCodec::setCodecForTr( QTextCodec::codecForName("UTF-8") );
		
		if (!Initialize())
		{
			QMessageBox::critical( NULL, QObject::tr("Error"), QObject::tr("config file not founed.(~/.linm/*.cfg") );
			return 0;
		}

		if (!Load_KeyFile())
		{
			QMessageBox::critical( NULL, QObject::tr("Error"), QObject::tr("Key file not found (qtkeyfile.cfg)") );
			return 0;
		}

		PanelToolTip*	pToolTip 	= new PanelToolTip();
		Qt_MainWindow*	pWin 		= new Qt_MainWindow( pToolTip );
		
		static Qt_Dialog		tDialog( pWin );
		static Qt_Progress		tProgress;

		SetDialogProgress( (MlsDialog*)&tDialog, (MlsProgress*)&tProgress);
		
		pWin->setCaption( "LinM - 0.8a" );

		const QPixmap&	iconLinm = LinMGlobal::GetSmallIcon( "linm" );
	
		if ( !iconLinm.isNull() )
			pWin->setIcon( iconLinm );

		pWin->show();

		app.setMainWidget( pWin );

		app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
		app.exec();
	}
	catch(Exception& ex)
	{
		qDebug("Exception :: %s", (char*)ex );
	}
	catch(...)
	{
		qDebug("Exception !!!!!! ");
	}
	return 0;
}
