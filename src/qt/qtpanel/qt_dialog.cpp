#include <QObject>
#include <QMessageBox>
#include <QTimer>
#include <QProgressDialog>
#include <QInputDialog>
#include <QIconSet>
#include <QPixmapCache>
#include <QFileIconProvider>
#include <QApplication>
#include <QDebug>

#ifdef 	LINM_KDE
	#include <KDE/KGlobal>
	#include <KDE/KIcon>
	#include <KDE/KIconTheme>	// kdelibs4-dev
	#include <KDE/KIconLoader> 	// kdelibs4-dev
	#include <KDE/KMimeType>	// kdelibs4-dev
	#include <KDE/KUrl>
#endif

#include "mlsdialog.h"
#include "qt_dialog.h"

void	Qt_Dialog::ProgressBreak()
{
	_bProgBreak = true;
}

void	Qt_Dialog::ProgressTimerPerform()
{
	if ( _pProgDig )
	{
		_pProgDig->setValue( _nSteps );
		_nSteps++;
		if ( _nSteps > _pProgDig->maximum() )
			_nSteps = 0;
	}
}

void	Qt_Dialog::MsgBox(const string& sTitle, const string& sMsg)
{
	QWidget* pWidget = QApplication::activeWindow();
	QMessageBox::critical( pWidget, QObject::tr(sTitle.c_str()), QObject::tr(sMsg.c_str()) );
}

bool	Qt_Dialog::YNBox(const string& sTitle, const string& sMsg, bool bYes)
{
	QWidget* pWidget = QApplication::activeWindow();
	if (QMessageBox::information( pWidget, 
								QObject::tr(sTitle.c_str()), 
								QObject::tr(sMsg.c_str()), 
								"&Yes", "&No", 
								QString::null, 0, 1 ) )
	{
		return false;
	}
	return true;
}

void*	Qt_Dialog::MsgWaitBox(const string& sTitle, const string& sMsg)
{
	MsgWaitEnd( 0 );

	_pProgDig = new QProgressDialog( QObject::tr(sTitle.c_str()), QObject::tr("Cancel"), 0, 100 );
	_pWidget->connect( _pProgDig, SIGNAL(canceled()), this, SLOT(ProgressBreak()) );
	_nSteps = 0;
	//ProgressTimerPerform();
			
	_pTimer = new QTimer( _pWidget );
	//_pTimer->changeInterval(100);
	_pWidget->connect( _pTimer, SIGNAL(timeout()), this, SLOT(ProgressTimerPerform()) );
	_pTimer->start( 100 );
	
	_bProgBreak = false;
	return false;
}

void	Qt_Dialog::MsgWaitEnd(void* p) 
{
	if ( _pProgDig )
	{
		if ( _pTimer ) 
			delete _pTimer; 
		_pTimer = 0;

		_pProgDig->setValue( _nSteps );

		_pProgDig->close();
		delete _pProgDig;
		_pProgDig = 0;
	}
}

int		Qt_Dialog::GetChar(bool bNoDelay)
{
	if ( _pProgDig && _bProgBreak ) return 27; // ESC 로 리턴.
	return 0;
}

int		Qt_Dialog::InputBox(const string& sTitle, string& sInputStr, bool bPasswd)
{
	QWidget* pWidget = QApplication::activeWindow();

	bool ok = false;
	QString		strRt;

	if ( !bPasswd )
		strRt = QInputDialog::getText( pWidget, QObject::tr(sTitle.c_str()), QObject::tr(sTitle.c_str()), 
										QLineEdit::Normal, sInputStr.c_str(), &ok );
	else
		strRt = QInputDialog::getText( pWidget, QObject::tr(sTitle.c_str()), QObject::tr(sTitle.c_str()), 	
										QLineEdit::Password, sInputStr.c_str(), &ok );

	sInputStr = (const char*)strRt.toLocal8Bit();
	if ( ok && !sInputStr.empty() ) return SUCCESS;
	return ERROR;
}

int		Qt_Dialog::SelectBox(const string& sTitle, vector<string>& vMsgStr, int n)
{
	// 구현해야 함.
	return 0;
}

int		Qt_Dialog::TextBox( const string& sTitle, vector<string>& vTextStr, bool bCurShow, int width )
{
	// 구현해야 함.
	return 0; 
}

QPixmap LinMGlobal::GetSmallIcon( MLS::File* pFile )
{
	QString		strIconName;

	if ( !pFile ) 
	{
	#ifdef 	LINM_KDE
		//KIconLoader* pIconLoader = KIconLoader::global();
		//qDebug() << pIconLoader->theme()->name();
		//qDebug() << KIconTheme::defaultThemeName();
		return SmallIcon( "unknown" );
	#else
		QPixmap pm;	
		if ( !QPixmapCache::find("unknown", pm) ) 
		{
			pm.load( strIconName + ".png" );		
			QPixmapCache::insert("unknown", pm);
		}
		return pm;
	#endif
	}

	if ( pFile->bDir ) 
		strIconName = "folder";
	else
	{
#ifdef 	LINM_KDE
		strIconName = KMimeType::iconNameForUrl( KUrl( pFile->sName.c_str() ) );
#else
		strIconName = pFile->Ext().c_str();
		if ( strIconName.isEmpty() )
			strIconName = pFile->sName.c_str();
#endif
	}
	
#ifdef 	LINM_KDE
	return SmallIcon( strIconName );
#else
	QPixmap pm;	
	if ( !QPixmapCache::find(strIconName, pm) ) 
	{
		pm.load( strIconName + ".png" );
		QPixmapCache::insert(strIconName, pm);
	}
	return pm;
#endif
}

QPixmap LinMGlobal::GetSmallIcon( const QString& strIconName )
{
#ifdef 	LINM_KDE
	//KIconLoader* pIconLoader = KIconLoader::global();
	//qDebug() << pIconLoader->theme()->dir();
	//qDebug() << KIconTheme::defaultThemeName();

	if ( strIconName.isEmpty() )
		return SmallIcon( "unknown" );
	else
		return SmallIcon( strIconName );
#else
	QPixmap 	pm;
	QString		strTmp = strIconName;
	
	if ( strTmp.isEmpty() )
		strTmp = "unknown";

	if ( !QPixmapCache::find(strTmp, pm) )
	{
		pm.load( strIconName + ".png" );
		QPixmapCache::insert(strTmp, pm);
	}
	return pm;	
#endif
}

QIcon	LinMGlobal::GetIconSet( const QString& strIconName )
{
#ifdef 	LINM_KDE
	return KIcon( strIconName );
	/*
	KIconLoader* pIconLoader = KIconLoader::global();
	qDebug() << pIconLoader->queryIconsByContext( 16 );

	return pIconLoader->loadIconSet( strIconName, KIconLoader::Small );
	*/
	//return BarIconSet( strIconName );
#else
	//return QIconSet( QPixmap::fromMimeSource( strIconName ) );
	return QIcon( strIconName );
#endif
}

QColor	LinMGlobal::GetColor( int nNum )
{
	// 0 검정   1 빨강   2 녹색   3 갈색   4 파랑   5 보라   6 청록   7 흰색
	// 8 회색   9 주황  10 연두  11 노랑  12 하늘  13 분홍  14 옥색  15 밝은 흰색
	switch( nNum )
	{
		case 0:	return Qt::black;
		case 1: return QColor(100,0,0); //Qt::darkRed;
		case 2: return QColor(30, 80, 0);
		case 3: return QColor( 152, 0, 0);	// 갈색
		case 4: return Qt::darkBlue;
		case 5: return QColor( 110, 2, 120);
		case 6: return QColor( 200,0,0); 	// 청록
		case 7: return QColor( 30, 30, 30);
		case 8: return Qt::gray;
		case 9: return QColor( 150, 0, 0 );
		case 10: return QColor( 0, 139, 0 );
		case 11: return Qt::yellow;
		case 12: return QColor( 0, 55, 165 );
		case 13: return QColor(119, 7, 129);
		case 14: return Qt::blue;
		case 15: return Qt::white;
	}
	return QColor( 0, 0, 0 );
}
