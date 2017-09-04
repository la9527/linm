#include "dialog_remoteconnprops.h"

using namespace MLS;
using namespace MLSUTIL;

Dialog_RemoteConnInsert::Dialog_RemoteConnInsert(const string& sTitle, int nDefNum): Form()
{
	SetTitle(sTitle);

	_tLabelName		 = Label(_("Name"));

	_tSFTP  = CheckRadioButton(_("SFTP"), false);
	_tFTP 	= CheckRadioButton(_("FTP"), false);
	_tSAMBA = CheckRadioButton(_("SAMBA"), false);

	_tLabelProtocol	 = Label(_("Protocol"));
	_tLabelAddr 	 = Label(_("Address"));
	_tLabelDirectory = Label(_("Directory"));
	_tLabelPort 	 = Label(_("Port"));

	_tLabelID 		 = Label(_("UserName"));
	_tLabelPasswd 	 = Label(_("Password"));

	_tLabelPublicKey = Label(_("PublicKey"));
	_tLabelPrivateKey = Label(_("PrivateKey"));
	
	_tAnonymous		 = CheckRadioButton("Anonymous", true);
	_tKeyFile		 = CheckRadioButton("UseKeyFile", true);

	_tOk 			 = Button(_("Ok"), 10);
	_tCancel 		 = Button(_("Cancel"), 10);

	_tPasswd.SetPasswdType( true );
	
	fontcolor = g_tColorCfg.GetColorEntry("Property").font;
	backcolor = g_tColorCfg.GetColorEntry("Property").back;

	_tSFTP.SetCheck( true );
	_tPort.SetStr("22");
	_tFTP.SetCheck( false );
	_tKeyFile.SetCheck( false );
	_tAnonymous.SetCheck( false );
	_nCurNum = nDefNum;

	_tPublicKey.SetStr("~/.ssh/id_rsa.pub");
	_tPrivateKey.SetStr("~/.ssh/id_rsa");
	_bOk = false;
}
	
void	Dialog_RemoteConnInsert::SetRemoteConnEntry(RemoteConnEntry	tEntry)
{
	_tName.SetStr(tEntry.sName);
	SetProtocol(tEntry.sProtocol);
	_tAddr.SetStr(tEntry.sAddress);
	_tPort.SetStr(tEntry.sPort);
	_tDirectory.SetStr(tEntry.sDirectory);
	_tKeyFile.SetCheck( tEntry.bKeyFile );
	_tID.SetStr(tEntry.sUserName);
	
	if ( tEntry.bKeyFile )
	{
		_tPublicKey.SetStr(tEntry.sPublicKey);
		_tPrivateKey.SetStr(tEntry.sPrivateKey );
	}
	else
	{
		_tPasswd.SetStr(tEntry.sPassword);
	}
}

void	Dialog_RemoteConnInsert::GetRemoteConnEntry(RemoteConnEntry&	tEntry)
{
	tEntry.sName 		= _tName.GetStr();
	tEntry.sProtocol 	= GetProtocol();
	tEntry.sAddress 	= _tAddr.GetStr();
	tEntry.sPort 		= _tPort.GetStr();
	tEntry.sDirectory 	= _tDirectory.GetStr();
	tEntry.bKeyFile 	= _tKeyFile.GetCheck();
	tEntry.sUserName 	= _tID.GetStr();

	if ( _tSFTP.GetCheck() && _tKeyFile.GetCheck() )
	{
		tEntry.sPublicKey 	= _tPublicKey.GetStr();
		tEntry.sPrivateKey 	= _tPrivateKey.GetStr();
	}
	else
	{
		#ifdef __LINM_SFTP_USE__
		tEntry.sPassword	= _tPasswd.GetStr();
		#endif
	}
	
	tEntry.bModify 		= true;
}


void	Dialog_RemoteConnInsert::SetProtocol(const string& str )
{ 
	_tFTP.SetCheck(false);
	_tSFTP.SetCheck(false);
	_tSAMBA.SetCheck(false);

	if (Tolower(str) == "ftp")
		_tFTP.SetCheck(true); 
	else if (Tolower(str) == "sftp")
		_tSFTP.SetCheck(true);
	else if (Tolower(str) == "smb")
		_tSAMBA.SetCheck(true);
	else
		_tSFTP.SetCheck(true);
}

string	Dialog_RemoteConnInsert::GetProtocol()
{ 
	if (_tSFTP.GetCheck() == true)
		return "sftp";
	else if (_tSAMBA.GetCheck() == true)
		return "smb";
	else
		return "ftp";
}

void	Dialog_RemoteConnInsert::Clear()
{
	_tName.SetClear();
	_tAddr.SetClear();
	_tID.SetClear();
	_tPasswd.SetClear();
	_tSFTP.SetCheck( true );
	_tFTP.SetCheck( false );
	_tSAMBA.SetCheck( false );
	_tKeyFile.SetCheck( false );
	_tAnonymous.SetCheck( false );
	_tPublicKey.SetClear();
	_tPrivateKey.SetClear();
	_nCurNum = 1;
	_bOk = false;
}

bool	Dialog_RemoteConnInsert::MouseEventExe(Position* pPosition, int nNum, int Y, int X, mmask_t bstate)
{
	if (pPosition->AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
	{
		_nCurNum = nNum;
		if (bstate & BUTTON1_DOUBLE_CLICKED) return true;
	}

	if (pPosition->AreaChk(Y, X))
		_nCurNum = nNum;
	
	return false;
}
	
bool	Dialog_RemoteConnInsert::MouseEvent(int Y, int X, mmask_t bstate) 
{ 
	if (MouseEventExe(&_tAddr,		0, Y, X, bstate) ) return true;

	if (MouseEventExe(&_tSFTP, 		1, Y, X, bstate) )
	{
		_tSFTP.SetCheck(true);
		_tFTP.SetCheck(false);
		_tSAMBA.SetCheck(false);
		_tPort.SetStr("22");
		_tPublicKey.SetStr("~/.ssh/id_rsa.pub");
		_tPrivateKey.SetStr("~/.ssh/id_rsa");
		_tID.SetStr("");
		_tPasswd.SetStr("");
		return true;
	}
	if (MouseEventExe(&_tFTP, 		2, Y, X, bstate) ) 
	{
		_tSFTP.SetCheck(false);
		_tFTP.SetCheck(true);
		_tSAMBA.SetCheck(false);
		_tPort.SetStr("21");
		_tKeyFile.SetCheck( false );
		_tPasswd.SetPasswdType( true );
		_tID.SetStr("");
		_tPasswd.SetStr("");
		return true;
	}
	if (MouseEventExe(&_tSAMBA, 		3, Y, X, bstate) ) 
	{
		_tSAMBA.SetCheck(true);
		_tSFTP.SetCheck(false);
		_tFTP.SetCheck(false);
		_tPort.SetStr("");
		_tKeyFile.SetCheck( false );
		_tPasswd.SetPasswdType( true );
		_tID.SetStr("");
		_tPasswd.SetStr("");
		return true;
	}
	if (MouseEventExe(&_tAddr,		4, Y, X, bstate) ) return true;	
	if (MouseEventExe(&_tDirectory,	5, Y, X, bstate) ) return true;
	if (MouseEventExe(&_tPort,		6, Y, X, bstate) ) return true;

	if (MouseEventExe(&_tAnonymous,	7, Y, X, bstate) ) 
	{
		_tAnonymous.SetCheck(!_tAnonymous.GetCheck());
		if (_tAnonymous.GetCheck())	
			_tID.SetStr("anonymous");
		return true;
	}

	if (MouseEventExe(&_tKeyFile,	8, Y, X, bstate) )
	{
		if ( _tSFTP.GetCheck() )
		{
			_tKeyFile.SetCheck(!_tKeyFile.GetCheck());
			
			if ( !_tKeyFile.GetCheck() )
			{
				_tPasswd.SetStr("");
			}
			ScrClear();
		}
		return true;
	}

	if (MouseEventExe(&_tID, 		9, Y, X, bstate) ) return true;
	
	if ( !_tKeyFile.GetCheck() )
	{
		if (MouseEventExe(&_tPasswd,	10, Y, X, bstate) ) return true;
		if (MouseEventExe(&_tOk,		11, Y, X, bstate) )
		{
			_bOk = true;
			_bExit = true;
			return true;
		}
		if (MouseEventExe(&_tCancel,	12, Y, X, bstate) )
		{
			_bOk = false;
			_bExit = true;
			return true;
		}
	}
	else
	{
		if (MouseEventExe(&_tPublicKey,	 10, Y, X, bstate) ) return true;
		if (MouseEventExe(&_tPrivateKey, 11, Y, X, bstate) ) return true;

		if (MouseEventExe(&_tOk,		12, Y, X, bstate) )
		{
			_bOk = true;
			_bExit = true;
			return true;
		}
		if (MouseEventExe(&_tCancel,	13, Y, X, bstate) )
		{
			_bOk = false;
			_bExit = true;
			return true;
		}
	}
	return false;
}
	
void		Dialog_RemoteConnInsert::SetPosition(Position* pPosition, Form* pForm, int y, int x, int width, int nColNum)
{
	pPosition->SetForm(pForm);
	pPosition->y = y;
	pPosition->x = x;
	pPosition->height = 1;
	pPosition->width = width;

	switch( nColNum)
	{
		case 0:
			pPosition->backcolor = backcolor;
			pPosition->fontcolor = fontcolor;
			break;
		case 1:
			pPosition->backcolor = g_tColorCfg.GetColorEntry("PropertyList").back;
			pPosition->fontcolor = g_tColorCfg.GetColorEntry("PropertyList").font;
			break;
		case 2:
			pPosition->backcolor = g_tColorCfg.GetColorEntry("PropertyBtn").back;
			pPosition->fontcolor = g_tColorCfg.GetColorEntry("PropertyBtn").font;
			break;
		case 4:
			pPosition->backcolor = g_tColorCfg.GetColorEntry("StatA").back;
			pPosition->fontcolor = g_tColorCfg.GetColorEntry("PropertyBtn").font;
			break;
	}	

	pPosition->Show();
	pPosition->SetFocus(false);
}

void	Dialog_RemoteConnInsert::Draw()
{
	setcol(fontcolor, backcolor, GetWin());
	wmove(GetWin(), 15, 2);
	whline(GetWin(), HLINE, width-4);

	_tOk.SetTxtAlign(MIDDLE);
	_tCancel.SetTxtAlign(MIDDLE);

	_tLabelName.SetTxtAlign(LEFT);
	_tLabelAddr.SetTxtAlign(LEFT);
	_tLabelID.SetTxtAlign(LEFT);
	_tLabelPasswd.SetTxtAlign(LEFT);
	
	SetPosition(&_tLabelName,		this, 3, 2, 16);

	SetPosition(&_tLabelProtocol,	this, 5, 2, 16);
	SetPosition(&_tLabelAddr, 		this, 6, 2, 16);
	SetPosition(&_tLabelDirectory,	this, 7, 2, 16);
	SetPosition(&_tLabelPort, 		this, 8, 2, 16);

	SetPosition(&_tName,			this, 3, 20, 36, 1);

	SetPosition(&_tSFTP,			this, 5, 20, 10);
	SetPosition(&_tFTP,				this, 5, 31, 10);
	SetPosition(&_tSAMBA,			this, 5, 40, 10);

	SetPosition(&_tAddr, 			this, 6, 20, 36, 1);
	SetPosition(&_tDirectory, 		this, 7, 20, 36, 1);
	SetPosition(&_tPort, 			this, 8, 20, 36, _tSAMBA.GetCheck() ? 4 : 1 );

	SetPosition(&_tAnonymous, 		this, 9, 20, 16, _tSAMBA.GetCheck() ? 4 : 0 );
	
	if ( _tSFTP.GetCheck() )
		SetPosition(&_tKeyFile, 	this, 9, 38, 16, 0);
	else
		SetPosition(&_tKeyFile, 	this, 9, 38, 16, 4);

	SetPosition(&_tLabelID, 		this, 11, 2, 16);
	SetPosition(&_tLabelPasswd, 	this, 12, 2, 16);
	SetPosition(&_tID, 				this, 11, 20, 36, _tSAMBA.GetCheck() ? 4 : 1);
	SetPosition(&_tPasswd, 			this, 12, 20, 36, 
				_tKeyFile.GetCheck() ? 4 : (_tSAMBA.GetCheck() ? 4 : 1));
	SetPosition(&_tLabelPublicKey, 	this, 13, 2, 16);
	SetPosition(&_tLabelPrivateKey, this, 14, 2, 16);
	SetPosition(&_tPublicKey, 		this, 13, 20, 36, _tKeyFile.GetCheck() ? 1 : 4);
	SetPosition(&_tPrivateKey, 		this, 14, 20, 36, _tKeyFile.GetCheck() ? 1 : 4);
	
	SetPosition(&_tOk, 				this, 16, 20, 10, 2);
	SetPosition(&_tCancel, 			this, 16, 35, 10, 2);

	if (_nCurNum == 0)	_tName.SetFocus(true);
	if (_nCurNum == 1)	_tSFTP.SetFocus(true);
	if (_nCurNum == 2)	_tFTP.SetFocus(true);
	if (_nCurNum == 3)	_tSAMBA.SetFocus(true);
	if (_nCurNum == 4)	_tAddr.SetFocus(true);
	if (_nCurNum == 5)	_tDirectory.SetFocus(true);
	if (_nCurNum == 6)	_tPort.SetFocus(true);
	if (_nCurNum == 7)	_tAnonymous.SetFocus(true);
	if (_tSFTP.GetCheck() && _nCurNum == 8)	 _tKeyFile.SetFocus(true);
	if (_nCurNum == 9)	_tID.SetFocus(true);
	if ( !_tKeyFile.GetCheck() )
	{
		if (_nCurNum == 10)	_tPasswd.SetFocus(true); 
		if (_nCurNum == 11)	_tOk.SetFocus(true);
		if (_nCurNum == 12) _tCancel.SetFocus(true);
	}
	else
	{
		if (_nCurNum == 10)	_tPublicKey.SetFocus(true);
		if (_nCurNum == 11)	_tPrivateKey.SetFocus(true);
		if (_nCurNum == 12)	_tOk.SetFocus(true);
		if (_nCurNum == 13) _tCancel.SetFocus(true);
	}
}

void	Dialog_RemoteConnInsert::InputExe(Input* pInput, KeyInfo& tKeyInfo)
{
	int nKey = pInput->DataInput( tKeyInfo );

	switch(nKey)
	{
		case KEY_MOUSE:
			MouseProc();
			break;
		case KEY_ENTER:
		case 13:
		case KEY_DOWN:
		case KEY_TAB:
			if ( !_tSFTP.GetCheck() && _nCurNum == 7 )
				_nCurNum = 9;
			else if ( _tSAMBA.GetCheck() && _nCurNum == 5 )
				_nCurNum = 11;
			else
				_nCurNum++;
			break;
		case KEY_UP:
			if ( !_tSFTP.GetCheck() && _nCurNum == 9 )
				_nCurNum = 7;
			else if ( _tSAMBA.GetCheck() && _nCurNum == 11 ) 
				_nCurNum = 5;
			else
				_nCurNum--;
			break;
		case KEY_ESC:
			if ( !_tKeyFile.GetCheck() )
				_nCurNum = 12; // Cancel Button
			else
				_nCurNum = 13; // Cancel Button
			break;
	}
	
	if ( !_tKeyFile.GetCheck() )
	{
		if (_nCurNum > 12) _nCurNum = 0;
		if (_nCurNum < 0) _nCurNum = 12;
	}
	else
	{
		if (_nCurNum > 13) _nCurNum = 0;
		if (_nCurNum < 0) _nCurNum = 13;
	}
}

void	Dialog_RemoteConnInsert::CheckRadioExe(CheckRadioButton* pRadio, KeyInfo& tKeyInfo, CheckRadioButton* pRadio2)
{
	int nKey = (int)tKeyInfo;
	
	switch(nKey)
	{
		case KEY_MOUSE:
			MouseProc();
			break;
		case KEY_ENTER:
		case 13:
		case ' ':
			if ( pRadio2 )
			{
				pRadio->SetCheck( true );
				pRadio2->SetCheck( false );
			}
			else
				pRadio->SetCheck( !pRadio->GetCheck() );
			
			if (pRadio->GetMsg() == "SFTP" && pRadio->GetCheck()) 
			{
				_tPort.SetStr("22");
				_tPasswd.SetClear();
				_tFTP.SetCheck(false);
				_tSAMBA.SetCheck(false);
			}
			if (pRadio->GetMsg() == "FTP" && pRadio->GetCheck())
			{
				_tPort.SetStr("21");
				_tKeyFile.SetCheck(false);
				_tSFTP.SetCheck(false);
				_tSAMBA.SetCheck(false);
				_tPasswd.SetClear();
				_tPublicKey.SetClear();
				_tPrivateKey.SetClear();
			}
			if (pRadio->GetMsg() == "SAMBA" && pRadio->GetCheck())
			{
				_tID.SetClear();
				_tPort.SetClear();
				_tKeyFile.SetCheck(false);
				_tFTP.SetCheck(false);
				_tSFTP.SetCheck(false);
				_tPasswd.SetClear();
				_tPublicKey.SetClear();
				_tPrivateKey.SetClear();
			}
			if (pRadio->GetMsg() == _("Anonymous"))
			{
				if (pRadio->GetCheck())
				{
					_tID.SetStr("anonymous");
					_tPasswd.SetStr("");
				}
				else
				{
					_tID.SetStr("");
					_tPasswd.SetStr("");
				}
			}

			if (pRadio->GetMsg() == "UseKeyFile")
			{	
				if (pRadio->GetCheck())
				{
					_tPublicKey.SetStr("~/.ssh/id_rsa.pub");
					_tPrivateKey.SetStr("~/.ssh/id_rsa");
				}
				else
				{
					_tID.SetStr("");
					_tPasswd.SetStr("");
				}
			}
			ScrClear();
			break;
		case KEY_DOWN:
		case KEY_RIGHT:
		case KEY_TAB:
			if ( !_tSFTP.GetCheck() && _nCurNum == 7 )
				_nCurNum = 9;
			else if ( _tSAMBA.GetCheck() && _nCurNum == 5 )
				_nCurNum = 11;
			else
				_nCurNum++;
			break;
		case KEY_UP:
			if ( !_tSFTP.GetCheck() && _nCurNum == 9 )
				_nCurNum = 7;
			else if ( _tSAMBA.GetCheck() && _nCurNum == 11 ) 
				_nCurNum = 5;
			else
				_nCurNum--;
			break;
		case KEY_ESC:
			if ( !_tKeyFile.GetCheck() )
				_nCurNum = 12; // Cancel Button
			else
				_nCurNum = 13; // Cancel Button
			break;
	}
	
	if ( !_tKeyFile.GetCheck() )
	{
		if (_nCurNum > 12) _nCurNum = 0;
		if (_nCurNum < 0) _nCurNum = 12;
	}
	else
	{
		if (_nCurNum > 13) _nCurNum = 0;
		if (_nCurNum < 0) _nCurNum = 13;
	}
}

void	Dialog_RemoteConnInsert::ButtonExe(Button* pButton, KeyInfo& tKeyInfo)
{
	int nKey = (int)tKeyInfo;

	switch(nKey)
	{
		case KEY_MOUSE:
			MouseProc();
			break;
		case KEY_ENTER:
		case 13:
		case ' ':
			if ( !_tKeyFile.GetCheck() )
			{
				if ( _nCurNum == 11 ) 	_bOk = true;
				if ( _nCurNum == 11 || _nCurNum == 12 ) _bExit = true;
			}
			else
			{
				if ( _nCurNum == 12 ) 	_bOk = true;
				if ( _nCurNum == 12 || _nCurNum == 13 ) _bExit = true;
			}
			break;
		case KEY_DOWN:
		case KEY_RIGHT:
		case KEY_TAB:
			if ( !_tSFTP.GetCheck() && _nCurNum == 7 )
				_nCurNum = 9;
			else if ( _tSAMBA.GetCheck() && _nCurNum == 6 ) 
				_nCurNum = 11;
			else
				_nCurNum++;
			break;
		case KEY_LEFT:
		case KEY_UP:
			if ( !_tSFTP.GetCheck() && _nCurNum == 9 )
				_nCurNum = 7;
			else if ( _tSAMBA.GetCheck() && _nCurNum == 11 ) 
				_nCurNum = 6;
			else
				_nCurNum--;
			break;
		case KEY_ESC:
			if ( !_tKeyFile.GetCheck() )
				_nCurNum = 12; // Cancel Button
			else
				_nCurNum = 13; // Cancel Button
			break;
	}
	
	if ( !_tKeyFile.GetCheck() )
	{
		if (_nCurNum > 12) _nCurNum = 0;
		if (_nCurNum < 0) _nCurNum = 12;
	}
	else
	{
		if (_nCurNum > 13) _nCurNum = 0;
		if (_nCurNum < 0) _nCurNum = 13;
	}
}

void	Dialog_RemoteConnInsert::Execute(KeyInfo& tKeyInfo)
{
	switch(_nCurNum)
	{
		case 0:		InputExe(&_tName, tKeyInfo);				return;
		case 1: 	CheckRadioExe(&_tSFTP, tKeyInfo, &_tFTP);	return;
		case 2:		CheckRadioExe(&_tFTP, tKeyInfo, &_tSFTP);	return;
		case 3:		CheckRadioExe(&_tSAMBA, tKeyInfo, &_tSFTP);	return;
		case 4:		InputExe(&_tAddr, tKeyInfo);				return;
		case 5:		InputExe(&_tDirectory, tKeyInfo);			return;
		case 6:		InputExe(&_tPort, tKeyInfo);				return;
		case 7:		CheckRadioExe(&_tAnonymous, tKeyInfo);		return;
		case 8:		CheckRadioExe(&_tKeyFile, tKeyInfo);		return;
		case 9:		InputExe(&_tID, tKeyInfo);					return;
	}
	
	if ( !_tKeyFile.GetCheck() )
	{
		switch(_nCurNum) 
		{
		 	case 10:	InputExe(&_tPasswd, tKeyInfo); 	break;
			case 11:	ButtonExe(&_tOk, tKeyInfo);		break;
			case 12:	ButtonExe(&_tCancel, tKeyInfo); break;
		}
	}
	else
	{
		switch(_nCurNum) 
		{
		 	case 10:	InputExe(&_tPublicKey, tKeyInfo);	break;
			case 11:	InputExe(&_tPrivateKey, tKeyInfo);	break;
			case 12:	ButtonExe(&_tOk, tKeyInfo);			break;
			case 13:	ButtonExe(&_tCancel, tKeyInfo);		break;
		}
	}
}

Dialog_RemoteConnProps::Dialog_RemoteConnProps(const string& sTitle, int nDefNum): Form()
{
	SetTitle(sTitle);
	_tInsert = Button(_("Insert"), 10);
	_tModify = Button(_("Modify"), 10);
	_tDelete = Button(_("Delete"), 10);
	_tClose  = Button(_("Close"), 10);
	_nStatusNum = 0;
	_nCur = nDefNum;

	_nViewFirstCur = nDefNum < 14 ? 0 : nDefNum - 13;
	_nViewCur = nDefNum < 14 ? nDefNum : 0;
	_nViewSize = 14; 
	
	_vGenaralButton.push_back(&_tInsert);
	_vGenaralButton.push_back(&_tModify);
	_vGenaralButton.push_back(&_tDelete);
	_vGenaralButton.push_back(&_tClose);

	fontcolor = g_tColorCfg.GetColorEntry("Property").font;
	backcolor = g_tColorCfg.GetColorEntry("Property").back;
}
	
void	Dialog_RemoteConnProps::Clear()
{
	CheckRadioButton*		pButton = NULL;
	for (int n = 0; n < (int)_vButtonList.size(); n++)
	{
		pButton = _vButtonList[n];
		delete pButton;
	}

	_nStatusNum = 0;
}
	
void	Dialog_RemoteConnProps::SetSelData(vector<string>	vData)
{
	Clear();
	_vData = vData;

	_nStrMaxLength = 40;
	if (_nViewSize > (int)vData.size() )
		_nViewSize = (int)vData.size();

	for (int n = 0; n < (int)vData.size(); n++)
	{
		CheckRadioButton* pButton = new CheckRadioButton(vData[n], false);
		
		if (_nStrMaxLength < scrstrlen(vData[n])) 
			_nStrMaxLength = scrstrlen(vData[n]);

		_vButtonList.push_back(pButton);
		
	}

	if (_nStrMaxLength > width) width = _nStrMaxLength + 16;

	if ( (int)vData.size() <= 10 || (int)vData.size()+6 <= LINES-6)
	{
		_nStrMaxLength = 40;
		height = LINES - 6;
		width = ( 14 * vData.size() ) + 6;
	}
	else
	{
		width = LINES - 6;
		height = _vButtonList.size() + 6;
		_nStrMaxLength = width - 4;
	}
}

bool	Dialog_RemoteConnProps::MouseEvent(int Y, int X, mmask_t bstate) 
{
	CheckRadioButton*	pButton = NULL;
	int 	n = 0;

	for (n = 0; n<(int)_vButtonList.size(); n++)
	{
		pButton = _vButtonList[n];

		if (pButton->AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
		{
			_nStatusNum = 0;
			_nCur = n;
			if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
			return true;
		}

		if (pButton->AreaChk(Y, X))
		{
			_nStatusNum = 0;
			_nCur = n;
			return true;
		}
	}
	
	Button*		pGenButton = NULL;

	for (n = 0; n<(int)_vGenaralButton.size(); n++)
	{
		pGenButton = _vGenaralButton[n];

		if (pGenButton->AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
		{
			_nStatusNum = n + 1;
			if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
			return true;
		}

		if (pGenButton->AreaChk(Y, X))
		{
			_nStatusNum = n + 1;
			return true;
		}
	}
	return false;
}

void	Dialog_RemoteConnProps::Draw()
{
	setcol(fontcolor, backcolor, GetWin());
	wmove(GetWin(), height-3, 2);
	whline(GetWin(), HLINE, width-4);

	CheckRadioButton*		pChkButton = NULL;

	for (int n = _nViewFirstCur; n < _nViewFirstCur+_nViewSize; n++)
	{
		if (n < (int)_vButtonList.size())
		{
			pChkButton = _vButtonList[n];
			pChkButton->SetForm((Form*)this);
			pChkButton->width = _nStrMaxLength;

			pChkButton->Move( 3 + n - _nViewFirstCur, 2 );
			pChkButton->backcolor = g_tColorCfg.GetColorEntry("PropertyList").back;
			pChkButton->fontcolor = g_tColorCfg.GetColorEntry("PropertyList").font;

			pChkButton->Show();

			if (_nCur == n)
				pChkButton->SetFocus(true);
			else
				pChkButton->SetFocus(false);
		}
	}

	Button*		pButton = NULL;

	for (int n = 0; n < (int)_vGenaralButton.size(); n++)
	{
		pButton = _vGenaralButton[n];
		pButton->SetForm((Form*)this);

		pButton->backcolor = g_tColorCfg.GetColorEntry("PropertyBtn").back;
		pButton->fontcolor = g_tColorCfg.GetColorEntry("PropertyBtn").font;

		if ( n < (int)_vGenaralButton.size() - 1 )
			pButton->Move(3+(n*2), width-12);
		else
			pButton->Move(height-2, width-12);
		pButton->SetTxtAlign(MIDDLE);
	
		pButton->Show();
		pButton->SetFocus(false);
		if (_nStatusNum == n + 1)
			pButton->SetFocus(true);
	}
}

void	Dialog_RemoteConnProps::Execute(KeyInfo& tKeyInfo) 
{
	switch((int)tKeyInfo)
	{
		case KEY_ESC:
			_nCur = -1;
			_nStatusNum = 4;
		case KEY_ENTER:
		case 13:
			_bExit = true;
			break;	
		case KEY_TAB:
			if (_nStatusNum < 4)
				_nStatusNum++;
			else
				_nStatusNum = 0;
			break;
		case KEY_UP:
		case KEY_LEFT:
			if (_nStatusNum != 0)
			{
				_nStatusNum--;
				if (_nStatusNum < 0) _nStatusNum = 0;
			}
			else
			{
				if (_nCur > 0) 	_nCur--;
				if (_nViewCur > 0)	_nViewCur--;
				if (_nCur < _nViewFirstCur)	_nViewFirstCur = _nCur;
			}
			break;

		case ' ':
			_vButtonList[_nCur]->SetCheck(!_vButtonList[_nCur]->GetCheck());
			// next move. 

		case KEY_DOWN:
		case KEY_RIGHT:
			if (_nStatusNum != 0)
			{
				_nStatusNum++;
				if (_nStatusNum >= 4) _nStatusNum = 4;
				break;
			}
			else
			{
				if (_nCur < (int)_vData.size()-1) 	_nCur++;
				if (_nViewCur < _nViewSize-1)	_nViewCur++;
				if (_nCur > _nViewFirstCur+(_nViewSize-1)) _nViewFirstCur = _nCur-(_nViewSize-1);
			}
			break;
	}
}
