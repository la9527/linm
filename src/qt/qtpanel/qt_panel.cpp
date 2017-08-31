#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QGroupBox>
#include <QScrollBar>
#include <QFont>
#include <QColor>
#include <QTimer>
#include <QDateTime>
#include <QApplication>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QDebug>

#ifdef LINM_KDE
#include <KDE/KMimeType>
#include <KDE/KUrl>
#else

#endif

#include "define.h"
#include "mpool.h"
#include "keycfgload.h"

#include "reader.h"
#include "panel.h"

#include "qt_dialog.h"
#include "qt_paneltooltip.h"
#include "qt_statusbar.h"

#include "qt_panelfilebox.h"
#include "qt_tabpanel.h"
#include "qt_panel.h"
#include "qt_mcd.h"
#include "qt_command.h"

Qt_PanelFriend::Qt_PanelFriend(QWidget *parent)
        : QWidget(parent) {
    setMinimumSize(200, 200);
    //setMaximumSize( 100, 100);
}

Qt_PanelFriend::~Qt_PanelFriend() {
}

Qt_Panel::Qt_Panel(PanelToolTip *pToolTip,
                   PanelStatusBar *pStatusBar,
                   PanelCmd *pPanelCmd,
                   Qt_TabPanel *pTabPanel,
                   QWidget *parent)
        :
        Qt_PanelFriend(parent),
        MLS::Panel(),
        _pStatusBar(pStatusBar),
        _pPanelCmd(pPanelCmd),
        _pToolTip(pToolTip),
        _pTabParent(pTabPanel) {
    qDebug() << "Qt_Panel::Qt_Panel";

    /*--------------------------------------------------*/
    /* Background black */
    QColor color(255, 255, 255);
    QPalette palette;
    palette.setColor(QPalette::Background, color);

    setPalette(palette);
    setAutoFillBackground(true);
    /*--------------------------------------------------*/

    setFocusPolicy(Qt::TabFocus);
    setFocusPolicy(Qt::ClickFocus); // for menu

    _nViewColumn = 0;
    _bViewRowFixed = false;
    _bFocus = false;
    _bScrollChg = false;
    _nDefFontWidth = 12;

    _pScroolBar = new QScrollBar(this);
    _pScroolBar->setFixedSize(18, height() - 20);
    _pScroolBar->setPageStep(1);

    _pixScreenshot = QPixmap::grabWindow(QApplication::desktop()->winId());
    _bTranslucency = false;
    _bWheelFileUpdown = true;

    _bTitleButton = false;

    _nBefSec = -1;

    connect(_pScroolBar, SIGNAL(valueChanged(int)), this, SLOT(scrollbarChg()));

    setMouseTracking(TRUE);
    setFocusPolicy(Qt::StrongFocus);

    _pStatusBar->setPanel(this);

    qDebug() << "Qt_Panel::Qt_Panel End";
}

Qt_Panel::~Qt_Panel() {
}

void Qt_Panel::setPanelToolTip(PanelToolTip *pToolTip) {
    _pToolTip = pToolTip;

    if (_pToolTip)
        _pToolTip->HideToolTip();
}

void Qt_Panel::setStatusBar(PanelStatusBar *pStatusBar) {
    _pStatusBar = pStatusBar;
}

void Qt_Panel::wheelEvent(QWheelEvent *event) {
    qDebug("wheel event x [%d] y [%d] delta [%d]", event->x(), event->y(), event->delta());

    if (!_bWheelFileUpdown) {
        int nPage = _pScroolBar->value();

        if (event->delta() < 0)
            nPage++;
        else if (event->delta() > 0)
            nPage--;

        if (_pScroolBar->maxValue() >= nPage && _pScroolBar->minValue() <= nPage)
            _pScroolBar->setValue(nPage);
    } else {
        if (focusWidget() == this) {
            if (event->delta() < 0)
                MLS::Panel::Key_Down();
            else if (event->delta() > 0)
                MLS::Panel::Key_Up();

            ChangeFocusUpdate(_uCur);
        }
    }
}

void Qt_Panel::moveEvent(QMoveEvent * /*event*/ ) {
#ifdef Q_WS_X11
    if( _bTranslucency )
    {
        //_pixScreenshot = QPixmap::grabWindow( qt_xrootwin() );
        //update();
        qDebug("_bTranslucency  ~~~~~~~ HideToolTip !!!!!!!");
    }
#endif
}

void Qt_Panel::ChangeFocusUpdate(uint uCur) {
    int nBefCur = 0, nCur = 0, nBefPage = 0, nPage = 0;

    if (_nCol != 0 && _nRow != 0) {
        nBefCur = _uBefCur % (_nCol * _nRow);
        nCur = uCur % (_nCol * _nRow);
        nBefPage = _uBefCur / (_nCol * _nRow);
        nPage = uCur / (_nCol * _nRow);
    }

    qDebug("Page [%d] [%d] [%d]", nBefPage, nPage, _pScroolBar->value());

    if (_uBefCur == uCur) return;
    if (nBefPage != nPage) {
        _bScrollChg = true; // 스크롤이 키버튼으로 옮겨질 경우.
        _pScroolBar->setValue(nPage + 1);
        Refresh();
    } else {
        Qt_FileBox *pFileBox = NULL;

        if ((int) _vDrawFileList.size() <= nBefCur)
            throw Exception("Program Error !!!");

        pFileBox = _vDrawFileList[nBefCur];
        pFileBox->SetFocus(false);
        update(pFileBox->GetQRect());

        pFileBox = _vDrawFileList[nCur];
        pFileBox->SetFocus(true);
        update(pFileBox->GetQRect());
    }
}

void Qt_Panel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        Qt_FileBox *pFileBox = NULL;

        for (int n = 0; n < (int) _vDrawFileList.size(); n++) {
            pFileBox = _vDrawFileList[n];
            if (pFileBox) {
                if (pFileBox->AreaChk(event->x(), event->y())) {
                    _uCur = (_nCol * _nRow) * _nPage + n;
                    ChangeFocusUpdate(_uCur);
                    break;
                }
            }
        }
    }

    _pToolTip->HideToolTip();
}

void Qt_Panel::TabLabelChg() {
    if (_pTabParent) {
        string strView = GetPathView();

        MLS::Reader *pReader = GetReader("file");

        if (strView == pReader->GetRealPath("~"))
            strView = "~";
        else {
            StringToken tToken(strView, "/");

            while (tToken.Next())
                strView = tToken.Get();
        }

        _pTabParent->setTabLabelChg(this, strView.c_str());
    }
}

void Qt_Panel::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        Qt_FileBox *pFileBox = NULL;

        for (int n = 0; n < (int) _vDrawFileList.size(); n++) {
            pFileBox = _vDrawFileList[n];
            if (pFileBox && pFileBox->GetFile()) {
                if (pFileBox->AreaChk(event->x(), event->y())) {
                    uint nCur = (_nCol * _nRow) * _nPage + n;

                    if (nCur < _vDirFiles.size()) {
                        _uCur = nCur;
                        _pPanelCmd->Enter();
                    }
                    break;
                }
            }
        }
    }
}

void Qt_Panel::keyPressEvent(QKeyEvent *event) {
    qDebug() << "Qt_Panel::keyPressEvent";
    int key = event->key();

    Qt::KeyboardModifiers eKeyStat = event->modifiers();

    qDebug("key keyPressEvent !!! Num [%d] [%s]", key, (const char *) event->text().toLocal8Bit());

    MLS::KeyInfo tKey(key);

    switch (eKeyStat) {
        case Qt::ShiftModifier: {
            qDebug() << "SHIFT";
            tKey.vKeyInfo.insert(tKey.vKeyInfo.begin(), Qt::Key_Shift);
            break;
        }
        case Qt::AltModifier:
            qDebug() << "ALT";
            tKey.vKeyInfo.insert(tKey.vKeyInfo.begin(), Qt::Key_Alt);
            break;
        case Qt::ControlModifier:
            qDebug() << "CTRL";
            tKey.vKeyInfo.insert(tKey.vKeyInfo.begin(), Qt::Key_Control);
            break;
    }

    string sCmd = MLS::g_tKeyCfg.GetCommand(tKey, PANEL);

    if (sCmd.size()) {
        String strCmd;

        if (sCmd.substr(0, 4) == "Cmd_")
            strCmd = sCmd.substr(4);

        strCmd.Printf("%d%s()", QSLOT_CODE, strCmd.c_str());

        qDebug("key button.. !!! Num [%d] [%s]", key, sCmd.c_str());

        connect(this, SIGNAL(CmdKeyExecute()), _pPanelCmd, strCmd.c_str());
        emit CmdKeyExecute();
        disconnect(this, SIGNAL(CmdKeyExecute()), _pPanelCmd, strCmd.c_str());

        _pScroolBar->update();
        ChangeFocusUpdate(_uCur);
    } else {
        qDebug("Not found... !!! Num [%d]", key);
        QWidget::keyPressEvent(event);
    }

    _pToolTip->HideToolTip();
}

void Qt_Panel::mouseMoveEvent(QMouseEvent *event) {
    //QTime t = QTime::currentTime();             // set random seed
    //if ( _nBefSec ==  t.second() ) return;

    //_nBefSec = t.second();

    //qDebug( "mouseMoveEvent [%d] [%d]", event->globalX(), event->globalY() );
    if (!_bFocus) return;
    if (event->button() != Qt::NoButton) return;

    Qt_FileBox *pFileBox = NULL;
    bool bFind = false;

    for (int n = 0; n < (int) _vDrawFileList.size(); n++) {
        pFileBox = _vDrawFileList[n];
        if (pFileBox) {
            if (pFileBox->AreaChk(event->x(), event->y())) {
                if (pFileBox->GetFile()) {
                    MLS::File *pFile = pFileBox->GetFile();
                    _pToolTip->setFile(pFile);

                    //qDebug("Mouse Event [%s]", pFile->sName.c_str() );
                    _pToolTip->move(event->globalX(), event->globalY() + 20);
                    return;
                }
            }
        }
    }

    if (!bFind) {
        _pToolTip->HideToolTip();
    }
}

void Qt_Panel::focusOutEvent(QFocusEvent * /*event*/ ) {
    if (_pToolTip)
        _pToolTip->HideToolTip();

    _bFocus = false;
    //update();

    //releaseKeyboard();
}

void Qt_Panel::focusInEvent(QFocusEvent * /*event*/ ) {
    //grabKeyboard();
    _bFocus = true;
    //update();
    _pStatusBar->setPanel(this);
    _pStatusBar->update();
}

void Qt_Panel::scrollbarChg() {
    qDebug("scrollbarChg() Bef [%d]", _uCur);

    if (!_bScrollChg) {
        if (_pScroolBar->value() > 1)
            _uCur = (_pScroolBar->value() - 1) * (_nCol * _nRow) + (_uBefCur % (_nCol * _nRow));
        else
            _uCur = _uBefCur % (_nCol * _nRow);

        if (_uCur >= _vDirFiles.size())
            _uCur = _vDirFiles.size() - 1;

        qDebug("scrollbarChg() Aft [%d]", _uCur);
        update();
    } else
        _bScrollChg = false;
}

void Qt_Panel::paintEvent(QPaintEvent */*event*/) {
    qDebug() << "Qt_Panel::paintEvent";

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);

    if (_bTranslucency) {
        QPixmap backPixmap;

        backPixmap.resize(size());
        bitBlt(&backPixmap, 0, 0, &_pixScreenshot, x(), y(), size().width(), size().height());

/*
		KPixmap background( backPixmap );
		KPixmapEffect::fade( background, 0.3, backgroundColor() );
		painter.drawPixmap( 0, 0, background );
*/
        painter.drawPixmap(0, 0, backPixmap);
    }

    QFontMetrics fm(font());
    _nDefFontWidth = fm.width("A");
    _nDefBoxHeight = fm.height() + 6;

    QPen pen(Qt::gray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    // Background Color Setting

    // Title 을 제외한 크기
    _viewPanelBox = QRect(2, 2 + _nDefBoxHeight, width() - 20, height() - _nDefBoxHeight - 2);

    InitDraw(_viewPanelBox);

    QRect viewBox = _viewPanelBox;

    viewBox.setY(viewBox.y());
    viewBox.setWidth(viewBox.width() - 2);
    viewBox.setHeight((_nRow * _nDefBoxHeight) + 4);

    //painter.fillRect(rect,  QBrush( Qt::gray ));
    //painter.setBackgroundColor( Qt::white );
    //painter.setBrush( QBrush( Qt::gray ) );
    //painter.drawRect( viewBox );

    int nNum = 0;
    Qt_FileBox *pFileBox = NULL;
    Qt_FileBox *pTitleBox = NULL;
    MLS::File *pFile = NULL;

    if (_nCol != 0 && _nRow != 0)
        _nPage = _uCur / (_nCol * _nRow);

    _nItemWidth = (viewBox.width() - (_nCol * 2)) / _nCol;

    int nCur = (_nCol * _nRow) * _nPage;

    int nColView = 0;
    for (int nCol = 0; nCol < _nCol; nCol++) {
        for (int nRow = 0; nRow < _nRow; nRow++) {
            pFileBox = _vDrawFileList[nNum];
            if (nCur < (int) _vDirFiles.size()) {
                pFile = _vDirFiles[nCur];

                pFileBox->SetWidget(this);
                pFileBox->height = _nDefBoxHeight;
                pFileBox->width = _nItemWidth;
                pFileBox->x = viewBox.x() + (nCol * (pFileBox->width + 2));
                pFileBox->y = viewBox.y() + (nRow * (pFileBox->height));

                //qDebug() << "Qt_Panel FILENAME ===";
                //qDebug() << "Qt_Panel FILENAME :: " << pFile->sName.c_str();

                if (nCur == (int) _uCur && _bFocus)
                    pFileBox->SetFile(pFile, true, nCur, _bShowFileOwner, _nDefFontWidth);
                else
                    pFileBox->SetFile(pFile, false, nCur, _bShowFileOwner, _nDefFontWidth);

                pFileBox->Show();
                nColView = nCol;
            } else
                pFileBox->SetFile(NULL, false, -1, _bShowFileOwner, _nDefFontWidth);
            nCur++;
            nNum++;
        }
    }
    _uBefCur = _uCur;

    // Title
    for (int nCol = 0; nCol < nColView + 1; nCol++) {
        if (nCol < (int) _vDrawTitleList.size()) {
            pTitleBox = _vDrawTitleList[nCol];
            pTitleBox->SetWidget(this);
            pTitleBox->height = _nDefBoxHeight;
            pTitleBox->width = _nItemWidth;

            if (_bTitleButton)
                pTitleBox->x = viewBox.x() + (nCol * pTitleBox->width);
            else
                pTitleBox->x = viewBox.x() + (nCol * (pTitleBox->width + 2));
            pTitleBox->y = _viewPanelBox.y() - _nDefBoxHeight - 1;
            pTitleBox->SetFontSize(_nDefFontWidth);
            qDebug() << "Title Write.......";
            pTitleBox->Show();
        }
    }

    _pScroolBar->setFixedSize(20, _viewPanelBox.height() + _nDefBoxHeight);
    _pScroolBar->move(viewBox.x() + viewBox.width(), _viewPanelBox.y() - _nDefBoxHeight);

    _pScroolBar->setMinValue(1);
    if (_nCol != 0 && _nRow != 0) {
        int nMaxPage = ((_vDirFiles.size() - 1) / (_nCol * _nRow)) + 1;
        qDebug("Page [%d] [%d] [%d]", nMaxPage, (int) _vDirFiles.size(), _nCol * _nRow);
        _pScroolBar->setMaxValue(nMaxPage);
    } else
        _pScroolBar->setMaxValue(1);

    _pStatusBar->update();

    qDebug() << "Qt_Panel::paintEvent End.................";
}

// _nCol, _nRow getsize 
void Qt_Panel::InitDraw(const QRect &drawRect) {
    //qDebug() << "Qt_Panel::InitDraw";

    int nSize = _vDirFiles.size();

    const int nMaxlen = (20 * _nDefFontWidth) + 25;
    int nViewBoxHeight = drawRect.height();

    // . 자동 컬럼 지정 모드
    // 1. 파일 개수에 맞게 컬럼수 지정 -> 모드 선택
    if (_nViewColumn == 0 || _nViewColumn > 6) {
        if (nSize * _nDefBoxHeight <= nViewBoxHeight) _nCol = 1;
        else if (nSize * _nDefBoxHeight <= nViewBoxHeight * 2) _nCol = 2;
        else if (nSize * _nDefBoxHeight <= nViewBoxHeight * 3) _nCol = 3;
        else if (nSize * _nDefBoxHeight <= nViewBoxHeight * 4) _nCol = 4;
        else if (nSize * _nDefBoxHeight <= nViewBoxHeight * 5) _nCol = 5;
        else _nCol = 6;

        // 최대 가능 컬럼수  maxcol = _cols / 12(최소 열두자)
        // 적어도 12자는 보이게 하자 ( Icon 까지 )
        if (_nCol > drawRect.width() / nMaxlen)
            _nCol = drawRect.width() / nMaxlen;
    } else {
        _nCol = _nViewColumn;
    }

    if (_bViewRowFixed && (nSize + _nCol - 1) / _nCol <= (drawRect.height() / _nDefBoxHeight))
        _nRow = (nSize + _nCol - 1) / _nCol;
    else
        _nRow = nViewBoxHeight / _nDefBoxHeight;

    int nNewSize = _nCol * _nRow;
    if (_nBefMemSize != nNewSize) {
        _tMemPoolFileBox.Clear();
        _tMemPoolTitleBox.Clear();
        _vDrawFileList.clear();
        _vDrawTitleList.clear();

        // Title List
        for (int n = 0; n < _nCol; n++) {
            Qt_FileBox &tTitleBox = _tMemPoolTitleBox.Get();
            tTitleBox.setFileBox(QColor(0, 255, 255),
                                 QColor(0, 0, 0),
                                 QColor(150, 150, 150, 150),
                                 QColor(220, 220, 220),
                                 true, _bTitleButton);
            _vDrawTitleList.push_back(&tTitleBox);
        }

        // FileList
        for (int n = 0; n < _nCol * _nRow; n++) {
            Qt_FileBox &tFileBox = _tMemPoolFileBox.Get();
            tFileBox.setFileBox(QColor(200, 200, 255),
                                QColor(0, 0, 0),
                                QColor(255, 255, 255),
                                Qt::gray);
            _vDrawFileList.push_back(&tFileBox);
        }
    }
    _nBefMemSize = _vDrawFileList.size();
    //qDebug() << "Qt_Panel::InitDraw End...";
}

void Qt_Panel::Refresh() {
    //repaint( true );
    qDebug() << "Qt_Panel::Refresh";
    repaint();
    qDebug() << "Qt_Panel::Refresh End...";
}

void Qt_Panel::Draw() {
    qDebug() << "Qt_Panel::Draw";
    if (_uBefCur != _uCur)
        update();
    qDebug() << "Qt_Panel::Draw End...";
}

void Qt_Panel::ParseAndRun(const string &sCmd, bool bPause) {
    _pPanelCmd->ParseAndRun(sCmd, bPause);
}

void Qt_Panel::Execute(const string &sCmd) {
    if (sCmd.size()) {
        String strCmd;

        if (sCmd.substr(0, 4) == "Cmd_")
            strCmd = sCmd.substr(4);

        strCmd.Printf("%d%s()", QSLOT_CODE, strCmd.c_str());

        qDebug("Execute :: -------- [%s]", strCmd.c_str());

        connect(this, SIGNAL(CmdExecute()), _pPanelCmd, strCmd.c_str());
        emit CmdExecute();
        disconnect(this, SIGNAL(CmdExecute()), _pPanelCmd, strCmd.c_str());

        ChangeFocusUpdate(_uCur);
    }
    qDebug("Qt_Panel::Execute :: -------- [%s]", sCmd.c_str());
}

void Qt_Panel::ReadEnd() {
    MLS::File *pFile = NULL;

    for (int n = 0; n < _vDirFiles.size(); n++) {
        pFile = _vDirFiles[n];
        if (pFile) {
#ifdef LINM_KDE
            if ( pFile->bDir )
                pFile->sTmp3 = "folder";
            else
                pFile->sTmp3 = (const char*)KMimeType::iconNameForUrl( KUrl( pFile->sName.c_str() ) ).toAscii(); // Tmp3 to iconName.
#else
            pFile->sTmp3 = pFile->Ext().c_str();
            if (pFile->sTmp3.empty())
                pFile->sTmp3 = pFile->sName.c_str();
#endif
        }
    }
}

