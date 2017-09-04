#ifndef __QT_MCD_H__
#define __QT_MCD_H__

#include <Q3ListViewItem>

#include "file.h"
#include "reader.h"

using namespace MLS;

class 	Qt_Mcd;
class	Qt_Panel;

class	Qt_McdItem : public Q3ListViewItem
{
public:
	Qt_McdItem( Qt_McdItem* pParent, Qt_Mcd* pMcd, MLS::File& tFile );
	Qt_McdItem( Qt_Mcd* pMcd, MLS::File& tFile );

	MLS::File*	GetFile();

	QString 	text( int column ) const;
	bool		OpenChk( const string& sPath, bool bChkSubDir, bool bSubDirAll );
	void 		setOpen( bool bOpen );

private:
	String		strTmp;
	Qt_Mcd*		_pMcd;
	MLS::File	_tFile;
};

class Qt_Mcd : public Q3ListView
{
Q_OBJECT

public:
	Qt_Mcd(Qt_Panel* pPanel, QWidget* parent, const char* name = "Qt_Mcd");
	~Qt_Mcd();

	bool			GetHidden();
	MLS::Reader*	GetReader();

	bool	InitMcd( MLS::Reader* pReader, const string& sRootPath );
	void 	setDir( const string &sFullName );
	void	setDir( const MLS::File& tFile );

	void	resizeEvent ( QResizeEvent * );

protected slots:
	void	SetPanelDirChg ( Q3ListViewItem * item );

private:
	MLS::Reader*	_pReader;
	bool			_bHidden;
	Qt_Panel*		_pPanel;
	
};

#endif // __QT_MCD_H__
