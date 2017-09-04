#ifndef __CMD_COMMON_IMP_H__
#define __CMD_COMMON_IMP_H__

#include "define.h"
#include "mlslocale.h"
#include "mlscfgload.h"
#include "ncurses_panel.h"
#include "ncurses_mcd.h"
#include "ncurses_editor.h"

namespace MLS {

class CmdCommonImp
{
protected:
	NCurses_Panel*			_pPanel;
	NCurses_Mcd*			_pMcd;
	NCurses_Editor*			_pEditor;

public:
	CmdCommonImp()
	{
		_pPanel = NULL;
		_pMcd = NULL;
		_pEditor = NULL;
	}

	virtual ~CmdCommonImp() { }

	virtual void SetPanel(NCurses_Panel* p)		{ _pPanel = p; }
	virtual void SetMcd(NCurses_Mcd* p)			{ _pMcd = p; }
	virtual void SetEditor(NCurses_Editor* p)	{ _pEditor = p; }

	virtual NCurses_Mcd* 	GetMcd()		{ return _pMcd; }	
	virtual NCurses_Panel* 	GetPanel()		{ return _pPanel; }
	virtual NCurses_Editor* GetEditor()		{ return _pEditor; }

	virtual void	About();
	virtual void	Help();
	virtual void	ReloadConfigChange( bool bCursesRefresh = false );

	virtual void	BoxCodeChange();
	virtual void	LangChange();
};

};

#endif
