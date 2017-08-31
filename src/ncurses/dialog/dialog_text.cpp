#include "mlslocale.h"
#include "strlinetoken.h"
#include "dialog.h"

using namespace MLS;
using namespace MLSUTIL;

TextBox::TextBox(int Y, int X, int H, int W) {
    x = X;
    y = Y;
    height = H;
    width = W;
    fontcolor = COLOR_WHITE;
    backcolor = COLOR_BLUE;

    _vText.clear();

    _nFirstLine = 0;
    _nLastLine = 0;
    _nCurLine = 0;
    _nLine = 0;
    _nCulumn = 0;

    _tColor.font = fontcolor;
    _tColor.back = backcolor;
}

void TextBox::SetStringList(vector<string> &vStrList) {
    _vText.clear();

    _nFirstLine = 0;
    _nLastLine = 0;
    _nCurLine = 0;

    _nLine = 0;
    _nCulumn = 0;

    _tColor.font = fontcolor;
    _tColor.back = backcolor;

    for (uint i = 0; i < vStrList.size(); i++)
        _vText.push_back(strtowstr(vStrList[i]));

    ScreenMemSave(height, width);
}

void TextBox::ScreenMemSave(int nLine, int nCulumn) {
    wstring sViewWStr, sLineWStr;
    StrLineToken tStrLineToken;

    if (_nCurLine > 0) {
        if (_nCurLine >= (int) _vText.size()) _nCurLine = _vText.size() - 1;
        if (_nCurLine <= _nFirstLine) _nFirstLine = _nFirstLine - 1;
        if (_nFirstLine <= 0) _nFirstLine = 0;
        if (_nLastLine - _nFirstLine >= 5 && _nLastLine - _nCurLine <= 0) {
            if ((int) _vViewString.size() >= nLine)
                if (_nFirstLine <= (int) _vText.size())
                    _nFirstLine = _nFirstLine + 1;

            if ((int) _vText.size() <= _nLine - 3)
                _nFirstLine = 0;
        }
    }

    for (;;) {
        int nViewLine = _nFirstLine;

        if (nViewLine < 0) return;

        _vViewString.clear();

        TextLineInfo tLineInfo;
        bool bNext = false;

        for (int t = 0; t < nLine; t++) {
            if (!tStrLineToken.NextChk()) {
                if (nViewLine >= (int) _vText.size()) break;
                sLineWStr = _vText[nViewLine];
                tStrLineToken.SetWString(sLineWStr, nCulumn);
                nViewLine++;
            }

            if (tStrLineToken.LineSize() - 1 == tStrLineToken.GetCurNum())
                bNext = false;
            else
                bNext = true;

            sViewWStr = tStrLineToken.Get();

            tLineInfo.nViewLine = t;
            tLineInfo.nTextLine = nViewLine - 1;
            tLineInfo.sWString = sViewWStr;
            tLineInfo.bNext = bNext;
            tLineInfo.nNextLineNum = tStrLineToken.GetCurNum();
            _vViewString.push_back(tLineInfo);
            tStrLineToken.Next();
        }
        _nLastLine = nViewLine - 1;

        if ((int) _vViewString.size() > nLine - 3) {
            if (_nLastLine == _nCurLine && tLineInfo.bNext == true) {
                _nFirstLine++;
                continue;
            }
            if (_nLastLine < _nCurLine) {
                _nFirstLine++;
                continue;
            }
        }
        break;
    }
    return;
}

void TextBox::LineDraw(const wstring &sViewWStr,
                       int nY,
                       int nN,
                       int nTextNum) {
    WINDOW *pWin = _pForm->GetWin();

    setcol(_tColor, pWin);
    wmove(pWin, nY, nN);
    whline(pWin, ' ', width);

    wstring sLineWStr = _vText[nTextNum];
    wstring sWStr;
    StrLineToken tStrLineToken;
    tStrLineToken.SetWString(sLineWStr, _nCulumn);

    if (_nCurLine == nTextNum && _bCursor) {
        string sViewStr = wstrtostr(sViewWStr);
        setrcol(_tColor, pWin); // 반전
        wmove(pWin, nY, nN);
        whline(pWin, ' ', width);
        mvwprintw(pWin, nY, nN, "%s", sViewStr.c_str());
    } else if (_nCurLine == nTextNum) {
        string sViewStr = wstrtostr(sViewWStr);
        setcol(COLOR_YELLOW, _tColor.back, pWin);
        mvwprintw(pWin, nY, nN, "%s", sViewStr.c_str());
    } else {
        string sViewStr = wstrtostr(sViewWStr);
        setcol(_tColor, pWin);
        mvwprintw(pWin, nY, nN, "%s", sViewStr.c_str());
    }
}

void TextBox::Draw() {
    WINDOW *pWin = _pForm->GetWin();

    uint nViewLine = 0;
    wstring sLineStr, sViewStr;

    TextLineInfo tLineInfo;

    _nLine = height;
    _nCulumn = width;

    ScreenMemSave(_nLine, _nCulumn);

    for (int n = 0; n < height; n++) {
        setcol(_tColor, pWin);
        wmove(pWin, y + n, x);
        whline(pWin, ' ', width);
    }

    for (int t = 0; t < _nLine; t++) {
        if ((int) _vViewString.size() > t) {
            tLineInfo = _vViewString[t];
            LineDraw(tLineInfo.sWString, y + t, x, tLineInfo.nTextLine);
        } else {
            setcol(_tColor, pWin);
            wmove(pWin, y + t, x);
            whline(pWin, ' ', _nCulumn);
        }
        nViewLine++;
    }

    /*
    setcol(_tColor, pWin);
    wmove(pWin, y+height, x);
    whline(pWin, ' ', width);
    */
}

void TextBox::GotoFirst() {
    _nCurLine = 0;
    _nFirstLine = 0;
}

void TextBox::GotoEnd() {
    _nCurLine = _vText.size() - 1;
    _nFirstLine = _nCurLine - 3;
}

void TextBox::Up() {
    if (_nCurLine > 0) _nCurLine--;
}

void TextBox::Down() {
    if (_nCurLine < (int) _vText.size() - 1) _nCurLine++;
}

void TextBox::PageUp() {
    int nSize = _nLastLine - _nFirstLine;
    int nCur = _nCurLine - _nFirstLine;

    if (_nFirstLine == 0) {
        _nCurLine = 0;
    } else {
        _nFirstLine = _nFirstLine - nSize;
        if (_nFirstLine < 0) _nFirstLine = 0;
        _nCurLine = _nFirstLine + nCur;
        if (_nCurLine <= 0) _nCurLine = 0;
    }
}

void TextBox::PageDown() {
    int nSize = _nLastLine - _nFirstLine;
    int nCur = _nCurLine - _nFirstLine;

    if ((int) _vText.size() < _nLine - 1) {
        _nCurLine = (int) _vText.size() - 1;
    } else if (_nFirstLine > (int) _vText.size() - _nLine + 1) {
        _nCurLine = (int) _vText.size() - 1;
    } else {
        _nCurLine = _nFirstLine + nSize + nCur;
        _nFirstLine = _nCurLine - nCur;

        if (_nFirstLine > (int) _vText.size() - _nLine + 1)
            _nFirstLine = (int) _vText.size() - _nLine + 1;
        if ((int) _vText.size() <= _nCurLine)
            _nCurLine = (int) _vText.size() - 1;
    }
}
