#include "ncurses_qcd.h"
#include "dialogbox.h"
#include "strutil.h"
#include "mainframe.h"

using namespace MLSUTIL;

namespace MLS {

Qcd::Qcd(): Curses_SelectBox("QCD - Quick Change Directory")
{
	UpdateConfig();
}


Qcd::~Qcd()
{
	SaveConfig();
}

void Qcd::UpdateConfig()
{
	_vItem.clear();
	
	for (int t=0; t<10; t++)
		_vItem.push_back(_Config.GetValue("QCD", "QcdF" + itoa(t+1)));
	
	SetSelData( _vItem, true );
}

void Qcd::SaveConfig()
{
	for (int t=0; t<10; t++)
		_Config.SetValue("QCD", "QcdF" + itoa(t+1), _vItem[t] );
}

void Qcd::Execute(KeyInfo& tKeyInfo)
{
	switch((int)tKeyInfo)
	{
		case KEY_ENTER:
			if ( !_vItem[_nCur].empty() )
			{
				_bExit = true;
				break;
			}
		case KEY_IC:
			g_tMainFrame.SetMcdExeMode(EXECUTE_QCD, itoa(_nCur+1));
			_bExit = true;
			return;
		case KEY_BS:
		case KEY_DC:
			_vItem[_nCur] = "";
			SaveConfig();
			UpdateConfig();
			break;
	}

	Curses_SelectBox::Execute(tKeyInfo);
	g_tMainFrame.SetMcdExeMode( MCD_EXEMODE_NONE );
}

const string& Qcd::GetData()
{
	return _vItem[_nCur];
}

}

