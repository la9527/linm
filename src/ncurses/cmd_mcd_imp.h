#ifndef __MLS_CMD_MCD_IMP_H__
#define __MLS_CMD_MCD_IMP_H__

#include "cmd_common_imp.h"

#include "command.h"
#include "selection.h"

// PluginLoader 때문.
#include "vfsplugin.h"
#include "readerctl.h"

namespace MLS {

    class CmdMcdImp : public CmdCommonImp, public Configurable {
    private:
        vector<NCurses_Panel *> _pCurPanel;

    public:
        CmdMcdImp() : CmdCommonImp() {
            UpdateConfig();

            // 이것은 Copy&Paste를 위해서 따로 쓰이는 Reader이다.
            // 여기에서 한번만 로드
            PluginLoader(&g_tReaderCtl);
        }

        ~CmdMcdImp() { SaveConfig(); }

        // IConfigurable
        void UpdateConfig();

        void SaveConfig();

        void GoRoot();

        void GoHome();

        void GoParent();

        void Key_Enter();

        void Key_Left();

        void Key_Right();

        void Key_Up();

        void Key_Down();

        void Key_Home();

        void Key_End();

        void Key_Ins() { ClipPaste(); }

        void Key_Del() { Remove(); }

        void Key_BS() { Key_Left(); }

        void Key_PageUp();

        void Key_PageDown();

        void Key_ESC();

        void ClearRefresh();

        void Refresh();

        void RemoteConnect();

        void RemoteClose();

        void Menu();

        void Quit();

        void Split();

        void NextWindow();

        void SplitViewSync();

        void ViewSync();

        void Remove();

        void ClipCopy();

        void ClipPaste();

        void ClipCut();

        void Rename();

        void Mkdir();

        void Shell();

        void McdSubDirOneSearch() { _pMcd->SubDirOneSearch(); }

        void McdSubDirAllSearch() { _pMcd->SubDirAllSearch(); }

        void McdSubDirClear() { _pMcd->SubDirClear(); }

        void HiddenFileView();

        void AllScan();

        void Scan();

        void MountList();

        void SizeInfo();

        void McdLoad();

        void McdSave();

    protected:
        void CutPaste();

        void CopyPaste();
    }; // class CommandImp

}; // namespace

#endif
