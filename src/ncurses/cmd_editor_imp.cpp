#include "cmd_editor_imp.h"
#include "mainframe.h"
#include "mlsmenu.h"

using namespace MLS;
using namespace MLSUTIL;

void CmdEditorImp::UpdateConfig() {
}

void CmdEditorImp::SaveConfig() {
}

void CmdEditorImp::Split() {
    g_tMainFrame.Split();
}

void CmdEditorImp::NextWindow() {
    g_tMainFrame.NextWindow();
}

void CmdEditorImp::Key_Enter() {
    _pEditor->Key_Enter();
}

void CmdEditorImp::Key_Left() {
    _pEditor->Key_Left();
}

void CmdEditorImp::Key_Right() {
    _pEditor->Key_Right();
}

void CmdEditorImp::Key_Up() {
    _pEditor->Key_Up();
}

void CmdEditorImp::Key_Down() {
    _pEditor->Key_Down();
}

void CmdEditorImp::Key_Home() {
    _pEditor->Key_Home();
}

void CmdEditorImp::Key_End() {
    _pEditor->Key_End();
}

void CmdEditorImp::Key_PageUp() {
    _pEditor->Key_PgUp();
}

void CmdEditorImp::Key_PageDown() {
    _pEditor->Key_PgDn();
}

void CmdEditorImp::Key_Del() {
    _pEditor->Key_Del();
}

void CmdEditorImp::Key_BS() {
    _pEditor->Key_BS();
}

void CmdEditorImp::Key_Ins() {
    _pEditor->Key_Ins();
}

void CmdEditorImp::Menu() {
    EditorMenu tEditorMenu;
    vector<string> vDisableList;

    if (_pPanel->GetReader()->GetReaderName() == "file") {
        vDisableList.push_back("Cmd_FtpClose");
    } else if (_pPanel->GetReader()->GetReaderName() == "ftp" ||
               _pPanel->GetReader()->GetReaderName() == "sftp" ||
               _pPanel->GetReader()->GetReaderName() == "samba") {
        vDisableList.push_back("Cmd_FtpConn");
    } else {
        vDisableList.push_back("Cmd_FtpConn");
        vDisableList.push_back("Cmd_FtpClose");
    }

    tEditorMenu.SetDisable(vDisableList);
    tEditorMenu.SetEditor(_pEditor);
    tEditorMenu.Create();
    tEditorMenu.Do();

    g_tMainFrame.Refresh(false);
    if (tEditorMenu.GetCurCmd().size() != 0) {
        LOG_WRITE("Menu Run [%s]", tEditorMenu.GetCurCmd().c_str());
        g_tMainFrame.GetCommand()->Execute(tEditorMenu.GetCurCmd());
    }
}

void CmdEditorImp::Quit() {
    if (_pEditor->_bFullScreen) {
        MouseInit();
        _pEditor->_bMouseMode = true;
    }

    if (_pEditor->Quit() == true) {
        // file 이 아니면 tmp에 저장 되어 있기 때문에 지운다.
        if (_pPanel->GetReader()->GetReaderName() != "file") {
            File *pFile = _pEditor->GetFile();

            string sTmpFile = pFile->sFullName;
            string sTmpDir = g_tCfg.getValue("Static", "TmpDir");
            if (sTmpDir.size() < 4) return;

            sTmpFile = sTmpFile.substr(sTmpDir.size());
            string::size_type p = sTmpFile.find("/");
            string sDeleteFile;

            if (p != string::npos) {
                sDeleteFile = "rm -rf " + sTmpDir +
                              sTmpFile.substr(0, p) + " > /dev/null 2> /dev/null";
            } else {
                sDeleteFile = "rm -rf " + sTmpDir +
                              sTmpFile + " > /dev/null 2> /dev/null";
            }
            system(sDeleteFile.c_str());
        }

        ReloadConfigChange();
        g_tMainFrame.SetActiveViewType(PANEL);
        RefreshAll();
    }
}

void CmdEditorImp::ClipCopy() {
    _pEditor->Copy();
}

void CmdEditorImp::ClipCut() {
    _pEditor->Cut();
}

void CmdEditorImp::ClipPaste() {
    _pEditor->Paste();
}

void CmdEditorImp::Undo() {
    _pEditor->Undo();
}

void CmdEditorImp::Key_ESC() {
    _pEditor->Key_ESC();
}

void CmdEditorImp::ClearRefresh() {
    ScreenClear();
    RefreshAll();
}

void CmdEditorImp::Refresh() {
    _pEditor->Refresh();
}

void CmdEditorImp::RefreshAll() {
    string sFileName1, sFileName2;
    if (g_tMainFrame.GetPanel(0)->GetCurFile() != NULL)
        sFileName1 = g_tMainFrame.GetPanel(0)->GetCurFile()->sFullName;
    if (g_tMainFrame.GetPanel(1)->GetCurFile() != NULL)
        sFileName2 = g_tMainFrame.GetPanel(1)->GetCurFile()->sFullName;

    g_tMainFrame.GetPanel(0)->Read(g_tMainFrame.GetPanel(0)->GetPath());
    g_tMainFrame.GetPanel(1)->Read(g_tMainFrame.GetPanel(1)->GetPath());

    if (!sFileName1.empty())
        g_tMainFrame.GetPanel(0)->SetCurFileName(sFileName1);
    if (!sFileName2.empty())
        g_tMainFrame.GetPanel(1)->SetCurFileName(sFileName2);
    _pEditor->Refresh();
    g_tMainFrame.Refresh();
}


void CmdEditorImp::FileNew() {
    if (_pPanel->GetReader()->GetReaderName() == "file") {
        chdir(_pPanel->GetReader()->GetPath().c_str());
        _pEditor->FileNew();
    } else
        MsgBox(_("Error"), _("NewFile is local use only."));
    RefreshAll();
}

void CmdEditorImp::FileSave() {
    if (!_pEditor->FileSave()) return;

    if (_pPanel->GetReader()->GetReaderName() == "sftp" ||
        _pPanel->GetReader()->GetReaderName() == "ftp" ||
        _pPanel->GetReader()->GetReaderName() == "samba") {
        File *pFile = _pEditor->GetFile();
        string sPath = pFile->sFullName.substr(0, pFile->sFullName.size() - pFile->sName.size());
        string sTmpFile = pFile->sFullName;

        pFile->sFullName = _pEditor->GetFileName();

        Selection tSelection;
        tSelection.Add(pFile);
        tSelection.SetSelectPath(sPath);
        _pPanel->GetReader()->Paste(tSelection);

        remove(sTmpFile.c_str());
        sTmpFile = _pEditor->GetFileName();
        remove(sTmpFile.c_str());
    }
    RefreshAll();
}

void CmdEditorImp::FileSaveAs() {
    if (!_pEditor->FileSaveAs()) return;

    if (_pPanel->GetReader()->GetReaderName() == "sftp" ||
        _pPanel->GetReader()->GetReaderName() == "ftp" ||
        _pPanel->GetReader()->GetReaderName() == "samba") {
        File *pFile = _pEditor->GetFile();
        string sPath = pFile->sFullName.substr(0, pFile->sFullName.size() - pFile->sName.size());
        string sTmpFile = pFile->sFullName;

        pFile->sFullName = _pEditor->GetFileName();
        LOG_WRITE("%s", pFile->sFullName.c_str());

        Selection tSelection;
        tSelection.Add(pFile);
        tSelection.SetSelectPath(sPath);
        _pPanel->GetReader()->Paste(tSelection);

        remove(sTmpFile.c_str());
        sTmpFile = _pEditor->GetFileName();
        remove(sTmpFile.c_str());
    }
    RefreshAll();
}

void CmdEditorImp::LineNumberView() {
    _pEditor->LineNumberView();
}

void CmdEditorImp::VimView() {
    if (YNBox(_("Vim View"), _("Do you wan't vi editor ?"), true) == false) return;

    string sFileName = _pEditor->GetFileName();
    if (sFileName.empty()) return;

    //Quit();

    if (g_tCfg.getValue("Default", "ExtEditor").empty())
        sFileName = "vi %F";
    else
        sFileName = g_tCfg.getValue("Default", "ExtEditor") + " %F";

    g_tMainFrame.GetCommand()->ParseAndRun(sFileName, false);

    _pEditor->Load(sFileName);
    RefreshAll();
}

void CmdEditorImp::Replace() {
    _pEditor->Replace();
}

void CmdEditorImp::MouseUse() {
    if (_pEditor->_bFullScreen == true) {
        _pEditor->_bMouseMode = !_pEditor->_bMouseMode;

        if (_pEditor->_bMouseMode)
            MouseInit();
        else
            MouseDestroy();
    }
}	
