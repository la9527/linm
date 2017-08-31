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

#include "define.h"
#include "panel.h"
#include "colorcfgload.h"
#include "sortfile.h"
#include "mlscfgload.h"
#include "cmdshell.h"
#include "mlslocale.h"
#include "mlsdialog.h"

using namespace MLSUTIL;
using namespace MLS;

void Panel::Init() {
    _uCur = 0;
    _nCol = 0;
    _nRow = 0;
    _nPage = 0;

    _uDir = 0;
    _uFile = 0;
    _uDirSize = 0;

    _uSelNum = 0;
    _uSelSize = 0;
    _bSearch = false;

    _tMemFile.Clear();
    _vDirFiles.clear();
}

void Panel::ConfigureLoad() {
    _bDirSort = g_tCfg.getBool("Default", "DirSort", true);
    _bShowHidden = g_tCfg.getBool("Default", "ShowHidden", false);
    _nSortMethod = (SortMethod) strtoint(g_tCfg.getValue("Default", "Sort", "5"));
}

void Panel::SetHiddenView(bool bView) {
    _bShowHidden = !bView;
}

void Panel::SetHiddenView() {
    _bShowHidden = !_bShowHidden;
}

/// \brief	파일명에 해당 되는 색상을 구함
/// \param	t		파일
/// \return	색상
ColorEntry Panel::GetColor(File *pFile) {
    string strName;

    // 확장자
    map<string, ColorEntry>::iterator exti = g_tColorCfg._mapExt.find(Tolower(pFile->Ext()));

    if (exti != g_tColorCfg._mapExt.end()) return (*exti).second;

    // 이름
    if (pFile->Ext().size())
        strName = pFile->sName.substr(0, pFile->sName.size() - pFile->Ext().size() - 1);
    else
        strName = pFile->sName;

    map<string, ColorEntry>::iterator namei = g_tColorCfg._mapName.find(Tolower(strName));

    if (namei != g_tColorCfg._mapName.end())
        return (*namei).second;

    // 퍼미션
    int mode = 0;
    for (int z = 1; z < 10; z++) {
        mode <<= 1;
        mode += pFile->sAttr[z] == '-' ? 0 : 1;
    }

    for (map<int, ColorEntry>::iterator mi = g_tColorCfg._mapMask.begin(); mi != g_tColorCfg._mapMask.end(); ++mi)
        if (mode & (*mi).first) return (*mi).second;

    return g_tColorCfg._DefaultColor;
}

void Panel::Sort(void) {
    vector<File *>::iterator begin = _vDirFiles.begin(),
            end = _vDirFiles.end();

    if (_bDirSort) {
        switch (_nSortMethod) {
            case SORT_NAME:
                sort(begin, end, sort_dir_adaptor<sort_name, sort_name>(_bDirSortAscend, _bFileSortAscend));
                break;
            case SORT_EXT:
                sort(begin, end, sort_dir_adaptor<sort_name, sort_ext>(_bDirSortAscend, _bFileSortAscend));
                break;
            case SORT_SIZE:
                sort(begin, end, sort_dir_adaptor<sort_name, sort_size>(_bDirSortAscend, _bFileSortAscend));
                break;
            case SORT_TIME:
                sort(begin, end, sort_dir_adaptor<sort_name, sort_time>(_bDirSortAscend, _bFileSortAscend));
                break;
            case SORT_COLOR:
                sort(begin, end, sort_dir_adaptor<sort_name, sort_color>(_bDirSortAscend, _bFileSortAscend));
                break;
            case SORT_NONE:
            default:
                return;
        }
    } else {
        switch (_nSortMethod) {
            case SORT_NAME:
                sort(begin, end, sort_ascend<sort_name>(_bFileSortAscend));
                break;
            case SORT_EXT:
                sort(begin, end, sort_ascend<sort_ext>(_bFileSortAscend));
                break;
            case SORT_SIZE:
                sort(begin, end, sort_ascend<sort_size>(_bFileSortAscend));
                break;
            case SORT_TIME:
                sort(begin, end, sort_ascend<sort_time>(_bFileSortAscend));
                break;
            case SORT_COLOR:
                sort(begin, end, sort_ascend<sort_color>(_bFileSortAscend));
                break;
            case SORT_NONE:
            default:
                return;
        }
    }
}

string Panel::GetPathView() const {
    if (_pReader == NULL) return "";

    string sTypeName, sName, sViewPath;
    string sInitType = _pReader->GetInitType();
    string::size_type idx = sInitType.find("://");
    if (idx != wstring::npos) {
        sTypeName = sInitType.substr(0, idx + 3);
        sName = sInitType.substr(idx + 3);
    } else
        sTypeName = sInitType;

    if (!sName.empty()) {
        string::size_type idx2 = sName.rfind("/");
        if (idx2 != wstring::npos)
            sName = sName.substr(idx2 + 1);
    }

    string sPath = _pReader->GetPath();
    sPath = isKorCode(sPath);

    if (sPath.size() != 0 && sPath[0] != '/')
        sPath = "/" + sPath;

    if (sTypeName == "file://")
        sViewPath = sPath;
    else
        sViewPath = sTypeName + sName + sPath;
    return sViewPath;
}

Reader *Panel::GetReader(const string &sPathOrType) {
    Reader *pReader = NULL;
    string sReaderName;

    string::size_type idx = sPathOrType.find("://");
    if (idx != wstring::npos)
        sReaderName = sPathOrType.substr(0, idx);
    else {
        if (sPathOrType.size() != 0)
            sReaderName = sPathOrType;
        else
            sReaderName = "file";
    }

    pReader = _tCtlReader.Get(sReaderName);

    if (pReader)
        pReader->SetTmpDir(g_tCfg.getValue("Static", "TmpDir"));

    LOG_WRITE("sReaderName [%s] pReader [%p]", sReaderName.c_str(), pReader);
    return pReader;
}

bool Panel::Read(const string &sPathConst) {
    Reader *pReader = NULL;
    string sPrevDir;

    string::size_type idx = string::npos;
    string sPath = sPathConst;

    LOG_WRITE("Panel::read [%s]", sPathConst.c_str());

    // . History에 예전 dir name 저장
    if (!_sCurPath.empty() && GetReader())
        _tHistoryMap.AddEntry(GetReader()->GetInitType(), _sCurPath);

    if (sPath.size() != 0) idx = sPath.find("://");

    if (idx != string::npos) {
        if ((pReader = GetReader(sPath)) == NULL) {
            MsgBox(_("Error"), _("plugin not found."));
            return false;
        }

        pReader->SetErrMsgShow(true);
        string sPath2 = sPath.substr(idx + 2);
        if (!pReader->Read(sPath2)) {
            if (pReader->GetConnected() == false) {
                MsgBox(_("Error"), _("remote disconnected."));

                pReader = GetReader();
                if (pReader->Read("~") == false) return false;
            } else
                return false;
        }
        pReader->SetErrMsgShow(false); // 메시지 박스 보이지 않게.
    } else {
        if ((pReader = GetReader(_sReaderType)) == NULL) {
            MsgBox(_("Error"), _("plugin not found."));
            return false;
        }

        pReader->SetErrMsgShow(true);
        if (!pReader->Read(sPath)) {
            if (pReader->GetConnected() == false) {
                MsgBox(_("Error"), _("remote disconnected."));

                pReader = GetReader();
                if (pReader->Read("~") == false) return false;
            } else
                return false;
        }
        pReader->SetErrMsgShow(false); // 메시지 박스 보이지 않게.
    }

    // . panel init
    Init();

    _bRoot = pReader->isRoot();

    while (pReader->Next()) {
        File &tFile = _tMemFile.Get();
        if (!pReader->GetInfo(tFile)) continue;

        if (tFile.sName == ".") continue;
        if (_bRoot && tFile.sName == "..") continue;
        if (!_bShowHidden && tFile.sName[0] == '.' && tFile.sName != "..") continue;

        _uDirSize += tFile.uSize;

        tFile.bSelected = false;
        tFile.tColor = GetColor(&tFile);

        _vDirFiles.push_back(&tFile);

        if (tFile.bDir) _uDir++;
        else _uFile++;
    }

    Sort();

    _pReader = pReader;

    if (sPrevDir != _sCurPath)
        sPrevDir = _sCurPath;

    _sCurPath = _pReader->GetPath();

    // . 바로 전 dir 에 커서를 위치시킨다.
    if (!sPrevDir.empty()) {
        if (sPath.size() != 0) idx = sPath.find("://");
        if (idx != string::npos) {
            LOG_WRITE("PrevDir 2 [%s]", sPrevDir.substr(idx + 2).c_str());

            SearchExactFile(sPrevDir.substr(idx + 2), _uCur, true);
        } else {
            LOG_WRITE("PrevDir 1 [%s]", sPrevDir.c_str());
            SearchExactFile(sPrevDir, _uCur, true);
        }
        _bChange = true;
    }

    ReadEnd();

    LOG_WRITE("read End [%s]", sPrevDir.c_str());
    return true;
}

/// @brief	왼쪽 방향키
void Panel::Key_Left() {
    if (_uCur < (unsigned) _nRow)
        SetCur(0);
    else
        SetCur(_uCur - _nRow);
    _bChange = false;
}

/// @brief	오른쪽 방향 키
void Panel::Key_Right() {
    int nCur;
    nCur = _uCur + _nRow;
    if (nCur > (int) _vDirFiles.size() - 1)
        nCur = _vDirFiles.size() - 1;
    SetCur(nCur);
    _bChange = false;
}

/// @brief	위쪽 방향키
void Panel::Key_Up() {
    if (_uCur > 0)
        SetCur(_uCur - 1);
    _bChange = false;
}

/// @brief	아래쪽 방향키
void Panel::Key_Down() {
    if (_uCur < _vDirFiles.size() - 1)
        SetCur(_uCur + 1);
    _bChange = false;
}

/// @brief	page up key
void Panel::Key_PageUp() {
    if (_uCur < (unsigned) _nRow * _nCol)
        SetCur(0);
    else
        SetCur(_uCur - (_nRow * _nCol));
    _bChange = false;
}

/// @brief	page down key
void Panel::Key_PageDown() {
    int nCur;
    nCur = _uCur + (_nRow * _nCol);
    if (nCur > (int) _vDirFiles.size() - 1)
        nCur = _vDirFiles.size() - 1;

    SetCur(nCur);
    _bChange = false;
}

/// @brief	home key
void Panel::Key_Home() {
    SetCur(0);
    _bChange = false;
}

/// @brief	Key End
void Panel::Key_End() {
    SetCur(_vDirFiles.size() - 1);
    _bChange = false;
}

void Panel::SelectExecute() {
    // 파일 실행
    if (_pReader->isChkFile(*_vDirFiles[_uCur]) == false) {
        return;
    }

    vector<string> q;

    string sFileName = MLSUTIL::addslash(_vDirFiles[_uCur]->sFullName);
    string sPrgExeCmd = g_tCfg.GetExtBind(_vDirFiles[_uCur]->Ext());
    string sCmd;

    vector<string> vCmd, vView;

    if (sPrgExeCmd.size() == 0) {
        sPrgExeCmd = g_tCfg.GetNameBind(_vDirFiles[_uCur]->sName);
    }

    LOG_WRITE("SelectExecute :: [%s]", sPrgExeCmd.c_str());

    if (sPrgExeCmd.size() == 0) {
        if (_vDirFiles[_uCur]->isExecute()) {
            sCmd = MLSUTIL::addslash(_vDirFiles[_uCur]->sFullName);
        }
    } else {
        StringToken tToken(sPrgExeCmd, ";");

        try {
            while (tToken.Next()) {
                string sTmp = tToken.Get();
                string sCmdTmp = getbetween(sTmp, '[', ']');

                string::size_type idx = sTmp.find("]");
                if (idx == string::npos) {
                    vView.push_back(sTmp);
                    vCmd.push_back(sTmp);
                } else {
                    vView.push_back(sCmdTmp);
                    vCmd.push_back(sTmp.substr(idx + 1));
                }
            }
        }
        catch (...) {
            MsgBox(_("Error"), _("configure file parsing error !!! "));
            return;
        }

        if (vView.size() != 0) {
            q = vView;
            q.push_back("-----------------");
        }
    }

    q.push_back(_("Run"));
    q.push_back(_("Run(root)"));
    q.push_back(_("Edit"));
    q.push_back(_("ExtEditor"));
    q.push_back(_("Parameter"));
    q.push_back(_("Parameter(root)"));
    q.push_back(_("Cancel"));

    int nSelect = SelectBox(_("Execute select"), q);

    if (nSelect == -1) return;

    if ((int) vView.size() > 0 && nSelect < (int) vView.size()) {
        sCmd = vCmd[nSelect];
        string sExeCmd = getbetween(sCmd, '<', '>');

        LOG_WRITE("Enter Command : [%s] [%s]", sCmd.c_str(), sExeCmd.c_str());

        if (sExeCmd.size() != 0) {
            // command 실행 Command 에 Execute 실행
            Execute(sExeCmd);
        } else {
            // 프로그램 실행 Command 에 ParseAndRun 실행
            ParseAndRun(sCmd, false);
        }
    } else {
        switch (nSelect - vView.size() - 1) {
            case 0:
                ParseAndRun(sFileName + " %W");
                return;
            case 1:
                ParseAndRun(sFileName + " %R %W");
                return;
            case 2:
                Execute("Cmd_Editor");
                return;
            case 3:
                Execute("Cmd_ExtEditor");
                return;
            case 4:
            case 5: {
                std::string sParam;
                if (InputBox(_("Input parameter."), sParam) > 0) {
                    if (nSelect == 4) {
                        ParseAndRun(sFileName + " %W " + sParam);
                    } else {
                        ParseAndRun(sFileName + " %R %W " + sParam);
                    }
                }
                return;
            }
        }
    }
}

/// @brief	Key Enter
void Panel::Key_Enter() {
    if (_vDirFiles.size() <= _uCur) return;

    File *pFile = _vDirFiles[_uCur];

    if (pFile->bDir) {
        LOG_WRITE("Key_Enter !!! [%s] [%s]", pFile->sFullName.c_str(), pFile->sName.c_str());
        // 이때는 플러그인에서 나올때 원래 위치로 돌아가기 위한..
        if (pFile->sFullName == "Exit" && pFile->sName == "..") {
            if (_pReader->GetInitType().substr(0, 10) == "archive://") {
                string sCurArchiveName = _pReader->GetInitType();
                sCurArchiveName = sCurArchiveName.substr(10);
                PluginClose();

                if (GetReader()->GetReaderName() == "ftp" ||
                    GetReader()->GetReaderName() == "sftp") {
                    remove(sCurArchiveName.c_str());
                } else {
                    SearchExactFile(sCurArchiveName, _uCur, true);
                    SetCur(_uCur);
                }

            } else {
                string sCurArchiveName = _pReader->GetInitType();
                PluginClose();

                if (GetReader()->GetReaderName() == "ftp" ||
                    GetReader()->GetReaderName() == "sftp") {
                    remove(sCurArchiveName.c_str());
                }
            }

            LOG_WRITE("Key_Enter Exit Zip !!!");
        } else {
            string sBefPath = _sCurPath;
            if (!Read(pFile->sFullName))
                if (!Read(sBefPath))
                    Read("~");
        }
    } else {
        File tFile;

        if (_pReader->isChkFile(*pFile) == false) return;

        // View 할때 파일 압축 파일이나 기타 파일들이 /tmp로 복사 할수 있게 한다.
        if (_pReader->View(pFile, &tFile) == false) return;

        string sCmd = g_tCfg.GetExtBind(tFile.Ext());

        if (sCmd.size() == 0) {
            string sName;

            if (tFile.Ext().size())
                sName = tFile.sName.substr(0, tFile.sName.size() - tFile.Ext().size() - 1);
            else
                sName = tFile.sName;

            sCmd = g_tCfg.GetNameBind(sName);
            if (sCmd.size() == 0) {
                LOG_WRITE("GetBind1 [%s] [%s]", sCmd.c_str(), sName.c_str());

                if (tFile.isExecute()) {
                    ParseAndRun(tFile.sFullName, true);
                }
                return;
            }
        }

        LOG_WRITE("GetBind [%s]2", sCmd.c_str());

        StringToken tToken(sCmd, ";");
        vector<string> vCmd, vView;

        try {
            while (tToken.Next()) {
                string sTmp = tToken.Get();
                string sCmdTmp = getbetween(sTmp, '[', ']');

                string::size_type idx = sTmp.find("]");
                if (idx == string::npos) {
                    vView.push_back(sTmp);
                    vCmd.push_back(sTmp);
                } else {
                    vView.push_back(sCmdTmp);
                    vCmd.push_back(sTmp.substr(idx + 1));
                }
            }
        }
        catch (...) {
            MsgBox(_("Error"), _("configure file parsing error !!! "));
            return;
        }

        bool bExecuteSelect = false;

        if (g_tCfg.getBool("Default", "Enter_RunSelect", false) == true) {
            if (vCmd.size() > 1) {
                int nSelect = SelectBox(_("Execute select"), vView);
                if (nSelect == -1) return;

                sCmd = vCmd[nSelect];
                bExecuteSelect = true;
            }
        }

        string sExeCmd;

        if (!bExecuteSelect) {
            for (int n = 0; n < (int) vCmd.size(); n++) {
                sExeCmd = getbetween(sCmd, '<', '>');
                sCmd = vCmd[n];

                // command number size 1 ; break
                if (vCmd.size() == 1 || sExeCmd.size() != 0) break;

                // get first word command.
                StringToken tToken(sCmd, " ");
                tToken.Next();
                string strCmd = tToken.Get();

                vector<string> vList = CmdShell::CmdExecute("whereis " + strCmd + " | wc -w");
                LOG_WRITE("CmdShell::CmdExecute.... [%s]", strCmd.c_str());

                if (vList.size() == 1 && MLSUTIL::strtoint(vList[0]) > 1) {
                    LOG_WRITE("MLSUTIL::strtoint(%s)", (const char *) vList[0].c_str());
                    break;
                }
            }
        }

        LOG_WRITE("Enter Command : [%s] [%s]", sCmd.c_str(), sExeCmd.c_str());

        if (sExeCmd.size() != 0) {
            // command 실행 Command 에 Execute 실행
            Execute(sExeCmd);
        } else {
            // 프로그램 실행 Command 에 ParseAndRun 실행
            ParseAndRun(sCmd, false);
        }
    }
    Refresh();
}

void Panel::Key_Select() {
    ToggleSelect();
    Key_Down();
    _bChange = true;
}

///	@brief	파일을 선택한다.(반전 시킴)
///	@param	f	선택할 파일
void Panel::Select(File &f) {
    if (f.sName == "..")
        return;

    if (f.bSelected)
        return;

    // 인덱스를 저장
    f.bSelected = true;
    _uSelSize += f.uSize;
    _uSelNum++;
}

///	@brief	파일선택을 해제한다.
///	@param	f	선택 해제할 파일
void Panel::Deselect(File &f) {
    if (!f.bSelected)
        return;

    f.bSelected = false;
    _uSelSize -= f.uSize;
    _uSelNum--;
}

/// @brief	현재 파일 선택
void Panel::ToggleSelect() {
    if (_vDirFiles[_uCur]->bSelected) {
        Deselect(*_vDirFiles[_uCur]);
    } else {
        Select(*_vDirFiles[_uCur]);
    }
}

/// @brief  키를 입력받아 파일 찾기
/// @param  tKeyInfo 입력받은 key값.
/// @return 찾았는가 못찾았는가
bool
Panel::SearchProcess(KeyInfo &tKeyInfo) {
    int key = (int) tKeyInfo;
    if (!_bSearch && ((32 < key && key <= 126) || key == '.')) _bSearch = true;

    if (_bSearch && key == '\t') {// 탭이 나오면 다음 것을 찾는다.
        if (SearchMatchingFile(_sStrSearch, _uCur, _uCur + 1)) {
            SetCur(_uCur);
        }
        return true;
    }

    if (_bSearch && tKeyInfo.sKeyName == "ESC") {
        _bSearch = false;
        _sStrSearch.erase();
        return true;
    }
    // .. 파일을 찾아 커서를 파일위에 둔다
    if (_bSearch && key != '/' && ((32 < key && key <= 126) || tKeyInfo.sKeyName == "BS")) {
        // search mode;
        // 뒷부분에 key를 넣고
        if (tKeyInfo.sKeyName == "BS") {
            _sStrSearch.erase(_sStrSearch.size() - 1, 1);
            if (_sStrSearch.empty()) {
                _bSearch = false;
                return true;
            }
        } else {
            _sStrSearch += (char) key;
        }

        bool bFind = false;
        if ((bFind = SearchMatchingFile(_sStrSearch, _uCur, _uCur)) == false) {
            // 없으면 처음에 넣고 다시 찾는다.
            // bugfix : gcc 2.96
            string sKey;
            sKey = (char) key;
            if (SearchMatchingFile(sKey, _uCur)) {
                _sStrSearch = (char) key;
                SetCur(_uCur);
            } else {
                _sStrSearch.erase(_sStrSearch.size() - 1, 1);
            }
        } else {
            SetCur(_uCur);
        }

        return true;
    }

    _bSearch = false;
    _sStrSearch.erase();
    return false;
}

/// @brief	비슷한 이름을 가진 파일을 찾는다.
/// @param	str		IN 찾을 파일 앞자리나 전체 파일명
/// @param	d_index	OUT 찾은 파일 Index.
/// @param  s_index IN 파일을 찾을 첫번째 인덱스
/// @return true, false
bool Panel::SearchMatchingFile(const string &str, uint &d_index, int s_index /* = 0 */) {
    if (_vDirFiles.empty()) return false;

    int l = _vDirFiles.size(), len = str.size();
    string p = Tolower(str);

    for (int t = 0; t < l; t++) {
        int x = (t + s_index) % l;
        if (_vDirFiles[x]->sName.size() < str.size()) continue;

        if (Tolower(_vDirFiles[x]->sName.substr(0, len)) == p) {
            d_index = x;
            return true;
        }
    }

    for (int t = 0; t < l; t++) {
        int x = (t + s_index) % l;
        if (_vDirFiles[x]->sName.size() < str.size()) continue;
        if (Tolower(_vDirFiles[x]->sName).find(p) != string::npos) // 비슷한 파일명을 찾기 위해서 사용
        {
            d_index = x;
            return true;
        }
    }
    // 없으면
    return false;
}

/// @brief	같은 이름을 가진 파일을 찾는다.
/// @param	fileName		IN 찾을 파일 파일명
/// @param	fileIndex		OUT 찾은 파일 위치 index
/// @return	성공 여부.
bool
Panel::SearchExactFile(const string &fileName, uint &fileIndex, bool bFullName) const {
    /// @todo implement me
    // 파일 목록 순환
    int filecnt = 0;
    vector<File *>::const_iterator i;
    string sName;

    for (i = _vDirFiles.begin(); i != _vDirFiles.end(); ++i) {
        if (bFullName) {
            if ((*i)->sFullName == fileName) {
                // 파일명이 같으면 index 반환
                fileIndex = filecnt;
                return true;
            }
        } else {
            if ((*i)->sName == fileName) {
                // 파일명이 같으면 index 반환
                fileIndex = filecnt;
                return true;
            }
        }
        ++filecnt;
    }
    return false;
}

int Panel::GetSelection(Selection &tSelection) {
    if (_uSelNum == 0) {
        if (_vDirFiles[_uCur]->sName != "..") {
            tSelection.Add(_vDirFiles[_uCur]);
        }
    } else {
        vector<File *>::const_iterator i;

        for (i = _vDirFiles.begin(); i != _vDirFiles.end(); ++i) {
            if ((*i)->bSelected) {
                tSelection.Add(*i);
            }
        }
    }

    //tSelection.SetBaseDir(_sCurPath);
    return SUCCESS;
}

bool Panel::SetCurFileName(const string &sFileName) {
    uint uNumber = 0;
    if (SearchExactFile(sFileName, uNumber, true)) {
        SetCur(uNumber);
        return true;
    }
    return false;
}

void Panel::MountList() {
    vector<LineArgData> vLineArgData;
    vector<string> vViewList;
    vector<string> vMountList;

    if (_pReader == NULL || _pReader->GetReaderName() != "file") {
        MsgBox(_("Error"), _("mounted list local system use only."));
        return;
    }

    if (CmdShell::CmdExeArg("mount", 0, vLineArgData, false) == -1) {
        LOG_WRITE("CmdExeArg Error !!!");
        return;
    }

    String sView;

    for (uint n = 0; n < vLineArgData.size(); n++) {
        LineArgData vArgData = vLineArgData[n];

        if (vArgData.size() > 5) {
            if (vArgData[0].substr(0, 4) == "/dev") {
                sView.setEmpty();
                sView.appendBlank(10, "%s", vArgData[4].c_str());
                sView.appendBlank(14, "%s", vArgData[0].c_str());
                sView.append(" %s", vArgData[2].c_str());

                vViewList.push_back(sView.c_str());
                vMountList.push_back(vArgData[2]);
            }
        }
    }

    if (vViewList.size() == 0) return;

    int nCho = SelectBox(_("go to the mounted directory."), vViewList, 0);
    if (nCho == -1) return;

    if ((int) vMountList.size() > nCho) {
        Read(vMountList[nCho]);
        Refresh();
    }
}

void Panel::Back() {
    string sPath = _tHistoryMap.GetPrev(GetReader()->GetInitType());

    if (!sPath.empty()) {
        Read(sPath);
        Refresh();
    }
}

void Panel::Forward() {
    string sPath = _tHistoryMap.GetNext(GetReader()->GetInitType());

    if (!sPath.empty()) {
        Read(sPath);
        Refresh();
    }
}
