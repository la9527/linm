#include <qframe.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QDebug>
#include <QPainter>

#include "strutil.h"
#include "file.h"
#include "qt_dialog.h"
#include "qt_panel.h"
#include "qt_paneltooltip.h"
#include "qt_panelfilebox.h"
#include "qt_statusbar.h"

PanelStatusBar::PanelStatusBar(QWidget *parent) : QWidget(parent) {
    //setFrameStyle( QFrame::Box | QFrame::Plain );
    //setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

    //setLineWidth( 0 );
    //setMidLineWidth( 0 );

    //setMargin( 0 );

    /*--------------------------------------------------*/
    /* Background black */
    /*
    QColor color(200, 200, 200);
    QPalette palette;
    palette.setColor( QPalette::Background, color );

    setPalette( palette );
    setAutoFillBackground( true );
    */
    /*--------------------------------------------------*/
    _pPanel = 0;
}

PanelStatusBar::~PanelStatusBar() {
}

void PanelStatusBar::setPanel(Qt_Panel *pPanel) {
    _pPanel = pPanel;
    /*
    if ( !_pHbox )
    {
        QHBoxLayout* _pHbox = new QHBoxLayout( this );
        _pHbox->setMargin( 0 );
        _pHbox->setSpacing( 0 );
        _pHbox->add( _pLabel );
    }

    if ( pPanel )
    {
        QString	strText;
        //strText = "<b><span style=\"color:#6546AB;\">" + QString::number( _pPanel->_uFile ) + "</span></b>";
        strText = strText + " File";

        //strText = "<b><span style=\"color:#6546AB;\">" + QString::number( _pPanel->_uDir ) + "</span></b>";
        strText = strText + " Directory";

        //strText = "<b><span style=\"color:#6546AB;\">" + QString::number( _pPanel->_uDir ) + "</span></b>";
        strText = strText + " Byte";

        _pLabel->setText( strText );
    }
    */
}

void PanelStatusBar::paintEvent(QPaintEvent *e) {
    if (_pPanel) {
        qDebug() << "PanelStatusBar::paintEvent";
        QFont fontDefault(font());
        QFont fontBold(font());
        fontBold.setBold(true);

        QFontMetrics fm(fontDefault);
        QFontMetrics fmBold(fontBold);

        QString strFileNum;
        QString strText;
        int nFontWidth = 10;

        QPainter painter(this);

        ///////////////////////////////////////////////////////
        // File number
        strFileNum = QString::number(_pPanel->_uFile);

        painter.setFont(fontBold);
        painter.setPen(QPen(QColor(101, 70, 171)));
        painter.drawText(QPoint(nFontWidth, fmBold.height() - 3), strFileNum);

        nFontWidth = nFontWidth + fmBold.width(strFileNum) + 5;
        strText = tr("File");

        painter.setFont(fontDefault);
        painter.setPen(QPen(QColor(0, 0, 0)));
        painter.drawText(QPoint(nFontWidth, fmBold.height() - 3), strText);

        ///////////////////////////////////////////////////////
        // Directory number
        nFontWidth = nFontWidth + fmBold.width(strText) + 20;
        strFileNum = QString::number(_pPanel->_uDir);

        painter.setFont(fontBold);
        painter.setPen(QPen(QColor(101, 70, 171)));
        painter.drawText(QPoint(nFontWidth, fmBold.height() - 3), strFileNum);

        nFontWidth = nFontWidth + fmBold.width(strFileNum) + 5;
        strText = tr("Directory");

        painter.setFont(fontDefault);
        painter.setPen(QPen(QColor(0, 0, 0)));
        painter.drawText(QPoint(nFontWidth, fmBold.height() - 3), strText);

        ///////////////////////////////////////////////////////
        // Files Byte
        nFontWidth = nFontWidth + fmBold.width(strText) + 20;
        strFileNum = MLSUTIL::toregular(_pPanel->_uDirSize).c_str();

        painter.setFont(fontBold);
        painter.setPen(QPen(QColor(101, 70, 171)));
        painter.drawText(QPoint(nFontWidth, fmBold.height() - 3), strFileNum);

        nFontWidth = nFontWidth + fmBold.width(strFileNum) + 5;
        strText = tr("Byte");

        painter.setFont(fontDefault);
        painter.setPen(QPen(QColor(0, 0, 0)));
        painter.drawText(QPoint(nFontWidth, fmBold.height() - 3), strText);
    }
}
