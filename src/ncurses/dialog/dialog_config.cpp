#include "define.h"
#include "strutil.h"
#include "dialog.h"
#include "dialog_config.h"

using namespace MLS;

class	InputBox
{
public:
	Input		_input;
	Label		_label;
};

Dialog_Config::Dialog_Config(): Form()
{
    SetTitle( _("Configure") );

    _tOk             = Button(_("Ok"), 10);
    _tCancel         = Button(_("Cancel"), 10);

	_bOk = false;
	_nCurNum = 0;

	fontcolor = g_tColorCfg.GetColorEntry("Property").font;
	backcolor = g_tColorCfg.GetColorEntry("Property").back;
}

Dialog_Config::~Dialog_Config()
{
	for ( int n1 = 0; n1 < _vConfigOBJS.size(); n1++ )
	{
		const ConfigOBJ& obj = _vConfigOBJS[n1];
		switch( obj._nViewType )
		{
			case CHECKBOX:
			{
				delete (CheckRadioButton*)obj._pBox;
				break;
			}
			case INPUTBOX:	
			{
				delete (InputBox*)obj._pBox;
				break;
			}
		}
	}
}

void Dialog_Config::SetViewConfig(const vector<ConfigOBJ>& vConfigOBJS)
{
	_vConfigOBJS = vConfigOBJS;

	int		nStrWidth = 30;

	for( int n = 0; n < _vConfigOBJS.size(); n++ )
	{
		const ConfigOBJ& obj = _vConfigOBJS[n];

		if ( MLSUTIL::scrstrlen( obj._text ) > nStrWidth )
			nStrWidth = MLSUTIL::scrstrlen( obj._text );
	}

	Size( vConfigOBJS.size() + 6, nStrWidth + 17 );

	int		nY = 3, nX = 3;

	for( int n = 0; n < _vConfigOBJS.size(); n++ )
	{
		ConfigOBJ& obj = _vConfigOBJS[n];
		
		switch( obj._nViewType )
		{
			case CHECKBOX:
			{
				CheckRadioButton*	pCheckBox = new CheckRadioButton( n + nY, nX, nStrWidth + 12 );
				pCheckBox->SetForm( this );
				pCheckBox->SetMsg( obj._text );
				pCheckBox->SetCheck( obj._value == "On" ? true : false );
				pCheckBox->SetRightBoxPos( true );
				pCheckBox->backcolor = backcolor;
				pCheckBox->fontcolor = fontcolor;

				obj._pBox = pCheckBox;
				break;
			}
			case INPUTBOX:
			{
				InputBox*	pBox = new InputBox;

				pBox->_label.SetMsg( obj._text );
				SetPosition( &pBox->_label, n+nY, nX, nStrWidth, 0 );
				
				pBox->_input.SetStr( obj._value, false );
				SetPosition( &pBox->_input, n+nY, nStrWidth+nX+2, 10, 1 );

				obj._pBox = pBox;
				break;
			}
		}
	}
}

bool	Dialog_Config::MouseEventExe(Position* pPosition, int nNum, int Y, int X, mmask_t bstate)
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

bool Dialog_Config::MouseEvent(int Y, int X, mmask_t bstate)
{
	for( int n = 0; n < _vConfigOBJS.size(); n++ )
	{
		if ( _vConfigOBJS[n]._nViewType == CHECKBOX )
		{
			CheckRadioButton* pCheck = (CheckRadioButton*)_vConfigOBJS[n]._pBox;
			if ( pCheck && MouseEventExe( pCheck, n, Y, X, bstate ) )
			{
				pCheck->SetCheck( !pCheck->GetCheck() );
				return true;
			}
		}
		else if ( _vConfigOBJS[n]._nViewType == INPUTBOX )
		{
			InputBox* pInputBox = (InputBox*)_vConfigOBJS[n]._pBox;
			if ( pInputBox && MouseEventExe( &pInputBox->_input, n, Y, X, bstate ) )
				return true;
		}
	}

	if ( MouseEventExe( &_tOk,  _vConfigOBJS.size(), Y, X, bstate ) )
    {
		_bOk = true;
		_bExit = true;
        return true;
    }

    if ( MouseEventExe( &_tCancel, _vConfigOBJS.size() + 1, Y, X, bstate ) )
    {
        _bExit = true;
        return true;
    }
    return false;
}

void        Dialog_Config::SetPosition(Position* pPosition, int y, int x, int width, int nColNum)
{
    pPosition->SetForm( this );
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

void Dialog_Config::Draw()
{
	if ( !_vConfigOBJS.size() ) return;
	
	for( int n = 0; n < _vConfigOBJS.size(); n++ )
	{
		if ( _vConfigOBJS[n]._nViewType == CHECKBOX )
		{
			Position* pPos = (Position*)_vConfigOBJS[n]._pBox;
	
			if ( pPos )
			{
				pPos->SetForm( this );
				pPos->Show();
				pPos->SetFocus( false );
			}
		}
		if ( _vConfigOBJS[n]._nViewType == INPUTBOX )
		{
			InputBox* pInputBox = (InputBox*)_vConfigOBJS[n]._pBox;
	
			if ( pInputBox )
			{
				pInputBox->_label.SetForm( this );
				pInputBox->_label.Show();

				pInputBox->_input.SetForm( this );
				pInputBox->_input.Show();
				pInputBox->_input.SetFocus( false );
			}
		}
	}

	Position* pPos = NULL;

	if ( _vConfigOBJS[_vConfigOBJS.size()-1]._nViewType == CHECKBOX )
		pPos = (Position*)_vConfigOBJS[_vConfigOBJS.size()-1]._pBox;
	else
	{
		InputBox*	pInputBox = (InputBox*)_vConfigOBJS[_vConfigOBJS.size()-1]._pBox;
		if ( pInputBox )
			pPos = (Position*)&pInputBox->_input;
	}

	SetPosition( &_tOk, 	pPos ? pPos->y + 2 : 2, (width/2) - 13, 10, 2 );
	SetPosition( &_tCancel, pPos ? pPos->y + 2 : 2, (width/2) + 3, 10, 2 );

	if ( _nCurNum < _vConfigOBJS.size() )
	{
		if ( _vConfigOBJS[_nCurNum]._nViewType == CHECKBOX )
		{
			Position* pPos = (Position*)_vConfigOBJS[_nCurNum]._pBox;

			if ( pPos ) pPos->SetFocus( true );
		}
		else
		{
			InputBox* pInputBox = (InputBox*)_vConfigOBJS[_nCurNum]._pBox;

			if ( pInputBox ) pInputBox->_input.SetFocus( true );
		}
	}

	if ( _vConfigOBJS.size() == _nCurNum )  	_tOk.SetFocus(true);
	if ( _vConfigOBJS.size() + 1 == _nCurNum)  _tCancel.SetFocus(true);
}

void Dialog_Config::ViewObjExec( int nViewType, Position* pPos, KeyInfo& tKeyInfo )
{
	int nKey = 0;

	if ( nViewType == INPUTBOX ) // Input
	{
		Input*	pInput = static_cast<Input*>(pPos);

		if ( pInput )
    		nKey = pInput->DataInput( tKeyInfo );
	}
	else
		nKey = (int)tKeyInfo;

    switch( nKey )
    {
        case KEY_MOUSE:
            MouseProc();
            break;
		case ' ':
        case KEY_ENTER:
        case 13:
		{
			if ( nViewType == CHECKBOX && _nCurNum < _vConfigOBJS.size() )
			{
				CheckRadioButton* pBox = static_cast<CheckRadioButton*>(_vConfigOBJS[_nCurNum]._pBox);
				pBox->SetCheck( !pBox->GetCheck() );
			}
		}
        case KEY_DOWN:
        case KEY_TAB:
            _nCurNum++;
            if ( _nCurNum > _vConfigOBJS.size() + 1 ) _nCurNum = 0;
            break;
        case KEY_UP:
            _nCurNum--;
            if ( _nCurNum < 0 ) _nCurNum = _vConfigOBJS.size() + 1;
            break;
        case KEY_ESC:
            _nCurNum = _vConfigOBJS.size() + 1; // Cancel Button
            break;
    }
}

void Dialog_Config::ButtonExec( Button* pButton, KeyInfo& tKeyInfo )
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
			if ( _nCurNum == _vConfigOBJS.size() )   _bOk = true;
			if ( _nCurNum == _vConfigOBJS.size() || _nCurNum == _vConfigOBJS.size()+1 ) _bExit = true;
			break;
		case KEY_RIGHT:
		case KEY_DOWN:
		case KEY_TAB:
            _nCurNum++;
            if ( _nCurNum > _vConfigOBJS.size()+1 ) _nCurNum = 0;
            break;
        case KEY_LEFT:
		case KEY_UP:
		{
			_nCurNum--;
			if ( _nCurNum < 0 ) _nCurNum = _vConfigOBJS.size()+1;
			break;
		}
		case KEY_ESC:
			_nCurNum = _vConfigOBJS.size()+1; // Cancel Button
			break;
    }
}

void Dialog_Config::Execute(KeyInfo & tKeyInfo)
{
	if ( _nCurNum < _vConfigOBJS.size() )
	{
		if ( _vConfigOBJS[_nCurNum]._nViewType == CHECKBOX )
		{
			ViewObjExec(_vConfigOBJS[_nCurNum]._nViewType,
						(Position*)_vConfigOBJS[_nCurNum]._pBox, tKeyInfo );
		}
		else
		{
			ViewObjExec(_vConfigOBJS[_nCurNum]._nViewType,
						(Position*)&((InputBox*)_vConfigOBJS[_nCurNum]._pBox)->_input, tKeyInfo );
		}
	}
	else
	{
		if ( _nCurNum == _vConfigOBJS.size())
			ButtonExec( &_tOk, tKeyInfo );
		else if ( _nCurNum == _vConfigOBJS.size()+1)
			ButtonExec( &_tCancel, tKeyInfo );
	} 
}

const vector<ConfigOBJ>& Dialog_Config::getConfigOBJS()
{
	for ( int n = 0; n < _vConfigOBJS.size(); n++ )
	{
		if ( _vConfigOBJS[n]._nViewType == INPUTBOX )
		{
			InputBox* pBox = static_cast<InputBox*>(_vConfigOBJS[n]._pBox);
			if ( pBox )
				_vConfigOBJS[n]._value = pBox->_input.GetStr();
		}
		else if ( _vConfigOBJS[n]._nViewType == CHECKBOX )
		{
			CheckRadioButton* pBox = static_cast<CheckRadioButton*>(_vConfigOBJS[n]._pBox);
			if ( pBox )
				_vConfigOBJS[n]._value = pBox->GetCheck() ? "On" : "Off";
		}
	}
	return _vConfigOBJS;
}
