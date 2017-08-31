#include <qlayout.h>
#include <qframe.h>
#include <qtabbar.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qsplitter.h>

#include <Q3ScrollView>
#include <QKeyEvent>
#include <QDebug>

#include "qt_dialog.h"
#include "qt_paneltooltip.h"
#include "qt_panel.h"
#include "qt_mcd.h"
#include "qt_statusbar.h"
#include "qt_tabpanel.h"
#include "qt_funcbar.h"

class QMcdPanel : public QSplitter {
public:
    QMcdPanel(Qt::Orientation o, QWidget *parent)
            : QSplitter(o, parent) {}

public:
    Qt_Panel *_pPanel;
    Qt_Mcd *_pMcd;
};

Qt_TabPanel::Qt_TabPanel(PanelToolTip *pToolTip,
                         PanelStatusBar *pStatusBar,
                         PanelCmd *pPanelCmd,
                         QWidget *parent) :
        QTabWidget(parent), _pToolTip(pToolTip), _pStatusBar(pStatusBar), _pPanelCmd(pPanelCmd) {


    qDebug() << "Qt_TabPanel::Qt_TabPanel";
    PanelInsert();

    QToolButton *addButton = new QToolButton(this);
    QToolButton *delButton = new QToolButton(this);

    const QIconSet &iconset = LinMGlobal::GetIconSet("add");
    addButton->setIconSet(iconset);
    setCornerWidget(addButton, Qt::TopLeft);

    const QIconSet &iconset2 = LinMGlobal::GetIconSet("remove");
    delButton->setIconSet(iconset2);
    setCornerWidget(delButton, Qt::TopRight);

    setFocusPolicy(Qt::NoFocus);
    setTabPosition(QTabWidget::South);

    connect(addButton, SIGNAL(clicked()), this, SLOT(PanelInsert()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(PanelRemove()));

    qDebug() << "Qt_TabPanel::Qt_TabPanel End";
}

Qt_TabPanel::~Qt_TabPanel() {
}

MLS::File *Qt_TabPanel::getCurFile() {
    QMcdPanel *pPanel = (QMcdPanel *) currentPage();
    return pPanel->_pPanel->GetCurFile();
}

Qt_Panel *Qt_TabPanel::getViewPanel() {
    QMcdPanel *pPanel = (QMcdPanel *) currentPage();
    return pPanel->_pPanel;
}

Qt_Mcd *Qt_TabPanel::getViewMcd() {
    QMcdPanel *pPanel = (QMcdPanel *) currentPage();
    return pPanel->_pMcd;
}

void Qt_TabPanel::Refresh() {
    return getViewPanel()->Refresh();
}

void Qt_TabPanel::setTabLabelChg(Qt_Panel *pPanel, const QString &strName) {
    for (int n = 0; n < count(); n++) {
        QMcdPanel *p = (QMcdPanel *) page(n);
        if (p->_pPanel == pPanel) {
            setTabLabel(p, strName);
            break;
        }
    }
}

void Qt_TabPanel::PanelInsert() {
    QMcdPanel *pPanelMcd = new QMcdPanel(Qt::Horizontal, this);

    Qt_Panel *pPanel = new Qt_Panel(_pToolTip, _pStatusBar,
                                    _pPanelCmd, this, pPanelMcd);
    Qt_Mcd *pMcd = new Qt_Mcd(pPanel, pPanelMcd);

    pPanelMcd->setResizeMode(pMcd, QSplitter::KeepSize);
    pPanelMcd->setResizeMode(pPanel, QSplitter::KeepSize);
    pPanelMcd->moveToFirst(pMcd);

    qDebug() << "PanelInsert() :: 1";
    pPanel->Read("~");
    pPanel->setBackgroundColor(Qt::white);
    pPanel->setMinimumSize(400, 200);

    qDebug() << "PanelInsert() :: 2";

    pMcd->addColumn("Name");
    pMcd->setTreeStepSize(20);
    pMcd->InitMcd(pPanel->GetReader(), "/");
    pMcd->setBackgroundColor(Qt::white);
    pMcd->setColumnWidth(0, 250);
    pMcd->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum));
    pMcd->setVScrollBarMode(Q3ScrollView::AlwaysOn);
    pMcd->setMinimumSize(100, 100);
    pMcd->setDir(pPanel->GetPath());

    qDebug() << "PanelInsert() :: 3";

    //pMcd->show();
    //pPanel->show();

    pPanelMcd->_pMcd = pMcd;
    pPanelMcd->_pPanel = pPanel;

    const QIconSet &iconset = LinMGlobal::GetIconSet("folder");
    addTab(pPanelMcd, iconset, "~");

    setCurrentPage(count() - 1);
    qDebug() << "PanelInsert() :: 4";
}

void Qt_TabPanel::PanelRemove() {
    if (count() > 1) {
        QMcdPanel *pPanelMcd = (QMcdPanel *) currentPage();

        if (pPanelMcd) {
            removePage(pPanelMcd);
            pPanelMcd->close();
            delete pPanelMcd;
            pPanelMcd = 0;
        }
    }
}

void Qt_TabPanel::keyPressEvent(QKeyEvent *event) {
    qDebug("Qt_TabPanel [%d]", event->key());
    switch (event->key()) {
        case Qt::Key_Left:
            qDebug("QKeyEvent :: KEYLEFT");
            break;
        case Qt::Key_Right:
            qDebug("QKeyEvent :: KEYRIGHT");
            break;
    }

    QWidget::keyPressEvent(event);
}

void Qt_TabPanel::focusInEvent(QFocusEvent *event) {
    qDebug("Qt_TabPanel :: focusInEvent");
    Qt_Panel *pPanel = (Qt_Panel *) currentPage();
    pPanel->setActiveWindow();
    pPanel->setFocus();
}

CentralMain::CentralMain(PanelToolTip *pToolTip,
                         PanelCmd *pPanelCmd,
                         QWidget *parent,
                         const char *name)
        : QFrame(parent, name) {
    //setBackgroundMode(PaletteBase);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setContentsMargins(0, 0, 0, 0);

    _pToolTip = pToolTip;
    _pPanelCmd = pPanelCmd;

    _pStatusBar = new PanelStatusBar(this);
    _pStatusBar->setFixedHeight(20);

    _pFuncBar = new FuncBar(this, pPanelCmd);
    _pFuncBar->init(PANEL);
    _pFuncBar->setFixedHeight(20);

    _pLeftTabWidget = new Qt_TabPanel(pToolTip, _pStatusBar, _pPanelCmd, this);
    _pRightTabWidget = new Qt_TabPanel(pToolTip, _pStatusBar, _pPanelCmd, this);

    _QHbox = 0;
    _QVbox = 0;

    _bSplit = false;
    _bViewType = false;

    DrawPanel();

    _pLeftTabWidget->setFocus();
    GetFocusPanel()->setFocus();
}

void CentralMain::DrawPanel() {
    qDebug() << "CentralMain::DrawPanel() start";
    if (_QHbox) delete _QHbox;
    if (_QVbox) delete _QVbox;

    _QHbox = 0;
    _QVbox = 0;

    if (!_bSplit) {
        _QHbox = new QHBoxLayout;
        _QVbox = new QVBoxLayout(this);
        _QHbox->setMargin(0);
        _QHbox->setSpacing(0);
        _QVbox->setMargin(0);
        _QVbox->setSpacing(0);

        if (_pLeftTabWidget->getViewPanel()->_bFocus) {
            _QHbox->add(_pLeftTabWidget);
            _pRightTabWidget->hide();
        } else {
            _QHbox->add(_pRightTabWidget);
            _pLeftTabWidget->hide();
        }

        _QVbox->addLayout(_QHbox);
        _QVbox->add(_pStatusBar);
        _QVbox->add(_pFuncBar);
    } else if (_bSplit && !_bViewType) {
        _QHbox = new QHBoxLayout;
        _QVbox = new QVBoxLayout(this);
        _QHbox->setMargin(0);
        _QHbox->setSpacing(0);
        _QVbox->setMargin(0);
        _QVbox->setSpacing(0);

        _QHbox->add(_pLeftTabWidget);
        _QHbox->add(_pRightTabWidget);

        _pLeftTabWidget->show();
        _pRightTabWidget->show();

        _QVbox->addLayout(_QHbox);
        _QVbox->setMargin(0);
        _QVbox->add(_pStatusBar);
        _QVbox->add(_pFuncBar);
    } else if (_bSplit && _bViewType) {
        _QHbox = new QVBoxLayout;
        _QVbox = new QVBoxLayout(this);
        _QHbox->setMargin(0);
        _QHbox->setSpacing(0);
        _QVbox->setMargin(0);
        _QVbox->setSpacing(0);

        _QHbox->add(_pLeftTabWidget);
        _QHbox->add(_pRightTabWidget);

        _pLeftTabWidget->show();
        _pRightTabWidget->show();

        _QVbox->addLayout(_QHbox);
        _QVbox->setMargin(0);
        _QVbox->add(_pStatusBar);
        _QVbox->add(_pFuncBar);
    }

    _QVbox->activate();

    _pStatusBar->show();
    qDebug() << "CentralMain::DrawPanel() end..";
}

void CentralMain::Split() {
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

    DrawPanel();
    //Refresh();
}

Qt_Panel *CentralMain::GetFocusPanel() {
    if (_pRightTabWidget->getViewPanel()->_bFocus)
        return _pRightTabWidget->getViewPanel();

    if (!_pLeftTabWidget->getViewPanel()->_bFocus)
        _pLeftTabWidget->getViewPanel()->setFocus();

    return _pLeftTabWidget->getViewPanel();
}

Qt_Mcd *CentralMain::GetFocusMcd() {
    if (_pRightTabWidget->getViewPanel()->_bFocus)
        return _pRightTabWidget->getViewMcd();

    if (!_pLeftTabWidget->getViewPanel()->_bFocus)
        _pLeftTabWidget->getViewPanel()->setFocus();

    return _pLeftTabWidget->getViewMcd();
}

void CentralMain::Refresh() {
    _pLeftTabWidget->Refresh();
    _pRightTabWidget->Refresh();
}
