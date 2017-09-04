/***************************************************************************
 *   Copyright (C) 2005 by Byoungyoung, La                                 *
 *   la9527@yahoo.co.kr                                                    *
 ***************************************************************************/

#include "define.h"
#include "strutil.h"
#include "editor.h"
#include "strlinetoken.h"

using namespace MLSUTIL;
using namespace MLS;

Editor::Editor() {
    _nTabSize = 8;
    _sFile = "";
    _bLineNumView = false;
    _bInsert = true;
    _EditMode = EDIT;
    _nCurCulumn_Max = 0;
    _bReadOnly = false;
    _bDosMode = false;
    _bIndentMode = true;
    _bKeyCfgLoad = false;
}

Editor::~Editor() {
    Destroy();
}

void Editor::Destroy() {/**/
    for (int n = 0; n < (int) _vDoInfo.size(); n++) {
        DoInfo *pDoInfo = _vDoInfo[n];
        delete pDoInfo;
    }
    _vDoInfo.clear();
    _bReadOnly = false;
    _bDosMode = false;
}

void Editor::SetEditor(int nTabSize, bool bBackup, bool bLineNumView) {
    _nTabSize = nTabSize;
    _bBackup = bBackup;
    _bLineNumView = bLineNumView;
}

void Editor::New(const string &sFile) {
    _sFile = sFile;
    _vText.clear();
    _eEncode = e_nCurLang;
    _nFirstLine = 0;
    _nCurLine = 0;
    _nCurCulumn = 0;
    _EditMode = EDIT;
    _nCurCulumn_Max = 0;
    _bInsert = true;
    _nConvInfo = 0;

    // find initialize
#ifdef __CYGWIN_C__
    _sFindStr = "";
#else
    _sFindStr = L"";
#endif

    _nFindPosX = 0;
    _nFindPosY = 0;

    for (int n = 0; n < (int) _vDoInfo.size(); n++) {
        DoInfo *pDoInfo = _vDoInfo[n];
        delete pDoInfo;
    }
    _vDoInfo.clear();
#ifdef __CYGWIN_C__
    _vText.push_back(LineSyntex(" "));
#else
    _vText.push_back(LineSyntex(L" "));
#endif

}

bool Editor::Load(File *pFile, bool bReadOnly) {
    _tFile = *pFile;
    return Load(pFile->sFullName, bReadOnly);
}

string Editor::TabToEdit(const string &sStr, char cTabChar, const int nTabSize) {
    int nTab;
    string sReplace;

    for (uint n = 0; n < sStr.size(); n++) {
        if (sStr[n] != '\t') {
            sReplace.append(1, sStr[n]);
            continue;
        }

        nTab = nTabSize - (sReplace.size() % nTabSize);
        sReplace.append(nTab, cTabChar);
    }
    return sReplace;
}

string Editor::EditToTab(const string &sStr, char cTabChar, const int nTabSize) {
    string sReplace;
    int nTabIns = 0;
    bool bTabOnly = false;

    for (uint n = 0; n < sStr.size(); n++) {
        if (sStr[n] != cTabChar) {
            if (nTabIns > 0)
                sReplace.append(1, '\t');

            sReplace.append(1, sStr[n]);
            nTabIns = 0;
            bTabOnly = false;
            continue;
        }

        bTabOnly = true;
        if (nTabIns > nTabSize - 1) {
            sReplace.append(1, '\t');
            nTabIns = 0;
        }
        nTabIns++;
    }
    if (bTabOnly) sReplace.append(1, '\t');
    return sReplace;
}

bool Editor::Load(const string &sFile, bool bReadOnly) {
    ifstream in(sFile.c_str());
    if (!in) return false;
    if (!in.good()) return false;

    string line, var, val;
    string sConvert, sConvert2;
    wstring wConv;
    int nUS = 0, nKO_EUCKR = 0, nKO_UTF8 = 0;

    // similer the new file open.
    New(sFile);
    ENCODING eEncode;

    _bReadOnly = bReadOnly;

    do {
        getline(in, line);

        if (line == "") {
            if (in.eof()) break;
            continue;
        }

        eEncode = AUTO;
        sConvert = isKorCode(line, &eEncode);
        if (eEncode == US) nUS++;
        else if (eEncode == KO_EUCKR) nKO_EUCKR++;
        else if (eEncode == KO_UTF8) nKO_UTF8++;
    } while (!in.eof());
    in.close();

    LOG_WRITE("US [%d] EUCKR [%d] UTF8 [%d]", nUS, nKO_EUCKR, nKO_UTF8);
    if (nKO_EUCKR > nKO_UTF8)
        _eEncode = KO_EUCKR;
    else if (nKO_EUCKR < nKO_UTF8)
        _eEncode = KO_UTF8;
    else
        _eEncode = US;

    _vText.clear();

    ifstream in2(sFile.c_str());
    if (!in2) return false;
    if (!in2.good()) return false;
    line = "";
    bool bNotSave = false;
    bool bDosMode = false;
    do {
        getline(in2, line);
        if (line == "") {
            if (in2.eof()) break;
#ifdef __CYGWIN_C__
            _vText.push_back(LineSyntex(""));
#else
            _vText.push_back(LineSyntex(L""));
#endif
            continue;
        }

        if (e_nCurLang == KO_UTF8) {
            if (_eEncode == KO_EUCKR) {
                sConvert = CodeConvert(line, "UTF-8", "EUC-KR"); // // change to utf-8
                if (sConvert == "") sConvert = line;
            } else sConvert = line;
        } else if (e_nCurLang == KO_EUCKR) {
            if (_eEncode == KO_UTF8) {
                sConvert = CodeConvert(line, "EUC-KR", "UTF-8"); // change to euc-kr
                if (sConvert == "") sConvert = line;
            } else sConvert = line;
        } else {
            // The locale is US(english), same the locale utf-8. (CTYPE : en_US.utf-8)
            if (_eEncode == KO_EUCKR) {
                sConvert = CodeConvert(line, "UTF-8", "EUC-KR"); // change to utf-8
                if (sConvert == "") sConvert = line;
            } else sConvert = line;
        }

        sConvert = TabToEdit(sConvert, (char) TABCONVCHAR, _nTabSize);
        sConvert2 = Replace(sConvert, "\r", "");

        if (sConvert2.size() != sConvert.size()) bDosMode = true;
        wConv = strtowstr(sConvert2);
        if (sConvert2.size() != 0 && wConv.size() == 0) bNotSave = true;
        _vText.push_back(LineSyntex(wConv));
    } while (!in2.eof());
    in2.close();

    if (_vText.size() == 0) {
#ifdef __CYGWIN_C__
        _vText.push_back(LineSyntex(""));
#else
        _vText.push_back(LineSyntex(L""));
#endif
    }

    PostLoad();

    if (bDosMode == true) _bDosMode = true;

    if (bNotSave == true) {
        MsgBox(_("Error"), _("This document locale error. current file read only."));
        _bReadOnly = true;
    }
    return true;
}

bool Editor::Save(File *pFile, ENCODING Encode, bool bDosMode, bool bBackup) {
    _tFile = *pFile;
    return Save(pFile->sFullName, Encode, bDosMode, bBackup);
}

bool Editor::Save(const string &sFile, ENCODING Encode, bool bDosMode, bool bBackup) {
    string sTmpFile;
    _sFile = sFile;
    sTmpFile = sFile + ".tmp";
    ofstream out(sTmpFile.c_str());
    if (!out) {
        String sMsg;
        sMsg.Append("%s : %s", sTmpFile.c_str(), strerror(errno));
        MsgBox(_("Error"), sMsg.c_str());
        return false;
    }

    string sSrc, sConvert;
    ENCODING eEncode;

    if (Encode == AUTO)
        eEncode = _eEncode;
    else
        eEncode = Encode;

    for (uint n = 0; n < _vText.size(); n++) {
        sSrc = wstrtostr(_vText[n].wLine);
        sSrc = EditToTab(sSrc, (char) TABCONVCHAR, _nTabSize);

        if (bDosMode) sSrc = sSrc + "\r";

        if (sSrc.size() == 0 && _vText[n].wLine.size() > 0) {
            MsgBox(_("Error"), _("Code convert failure."));
            return false;
        }

        if (eEncode == KO_EUCKR) {
            sConvert = "";
            if (e_nCurLang == KO_UTF8)
                sConvert = CodeConvert(sSrc, "EUC-KR", "UTF-8"); // change to euckr
            if (e_nCurLang == KO_EUCKR)
                sConvert = sSrc;
            if (e_nCurLang == US)
                sConvert = CodeConvert(sSrc, "EUC-KR", "UTF-8");
            if (sConvert == "") out << sSrc << endl;
            else out << sConvert << endl;
        } else if (eEncode == KO_UTF8) {
            sConvert = "";
            if (e_nCurLang == KO_EUCKR)
                sConvert = CodeConvert(sSrc, "UTF-8", "EUC-KR"); // change from euc-kr to utf-8.
            if (e_nCurLang == KO_UTF8)
                sConvert = sSrc;
            if (e_nCurLang == US)
                sConvert = sSrc;
            if (sConvert == "") out << sSrc << endl;
            else out << sConvert << endl;
        } else {
            out << sSrc << endl;
        }
    }
    out.close();

    if (bDosMode) _bDosMode = true;
    else _bDosMode = false;

    struct stat t_stat;
    if (stat(sFile.c_str(), &t_stat) == -1) {
        t_stat.st_mode = 0644;
    }

    if (bBackup == true) {
        string sBackFile = sFile + ".bak";
        if (rename(sFile.c_str(), sBackFile.c_str()) == -1) {
            if (errno != 2) // if not exist the file, run the next line script. (it's only exist the new file)
            {
                MsgBox(_("Error"), strerror(errno));
                return false;
            }
        }
    }

    if (rename(sTmpFile.c_str(), sFile.c_str()) == -1) {
        MsgBox(_("Error"), strerror(errno));
        return false;
    }

    if (chmod(sFile.c_str(), t_stat.st_mode) == -1) {
        MsgBox(_("Error"), strerror(errno));
    }
    _eEncode = eEncode;
    return true;
}

void Editor::ScreenMemSave(int nLine, int nCulumn) {
    wstring sViewWStr, sLineWStr;
    StrLineToken tStrLineToken;

    if (_nCurLine > 0) {
        if (_nCurLine >= (int) _vText.size()) _nCurLine = _vText.size() - 1;
        if (_nCurLine <= _nFirstLine) _nFirstLine = _nFirstLine - 1;
        if (_nFirstLine <= 0) _nFirstLine = 0;
        if (_nLastLine - _nFirstLine >= 10 && _nLastLine - _nCurLine <= 0) {
            if ((int) _vViewString.size() >= nLine)
                if (_nFirstLine <= (int) _vText.size())
                    _nFirstLine = _nFirstLine + 1;

            if ((int) _vText.size() <= _nLine - 5)
                _nFirstLine = 0;
        }
    }

    LOG_WRITE("_FirstLine [%d] [%d]", _nCurLine, _nFirstLine);

    for (;;) {
        int nViewLine = _nFirstLine;

        if (nViewLine < 0) return;

        _vViewString.clear();

        LineInfo tLineInfo;
        bool bNext = false;

        vector<SyntexData> vSyntexData;
        SyntexData tNextSyntexAdd;
        int nLastSyntexVectorPos = 0;
        bool bBefSyntexDataExist = false;

        for (int t = 0; t < nLine; t++) {
            if (!tStrLineToken.NextChk()) {
                if (nViewLine >= (int) _vText.size()) break;
                sLineWStr = _vText[nViewLine].wLine;
                vSyntexData = _vText[nViewLine].vSyntexData;
                LOG_WRITE("ViewLine Syntex [%d] [%d]", nViewLine, (int) _vText[nViewLine].vSyntexData.size());
                tStrLineToken.SetWString(sLineWStr, nCulumn);
                nViewLine++;
                nLastSyntexVectorPos = 0;
                bBefSyntexDataExist = false;
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
            tLineInfo.vSyntexData.clear();

            // Syntex Token.
            int nCurNum = tLineInfo.nNextLineNum * nCulumn;
            if (nLastSyntexVectorPos < vSyntexData.size()) {
                for (int n = nLastSyntexVectorPos; n < vSyntexData.size(); n++) {
                    SyntexData tSyntexData = vSyntexData[n];
                    if (nCurNum == 0 || nCurNum >= tSyntexData.nStart) {
                        if (bBefSyntexDataExist) {
                            bBefSyntexDataExist = false;
                            nLastSyntexVectorPos = 0;
                            LOG_WRITE("tNextSyntexAdd [%d] [%s]", tNextSyntexAdd.nStart,
                                      (const char *) tNextSyntexAdd.strString.c_str());
                            tLineInfo.vSyntexData.push_back(tNextSyntexAdd);
                            continue;
                        }

                        int nNextCulumn = nCurNum + nCulumn;
                        int nLastWordPosition = tSyntexData.nStart + scrstrlen(tSyntexData.strString);
                        LOG_WRITE("nLastSyntexVectorPos [%d] nCurNum [%d] [%d] [%d]", nLastSyntexVectorPos, nCurNum,
                                  nNextCulumn, nLastWordPosition);

                        if (nLastWordPosition <= nNextCulumn) {
                            SyntexData tNewSyntexData = tSyntexData;
                            tNewSyntexData.nStart = tNewSyntexData.nStart - nCurNum;
                            LOG_WRITE("tSyntexData [%d] [%s]", tNewSyntexData.nStart,
                                      (const char *) tNewSyntexData.strString.c_str());
                            tLineInfo.vSyntexData.push_back(tSyntexData);
                            bBefSyntexDataExist = false;
                        } else if (nLastWordPosition > nNextCulumn) {
                            SyntexData tNewSyntexData = tSyntexData;
                            int nClopWidth = nNextCulumn - tSyntexData.nStart;
                            int nStrLength = scrstrlen(tSyntexData.strString);

                            tNewSyntexData.nStart = tSyntexData.nStart - nCurNum;
                            tNewSyntexData.strString = scrstrncpy(tNewSyntexData.strString, 0, nClopWidth);
                            LOG_WRITE("tSyntexData2 [%d] [%s]", tNewSyntexData.nStart,
                                      (const char *) tNewSyntexData.strString.c_str());
                            tLineInfo.vSyntexData.push_back(tSyntexData);

                            tNextSyntexAdd = tSyntexData;
                            tNextSyntexAdd.nStart = nNextCulumn;
                            tNextSyntexAdd.strString = scrstrncpy(tSyntexData.strString, nClopWidth,
                                                                  nStrLength - nClopWidth);

                            nLastSyntexVectorPos = n;
                            bBefSyntexDataExist = true;
                            break;
                        }
                    }

                    if (n == vSyntexData.size() - 1) {
                        nLastSyntexVectorPos = 0;
                        bBefSyntexDataExist = false;
                    }
                }
            } else {
                nLastSyntexVectorPos = 0;
                bBefSyntexDataExist = false;
            }
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

void Editor::InputData(const string &sKrStr) {
    if (_bReadOnly == true) return;

    wstring sLine, sLine2, sChar;

    if (_EditMode != EDIT) {
        Selected_Del();
        _EditMode = EDIT;
    }

    if ((int) _vText.size() > _nCurLine) {
        sLine = _vText[_nCurLine].wLine;
        // data save for undo
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, sLine));

        sChar = strtowstr(sKrStr);

        if (_bInsert)
            sLine.insert(_nCurCulumn, sChar);
        else
            sLine.replace(_nCurCulumn, sChar.size(), sChar);

        _vText[_nCurLine].wLine = sLine;
        _nCurCulumn = _nCurCulumn + sChar.size();
        PostUpdateLines(_nCurLine);
    }
    _nCurCulumn_Max = _nCurCulumn;
}

void Editor::Key_Del() {
    if (_bReadOnly == true) return;

    wstring sLine, sLine2;
    string sTmp;
    int nTabSize = 0;

    if (_EditMode != EDIT) {
        Selected_Del();
        _EditMode = EDIT;
        return;
    }

    sLine = _vText[_nCurLine].wLine;
    int nStrSize = sLine.size();

    if (_nCurCulumn < nStrSize) {
        // save data for undo function.
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, sLine));

        sLine2 = sLine.substr(_nCurCulumn, _nTabSize);
        sTmp = wstrtostr(sLine2);

        for (int n = 0; n < (int) sTmp.size(); n++)
            if (sTmp[n] == (char) TABCONVCHAR)
                nTabSize++;

        if (nTabSize > 0 && nStrSize - (_nCurCulumn + nTabSize) >= 0) {
            sLine2 = sLine.substr(0, _nCurCulumn);
            sLine2 = sLine2 + sLine.substr(_nCurCulumn + nTabSize, nStrSize - (_nCurCulumn + nTabSize));
        } else {
            sLine2 = sLine.substr(0, _nCurCulumn);
            sLine2 = sLine2 + sLine.substr(_nCurCulumn + 1, nStrSize - (_nCurCulumn + 1));
        }

        _vText[_nCurLine].wLine = sLine2;
        PostUpdateLines(_nCurLine);
    } else {
        if ((int) _vText.size() > _nCurLine + 1) {
            sLine2 = _vText[_nCurLine + 1].wLine;
            // data save for undo
            _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, sLine, sLine2));

            sLine2 = sLine + sLine2;
            _vText[_nCurLine].wLine = sLine2;
            _vText.erase(_vText.begin() + _nCurLine + 1);
            PostUpdateLines(_nCurLine);
        }
    }
    _nCurCulumn_Max = _nCurCulumn;

    if (_vText.size() == 0) {
#ifdef __CYGWIN_C__
        _vText.push_back(LineSyntex(""));
#else
        _vText.push_back(LineSyntex(L""));
#endif
    }
}

void Editor::Key_BS() {
    if (_bReadOnly == true) return;

    wstring sLine, sLine2;
    int nStrSize;

    if (_EditMode != EDIT) {
        Selected_Del();
        _EditMode = EDIT;
        _nCurLine = _EditSelect.y1;
        _nCurCulumn = _EditSelect.x1;
        _nCurCulumn_Max = _nCurCulumn;
        return;
    }

    if (_nCurLine == 0 && _nCurCulumn == 0) return;

    if ((int) _vText.size() > _nCurLine) {
        sLine = _vText[_nCurLine].wLine;

        if (_nCurCulumn == 0 && _vText.size() > 0 && _nCurLine > 0) {
            sLine2 = _vText[_nCurLine - 1].wLine;
            // data save for undo function
            _vDoInfo.push_back(new DoInfo(_nCurLine - 1, _nCurCulumn, sLine2, sLine));

            nStrSize = sLine2.size();
            sLine2 = sLine2 + sLine;

            _vText[_nCurLine - 1].wLine = sLine2;
            _vText.erase(_vText.begin() + _nCurLine);
            PostUpdateLines(_nCurLine);

            Key_Up();
            _nCurCulumn = nStrSize;
        } else {
            nStrSize = _vText[_nCurLine].wLine.size();

            int nTabSize = 0;

            if (_nCurCulumn <= nStrSize) {
                // data save for undo function.
                _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, sLine));

                wstring sTabCheck;
                string sTmp;

                if (_nCurCulumn - _nTabSize >= 0)
                    sTabCheck = _vText[_nCurLine].wLine.substr(_nCurCulumn - _nTabSize, _nTabSize);
                else
                    sTabCheck = _vText[_nCurLine].wLine.substr(0, _nCurCulumn);

                sTmp = wstrtostr(sTabCheck);
                for (int n = sTmp.size() - 1; n >= 0; n--) {
                    if (sTmp[n] == (char) TABCONVCHAR)
                        nTabSize++;
                    else
                        break;
                }

                if (nTabSize > 0) {
                    sLine2 = _vText[_nCurLine].wLine.substr(0, _nCurCulumn - nTabSize);
                    sLine2 = sLine2 + _vText[_nCurLine].wLine.substr(_nCurCulumn, nStrSize - _nCurCulumn);
                    _nCurCulumn = _nCurCulumn - nTabSize;
                } else {
                    sLine2 = _vText[_nCurLine].wLine.substr(0, _nCurCulumn - 1);
                    sLine2 = sLine2 + _vText[_nCurLine].wLine.substr(_nCurCulumn, nStrSize - _nCurCulumn);
                    _nCurCulumn = _nCurCulumn - 1;
                }
            }

            _vText[_nCurLine] = sLine2;
            PostUpdateLines(_nCurLine);

            _EditSelect.x2 = _nCurCulumn;
            _EditSelect.y2 = _nCurLine;
            _nCurCulumn_Max = _nCurCulumn;
            if (_EditMode == SHIFT_SELECT) _EditMode = EDIT;
        }
    }
    _nCurCulumn_Max = _nCurCulumn;
}

void Editor::Key_Enter() {
    if (_bReadOnly == true) return;

    wstring sLine, sLine2, sLine3;

    int nStrlen;

    if (_EditMode != EDIT) {
        Selected_Del();
        _EditMode = EDIT;
    }

    string p = wstrtostr(_vText[_nCurLine].wLine);
    string p1;

    if (_bIndentMode) {
        for (int n = 0; n < (int) p.size(); n++) {
            if (p[n] != ' ' && p[n] != (char) TABCONVCHAR) {
                p1 = p.substr(0, n);
                break;
            }
        }
    }

    if ((int) _vText.size() > _nCurLine) {
        sLine = _vText[_nCurLine].wLine;
        // data save for undo
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, 2, sLine));

        nStrlen = sLine.size();

        sLine2 = sLine2 + sLine.substr(0, _nCurCulumn);
        sLine3 = strtowstr(p1) + sLine3 + sLine.substr(_nCurCulumn, nStrlen - _nCurCulumn);

        _vText[_nCurLine].wLine = sLine2;
        _vText.insert(_vText.begin() + _nCurLine + 1, sLine3);
        PostUpdateLines(_nCurLine);
    } else {
        // data save for undo
        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, 2, sLine));

        _vText.push_back(LineSyntex(strtowstr(p1)));
        ScreenMemSave(_nLine, _nCulumn);
    }
    _nCurCulumn = p1.size();
    _nCurCulumn_Max = _nCurCulumn;
    Key_Down();
}


void Editor::Key_Tab() {
    if (_bReadOnly == true) return;

    if (_EditMode != EDIT) {
        //Selected_Del();
        //_EditMode = EDIT;
        wstring WStr;
        vector<wstring> vSave;
        string sTab;

        sTab.append(_nTabSize, (char) TABCONVCHAR);

        SelectSort(&_EditSelect);

        for (int y = _EditSelect.y1; y <= _EditSelect.y2; y++) {
            WStr = _vText[y].wLine;
            vSave.push_back(WStr);
        }

        // data save for undo
        _vDoInfo.push_back(new DoInfo(_EditSelect.y1, 0, vSave, -1));

        for (int y = _EditSelect.y1; y <= _EditSelect.y2; y++) {
            WStr = _vText[y].wLine;
            WStr = strtowstr(sTab) + WStr;
            _vText[y].wLine = WStr;
        }

        PostUpdateLines(_EditSelect.y1, _EditSelect.y2 - _EditSelect.y1 + 1);
        ScreenMemSave(_nLine, _nCulumn);
        return;
    }

    int nTabSize = 4 - (_nCurCulumn % 4);

    for (int n = 0; n < nTabSize; n++) {
        string sTab;
        sTab.append(1, (char) TABCONVCHAR);
        InputData(sTab);
        ScreenMemSave(_nLine, _nCulumn);
    }
}

void Editor::Key_Untab() {
    if (_bReadOnly == true) return;

    if (_EditMode != EDIT) {
        wstring WStr;
        vector<wstring> vSave;

        SelectSort(&_EditSelect);

        for (int y = _EditSelect.y1; y <= _EditSelect.y2; y++) {
            WStr = _vText[y].wLine;
            vSave.push_back(WStr);
        }

        // data save for undo
        _vDoInfo.push_back(new DoInfo(_EditSelect.y1, 0, vSave, -1));

        for (int y = _EditSelect.y1; y <= _EditSelect.y2; y++) {
            WStr = _vText[y].wLine;
            string sTmp = wstrtostr(WStr.substr(0, _nTabSize));
            int nTabSize = 0;

            for (int n = 0; n < (int) sTmp.size(); n++)
                if (sTmp[n] == (char) TABCONVCHAR)
                    nTabSize++;

            if (nTabSize > 0)
                _vText[y].wLine = WStr.substr(nTabSize);
        }
        ScreenMemSave(_nLine, _nCulumn);
    }
}

void Editor::GotoLine() {
    string sNumber;
    int nLine;

    if (InputBox(_("go to line number."), sNumber) == -1) return;

    for (int n = 0; n < (int) sNumber.size(); n++) {
        if (isdigit((char) sNumber[n]) == 0) {
            String sMsg;
            sMsg.Append(_("not digit [%s"), sNumber.c_str());
            MsgBox(_("Error"), sMsg.c_str());
            return;
        }
    }

    if (sNumber.size() == 0) return;
    nLine = atoi(sNumber.c_str());

    if (nLine < (int) _vText.size()) {
        _nCurLine = nLine - 1;
        if (_nCurLine <= 0) _nCurLine = 0;
        _nFirstLine = _nCurLine - 10;
        if (_nFirstLine <= 0) _nFirstLine = 0;
    } else {
        _nCurLine = (int) _vText.size() - 1;
        _nFirstLine = _nCurLine - 10;
    }
    _EditMode = EDIT;
}

void Editor::GotoFirst() {
    _nCurLine = 0;
    _nFirstLine = 0;
    _EditMode = EDIT;
}

void Editor::GotoEnd() {
    _nCurLine = _vText.size() - 1;
    _nFirstLine = _nCurLine - 10;
    _EditMode = EDIT;
}

bool Editor::Quit() {
    vector<string> vStr;
    vStr.push_back(_("Yes"));
    vStr.push_back(_("No"));

    if (_bReadOnly == true) {
        Destroy();
        return true;
    }

    // if exist the change action, ask the file save.
    if (_nConvInfo != (int) _vDoInfo.size()) {
        vStr.push_back(_("Cancel"));

        int nCho = SelectBox(_("Editor exit. Do you want to save ?"), vStr, 2);

        switch (nCho) {
            case 0:
                FileSave();
            case 1:
                break;
            case 2:
            case -1:
                return false;
        }
    }

    Destroy();
    return true;
}
