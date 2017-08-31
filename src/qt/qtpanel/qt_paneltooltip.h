#ifndef __QT_PANELTOOLTIP_H__
#define __QT_PANELTOOLTIP_H__

#include <qwidget.h>
#include <qpixmap.h>

#include "file.h"

class QColor;

class QString;

class QRect;

class QSize;

class PanelToolTip : public QWidget {
    Q_OBJECT

public:
    PanelToolTip(QWidget *parent = 0);

    ~PanelToolTip();

    void setFile(MLS::File *pFile);

public
    slots:
    void

    ShowToolTip();

    void HideToolTip();

protected:
    void DrawToolTip(const QSize &rect);

    void moveEvent(QMoveEvent *event);

    void showEvent(QShowEvent *event);

    void paintEvent(QPaintEvent *event);

    void keyPressEvent(QKeyEvent *event);

private:
    MLS::File *_pFile;
    QColor _lineColor;
    QColor _backColor;
    QColor _font1Color;
    QColor _font2Color;
    int _nDefFontWidth;
    bool _bHide;
    QString _sInfo1;
    QString _sInfo2;
    QTimer *_pShowTimer;
    QTimer *_pHideTimer;

    QRect _rectToolTip;
    bool _bTranslucency;
    QPixmap _pixScreenshot;
    QPixmap _pixBuffer;
    int _nDuration;
};

#endif // __QT_PANELTOOLTIP_H__
