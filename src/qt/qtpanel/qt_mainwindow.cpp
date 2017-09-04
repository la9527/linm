#include <QKeyEvent>
#include <QWidget>
#include <QMainWindow>

#include "define.h"
#include "strutil.h"
#include "mlslog.h"
#include "exception.h"
#include "mlscfgload.h"

#include "qt_dialog.h"
#include "qt_paneltooltip.h"
#include "qt_panel.h"
#include "qt_mcd.h"
#include "qt_statusbar.h"
#include "qt_tabpanel.h"

#include "qt_mainwindow.h"
#include "qt_command.h"
#include "qt_menu.h"

using namespace MLS;
using namespace MLSUTIL;

Qt_MainWindow::Qt_MainWindow( PanelToolTip* pToolTip, QWidget* parent ):
	QMainWindow( parent/*, Qt::WGroupLeader*/ )
{
	setFocusPolicy( Qt::NoFocus ); // for menu

	_pPanelCmd = new PanelCmd( this );
	_pPanelCmd->SetMain( this );

	MenuDraw();
	
	_pCentralMain = new CentralMain( pToolTip, _pPanelCmd, this );
	setCentralWidget( _pCentralMain );
	resize( 800, 600 );

	//QTimer::singleShot( 100, _pCentralMain, SLOT( ) );
}

Qt_MainWindow::~Qt_MainWindow()
{
}

Qt_Panel* 	Qt_MainWindow::GetFocusPanel()
{
	return _pCentralMain->GetFocusPanel();
}

void	Qt_MainWindow::NextFocus()
{
	_pCentralMain->NextFocus();
}

Qt_Mcd* 	Qt_MainWindow::GetFocusMcd()
{
	return _pCentralMain->GetFocusMcd();
}

void	Qt_MainWindow::Refresh()
{
	_pCentralMain->Refresh();
}

void	Qt_MainWindow::Split()
{
	_pCentralMain->Split();
}

void	Qt_MainWindow::MenuDraw()
{
	LinMMenu	tMenu( this, _pPanelCmd );

	MenuCategory files(_("&File"));
		files.AddItem("document-new", _("&New"), "Cmd_NewFile", true);
		files.AddItem("", _("Touch File"), "Cmd_TouchFile", true);
		files.AddItem("", "", "");
		files.AddItem("", _("FileInfo"), "Cmd_FileInfo");
		files.AddItem("", "", "");
		files.AddItem("dialog-close", _("&Quit LinM"), "Cmd_Quit", true);
	tMenu.AddCategory(files);

	MenuCategory edit(_("&Edit"));
		edit.AddItem("", "", "");
		edit.AddItem("edit-cut", _("ClipCut"), "Cmd_ClipCut", true);
		edit.AddItem("edit-copy", _("ClipCopy"), "Cmd_ClipCopy", true);
		edit.AddItem("edit-paste", _("ClipPaste"), "Cmd_ClipPaste", true);
		edit.AddItem("", "", "");
		edit.AddItem("", _("Select &All"), "Cmd_SelectAll");
		edit.AddItem("", _("&Invert select"), "Cmd_SelectInvert");
		edit.AddItem("", "", "");
		edit.AddItem("edit-copy", _("Copy"), "Cmd_Copy");
		edit.AddItem("go-jump",   _("Move"), "Cmd_Move");
		edit.AddItem("edit-rename", _("Rename"), "Cmd_Rename");
		edit.AddItem("edit-delete", _("Delete"), "Cmd_Remove");
		edit.AddItem("", "", "");
		edit.AddItem("edit-find", _("&Find"), "Cmd_FileFind");
		edit.AddItem("", _("Diff"), "Cmd_Diff");
		edit.AddItem("", "", "");
		edit.AddItem("", _("Chmod"), "Cmd_Chmod");
		edit.AddItem("", _("Chown"), "Cmd_Chown");

	tMenu.AddCategory(edit);

	MenuCategory dire(_("&Directory"));
		dire.AddItem("", "Mcd", "Cmd_MCD");
		dire.AddItem("", "Qcd", "Cmd_QCD");
		dire.AddItem("", "", "");
		dire.AddItem("", _("Mount List"), "Cmd_MountList");
		dire.AddItem("", _("&SyncDir"), "Cmd_SyncDirectory");
		dire.AddItem("", "", "");
		dire.AddItem("folder_new", _("Mkdir"), "Cmd_Mkdir", true);
		dire.AddItem("go-up", _("To parent"), "Cmd_GoParent", true);
		dire.AddItem("", _("To root"), "Cmd_GoRoot");
		dire.AddItem("go-home", _("To home"), "Cmd_GoHome", true);
		dire.AddItem("", "", "");
		dire.AddItem("media-skip-backward", _("&Back"), "Cmd_Back", true);
		dire.AddItem("media-skip-forward", _("&Forward"), "Cmd_Forward", true);

	tMenu.AddCategory(dire);	

	MenuCategory run(_("&Run"));
		run.AddItem("system-run", _("&Run"), "Cmd_Enter");
		run.AddItem("system-run", _("Run(select)"), "Cmd_Execute");
		run.AddItem("", "", "");
		run.AddItem("", _("Console Mode"), "Cmd_ConsoleMode");
		run.AddItem("", _("Shell command"), "Cmd_Shell");
		run.AddItem("", "", "");
		run.AddItem("", _("View console"), "Cmd_ESC");
	
	tMenu.AddCategory(run);

	MenuCategory util(_("&Util"));
		util.AddItem("network-connect", _("&Remote Connect"), "Cmd_RemoteConnProps");
		util.AddItem("network-connect", _("&Quick Connect"), "Cmd_RemoteConnect");
		util.AddItem("network-disconnect", _("&Disconnect"), "Cmd_RemoteClose");
		util.AddItem("", "","");
		util.AddItem("package-x-generic", _("&Extract"), "Cmd_Extract");
		util.AddItem("", "", "");
		util.AddItem("application-x-tarz", _("Compress (tar.gz)"), "Cmd_TargzComp");
		util.AddItem("application-x-bzip", _("Compress (tar.bz2)"), "Cmd_Tarbz2Comp");
		util.AddItem("application-zip", _("Compress (zip)"), "Cmd_ZipComp");		
	tMenu.AddCategory(util);

	MenuCategory view(_("&View"));
		view.AddItem("view-refresh", _("&Refresh"), "Cmd_Refresh");
		view.AddItem("", "", "");
		view.AddItem("", _("Column AUTO"), "Cmd_ColumnAuto");
		view.AddItem("", "", "");
		view.AddItem("", _("Column 1"), "Cmd_Column1");
		view.AddItem("", _("Column 2"), "Cmd_Column2");
		view.AddItem("", _("Column 3"), "Cmd_Column3");
		view.AddItem("", _("Column 4"), "Cmd_Column4");
		util.AddItem("", "", "");
		util.AddItem("", _("ViewClip"), "Cmd_ViewClip");
		view.AddItem("", "", "");
		view.AddItem("", _("Hide hidden file"), "Cmd_HiddenFileView");
		view.AddItem("", _("Show owner"), "Cmd_FileOwnerView");
		
		view.AddItem("", _("Sort Change"), "Cmd_SortChange");
		view.AddItem("", _("Sort Asc/Descend"), "Cmd_SortAscDescend");
		view.AddItem("", "", "");
		view.AddItem("", _("Split"),  "Cmd_Split");
		view.AddItem("", _("Next window"), "Cmd_NextWindow");
		view.AddItem("", "", "");
		view.AddItem("", _("SplitSync On/Off"), "Cmd_SplitViewSync");
	tMenu.AddCategory(view);

	MenuCategory op(_("&Option") );
		op.AddItem("", _("Linecode Change"), "Cmd_BoxCodeChange");
		op.AddItem("", _("Locale Change"), "Cmd_LangChange");
		op.AddItem("", "", "");
		op.AddItem("", _("Edit Config"), "Cmd_DefaultCfgFileChg");
		op.AddItem("", _("Edit Keybind"), "Cmd_KeyCfgFileChg");
		op.AddItem("", _("Edit Colorset"), "Cmd_ColorCfgFileChg");
	tMenu.AddCategory(op);	

	MenuCategory mls("&Help");
		mls.AddItem("linm", _("&About"), "Cmd_About");
		mls.AddItem("", "", "");
		mls.AddItem("help-about", _("&Help"), "Cmd_Help");
	tMenu.AddCategory(mls);
}

void 	Qt_MainWindow::keyPressEvent( QKeyEvent* event )
{
	qDebug("Qt_MainWindow :: [%d]", event->key() );

	switch( event->key() )
	{
		case Qt::Key_Left:
			qDebug("Qt_MainWindow :: KEYLEFT" );
			break;
		case Qt::Key_Right:
			qDebug("Qt_MainWindow :: KEYRIGHT" );
			break;
	}

	//QWidget::keyPressEvent( event );
}

void	Qt_MainWindow::closeEvent( QCloseEvent* event )
{
	Qt_Panel* pPanel = GetFocusPanel();

	if (g_tCfg.GetBool("Default", "AskMlsExit") == true)
	{
		bool bYN = YNBox(_("LinM Quit"), _("Do you really want to quit the LinM ?"), true);
		if (bYN == false)
		{
			event->ignore();
			return;
		}
	}
	
	if ( pPanel && pPanel->GetReader() )
	{
		// 마지막 mls 가 마지막으로 사용한 디렉토리를 저장한다.
		if (pPanel->GetReader()->GetReaderName() == "file")
		{
			FILE *fp= fopen((g_tCfg.GetValue("Static", "CfgHome") + "path").c_str(), "wb");
	
			if (fp)
			{
				fputs(pPanel->GetPath().c_str(), fp);
				fclose(fp);
			}
		}
	}

	event->accept();
}

bool Qt_MainWindow::isSplit()
{
	return _pCentralMain->isSplit();
}


