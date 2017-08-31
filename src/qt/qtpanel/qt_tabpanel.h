#ifndef __QT_TABPANEL_H__
#define __QT_TABPANEL_H__

#include <qframe.h>
#include <qtabwidget.h>

#include "file.h"

class Qt_Panel;

class PanelToolTip;

class PanelStatusBar;

class PanelCmd;

class FuncBar;

class QBoxLayout;

class Qt_Mcd;

class Qt_TabPanel : public QTabWidget {
    Q_OBJECT

public:
    Qt_TabPanel(PanelToolTip *pToolTip,
                PanelStatusBar *pStatusBar,
                PanelCmd *pPanelCmd,
                QWidget *parent = 0);

    ~Qt_TabPanel();

    MLS::File *getCurFile();

    Qt_Panel *getViewPanel();

    Qt_Mcd *getViewMcd();

    void setTabLabelChg(Qt_Panel *pPanel, const QString &strName);

    void Refresh();

protected
    slots: void PanelInsert();

    void PanelRemove();

protected:
    void keyPressEvent(QKeyEvent *event);

    void focusInEvent(QFocusEvent *event);

private:
    PanelToolTip *_pToolTip;
    PanelStatusBar *_pStatusBar;
    PanelCmd *_pPanelCmd;
};

class CentralMain : public QFrame {
public:
    CentralMain(PanelToolTip *pToolTip,
                PanelCmd *pPanelCmd,
                QWidget *parent = 0, const char *name = 0);

    Qt_Panel *GetFocusPanel();

    Qt_Mcd *GetFocusMcd();

    void Refresh();

    void Split();

    void DrawPanel();

private:
    Qt_TabPanel *_pLeftTabWidget;
    Qt_TabPanel *_pRightTabWidget;

    QBoxLayout *_QHbox;
    QBoxLayout *_QVbox;

    PanelToolTip *_pToolTip;
    PanelStatusBar *_pStatusBar;
    PanelCmd *_pPanelCmd;
    FuncBar *_pFuncBar;

    bool _bSplit;
    bool _bViewType;
};

#endif // __QT_TABPANEL_H__
