#include "cmd_panel_imp.h"
#include "dialog_config.h"
#include "mainframe.h"

using namespace MLS;

void CmdPanelImp::SettingChange() {
    vector<ConfigOBJ> tOBJ;

    tOBJ.push_back(ConfigOBJ("Sort", _("file sort (0:none,1:name,2:ext,3:size,4:time,5:color)"),
                             g_tCfg.GetValue("Default", "Sort", "5"),
                             INPUTBOX));
    tOBJ.push_back(ConfigOBJ("Column", _("max column size ( 0:Auto, 1~6 )"),
                             g_tCfg.GetValue("Default", "Column", "0"),
                             INPUTBOX));
    tOBJ.push_back(ConfigOBJ("DirSort", _("directory first sort"),
                             g_tCfg.GetValue("Default", "DirSort", "On"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("Enter_RunSelect", _("choice file execute"),
                             g_tCfg.GetValue("Default", "Enter_RunSelect", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("ShowHidden", _("hidden file view"),
                             g_tCfg.GetValue("Default", "ShowHidden", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("HintView", _("hint view"),
                             g_tCfg.GetValue("Default", "HintView", "On"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("AlwaysRedraw", _("always screen redraw"),
                             g_tCfg.GetValue("Default", "AlwaysRedraw", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("Use_LinM_Editor", _("default use LinM editor."),
                             g_tCfg.GetValue("Default", "Use_LinM_Editor", "On"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("ExtEditor", _("external editor program"),
                             g_tCfg.GetValue("Default", "ExtEditor", "vi"),
                             INPUTBOX));
    tOBJ.push_back(ConfigOBJ("EditorTabSize", _("LinM editor tab width"),
                             g_tCfg.GetValue("Default", "EditorTabSize", "4"),
                             INPUTBOX));
    tOBJ.push_back(ConfigOBJ("EditorBackup", _("LinM editor backup on save"),
                             g_tCfg.GetValue("Default", "EditorBackup", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("EditorLineNum", _("LinM editor line number view"),
                             g_tCfg.GetValue("Default", "EditorLineNum", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("LineCodeChange", _("line code change '-,+'"),
                             g_tCfg.GetValue("Default", "LineCodeChange", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("DiffProgram", _("diff program"),
                             g_tCfg.GetValue("Default", "DiffProgram", "diff"),
                             INPUTBOX));
    tOBJ.push_back(ConfigOBJ("Transparency", _("console tansparency"),
                             g_tCfg.GetValue("Default", "Transparency", "Off"),
                             CHECKBOX));
    tOBJ.push_back(ConfigOBJ("NoSplit_NextWindow", _("always enable change to next window"),
                             g_tCfg.GetValue("Default", "NoSplit_NextWindow", "Off"),
                             CHECKBOX));


    DIALOG_VIEW:
    Dialog_Config tDialog;

    tDialog.SetViewConfig(tOBJ);

    tDialog.Move(MIDDLE, MIDDLE);
    tDialog.Do();
    tDialog.Close();

    if (!tDialog.GetOk()) return;

    bool bChgTransparency = false;

    const vector<ConfigOBJ> tObj2 = tDialog.getConfigOBJS();

    for (int n = 0; n < tObj2.size(); n++) {
        if (tObj2[n]._id == "Sort" || tObj2[n]._id == "Column" || tObj2[n]._id == "EditorTabSize") {
            try {
                int nValue = MLSUTIL::strtoint(tObj2[n]._value);

                if (tObj2[n]._id == "Sort" && (nValue < 0 || nValue > 5))
                    throw MLSUTIL::Exception("invalid value.");

                if (tObj2[n]._id == "Column" && (nValue < 0 || nValue >= 7))
                    throw MLSUTIL::Exception("invalid value.");
            }
            catch (MLSUTIL::Exception &ex) {
                MLSUTIL::MsgBox(_("Error"), string("'" + tObj2[n]._text + "' " + _("is invalid.")));
                goto DIALOG_VIEW;
            }
        }

        if (tObj2[n]._id == "Transparency") {
            string strOn = g_tCfg.GetValue("Default", "Transparency", "Off");
            if (tObj2[n]._value != strOn)
                bChgTransparency = true;
        }
    }

    for (int n = 0; n < tObj2.size(); n++) {
        g_tCfg.SetValue("Default", tObj2[n]._id, tObj2[n]._value);
    }

    g_tCfg.Save();

    _pEditor->_bConfChg = true;
    ReloadConfigChange(bChgTransparency);
}
