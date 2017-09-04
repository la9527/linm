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

#ifndef __DIALOG_REPLACE_H__
#define __DIALOG_REPLACE_H__

#include "dialog.h"

namespace MLS
{

class   Dialog_Replace: public Form
{
public:
    Dialog_Replace(const string& sTitle, const string& strFind = "", const string& strReplace = "");
    
    string  GetFind()       { return _tFind.GetStr(); }
    string  GetReplace()    { return _tReplace.GetStr(); }
    bool    GetOk()         { return _bOk; }

protected:  
    void    InputExec( Input* pInput, KeyInfo& tKeyInfo );
    void    ButtonExec( Button* pButton, KeyInfo& tKeyInfo );

    void    SetPosition(Position* pPosition, int y, int x, int width, int nColNum);

    bool    MouseEvent(int Y, int X, mmask_t bstate);
    void    Draw();
    void    Execute(KeyInfo& tKeyInfo);
    
private:
    Label           _tLabelFind;
    Label           _tLabelReplace;

    Input           _tFind;
    Input           _tReplace;

    Button          _tOk;
    Button          _tCancel;
    
    bool            _bOk;
    int             _nCurNum;
};

};

#endif  // __DIALOG_REPLACE_H__
