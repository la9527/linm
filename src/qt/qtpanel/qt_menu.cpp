#include <QObject>
#include <QAction>
#include <QLabel>
#include <QToolBar>
#include <QMainWindow>
#include <QDebug>

#include "qt_menu.h"
#include "qt_dialog.h"

MenuItem::MenuItem(const string &icon,
                   const string &viewname,
                   const string &cmd,
                   const bool bTb,
                   const string &sTip) :
        strIconPath(icon), strViewName(viewname),
        strCmd(cmd), pChild(0), bToolbar(bTb), strTip(sTip) {
}

MenuCategory::MenuCategory(const string &strTitle) : _strTitle(strTitle) {
    _vKeyList = MLS::g_tKeyCfg.GetKeyInfo();
}

QKeySequence MenuCategory::GetKeyNameToNum(const string &strKeyName) {
    for (int n = 0; n < (int) _vKeyList.size(); n++) {
        MLS::KeyInfo tKeyInfo2 = _vKeyList[n];

        if (tKeyInfo2.sKeyName == strKeyName) {
            if (strKeyName.substr(0, 4) == "Alt+") {
                tKeyInfo2.vKeyInfo[0] = (int) Qt::ALT;
            }

            if (strKeyName.substr(0, 5) == "Ctrl+") {
                tKeyInfo2.vKeyInfo[0] = (int) Qt::CTRL;
            }

            if (strKeyName.substr(0, 5) == "Shift+") {
                tKeyInfo2.vKeyInfo[0] = (int) Qt::SHIFT;
            }

            if (tKeyInfo2.size() == 1)
                return QKeySequence(tKeyInfo2[0]);

            if (tKeyInfo2.size() == 2)
                return QKeySequence(tKeyInfo2[0] + tKeyInfo2[1]);

            if (tKeyInfo2.size() == 3)
                return QKeySequence(tKeyInfo2[0], tKeyInfo2[1], tKeyInfo2[2]);

            if (tKeyInfo2.size() == 4)
                return QKeySequence(tKeyInfo2[0], tKeyInfo2[1], tKeyInfo2[2], tKeyInfo2[3]);
        }
    }
    return QKeySequence();
}

void MenuCategory::AddItem(const string &strIconPath,
                           const string &strName,
                           const string &strCmd,
                           const bool bTb,
                           const string &sTip) {
    MenuItem tMenu(strIconPath, strName, strCmd, bTb, sTip);
    tMenu.tKeySeq = GetKeyNameToNum(MLS::g_tKeyCfg.CmdToKeyName(strCmd, PANEL));
    _vPopMenu.push_back(tMenu);
}

vector<MenuItem> &MenuCategory::GetMenuInfo() {
    return _vPopMenu;
}

const string &MenuCategory::GetTitle() {
    return _strTitle;
}

LinMMenu::LinMMenu(QMainWindow *pTarget, PanelCmd *pCmdList) {
    Q_CHECK_PTR(pTarget);
    Q_CHECK_PTR(pCmdList);

    _pDrawWidget = pTarget;
    _pCmdList = pCmdList;

    _pMenuBar = pTarget->menuBar();
    _pMenuBar->setSeparator(QMenuBar::InWindowsStyle);
}

LinMMenu::~LinMMenu() {
}

QMenu *LinMMenu::AddCategory(MenuCategory &tMenu) {
    vector<MenuItem> &vMenuItem = tMenu.GetMenuInfo();

    QToolBar *pToolbar = _pDrawWidget->addToolBar(_(tMenu.GetTitle().c_str()));
    QMenu *view = _pMenuBar->addMenu(tMenu.GetTitle().c_str());

    string sChildTitle;

    bool bToolBar = false;

    for (int n = 0; n < (int) vMenuItem.size(); n++) {
        MenuItem &tMenuItem = vMenuItem[n];

        if (tMenuItem.pChild) {
            qDebug() << "TEST !!!!!!!!!!!!!";

            QMenu *pChildMenu = AddCategory(*tMenuItem.pChild);

            if (pChildMenu) {
                pChildMenu->setTitle(tMenuItem.strViewName.c_str());
                view->addMenu(pChildMenu);
            }
            continue;
        }

        if (tMenuItem.strViewName.size()) {
            if (tMenuItem.strCmd.size()) {
                QString strCmd;

                if (tMenuItem.strCmd.substr(0, 4) == "Cmd_")
                    strCmd = tMenuItem.strCmd.substr(4).c_str();
                else
                    strCmd = tMenuItem.strCmd.c_str();

                strCmd.sprintf("%d%s()", QSLOT_CODE, (const char *) strCmd.toAscii());

                if (tMenuItem.strIconPath.size()) {
                    const QIcon &tIconData = LinMGlobal::GetIconSet(tMenuItem.strIconPath.c_str());

                    QAction *newAction = new QAction(tIconData, tMenuItem.strViewName.c_str(), _pDrawWidget);

                    newAction->setShortcut(tMenuItem.tKeySeq);
                    newAction->setStatusTip(tMenuItem.strTip.c_str());

                    QObject::connect(newAction, SIGNAL(triggered()), _pCmdList, (const char *) strCmd.toAscii());

                    view->addAction(newAction);
                    newAction->addTo(pToolbar);
                    bToolBar = true;
                } else {
                    QAction *newAction = new QAction(tMenuItem.strViewName.c_str(), _pDrawWidget);

                    newAction->setShortcut(QKeySequence(tMenuItem.tKeySeq));
                    newAction->setStatusTip(tMenuItem.strTip.c_str());

                    QObject::connect(newAction, SIGNAL(activated()), _pCmdList, (const char *) strCmd.toAscii());

                    view->addAction(newAction);
                }
            } else {
                view->addAction(tMenuItem.strViewName.c_str(),
                                _pCmdList,
                                SLOT(Cmd_Empty()),
                                QKeySequence(tMenuItem.tKeySeq));
            }
        } else {
            view->addSeparator();
        }
    }

    return view;
}

QMenu *LinMMenu::PopupCategory(MenuCategory &tMenu,
                               QWidget *pWidget,
                               PanelCmd *pCmdList,
                               bool bTitle) {
    vector<MenuItem> &vMenuItem = tMenu.GetMenuInfo();

    QMenu *view = new QMenu(pWidget);
    string sChildTitle;

    /*
    if( bTitle )
    {
        string sMsg;
        sMsg = "<font color=darkblue><u><b>";
        sMsg =+ tMenu.GetTitle().c_str();
        sMsg =+ "Context Menu</b></u></font>";

        QLabel *caption = new QLabel( sMsg, pWidget );
        caption->setAlignment( Qt::AlignCenter );
        view->insertItem( caption );
    }
    */

    for (int n = 0; n < (int) vMenuItem.size(); n++) {
        const MenuItem &tMenuItem = vMenuItem[n];
        const string &strCmd = tMenuItem.strCmd;

        if (tMenuItem.pChild) {
            QMenu *pChildMenu = PopupCategory(*tMenuItem.pChild, pWidget, pCmdList, false);

            if (pChildMenu) {
                pChildMenu->setTitle(tMenuItem.strViewName.c_str());
                view->addMenu(pChildMenu);
            }
            continue;
        }

        if (tMenuItem.strViewName.size()) {
            const QPixmap &tIconData = LinMGlobal::GetSmallIcon(tMenuItem.strIconPath.c_str());

            if (tMenuItem.strCmd.size()) {
                QString strCmd;
                strCmd.sprintf("%d%s()", QSLOT_CODE, tMenuItem.strCmd.c_str());

                if (tMenuItem.strIconPath.size()) {
                    const QIcon &tIconData = LinMGlobal::GetIconSet(tMenuItem.strIconPath.c_str());

                    QAction *newAction = new QAction(tIconData, tMenuItem.strViewName.c_str(), pWidget);

                    newAction->setShortcut(tMenuItem.tKeySeq);
                    newAction->setStatusTip(tMenuItem.strTip.c_str());

                    QObject::connect(newAction, SIGNAL(triggered()), pCmdList, (const char *) strCmd.toAscii());

                    newAction->setMenu(view);
                } else {
                    QAction *newAction = new QAction(tMenuItem.strViewName.c_str(), pWidget);

                    newAction->setShortcut(tMenuItem.tKeySeq);
                    newAction->setStatusTip(tMenuItem.strTip.c_str());

                    QObject::connect(newAction, SIGNAL(triggered()), pCmdList, (const char *) strCmd.toAscii());

                    newAction->setMenu(view);
                }
            } else {
                view->addAction(tMenuItem.strViewName.c_str(),
                                pCmdList,
                                SLOT(Cmd_Empty()),
                                tMenuItem.tKeySeq);
            }
        } else {
            view->addSeparator();
        }
    }

    view->exec(QCursor::pos());
    delete view;
    return NULL;
}
