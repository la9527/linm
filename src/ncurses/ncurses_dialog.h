#ifndef __NCURESES_DIALOG_H__
#define __NCURESES_DIALOG_H__

#include "dialogbox.h"
#include "mlsdialog.h"
#include "mlsthread.h"

using namespace MLSUTIL;

namespace MLS
{

class Curses_Dialog:public MlsDialog
{
protected:
	Curses_MsgBox	s_tMsgBox;

public:
	Curses_Dialog()
	{
		s_tMsgBox = Curses_MsgBox("", "");
	}

	void	MsgBox(const string& sTitle, const string& sMsg);
	bool	YNBox(const string& sTitle, const string& sMsg, bool bYes = false);
	void*	MsgWaitBox(const string& sTitle, const string& sMsg);
	void	MsgWaitEnd(void* pWait);
	int		InputBox(const string& sTitle, string& sInputStr, bool bPasswd = false);
	int		SelectBox(	const string& sTitle, vector<string>& vMsgStr, int n = 0);
	int		GetChar(bool bNoDelay = false);
	int		SetKeyBreakUse(bool bBreak = false);
	int		TextBox( const string& sTitle, vector<string>& vTextStr, bool bCurShow = false, int width = 50 );
};

///	@brief	 Status output box.
class Curses_Progress:public MlsProgress
{
protected:
	void*		_pVoidTmp;		///< void* type. Shoud be change the type.
	string 		_sLeftStr, _sRightStr;
	string 		_sLeftStr2, _sRightStr2;

	int 		_nCnt;  		///< 1 progress counter (%)
	int 		_nCnt2;  		///< 2 progress counter (%)

public:
	Curses_Progress();
	virtual ~Curses_Progress();
	
	void 	SetTitle(const string& str)		{ _sTitle = str; }
	void	SetMsg(const string& str)		{ _sMsg = str; }
	void	SetDouble(bool bDouble) 		{ _bDouble = bDouble; }

	virtual void setLeftStr(const string& p)  
	{ 
		MlsMutexSelf	tMutex(_pMutex);
		_sLeftStr=p;
	}
	virtual void setRightStr(const string& p) 
	{ 
		MlsMutexSelf	tMutex(_pMutex);
		_sRightStr=p;
	}
	virtual void setLeftStr2(const string& p) 
	{ 
		MlsMutexSelf	tMutex(_pMutex);
		_sLeftStr2=p;
	}
	virtual void setRightStr2(const string& p) 	
	{
		MlsMutexSelf	tMutex(_pMutex);
		_sRightStr2=p;
	}

	virtual	bool isExit();
	virtual void show();
	virtual void redraw();

	virtual void setCount(int nCnt, int nCnt2 = -1)
	{
		MlsMutexSelf	tMutex(_pMutex);
		_nCnt = nCnt;
		if (nCnt2 != -1) _nCnt2 = nCnt2;
	}

	void*	Execute(void* pArg);
};

};

#endif
