#ifndef __QT_MENU_H__
#define __QT_MENU_H__

#include <QMenuBar>
#include <QMenu>
#include <QWidget>
#include <QPixmap>
#include <QCursor>
#include <QWidget>

#include "keycfgload.h"
#include "qt_command.h"

class MenuCategory;

class MenuItem {
public:
    MenuItem(const string &icon,
             const string &viewname,
             const string &cmd,
             const bool bTb = false,
             const string &sTip = "");

    string strIconPath;
    string strViewName;
    string strCmd;
    QKeySequence tKeySeq;
    MenuCategory *pChild;
    bool bToolbar;
    string strTip;
};

class MenuCategory {
protected:
    QKeySequence GetKeyNameToNum(const string &strKeyName);

public:
    MenuCategory(const string &strTitle);

    void AddItem(const string &strIconPath,
                 const string &strName,
                 const string &strCmd,
                 const bool bTb = false,
                 const string &sTip = "");

    vector<MenuItem> &GetMenuInfo();

    const string &GetTitle();

private:
    vector<MenuItem> _vPopMenu;
    vector<MLS::KeyInfo> _vKeyList;
    string _strTitle;
};

class LinMMenu {
public:
    LinMMenu(QMainWindow *pTarget, PanelCmd *pCmdList);

    ~LinMMenu();

    QMenu *AddCategory(MenuCategory &tMenu);

    static QMenu *PopupCategory(MenuCategory &tMenu,
                                QWidget *pWidget,
                                PanelCmd *pCmdList,
                                bool bTitle = true);

private:
    PanelCmd *_pCmdList;
    QMainWindow *_pDrawWidget;
    QMenuBar *_pMenuBar;
};

#endif
