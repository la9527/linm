#ifndef QT_PANEL_H
#define QT_PANEL_H

#include "define.h"
#include "mpool.h"
#include "file.h"
#include "panel.h"

#include <QWidget>
#include <QPixmap>
#include <QVector>
#include <QPushButton>

class QRect;

class QScrollBar;

class QKeyEvent;

class QWheelEvent;

class QMouseEvent;

class QPaintEvent;

class Qt_FileBox;

class PanelToolTip;

class PanelStatusBar;

class PanelCmd;

class Qt_TabPanel;

class Qt_PanelFriend : public QWidget {
    Q_OBJECT

public:
    Qt_PanelFriend(QWidget *parent = 0);

    ~Qt_PanelFriend();

public
    slots:

    virtual void scrollbarChg() = 0;

    signals:
    void

    CmdKeyExecute();

    void CmdExecute();
};

class Qt_Panel : public Qt_PanelFriend, public MLS::Panel {
    friend class Qt_PanelFriend; // QT 의 Q_OBJECT 떄메 ㅠㅠ

public:
    Qt_Panel(PanelToolTip *pToolTip,
             PanelStatusBar *pStatusBar,
             PanelCmd *pPanelCmd,
             Qt_TabPanel *pTabPanel,
             QWidget *parent);

    ~Qt_Panel();

    void setPanelToolTip(PanelToolTip *pToolTip);

    void setStatusBar(PanelStatusBar *pStatusBar);

    void Refresh();

    void TabLabelChg();


public:
    bool _bFocus;            /// 현재 포커스를 가지고 있는가?

protected:
    void InitDraw(const QRect &drawRect);

    void ChangeFocusUpdate(uint uCur);

    void ParseAndRun(const string &sCmd, bool bPause);

    void Execute(const string &sCmd);

    void Draw();

    void ReadEnd();

protected:
    void keyPressEvent(QKeyEvent *event);

    void wheelEvent(QWheelEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

    void moveEvent(QMoveEvent *event);

    void focusOutEvent(QFocusEvent *event);

    void focusInEvent(QFocusEvent *event);

    void scrollbarChg();


private:
    QVector<int> _vKeyData;
    int _nItemWidth;
    int _nViewColumn;
    QVector<Qt_FileBox *> _vDrawFileList;
    QVector<Qt_FileBox *> _vDrawTitleList;
    MLS::MemoryPool<Qt_FileBox> _tMemPoolFileBox;
    MLS::MemoryPool<Qt_FileBox> _tMemPoolTitleBox;

    int _nBefPage;            /// 화면 새로고침은 위한.
    int _nBefMemSize;
    uint _uBefCur;
    bool _bViewRowFixed;    /// Rows 고정
    bool _bShowFileOwner;    /// Show 파일 Owner/Group을 보여줄 것인가?

    int _nDefFontWidth;
    int _nDefBoxHeight;
    QRect _viewPanelBox;
    QScrollBar *_pScroolBar;
    bool _bScrollChg;

    PanelStatusBar *_pStatusBar;

    PanelCmd *_pPanelCmd;
    PanelToolTip *_pToolTip;
    Qt_TabPanel *_pTabParent;

    int _nBefSec;

    QPixmap _pixScreenshot;
    bool _bTranslucency;        // 투명여부
    bool _bWheelFileUpdown;    // 휠 사용시 파일 up/down 여부
    bool _bTitleButton;        // 타이틀을 버튼으로 만듬.
};

#endif // QT_PANEL_H
