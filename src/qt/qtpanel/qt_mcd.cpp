#include <QResizeEvent>
#include <QDebug>

#include "qpixmap.h"
#include "exception.h"
#include "qt_dialog.h"
#include "qt_panel.h"
#include "qt_mcd.h"

Qt_McdItem::Qt_McdItem( Qt_McdItem* pParent, Qt_Mcd* pMcd, MLS::File& tFile ):
	Q3ListViewItem( pParent ), _pMcd( pMcd ), _tFile( tFile )
{
	if ( tFile.bDir && !tFile.isExecute() )
		setPixmap( 0, LinMGlobal::GetSmallIcon( "folder-locked" ) );
	else
		setPixmap( 0, LinMGlobal::GetSmallIcon( "folder" ) );

	setup();
	widthChanged( 0 );
	invalidateHeight();
}

Qt_McdItem::Qt_McdItem( Qt_Mcd* pMcd, MLS::File& tFile ): 
	Q3ListViewItem( (Q3ListView*)pMcd ), _pMcd( pMcd ), _tFile( tFile )
{
}

MLS::File*	Qt_McdItem::GetFile()
{
	return &_tFile;
}

QString 	Qt_McdItem::text( int column ) const
{
	if ( column == 0 )
		return QObject::tr( _tFile.sName.c_str() );
}

bool		Qt_McdItem::OpenChk( const string& sPath, bool bChkSubDir, bool bSubDirAll )
{
	Reader* pReader = _pMcd->GetReader();

	if (pReader == NULL)
		throw Exception( "Qt_McdItem pReader is NULL." );

	string 	sBefPath = pReader->GetPath();
	
	if ( !_tFile.bDir ) return -1;

	if ( pReader->Read( sPath ) == false)
		return -1;
	
	vector<Qt_McdItem*>	vDirList;
	bool	bChkDir = false;

	while( pReader->Next() )
	{
		File	tFile;
		if (!pReader->GetInfo(tFile)) continue;

		if (tFile.sName == "." || tFile.sName == "..")
			continue;

		if ( tFile.bDir && !tFile.bLink )
		{
			if (_pMcd->GetHidden() == false)
				if (tFile.sName.substr(0, 1) == ".")
					continue;

			if ( !bChkSubDir )
			{
				Qt_McdItem* pItem = new Qt_McdItem( this, _pMcd, tFile );
				vDirList.push_back( pItem );
			}
			
			bChkDir = true;
		}
	}

	while( !vDirList.empty() )
	{
		Qt_McdItem*	pDirNode = vDirList.back();
		vDirList.pop_back();
	
		if ( bSubDirAll )
			pDirNode->setOpen( TRUE ); // be interesting
		else
		{
			if ( pDirNode->OpenChk( pDirNode->GetFile()->sFullName, true, false ) )
				pDirNode->setExpandable( TRUE );
			else
				pDirNode->setExpandable( FALSE );
		}
	}

	// 함수를 실행하기 전 디렉토리로 이동.
	pReader->Read( sBefPath );
	return bChkDir;
}

void Qt_McdItem::setOpen( bool bOpen )
{
	if ( bOpen )
		setPixmap( 0, LinMGlobal::GetSmallIcon( "folder-open" ) );
	else
		setPixmap( 0, LinMGlobal::GetSmallIcon( "folder" ) );

	if ( bOpen && !childCount() )
	{
		listView()->setUpdatesEnabled( FALSE );

		OpenChk( _tFile.sFullName, false, false );

		listView()->setUpdatesEnabled( TRUE );
	}
	Q3ListViewItem::setOpen( bOpen );
}

Qt_Mcd::Qt_Mcd(Qt_Panel* pPanel, QWidget* parent, const char* name):
	Q3ListView( parent, name ), _pPanel( pPanel )
{
	_bHidden = false;
	_pReader = 0;

	setFocusPolicy( Qt::NoFocus );

	connect( this, SIGNAL( doubleClicked( Q3ListViewItem * ) ),
			 this, SLOT( SetPanelDirChg( Q3ListViewItem * ) ) );
}

Qt_Mcd::~Qt_Mcd()
{
}

bool	Qt_Mcd::GetHidden()
{
	return _bHidden;
}

MLS::Reader*	Qt_Mcd::GetReader()
{
	return _pReader;
}

bool	Qt_Mcd::InitMcd( MLS::Reader* pReader, const string& sRootPath )
{
	qDebug() << "Qt_Mcd::InitMcd";
	if ( !pReader ) 
		throw Exception( "InitMcd Reader is NULL !!!" );

	_pReader = pReader;

	string 	sBefPath = pReader->GetPath();
		
	if ( pReader->Read( sRootPath.c_str() ) == false)
		return false;

	File	tFile;
	tFile.sFullName = "/";
	tFile.sName = "/";
	tFile.bDir = true;

	clear();
	
	Qt_McdItem* pRoot = new Qt_McdItem( this, tFile );

	//pRoot->OpenChk( tFile.sFullName, true, false );
	pRoot->setOpen( TRUE ); // be interesting

	pReader->Read( sBefPath );

	//resize( 400, 400 );
	setAllColumnsShowFocus( TRUE );
	qDebug() << "Qt_Mcd::InitMcd End";
	return true;
}

void 	Qt_Mcd::setDir( const string &sFullName )
{
	Q3ListViewItemIterator it( this );

	++it;

	for( ; it.current(); ++it )
		it.current()->setOpen( FALSE );
	
	QStringList lst( QStringList::split( "/", tr( sFullName.c_str() ) ) );
	QStringList::Iterator it2 = lst.begin();
	
	Qt_McdItem *item = (Qt_McdItem*)firstChild();
	
	for ( ; it2 != lst.end(); ++it2 )
	{
		while ( item )
		{
			//if ( item->GetFile()->sFullName == sFullName )
			if ( item->text( 0 ) == *it2 )
			{
				item->setOpen( TRUE );
				break;
			}

			item = (Qt_McdItem*)item->itemBelow();
		}
	}

	if ( item )
		setCurrentItem( item );
}

void	Qt_Mcd::setDir( const MLS::File& file )
{
	setDir( file.sFullName );
}

void	Qt_Mcd::SetPanelDirChg ( Q3ListViewItem * item )
{
	MLS::File*	pFile = ((Qt_McdItem*)item)->GetFile();

	_pPanel->Read( pFile->sFullName );

	if ( _pPanel->isVisible() )
		_pPanel->Refresh();
	
	item->setPixmap( 0, LinMGlobal::GetSmallIcon( "folder-open" ) );
}

void	Qt_Mcd::resizeEvent( QResizeEvent* e )
{
	setColumnWidth( 0, e->size().width() - 20 );
	Q3ListView::resizeEvent( e );
}
