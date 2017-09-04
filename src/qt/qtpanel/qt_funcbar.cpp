#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "qt_funcbar.h"

#include "strutil.h"
#include "mlslog.h"
#include "exception.h"
#include "mlscfgload.h"
#include "keycfgload.h"
#include "qt_command.h"

FuncBar::FuncBar(QWidget * parent, PanelCmd * pCmdList) 
	: QWidget( parent ), _pCmdList( pCmdList )
{
}

FuncBar::~FuncBar()
{
	for ( int n = 0; n <  _vButtons.size(); n++ )
		if ( _vButtons[n] ) delete _vButtons[n];

	if ( _pHBox )
		delete _pHBox;
}

void FuncBar::init( ViewType tType )
{
	QBoxLayout*	_pHBox = new QHBoxLayout( this );
	_pHBox->setMargin( 0 );
	_pHBox->setSpacing( 1 );

	QColor color(120, 120, 255);
	QPalette palette;
	palette.setColor( QPalette::Background, color );

	for ( int n = 1; n <= 12; n++ )
	{
		MLSUTIL::String		str;
		str.Printf( "F%d", n );

		string strFuncName =  MLS::g_tKeyCfg.GetCommand( str, ::PANEL );
		string strExecName =  MLS::g_tKeyCfg.GetHelp_Key( str, ::PANEL );

		//strExecName = "<center>" + str.Get() + " <span style=\"color:#ffffff;\">" + strExecName + "</span></center>";
		strExecName = str.Get() + " " + strExecName;

		str.Printf("%dCmd_%s()", QSLOT_CODE, (const char*)strFuncName.c_str() );

		QPushButton*	pButton = new QPushButton( this );
		pButton->setFocusPolicy( Qt::NoFocus );
		connect( pButton, SIGNAL( clicked() ), _pCmdList, str.c_str() );
		pButton->setText( tr( (const char*)strExecName.c_str() ) );
		//pButton->setTextFormat( Qt::AutoText );

		pButton->setPalette( palette );
		pButton->setAutoFillBackground( true );

		_pHBox->addWidget( pButton );
		_vButtons.push_back( pButton );
	}
}

