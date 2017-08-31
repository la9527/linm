#ifndef __PANELSTATAUSBAR_H__
#define __PANELSTATAUSBAR_H__

#include <QWidget>
#include "qt_dialog.h"

class QLabel;

class Qt_Panel;

class QHBoxLayout;

class PanelStatusBar : public QWidget {
public:
    PanelStatusBar(QWidget *parent = 0);

    ~PanelStatusBar();

    void setPanel(Qt_Panel *pPanel);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QHBoxLayout *_pHbox;
    Qt_Panel *_pPanel;
    QLabel *_pLabel;
};

#endif
