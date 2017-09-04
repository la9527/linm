#ifndef __QT_COMMAND_H__
#define __QT_COMMAND_H__

#include <qobject.h>

class Qt_Panel;
class Qt_MainWindow;

class PanelCmd:public QObject
{
Q_OBJECT

public:
	PanelCmd(QObject * parent = 0 );

	void	SetMain( Qt_MainWindow* pMain );

	int		Run( const std::string &cmd, bool bPause, bool bBackground);
	int		ParseAndRun(const std::string &p, bool Pause);

public slots:
	void	Refresh();
	void	Left();
	void	Right();
	void	Up();
	void	Down();
	void	Enter();
	void	Home();
	void	End();
	void	PgDn();
	void	PgUp();
	void	Select();
	void	Archive();
	void	RemoteConnect();
	void	RemoteClose();
	void	Split();
	bool	Quit();
	void	Empty();
	void	NextWindow();

protected:
	void	GetMainPanel();

private:
	Qt_Panel*		_pPanel;
	Qt_MainWindow*	_pMain;
};

#endif // __QT_COMMAND_H__
