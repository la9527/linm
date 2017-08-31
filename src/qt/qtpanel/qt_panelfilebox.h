#ifndef __QT_PANELFILEBOX_H__
#define    __QT_PANELFILEBOX_H__

#include "define.h"
#include "file.h"

#include <qobject.h>
#include <qpushbutton.h>

enum POSITION {
    LEFT,
    RIGHT,
    MIDDLE,
    TOP,
    BOTTOM,
    CURRENT
};

class Position : public QObject {
public:
    Position() : QObject(), _ObjPosY(CURRENT), _ObjPosX(CURRENT) {}

    virtual ~Position() {}

    void SetWidget(QWidget *pParent) {
        _pWidget = pParent;
    }

    inline void Move(POSITION posX, POSITION posY) {
        _ObjPosY = posY;
        _ObjPosX = posX;
    }

    inline void Move(POSITION posX, int Y) {
        _ObjPosX = posX;
        y = Y;
    }

    inline void Move(int X, POSITION posY) {
        _ObjPosY = posY;
        x = X;
    }

    inline void Move(int X, int Y) {
        x = X;
        y = Y;
    }

    inline bool AreaChk(int X, int Y) {
        if (y <= Y && y + height > Y)
            if (x <= X && x + width > X)
                return true;
        return false;
    }

    void Show() {
        Resize();
        Draw();
    }

    int x, y, height, width;
    int fontcolor;
    int backcolor;

protected:
    virtual void Draw() = 0;

    void Resize();

    QWidget *_pWidget;
    bool _bShow;
    POSITION _ObjPosY, _ObjPosX;
};

class TabViewInfo {
public:
    string sName;
    int nWidth;

public:
    TabViewInfo(const string &str = "", int w = 0) : sName(str), nWidth(w) {}
};

class TitleButtonItem {
public:
    QWidget *_pParent;
    QPushButton *_pButton;
    QString _strTitleName;
    QRect _rect;

    TitleButtonItem(QWidget *pParent, const QString &strTitleName, const QRect &rect)
            : _pParent(pParent), _strTitleName(strTitleName), _rect(rect), _pButton(0) {}
};

class QRect;

class QColor;

class QPushButton;

class Qt_FileBox : public Position {
    Q_OBJECT

public:
    Qt_FileBox();

    ~Qt_FileBox();

    void setFileBox(const QColor &focusColor = QColor(Qt::blue),
                    const QColor &fontColor = QColor(Qt::black),
                    const QColor &backColor = QColor(Qt::white),
                    const QColor &lineColor = QColor(Qt::white),
                    bool bTitle = false,
                    bool bTitleButton = false);

    void SetFile(MLS::File *pFile, bool bFocus, int nNumber, bool bFileOwner, int nDefFontSize);

    virtual void SetMouseOver(bool bFocus = true) { _bMouseFocus = bFocus; }

    virtual bool GetMouseOver() const { return _bMouseFocus; }

    virtual void SetFocus(bool bFocus = true) { _bFocus = bFocus; }

    virtual bool GetFocus() const { return _bFocus; }

    const QRect &GetQRect() const { return _viewRect; }

    MLS::File *GetFile() { return _pFile; }

    void SetFontSize(int nFontSize) { _nDefFontSize = nFontSize; }

public
    slots:
    void

    slotTitleShow();

protected:
    void FileView(const QRect &box);

    void ExtView(QRect &box);

    void SizeView(QRect &box);

    void IconView(QRect &box);

    void AttrView(QRect &box);

    void DateView(QRect &box);

    void TimeView(QRect &box);

    void OwnerView(QRect &box);

    void GroupView(QRect &box);

    void checkAddedTitle(TitleButtonItem *item);

    void DrawTabBox(const string &str, QRect &viewRect);

    void SortViewTab(const vector<TabViewInfo> &vViewList, const QRect &viewRect);

    void Draw();

protected
    slots:
    void

    FilenameClicked();

    void SizeClicked();

    void IconClicked();

    void AttrClicked();

    void DateClicked();

    void TimeClicked();

    void OwnerClicked();

    void GroupClicked();

private:
    MLS::File *_pFile;
    bool _bFocus;
    bool _bMouseFocus;
    int _nDrawType;
    int _nNumber;
    bool _bShowFileOwner;
    bool _bTitle;
    int _nDefFontSize;
    bool _bExtBind;
    QRect _viewRect;
    QVector<TitleButtonItem *> _vTitleButtonList;
    bool _bTitleButton;

    QColor _backColor;
    QColor _fontColor;
    QColor _focusColor;
    QColor _lineColor;
};

#endif // __QT_PANELFILEBOX_H__
