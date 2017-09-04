#ifndef __DIALOG_REMOTECONNPROPERTIES_H__
#define __DIALOG_REMOTECONNPROPERTIES_H__

#include "dialogbox.h"
#include "mlsdialog.h"
#include "mlsthread.h"
#include "dialog.h"
#include "remotecfgload.h"

using namespace MLSUTIL;

namespace MLS
{

class	Dialog_RemoteConnInsert: public Form
{
public:
	Dialog_RemoteConnInsert(const string& sTitle, int nDefNum = 0);
	
	void	SetRemoteConnEntry(RemoteConnEntry	tEntry);
	void	GetRemoteConnEntry(RemoteConnEntry&	tEntry);

	void	SetName(const string& str)	{ _tName.SetStr(str); }
	string	GetName()					{ return _tName.GetStr(); }
	
	void	SetAddr(const string& str) 	{ _tAddr.SetStr(str); }
	string	GetAddr() 					{ return _tAddr.GetStr(); }

	void	SetPort(const string& str) 	{ _tPort.SetStr(str); }
	string	GetPort() 					{ return _tPort.GetStr(); }

	void	SetDirectory(const string& str) { _tDirectory.SetStr(str); }
	string	GetDirectory() 					{ return _tDirectory.GetStr(); }

	void	SetID(const string& str) 	{ _tID.SetStr(str); }
	string	GetID()						{ return _tID.GetStr(); }

	void	SetPasswd(const string& str) { _tPasswd.SetStr(str); }
	string	GetPasswd()					{ return _tPasswd.GetStr();	}

	void	SetProtocol(const string& str );
	string	GetProtocol();

	void	SetKeyFile(bool	bBool)		{ _tKeyFile.SetCheck(bBool); }
	bool	GetKeyFile()				{ return _tKeyFile.GetCheck(); }

	bool	GetOk()						{ return _bOk; }
	void	Clear();

protected:	
	bool	MouseEventExe(Position* pPosition, int nNum, int Y, int X, mmask_t bstate);
	bool	MouseEvent(int Y, int X, mmask_t bstate);
	void	SetPosition(Position* pPosition, Form* pForm, int y, int x, int width, int nColNum = 0);

	virtual void	Draw();

	void	InputExe(Input* pInput, KeyInfo& tKeyInfo);

	void	CheckRadioExe(CheckRadioButton* pRadio, KeyInfo& tKeyInfo, CheckRadioButton* pRadio2 = NULL);
	void	ButtonExe(Button* pButton, KeyInfo& tKeyInfo);
	virtual void	Execute(KeyInfo& tKeyInfo);

private:
	Button				_tOk;
	Button				_tCancel;

	Input				_tName;
	Input				_tAddr;
	Input				_tPort;
	Input				_tDirectory;
	Input				_tID;
	Input				_tPasswd;
	Input				_tPublicKey;
	Input				_tPrivateKey;

	Label				_tLabelName;
	Label				_tLabelProtocol;
	Label				_tLabelAddr;
	Label				_tLabelPort;
	Label				_tLabelDirectory;

	Label				_tLabelID;
	Label				_tLabelPasswd;

	Label				_tLabelPublicKey;
	Label				_tLabelPrivateKey;	

	CheckRadioButton	_tSAMBA;
	CheckRadioButton	_tSFTP;
	CheckRadioButton	_tFTP;
	CheckRadioButton	_tAnonymous;
	CheckRadioButton	_tKeyFile;

	int					_nCurNum;
	bool				_bOk;
};

class	Dialog_RemoteConnProps: public Form
{
public:
	Dialog_RemoteConnProps(const string& sTitle, int nDefNum = 0);
	
	void	Clear();
	
	int		GetListCur() 		{ return _nCur; }
	int		GetButtonNum() 		{ return _nStatusNum; }
	
	void	SetSelData(vector<string>	vData);

protected:	
	virtual bool	MouseEvent(int Y, int X, mmask_t bstate);
	virtual void	Draw();
	virtual void	Execute(KeyInfo& tKeyInfo);
	
private:
	Button			_tInsert;
	Button			_tModify;
	Button			_tDelete;
	Button			_tClose;
	
	vector<string>				_vData;
	vector<CheckRadioButton*>	_vButtonList;
	vector<Button*>				_vGenaralButton;
	
	int				_nViewSize;
	int				_nViewFirstCur;
	int				_nViewCur;
	int				_nCur;
	int				_nStatusNum;
	
	int				_nStrMaxLength;
};

};

#endif // __DIALOG_SSHPROPERTIES_H__
