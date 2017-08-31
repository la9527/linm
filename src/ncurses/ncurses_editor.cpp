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

#include "ncurses_editor.h"
#include "dialog_replace.h"
#include "strlinetoken.h"

using namespace MLSUTIL;
using namespace MLS;

void NCurses_Editor::Init() {
    _tColorEdit = g_tColorCfg.GetColorEntry("Edit");
    _tColorStat = g_tColorCfg.GetColorEntry("Stat");
    _tColorEditInfo = g_tColorCfg.GetColorEntry("EditInfo");
    _tColorEditInfoA = g_tColorCfg.GetColorEntry("EditInfoA");
    _tLine = g_tColorCfg.GetColorEntry("Line");
}

void NCurses_Editor::BackDrawEtc() {
    _bNoOutRefresh = true; // Refresh 할때 doupdate 하지 않는다.
    _bNoViewUpdate = true; // 업데이트 하지 않는다.
    _bNotDrawBox = true; // 박스를 그리지 않는다.

    wbkgd(_pWin, COLOR(_tColorEdit.font, _tColorEdit.back));

    setcol(_tColorEdit, _pWin);

    for (int n = 0; n < height; n++) {
        wmove(_pWin, n, 0);
        whline(_pWin, ' ', width);
    }

    if (!_bFullScreen) {
        setcol(_tLine, _pWin);
        mvwaddch (_pWin, 0, 0, ULCORNER);
        mvwaddch (_pWin, 0, width - 1, URCORNER);
        mvwaddch (_pWin, height - 1, 0, LLCORNER);
        mvwaddch (_pWin, height - 1, width - 1, LRCORNER);
        mvwhline(_pWin, 0, 1, HLINE, width - 2);
        mvwvline(_pWin, 1, 0, VLINE, height - 2);
        mvwhline(_pWin, height - 1, 1, HLINE, width - 2);
        mvwvline(_pWin, 1, width - 1, VLINE, height - 2);
    }
    // title 출력
    /*
    setcol(_tColorStat, _pWin);
    wmove(_pWin, 1, 0);
    whline(_pWin, ' ', width);
    mvwprintw(_pWin, 1, (width - krstrlen(_title))/2 , "%s", _title.c_str());
    */
}

void NCurses_Editor::SelectionDraw(const wstring &sViewWStr, int nY, int nN, int x1, int x2) {
    string sViewStr, sViewStr2, sViewStr3;
    wstring sWStr1, sWStr2, sWStr3;

    string sTab;
    sTab.append(1, (char) TABCONVCHAR);

    if (sViewWStr.size() == 0) return;

    if (x1 >= sViewWStr.size()) x1 = sViewWStr.size();
    if (x2 >= sViewWStr.size()) x2 = sViewWStr.size();

    sWStr1 = sViewWStr.substr(0, x1);
    sWStr2 = sViewWStr.substr(x1, x2 - x1);
    sWStr3 = sViewWStr.substr(x2, sViewWStr.size());

    if (sWStr1.size() != 0) {
        sViewStr = wstrtostr(sWStr1);
        sViewStr = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space


        setcol(_tColorEdit, _pWin);
        mvwprintw(_pWin, nY, nN, "%s", sViewStr.c_str());
    }
    if (sWStr2.size() != 0) {
        sViewStr2 = wstrtostr(sWStr2);
        sViewStr2 = MLSUTIL::Replace(sViewStr2, sTab.c_str(), " "); // Tab -> Space

        setrcol(_tColorEdit, _pWin);// 반전
        mvwprintw(_pWin, nY, nN + wstrlen(sWStr1), "%s", sViewStr2.c_str());
    }
    if (sWStr3.size() != 0) {
        sViewStr3 = wstrtostr(sWStr3);
        sViewStr3 = MLSUTIL::Replace(sViewStr3, sTab.c_str(), " "); // Tab -> Space

        setcol(_tColorEdit, _pWin);
        mvwprintw(_pWin, nY, nN + wstrlen(sWStr1) + wstrlen(sWStr2), "%s", sViewStr3.c_str());
    }
}

void NCurses_Editor::LineDraw(const wstring &sViewWStr,
                              int nY,
                              int nN,
                              int nTextNum,
                              int nNum) {
    setcol(_tColorEdit, _pWin);
    wmove(_pWin, nY, nN);

    int nBoxLine = 0;

    if (_bFullScreen)
        nBoxLine = 0;
    else
        nBoxLine = 1;

    whline(_pWin, ' ', width - (nBoxLine * 2));

    if (_bLineNumView == true) {
        setcol(_tColorEditInfo, _pWin);
        mvwprintw(_pWin, nY, nN + 1, "%*d", _nLineWidth, nTextNum + 1);
        setcol(_tColorEditInfo, _pWin);
        mvwhline(_pWin, nY, nN + _nLineWidth + 1, VLINE, 1);
        nN = 3 + _nLineWidth + nN;
    }

    string sTab;
    sTab.append(1, (char) TABCONVCHAR);

    if (_EditMode == EDIT) {
        string sViewStr = wstrtostr(sViewWStr);
        string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
        setcol(_tColorEdit, _pWin);
        mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
    } else if (_EditMode == SELECT || _EditMode == SHIFT_SELECT) {
        EditSelect tEditSelect = _EditSelect;
        SelectSort(&tEditSelect);

        int x1 = tEditSelect.x1;
        int x2 = tEditSelect.x2;

        wstring sLineWStr = _vText[nTextNum];
        wstring sWStr;
        StrLineToken tStrLineToken;
        tStrLineToken.SetWString(sLineWStr, _nCulumn);

        int nCurSize = 0;
        for (int n = 0; n < tStrLineToken.LineSize(); n++) {
            sWStr = tStrLineToken.GetLineStr(n);
            if (n == nNum) break;
            nCurSize = nCurSize + sWStr.size();
        }

        int nCurEndSize = nCurSize + sViewWStr.size();

        if (tEditSelect.x1 > nCurSize) x1 = tEditSelect.x1 - nCurSize;
        if (tEditSelect.x2 > nCurSize) x2 = tEditSelect.x2 - nCurSize;

        if (tEditSelect.y1 == nTextNum && tEditSelect.y2 == nTextNum) {
            if (tEditSelect.x1 >= nCurSize && tEditSelect.x2 <= nCurEndSize) {
                SelectionDraw(sViewWStr, nY, nN, x1, x2);
            } else if (tEditSelect.x1 >= nCurSize && tEditSelect.x2 > nCurEndSize) {
                SelectionDraw(sViewWStr, nY, nN, x1, sViewWStr.size());
            } else if (tEditSelect.x1 < nCurSize && tEditSelect.x2 > nCurSize) {
                SelectionDraw(sViewWStr, nY, nN, 0, x2);
            } else if (tEditSelect.x1 < nCurSize && tEditSelect.x2 > nCurEndSize) {
                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
                setrcol(_tColorEdit, _pWin); // 반전
                mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
            } else {
                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
                setcol(_tColorEdit, _pWin);
                mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
            }
        } else if (tEditSelect.y1 == nTextNum) {
            if (tEditSelect.x1 >= nCurSize) {
                SelectionDraw(sViewWStr, nY, nN, x1, sViewWStr.size());

                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
                wmove(_pWin, nY, nN + scrstrlen(sViewStr2));
                whline(_pWin, ' ', width - nN - scrstrlen(sViewStr2) - nBoxLine);
            } else if (tEditSelect.x1 < nCurSize) {
                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
                setrcol(_tColorEdit, _pWin); // 반전
                mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
            } else {
                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space

                mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
            }
        } else if (tEditSelect.y2 == nTextNum) {
            if (tEditSelect.x2 > nCurSize) {
                SelectionDraw(sViewWStr, nY, nN, 0, x2);
            } else if (tEditSelect.x2 > nCurEndSize) {
                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
                setrcol(_tColorEdit, _pWin); // 반전
                mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
            } else {
                string sViewStr = wstrtostr(sViewWStr);
                string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
                setcol(_tColorEdit, _pWin);
                mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
            }
        } else if (tEditSelect.y1 < nTextNum && tEditSelect.y2 > nTextNum) {
            string sViewStr = wstrtostr(sViewWStr);
            string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
            setrcol(_tColorEdit, _pWin); // 반전
            wmove(_pWin, nY, nN);
            whline(_pWin, ' ', width - nBoxLine);
            mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
        } else {
            string sViewStr = wstrtostr(sViewWStr);
            string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
            setcol(_tColorEdit, _pWin);
            mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
        }
    }
}

void NCurses_Editor::CurserDraw(int nY, int nN) {
    LineInfo tLineInfo;
    vector<LineInfo> vLineInfo;

    for (uint n = 0; n < _vViewString.size(); n++) {
        tLineInfo = _vViewString[n];
        if (tLineInfo.nTextLine == _nCurLine)
            vLineInfo.push_back(tLineInfo);
    }

    int cur = _nCurCulumn;
    int length = 0;
    int line1 = 0;

    for (uint n = 0; n < vLineInfo.size(); n++) {
        tLineInfo = vLineInfo[n];
        length = length + tLineInfo.sWString.size();
        if (length >= _nCurCulumn) {
            line1 = tLineInfo.nViewLine;
            break;
        }
        cur = cur - tLineInfo.sWString.size();
    }
    cur = wstrlen((tLineInfo.sWString.substr(0, cur)));

    touchwin(_pWin);
    wmove(_pWin, nY + line1, nN + cur);
    //setsyx(y+nY+line1, x+nN+cur);

    wnoutrefresh(_pWin);
    LOG_WRITE("CURSOR :: [%d] [%d] [%d] [%d]", line1, cur, _nCurCulumn, vLineInfo.size());
}

void NCurses_Editor::Draw() {
    _pWin = GetWin();

    if (!_pWin) {
        LOG_WRITE("NCurses_Editor pWin NULL");
        return;
    }

    uint nViewLine = 0;
    wstring sLineStr, sViewStr;
    string sSpaceTab;
    string sInfo1, sInfo2, sInfo3;
    char sLineFormat[50];

    int nViewWidth = 0, nKrStrSize = 0, nBoxLine = 0;

    LineInfo tLineInfo;

    if (_bFullScreen)
        nBoxLine = 0;
    else
        nBoxLine = 1;

    _nLine = height - nBoxLine - 2;
    _nLineWidth = itoa(_vText.size()).size();
    if (_bLineNumView == true) {
        _nCulumn = width - (_nLineWidth + 5 + (nBoxLine * 2));
    } else
        _nCulumn = width - (nBoxLine * 2);

    ScreenMemSave(_nLine, _nCulumn);

    werase(_pWin);

    BackDrawEtc();

    for (int t = 0; t < _nLine; t++) {
        if (_vViewString.size() > t) {
            tLineInfo = _vViewString[t];
            LineDraw(tLineInfo.sWString, nBoxLine + t, nBoxLine, tLineInfo.nTextLine, tLineInfo.nNextLineNum);
        } else {
            setcol(_tColorEdit, _pWin);
            wmove(_pWin, nBoxLine + t, nBoxLine);
            whline(_pWin, ' ', _nCulumn);

            if (_bLineNumView == true) {
                setcol(COLOR_BLACK, _tColorEdit.back, _pWin);
                mvwprintw(_pWin, nBoxLine + t, nBoxLine + 3 + _nLineWidth, "~");
            } else {
                setcol(COLOR_BLACK, _tColorEdit.back, _pWin);
                mvwprintw(_pWin, nBoxLine + t, nBoxLine, "~");
            }
        }
        nViewLine++;
    }

    setcol(_tColorEdit, _pWin);
    wmove(_pWin, height - (nBoxLine * 2), nBoxLine);
    whline(_pWin, ' ', width - (nBoxLine * 2));

    if (_bInsert)
        sInfo1 = _("[Ins]");
    else
        sInfo1 = _("[Ovr]");

    if (_EditMode == SELECT)
        sInfo1 = _("[Select]");

    if (_bDosMode) sInfo2 = "[DOS]";

    if (_eEncode == KO_EUCKR) sInfo3 = "[EUC-KR]";
    else if (_eEncode == KO_UTF8) sInfo3 = "[UTF-8]";
    else sInfo3 = "[US]";

    setcol(_tColorEditInfoA, _pWin);
    mvwprintw(_pWin, height - nBoxLine - 1, width - scrstrlen(sInfo1) - nBoxLine, (char *) sInfo1.c_str());
    if (_bDosMode) {
        setcol(_tColorEditInfoA, _pWin);
        mvwprintw(_pWin, height - nBoxLine - 1, width - scrstrlen(sInfo2) - scrstrlen(sInfo1) - nBoxLine,
                  (char *) sInfo2.c_str());
    }
    setcol(_tColorEditInfo, _pWin);
    mvwprintw(_pWin, height - nBoxLine - 1,
              width - scrstrlen(sInfo3) - scrstrlen(sInfo2) - scrstrlen(sInfo1) - nBoxLine, (char *) sInfo3.c_str());

    setcol(_tColorEditInfo, _pWin);
    snprintf(sLineFormat, sizeof(sLineFormat), "Line: %d Col: %d (%ld%%) ",
             _nCurLine + 1, _nCurCulumn, (_nCurLine + 1) * 100 / _vText.size());
    mvwprintw(_pWin, height - nBoxLine - 1,
              width - scrstrlen(sLineFormat) - scrstrlen(sInfo3) - scrstrlen(sInfo2) - scrstrlen(sInfo1) - nBoxLine,
              sLineFormat);

    if (_bReadOnly == true) {
        setcol(_tColorEditInfo, _pWin);
        mvwprintw(_pWin, height - nBoxLine - 1, 10, _("[read Only]"));
    } else {
        if (_nConvInfo != _vDoInfo.size()) {
            setcol(_tColorEditInfoA, _pWin);
            mvwprintw(_pWin, height - nBoxLine - 1, 10, _("[Change]"));
        }
    }

    //  커서 위치를 옮긴다.
    if (_bLineNumView == true)
        CurserDraw(nBoxLine, nBoxLine + 3 + _nLineWidth);
    else
        CurserDraw(nBoxLine, nBoxLine);
}

void NCurses_Editor::Replace() {
    string sFind = wstrtostr(_sFindStr);
    string sReplace = wstrtostr(_sReplaceStr);

    {
        Dialog_Replace tReplace(_("Replace"), sFind, sReplace);

        tReplace.Size(9, 50);
        tReplace.Move(MIDDLE, MIDDLE);

        tReplace.Do();
        tReplace.Close();

        if (!tReplace.GetOk()) return;

        sFind = tReplace.GetFind();
        sReplace = tReplace.GetReplace();
    }

    if (sFind.size() == 0) return;
    if (sReplace.size() == 0) return;

    _sFindStr = strtowstr(sFind);
    _sReplaceStr = strtowstr(sReplace);

    _nFindPosX = 0;
    _nFindPosY = _nCurLine;

    wstring str;
    wstring sLine, sLine1, sLine2, sLine3;
    wstring::size_type idx;
    bool bReplaceAll = false;

    for (;;) {
        for (int n = _nFindPosY; n < (int) _vText.size(); n++) {
            do {
                idx = _vText[n].find(_sFindStr, _nFindPosX);
                if (idx != wstring::npos) {
                    _nFindPosX = idx + _sFindStr.size();
                    _nFindPosY = n;
                    _EditMode = SHIFT_SELECT;
                    _EditSelect.x1 = idx;
                    _EditSelect.y1 = n;
                    _EditSelect.x2 = idx + _sFindStr.size();
                    _EditSelect.y2 = n;
                    _nCurCulumn = idx + _sFindStr.size();
                    _nCurLine = n;
                    _nCurCulumn_Max = _nCurCulumn;
                    _nFirstLine = _nCurLine - 10;

                    Draw();

                    bool bSkip = false;

                    if (!bReplaceAll) {
                        vector<string> q;
                        q.push_back(_("Replace"));
                        q.push_back(_("Replace All"));
                        q.push_back(_("Skip"));
                        q.push_back(_("Cancel"));

                        int nSelect = 0;
                        nSelect = SelectBox(_("Replace"), q, 0);

                        switch (nSelect) {
                            case 0:
                                break;
                            case 1:
                                bReplaceAll = true;
                                break;
                            case 2:
                                bSkip = true;
                                break;
                            case 3:
                                return;
                                break;
                            default:
                                return;
                        }
                    }

                    if (!bSkip) {
                        sLine = _vText[_nCurLine];

                        // Undo를 위한 데이터 저장
                        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, 1, sLine));

                        _nFindPosX = idx + _sReplaceStr.size();
                        _nFindPosY = n;
                        _EditMode = SHIFT_SELECT;
                        _EditSelect.x1 = idx;
                        _EditSelect.y1 = n;
                        _EditSelect.x2 = idx + _sReplaceStr.size();
                        _EditSelect.y2 = n;
                        _nCurCulumn = idx + _sReplaceStr.size();
                        _nCurLine = n;
                        _nCurCulumn_Max = _nCurCulumn;
                        _nFirstLine = _nCurLine - 10;


                        sLine2 = sLine.substr(0, idx) +
                                 _sReplaceStr +
                                 sLine.substr(idx + _sFindStr.size());

                        _vText[_nCurLine] = sLine2;
                    }
                }
            } while (idx != wstring::npos);
            _nFindPosX = 0;
        }

        _nFindPosY = 0;
        Draw();
        if (YNBox(_("Replace Next"),
                  _("End of document reached. Continue from the begining?"),
                  true) == false)
            break;
    }
}

