#include "command.h"
#include "cmd_panel_imp.h"
#include "cmd_mcd_imp.h"
#include "cmd_editor_imp.h"
#include "mlsdialog.h"
#include "ncurses_panel.h"
#include "ncurses_mcd.h"
#include "ncurses_editor.h"
#include "mainframe.h"

namespace MLS {

    Command::Command() {
        _pPanelImp = new CmdPanelImp;
        _pMcdImp = new CmdMcdImp;
        _pEditorImp = new CmdEditorImp;
        Init();
    }

    Command::~Command() {
        PanelMap::iterator i;
        McdMap::iterator j;
        EditorMap::iterator k;

        for (i = _mapPanelFunc.begin(); i != _mapPanelFunc.end(); ++i)
            delete i->first;

        for (j = _mapMcdFunc.begin(); j != _mapMcdFunc.end(); ++j)
            delete j->first;

        for (k = _mapEditorFunc.begin(); k != _mapEditorFunc.end(); ++k)
            delete k->first;

        if (_pPanelImp) delete _pPanelImp;
        if (_pMcdImp) delete _pMcdImp;
        if (_pEditorImp) delete _pEditorImp;
    }

#define MAP_COMMONKEY_INSERT(MAPFUNC, CLASS)                    \
    _map##MAPFUNC[new TypeInfo("Left")]    = &Cmd##CLASS::Key_Left;    \
    _map##MAPFUNC[new TypeInfo("Right")]= &Cmd##CLASS::Key_Right;    \
    _map##MAPFUNC[new TypeInfo("Up")]    = &Cmd##CLASS::Key_Up;        \
    _map##MAPFUNC[new TypeInfo("Down")]    = &Cmd##CLASS::Key_Down;    \
    _map##MAPFUNC[new TypeInfo("Home")]    = &Cmd##CLASS::Key_Home;    \
    _map##MAPFUNC[new TypeInfo("End")]    = &Cmd##CLASS::Key_End;        \
    _map##MAPFUNC[new TypeInfo("PgUp")]    = &Cmd##CLASS::Key_PageUp;    \
    _map##MAPFUNC[new TypeInfo("PgDn")]    = &Cmd##CLASS::Key_PageDown;\
    _map##MAPFUNC[new TypeInfo("Enter")]= &Cmd##CLASS::Key_Enter;    \
    _map##MAPFUNC[new TypeInfo("ESC")]    = &Cmd##CLASS::Key_ESC;        \
    _map##MAPFUNC[new TypeInfo("Ins")]    = &Cmd##CLASS::Key_Ins;        \
    _map##MAPFUNC[new TypeInfo("Del")]    = &Cmd##CLASS::Key_Del;        \
    _map##MAPFUNC[new TypeInfo("BS")]    = &Cmd##CLASS::Key_BS;        \
    _map##MAPFUNC[new TypeInfo("Quit")]    = &Cmd##CLASS::Quit;        \
    _map##MAPFUNC[new TypeInfo("Menu")]    = &Cmd##CLASS::Menu;        \
    _map##MAPFUNC[new TypeInfo("Refresh")]= &Cmd##CLASS::ClearRefresh;    \
    _map##MAPFUNC[new TypeInfo("About")]= &Cmd##CLASS::About;

    void Command::Init() {
        // -- Key
        // Common key
        MAP_COMMONKEY_INSERT(PanelFunc, PanelImp);
        MAP_COMMONKEY_INSERT(McdFunc, McdImp);
        MAP_COMMONKEY_INSERT(EditorFunc, EditorImp);

        _mapPanelFunc[new TypeInfo(PANEL, "Select")] = &CmdPanelImp::Key_Select;
        _mapPanelFunc[new TypeInfo(PANEL, "GoRoot")] = &CmdPanelImp::GoRoot;
        _mapPanelFunc[new TypeInfo(PANEL, "GoHome")] = &CmdPanelImp::GoHome;
        _mapPanelFunc[new TypeInfo(PANEL, "GoParent")] = &CmdPanelImp::GoParent;
        _mapPanelFunc[new TypeInfo(PANEL, "SelectInvert")] = &CmdPanelImp::SelectInvert;
        _mapPanelFunc[new TypeInfo(PANEL, "SelectAll")] = &CmdPanelImp::SelectAll;
        _mapPanelFunc[new TypeInfo(PANEL, "Editor")] = &CmdPanelImp::Editor;
        _mapPanelFunc[new TypeInfo(PANEL, "ExtEditor")] = &CmdPanelImp::ExtEditor;
        _mapPanelFunc[new TypeInfo(PANEL, "ViewSync")] = &CmdPanelImp::ViewSync;
        _mapPanelFunc[new TypeInfo(PANEL, "SplitViewSync")] = &CmdPanelImp::SplitViewSync;
        _mapPanelFunc[new TypeInfo(PANEL, "Split")] = &CmdPanelImp::Split;
        _mapPanelFunc[new TypeInfo(PANEL, "NextWindow")] = &CmdPanelImp::NextWindow;

        _mapPanelFunc[new TypeInfo(PANEL, "Copy")] = &CmdPanelImp::Copy;
        _mapPanelFunc[new TypeInfo(PANEL, "Move")] = &CmdPanelImp::Move;

        _mapPanelFunc[new TypeInfo(PANEL, "Archive")] = &CmdPanelImp::ArchiveFileView;
        _mapPanelFunc[new TypeInfo(PANEL, "ClipCopy")] = &CmdPanelImp::ClipCopy;
        _mapPanelFunc[new TypeInfo(PANEL, "ClipCut")] = &CmdPanelImp::ClipCut;
        _mapPanelFunc[new TypeInfo(PANEL, "ClipPaste")] = &CmdPanelImp::ClipPaste;
        _mapPanelFunc[new TypeInfo(PANEL, "ViewClip")] = &CmdPanelImp::ViewClip;

        _mapPanelFunc[new TypeInfo(PANEL, "Remove")] = &CmdPanelImp::Remove;
        _mapPanelFunc[new TypeInfo(PANEL, "Rename")] = &CmdPanelImp::Rename;
        _mapPanelFunc[new TypeInfo(PANEL, "Mkdir")] = &CmdPanelImp::Mkdir;
        _mapPanelFunc[new TypeInfo(PANEL, "RemoteConnect")] = &CmdPanelImp::RemoteConnect;
        _mapPanelFunc[new TypeInfo(PANEL, "RemoteClose")] = &CmdPanelImp::RemoteClose;
        _mapPanelFunc[new TypeInfo(PANEL, "Shell")] = &CmdPanelImp::Shell;
        _mapPanelFunc[new TypeInfo(PANEL, "BoxCodeChange")] = &CmdPanelImp::BoxCodeChange;
        _mapPanelFunc[new TypeInfo(PANEL, "LangChange")] = &CmdPanelImp::LangChange;
        _mapPanelFunc[new TypeInfo(PANEL, "HiddenFileView")] = &CmdPanelImp::HiddenFileView;
        _mapPanelFunc[new TypeInfo(PANEL, "FileOwnerView")] = &CmdPanelImp::FileOwnerView;

        _mapPanelFunc[new TypeInfo(PANEL, "SortChange")] = &CmdPanelImp::SortChange;
        _mapPanelFunc[new TypeInfo(PANEL, "SortAscDescend")] = &CmdPanelImp::SortAscDescend;
        _mapPanelFunc[new TypeInfo(PANEL, "ColumnAuto")] = &CmdPanelImp::ColumnAuto;
        _mapPanelFunc[new TypeInfo(PANEL, "Column1")] = &CmdPanelImp::Column1;
        _mapPanelFunc[new TypeInfo(PANEL, "Column2")] = &CmdPanelImp::Column2;
        _mapPanelFunc[new TypeInfo(PANEL, "Column3")] = &CmdPanelImp::Column3;
        _mapPanelFunc[new TypeInfo(PANEL, "Column4")] = &CmdPanelImp::Column4;
        _mapPanelFunc[new TypeInfo(PANEL, "MCD")] = &CmdPanelImp::Mcd;

        _mapPanelFunc[new TypeInfo(PANEL, "SettingChange")] = &CmdPanelImp::SettingChange;

        _mapPanelFunc[new TypeInfo(PANEL, "DefaultCfgFileChg")] = &CmdPanelImp::DefaultCfgFileChg;
        _mapPanelFunc[new TypeInfo(PANEL, "ColorCfgFileChg")] = &CmdPanelImp::ColorCfgFileChg;
        _mapPanelFunc[new TypeInfo(PANEL, "KeyCfgFileChg")] = &CmdPanelImp::KeyCfgFileChg;
        _mapPanelFunc[new TypeInfo(PANEL, "MountList")] = &CmdPanelImp::MountList;
        _mapPanelFunc[new TypeInfo(PANEL, "SizeInfo")] = &CmdPanelImp::SizeInfo;

        _mapPanelFunc[new TypeInfo(PANEL, "About")] = &CmdPanelImp::About;
        _mapPanelFunc[new TypeInfo(PANEL, "Help")] = &CmdPanelImp::Help;

        _mapPanelFunc[new TypeInfo(PANEL, "Back")] = &CmdPanelImp::Back;
        _mapPanelFunc[new TypeInfo(PANEL, "Forward")] = &CmdPanelImp::Forward;

        _mapPanelFunc[new TypeInfo(PANEL, "NewFile")] = &CmdPanelImp::NewFile;
        _mapPanelFunc[new TypeInfo(PANEL, "TouchFile")] = &CmdPanelImp::TouchFile;

        _mapPanelFunc[new TypeInfo(PANEL, "Extract")] = &CmdPanelImp::Extract;

        _mapPanelFunc[new TypeInfo(PANEL, "TargzComp")] = &CmdPanelImp::TargzCompress;
        _mapPanelFunc[new TypeInfo(PANEL, "Tarbz2Comp")] = &CmdPanelImp::Tarbz2Compress;
        _mapPanelFunc[new TypeInfo(PANEL, "ZipComp")] = &CmdPanelImp::ZipCompress;

        _mapPanelFunc[new TypeInfo(PANEL, "Execute")] = &CmdPanelImp::Execute;
        _mapPanelFunc[new TypeInfo(PANEL, "QCD")] = &CmdPanelImp::QCD;
        _mapPanelFunc[new TypeInfo(PANEL, "Chmod")] = &CmdPanelImp::Chmod;

        _mapPanelFunc[new TypeInfo(PANEL, "RemoteConnProps")] = &CmdPanelImp::RemoteConnProps;
        _mapPanelFunc[new TypeInfo(PANEL, "SyncDirectory")] = &CmdPanelImp::SyncDirectory;

        _mapPanelFunc[new TypeInfo(PANEL, "ConsoleMode")] = &CmdPanelImp::ConsoleMode;
        _mapPanelFunc[new TypeInfo(PANEL, "Chown")] = &CmdPanelImp::Chown;

        _mapPanelFunc[new TypeInfo(PANEL, "Diff")] = &CmdPanelImp::Diff;
        _mapPanelFunc[new TypeInfo(PANEL, "FileFind")] = &CmdPanelImp::FileFind;

        _mapMcdFunc[new TypeInfo(MCD, "AllScan")] = &CmdMcdImp::AllScan;
        _mapMcdFunc[new TypeInfo(MCD, "Scan")] = &CmdMcdImp::Scan;
        _mapMcdFunc[new TypeInfo(MCD, "MountList")] = &CmdMcdImp::MountList;
        _mapMcdFunc[new TypeInfo(MCD, "SplitViewSync")] = &CmdMcdImp::SplitViewSync;

        _mapMcdFunc[new TypeInfo(MCD, "Mkdir")] = &CmdMcdImp::Mkdir;
        _mapMcdFunc[new TypeInfo(MCD, "Rename")] = &CmdMcdImp::Rename;
        _mapMcdFunc[new TypeInfo(MCD, "Remove")] = &CmdMcdImp::Remove;
        _mapMcdFunc[new TypeInfo(MCD, "ClipCopy")] = &CmdMcdImp::ClipCopy;
        _mapMcdFunc[new TypeInfo(MCD, "ClipPaste")] = &CmdMcdImp::ClipPaste;
        _mapMcdFunc[new TypeInfo(MCD, "ClipCut")] = &CmdMcdImp::ClipCut;
        _mapMcdFunc[new TypeInfo(MCD, "SubDirOneSearch")] = &CmdMcdImp::McdSubDirOneSearch;
        _mapMcdFunc[new TypeInfo(MCD, "SubDirAllSearch")] = &CmdMcdImp::McdSubDirAllSearch;
        _mapMcdFunc[new TypeInfo(MCD, "SubDirClear")] = &CmdMcdImp::McdSubDirClear;
        _mapMcdFunc[new TypeInfo(MCD, "RemoteConnect")] = &CmdMcdImp::RemoteConnect;
        _mapMcdFunc[new TypeInfo(MCD, "RemoteClose")] = &CmdMcdImp::RemoteClose;
        _mapMcdFunc[new TypeInfo(MCD, "Menu")] = &CmdMcdImp::Menu;
        _mapMcdFunc[new TypeInfo(MCD, "Split")] = &CmdMcdImp::Split;
        _mapMcdFunc[new TypeInfo(MCD, "NextWindow")] = &CmdMcdImp::NextWindow;
        _mapMcdFunc[new TypeInfo(MCD, "GoRoot")] = &CmdMcdImp::GoRoot;
        _mapMcdFunc[new TypeInfo(MCD, "GoParent")] = &CmdMcdImp::GoParent;
        _mapMcdFunc[new TypeInfo(MCD, "GoHome")] = &CmdMcdImp::GoHome;
        _mapMcdFunc[new TypeInfo(MCD, "SizeInfo")] = &CmdMcdImp::SizeInfo;
        _mapMcdFunc[new TypeInfo(MCD, "Help")] = &CmdMcdImp::Help;
        _mapMcdFunc[new TypeInfo(MCD, "About")] = &CmdMcdImp::About;

        _mapEditorFunc[new TypeInfo(EDITOR, "Split")] = &CmdEditorImp::Split;
        _mapEditorFunc[new TypeInfo(EDITOR, "NextWindow")] = &CmdEditorImp::NextWindow;

        _mapEditorFunc[new TypeInfo(EDITOR, "FileNew")] = &CmdEditorImp::FileNew;
        _mapEditorFunc[new TypeInfo(EDITOR, "Save")] = &CmdEditorImp::FileSave;
        _mapEditorFunc[new TypeInfo(EDITOR, "SaveAS")] = &CmdEditorImp::FileSaveAs;

        _mapEditorFunc[new TypeInfo(EDITOR, "Undo")] = &CmdEditorImp::Undo;
        _mapEditorFunc[new TypeInfo(EDITOR, "ClipCopy")] = &CmdEditorImp::ClipCopy;
        _mapEditorFunc[new TypeInfo(EDITOR, "ClipPaste")] = &CmdEditorImp::ClipPaste;
        _mapEditorFunc[new TypeInfo(EDITOR, "ClipCut")] = &CmdEditorImp::ClipCut;

        _mapEditorFunc[new TypeInfo(EDITOR, "SelectMode")] = &CmdEditorImp::Select;
        _mapEditorFunc[new TypeInfo(EDITOR, "SelectAll")] = &CmdEditorImp::SelectAll;
        _mapEditorFunc[new TypeInfo(EDITOR, "Find")] = &CmdEditorImp::Find;
        _mapEditorFunc[new TypeInfo(EDITOR, "FindNext")] = &CmdEditorImp::FindNext;
        _mapEditorFunc[new TypeInfo(EDITOR, "FindPrevious")] = &CmdEditorImp::FindPrevious;
        _mapEditorFunc[new TypeInfo(EDITOR, "Tab")] = &CmdEditorImp::Key_Tab;
        _mapEditorFunc[new TypeInfo(EDITOR, "Untab")] = &CmdEditorImp::Key_Untab;
        _mapEditorFunc[new TypeInfo(EDITOR, "Find")] = &CmdEditorImp::Find;
        _mapEditorFunc[new TypeInfo(EDITOR, "FindNext")] = &CmdEditorImp::FindNext;
        _mapEditorFunc[new TypeInfo(EDITOR, "FindPrevious")] = &CmdEditorImp::FindPrevious;

        _mapEditorFunc[new TypeInfo(EDITOR, "Replace")] = &CmdEditorImp::Replace;

        _mapEditorFunc[new TypeInfo(EDITOR, "LineNumberView")] = &CmdEditorImp::LineNumberView;
        _mapEditorFunc[new TypeInfo(EDITOR, "IndentMode")] = &CmdEditorImp::IndentMode;
        _mapEditorFunc[new TypeInfo(EDITOR, "GotoLine")] = &CmdEditorImp::GotoLine;
        _mapEditorFunc[new TypeInfo(EDITOR, "GotoFirst")] = &CmdEditorImp::GotoFirst;
        _mapEditorFunc[new TypeInfo(EDITOR, "GotoEnd")] = &CmdEditorImp::GotoEnd;
        _mapEditorFunc[new TypeInfo(EDITOR, "VimView")] = &CmdEditorImp::VimView;
        _mapEditorFunc[new TypeInfo(EDITOR, "MouseUse")] = &CmdEditorImp::MouseUse;

        _mapEditorFunc[new TypeInfo(EDITOR, "BoxCodeChange")] = &CmdEditorImp::BoxCodeChange;
        _mapEditorFunc[new TypeInfo(EDITOR, "LangChange")] = &CmdEditorImp::LangChange;

        _mapEditorFunc[new TypeInfo(EDITOR, "About")] = &CmdEditorImp::About;
        _mapEditorFunc[new TypeInfo(EDITOR, "Help")] = &CmdMcdImp::Help;
    }

    void Command::SetPanel(NCurses_Panel *p) {
        _pPanelImp->SetPanel(p);
        _pMcdImp->SetPanel(p);
        _pEditorImp->SetPanel(p);
    }

    void Command::SetMcd(NCurses_Mcd *p) {
        _pPanelImp->SetMcd(p);
        _pMcdImp->SetMcd(p);
        _pEditorImp->SetMcd(p);
    }

    void Command::SetEditor(NCurses_Editor *p) {
        _pPanelImp->SetEditor(p);
        _pMcdImp->SetEditor(p);
        _pEditorImp->SetEditor(p);
    }

    NCurses_Panel *Command::GetPanel() {
        return _pPanelImp->GetPanel();
    }

/// @brief	해당 str 명령을 실행시킨다.
/// @param str  실행할 명령어
/// @return	0
    int Command::Execute(const std::string &strCmd) {
        if (strCmd.empty()) return ERROR;

        LOG_WRITE("Execute [%s]", strCmd.c_str());

        if (strCmd.size() > 4 && strCmd.substr(0, 4) == "Cmd_") {
            string sCmd = strCmd.substr(4);
            switch (g_tMainFrame.GetActiveViewType()) {
                case PANEL: {
                    PanelMap::iterator i;
                    for (i = _mapPanelFunc.begin(); i != _mapPanelFunc.end(); ++i) {
                        if (sCmd == i->first->sValue) {
                            (_pPanelImp->*((*i).second))();
                            return SUCCESS;
                        }
                    }
                    break;
                }
                case MCD: {
                    McdMap::iterator i;
                    for (i = _mapMcdFunc.begin(); i != _mapMcdFunc.end(); ++i) {
                        if (sCmd == i->first->sValue) {
                            (_pMcdImp->*((*i).second))();
                            return SUCCESS;
                        }
                    }
                    break;
                }
                case EDITOR: {
                    EditorMap::iterator i;
                    for (i = _mapEditorFunc.begin(); i != _mapEditorFunc.end(); ++i) {
                        if (sCmd == i->first->sValue) {
                            (_pEditorImp->*((*i).second))();
                            return SUCCESS;
                        }
                    }
                    return -2;
                    break;
                }

                default:
                    break;
            }
        }
        return ERROR;
    }

    int Command::Run(const std::string &str, bool bPause) {
        _pPanelImp->Run(str, bPause);
        return 0;
    }

    int Command::ParseAndRun(const std::string &sCmd, bool bPause) {
        _pPanelImp->ParseAndRun(sCmd, bPause);
        return 0;
    }

};
