#ifndef __MLS_CMD_EDITOR_IMP_H__
#define __MLS_CMD_EDITOR_IMP_H__

#include "cmd_common_imp.h"

#include "command.h"
#include "selection.h"

// PluginLoader.
#include "vfsplugin.h"
#include "readerctl.h"

namespace MLS {

class CmdEditorImp:public CmdCommonImp, public Configurable
{
public:
	CmdEditorImp() : CmdCommonImp()
	{
		UpdateConfig();
	}
	~CmdEditorImp()
	{	
		SaveConfig(); 
	}

	// IConfigurable
	void 	UpdateConfig();
	void 	SaveConfig();

	void	Split();
	void	NextWindow();

	void	Key_Enter();
	void	Key_Left();
	void	Key_Right();
	void	Key_Up();
	void	Key_Down();
	void	Key_Home();
	void	Key_End();
	void	Key_PageUp();
	void	Key_PageDown();
	void	Key_Ins();
	void	Key_Del();
	void	Key_BS();
	void	Key_ESC();

	void	Refresh();
	void	ClearRefresh();
	void	RefreshAll();
	
	void	Menu();
	void	Quit();
	
	void	ClipCopy();
	void	ClipPaste();
	void	ClipCut();

	void	Undo();

	void	FileNew();
	void	FileSave();
	void	FileSaveAs();

	void	LineNumberView();

	void	Select()		{ _pEditor->Select();		}
	void	SelectAll()		{ _pEditor->SelectAll();	}

	void	Find() 			{ _pEditor->Find(); }
	void	FindNext() 		{ _pEditor->FindNext(); }
	void	FindPrevious()	{ _pEditor->FindPrevious(); }

	void	Key_Tab()		{ _pEditor->Key_Tab();     }
	void	Key_Untab()		{ _pEditor->Key_Untab();   }

	void	IndentMode()	{ _pEditor->IndentMode(); }
	
	void	GotoLine()		{ _pEditor->GotoLine();  }
	void	GotoFirst()		{ _pEditor->GotoFirst(); }
	void	GotoEnd()		{ _pEditor->GotoEnd();   }

	void	VimView();
	void	MouseUse();

    void    Replace();
}; // class CommandImp

}; // namespace

#endif
