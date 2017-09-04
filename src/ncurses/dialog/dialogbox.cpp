#include "dialogbox.h"
#include "keyreader.h"
#include "exception.h"

using namespace MLSUTIL;
using namespace MLS;

Curses_MsgBox::
Curses_MsgBox(const string& sTitle, const string& sMsg): Form()
{
	SetTitle(sTitle);
	_tButton = Button(_("Ok"), 12);
	_tLabel	 = Label(sMsg);
	fontcolor = g_tColorCfg.GetColorEntry("Dialog").font;
	backcolor = g_tColorCfg.GetColorEntry("Dialog").back;
	_tButton.backcolor = backcolor; _tButton.fontcolor = fontcolor;
	_tLabel.backcolor = backcolor; _tLabel.fontcolor = fontcolor;
}

Curses_MsgBox::Curses_MsgBox(): Form()
{
	_tButton = Button(_("Ok"), 12);
	_tLabel	 = Label("");

	fontcolor = g_tColorCfg.GetColorEntry("Dialog").font;
	backcolor = g_tColorCfg.GetColorEntry("Dialog").back;
	_tButton.backcolor = backcolor; _tButton.fontcolor = fontcolor;
	_tLabel.backcolor = backcolor; _tLabel.fontcolor = fontcolor;
}

void Curses_MsgBox::SetMsg(const string& str)	
{ 
	_tLabel.SetMsg(str); 
	_tButton.backcolor = backcolor; _tButton.fontcolor = fontcolor;
	_tLabel.backcolor = backcolor; _tLabel.fontcolor = fontcolor;
}


void Curses_MsgBox::Draw()
{
	LOG_WRITE("Form height [%d] width [%d]", height, width);
	_tButton.SetForm((Form*)this);
	_tLabel.SetForm((Form*)this);

	_tButton.SetTxtAlign(MIDDLE);
	_tButton.Move(height - 2, MIDDLE);
	_tButton.Show();

	_tLabel.SetTxtAlign(MIDDLE);
	_tLabel.Move(3, MIDDLE);
	_tLabel.Show();

	_tButton.SetFocus(true);
}

bool Curses_MsgBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (_tButton.AreaChk(Y, X) && (bstate & BUTTON1_CLICKED))
	{
		_tButton.SetFocus(true);
		_bExit = true;
		LOG_WRITE("Mouse Event True !!!");
		return true;
	}
	return false;
}

void Curses_MsgBox::Execute(KeyInfo& tKeyInfo)
{
	switch ((int)tKeyInfo)
	{
		case KEY_ESC:
		case KEY_ENTER:
		case 13:
		case ' ':
			_bExit = true;
			break;
	}
}

// YNBox
Curses_YNBox::
Curses_YNBox(const string& sTitle, const string& sMsg, bool bYes): Form()
{
	SetTitle(sTitle);
	_tButtonOk = Button(_("Yes(Y)"), 11);
	_tButtonCancel = Button(_("No(N)"), 11);

	_tLabel	 = Label(sMsg);

	fontcolor = g_tColorCfg.GetColorEntry("Dialog").font;
	backcolor = g_tColorCfg.GetColorEntry("Func").back;
	_tButtonOk.backcolor = backcolor; _tButtonOk.fontcolor = fontcolor;
	_tButtonCancel.backcolor = backcolor; _tButtonCancel.fontcolor = fontcolor;
	_tLabel.backcolor = backcolor; _tLabel.fontcolor = fontcolor;

	_bYes = bYes;
}

void	Curses_YNBox::Resize()
{
	if (width < 34) width = 34;
}

void	Curses_YNBox::Draw()
{
	LOG_WRITE("Form height [%d] width [%d]", height, width);
	
	_tButtonOk.SetForm((Form*)this);
	_tButtonCancel.SetForm((Form*)this);
	_tLabel.SetForm((Form*)this);

	_tButtonOk.backcolor = backcolor; _tButtonOk.fontcolor = fontcolor;
	_tButtonCancel.backcolor = backcolor; _tButtonCancel.fontcolor = fontcolor;
	_tLabel.backcolor = backcolor; _tLabel.fontcolor = fontcolor;

	_tButtonOk.Move(height - 2, (width/2)-12);
	_tButtonCancel.Move(height - 2, (width/2)+3);
	_tLabel.Move(3, MIDDLE);

	_tButtonOk.SetTxtAlign(MIDDLE);
	_tButtonCancel.SetTxtAlign(MIDDLE);
	_tLabel.SetTxtAlign(MIDDLE);

	_tButtonOk.Show();
	_tButtonCancel.Show();
	_tLabel.Show();

	if (_bYes == true)
	{
		_tButtonOk.SetFocus(true);
		_tButtonCancel.SetFocus(false);
	}
	else
	{
		_tButtonOk.SetFocus(false);
		_tButtonCancel.SetFocus(true);
	}
}

bool	Curses_YNBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (_tButtonOk.AreaChk(Y, X))
	{
		_tButtonOk.SetFocus(true);
		_bYes = true;
		if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
	}

	if (_tButtonCancel.AreaChk(Y, X))
	{
		_tButtonCancel.SetFocus(true);
		_bYes = false;
		if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
	}

	return false;
}

void	Curses_YNBox::Execute(KeyInfo&	tKeyInfo)
{
	LOG_WRITE("Key [%d]", (int)tKeyInfo);

	switch((int)tKeyInfo)
	{
		case KEY_ESC:
			_bYes = false;	
		case KEY_ENTER:
		case 13:
			_bExit = true;
			break;
		case 'Y':	case 'y':
			_bYes = true;
			_bExit = true;
			break;
		case 'N':	case 'n':
			_bYes = false;
			_bExit = true;
			break;

		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_DOWN:
		case KEY_UP:
		case KEY_TAB:
			_bYes = !_bYes;
			break;
	}
}

// Select Box
Curses_SelectBox::Curses_SelectBox(const string& sTitle, int nDef): Form()
{
	_sTitle = sTitle;
	_nCur = nDef;
	fontcolor = g_tColorCfg.GetColorEntry("SelectBox").font;
	backcolor = g_tColorCfg.GetColorEntry("SelectBox").back;
	_bHoriz = false;
	_bShowNumber = false;
}

void	Curses_SelectBox::Clear()
{
	Button*		pButton = NULL;
	for (int n = 0; n<(int)_vButtonList.size(); n++)
	{
		pButton = _vButtonList[n];
		delete pButton;
	}
	_vButtonList.clear();
}

void	Curses_SelectBox::SetSelData(vector<string>	vData, bool bShowNumber)
{
	Button*		pButton = NULL;

	Clear();
	_vData.clear();

	if (vData.size() >= 6)
		_nStrMaxLength = 30;
	else
		_nStrMaxLength = 0;

	for (int n = 0; n < (int)vData.size(); n++)
	{
		if ( !bShowNumber )
			pButton = new Button(vData[n]);
		else
		{
			String sView("[%c] %s", '0' + n, vData[n].c_str());
			pButton = new Button(sView.c_str());
		}
	
		if (_nStrMaxLength < scrstrlen(vData[n])) _nStrMaxLength = scrstrlen(vData[n]);
		
		_vButtonList.push_back(pButton);
	}

	if (scrstrlen(_sTitle) > width) width = scrstrlen(_sTitle) + 4;
	if (_nStrMaxLength > width) width = _nStrMaxLength + 4;

	if ( _nStrMaxLength < 12 && (int)vData.size() <= 4 &&
		( 12 * (int)vData.size() ) + 6 < COLS )
	{
		_nStrMaxLength = 12;
		width = ( 14 * vData.size() ) + 6;
		height = 5;
		_bHoriz = true;
	}
	else
	{
		height = vData.size() + 4;
		_nStrMaxLength = width - 4;
	}

	
	_vData = vData;
	_bShowNumber = bShowNumber;
}

void	Curses_SelectBox::Draw()
{
	Button*	pButton = NULL;

	if ( _bHoriz )
	{
		int nLeft = ( width - ( 14 * (int)_vButtonList.size() ) ) / 2;
		for (int n = 0; n<(int)_vButtonList.size(); n++)
		{
			pButton = _vButtonList[n];
			pButton->SetForm((Form*)this);
			pButton->width = _nStrMaxLength;
			pButton->backcolor = backcolor;
			pButton->fontcolor = fontcolor;
			pButton->Move(3, nLeft + ( 14 * n ) );
			pButton->SetTxtAlign(MIDDLE);
			pButton->Show();
	
			if (_nCur == n)
				pButton->SetFocus(true);
			else
				pButton->SetFocus(false);
		}
	}
	else
	{
		for (int n = 0; n<(int)_vButtonList.size(); n++)
		{
			pButton = _vButtonList[n];
			pButton->SetForm((Form*)this);
			pButton->width = width - 4;
			pButton->Move( 3 + n, MIDDLE );
			pButton->SetTxtAlign(LEFT);
			pButton->backcolor = backcolor;
			pButton->fontcolor = fontcolor;
			pButton->Show();
	
			if (_nCur == n)
				pButton->SetFocus(true);
			else
				pButton->SetFocus(false);
		}
	}
}

bool	Curses_SelectBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	Button*	pButton = NULL;

	for (int n = 0; n<(int)_vButtonList.size(); n++)
	{
		pButton = _vButtonList[n];

		if (pButton->AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
		{
			_nCur = n;
			if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
			return true;
		}

		if (pButton->AreaChk(Y, X))
		{
			_nCur = n;
			return true;
		}
	}
	return false;
}

void	Curses_SelectBox::Execute(KeyInfo&	tKeyInfo)
{
	switch((int)tKeyInfo)
	{
		case KEY_ESC:
			_nCur = -1;
			_bExit = true;
			break;
		case KEY_ENTER:
		case 13:
			LOG_WRITE("Key ENTER [%d]", (int)tKeyInfo);
			_bExit = true;
			break;
		case KEY_PPAGE:
			_nCur = 0;
			break;
		case KEY_NPAGE:
			_nCur = _vData.size() - 1;
			break;
		case KEY_TAB:
		case KEY_UP:
		case KEY_LEFT:
			_nCur--;
			if (_nCur < 0) _nCur = _vData.size() - 1;
			break;
		case KEY_DOWN:
		case KEY_RIGHT:
			_nCur++;
			if ((int)_vData.size() <= _nCur) _nCur = 0;
			break;
		default:
			if ( _bShowNumber )
			{
				int  nKeyNum = (int)tKeyInfo;
				if ( (int)_vData.size() > nKeyNum - '0' )
					_nCur = nKeyNum - '0';
			}
			break;
	}
}

// Input Box

Curses_InputBox::Curses_InputBox(const string& sTitle, const string& sStr, bool bPasswd): Form()
{
	SetTitle(sTitle);
	_tInputBox = Input(sStr);
	_tInputBox.SetPasswdType(bPasswd);
	_tButtonOk = Button(_("Ok"), 12);
	_tButtonCancel = Button(_("Cancel"), 12);
	_bStat = true;
	_bYes = true;

	fontcolor = g_tColorCfg.GetColorEntry("Dialog").font;
	backcolor = g_tColorCfg.GetColorEntry("Func").back;
	_tButtonOk.backcolor = backcolor; 	_tButtonCancel.backcolor = backcolor;
	_tButtonOk.fontcolor = fontcolor;	_tButtonCancel.fontcolor = fontcolor;
	_tInputBox.backcolor = 0;
}

void	Curses_InputBox::Draw()
{
	_tInputBox.SetSize(1, width - 4);

	_tInputBox.backcolor = 4;
	_tButtonOk.SetForm((Form*)this);
	_tButtonCancel.SetForm((Form*)this);
	_tInputBox.SetForm((Form*)this);

	_tButtonOk.backcolor = backcolor; 	_tButtonCancel.backcolor = backcolor;
	_tButtonOk.fontcolor = fontcolor;	_tButtonCancel.fontcolor = fontcolor;

	_tButtonOk.Move(height - 2, (width/2)-12);
	_tButtonCancel.Move(height - 2, (width/2)+3);
	_tInputBox.Move(3, MIDDLE);

	_tButtonOk.SetTxtAlign(MIDDLE);
	_tButtonCancel.SetTxtAlign(MIDDLE);
	
	_tButtonOk.Show();
	_tButtonCancel.Show();
	_tInputBox.Show();
	
	if (_bStat == false)
	{
		if (_bYes == true)
		{
			_tButtonOk.SetFocus(true);
			_tButtonCancel.SetFocus(false);
		}
		else
		{
			_tButtonOk.SetFocus(false);
			_tButtonCancel.SetFocus(true);
		}
	}
	else
	{
		_tButtonOk.SetFocus(false);
		_tButtonCancel.SetFocus(false);
		_tInputBox.SetFocus();
	}
}

bool	Curses_InputBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (_tButtonOk.AreaChk(Y, X))
	{
		_tButtonOk.SetFocus(true);
		_bYes = true;
		if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
		return true;
	}

	if (_tButtonCancel.AreaChk(Y, X))
	{
		_tButtonCancel.SetFocus(true);
		_bYes = false;
		if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
		return true;
	}

	if (_tInputBox.AreaChk(Y, X))
	{
		_bStat = true;
		return true;
	}
	return false;
}

void	Curses_InputBox::Execute(KeyInfo& tKeyInfo)
{
	int nKey = (int)tKeyInfo;

	if (_bStat == true)
	{
		nKey = _tInputBox.DataInput(tKeyInfo);
		
		switch(nKey)
		{
			case KEY_MOUSE:
				MouseProc();
				break;
			case KEY_ENTER:
			case 13:
				_bExit = true;
			case KEY_TAB:
				_bYes = true;
				_bStat = false;
				break;
			case KEY_ESC:
				_bYes = false;	
				_bExit = true;
				break;
			default:
				_bStat = false;
		}
	}
	else
	{
		switch(nKey)
		{
			case KEY_TAB:
				if (_bYes == false)
					_bStat = true;
				else
					_bYes = !_bYes;
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
			case KEY_UP:
			case KEY_DOWN:
				_bYes = !_bYes;
				break;
			case KEY_ESC:
				_bYes = false;
			case KEY_ENTER:
			case 13:
				_bExit = true;
				break;
		}
	}
}

// Progress Box

Curses_ProgressBox::Curses_ProgressBox(const string& sTitle, const string& sMsg, bool bDouble): Form()
{
	SetTitle(sTitle);
	_sTitle = sTitle; _sMsg = sMsg;	_bDouble = bDouble;
	_bExit = false;

	_tButtonCancel = Button(_("Cancel"), 12);
	_tLabel	 = Label(sMsg);

	_tLabelUpLeft = Label("");	_tLabelUpRight = Label("");
	_tLabelDnLeft = Label("");	_tLabelDnRight = Label("");

	fontcolor = g_tColorCfg.GetColorEntry("Dialog").font;
	backcolor = g_tColorCfg.GetColorEntry("Func").back;
	_tLabel.fontcolor = _tLabelUpLeft.fontcolor = _tLabelUpRight.fontcolor = fontcolor;
	_tLabelDnLeft.fontcolor = _tLabelDnRight.fontcolor = fontcolor;
	_tLabel.backcolor = _tLabelUpLeft.backcolor = _tLabelUpRight.backcolor = backcolor;
	_tLabelDnLeft.backcolor = _tLabelDnRight.backcolor = backcolor;
}

void Curses_ProgressBox::setCount(int nCnt, int nCnt2)
{
	_nCnt = nCnt;
	if (nCnt2 != -1) _nCnt2 = nCnt2;
}

void	Curses_ProgressBox::Draw()
{
	_tProgress1.SetForm((Form*)this);
	_tProgress2.SetForm((Form*)this);
	_tButtonCancel.SetForm((Form*)this);
	_tLabel.SetForm((Form*)this);
	_tLabelUpLeft.SetForm((Form*)this);
	_tLabelUpRight.SetForm((Form*)this);
	_tLabelDnLeft.SetForm((Form*)this);
	_tLabelDnRight.SetForm((Form*)this);

	_tLabel.fontcolor = _tLabelUpLeft.fontcolor = _tLabelUpRight.fontcolor = fontcolor;
	_tLabelDnLeft.fontcolor = _tLabelDnRight.fontcolor = fontcolor;
	_tLabel.backcolor = _tLabelUpLeft.backcolor = _tLabelUpRight.backcolor = backcolor;
	_tLabelDnLeft.backcolor = _tLabelDnRight.backcolor = backcolor;

	_tProgress1.width = width - 4;
	_tProgress2.width = width - 4;

	_tButtonCancel.Move(height - 2, MIDDLE);
	_tButtonCancel.SetTxtAlign(MIDDLE);
	_tButtonCancel.Show();

	_tLabel.SetTxtAlign(MIDDLE);
	_tLabel.Move(3, MIDDLE);
	_tLabel.Show();

	_tProgress1.Move(5, MIDDLE);
	_tProgress1.SetPercent(_nCnt);
	_tProgress1.Show();

	setcol(fontcolor, backcolor, GetWin());
	wmove(GetWin(), 4, _tProgress1.x);
	whline(GetWin(), ' ', _tProgress1.width);

	_tLabelUpLeft.SetTxtAlign(LEFT);
	_tLabelUpLeft.Move(4, _tProgress1.x);
	_tLabelUpLeft.Show();

	_tLabelUpRight.SetTxtAlign(LEFT);
	_tLabelUpRight.Move(4, _tProgress1.x + (_tProgress1.width - scrstrlen(_sRightStr)));
	_tLabelUpRight.Show();

	if (_bDouble)
	{
		_tProgress2.Move(7, MIDDLE);
		_tProgress2.SetPercent(_nCnt2);
		_tProgress2.Show();

		setcol(fontcolor, backcolor, GetWin());
		wmove(GetWin(), 8, _tProgress2.x);
		whline(GetWin(), ' ', _tProgress2.width);

		_tLabelUpLeft.SetTxtAlign(LEFT);
		_tLabelDnLeft.Move(8, _tProgress2.x);
		_tLabelDnLeft.Show();
		_tLabelDnRight.SetTxtAlign(LEFT);
		_tLabelDnRight.Move(8, _tProgress2.x + _tProgress2.width - scrstrlen(_sRightStr2));
		_tLabelDnRight.Show();
	}

	_tButtonCancel.SetFocus(true);
}

bool	Curses_ProgressBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (_tButtonCancel.AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
	{
		_tButtonCancel.SetFocus(true);
		_bExit = true;
		return true;
	}
	return false;
}

void	Curses_ProgressBox::Do()
{
	string 		sKey;
	KeyReader	tKeyReader;

	KeyInfo		tKeyInfo = tKeyReader.Read(stdscr, true);

	switch((int)tKeyInfo)
	{
		case KEY_MOUSE:
			MouseProc();
			if (_bExit == true) break;
			break;
		case KEY_RESIZE:
		case KEY_REFRESH:
			Refresh(_bNoOutRefresh);
			break;
		case ERR:
			break;
	}

	Execute(tKeyInfo);
}

void 	Curses_ProgressBox::Execute(KeyInfo& tKeyInfo)
{
	switch ((int)tKeyInfo)
	{
		case KEY_ESC:
		case KEY_ENTER:
		case ' ':
			_bExit = true;
			break;
	}
}

Curses_TextBox::
Curses_TextBox(const string& sTitle): Form()
{
	SetTitle(sTitle);
	_tButton = Button(_("Ok"), 12);
	_tTextBox = TextBox(1, 1, 10, 40);
	fontcolor = g_tColorCfg.GetColorEntry("Help").font;
	backcolor = g_tColorCfg.GetColorEntry("Help").back;
	LOG_WRITE("TextBox Color [%d][%d]", fontcolor, backcolor);
	_tButton.backcolor = backcolor; _tButton.fontcolor = fontcolor;
	_tTextBox.backcolor = backcolor; _tTextBox.fontcolor = fontcolor;
	_bESC = false;
}

void	Curses_TextBox::setText(vector<string>& vText)
{
	_tTextBox.SetStringList( vText );
}

void	Curses_TextBox::setCurShow( bool bCursor )
{
	_tTextBox.ShowCursor( bCursor );
}

void	Curses_TextBox::Draw()
{
	_tButton.backcolor = backcolor; _tButton.fontcolor = fontcolor;
	_tTextBox.backcolor = backcolor; _tTextBox.fontcolor = fontcolor;

	_tButton.SetForm((Form*)this);
	_tTextBox.SetForm((Form*)this);

	if ( width > 4 )
		_tTextBox.width = width - 4;
	else 
		_tTextBox.height = 1;

	if ( height > 6 )
		_tTextBox.height = height - 6;
	else
		_tTextBox.height = 1;

	_tTextBox.Move(3, MIDDLE);
	_tTextBox.Show();

	_tButton.Move(height - 2, MIDDLE);
	_tButton.SetTxtAlign(MIDDLE);
	_tButton.Show();
	_tButton.SetFocus(true);
}

void	Curses_TextBox::Execute(KeyInfo& tKeyInfo)
{
	switch ((int)tKeyInfo)
	{
		case KEY_DOWN:
		case KEY_RIGHT:
			_tTextBox.Down();
			break;
		case KEY_UP:
		case KEY_LEFT:
			_tTextBox.Up();
			break;
		case KEY_PPAGE:
			_tTextBox.PageUp();
			break;
		case KEY_NPAGE:
			_tTextBox.PageDown();
			break;
		case KEY_ESC:
			_bESC = true;
		case KEY_ENTER:
		case 13:
		case ' ':
			_bExit = true;
			break;
	}
}

bool	Curses_TextBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	if (_tButton.AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
	{
		_tButton.SetFocus(true);
		_bExit = true;
		LOG_WRITE("MouseEvent :: Button Click");
		return true;
	}
	return false;
}

// Check Box
Curses_CheckBox::Curses_CheckBox(const string& sTitle): Form()
{
	_sTitle = sTitle;
	_nCur = 0;

	_tButtonOk = Button(_("Ok"), 12);
	_tButtonCancel = Button(_("Cancel"), 12);
	fontcolor = g_tColorCfg.GetColorEntry("SelectBox").font;
	backcolor = g_tColorCfg.GetColorEntry("SelectBox").back;

	_tButtonOk.backcolor = backcolor; 	_tButtonCancel.backcolor = backcolor;
	_tButtonOk.fontcolor = fontcolor;	_tButtonCancel.fontcolor = fontcolor;

	_bCheckList = true;
	_bYes = true;
}

void	Curses_CheckBox::Clear()
{
	CheckRadioButton*		pButton = NULL;
	for (int n = 0; n<(int)_vChkButtonList.size(); n++)
	{
		pButton = _vChkButtonList[n];
		delete pButton;
	}
}

void	Curses_CheckBox::SetChkData(vector<string>&	vData, vector<bool>& vChkList)
{
	CheckRadioButton*		pButton = NULL;

	Clear();
	_vData.clear();

	if ( vData.size() != vChkList.size() )
		throw Exception("Curses_CheckBox::SetChkData vData.size() != vChkList.size()");

	_nStrMaxLength = 36;
	
	for (int n = 0; n < (int)vData.size(); n++)
	{
		pButton = new CheckRadioButton(vData[n], true);
		if (_nStrMaxLength < scrstrlen(vData[n])) _nStrMaxLength = scrstrlen(vData[n]);
		
		_vChkButtonList.push_back(pButton);
	}

	if (_nStrMaxLength > width) 
		width = _nStrMaxLength + 4;

	height = vData.size() + 6;
	_nStrMaxLength = width - 4;
	
	if (scrstrlen(_sTitle) > width) 
		width = scrstrlen(_sTitle) + 4;

	_vData = vData;
	_vChkList = vChkList;
}

void	Curses_CheckBox::Draw()
{
	CheckRadioButton*	pButton = NULL;

	for (int n = 0; n<(int)_vChkButtonList.size(); n++)
	{
		pButton = _vChkButtonList[n];
		pButton->SetForm((Form*)this);
		pButton->width = width - 4;
		pButton->Move( 3 + n, MIDDLE );
		pButton->SetCheck( _vChkList[n] );
		pButton->Show();
		pButton->backcolor = backcolor; 
		pButton->fontcolor = fontcolor;
		if ( _bCheckList )
		{
			if (_nCur == n)
				pButton->SetFocus(true);
			else
				pButton->SetFocus(false);
		}
		else
			pButton->SetFocus(false);
	}

	_tButtonOk.SetForm((Form*)this);
	_tButtonCancel.SetForm((Form*)this);
	
	_tButtonOk.Move(height - 2, (width/2)-12);
	_tButtonCancel.Move(height - 2, (width/2)+3);
	
	_tButtonOk.SetTxtAlign(MIDDLE);
	_tButtonCancel.SetTxtAlign(MIDDLE);

	_tButtonOk.backcolor = backcolor; 	_tButtonCancel.backcolor = backcolor;
	_tButtonOk.fontcolor = fontcolor;	_tButtonCancel.fontcolor = fontcolor;
	
	_tButtonOk.Show();
	_tButtonCancel.Show();
	
	if (_bCheckList == false)
	{
		if (_bYes == true)
		{
			_tButtonOk.SetFocus(true);
			_tButtonCancel.SetFocus(false);
		}
		else
		{
			_tButtonOk.SetFocus(false);
			_tButtonCancel.SetFocus(true);
		}
	}
	else
	{
		_tButtonOk.SetFocus(false);
		_tButtonCancel.SetFocus(false);
	}
}

bool	Curses_CheckBox::MouseEvent(int Y, int X, mmask_t bstate)
{
	CheckRadioButton*	pButton = NULL;

	for (int n = 0; n<(int)_vChkButtonList.size(); n++)
	{
		pButton = _vChkButtonList[n];

		if (pButton->AreaChk(Y, X) && (bstate & BUTTON1_DOUBLE_CLICKED))
		{
			_nCur = n;
			if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
			_bCheckList = true;
			return true;
		}

		if (pButton->AreaChk(Y, X))
		{
			_nCur = n;
			_bCheckList = true;
			return true;
		}
	}

	if (_tButtonOk.AreaChk(Y, X))
	{
		_tButtonOk.SetFocus(true);
		_bYes = true;
		if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
		_bCheckList = false;
		return true;
	}

	if (_tButtonCancel.AreaChk(Y, X))
	{
		_tButtonCancel.SetFocus(true);
		_bYes = false;
		if (bstate & BUTTON1_DOUBLE_CLICKED) _bExit = true;
		_bCheckList = false;
		return true;
	}

	return false;
}

void	Curses_CheckBox::Execute(KeyInfo&	tKeyInfo)
{
	if ( _bCheckList )
	{
		switch((int)tKeyInfo)
		{
			case KEY_MOUSE:
				MouseProc();
				break;
			case ' ':
			case KEY_ENTER:
			case 13:
				_vChkList[_nCur] = !_vChkList[_nCur];
				break;
			case KEY_TAB:
				_bYes = true;
				_bCheckList = false;
				break;
			case KEY_ESC:
				_bYes = false;
				_bCheckList = false;
				break;
			case KEY_UP:
			case KEY_LEFT:
				_nCur--;
				if (_nCur < 0) _nCur = _vData.size() - 1;
				break;	
			case KEY_DOWN:
			case KEY_RIGHT:
				_nCur++;
				if ((int)_vData.size() <= _nCur) _nCur = 0;
				break;
		}
	}
	else
	{
		switch((int)tKeyInfo)
		{
			case KEY_TAB:
				if (_bYes == false)
					_bCheckList = true;
				else
					_bYes = !_bYes;
				break;
			case KEY_LEFT:
			case KEY_RIGHT:
			case KEY_UP:
			case KEY_DOWN:
				_bYes = !_bYes;
				break;
			case KEY_ESC:
			case KEY_ENTER:
			case 13:
				_bExit = true;
				break;
		}
	}
}

