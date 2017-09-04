#ifndef __MLSDIALOG_H__
#define __MLSDIALOG_H__

#include <stdarg.h>
#include "define.h"
#include "mlsthread.h"
#include "mlsmutex.h"

namespace MLSUTIL
{

class MlsDialog
{
public:
	MlsDialog() {}
	virtual ~MlsDialog() {}

	virtual void	MsgBox(const string& sTitle, const string& sMsg)
	{
		cout << "MsgBox :: " << sTitle << "Msg :: " << sMsg << endl;
	}
	virtual bool	YNBox(const string& sTitle, const string& sMsg, bool /*bYes*/ = false)
	{
		cout << "YNBox :: " << sTitle << "Msg :: " << sMsg << endl;
		return false;
	}
	virtual void*	MsgWaitBox(const string& sTitle, const string& sMsg)
	{
		cout << "MsgWaitBox :: " << sTitle << "Msg :: " << sMsg << endl;
		return nullptr;
	}
	virtual void	MsgWaitEnd(void* ) {}
	virtual int		InputBox(const string& sTitle, string& sInputStr, bool /*bPasswd*/ = false)
	{
		cout << "InputBox :: " << sTitle << endl;
		cin >> sInputStr;
		return 0;
	}
	virtual int		SelectBox(	const string& /*sTitle*/, vector<string>& /*vMsgStr*/, int /*n*/ = 0) { return 0; }
	virtual int		GetChar(bool /*bNoDelay*/ = false) { return 0; }
	virtual int		SetKeyBreakUse(bool /*bBreak*/ = false) { return 0; }

	virtual int		TextBox( const string& /*sTitle*/, vector<string>& /*vTextStr*/, bool /*bCurShow*/ = false, int /*width*/ = 40 ) { return 0; }
};

///	@brief	 progress class
class MlsProgress:public MlsThread
{
public:
	string		_sTitle;
	string		_sMsg;
	bool		_bDouble;
	MlsMutex*	_pMutex;

protected:
	virtual void* Execute(void* /*pArg*/) { return NULL; }

public:
	bool	_bStarted;

public:
	MlsProgress() { _bDouble = false; _bStarted = false; _pMutex = NULL; }
	MlsProgress(const string& sTitle, const string& sMsg, bool bDouble = false)
	{
		_sTitle = sTitle; _sMsg = sMsg; _bDouble = bDouble; _pMutex = NULL;
	}

	virtual ~MlsProgress() {}

	virtual void setLeftStr(const string& ) {}
	virtual void setRightStr(const string& ) {}
	virtual void setLeftStr2(const string& ) {}
	virtual void setRightStr2(const string& ) {}

	virtual	bool isExit() { return false; }
	virtual void Show() {}
	virtual void Redraw() {}

	virtual void setCount(int /*nCnt*/, int /*nCnt2*/ = -1) {}
};

class CommonProgress
{
protected:
	string			_sTitle;
	string			_sMsg;
	bool			_bDouble;
	MlsProgress*	_pProgress;
	bool			_bStarted;

public:
	CommonProgress(const string& sTitle, const string& sMsg, bool bDouble = false);
	virtual ~CommonProgress();

	void SetProgress( MlsProgress* p);

	void setLeftStr(const string& p);
	void setRightStr(const string& p);
	void setLeftStr2(const string& p);
	void setRightStr2(const string& p);
	bool isExit();
	void show();
	void redraw();
	void setCount(int nCnt, int nCnt2 = -1);
	void Start(void* pArg = NULL);
	void End();
};

/// @brief	SetDialog
void	SetDialogProgress( MlsDialog* pDialog, MlsProgress* pProgress );

///	@brief	 message box.
void	MsgBox(const string& sTitle, const string& sMsg);

///	@brief	 yes, no message box.
bool	YNBox(const string& sTitle, const string& sMsg, bool bYes = false);

///	@brief	 start function of message box. ( use only wait message box ) 
void*	MsgWaitBox(const string& sTitle, const string& sMsg);

///	@brief	 end function of message box. (use only wait messge box )
void	MsgWaitEnd(void* p);

///	@brief	 Input box.
/// @param	sTitle		title
/// @param	sInputStr	inputed string.
/// @param	bPasswd		if input string is password type, bPasswd is true. ( english input only )
/// @return	성공 SUCCESS
int		InputBox(const string& sTitle, string& sInputStr, bool bPasswd = false);

///	@brief	 select vbox
/// @param	sTitle		title
/// @param	vMsgStr		data
/// @param	n			select number
/// @return	user selected number
int 	SelectBox(	const string& sTitle,
					vector<string>& vMsgStr,
					int n = 0);

///	@brief	 get key number. (bNoDelay is use the progress box )
int 	GetChar(bool bNoDelay = false);

/// @brief	can use ctrl+c key.
int		SetKeyBreakUse(bool bBreak = false);

/// @brief	text box
int 	TextListBox( const string& sTitle, vector<string>& vTextStr, bool bCurShow = false, int width = 40 );

};

#endif
