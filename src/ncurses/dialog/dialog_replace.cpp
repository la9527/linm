/******************************************************************************
 *   Copyright (C) 2005 by la9527                                             *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      * 
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*
 ******************************************************************************/

#include "dialog.h"
#include "dialog_replace.h"

using namespace MLS;

Dialog_Replace::Dialog_Replace(const string & sTitle, const string & strFind, const string & strReplace)
{
    SetTitle( sTitle );

    _tLabelFind     = Label(_("Find"));
    _tLabelReplace  = Label(_("Replace"));

    _tOk             = Button(_("Ok"), 10);
    _tCancel         = Button(_("Cancel"), 10);

    _tFind.SetStr( strFind );
    _tReplace.SetStr( strReplace );

    _nCurNum = 0;
    _bOk = false;

    fontcolor = g_tColorCfg.GetColorEntry("Property").font;
    backcolor = g_tColorCfg.GetColorEntry("Property").back;
}

bool Dialog_Replace::MouseEvent(int Y, int X, mmask_t bstate)
{
    if (_tFind.AreaChk(Y, X))      
    {
        _nCurNum = 0;
        return true;
    }
    if (_tReplace.AreaChk(Y, X))   
    {
        _nCurNum = 1;
        return true;
    }

    if (_tOk.AreaChk(Y, X))        
    {
        _nCurNum = 2;
        return true;
    }

    if (_tCancel.AreaChk(Y, X))
    {
        _nCurNum = 3;
        return true;
    }
    return false;
}

void        Dialog_Replace::SetPosition(Position* pPosition, int y, int x, int width, int nColNum)
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

void Dialog_Replace::Draw()
{
    SetPosition(&_tLabelFind    , 3, 3,  10, 0 );
    SetPosition(&_tLabelReplace , 5, 3, 10, 0 );

    SetPosition(&_tFind    , 3, 15, 32, 1 );
    SetPosition(&_tReplace , 5, 15, 32, 1 );

    SetPosition(&_tOk       , 7, 10, 10, 2 );
    SetPosition(&_tCancel   , 7, 30, 10, 2 );

    if (_nCurNum == 0)  _tFind.SetFocus(true);
    if (_nCurNum == 1)  _tReplace.SetFocus(true);
    if (_nCurNum == 2)  _tOk.SetFocus(true);
    if (_nCurNum == 3)  _tCancel.SetFocus(true);
}


void Dialog_Replace::InputExec( Input* pInput, KeyInfo& tKeyInfo )
{
    int nKey = pInput->DataInput( tKeyInfo );

    switch( nKey )
    {
        case KEY_MOUSE:
            MouseProc();
            break;
        case KEY_ENTER:
        case 13:
        case KEY_DOWN:
        case KEY_TAB:
            _nCurNum++;
            if ( _nCurNum > 3 ) _nCurNum = 0;
            break;
        case KEY_UP:
            _nCurNum--;
            if ( _nCurNum < 0 ) _nCurNum = 3;
            break;
        case KEY_ESC:
            _nCurNum = 3; // Cancel Button
            break;
    }
}

void Dialog_Replace::ButtonExec( Button* pButton, KeyInfo& tKeyInfo )
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
            if ( _nCurNum == 2 )   _bOk = true;
            if ( _nCurNum == 2 || _nCurNum == 3 ) _bExit = true;
            break;
        case KEY_DOWN:
        case KEY_RIGHT:
        case KEY_TAB:
            _nCurNum++;
            if ( _nCurNum > 3 ) _nCurNum = 0;
            break;
        case KEY_LEFT:
        case KEY_UP:
            _nCurNum--;
            if ( _nCurNum < 0 ) _nCurNum = 3;
            break;
        case KEY_ESC:
            _nCurNum = 3; // Cancel Button
            break;
    }
}

void Dialog_Replace::Execute(KeyInfo & tKeyInfo)
{
    switch( _nCurNum )
    {
        case 0: InputExec( &_tFind, tKeyInfo );    break;
        case 1: InputExec( &_tReplace, tKeyInfo ); break;
        case 2: ButtonExec( &_tOk, tKeyInfo );     break;
        case 3: ButtonExec( &_tCancel, tKeyInfo ); break;
    }
}
