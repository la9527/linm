#include <QBrush>
#include <QPen>
#include <QRect>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>

#ifdef 	LINM_KDE
//	#include <kpixmapeffect.h>
#endif

#include "define.h"
#include "file.h"
#include "strutil.h"

#include "qt_paneltooltip.h"

PanelToolTip::PanelToolTip( QWidget* parent )
	: QWidget( parent, 0, Qt::WX11BypassWM
				/*WType_TopLevel | WStyle_Customize | WX11BypassWM | WStyle_StaysOnTop*/ )
{
	_pFile = NULL;

	setAutoFillBackground( true );
	setAttribute( Qt::WA_OpaquePaintEvent );

	_lineColor.setRgb( 0, 0, 255 );
 	_backColor.setRgb( 250, 100, 255, 200 );
	_font1Color.setRgb( 255, 255, 255 );
	_font2Color.setRgb( 0, 0, 0 );

	_nDuration = 2000;
	_bTranslucency = false;

	_pShowTimer = new QTimer( this );
	_pHideTimer = new QTimer( this );

	connect( _pShowTimer, SIGNAL(timeout()), this, SLOT(ShowToolTip()) );
	connect( _pHideTimer, SIGNAL(timeout()), this, SLOT(HideToolTip()) );

	if( _bTranslucency )
		_pixScreenshot = QPixmap::grabWindow( QApplication::desktop()->winId() );
}

PanelToolTip::~PanelToolTip()
{
}

void	PanelToolTip::setFile( MLS::File* pFile ) 
{
	if ( pFile && _pFile != pFile )
	{
		qDebug("PanelToolTip::setFile");
		_pFile = pFile;

		if ( &_pFile->sName == 0x00 ) return;

		QString		sInfo;
		QString		strName = _pFile->sName.c_str();
		QFontMetrics 	fm( font() );
		
		int 		nNameWidth = fm.width( QObject::tr(strName) );
		
		if (_pFile->uSize >= 1000000000)
		{
			sInfo.sprintf( "%s Byte (%.2fG) / %s %s / %s", 	MLSUTIL::toregular( _pFile->uSize ).c_str(), 
										(float)_pFile->uSize/1073741824,
										_pFile->sDate.c_str(),
										_pFile->sTime.c_str(),
										_pFile->sAttr.c_str() );
		}
		else if (_pFile->uSize >= 10000000)
		{
			sInfo.sprintf( "%s Byte (%.2fM) / %s %s / %s", MLSUTIL::toregular( _pFile->uSize ).c_str(), 
													(float)_pFile->uSize/1048576, 
													_pFile->sDate.c_str(),
													_pFile->sTime.c_str(),
													_pFile->sAttr.c_str() );
		}
		else
		{
			sInfo.sprintf( "%s / %s %s / %s", MLSUTIL::toregular( _pFile->uSize ).c_str(),
													_pFile->sDate.c_str(),
													_pFile->sTime.c_str(),
													_pFile->sAttr.c_str() );
		}

		
		int nInfoWidth = fm.width( sInfo );
		int	nWidth = 0;
		
		if ( nInfoWidth > nNameWidth )
			nWidth = nInfoWidth + 10;
		else
			nWidth = nNameWidth + 10;
		
		_sInfo1 = strName;
		_sInfo2 = sInfo;

		_rectToolTip.setWidth( nWidth );
		_rectToolTip.setHeight( (fm.height() * 2) + 4 );

		resize( _rectToolTip.width(), _rectToolTip.height() );

		qDebug( "setFile !!!!!!! resize signal _pShowTimer !!!");
		_pShowTimer->start( 1500, true );
		//_bHide = false;
	}
}

void	PanelToolTip::ShowToolTip()
{
	qDebug("ShowToolTip !!!!!!!!!!");
	if ( !isVisible() )
		show();
	else
		update();

	_pHideTimer->stop();
}

void	PanelToolTip::HideToolTip()
{
	//if ( _bHide )
	hide();
	_pShowTimer->stop();

	if( _bTranslucency )
	{
		_pixScreenshot = QPixmap::grabWindow( QApplication::desktop()->winId() );
		qDebug("_bTranslucency  ~~~~~~~ HideToolTip !!!!!!!");
	}
}

void	PanelToolTip::showEvent( QShowEvent* /*event*/ )
{
	if( _nDuration ) //duration 0 -> stay forever
	{
		_pHideTimer->start( _nDuration, true );	//calls hide()
	}
}

void 	PanelToolTip::moveEvent( QMoveEvent * /*event*/ )
{
	if( _nDuration ) //duration 0 -> stay forever
	{
		if ( _pHideTimer->isActive() )
			_pHideTimer->stop();
	}
}

void	PanelToolTip::paintEvent( QPaintEvent* event )
{
	if ( !_pFile ) return;
	qDebug( "PanelToolTip :: paintEvent Event !!! - event ");

	QFontMetrics 	fm( font() );
	QPoint 			point;
    QRect 			rect( point, size() );

	QPainter 		painter( this );
	painter.setRenderHint(QPainter::Antialiasing);

	if( _bTranslucency )
	{
		QPixmap		backPixmap;
		backPixmap.resize( size() );
		bitBlt( &backPixmap, 0, 0, &_pixScreenshot, x(), y(), width(), height() );

/*
		#ifdef 	LINM_KDE
		QPixmap background( backPixmap );
		KPixmapEffect::fade( background, 0.8, backgroundColor() );
		painter.drawPixmap( 0, 0, background );
		#else
*/
		painter.drawPixmap( 0, 0, backPixmap );
//		#endif
	}
	
	painter.setPen( _lineColor );
	//painter.setBrush( QBrush( _backColor, Qt::SolidPattern )  );

	QRect	rectLine = QRect( rect.x()+1, rect.y()+1, rect.width() - 2, rect.height() - 2 );
	painter.drawRoundRect( rectLine, 3 );

	painter.setPen( QPen( _font1Color ) ); // Color
	painter.drawText(5, fm.height(), QObject::tr( _sInfo1 ) );

	painter.setPen( QPen( _font2Color ) ); // Color
	painter.drawText(5, fm.height()*2, QObject::tr( _sInfo2 ) );
	painter.end();

	qDebug("PanelToolTip :: Painter [%d] [%d] [%d] [%d]", x(), y(), width(), height() );
}

void	PanelToolTip::keyPressEvent( QKeyEvent* event )
{
	qDebug("PanelToolTip keyPressEvent [%d]", event->key() );
}
