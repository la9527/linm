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

#include "mainframe.h"
#include "subshell.h"
#include <functional>

using namespace MLSUTIL;
using namespace MLS;

inline void SetPosition(Position *pPosition, Form *pForm, int y, int x, int height, int width) {
    pPosition->SetForm(pForm);
    pPosition->y = y;
    pPosition->x = x;
    pPosition->height = height;
    pPosition->width = width;
}

inline void SetPosition(Form *pForm, int y, int x, int height, int width) {
    pForm->y = y;
    pForm->x = x;
    pForm->height = height;
    pForm->width = width;
}

void TitleChange(const string &sPath, bool bHostNameShow) {
    string sHostName, sLogin;
    string sStr = sPath;
    char cHostName[100];

    memset(&cHostName, 0, sizeof(cHostName));

    if (scrstrlen(sStr) > COLS) {
        sStr = scrstrncpy(sStr, scrstrlen(sStr) - COLS + 20, COLS - 20);
        sStr = "..." + sStr;
    }

    if (gethostname((char *) &cHostName, sizeof(cHostName)) != -1) {
        struct passwd *pw = getpwuid(getuid());
        if (pw)
            sLogin = pw->pw_name;
    } else
        sprintf(cHostName, "localhost");

    if (strlen(cHostName) == 0 || sLogin.size() == 0 || !bHostNameShow) {
        printf("%c]0;LinM %s - %s%c", '\033', VERSION, sStr.c_str(), '\007');
    } else {
        sHostName = cHostName;
        if (sHostName.find(".") != string::npos)
            sHostName = sHostName.substr(0, sHostName.find("."));

        printf("%c]0;LinM %s - %s@%s:%s%c", '\033', VERSION, sLogin.c_str(), sHostName.c_str(), sStr.c_str(), '\007');
    }
}

void MainFrame::UpdateConfig() {
    _bSplit = _Config.GetBool("User", "SplitWindow", false);
    _bViewType = _Config.GetBool("User", "SplitType", false);
    _bScrSync = _Config.GetBool("User", "ScreenSync", false);
    _sLastPath = _Config.GetValue("User", "LastPath", "~");
    _bAlwaysRedraw = _Config.GetBool("Default", "AlwaysRedraw", false);
    _sLineCodeChange = Tolower(_Config.GetValue("Default", "LineCodeChange", "Auto"));
    {
        int i_sort;
        i_sort = _Config.GetValueNum("Default", "Sort", 5);
        _tPanel[0].SetSortMethod(i_sort);
        _tPanel[1].SetSortMethod(i_sort);
    }
    _bHintView = _Config.GetBool("Default", "HintView", true);

    if (_sLineCodeChange == "auto")
        e_nBoxLineCode = AUTOLINE;
    else if (_sLineCodeChange == "on")
        e_nBoxLineCode = CHARLINE;
    else
        e_nBoxLineCode = ACSLINE;

    Set_BoxLine(e_nBoxLineCode);
}

void MainFrame::SaveConfig() {
    _Config.SetBool("User", "SplitWindow", _bSplit, true);
    _Config.SetBool("User", "SplitType", _bViewType, true);
    _Config.SetBool("User", "ScreenSync", _bScrSync, true);

    if (e_nBoxLineCode == CHARLINE)
        _Config.SetValue("Default", "LineCodeChange", "On", true);
    else if (e_nBoxLineCode == ACSLINE)
        _Config.SetValue("Default", "LineCodeChange", "Off", true);
    else
        _Config.SetValue("Default", "LineCodeChange", "Auto", true);

    if (_tCommand.GetPanel()->GetReader()->GetInitType() == "file://") {
        _sLastPath = _tPanel[_nActive].GetPath();
        _Config.SetValue("User", "LastPath", _sLastPath);
    }
    g_tCfg.Save(); // when quit, configuration file save.
}

void MainFrame::Init() {
    if (!_tPanel[0].Read("."))
        if (!_tPanel[0].Read("~"))
            exit(1);
    if (!_tPanel[1].Read(_sLastPath))
        if (!_tPanel[1].Read("~"))
            exit(1);

    _tMcd[0]._bFocus = _tPanel[0]._bFocus = _nActive ? false : true;
    _tMcd[1]._bFocus = _tPanel[1]._bFocus = _nActive ? true : false;

    if (!_bScrSync) {
        _eViewType[0] = PANEL;
        _eViewType[1] = PANEL;
    } else {
        _eViewType[0] = MCD;
        _eViewType[1] = PANEL;
        _tMcd[0]._bFocus = true;
        _tPanel[1]._bFocus = true;
        _tMcd[0].AddDirectory(_tPanel[1].GetReader()->GetPath());
        _tMcd[0].SetCur(_tPanel[1].GetReader()->GetPath());
    }
    _tCommand.SetPanel(&_tPanel[_nActive]);
    _tCommand.SetMcd(&_tMcd[_nActive]);
    _tCommand.SetEditor(&_tEditor[_nActive]);
    _bShell = false;
}

void MainFrame::DoMcd() {
    _bSplit = false;
    _tMcd[0]._bFocus = true;
    _tPanel[1]._bFocus = true;
    _tMcd[0].AddDirectory(_tPanel[0].GetReader()->GetPath());
    _tMcd[0].SetCur(_tPanel[1].GetReader()->GetPath());
    _eViewType[0] = MCD;
}

void MainFrame::DrawInit() {
    bool bHostShow = false;
    NCurses_Panel *pPanel = _tCommand.GetPanel();

    auto funcDrawChange = [this, pPanel](const int active,
                                         const std::function<void()> &positionFunc = nullptr) -> void {
        if (positionFunc) {
            positionFunc();
        } else {
            SetPosition(&_tDrawPath[active], this, 1, 0, 1, width);
        }

        ViewType viewType = _eViewType[active];
        if (viewType == PANEL) {
            _tDrawPath[active].SetData(pPanel->GetPathView());
        } else if (viewType == MCD) {
            _tDrawPath[active].SetData(_tMcd[active].GetCurName());
        } else if (viewType == EDITOR) {
            _tDrawPath[active].SetData(_tEditor[active].GetViewTitle());
        }
        _tDrawPath[active].Show();
    };

    if (pPanel->GetReader()->GetInitType() == "file://")
        bHostShow = true;

    if (_bTitleChange) {
        ViewType viewType = _eViewType[_nActive];

        if (viewType == PANEL) {
                TitleChange(pPanel->GetPathView(), bHostShow);
        } else if (viewType == MCD) {
                TitleChange(_tMcd[_nActive].GetCurName(), bHostShow);
        } else if (viewType == EDITOR) {
                TitleChange(_tEditor[_nActive].GetViewTitle(), bHostShow);
        }
    }

    //if (pPanel->_bChange || _bAlwaysRedraw) // 화면이 바뀔 경우에만 그린다.
    {
        SetPosition(&_tDrawTop, this, 0, 0, 1, width);
        _tDrawTop.SetViewType(_eViewType[_nActive]);
        _tDrawTop.Show();

        if (!_bSplit) {
            funcDrawChange(_nActive, nullptr);
        } else if (_bSplit && !_bViewType) {
            if (_bScrSync) {
                funcDrawChange(_nActive, nullptr);
            } else {
                funcDrawChange(0, [this]() {
                SetPosition(&_tDrawPath[0], this, 1, 0, 1, width / 2);
                });
                funcDrawChange(1, [this]() {
                    SetPosition(&_tDrawPath[1], this, 1, _tDrawPath[0].width, 1, width - _tDrawPath[0].width);
                });
            }
        } else if (_bSplit && _bViewType) {
            funcDrawChange(0, [this]() {
            SetPosition(&_tDrawPath[0], this, 1, 0, 1, width);
            });
        }
    }
}

void MainFrame::DrawStatus() {
    NCurses_Panel *pPanel = _tCommand.GetPanel();

    if (_bHintView) {
        SetPosition(&_tHint, this, height - 2, 0, 1, width);
        _tHint.SetClip(_eMcdClipCopy);
        _tHint.SetMcdExeMode(_tMcdExecuteMode.eMcdExeMode);
        _tHint.SetViewType(_eViewType[_nActive]);
        _tHint.Show();
    }

    if (!_bShell) {
        SetPosition(&_tDirInfo, this, height - 1, 0, 1, width);
        _tDirInfo.SetFile(pPanel);
        _tDirInfo.Show();
    }

    //if (pPanel->_bChange || _bAlwaysRedraw)
    {
        if (!_bSplit) {
            switch (_eViewType[_nActive]) {
                case PANEL:
                    SetPosition(&_tStatusInfo[0], pPanel, pPanel->_nRow + 2, 0, 1, pPanel->width);
                    _tStatusInfo[0].SetFile(pPanel);
                    _tStatusInfo[0].Show();
                    break;
                default:
                    break;
            }
        } else if (_bSplit && !_bViewType) {
            if (_eViewType[0] == PANEL || _eViewType[1] == PANEL) {
                SetPosition(&_tStatusInfo[0], this, pPanel->height + 2, 0, 1, width);
                _tStatusInfo[0].SetFile(pPanel);
                _tStatusInfo[0].Show();
            }
        } else if (_bSplit && _bViewType) {
            switch (_eViewType[0]) {
                case PANEL:
                    SetPosition(&_tStatusInfo[0], this, _tPanel[0].height + 2, 0, 1, width);
                    _tStatusInfo[0].SetFile(&_tPanel[0]);
                    _tStatusInfo[0].Show();
                    break;
                default:
                    break;
            }

            ColorEntry tLineColor = g_tColorCfg.GetColorEntry("Line");
            setcol(tLineColor, _pWin);
            mvwhline(_pWin, _tPanel[0].height + 3, 0, HLINE, width);

            SetPosition(&_tDrawPath[1], this, _tPanel[0].height + 4, 0, 1, width);
            switch (_eViewType[1]) {
                case PANEL:
                    _tDrawPath[1].SetData(_tPanel[1].GetPathView());
                    break;
                case MCD:
                    _tDrawPath[1].SetData(_tMcd[1].GetCurName());
                    break;
                case EDITOR:
                    _tDrawPath[1].SetData(_tEditor[1].GetViewTitle());
                    break;
                default:
                    break;
            }
            _tDrawPath[1].Show();

            SetPosition(&_tStatusInfo[1], this, _tPanel[0].height + _tPanel[1].height + 5, 0, 1, width);
            _tStatusInfo[1].SetFile(&_tPanel[1]);
            _tStatusInfo[1].Show();
        }
    }
}

void MainFrame::Draw() {
    x = 0;
    y = 0;
    width = COLS;
    height = LINES;

    DrawInit();

    int nHintHideHeight = _bHintView ? 0 : 1;

    if (!_bSplit) {
        SetPosition(&_tPanel[_nActive], 2, 0, height - 4 + nHintHideHeight, width);
        _tPanel[_nActive].SetViewRowFixed(false);
        SetPosition(&_tMcd[_nActive], 2, 0, height - 4 + nHintHideHeight, width);
        _tMcd[_nActive].SetReader(_tPanel[_nActive].GetReader());
        SetPosition(&_tEditor[_nActive], 2, 0, height - 4 + nHintHideHeight, width);
        //_tEditor[_nActive].SetReader(_tEditor[_nActive].GetReader());

        switch (_eViewType[_nActive]) {
            case PANEL:
                _tPanel[_nActive].Show();
                break;
            case MCD:
                _tMcd[_nActive].Show();
                break;
            default:
                break;
        }
    } else if (_bSplit && !_bViewType)    // vertical (left, right)
    {
        SetPosition(&_tPanel[0], 2, 0, height - 5 + nHintHideHeight, width / 2);
        _tPanel[0].SetViewRowFixed(true);
        SetPosition(&_tMcd[0], 2, 0, height - 5 + nHintHideHeight, width / 2);
        _tMcd[0].SetReader(_tPanel[0].GetReader());
        SetPosition(&_tEditor[0], 2, 0, height - 5 + nHintHideHeight, width / 2);
        //_tEditor[0].SetReader(_tEditor[0].GetReader());

        switch (_eViewType[0]) {
            case PANEL:
                _tPanel[0].Show();
                break;
            case MCD:
                _tMcd[0].Show();
                break;
            default:
                break;
        }

        SetPosition(&_tPanel[1], 2, _tPanel[0].width, height - 5 + nHintHideHeight, width - _tPanel[0].width);
        _tPanel[1].SetViewRowFixed(true);
        SetPosition(&_tMcd[1], 2, _tMcd[0].width, height - 5 + nHintHideHeight, width - _tMcd[0].width);
        _tMcd[1].SetReader(_tPanel[1].GetReader());
        SetPosition(&_tEditor[1], 2, _tEditor[0].width, height - 5 + nHintHideHeight, width - _tEditor[0].width);
        //_tEditor[1].SetReader(_tEditor[1].GetReader());

        switch (_eViewType[1]) {
            case PANEL:
                _tPanel[1].Show();
                break;
            case MCD:
                _tMcd[1].Show();
                break;
            default:
                break;
        }
    } else if (_bSplit && _bViewType)    // horizontal (left, right)
    {
        SetPosition(&_tPanel[0], 2, 0, (height - 8) / 2, width);
        _tPanel[0].SetViewRowFixed(true);
        SetPosition(&_tMcd[0], 2, 0, (height - 8) / 2, width);
        _tMcd[0].SetReader(_tPanel[0].GetReader());
        SetPosition(&_tEditor[0], 2, 0, (height - 6) / 2, width);
        //_tEditor[0].SetReader(_tEditor[0].GetReader());

        SetPosition(&_tPanel[1], _tPanel[0].height + 5, 0, height - (_tPanel[0].height + 8) + nHintHideHeight, width);
        _tPanel[1].SetViewRowFixed(true);
        SetPosition(&_tMcd[1], _tMcd[0].height + 5, 0, height - (_tMcd[0].height + 8) + nHintHideHeight, width);
        _tMcd[1].SetReader(_tPanel[1].GetReader());
        SetPosition(&_tEditor[1], _tEditor[0].height + 4, 0, height - (_tEditor[0].height + 7) + nHintHideHeight,
                    width);
        //_tEditor[1].SetReader(_tPanel[1].GetReader());

        switch (_eViewType[0]) {
            case PANEL:
                _tPanel[0].Show();
                break;
            case MCD:
                _tMcd[0].Show();
                break;
            default:
                break;
        }

        switch (_eViewType[1]) {
            case PANEL:
                _tPanel[1].Show();
                break;
            case MCD:
                _tMcd[1].Show();
                break;
            default:
                break;
        }
    }

    curs_set(0);
    DrawStatus();

    // Draw on here, because cursor position.
    if (!_bSplit) {
        if (_eViewType[_nActive] == EDITOR) {
            _tEditor[_nActive]._bFullScreen = true;
            _tEditor[_nActive].Show();

            if (_tEditor[_nActive].IsEditMode())
                curs_set(1);
        }
    } else {
        if (_eViewType[!_nActive] == EDITOR) {
            _tEditor[_nActive]._bFullScreen = false;
            _tEditor[!_nActive].Show();
        }
        if (_eViewType[_nActive] == EDITOR) {
            _tEditor[_nActive]._bFullScreen = false;
            _tEditor[_nActive].Show();
            if (_tEditor[_nActive].IsEditMode())
                curs_set(1);
        }
    }

    if (_bShell) {
        SetPosition(&_tShell, this, height - 1, 0, 1, width);
        _tShell.SetPanel(_tCommand.GetPanel());
        _tShell.Show();
    }
}

void MainFrame::CmdShell(bool bExecute) {
    _bShell = bExecute;
}

void MainFrame::Execute(KeyInfo &tKeyInfo) {
    _tPanel[_nActive]._bChange = true;
    LOG_WRITE("Key [%d] [%s]", (int) tKeyInfo, tKeyInfo.sKeyName.c_str());

    if (_bShell) {
        int nKey = _tShell.DataInput(tKeyInfo);

        switch (nKey) {
            case KEY_ENTER: {
                string str = _tShell.GetCmdStr();
                _tShell.DataClear();

                if (str == "") break;

                _tCommand.ParseAndRun(str, true);
                Refresh(false);
                return;
            }

            case KEY_MOUSE:
                MouseProc();
                return;

            case 15:    // SubShell (ConsoleMode)
                _tCommand.Execute("Cmd_ConsoleMode");
                curs_set(1);
                return;
            default:
                break;
        }
        _bShell = false;
        curs_set(0);
        return;
    }

    string sCmd = g_tKeyCfg.GetCommand(tKeyInfo, _eViewType[_nActive]);
    int nRt = 0;

    bool bSearch = true;
    if (sCmd.size() && (32 < (int) tKeyInfo && (int) tKeyInfo <= 126)) {
        if (tKeyInfo.sKeyName != "ESC" && tKeyInfo.sKeyName != "BS")
            if (!_tPanel[_nActive]._bSearch)
                bSearch = false;
    }

    if (bSearch) {
        switch (_eViewType[_nActive]) {
            case PANEL:
                if (_tPanel[_nActive].SearchProcess(tKeyInfo)) return;
                break;
            case MCD:
                if (_tMcd[_nActive].SearchProcess(tKeyInfo)) return;
                break;
            default:
                break;
        }
    }

    if (sCmd.size()) {
        if (_eViewType[_nActive] == EDITOR)
            curs_set(0);


        nRt = _tCommand.Execute(sCmd);

        /*
        String sMsg;
        sMsg.Append(_("Execute [%d] [%s]"), nRt, ((string)tKeyInfo).c_str());
        MsgBox(_("Error"), sMsg.c_str());
        */

        if (nRt == -2) // editor -2 return
        {
            if (_eViewType[_nActive] == EDITOR)
                if ((int) tKeyInfo > 27)
                    _tEditor[_nActive].InputData((string) tKeyInfo);
        }
    } else {
        if (_eViewType[_nActive] == EDITOR) {
            if ((int) tKeyInfo == 8 || (int) tKeyInfo == 263) // OntheSpot Patch
            {
                _tEditor[_nActive].Key_BS();

                if (tKeyInfo.size() > 1)
                    _tEditor[_nActive].InputData((string) tKeyInfo);
            } else if ((int) tKeyInfo > 27) {
                _tEditor[_nActive].InputData((string) tKeyInfo);
                LOG_WRITE("KEY INPUT :: [%s]", ((string) tKeyInfo).c_str());
            }
        } else {
            string sCmd = g_tKeyCfg.GetRunCmd(tKeyInfo);

            nRt = _tCommand.Execute(sCmd);
        }
    }

    if (_bShell) curs_set(1);

#ifdef __DEBUGMODE__
    if (nRt == ERROR) {
        String sMsg(_("configure command not found.."));
        sMsg.Append(_("[%s]"), sCmd.c_str());
        MsgBox(_("Error"), sMsg.c_str());
        return;
    }
#endif
}

bool MainFrame::MouseEvent(int Y, int X, mmask_t bstate) {
    if (_bShell && Y != height - 1)
        _bShell = false;

    if (Y == 0) {
        LOG_WRITE("MouseEvent Y [%d] X [%d]", Y, X);
        int nFunc = _tDrawTop.MouseEvent(Y, X, bstate);
        if (nFunc != -1) {
            LOG_WRITE("Mouse Event FUNC [%d]", nFunc);
            String sKeyStr;
            sKeyStr.Append("F%d", nFunc);
            _tCommand.Execute(g_tKeyCfg.GetCommand(sKeyStr.c_str(), _eViewType[_nActive]));
        }
        return false;
    }

    if (!_bSplit) {
        if (_tPanel[_nActive].AreaChk(Y, X)) {
            switch (_eViewType[_nActive]) {
                case PANEL:
                    _tPanel[_nActive].MouseEvent(Y, X, bstate);
                    break;
                case MCD:
                    if (_tMcd[_nActive].MouseEvent(Y, X, bstate) == true) {
                        _tCommand.Execute("Cmd_Enter");
                        _eViewType[_nActive] = PANEL;
                        _tPanel[_nActive]._bFocus = true;
                        Refresh();
                    }
                    break;
                case EDITOR: {
                    //_tEditor[_nActive].MouseEvent(Y, X, bstate);
                }
                default:
                    break;
            }
        }
    } else {
        if (_tPanel[0].AreaChk(Y, X)) {
            switch (_eViewType[0]) {
                case PANEL:
                    _tPanel[0].MouseEvent(Y, X, bstate);
                    _tPanel[0]._bFocus = true;
                    break;
                case MCD:
                    if (_tMcd[0].MouseEvent(Y, X, bstate) == true) {
                        _tCommand.Execute("Cmd_Enter");
                        if (!_bScrSync) {
                            _eViewType[0] = PANEL;
                            _tPanel[0]._bFocus = true;
                        }
                    } else
                        _tMcd[0]._bFocus = true;
                    break;
                case EDITOR: {
                    //_tEditor[0].MouseEvent(Y, X, bstate);
                }
                default:
                    break;
            }

            _nActive = 0;
        }

        if (_tPanel[1].AreaChk(Y, X)) {
            switch (_eViewType[1]) {
                case PANEL:
                    _tPanel[1].MouseEvent(Y, X, bstate);
                    _tPanel[1]._bFocus = true;
                    break;
                case MCD:
                    if (_tMcd[1].MouseEvent(Y, X, bstate) == true) {
                        _tCommand.Execute("Cmd_Enter");
                        if (!_bScrSync) {
                            _eViewType[1] = PANEL;
                            _tPanel[1]._bFocus = true;
                        }
                    } else
                        _tMcd[1]._bFocus = true;
                    break;
                case EDITOR: {
                    //_tEditor[1].MouseEvent(Y, X, bstate);
                    break;
                }
                default:
                    break;
            }

            _nActive = 1;
        }

        if (!_bScrSync) {
            _tEditor[0]._bFocus = _tMcd[0]._bFocus = _tPanel[0]._bFocus = _nActive ? false : true;
            _tEditor[1]._bFocus = _tMcd[1]._bFocus = _tPanel[1]._bFocus = _nActive ? true : false;
        }
        _tCommand.SetPanel(&_tPanel[_nActive]);
        _tCommand.SetMcd(&_tMcd[_nActive]);
        _tCommand.SetEditor(&_tEditor[_nActive]);
        Refresh();
    }
    return false;
}

void MainFrame::Split() {
    _bScrSync = false;

    if (_bSplit && !_bViewType) {
        _bSplit = true;
        _bViewType = true;
    } else if (_bSplit && _bViewType) {
        _bSplit = false;
        _bViewType = false;
    } else {
        _bSplit = !_bSplit;
        _bViewType = false;
    }
    Refresh();

    if (_tEditor[_nActive]._bFocus) {
        if (_bSplit) {
            MouseInit();
            _tEditor[_nActive]._bMouseMode = true;
        } else {
            MouseDestroy();
            _tEditor[_nActive]._bMouseMode = false;
        }
    }
}

void MainFrame::NextWindow() {
    if (!_bSplit && !g_tCfg.GetBool("Default", "NoSplit_NextWindow", false))
        return;

    _nActive = (_nActive + 1) % 2;
    _tEditor[0]._bFocus = _tMcd[0]._bFocus = _tPanel[0]._bFocus = _nActive ? false : true;
    _tEditor[1]._bFocus = _tMcd[1]._bFocus = _tPanel[1]._bFocus = _nActive ? true : false;
    _tCommand.SetPanel(&_tPanel[_nActive]);
    _tCommand.SetMcd(&_tMcd[_nActive]);
    _tCommand.SetEditor(&_tEditor[_nActive]);
    Refresh();
}

void MainFrame::Refresh(bool bNoOutRefresh) {
    if (_pWin) werase(_pWin);

    if (_tPanel[0].GetWin())
        werase(_tPanel[0].GetWin());

    if (_bSplit)
        if (_tPanel[1].GetWin())
            werase(_tPanel[1].GetWin());

    _tPanel[0]._bChange = true;
    _tPanel[1]._bChange = true;
    _bNoOutRefresh = bNoOutRefresh;

    Show();
}

///	Alt+C : Select -> selected file copy on the MCD
void MainFrame::Copy() {
    if (!_bSplit) {
        _eMcdClipCopy = CLIP_COPY;
        _tCommand.Execute("Cmd_ClipCopy");
        _tCommand.Execute("Cmd_MCD");
        _tCommand.Execute("Cmd_MountList");
    } else {
        bool bYn = YNBox(_("Copy"), _("Do you really want to select files copy int next panel directory?"));

        if (bYn) {
            _tCommand.Execute("Cmd_ClipCopy");
            _tCommand.Execute("Cmd_NextWindow");
            _tCommand.Execute("Cmd_ClipPaste");
        }
    }
}

///	Alt+X : Select -> selected file move on the MCD
void MainFrame::Move() {
    if (!_bSplit) {
        _eMcdClipCopy = CLIP_CUT;
        _tCommand.Execute("Cmd_ClipCut");
        _tCommand.Execute("Cmd_MCD");
        _tCommand.Execute("Cmd_MountList");
    } else {
        bool bYn = YNBox(_("Move"), _("Do you really want to select files move into next panel directory?"));

        if (bYn) {
            _tCommand.Execute("Cmd_ClipCut");
            _tCommand.Execute("Cmd_NextWindow");
            _tCommand.Execute("Cmd_ClipPaste");
        }
    }
}

void MainFrame::McdClipCopyClear() {
    _eMcdClipCopy = CLIP_NONE;
    _tSelection.Clear();
    _tPanel[_nActive]._bChange = true;
}

void MainFrame::Reload() {
    UpdateConfig();

    for (int n = 0; n < 2; n++) {
        _tPanel[n].Init();
        _tPanel[n].ConfigureLoad();
        _tMcd[n].Init();
        _tEditor[n].Init();
    }

    _tEditor[0]._bFocus = _tMcd[0]._bFocus = _tPanel[0]._bFocus = _nActive ? false : true;
    _tEditor[1]._bFocus = _tMcd[1]._bFocus = _tPanel[1]._bFocus = _nActive ? true : false;
    _tCommand.SetPanel(&_tPanel[_nActive]);
    _tCommand.SetMcd(&_tMcd[_nActive]);
    _tCommand.SetEditor(&_tEditor[_nActive]);
    _tCommand.Execute("Cmd_Refresh");
}

void MainFrame::SyncDirectory() {
    if (_bSplit) {
        if (_tPanel[0].GetReader()->GetReaderName() ==
            _tPanel[1].GetReader()->GetReaderName()) {
            string str = _tPanel[!_nActive].GetReader()->GetPath();
            _tPanel[_nActive].GetReader()->Read(str);
            _tCommand.Execute("Cmd_Refresh");
        }
    }
    return;
}

