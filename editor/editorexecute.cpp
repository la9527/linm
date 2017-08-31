#include "editor.h"

using namespace MLSUTIL;
using namespace MLS;

void Editor::Key_Home() {
    string p = wstrtostr(_vText[_nCurLine]);
    int nNew = 0;
    int nOld = _nCurCulumn;
    for (int n = 0; n < (int) p.size(); n++) {
        if (p[n] != ' ' && p[n] != (char) TABCONVCHAR) {
            nNew = n;
            break;
        }
    }
    if (nOld == nNew)
        _nCurCulumn = 0;
    else
        _nCurCulumn = nNew;
    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    _nCurCulumn_Max = _nCurCulumn;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_End() {
    _nCurCulumn = _vText[_nCurLine].size();
    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    _nCurCulumn_Max = _nCurCulumn;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_Left() {
    if (_nCurCulumn > 0) {
        wstring sWStr;
        string sTab;
        int nCul = 0;

        sTab.append(1, (char) TABCONVCHAR);

        for (int n = 0; n < _nTabSize; n++) {
            _nCurCulumn = _nCurCulumn - 1;
            nCul++;
            sWStr = _vText[_nCurLine].substr(_nCurCulumn, 1);
            if (_nCurCulumn <= 0 || wstrtostr(sWStr) != sTab) break;
        }

        if (nCul != _nTabSize && nCul > 1)
            _nCurCulumn = _nCurCulumn + 1;
    } else if (_nCurLine > 0)
        _nCurCulumn = _vText[--_nCurLine].size();

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    _nCurCulumn_Max = _nCurCulumn;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_Right() {
    wstring sWStr = _vText[_nCurLine];

    if ((int) sWStr.size() > _nCurCulumn) {
        wstring sWTabStrChk;
        string sStr;
        sStr.append(1, (char) TABCONVCHAR);

        for (int n = 0; n < _nTabSize; n++) {
            _nCurCulumn = _nCurCulumn + 1;
            sWTabStrChk = _vText[_nCurLine].substr(_nCurCulumn, 1);
            if (_nCurCulumn >= (int) _vText[_nCurLine].size() || wstrtostr(sWTabStrChk) != sStr) break;
            if (n == 0) {
                sWTabStrChk = _vText[_nCurLine].substr(_nCurCulumn - 1, 1);
                if (wstrtostr(sWTabStrChk) != sStr) break;
            }
        }
    } else if ((int) sWStr.size() == _nCurCulumn) {
        _nCurLine++;
        _nCurCulumn = 0;
    }

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    _nCurCulumn_Max = _nCurCulumn;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_Up() {
    if (_nCurLine > 0) _nCurLine--;

    if (_nCurCulumn_Max < _nCurCulumn)
        _nCurCulumn_Max = _nCurCulumn;
    else
        _nCurCulumn = _nCurCulumn_Max;

    // 커서가 문자의 마지막을 넘어서면 마지막으로 가게 한다.
    int nStrlen = _vText[_nCurLine].size();
    if (nStrlen < _nCurCulumn)
        _nCurCulumn = nStrlen;
    else {
        // Tab space fix.
        Key_Right();
        Key_Left();
    }

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_Down() {
    if (_nCurLine < (int) _vText.size() - 1)
        _nCurLine++;

    if (_nCurCulumn_Max < _nCurCulumn)
        _nCurCulumn_Max = _nCurCulumn;
    else
        _nCurCulumn = _nCurCulumn_Max;

    // 커서가 문자의 마지막을 넘어서면 마지막으로 가게 한다.
    int nStrlen = _vText[_nCurLine].size();
    if (nStrlen < _nCurCulumn)
        _nCurCulumn = nStrlen;
    else {
        // Tab space fix.
        Key_Right();
        Key_Left();
    }

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_PgUp() {
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

    if (_nCurCulumn_Max < _nCurCulumn)
        _nCurCulumn_Max = _nCurCulumn;
    else
        _nCurCulumn = _nCurCulumn_Max;

    // 커서가 문자의 마지막을 넘어서면 마지막으로 가게 한다.
    int nStrlen = _vText[_nCurLine].size();
    if (nStrlen < _nCurCulumn) _nCurCulumn = nStrlen;

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_PgDn() {
    int nSize = _nLastLine - _nFirstLine;
    int nCur = _nCurLine - _nFirstLine;

    if ((int) _vText.size() < _nLine - 1) {
        _nCurLine = _vText.size() - 1;
    } else if (_nFirstLine > (int) _vText.size() - _nLine + 1) {
        _nCurLine = _vText.size() - 1;
    } else {
        _nCurLine = _nFirstLine + nSize + nCur;
        _nFirstLine = _nCurLine - nCur;

        if (_nFirstLine > (int) _vText.size() - _nLine + 1)
            _nFirstLine = _vText.size() - _nLine + 1;
        if ((int) _vText.size() <= _nCurLine)
            _nCurLine = _vText.size() - 1;
    }

    if (_nCurCulumn_Max < _nCurCulumn)
        _nCurCulumn_Max = _nCurCulumn;
    else
        _nCurCulumn = _nCurCulumn_Max;

    // 커서가 문자의 마지막을 넘어서면 마지막으로 가게 한다.
    int nStrlen = _vText[_nCurLine].size();
    if (nStrlen < _nCurCulumn) _nCurCulumn = nStrlen;

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
}

void Editor::Key_ShiftUp() {
    if (_EditMode != SHIFT_SELECT) {
        _EditSelect.x1 = _nCurCulumn;
        _EditSelect.y1 = _nCurLine;
    }
    Key_Up();
    _EditMode = SHIFT_SELECT;
}

void Editor::Key_ShiftDown() {
    if (_EditMode != SHIFT_SELECT) {
        _EditSelect.x1 = _nCurCulumn;
        _EditSelect.y1 = _nCurLine;
    }
    Key_Down();
    _EditMode = SHIFT_SELECT;
}

void Editor::Key_ShiftLeft() {
    if (_EditMode != SHIFT_SELECT) {
        _EditSelect.x1 = _nCurCulumn;
        _EditSelect.y1 = _nCurLine;
    }
    Key_Left();
    _EditMode = SHIFT_SELECT;
}

void Editor::Key_ShiftRight() {
    if (_EditMode != SHIFT_SELECT) {
        _EditSelect.x1 = _nCurCulumn;
        _EditSelect.y1 = _nCurLine;
    }
    Key_Right();
    _EditMode = SHIFT_SELECT;
}

void Editor::Key_ESC() {
    if (_EditMode != EDIT) _EditMode = EDIT;
}

void Editor::Select() {
    if (_EditMode == SELECT) _EditMode = EDIT;
    else _EditMode = SELECT;

    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    _EditSelect.x1 = _nCurCulumn;
    _EditSelect.y1 = _nCurLine;
}

void Editor::BlockSelect() {
    _EditMode = BLOCK;
    _EditSelect.x2 = _nCurCulumn;
    _EditSelect.y2 = _nCurLine;
    _EditSelect.x1 = _nCurCulumn;
    _EditSelect.y1 = _nCurLine;
}

void Editor::SelectAll() {
    _EditMode = SHIFT_SELECT;

    _EditSelect.x1 = 0;
    _EditSelect.y1 = 0;
    _EditSelect.x2 = _vText[_vText.size() - 1].size();
    _EditSelect.y2 = _vText.size() - 1;
}

void Editor::SelectSort(EditSelect *pEditSelect) {
    if (pEditSelect == NULL) return;

    int xTmp, yTmp;
    if (pEditSelect->y1 > pEditSelect->y2) {
        yTmp = pEditSelect->y1;
        pEditSelect->y1 = pEditSelect->y2;
        pEditSelect->y2 = yTmp;

        xTmp = pEditSelect->x1;
        pEditSelect->x1 = pEditSelect->x2;
        pEditSelect->x2 = xTmp;
    } else if (pEditSelect->y1 == pEditSelect->y2) {
        if (pEditSelect->x1 > pEditSelect->x2) {
            yTmp = pEditSelect->y1;
            pEditSelect->y1 = pEditSelect->y2;
            pEditSelect->y2 = yTmp;
            xTmp = pEditSelect->x1;
            pEditSelect->x1 = pEditSelect->x2;
            pEditSelect->x2 = xTmp;
        }
    }
}

void Editor::Copy() {
    if (_EditMode == EDIT) return;

    SelectSort(&_EditSelect);

    vector<wstring> vWStr;
    wstring WStr;

    if (_EditSelect.y2 >= (int) _vText.size()) {
        LOG_WRITE("Error :: _EditSelect.y2 > _vText.size ");
        return;
    }

    if (_EditSelect.y1 == _EditSelect.y2) {
        WStr = _vText[_EditSelect.y1];
        WStr = WStr.substr(_EditSelect.x1, _EditSelect.x2 - _EditSelect.x1);
        vWStr.push_back(WStr);
    } else {
        for (int y = _EditSelect.y1; y <= _EditSelect.y2; y++) {
            if (_EditSelect.y1 == y) {
                WStr = _vText[y];
                WStr = WStr.substr(_EditSelect.x1, WStr.size() - _EditSelect.x1);
            } else if (_EditSelect.y2 == y) {
                WStr = _vText[y];
                WStr = WStr.substr(0, _EditSelect.x2);
            } else {
                WStr = _vText[y];
            }
            vWStr.push_back(WStr);
        }
    }

    g_tEditorClip.Set(vWStr, Edit_ClipCopy);
    _EditMode = EDIT;
}

void Editor::Selected_Del() {
    if (_bReadOnly == true) return;

    SelectSort(&_EditSelect);

    wstring WStr, WStr1, WStr2, WStr3, WStr4;

    if (_EditSelect.y2 >= (int) _vText.size()) {
        LOG_WRITE("Error :: _EditSelect.y2 > _vText.size ");
        _EditMode = EDIT;
        return;
    }

    if (_EditSelect.y1 == _EditSelect.y2) {
        WStr = _vText[_EditSelect.y1];

        // Undo를 위한 데이터 저장
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, WStr));

        WStr1 = WStr.substr(0, _EditSelect.x1);
        WStr2 = WStr.substr(_EditSelect.x2, WStr1.size() - _EditSelect.x2);
        WStr = WStr1 + WStr2;
        _vText[_EditSelect.y1] = WStr;
    } else {
        vector<wstring> vSave;

        WStr1 = _vText[_EditSelect.y1];
        WStr2 = _vText[_EditSelect.y2];
        WStr3 = WStr1.substr(0, _EditSelect.x1);
        WStr4 = WStr2.substr(_EditSelect.x2, WStr2.size() - _EditSelect.x2);
        WStr = WStr3 + WStr4;

        for (int y = _EditSelect.y1; y <= _EditSelect.y2; ++y) {
            if (_EditSelect.y1 == y) {
                vSave.push_back(WStr1);
                _vText[_EditSelect.y1] = WStr;
            } else if (_EditSelect.y2 == y) {
                vSave.push_back(WStr2);
                _vText[_EditSelect.y1] = WStr;
                _vText.erase(_vText.begin() + _EditSelect.y1 + 1);
            } else {
                vSave.push_back(_vText[_EditSelect.y1 + 1]);
                _vText.erase(_vText.begin() + _EditSelect.y1 + 1);
            }
        }

        // Undo를 위한 데이터 저장
        _vDoInfo.push_back(new DoInfo(_EditSelect.y1, _EditSelect.x1, vSave));
    }

    _nCurLine = _EditSelect.y1;
    _nCurCulumn = _EditSelect.x1;
    _nCurCulumn_Max = _nCurCulumn;
    if (_nCurLine < _nFirstLine) _nFirstLine = _nCurLine - 10;
    _EditMode = EDIT;

    if (_vText.size() == 0) {
#ifdef __CYGWIN_C__
        _vText.push_back(" ");
#else
        _vText.push_back(L" ");
#endif
    }
}

void Editor::Cut() {
    if (_EditMode == EDIT) return;

    SelectSort(&_EditSelect);

    vector<wstring> vWStr;
    wstring WStr;

    if (_EditSelect.y2 >= (int) _vText.size()) {
        LOG_WRITE("Error :: _EditSelect.y2 > _vText.size ");
        _EditMode = EDIT;
        return;
    }

    if (_EditSelect.y1 == _EditSelect.y2) {
        WStr = _vText[_EditSelect.y1];
        WStr = WStr.substr(_EditSelect.x1, _EditSelect.x2 - _EditSelect.x1);
        vWStr.push_back(WStr);
    } else {
        for (int y = _EditSelect.y1; y <= _EditSelect.y2; y++) {
            if (_EditSelect.y1 == y) {
                WStr = _vText[y];
                WStr = WStr.substr(_EditSelect.x1, WStr.size() - _EditSelect.x1);
            } else if (_EditSelect.y2 == y) {
                WStr = _vText[y];
                WStr = WStr.substr(0, _EditSelect.x2);
            } else {
                WStr = _vText[y];
            }
            vWStr.push_back(WStr);
        }
    }
    g_tEditorClip.Set(vWStr, Edit_ClipCut);

    Selected_Del();
    _nCurCulumn_Max = _nCurCulumn;
    _EditMode = EDIT;
}

void Editor::Paste() {
    if (_bReadOnly == true) return;

    vector<wstring> vClip;
    wstring WStr, WStr1, WStr2;
    wstring WClip, WClip2;

    vClip = g_tEditorClip.Get();
    LOG_WRITE("ClipSize :: [%d]", vClip.size());

    if (_EditMode != EDIT) Selected_Del();

    WStr = _vText[_nCurLine];
    WStr1 = WStr.substr(0, _nCurCulumn);
    WStr2 = WStr.substr(_nCurCulumn, WStr.size() - _nCurCulumn);

    if (vClip.size() == 1) {
        WClip = vClip[0];
        WClip2 = WStr1 + WClip + WStr2;

        // Undo를 위한 데이터 저장
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, _vText[_nCurLine]));

        _vText[_nCurLine] = WClip2;
        _nCurCulumn = _nCurCulumn + WClip.size();
    } else {
        // Undo를 위한 데이터 저장
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, vClip.size(), _vText[_nCurLine]));

        for (int y = 0; y < (int) vClip.size(); y++) {
            if (y == 0) {
                WClip = vClip[y];
                WClip = WStr1 + WClip;
                _vText[_nCurLine] = WClip;
            } else if (y == (int) vClip.size() - 1) {
                WClip = vClip[y];
                WClip2 = WClip + WStr2;
                _vText.insert(_vText.begin() + _nCurLine + y, WClip2);
                _nCurCulumn = WClip2.size();
                _nCurLine = _nCurLine + (vClip.size() - 1);
            } else {
                WClip = vClip[y];
                _vText.insert(_vText.begin() + _nCurLine + y, WClip);
            }
        }
    }

    if (_nCurLine > _nLastLine) {
        ScreenMemSave(_nLine, _nCulumn);
        //_nFirstLine = _nCurLine - 10;
    }
    _nCurCulumn_Max = _nCurCulumn;
    _EditMode = EDIT;
}


void Editor::Undo() {
    DoInfo *pDoInfo = NULL;

    if (_vDoInfo.size() == 0) return;

    pDoInfo = _vDoInfo[_vDoInfo.size() - 1];

    if (pDoInfo == NULL) {
        LOG_WRITE("Undo pDoInfo NULL !!!");
        return;
    }

    int nLine = pDoInfo->nLine;

    if (pDoInfo->nDelSize == -1) // 덮어씌움. (Tab)
    {
        for (int n = 0; n < (int) pDoInfo->vData.size(); n++) {
            _vText[nLine + n] = pDoInfo->vData[n];
        }
    } else if (pDoInfo->nDelSize == 0) // 지워진 데이터 (삽입)
    {
        for (int n = 0; n < (int) pDoInfo->vData.size(); n++) {
            if (n == 0)
                _vText[nLine] = pDoInfo->vData[n];
            else
                _vText.insert(_vText.begin() + nLine + n, pDoInfo->vData[n]);
        }
        if (pDoInfo->vData.size() > 0)
            _nCurLine = nLine + (pDoInfo->vData.size() - 1);

        if (_nCurLine < _nFirstLine) {
            _nFirstLine = _nCurLine;
        }
    } else // 입력된 데이터 (삭제)
    {
        int nDelSize = pDoInfo->nDelSize;
        wstring sWStr;

        if (pDoInfo->vData.size() == 1)
            sWStr = pDoInfo->vData[0];

        for (int y = nLine; y <= nLine + nDelSize; ++y) {
            if (nLine == y || nLine + nDelSize == y) {
                _vText[nLine] = sWStr;
            } else {
                _vText.erase(_vText.begin() + nLine + 1);
            }
        }
        _nCurLine = pDoInfo->nLine;

        if (_nCurLine < _nFirstLine) {
            _nFirstLine = _nCurLine;
        }
    }
    _nCurCulumn = pDoInfo->nCulumn;
    delete pDoInfo;
    _vDoInfo.pop_back();
    _nCurCulumn_Max = _nCurCulumn;
}


void Editor::FileNew() {
    if (_bReadOnly == true) {
        MsgBox(_("Error"), _("read only file loaded."));
        return;
    }

    // 변동이 있으면 현재 파일을 저장할지 물어본다.
    if (_nConvInfo != (int) _vDoInfo.size()) {
        if (YNBox(_("File New"), _("New file. Do you want to save ?"), true) == true) {
            FileSave();
        }
    }

    string sFilename;
    if (InputBox("New File...", sFilename) == ERROR) return;
    if (sFilename.size() == 0) return;

    ifstream in(sFilename.c_str());
    if (in) {
        MsgBox(_("Error"), _("current file using."));
        return;
    }

    New(sFilename);
    SetViewTitle("[" + sFilename + "]");
}

bool Editor::FileSave() {
    if (_bReadOnly == true) {
        MsgBox(_("Error"), _("This file read only."));
        return false;
    }
    if (Save(_sFile, AUTO, _bDosMode, _bBackup) == true) {
        _nConvInfo = _vDoInfo.size(); // 저장 시점 저장. 저장 여부판단.
        return true;
    }
    return false;
}

bool Editor::FileSaveAs() {
    string sFilename;
    string sPath;

    string::size_type p = _sFile.rfind("/");

    if (p != string::npos) {
        sPath = _sFile.substr(0, p + 1);
        sFilename = _sFile.substr(p + 1);
    } else {
        sFilename = _sFile;
    }

    if (InputBox("Save as", sFilename) == ERROR) return false;
    if (sFilename.size() == 0) return false;

    vector<string> vStr;
    vStr.push_back(strmid(16, _("Current Encode")));
    vStr.push_back(strmid(15, _("EUC-KR")));
    vStr.push_back(strmid(15, _("UTF-8")));
    if (_bDosMode)
        vStr.push_back(strmid(15, _("UNIX Mode")));
    else
        vStr.push_back(strmid(15, _("DOS Mode")));
    vStr.push_back(strmid(15, _("Cancel")));

    int nCho = SelectBox(_("Encoding convert."), vStr, 0);

    switch (nCho) {
        case -1:
            return false;
            break;
        case 0:
            if (Save(sPath + sFilename, AUTO, _bDosMode) == true)
                _nConvInfo = _vDoInfo.size(); // 저장 시점 저장
            break;
        case 1:
            if (Save(sPath + sFilename, KO_EUCKR, _bDosMode) == true)
                _nConvInfo = _vDoInfo.size(); // 저장 시점 저장
            break;
        case 2:
            if (Save(sPath + sFilename, KO_UTF8, _bDosMode) == true)
                _nConvInfo = _vDoInfo.size(); // 저장 시점 저장
            break;
        case 3:
            if (Save(sPath + sFilename, AUTO, !_bDosMode) == true)
                _nConvInfo = _vDoInfo.size(); // 저장 시점 저장
        case 4:
            return false;
    }
    SetViewTitle("[" + sFilename + "]");
    return true;
}

void Editor::Find() {
    string sFind = wstrtostr(_sFindStr);
    if (InputBox(_("Find Text."), sFind) == ERROR) return;
    if (sFind.size() == 0) return;
    _sFindStr = strtowstr(sFind);
    _nFindPosX = 0;
    _nFindPosY = 0;
    FindNext();
}

void Editor::FindNext() {
    if (_sFindStr.size() == 0) return;

    if (_EditMode == EDIT) {
        _nFindPosY = _nCurLine;
        _nFindPosX = 0;
    }

    wstring::size_type idx;

    for (;;) {
        for (int n = _nFindPosY; n < (int) _vText.size(); n++) {
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
                return;
            }
            _nFindPosX = 0;
        }
        _nFindPosY = 0;
        if (YNBox(_("FindNext"),
                  _("End of document reached. Continue from the begining?"),
                  true) == false)
            break;
    }
}

void Editor::FindPrevious() {
    if (_sFindStr.size() == 0) return;

    if (_EditMode == EDIT) {
        _nFindPosY = _nCurLine;
        _nFindPosX = 0;
    }

    wstring::size_type idx;
    wstring wstr;

    _nFindPosX = _nFindPosX - _sFindStr.size();

    for (;;) {
        for (int n = _nFindPosY; n >= 0; --n) {
            wstr = _vText[n].substr(0, _nFindPosX);
            idx = wstr.rfind(_sFindStr);
            if (idx != wstring::npos) {
                _nFindPosX = idx;
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
                return;
            }
            if (n > 0)
                _nFindPosX = _vText[n - 1].size();
        }
        _nFindPosY = _vText.size() - 1;
        if (YNBox(_("FindPrevious"),
                  _("First of document reached. Continue from the begining?"),
                  true) == false)
            break;
    }
}

