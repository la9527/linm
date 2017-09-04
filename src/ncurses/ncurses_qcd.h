#ifndef __NCURSES_QCD_H
#define __NCURSES_QCD_H

#include "mlscfgload.h"
#include "dialogbox.h"
#include <vector>
#include <string>

using namespace MLSUTIL;

namespace MLS {

class Qcd: public Curses_SelectBox, public Configurable
{
public:
	Qcd();
	~Qcd();

	void UpdateConfig();
	void SaveConfig();

	const string& 	GetData();
	
protected:
	void	Execute(KeyInfo& tKeyInfo);	

private:
	vector<string>	_vItem;
};

}

#endif

