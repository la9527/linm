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

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "config.h"

#ifdef HAVE_NCURSES_H

#include <ncurses.h>

#else
#include <curses.h>
#endif

#include "define.h"
#include "drawset.h"
#include "colorcfgload.h"
#include "keycfgload.h"
#include "keyreader.h"

namespace MLS {

    enum POSITION {
        LEFT,
        RIGHT,
        MIDDLE,
        TOP,
        BOTTOM,
        CURRENT
    };

    class Form {
    public:
        Form(WINDOW *pParentWin = NULL);

        virtual ~Form() {}

        void SetTitle(const string &sTitle) { _sTitle = sTitle; }

        WINDOW *GetWin() { return _pWin; }

        void SetWin(WINDOW *pWin);

        void SetParentWin(WINDOW *pParent = NULL);

        inline void Move(POSITION posY, POSITION posX) {
            _posY = posY;
            _posX = posX;
        }

        inline void Move(POSITION posY, int X) {
            _posY = posY;
            x = X;
        }

        inline void Move(int Y, POSITION posY) {
            y = Y;
            _posY = posY;
        }

        inline void Move(int Y, int X) {
            y = Y;
            x = X;
        }

        inline void Size(int H, int W) {
            height = H;
            width = W;
        }

        inline bool AreaChk(int Y, int X) {
            if (y <= Y && y + height > Y)
                if (x <= X && x + width > X)
                    return true;
            return false;
        }

        void Popup();

        void Hide();

        void Show();

        void Close();

        void ScrClear();

        void *MouseEvent(int y, int x);

        virtual void Refresh(bool bNoOutRefresh = false);

        virtual void Do();

    protected:
        void DrawBox();

        void FormResize();

        void MouseProc();

        virtual bool MouseEvent(int Y, int X, mmask_t bstate) { return false; }

        virtual void DrawFirst() {}

        virtual void Draw() = 0;

        virtual void Execute(KeyInfo &tKeyInfo) {}

        virtual void DrawEtc() {}

        virtual void Resize() {}

    private:
        int _nBefMPosY, _nBefMPosX;
        mmask_t _BefMmask;
        int _nKeyErr;

    protected:
        WINDOW *_pWin;
        WINDOW *_pParentWin;
        bool _bExit;
        bool _bNotDrawBox;
        bool _bNoOutRefresh; // 내부적 Refresh 사용
        bool _bNoViewUpdate; // 화면 업데이트 사용?
        string _sTitle;

        POSITION _posX, _posY;

    public:
        int y, x;
        int height, width;
        int backcolor, fontcolor;
    };

    class Position {
    public:
        Position() : _ObjPosY(CURRENT), _ObjPosX(CURRENT) {}

        virtual ~Position() {}

        void SetForm(Form *pForm);

        inline void Move(POSITION posY, POSITION posX) {
            _ObjPosY = posY;
            _ObjPosX = posX;
        }

        inline void Move(POSITION posY, int X) {
            _ObjPosY = posY;
            x = X;
        }

        inline void Move(int Y, POSITION posX) {
            _ObjPosX = posX;
            y = Y;
        }

        inline void Move(int Y, int X) {
            x = X;
            y = Y;
        }

        inline bool AreaChk(int Y, int X) {
            if (y <= Y && y + height > Y)
                if (x <= X && x + width > X)
                    return true;
            return false;
        }

        virtual void SetFocus(bool bFocus = true) {}

        virtual bool GetFocus() { return false; }

        void Show() {
            Resize();
            Draw();
        }

        int x, y, height, width;
        int fontcolor;
        int backcolor;

    protected:
        virtual void Draw() = 0;

        void Resize();

        Form *_pForm;
        bool _bShow;
        POSITION _ObjPosY, _ObjPosX;
    };

    class Label : public Position {
    public:
        Label(const string &sMsg);

        Label(POSITION Y, POSITION X, const string &sMsg);

        Label(int Y, POSITION X, const string &sMsg);

        Label(POSITION Y, int X, const string &sMsg);

        Label(int Y, int X, const string &sMsg);

        Label();

        virtual ~Label() {}

        void SetSize(int nHeight, int nWidth, POSITION posX = LEFT);

        void SetMsg(const string &sMsg);

        inline void SetTxtAlign(POSITION TextPos) { _TextPos = TextPos; }

    protected:
        void Draw();

    private:
        string _sMsg;
        POSITION _TextPos;
    };

    class Button : public Position {
    public:
        Button(const string &str, int W = 15);

        Button(int Y, int X, int W = 15);

        Button();

        virtual ~Button() {}

        void SetSize(int Y, int X, int W = 15);

        void SetTxtAlign(POSITION posX = MIDDLE);

        void SetMsg(const string &sMsg);

        void SetFocus(bool bFocus = true);

        bool GetFocus() { return _bFocus; }

    protected:
        void Draw();

    private:
        string _sMsg;
        bool _bFocus;
        POSITION _MsgPosition;
    };

    class Input : public Position {
    public:
        Input(const string &str = "", bool bFirstCursorEnd = true);

        virtual ~Input() {}

        void SetPasswdType(bool bType = true) { _bPasswdType = bType; }

        void SetSize(int nHeight, int nWidth) {
            height = nHeight;
            width = nWidth;
        }

        int DataInput(KeyInfo &tKeyInfoBef);

        void SetFocus(bool bFocus = true);

        void SetClear();

        inline string GetStr() { return wstrtostr(_sWStr); }

        void SetStr(const string &str, bool bFirstCursorEnd = true) {
            _sWStr = strtowstr(str);
            _bFirstCursorEnd = bFirstCursorEnd;

            if (_bFirstCursorEnd)
                _nCur = _sWStr.size();
            else
                _nCur = 0;
            if ((int) _nCur > width - 6)
                _nFirst = _nCur - width + 3;
            else
                _nFirst = 0;
            if (_nFirst < 0) _nFirst = 0;
        }

    protected:
        void Draw();

    private:
        wstring _sWStr;
        int _nCur;
        int _nFirst;
        int _nEnd;
        bool _bPasswdType;
        bool _bEnter;
        bool _bExit;
        bool _bMouse;
        bool _bFirstCursorEnd;
    };

    class CheckRadioButton : public Position {
    public:
        CheckRadioButton();

        CheckRadioButton(const string &str, bool bCheckButton = true);

        CheckRadioButton(int Y, int X, int W = 20, bool bCheckButton = true);

        ~CheckRadioButton() {}

        void SetSize(int X, int Y, int W = 20);

        void SetMsg(const string &sMsg);

        string GetMsg();

        void SetFocus(bool bFocus = true);

        bool GetFocus() { return _bFocus; }

        void SetCheck(bool bCheck = true);

        bool GetCheck() { return _bCheck; }

        void SetRightBoxPos(bool bRight = true);

    protected:
        void Draw();

    private:
        string _sMsg;
        bool _bRight;
        bool _bFocus;
        bool _bCheck;
        bool _bCheckButton;
    };

    class Progress : public Position {
    public:
        Progress(int Y = 0, int X = 0, int W = 30) {
            x = X;
            y = Y;
            height = 1;
            width = W;
            fontcolor = COLOR_WHITE;
            backcolor = 0;
            _nNum = 0;
        }

        inline void SetPercent(int nPercent = 0) {
            if (nPercent > 100) nPercent = 100;
            if (nPercent < 0) nPercent = 0;
            _nNum = (int) (((double) nPercent / 100.0) * (double) width);
        }

    protected:
        inline void Draw() {
            WINDOW *pWin = _pForm->GetWin();

            wattron(pWin, COLOR(fontcolor, backcolor));
            wattron(pWin, A_BOLD);
            wmove(pWin, y, x);
            whline(pWin, '.', width);
            wattroff(pWin, A_BOLD);

            wattron(pWin, COLOR(fontcolor, fontcolor));
            wmove(pWin, y, x);
            whline(pWin, ' ', _nNum);
            wnoutrefresh(pWin);
        }

    private:
        int _nNum;
    };

    class TextLineInfo {
    public:
        int nTextLine;        /// Text Position
        int nViewLine;        /// screen view position
        int nNextLineNum;    /// 한 라인을 넘어 서는 경우에 라인 위치
        bool bNext;            /// 한 라인을 넘어섰는가?
        wstring sWString;        /// wstring
    };

    class TextBox : public Position {
    public:
        TextBox(int Y = 0, int X = 0, int H = 10, int W = 30);

        void SetStringList(vector<string> &vStrList);

        void GotoFirst();

        void GotoEnd();

        void Down();

        void Up();

        void PageDown();

        void PageUp();

        void ShowCursor(bool bCursor) { _bCursor = bCursor; }

        int GetCur() { return _nCurLine; }

        void SetCur(int nCur) {
            _nFirstLine = nCur;
            _nCurLine = nCur;
            ScreenMemSave(_nLine, _nCulumn);
        }

    protected:
        void ScreenMemSave(int nLine, int nCulumn);

        void LineDraw(const wstring &sViewWStr, int nY, int nN, int nTextNum);

        void Draw();

    protected:
        ColorEntry _tColor;

    private:
        vector<wstring> _vText;
        vector<TextLineInfo> _vViewString;
        bool _bCursor;
        int _nFirstLine, _nLastLine, _nCurLine, _nLine, _nCulumn, _nRow;
    };

};

#endif

