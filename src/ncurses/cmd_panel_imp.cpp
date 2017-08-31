#include "cmd_panel_imp.h"
#include "drawset.h"
#include "mainframe.h"
#include "selection.h"
#include "mlsmenu.h"
#include "archive.h"
#include "ncurses_qcd.h"
#include "dialogbox.h"
#include "dialog_remoteconnprops.h"
#include "remotecfgload.h"
#include "mainframe_view.h"
#include "subshell.h"
#include <fcntl.h>

using namespace MLS;
using namespace MLSUTIL;

void CmdPanelImp::UpdateConfig() {
}

void CmdPanelImp::SaveConfig() {
}

///	@brief	명령어를 실행한다.
///	@param	cmd		명령어및 인자
///	@param	bPause	명령어를 실행시키고 종료시 잠시 정지시킬지 여부
/// @param	bFork	Fork할 것인지?
///	@return	명령어 실행 결과
int CmdPanelImp::Run(const std::string &cmd, bool bPause, bool bBackground) {
    // command 창에서 exit, quit 를 치면 종료하게 한다.
    if (cmd == "exit" || cmd == "quit") {
        Quit();
        return 0;
    }

    CursesDestroy();
    int status;

    if (_pPanel->GetReader()->GetReaderName() == "file") {
        chdir(_pPanel->GetReader()->GetPath().c_str());
    }

    if (bBackground) {
        cout << "linm $ " << cmd.c_str() << endl;
        string sCmd = cmd + " > /dev/null 2>&1 &";
        status = system(sCmd.c_str());
    } else {
        cout << "linm $ " << cmd.c_str() << endl;
        status = system(cmd.c_str());
    }


    if (bPause) {
        cbreak();
        noecho();

        cout << endl << _("Press any key to return LinM...") << '\r' << endl;
        getch();
    }

    CursesInit(g_tCfg.getBool("Default", "Transparency", false));
    Refresh();
    return 0;
}

///	@brief	모든 커맨드 메시지를 관리
///	@param	p	실행 시킬 명령어
///	@return		명령어 실행 결과
int CmdPanelImp::ParseAndRun(const string &p, bool Pause) {
    bool bFlag = false,
            bPause = Pause,
            bParam = false,
            bMcd = false,
            bConfirm = false,
            bBackground = false,
            bRootExe = false;

    // cd 구현
    if (p.substr(0, 2) == "cd") {
        string sStr;
        if (p == "cd" || p == "cd ")
            sStr = _pPanel->GetReader()->GetRealPath("~");
        else
            sStr = _pPanel->GetReader()->GetRealPath(p.substr(3));
        sStr = Replace(sStr, "\\", "");
        if (sStr != "") {
            if (_pPanel->Read(sStr) == true) {
                if (g_tMainFrame.GetScreenSync())
                    ViewSync();
                return 1;
            }
        }
        return 0;
    }

    string arg, tmp_name;

    File tFile;

    // 파일을 복사 해서 보여주기 위한.
    {
        File *pFile = _pPanel->GetCurFile();
        if (_pPanel->GetReader()->View(pFile, &tFile) == false) {
            //MsgBox(	_("Error"), _("file reading failure !!!!"));
            return 0;
        }
    }

    // 파싱
    for (string::const_iterator i = p.begin(); i != p.end(); i++) {
        if (!bFlag && *i == '%') {
            bFlag = true;
            continue;
        }

        if (bFlag) {
            switch (*i) {
                // 파일을 리턴해줌
                case '1':
                case 'F':
                    arg += addslash(tFile.sFullName);
                    break;

                case 'N': {
                    if (tFile.sName[0] == '.') return 0;

                    string::size_type p = tFile.sName.rfind('.');

                    if (p != string::npos)
                        arg += addslash(tFile.sFullName.substr(0, p - 1));
                    else
                        arg += addslash(tFile.sFullName);
                    break;
                }

                case 'E':
                    if (tFile.Ext().empty()) return 0;
                    arg += addslash(tFile.Ext());
                    break;

                case 'W':
                    bPause = true;
                    break;

                case '%':
                    arg += '%';
                    break;

                case 'S': {
                    Selection tSelection;
                    _pPanel->GetSelection(tSelection);

                    if (tSelection.GetSize() == 0) return 0;
                    bool first = true;

                    for (int t = 0; t < (int) tSelection.GetSize(); t++) {
                        if (first) first = false;
                        else arg += ' ';
                        arg += addslash(tSelection[t]->sFullName);
                    }
                    break;
                }

                case 's': {
                    Selection tSelection;
                    _pPanel->GetSelection(tSelection);

                    if (tSelection.GetSize() != 0) {
                        bool first = true;

                        for (int t = 0; t < (int) tSelection.GetSize(); t++) {
                            if (first) first = false;
                            else arg += ' ';

                            arg += addslash(tSelection[t]->sFullName);
                        }
                    } else {
                        if (tFile.sName == "..") return 0;
                        arg += addslash(tFile.sFullName);
                    }
                    break;
                }

                case 'd':
                    arg += addslash(_pPanel->GetPath());
                    break;

                    // MCD remove...
                case 'D':
                    bMcd = true;
                    break;

                case 'P':
                    bParam = true;
                    break;

                case 'R': // root 계정 실행시
                    bRootExe = true;
                    break;

                case 'H':
                    arg += addslash(_pPanel->GetReader()->GetRealPath("~"));
                    break;

                case 'Q':
                    bConfirm = true;
                    break;

                case 'B':
                    bBackground = true;
                    break;

                default:
                    break;
            }

            bFlag = false;
            continue;
        } else arg += *i;
    }

    if (bMcd) {
        /*
        Mcd mcd(_tPanel);
        mcd.setCur(_tPanel->GetCurrentPath());
        if (!mcd.proc())
            return 0;
        arg += addslash(mcd.getCur());
        */
    }

    if (bParam) if (InputBox(_("Edit Parameter"), arg) < 0) return 0;

    if (bConfirm)
        if (YNBox(_("Question"), _("Do you want to run this operation?"), false) != true) return 0;

    if (bRootExe)
        arg = "su - --command=\"" + arg + "\"";

    // 실행
    Run(arg, bPause, bBackground);
    return 0;
}

void CmdPanelImp::SplitViewSync() {
    g_tMainFrame.SetScreenSync(!g_tMainFrame.GetScreenSync());

    if (g_tMainFrame.GetScreenSync()) {
        NCurses_Panel *pPanel = g_tMainFrame.GetPanel(1);
        NCurses_Mcd *pMcd = g_tMainFrame.GetMcd(0);

        void *pWait = MsgWaitBox(_("Wait"),
                                 _("Please wait !!! - Cancel Key [ESC]"));
        pMcd->Destroy();
        pMcd->SetReader(pPanel->GetReader());
        pMcd->Rescan(2);
        pMcd->AddDirectory(pPanel->GetReader()->GetPath());
        pMcd->SetCur(pPanel->GetReader()->GetPath());
        MsgWaitEnd(pWait);
        ViewSync();
    }
}

void CmdPanelImp::ViewSync() {
    //int nActive = g_tMainFrame.GetActiveNum();

    NCurses_Panel *pPanel = g_tMainFrame.GetPanel(1);
    NCurses_Mcd *pMcd = g_tMainFrame.GetMcd(0);

    pMcd->AddDirectory(pPanel->GetReader()->GetPath());
    pMcd->SetCur(pPanel->GetReader()->GetPath());
    pPanel->_bFocus = true;
    pMcd->_bFocus = true;
    g_tMainFrame.Refresh(false);
}

void CmdPanelImp::Key_Enter() {
    if (g_tMainFrame.GetScreenSync()) {
        NCurses_Panel *pPanel = g_tMainFrame.GetPanel(1);
        NCurses_Mcd *pMcd = g_tMainFrame.GetMcd(0);

        Reader *pBefReader = pPanel->GetReader();
        pPanel->Key_Enter();

        if (pPanel->GetReader() != pBefReader) {
            pMcd->Destroy();
            pMcd->SetReader(pPanel->GetReader());
            pMcd->Rescan(2);
            pMcd->AddDirectory(pPanel->GetReader()->GetPath());
            pMcd->SetCur(pPanel->GetReader()->GetPath());
            pMcd->_bFocus = true;
        } else {
            pMcd->AddDirectory(pPanel->GetReader()->GetPath());
            pMcd->SetCur(pPanel->GetReader()->GetPath());
        }

        g_tMainFrame.Refresh(false);
    } else {
        _pPanel->Key_Enter();
        if (g_tCfg.getBool("Default", "AlwaysCleanRefresh"))
            Refresh();
    }
}

void CmdPanelImp::Key_Left() {
    _pPanel->Key_Left();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_Right() {
    _pPanel->Key_Right();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_Up() {
    _pPanel->Key_Up();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_Down() {
    _pPanel->Key_Down();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_Home() {
    _pPanel->Key_Home();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_End() {
    _pPanel->Key_End();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_PageUp() {
    _pPanel->Key_PageUp();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Key_PageDown() {
    _pPanel->Key_PageDown();
    if (g_tMainFrame.GetScreenSync()) ViewSync();
}

void CmdPanelImp::Mcd() {
    g_tMainFrame.SetScreenSync(false);

    void *pWait = MsgWaitBox(_("Wait"),
                             _("Please wait !!! - Cancel Key [ESC]"));

    _pMcd->Destroy();
    _pMcd->SetReader(_pPanel->GetReader());

    if (_pPanel->GetReader() && _pPanel->GetReader()->GetReaderName() != "archive") {
        string sPath = _pMcd->GetReader()->GetInitType();
        sPath = Replace(sPath, "/", "\\");
        string sLoadName = g_tCfg.getValue("Static", "CfgHome") + "McdDirSave/" +
                           sPath;

        if (!_pMcd->Load(sLoadName.c_str()))
            _pMcd->Rescan(2);
    } else {
        _pMcd->Rescan(2);
    }

    _pMcd->AddDirectory(_pPanel->GetReader()->GetPath());
    _pMcd->SetCur(_pPanel->GetReader()->GetPath());

    MsgWaitEnd(pWait);
    g_tMainFrame.SetActiveViewType(MCD);
    _pMcd->_bFocus = true;
}

void CmdPanelImp::Editor() {
    File *pFile = _pPanel->GetCurFile();

    if (pFile == NULL || pFile->bDir == true) {
        MsgBox(_("Error"), _("Don't directory edit."));
        return;
    }

    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    if (g_tCfg.getBool("Default", "Use_LinM_Editor", true))
        EditorChoice(false);
    else
        EditorChoice(false, g_tCfg.getValue("Default", "ExtEditor", "vi"));
}

void CmdPanelImp::ExtEditor() {
    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    EditorChoice(false, g_tCfg.getValue("Default", "ExtEditor", "vi"));
}

void CmdPanelImp::EditorChoice(bool bReadOnly, const string &sEditorCmd, File *pFile) {
    File tFile;
    bool bExtEditor = false;

    if (!pFile) {
        pFile = _pPanel->GetCurFile();
        if (!pFile) return;

        if (_pPanel->GetReader()->View(pFile, &tFile) == false) return;
    } else
        tFile = *pFile;

    if (!sEditorCmd.empty()) bExtEditor = true;

    if (!bExtEditor) {
        g_tMainFrame.SetScreenSync(false);

        void *pWait = MsgWaitBox(_("Wait"),
                                 _("Please wait !!! - Cancel Key [ESC]"));

        int nSize = 0;
        string sSize = g_tCfg.getValue("Default", "EditorTabSize");
        if (sSize.size() == 0) nSize = 8;
        else nSize = atoi(sSize.c_str());

        bool bEditorLineNum = g_tCfg.getBool("Default", "EditorLineNum");
        bool bEditorBackup = g_tCfg.getBool("Default", "EditorBackup");

        const string &sReaderName = _pPanel->GetReader()->GetReaderName();

        if (bReadOnly == false)
            bReadOnly = (sReaderName == "archive") ? true : false;

        // title 은 원본을 보여준다.
        if (pFile) {
            string sTitle = sReaderName + ":/" + isKorCode(pFile->sFullName);
            _pEditor->SetViewTitle(sTitle);
        } else {
            string sTitle = isKorCode(tFile.sName);
            _pEditor->SetViewTitle(sTitle);
        }

        _pEditor->SetEditor(nSize, bEditorBackup, bEditorLineNum);

        if (_pEditor->Load(&tFile, bReadOnly) == false) {
            MsgWaitEnd(pWait);
            MsgBox(_("Error"), _("File loading failure."));
        } else {
            g_tMainFrame.SetActiveViewType(EDITOR);
            MsgWaitEnd(pWait);
            _pEditor->_bFocus = true;

            g_tMainFrame.Refresh();

            if (_pEditor->_bFullScreen) {
                MouseDestroy();
                _pEditor->_bMouseMode = false;
            }
        }
    } else {
        LOG_WRITE("ParseAndRun !!! [%s]", sEditorCmd.c_str());

        if (sEditorCmd.empty())
            ParseAndRun("vi %F");
        else
            ParseAndRun(sEditorCmd + " %F");

        if (!pFile) {
            if (_pPanel->GetReader()->GetReaderName() == "sftp" ||
                _pPanel->GetReader()->GetReaderName() == "ftp" ||
                _pPanel->GetReader()->GetReaderName() == "smb") {
                string sPath = tFile.sFullName.substr(0, tFile.sFullName.size() - tFile.sName.size());

                Selection tSelection;
                tSelection.Add(&tFile);
                tSelection.SetSelectPath(sPath);
                _pPanel->GetReader()->Paste(tSelection);

                remove(tFile.sFullName.c_str());
            }
        }

        ReloadConfigChange();
    }
    return;
}

void CmdPanelImp::ClearRefresh() {
    ScreenClear();
    Refresh();
}

void CmdPanelImp::Refresh() {
    string sFileName;

    for (int n = 0; n < 2; n++) {
        if (g_tMainFrame.GetPanel(n)->GetCurFile() != NULL)
            if (g_tMainFrame.GetPanel(n)->GetCurFile()->sName != "..")
                sFileName = g_tMainFrame.GetPanel(n)->GetCurFile()->sFullName;

        g_tMainFrame.GetPanel(n)->Read(g_tMainFrame.GetPanel(n)->GetPath());

        if (!sFileName.empty())
            g_tMainFrame.GetPanel(n)->SetCurFileName(sFileName);
    }
    g_tMainFrame.Refresh();
}

void CmdPanelImp::Menu() {
    MlsMenu tMlsMenu;
    vector<string> vDisableList;

    const string &sReaderName = _pPanel->GetReader()->GetReaderName();

    if (sReaderName == "file") {
        vDisableList.push_back("Cmd_RemoteClose");
    } else if (sReaderName == "ftp" || sReaderName == "sftp" || sReaderName == "smb") {
        vDisableList.push_back("Cmd_RemoteConnect");
        vDisableList.push_back("Cmd_RemoteConnProps");
    } else {
        vDisableList.push_back("Cmd_RemoteConnect");
        vDisableList.push_back("Cmd_RemoteConnProps");
        vDisableList.push_back("Cmd_RemoteClose");
    }

    tMlsMenu.SetDisable(vDisableList);
    tMlsMenu.SetPanel(_pPanel);
    tMlsMenu.Create();
    tMlsMenu.Do();

    g_tMainFrame.Refresh(false);
    if (tMlsMenu.GetCurCmd().size() != 0) {
        LOG_WRITE("Menu Run [%s]", tMlsMenu.GetCurCmd().c_str());
        g_tMainFrame.GetCommand()->Execute(tMlsMenu.GetCurCmd());
    }
}

void CmdPanelImp::Quit() {
    if (_pPanel->GetReader()->GetReaderName() == "sftp" ||
        _pPanel->GetReader()->GetReaderName() == "ftp" ||
        _pPanel->GetReader()->GetReaderName() == "smb") {
        RemoteClose();
        return;
    }

    if (g_tCfg.getBool("Default", "AskMlsExit") == true) {
        bool bYN = YNBox(_("LinM Quit"), _("Do you really want to quit the LinM ?"), true);
        if (bYN == false) return;
    }

    // 마지막 mls 가 마지막으로 사용한 디렉토리를 저장한다.
    if (_pPanel->GetReader()->GetReaderName() == "file") {
        FILE *fp = fopen((g_tCfg.getValue("Static", "CfgHome") + "path").c_str(), "wb");

        if (fp) {
            fputs(_pPanel->GetPath().c_str(), fp);
            fclose(fp);
        }
    }

    g_tMainFrame.SaveConfig(); // 이때 Save
    g_tMainFrame.Exit();
}

void CmdPanelImp::Split() {
    g_tMainFrame.Split();
}

void CmdPanelImp::NextWindow() {
    g_tMainFrame.NextWindow();
}

void CmdPanelImp::ArchiveFileView() {
    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    File *pFile = _pPanel->GetCurFile();

    LOG_WRITE("ArchiveFileView [%s]", pFile->sFullName.c_str());
    if (pFile->sType.substr(0, 7) == "archive")
        MsgBox(_("Error"), "Archive file view failure !!!");
    else {
        if (_pPanel->GetReader()->GetReaderName() != "file")
            _pPanel->PluginOpen(g_tCfg.getValue("Static", "TmpDir") + pFile->sName, "archive");
        else
            _pPanel->PluginOpen(pFile->sFullName, "archive");
    }
}

void CmdPanelImp::ArchiveFileMake() {
    if (g_tMainFrame.GetActiveViewType() != PANEL) return;
}

void CmdPanelImp::RemoteConnect() {
    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("current remote connected."));
        return;
    }

    string sConnectionInfo;

    sConnectionInfo = _Config.getValue("Default", "LastRemoteConnected");

    if (InputBox(_("Input sftp,ftp,smb connect url (ex. sftp://user:pswd@hostname)"),
                 sConnectionInfo) == ERROR)
        return;

    if (sConnectionInfo.substr(0, 6) == "ftp://") {
        sConnectionInfo = sConnectionInfo.substr(6);
        _pPanel->PluginOpen(sConnectionInfo, "ftp");
    } else if (sConnectionInfo.substr(0, 7) == "sftp://") {
        sConnectionInfo = sConnectionInfo.substr(7);
        _pPanel->PluginOpen(sConnectionInfo, "sftp");
    } else if (sConnectionInfo.substr(0, 6) == "smb://") {
        //sConnectionInfo = sConnectionInfo.substr(6);
        _pPanel->PluginOpen(sConnectionInfo, "smb");
    } else
        MsgBox(_("Error"), _("input url invalid format."));

    if (_pPanel->GetReader()->GetReaderName() == "ftp" ||
        _pPanel->GetReader()->GetReaderName() == "sftp" ||
        _pPanel->GetReader()->GetReaderName() == "smb") {
        _Config.setValue("Default",
                         "LastRemoteConnected",
                         _pPanel->GetReader()->GetInitType());
    }
    g_tMainFrame.Refresh();
}

void CmdPanelImp::RemoteClose() {
    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    if (g_tCfg.getBool("Default", "AskRemoteExit") == true) {
        bool bYN = YNBox(_("Remote Close"), _("Do you really want to remote connection close ?"), true);
        if (bYN == false) return;
    }

    _pPanel->PluginClose("file");
    Refresh();
}

void CmdPanelImp::Remove() {
    Selection &tMainSelection = *(g_tMainFrame.GetSelection());
    tMainSelection.Clear();
    _pPanel->GetSelection(tMainSelection);
    tMainSelection.ExpandDir(_pPanel->GetReader(), false);

    string sFileName;
    File *pNextFile = _pPanel->GetNextFile();

    if (pNextFile) {
        if (pNextFile->sName != "..")
            sFileName = pNextFile->sFullName;
    }

    _pPanel->GetReader()->Remove(tMainSelection);
    Refresh();

    if (!sFileName.empty())
        _pPanel->SetCurFileName(sFileName);
}

void CmdPanelImp::ClipCopy() {
    Selection &tMainSelection = *(g_tMainFrame.GetSelection());
    tMainSelection.Clear();
    _pPanel->GetSelection(tMainSelection);
    tMainSelection.SetSelectPath(_pPanel->GetReader()->GetPath());
    tMainSelection.SetReader(_pPanel->GetReader());

    if (tMainSelection.GetSize() == 0)
        g_tMainFrame.GetClip()->Set(CLIP_NONE);
    else
        g_tMainFrame.GetClip()->Set(CLIP_COPY);

    LOG_WRITE("ClipCopy [%s]", _pPanel->GetReader()->GetPath().c_str());
}

void CmdPanelImp::ClipCut() {
    Selection &tMainSelection = *(g_tMainFrame.GetSelection());
    tMainSelection.Clear();
    _pPanel->GetSelection(tMainSelection);
    tMainSelection.SetSelectPath(_pPanel->GetReader()->GetPath());
    tMainSelection.SetReader(_pPanel->GetReader());

    if (tMainSelection.GetSize() == 0)
        g_tMainFrame.GetClip()->Set(CLIP_NONE);
    else
        g_tMainFrame.GetClip()->Set(CLIP_CUT);

    LOG_WRITE("ClipCut [%s]", _pPanel->GetReader()->GetPath().c_str());
}

void CmdPanelImp::ClipPaste() {
    ClipState state = g_tMainFrame.GetClip()->GetState();

    if (state == CLIP_NONE) return;

    if (state == CLIP_COPY) {
        CopyPaste();
    } else if (state == CLIP_CUT) {
        CutPaste();
    }
    Refresh();
}

void CmdPanelImp::CopyPaste() {
    Selection &_tMainSelection = *(g_tMainFrame.GetSelection());

    if (_tMainSelection.GetSize() <= 0) return;

    Selection tSelectionCopy;

    LOG_WRITE("ClipPaste Copy Start 1 - [%d] [%s] [%s] !!!",
              _tMainSelection.GetSize(),
              _tMainSelection[0]->sType.c_str(),
              _pPanel->GetReader()->GetInitType().c_str());

    // Source 나 Target이 file이면 그대로 이용한다.
    if (_tMainSelection[0]->sType == "file://" &&
        _pPanel->GetReader()->GetInitType() == "file://") {
        _tMainSelection.ExpandDir(_tMainSelection.GetReader(), false);

        // TmpDir 에 복사하고 나서 옮긴다.
        LOG_WRITE("ClipPaste Copy Start !!!");
        _pPanel->GetReader()->Copy(_tMainSelection);
        LOG_WRITE("ClipPaste Copy End !!!");
    } else {
        bool bUseGlobalReaderCtl = false;

        // 패널이 두개이기 때문에 mainframe 의 panel에서 Type에 맞는 데이터를 읽어 온다.
        Reader *pReader = g_tMainFrame.GetPanel(0)->GetReader(_tMainSelection[0]->sType);
        if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType) {
            pReader = g_tMainFrame.GetPanel(1)->GetReader(_tMainSelection[0]->sType);

            if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType) {
                // 그래도 없으면 만들서 넣어야 함.
                string::size_type p = _tMainSelection[0]->sType.find("://");
                string sInitName;

                if (p == string::npos)
                    sInitName = _tMainSelection[0]->sType;
                else
                    sInitName = _tMainSelection[0]->sType.substr(0, p);

                // 이때는 g_tReaderCtl로 읽어 들여옴.
                pReader = g_tReaderCtl.Get(sInitName);
                if (pReader == NULL) {
                    MsgBox(_("Error"), _("plugin not found."));
                    return;
                }

                LOG_WRITE("g_tReaderCtl USE [%p] [%s]", pReader, sInitName.c_str());

                bUseGlobalReaderCtl = true;
                if (p == string::npos)
                    sInitName = _tMainSelection[0]->sType;
                else
                    sInitName = _tMainSelection[0]->sType.substr(p + 3);

                LOG_WRITE("g_tReaderCtl USE [%p] [%s]", pReader, sInitName.c_str());

                LOG_WRITE("%s", pReader->GetReaderName().c_str());
                pReader->Init(sInitName);

                if (pReader->GetConnected() == false) {
                    pReader->Destroy();
                }
                LOG_WRITE("g_tReaderCtl Init [%p] [%s]", pReader, sInitName.c_str());
            }
        }

        if (pReader->GetConnected() == false) {
            pReader->Init(_tMainSelection[0]->sType);
            if (pReader->GetConnected() == false) return;
        }

        // 압축은 디렉토리를 선택하면 디렉토리안의 내용도 같이 압축해제 하기 때문에.
        // 하위디렉토리까지 검사하지 않는다. 하위디렉토리 검사하면 시간이 더걸림.

        if (pReader->GetReaderName() == "archive") {
            Archive tArchive(pReader->GetInitType());
            //int		nType = tArchive.GetCurZipType();

            //if ( nType == ZIP || nType == ALZ || nType == RPM)
            _tMainSelection.ExpandDir(pReader, false);
        } else
            _tMainSelection.ExpandDir(pReader, false);

        if (g_tCfg.getValue("Static", "TmpCopyDir") != "") {
            string sTmpDel = "rm -rf " + g_tCfg.getValue("Static", "TmpCopyDir") + "*";
            system(sTmpDel.c_str());
        }

        // TmpDir 에 복사하고 나서 옮긴다.
        if (pReader->Copy(_tMainSelection,
                          g_tCfg.getValue("Static", "TmpCopyDir"),
                          &tSelectionCopy)) {
            LOG_WRITE("File Copy Complite !!!");

            if (_pPanel->GetReader()->GetInitType() == "file://") {
                LOG_WRITE("File Copy Move !!!");

                // 파일일 경우는 이동한다.
                _pPanel->GetReader()->Move(tSelectionCopy);
            } else {
                LOG_WRITE("File Copy TmpDir Remove !!!");

                // 파일이 아닌 경우는 /tmp에서 다른 곳으로 복사 한 후
                // /tmp에 있는 내용은 지운다.
                if (_pPanel->GetReader()->Paste(tSelectionCopy))
                    if ((pReader = _pPanel->GetReader("file://")) != NULL)
                        pReader->Remove(tSelectionCopy, false);
            }
        }

        if (bUseGlobalReaderCtl == true)
            pReader->Destroy();
    }
}

void CmdPanelImp::CutPaste() {
    Selection &_tMainSelection = *(g_tMainFrame.GetSelection());

    if (_tMainSelection.GetSize() <= 0) return;

    LOG_WRITE("ClipPaste CLIP_CUT Start - [%d] [%s] [%s] !!!",
              _tMainSelection.GetSize(),
              _tMainSelection[0]->sType.c_str(),
              _pPanel->GetReader()->GetInitType().c_str());

    if (_tMainSelection[0]->sType == "file://" && _pPanel->GetReader()->GetInitType() == "file://") {
        _tMainSelection.ExpandDir(_tMainSelection.GetReader(), false);
        // sType이 file 이면 그냥 옮긴다.
        _pPanel->GetReader()->Move(_tMainSelection);
    } else {
        bool bUseGlobalReaderCtl = false;

        // 패널이 두개이기 때문에 mainframe 의 panel에서 Type에 맞는 데이터를 읽어 온다.
        Reader *pReader = g_tMainFrame.GetPanel(0)->GetReader(_tMainSelection[0]->sType);
        if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType) {
            pReader = g_tMainFrame.GetPanel(1)->GetReader(_tMainSelection[0]->sType);
            if (pReader == NULL || pReader->GetInitType() != _tMainSelection[0]->sType) {
                // 그래도 없으면 만들서 넣어야 함.
                string::size_type p = _tMainSelection[0]->sType.find("://");
                string sInitName;

                if (p != string::npos)
                    sInitName = _tMainSelection[0]->sType;
                else
                    sInitName = _tMainSelection[0]->sType.substr(0, p);

                // 이때는 g_tReaderCtl로 읽어 들여옴.
                pReader = g_tReaderCtl.Get(sInitName);
                if (pReader == NULL) {
                    MsgBox(_("Error"), _("plugin not found."));
                    return;
                }

                bUseGlobalReaderCtl = true;
                if (p == string::npos)
                    sInitName = _tMainSelection[0]->sType;
                else
                    sInitName = _tMainSelection[0]->sType.substr(p + 3);

                pReader->Init(sInitName);
                if (pReader->GetConnected() == false) return;
            }
        }

        if (pReader->GetConnected() == false) {
            pReader->Init(_tMainSelection[0]->sType);
            if (pReader->GetConnected() == false) return;
        }

        // 압축은 디렉토리를 선택하면 디렉토리안의 내용도 같이 압축해제 하기 때문에.
        // 하위디렉토리까지 검사하지 않는다. 하위디렉토리 검사하면 시간이 더걸림.
        if (pReader->GetReaderName() != "archive")
            _tMainSelection.ExpandDir(pReader, false);

        Selection tSelectionMove;

        if (g_tCfg.getValue("Static", "TmpCopyDir") != "") {
            string sTmpDel = "rm -rf " + g_tCfg.getValue("Static", "TmpCopyDir") + "*";
            system(sTmpDel.c_str());
        }

        // TmpDir 에 복사하고 나서 옮긴다.
        if (pReader->Copy(_tMainSelection,
                          g_tCfg.getValue("Static", "TmpCopyDir"),
                          &tSelectionMove)) {
            Reader *pCurReader = _pPanel->GetReader();
            if (pCurReader->GetInitType() == "file://") {
                // 파일일 경우는 이동한다.
                if (pCurReader->Move(tSelectionMove))
                    pCurReader->Remove(_tMainSelection, false); // 이동하고 원본 삭제
            } else {
                // 파일이 아닌 경우는 TmpDir 에서 다른 곳으로 복사 한 후
                // TmpDir 에 있는 내용은 지운다.
                if (pCurReader->Paste(tSelectionMove))
                    if ((pReader = _pPanel->GetReader("file://")) != NULL) {
                        pReader->Remove(tSelectionMove, false);    // Tmp 디렉토리 지움.
                        pCurReader->Remove(_tMainSelection, false); // 원본 삭제
                    }
            }
        }

        if (bUseGlobalReaderCtl == true)
            pReader->Destroy();
    }
}

void CmdPanelImp::Key_ESC() {
    if (g_tMainFrame.GetScreenSync()) {
        g_tMainFrame.SetScreenSync(false);
        return;
    }

    if (_pPanel->GetReader()->GetReaderName() != "file") {
        if (_pPanel->GetReader()->GetReaderName() == "sftp" ||
            _pPanel->GetReader()->GetReaderName() == "ftp") {
            RemoteClose();
            return;
        }
        _pPanel->PluginClose("file");
    } else {
        CursesDestroy();
        cbreak();
        noecho();
        getch();
        CursesInit(g_tCfg.getBool("Default", "Transparency", false));
    }
    g_tMainFrame.Refresh(false);
}

void CmdPanelImp::Shell() {
    if (_pPanel->GetReader()->GetInitType() == "file://")
        g_tMainFrame.CmdShell(true);
    else
        MsgBox("Error", "shell is local file system use only.");
}

void CmdPanelImp::Rename() {
    File *pFile = _pPanel->GetCurFile();
    _pPanel->GetReader()->Rename(pFile);
    string sName = pFile->sFullName;
    _pPanel->SetCurFileName(sName);
    Refresh();
}

void CmdPanelImp::Mkdir() {
    _pPanel->GetReader()->Mkdir();
    Refresh();
}

void CmdPanelImp::GoRoot() {
    _pPanel->Read("/");
    if (g_tMainFrame.GetScreenSync()) ViewSync();
    Refresh();
}

void CmdPanelImp::GoHome() {
    _pPanel->Read("~");
    if (g_tMainFrame.GetScreenSync()) ViewSync();
    Refresh();
}

void CmdPanelImp::GoParent() {
    _pPanel->Read("..");
    if (g_tMainFrame.GetScreenSync()) ViewSync();
    Refresh();
}

void CmdPanelImp::HiddenFileView() {
    LOG_WRITE("_pPanel->GetHiddenView() [%s]", _pPanel->GetHiddenView() ? "TRUE" : "FALSE");
    _pPanel->SetHiddenView();
    Refresh();
}

void CmdPanelImp::FileOwnerView() {
    LOG_WRITE("_pPanel->GetHiddenView() [%s]", _pPanel->GetHiddenView() ? "TRUE" : "FALSE");
    _pPanel->SetViewOwner(!_pPanel->GetViewOwner());
    Refresh();
}

// sort
void CmdPanelImp::SortChange() {
    int nSort = _pPanel->GetSortMethod() + 1;
    if (nSort >= SORT_END_) nSort = 0;
    switch (nSort) {
        case SORT_NONE:
            SortNone();
            break;
        case SORT_NAME:
            SortName();
            break;
        case SORT_EXT:
            SortExt();
            break;
        case SORT_SIZE:
            SortSize();
            break;
        case SORT_TIME:
            SortTime();
            break;
        case SORT_COLOR:
            SortColor();
            break;
    }
}

void CmdPanelImp::SortAscDescend() {
    int nSort = _pPanel->GetSortMethod();
    if (nSort != SORT_NONE) {
        _pPanel->_bFileSortAscend = !_pPanel->_bFileSortAscend;
        _pPanel->Sort();
    }
}

void CmdPanelImp::SortNone() {
    _pPanel->SetSortMethod(SORT_NONE);
    _pPanel->Sort();
}

void CmdPanelImp::SortName() {
    _pPanel->SetSortMethod(SORT_NAME);
    _pPanel->Sort();
}

void CmdPanelImp::SortExt() {
    _pPanel->SetSortMethod(SORT_EXT);
    _pPanel->Sort();
}

void CmdPanelImp::SortSize() {
    _pPanel->SetSortMethod(SORT_SIZE);
    _pPanel->Sort();
}

void CmdPanelImp::SortTime() {
    _pPanel->SetSortMethod(SORT_TIME);
    _pPanel->Sort();
}

void CmdPanelImp::SortColor() {
    _pPanel->SetSortMethod(SORT_COLOR);
    _pPanel->Sort();
}

void CmdPanelImp::ColumnAuto() {
    _pPanel->SetViewColumn(0);
}

void CmdPanelImp::Column1() {
    _pPanel->SetViewColumn(1);
}

void CmdPanelImp::Column2() {
    _pPanel->SetViewColumn(2);
}

void CmdPanelImp::Column3() {
    _pPanel->SetViewColumn(3);
}

void CmdPanelImp::Column4() {
    _pPanel->SetViewColumn(4);
}

void CmdPanelImp::DefaultCfgFileChg() {
    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("Don't execute remote connect or archive file view."));
        return;
    }

    File tFile;
    tFile.sFullName = g_tCfg.getValue("Static", "CfgHome") + "default.cfg";
    tFile.sName = "default.cfg";
    EditorChoice(false, "", &tFile);
    _pEditor->_bConfChg = true;
}

void CmdPanelImp::ColorCfgFileChg() {
    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("Don't execute remote connect or archive file view."));
        return;
    }
    File tFile;
    tFile.sFullName = g_tCfg.getValue("Static", "CfgHome") + "colorset.cfg";
    tFile.sName = "colorset.cfg";
    EditorChoice(false, "", &tFile);
    _pEditor->_bConfChg = true;
}

void CmdPanelImp::KeyCfgFileChg() {
    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("Don't execute remote connect or archive file view."));
        return;
    }

    File tFile;
    tFile.sFullName = g_tCfg.getValue("Static", "CfgHome") + "keyset.cfg";
    tFile.sName = "keyset.cfg";
    EditorChoice(false, "", &tFile);
    _pEditor->_bConfChg = true;
}

void CmdPanelImp::Copy() {
    g_tMainFrame.Copy();
}

void CmdPanelImp::Move() {
    g_tMainFrame.Move();
}

void CmdPanelImp::MountList() {
    _pPanel->MountList();
}

void CmdPanelImp::SizeInfo() {
    Selection tSelection;
    _pPanel->GetSelection(tSelection);

    void *pWin = MsgWaitBox(_("Wait"),
                            _("Please wait !!! - Cancel Key [ESC]"));

    tSelection.ExpandDir(_pPanel->GetReader(), true);

    MsgWaitEnd(pWin);

    String sMsg;
    sMsg.append(_("  Selected files     %10s  /  %10s byte  "),
                toregular(tSelection.GetSize()).c_str(),
                toregular(tSelection.CalcSize()).c_str());

    MsgBox(_("Size info"), sMsg.c_str());
}

void CmdPanelImp::Forward() {
    _pPanel->Forward();
}

void CmdPanelImp::Back() {
    _pPanel->Back();
}

void CmdPanelImp::NewFile() {
    if (_pPanel->GetReader()->GetReaderName() == "archive") {
        MsgBox("Error", "Don't new file in archive.");
        return;
    }

    string sFilename;
    if (InputBox("Input new file name.", sFilename) == -1) return;
    if (sFilename.size() == 0) return;

    if (_pPanel->GetReader()->GetReaderName() == "file")
        sFilename = _pPanel->GetReader()->GetPath() + sFilename;
    else
        sFilename = g_tCfg.getValue("Static", "TmpDir") + sFilename;

    LOG_WRITE("NewFile !!! [%s]", sFilename.c_str());

    bool bUseEditor = g_tCfg.getBool("Default", "Use_LinM_Editor");

    if (!bUseEditor) {
        string sEditorCmd = g_tCfg.getValue("Default", "ExtEditor");
        ParseAndRun(sEditorCmd + " " + sFilename);
        Refresh();
        return;
    }

    int nSize = 0;
    string sSize = g_tCfg.getValue("Default", "EditorTabSize");
    if (sSize.size() == 0) nSize = 8;
    else nSize = atoi(sSize.c_str());

    bool bEditorLineNum = g_tCfg.getBool("Default", "EditorLineNum");
    bool bEditorBackup = g_tCfg.getBool("Default", "EditorBackup");

    // title 은 원본을 보여준다.
    string sTitle = _pPanel->GetReader()->GetReaderName() + ":/" + sFilename;

    _pEditor->SetViewTitle(sTitle);
    _pEditor->SetEditor(nSize, bEditorBackup, bEditorLineNum);
    _pEditor->New(sFilename);

    g_tMainFrame.SetActiveViewType(EDITOR);
    _pEditor->_bFocus = true;

    g_tMainFrame.Refresh();

    if (_pEditor->_bFullScreen) {
        MouseDestroy();
        _pEditor->_bMouseMode = false;
    }
}

void CmdPanelImp::TouchFile() {
    if (_pPanel->GetReader()->GetReaderName() == "archive") {
        MsgBox("Error", "Don't touch file in archive.");
        return;
    }

    string sFilename;
    if (InputBox("Input touch file name.", sFilename) == -1) return;
    if (sFilename.size() == 0) return;

    if (_pPanel->GetReader()->GetReaderName() == "file")
        sFilename = _pPanel->GetReader()->GetPath() + sFilename;
    else
        sFilename = g_tCfg.getValue("Static", "TmpDir") + sFilename;

    sFilename = "touch " + sFilename;
    system(sFilename.c_str());
    Refresh();
}

/// @brief    각 type 에 맞게 현재 선택된 파일을 압축해준다.
///    @param    nType    0 : tar.gz, 1 : tar.bz2, 2 : zip
void CmdPanelImp::FileCompress(int nType, const string &sName) {
    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox("Error", "local file system use only.");
        return;
    }

    string sTitle = _("Input name of the compressed file. [filename]");
    string sFileName = sName;

    switch (nType) {
        case 0:
            InputBox((sTitle + ".tar.gz").c_str(), sFileName);
            if (sFileName == "") return;
            sFileName = sFileName + ".tar.gz";
            break;
        case 1:
            InputBox((sTitle + ".tar.bz2").c_str(), sFileName);
            if (sFileName == "") return;
            sFileName = sFileName + ".tar.bz2";
            break;
        case 2:
            InputBox((sTitle + ".zip").c_str(), sFileName);
            if (sFileName == "") return;
            sFileName = sFileName + ".zip";
            break;
        default:
            return;
            break;
    }

    sFileName = _pPanel->GetReader()->GetPath() + sFileName;

    if (access(sFileName.c_str(), R_OK) == 0) {
        MsgBox(_("Error"), _("input file used."));
        return;
    }

    if (access(_pPanel->GetReader()->GetPath().c_str(), W_OK) == -1) {
        MsgBox(_("Error"), strerror(errno));
        return;
    }

    string sView = _("Please wait !!! - Compress file [") + sFileName + "] - Cancel Key [Ctrl+C]";

    void *pWin = MsgWaitBox(_("Wait"),
                            sView);

    Selection tSelection;
    _pPanel->GetSelection(tSelection);

    vector<File *> tFileList = tSelection.GetData();

    SetKeyBreakUse(true);

    Archive tArchive(sFileName);
    tArchive.Compress(tFileList, TAR_APPEND, _pPanel->GetReader()->GetPath());

    SetKeyBreakUse(false);

    MsgWaitEnd(pWin);
    Refresh();
}

void CmdPanelImp::ViewClip() {
    void *pWait = MsgWaitBox(_("Wait"),
                             _("Please wait !!! - Cancel Key [ESC]"));

    Selection tSelection;
    _pPanel->GetSelection(tSelection);
    tSelection.ExpandDir(_pPanel->GetReader(), true);

    MsgWaitEnd(pWait);
    Refresh();

    vector<File *> tFileList = tSelection.GetData();
    vector<string> tViewList;

    String sSize;

    sSize.append(_("Selected files     %10s  /  %10s byte  "),
                 toregular(tSelection.GetSize()).c_str(),
                 toregular(tSelection.CalcSize()).c_str());

    tViewList.push_back(sSize.c_str());
    tViewList.push_back("");

    string sCurPath = _pPanel->GetPath();

    for (uint n = 0; n < tFileList.size(); n++) {
        File *pFile = tFileList[n];

        String sData;
        string sName;

        if (pFile->sFullName.substr(0, sCurPath.size()) == sCurPath)
            sName = "./" + pFile->sFullName.substr(sCurPath.size());
        else
            sName = pFile->sFullName;

        if (sName.size() > 51) {
            string sCutName = sName.substr(sName.size() - 50, 50);
            sData.appendBlank(50, sCutName.c_str());
        } else
            sData.appendBlank(50, sName.c_str());

        if (pFile->uSize >= 10000000)
            sData.append("%9.1fM", (float) pFile->uSize / 1048576);
        else
            sData.append("%10s", toregular(pFile->uSize).c_str());

        tViewList.push_back(sData.c_str());
    }

    TextListBox(_("Clipboard View"), tViewList, true);
    Refresh();
}

void CmdPanelImp::Extract() {
    if (_pPanel->GetReader()->GetReaderName() == "archive") {
        MsgBox("Error", "Don't touch file in archive.");
        return;
    }

    LOG_WRITE("Extract !!!");

    File tFile;
    if (_pPanel->GetReader()->View(_pPanel->GetCurFile(), &tFile) == false) return;

    string sMsg = _("Compress the select files.");

    vector<string> q;

    Archive tArchive(tFile.sFullName);
    int nFileinfo = tArchive.GetCurZipType();

    if (_pPanel->GetReader()->GetReaderName() == "file") {
        q.push_back(sMsg + "(tar.gz)");
        q.push_back(sMsg + "(tar.bz2)");
        q.push_back(sMsg + "(zip)");

        if (nFileinfo == ERROR) {
            q.push_back(_("Cancel"));
            LOG_WRITE("Extract() !!!");
            int nSelect = SelectBox(sMsg.c_str(), q, 0);
            if (nSelect == -1) return;

            FileCompress(nSelect, tFile.sName);
            return;
        }
    }

    string sInsertMsg;
    sInsertMsg = sInsertMsg + _("Current file install.") + "[" + tFile.sName + "]";

    q.insert(q.begin(), sInsertMsg);
    q.push_back(_("Cancel"));


    int nSelect = SelectBox(sMsg.c_str(), q, 0);
    if (nSelect == -1) return;

    if (_pPanel->GetReader()->GetReaderName() == "file" && nFileinfo == SUCCESS) {
        if (nSelect != 0) {
            FileCompress(nSelect - 1, tFile.sName);
            return;
        }
    } else {
        if (nSelect == (int) q.size() - 1) return;
    }

    int nZipType = tArchive.GetCurZipType();

    string sFilename = tFile.sFullName;

    switch (nZipType) {
        case TAR_GZ:
        case TAR_BZ:
        case ZIP:
        case ALZ:
        case TAR: {
            g_tMainFrame.SetMcdExeMode(ARCHIVE_COPY, tFile.sFullName);
            Mcd();
            break;
        }

        case RPM: {
            vector<string> q;
            q.push_back(_("Install"));
            q.push_back(_("Install Nodeps"));
            q.push_back(_("Cancel"));

            int nSelect = SelectBox(tFile.sName.c_str(), q, 0);

            string sCommand;

            switch (nSelect) {
                case 0:    // root 계정으로 실행
                    sCommand = "rpm -Uvh " + sFilename + " %R %W";
                    break;
                case 1: // root 계정으로 실행
                    sCommand = "rpm -Uvh --nodeps " + sFilename + " %R %W";
                    break;
                case 2:
                    return;
                    break;
                default:
                    return;
            }

            ParseAndRun(sCommand, true);
            break;
        }

        case DEB: {
            string sMsg = _("Do you want install current file ?");
            sMsg = sMsg + "[" + tFile.sName + "]";

            if (YNBox(_("Question"), sMsg.c_str(), false) != true) {
                string sCommand = "dpkg -i " + sFilename + " %R %W";
                ParseAndRun(sCommand, true);
            }
            break;
        }

        default:
            return;
    }
    Refresh();
}

void CmdPanelImp::Execute() {
    _pPanel->SelectExecute();
}

void CmdPanelImp::QCD() {
    Qcd tQCD;
    tQCD.Size(14, 50);
    tQCD.Move(MIDDLE, MIDDLE);
    tQCD.Do();

    if (tQCD.GetNum() != -1) {
        if (g_tMainFrame.GetMcdExeMode().eMcdExeMode == MCD_EXEMODE_NONE) {
            _pPanel->GetReader()->Read(tQCD.GetData());
            Refresh();
        } else {
            Mcd();
            Refresh();
        }
    }
}

void ModeToBool(mode_t tMode, vector<bool> &vBool) {
    for (int n = 0; n < 12; n++) vBool.push_back(false);
    if (tMode & S_ISUID) vBool[0] = true;
    if (tMode & S_ISGID) vBool[1] = true;
    if (tMode & S_ISVTX) vBool[2] = true;
    if (tMode & S_IRUSR) vBool[3] = true;
    if (tMode & S_IWUSR) vBool[4] = true;
    if (tMode & S_IXUSR) vBool[5] = true;
    if (tMode & S_IRGRP) vBool[6] = true;
    if (tMode & S_IWGRP) vBool[7] = true;
    if (tMode & S_IXGRP) vBool[8] = true;
    if (tMode & S_IROTH) vBool[9] = true;
    if (tMode & S_IWOTH) vBool[10] = true;
    if (tMode & S_IXOTH) vBool[11] = true;
}

mode_t BoolToMode(vector<bool> &vBool) {
    mode_t tMode = 0;
    if (vBool[0] == true) tMode = tMode | S_ISUID;
    if (vBool[1] == true) tMode = tMode | S_ISGID;
    if (vBool[2] == true) tMode = tMode | S_ISVTX;
    if (vBool[3] == true) tMode = tMode | S_IRUSR;
    if (vBool[4] == true) tMode = tMode | S_IWUSR;
    if (vBool[5] == true) tMode = tMode | S_IXUSR;
    if (vBool[6] == true) tMode = tMode | S_IRGRP;
    if (vBool[7] == true) tMode = tMode | S_IWGRP;
    if (vBool[8] == true) tMode = tMode | S_IXGRP;
    if (vBool[9] == true) tMode = tMode | S_IROTH;
    if (vBool[10] == true) tMode = tMode | S_IWOTH;
    if (vBool[11] == true) tMode = tMode | S_IXOTH;
    return tMode;
}

void CmdPanelImp::Chmod() {
    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("chmod local file system only."));
        return;
    }

    Selection tSelection;
    _pPanel->GetSelection(tSelection);
    vector<File *> tFileList = tSelection.GetData();

    for (int n = 0; n < (int) tFileList.size(); n++) {
        if (tFileList[n]->bDir) {
            if (YNBox(_("Question"), _("subdirectory files permision change ?"))) {
                tSelection.ExpandDir(_pPanel->GetReader(), true);
                tFileList = tSelection.GetData();
            }
            Refresh();
            break;
        }
    }

    struct stat t_stat;
    mode_t mode_1, mode_2;
    bool bStat = true;

    for (int n = 0; n < (int) tFileList.size(); n++) {
        if (stat(tFileList[n]->sFullName.c_str(), &t_stat) == -1) continue;
        mode_1 = t_stat.st_mode;
        if (mode_1 != mode_2 && n != 0) {
            bStat = false;
            break;
        }
        mode_2 = t_stat.st_mode;
    }

    vector<string> vViewStr;
    vector<bool> vBool;
    vViewStr.push_back(_("set user ID on execution"));
    vViewStr.push_back(_("set group ID on execution"));
    vViewStr.push_back(_("sticky bit"));

    vViewStr.push_back(_("read by owner"));
    vViewStr.push_back(_("write by owner"));
    vViewStr.push_back(_("execute/search by owner"));

    vViewStr.push_back(_("read by group"));
    vViewStr.push_back(_("write by group"));
    vViewStr.push_back(_("execute/search by group"));

    vViewStr.push_back(_("read by other"));
    vViewStr.push_back(_("write by other"));
    vViewStr.push_back(_("execute/search by other"));

    ModeToBool(mode_1, vBool);

    Curses_CheckBox tCheckBox(_("Chmod selected files"));

    tCheckBox.Size(16, 30);
    tCheckBox.SetChkData(vViewStr, vBool);
    tCheckBox.Move(MIDDLE, MIDDLE);
    tCheckBox.Do();
    if (!tCheckBox.IsYes()) {
        Refresh();
        return;
    }

    vBool = tCheckBox.GetChkList();

    mode_1 = BoolToMode(vBool);

    vViewStr.clear(); // Errstring 역활을 한다.
    for (int n = 0; n < (int) tFileList.size(); n++) {
        if (chmod(tFileList[n]->sFullName.c_str(), mode_1) == -1) {
            string sStr = tFileList[n]->sFullName + " - " + strerror(errno);
            vViewStr.push_back(sStr);
        }
    }

    Refresh();

    if (vViewStr.size() != 0) {
        Curses_TextBox tTextBox(_("Error Info"));

        int nHeightSize = vViewStr.size();

        if (nHeightSize > LINES - 6 && LINES - 6 > 0)
            nHeightSize = LINES - 6;
        else
            nHeightSize = vViewStr.size() + 6;

        int nMaxWidth = COLS - 4;
        for (uint n = 0; n < vViewStr.size(); n++) {
            int nSize = scrstrlen(vViewStr[n]);
            if (nSize > nMaxWidth) nMaxWidth = nSize;
        }

        tTextBox.backcolor = COLOR_RED;
        tTextBox.fontcolor = COLOR_WHITE;

        if (nMaxWidth + 4 > COLS - 8) nMaxWidth = COLS - 8 - 4;
        tTextBox.setCurShow(true);
        tTextBox.Size(nHeightSize, nMaxWidth + 4);
        tTextBox.setText(vViewStr);
        tTextBox.Move(MIDDLE, MIDDLE);
        tTextBox.Do();
    }
}

void CmdPanelImp::RemoteConnProps() {
    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("current remote connected."));
        return;
    }

    RemoteCfgLoad tRemoteCfg;

    string sPath = g_tCfg.getValue("Default", "RemoteConnCfgFile", "remoteconn.cfg");
    sPath = g_tCfg.getValue("Static", "CfgHome") + sPath;

    tRemoteCfg.Load(sPath);

    if (COLS < 60) {
        MsgBox(_("Error"), _("Terminal size is too small."));
        return;
    }

    bool bExit = false;
    bool bModify = false;

    int nNum = 0;

    do {
        vector<string> vData;
        String sStr;

        int nButtonNum = 1;

        for (int n = 0; n < (int) tRemoteCfg._vConnInfo.size(); n++)
            vData.push_back(tRemoteCfg._vConnInfo[n].sName.c_str());

        if (tRemoteCfg._vConnInfo.size() > 0) {
            if (nNum > (int) tRemoteCfg._vConnInfo.size())
                nNum = (int) tRemoteCfg._vConnInfo.size();

            Dialog_RemoteConnProps tProps(_("Remote Connect Properties"), nNum);

            tProps.SetSelData(vData);
            tProps.Size(20, 60);
            tProps.Move(MIDDLE, MIDDLE);

            tProps.Do();
            tProps.Close();

            nButtonNum = tProps.GetButtonNum();
            nNum = tProps.GetListCur();
            Refresh();
        }

        switch (nButtonNum) {
            case 1: // Insert
            {
                bool bContinue = false;
                do {
                    bContinue = false;

                    Dialog_RemoteConnInsert tConnInsert(_("Remote Connect Insert"));
                    tConnInsert.Size(18, 60);
                    tConnInsert.Move(MIDDLE, MIDDLE);
                    tConnInsert.Do();

                    if (tConnInsert.GetOk() == true) {
                        RemoteConnEntry tEntry;
                        tConnInsert.GetRemoteConnEntry(tEntry);

                        for (int n = 0; n < (int) tRemoteCfg._vConnInfo.size(); n++) {
                            if (tRemoteCfg._vConnInfo[n].sName == tEntry.sName) {
                                MsgBox(_("Error"), _("Connection Name Overlapping."));
                                bContinue = true;
                                break;
                            }
                        }

                        if (!bContinue) {
                            tRemoteCfg._vConnInfo.push_back(tEntry);
                            nNum = tRemoteCfg._vConnInfo.size() - 1;
                        }
                        bModify = true;
                    } else if (tRemoteCfg._vConnInfo.size() == 0) {
                        bExit = true;
                    }
                } while (bContinue);
                break;
            }

            case 2: // Modify
            {
                bool bContinue = false;
                do {
                    bContinue = false;

                    Dialog_RemoteConnInsert tConnModify(_("Remote Connect Modify"));
                    tConnModify.Size(18, 60);
                    tConnModify.Move(MIDDLE, MIDDLE);
                    tConnModify.SetRemoteConnEntry(tRemoteCfg._vConnInfo[nNum]);
                    tConnModify.Do();
                    if (tConnModify.GetOk() == true) {
                        RemoteConnEntry tEntry;
                        tConnModify.GetRemoteConnEntry(tEntry);

                        for (int n = 0; n < (int) tRemoteCfg._vConnInfo.size(); n++) {
                            if (tRemoteCfg._vConnInfo[n].sName == tEntry.sName
                                && tRemoteCfg._vConnInfo[nNum].sName != tEntry.sName) {
                                MsgBox(_("Error"), _("Connection Name Overlapping."));
                                bContinue = true;
                                break;
                            }
                        }

                        if (!bContinue)
                            tRemoteCfg._vConnInfo[nNum] = tEntry;
                        bModify = true;
                    }
                } while (bContinue);
                break;
            }

            case 3:    // Delete
            {
                String sMsg(_("Do you really remove connection infomation? - %s"),
                            tRemoteCfg._vConnInfo[nNum].sName.c_str());
                if (YNBox(_("Error"), sMsg.c_str(), false) == true) {
                    tRemoteCfg._vConnInfo.erase(tRemoteCfg._vConnInfo.begin() + nNum);
                    bModify = true;
                }
                break;
            }

            case 4:    // Close
            {
                bExit = true;
                break;
            }

            case 0: // List
            {
                string sConnectionInfo = tRemoteCfg._vConnInfo[nNum].GetURLFormat(true);
                string sDir = tRemoteCfg._vConnInfo[nNum].sDirectory;
                sConnectionInfo = sConnectionInfo.substr(0, sConnectionInfo.size() - sDir.size());

                if (sConnectionInfo.size() > 6) {
                    if (sConnectionInfo.substr(0, 6) == "ftp://") {
                        sConnectionInfo = sConnectionInfo.substr(6);
                        _pPanel->PluginOpen(sConnectionInfo, "ftp");
                        if (_pPanel->GetReader()->GetConnected()) {
                            _pPanel->Read(sDir);
                            bExit = true;
                        }
                    } else if (sConnectionInfo.substr(0, 7) == "sftp://") {
                        sConnectionInfo = sConnectionInfo.substr(7);
                        _pPanel->PluginOpen(sConnectionInfo, "sftp");
                        if (_pPanel->GetReader()->GetConnected()) {
                            _pPanel->Read(sDir);
                            bExit = true;
                        }
                    } else if (sConnectionInfo.substr(0, 5) == "smb:/") {
                        sConnectionInfo = sConnectionInfo.substr(5);
                        _pPanel->PluginOpen(sConnectionInfo, "smb");
                        if (_pPanel->GetReader()->GetConnected()) {
                            _pPanel->Read(sDir);
                            bExit = true;
                        }
                    } else {
                        MsgBox(_("Error"), _("input url invalid format."));
                    }
                }
            }
        }

        Refresh();
        g_tMainFrame.Refresh(false);
    } while (!bExit);

    if (bModify) {
        tRemoteCfg.Save(sPath);
        chmod(sPath.c_str(), S_IRUSR | S_IWUSR);
    }
}

void CmdPanelImp::SyncDirectory() {
    g_tMainFrame.SyncDirectory();
}

void CmdPanelImp::ConsoleMode() {
#ifndef __CYGWIN_C__

    if (g_tMainFrame.GetActiveViewType() != PANEL) return;

    if (_pPanel->GetReader()->GetReaderName() != "file") {
        MsgBox(_("Error"), _("current connect mode or archive mode."));
        return;
    }

    CursesDestroy();

    // 서브 쉘의 터미널 사이즈 자동 변경 적용되게.
    Signal_ResizeBlocking();

    bool bShowErrMsg = false;
    string sCurDir, sPrompt;

    if (!g_SubShell.IsAlive()) {
        // 서브 쉘 초기화.
        g_SubShell.InitSubShell(_pPanel->GetReader()->GetPath());
    } else
        sPrompt = g_SubShell.GetPrompt_SubShell();

    if (!sPrompt.empty()) {
        // Remote 접속시에는 디렉토리 적용되지 않게.
        char cHostName[100];
        memset(&cHostName, 0, sizeof(cHostName));
        if (gethostname((char *) &cHostName, sizeof(cHostName)) != -1) {
            string sHostName = cHostName;
            if (sHostName.find(".") != string::npos)
                sHostName = sHostName.substr(0, sHostName.find("."));

            if (getbetween(sPrompt, '@', ':') == sHostName)
                g_SubShell.SetDir_SubShell(_pPanel->GetReader()->GetPath().c_str());
        }
    }

    if (g_SubShell.IsAlive()) {
        // 서브 쉘 실행.
        g_SubShell.FeedSubShell(VISIBLY, false);
        sPrompt = g_SubShell.GetPrompt_SubShell();
    } else
        bShowErrMsg = true;

    // 서브 쉘에서 터미널 사이즈 변경 원상 복구.
    Signal_ResizeUnblocking();

    CursesInit(g_tCfg.getBool("Default", "Transparency", false));

    if (!sPrompt.empty()) {
        char cHostName[100];
        memset(&cHostName, 0, sizeof(cHostName));
        if (gethostname((char *) &cHostName, sizeof(cHostName)) != -1) {
            string sHostName = cHostName;
            if (sHostName.find(".") != string::npos)
                sHostName = sHostName.substr(0, sHostName.find("."));

            if (getbetween(sPrompt, '@', ':') == sHostName) {
                string sTmp = getbetween(sPrompt, ':', '$');
                if (sTmp.empty())
                    sTmp = getbetween(sPrompt, ':', '#');
                if (!sTmp.empty()) {
                    sCurDir = chop(sTmp);
                    if (sTmp[0] == '~') {
                        string::size_type s = sPrompt.find_first_of('@');
                        if (s != string::npos) {
                            struct passwd *pw = getpwnam(sPrompt.substr(0, s).c_str());
                            if (pw)
                                sCurDir = pw->pw_dir + chop(sTmp.substr(1));
                        }
                    }
                }
            }
        }
    }

    if (!sCurDir.empty() && sCurDir != _pPanel->GetReader()->GetPath()) {
        _pPanel->Read(sCurDir);
        if (g_tMainFrame.GetScreenSync())
            ViewSync();
        Refresh();
    }
#else
    bool bShowErrMsg = true;
#endif

    if (bShowErrMsg)
        MsgBox(_("Error"), _("this system can't console mode."));
}

void CmdPanelImp::Diff() {
    string sDiffProgram;

    File *pFile1 = g_tMainFrame.GetPanel(0)->GetCurFile();
    File *pFile2 = g_tMainFrame.GetPanel(1)->GetCurFile();

    if (!pFile1 || !pFile2)
        return;

    File tFile1, tFile2;

    if (g_tMainFrame.GetPanel(0)->GetReader()->View(pFile1, &tFile1) == false) return;
    if (g_tMainFrame.GetPanel(1)->GetReader()->View(pFile2, &tFile2) == false) return;

    sDiffProgram = g_tCfg.getValue("Default", "DiffProgram", "diff");

    string sCommand;

    if (sDiffProgram == "diff") {
        File tFileTmp;

        tFileTmp.sName = tFile1.sName + ".diff";
        tFileTmp.sFullName = g_tCfg.getValue("Static", "TmpDir") + tFileTmp.sName;

        sCommand = sDiffProgram + " " +
                   tFile1.sFullName + " " +
                   tFile2.sFullName + " > " + tFileTmp.sFullName;

        LOG_WRITE("Diff [%s]", sCommand.c_str());

        system(sCommand.c_str());

        EditorChoice(true, g_tCfg.getValue("Default", "ExtEditor", "vi"), &tFileTmp);

        if (g_tMainFrame.GetPanel(0)->GetReader()->GetReaderName() == "sftp" ||
            g_tMainFrame.GetPanel(0)->GetReader()->GetReaderName() == "ftp" ||
            g_tMainFrame.GetPanel(0)->GetReader()->GetReaderName() == "smb")
            remove(tFile1.sFullName.c_str());

        if (g_tMainFrame.GetPanel(1)->GetReader()->GetReaderName() == "sftp" ||
            g_tMainFrame.GetPanel(1)->GetReader()->GetReaderName() == "ftp" ||
            g_tMainFrame.GetPanel(0)->GetReader()->GetReaderName() == "smb")
            remove(tFile2.sFullName.c_str());

        remove(tFileTmp.sFullName.c_str());
    } else {
        sCommand = sDiffProgram + " " + tFile1.sFullName + " " + tFile2.sFullName + " %B";
        ParseAndRun(sCommand, false);
    }
}
