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

#ifndef __MAINFRAME_VIEW_H__
#define __MAINFRAME_VIEW_H__

#include "define.h"
#include "dialog.h"
#include "drawset.h"
#include "ncurses_panel.h"
#include "mlscfgload.h"

using namespace MLSUTIL;

namespace MLS {

    inline int GetColumnIndex(int width, int n, int i) {
        // width : 80,  n : 7 일때
        // 0, 12, 24, 36, 47, 58, 69
        int a = width / n;
        int r = width - a * n;

        if (i <= r)
            return (a + 1) * i;
        else
            return a * i + r;
    }

    inline int GetIndex(int width, int n, int x) {// inverse 함수..
        int a = width / n;
        int r = width - a * n;

        if (x <= (a + 1) * r)
            return x / (a + 1);
        else
            return (x - r) / a;
    }

    class DrawTop : public Position {
    private:
        ViewType _eViewType;

    protected:
        void Draw();

    public:
        int MouseEvent(int Y, int X, mmask_t bstate);

        void SetViewType(ViewType e) { _eViewType = e; }
    };

    class DrawPath : public Position {
    public:
        DrawPath() { _sStr = ""; }

        void SetData(const string &sStr) { _sStr = sStr; }

    protected:
        void Draw();

    private:
        string _sStr;
    };

    class HintData {
    public:
        HintData(const string &s1, const string &s2) :
                sData(s1), sValue(s2) {}

        std::string sData;
        std::string sValue;
    };

    class Hint : public Position {
        ViewType _eViewType;
        ClipState _eClip;
        McdExeMode _eMcdExeMode;

    protected:
        void Draw();

    public:
        Hint() { _eClip = CLIP_NONE; }

        void SetViewType(ViewType e) { _eViewType = e; }

        void SetClip(ClipState tClip) { _eClip = tClip; }

        void SetHintData(std::vector<HintData> &vHints);

        void SetMcdExeMode(McdExeMode eMcdExeMode) { _eMcdExeMode = eMcdExeMode; }
    };

    class StatusInfo : public Position {
    public:
        StatusInfo() { _pPanel = NULL; }

        void SetFile(NCurses_Panel *pPanel) { _pPanel = pPanel; }

    protected:
        void Draw();

    private:
        NCurses_Panel *_pPanel;
    };

    class DirInfo : public Position {
    public:
        DirInfo() { _pPanel = NULL; }

        void SetFile(NCurses_Panel *pPanel) { _pPanel = pPanel; }

    protected:
        void Draw();

    private:
        NCurses_Panel *_pPanel;
    };

    class ShellCmd : public Position {
    public:
        ShellCmd() { _pPanel = NULL; }

        void SetPanel(NCurses_Panel *pPanel) { _pPanel = pPanel; }

        string GetCmdStr() { return wstrtostr(_sWStr); }

        void DataClear() {
            _nFirst = 0;
            _nEnd = 0;
            _nStart = 0;
#ifdef __CYGWIN_C__
            _sWStr = "";
#else
            _sWStr = L"";
#endif
            _nCur = 0;
            _nTabIndex = 0;
        }

        int DataInput(KeyInfo &tKeyInfoBef);

        static string GetPromptFront();

    protected:
        string GetPrompt();

        void Draw();

        string Com_entry(vector<string> &vStr, int cur);

        vector<string> PathComplete(const string &path, bool bPathsearch);

    private:
        NCurses_Panel *_pPanel;
        int _nFirst, _nEnd;
        int _nCur;
        wstring _sWStr;
        int _nTabIndex, _nStart;
        int _nCmdWidth;
        vector<string> _vCom_entry;
    };

};

#endif

