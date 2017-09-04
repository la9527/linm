#ifndef __QT_MAINWINDOW_H__
#define __QT_MAINWINDOW_H__

#include <qmainwindow.h>

class Qt_TabPanel;
class QWidget;
class PanelToolTip;
class PanelStatusBar;
class CentralMain;
class Qt_Mcd;

class Qt_MainWindow:public QMainWindow
{
public:
	Qt_MainWindow( PanelToolTip* pToolTip = 0, QWidget* parent = 0 );
	~Qt_MainWindow();

	Qt_Panel* 	GetFocusPanel();
	Qt_Mcd*		GetFocusMcd();

	bool		isSplit();
	void		Refresh();
	void		Split();
	void		NextFocus();

protected:
	void	closeEvent( QCloseEvent* event );

	void 	keyPressEvent( QKeyEvent* event );
	void	MenuDraw();

private:
	CentralMain*		_pCentralMain;
	PanelCmd*			_pPanelCmd;
};

#endif //__QT_MAINWINDOW_H__
