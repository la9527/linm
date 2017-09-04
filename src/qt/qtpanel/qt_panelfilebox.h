#ifndef __QT_PANELFILEBOX_H__
#define	__QT_PANELFILEBOX_H__

#include "define.h"
#include "file.h"

#include <QObject>
#include <QPushButton>
#include <QImage>

enum POSITION
{
	LEFT,
	RIGHT,
	MIDDLE,
	TOP,
	BOTTOM,
	CURRENT
};

class Position: public QObject
{
	public:
		Position() : QObject(), _ObjPosY(CURRENT), _ObjPosX(CURRENT), _pParent(0), _pImage(0) {}
		virtual ~Position() {}
		
		void	SetWidget( QWidget* pParent ) { _pParent = pParent; }
		inline void Move(POSITION posX, POSITION posY) { _ObjPosY = posY; _ObjPosX = posX; }
		inline void Move(POSITION posX, int Y) 	{ _ObjPosX = posX; y = Y; }
		inline void Move(int X, POSITION posY) 	{ _ObjPosY = posY; x = X; }
		inline void Move(int X, int Y)			{ x = X;	y = Y; }
		inline bool AreaChk(int X, int Y)
		{
			if (y <= Y && y+height > Y)
				if (x <= X && x+width > X)
					return true;
			return false;
		}

		QImage* GetImage() { return _pImage; }
		void 	Show();

		int x, y, height, width;
		int fontcolor;
		int backcolor;

	protected:
		virtual void 	DrawObject() = 0;
		void			Resize();
		
		bool			_bShow;
		POSITION		_ObjPosY, _ObjPosX;
		QWidget*		_pParent;
		QImage*			_pImage;
};

class	TabViewInfo
{
public:
	string		sName;
	int			nWidth;

public:
	TabViewInfo( const string& str = "", int w = 0): sName( str ), nWidth( w ) {}	
};

class 	TitleButtonItem
{
public:
	QWidget*		_pParent;
	QString			_strTitleName;
	QRect			_rect;
	QPushButton*	_pButton;
	
	TitleButtonItem( QWidget* pParent, const QString& strTitleName, const QRect& rect )
	 : _pParent( pParent ), _strTitleName( strTitleName ), _rect( rect ), _pButton( 0 ) {}
};

class	QRect;
class	QColor;
class	QPushButton;

class	Qt_FileBox: public Position
{
Q_OBJECT

public:
	Qt_FileBox();
	~Qt_FileBox();

	void	setFileBox( const QColor& focusColor = QColor( Qt::blue ),
						const QColor& fontColor = QColor( Qt::black ),
						const QColor& backColor = QColor( Qt::white ),
						const QColor& lineColor = QColor( Qt::white ),
						bool  bTitle = false,
						bool  bTitleButton = false );

	void	SetFile(MLS::File* pFile, bool bFocus, int nNumber, bool bFileOwner, int nDefFontSize);

	int		GetNumber()	{ return _nNumber; }
	void	SetNumber( int nNum ) { _nNumber = nNum; }

	virtual void	SetMouseOver( bool bFocus = true) {	_bMouseFocus = bFocus; 	}
	virtual bool	GetMouseOver() const			  { return _bMouseFocus; }

	virtual void	SetFocus( bool bFocus = true)	  { _bFocus = bFocus; }
	virtual bool	GetFocus() const				  { return _bFocus; }

	const QRect&	GetQRect() const	{ return _viewRect; }

	MLS::File* 	GetFile()	{ return _pFile; }
	void	SetFontSize( int nFontSize ) { _nDefFontSize = nFontSize; }

	QImage		GetFocusImage();

public slots:
	void	slotTitleShow();

protected:
	void	FileView( QPainter& painter, const QRect& box );
	void	ExtView( QPainter& painter, QRect& box );
	void	SizeView( QPainter& painter, QRect& box );
	void	IconView( QPainter& painter, QRect& box );
	void	AttrView( QPainter& painter, QRect& box );
	void	DateView( QPainter& painter, QRect& box );
	void	TimeView( QPainter& painter, QRect& box );
	void	OwnerView( QPainter& painter, QRect& box );
	void	GroupView( QPainter& painter, QRect& box );

	void	checkAddedTitle( TitleButtonItem* item );

	void	DrawTabBox( QPainter& painter, const string& str, QRect& viewRect );
	void	SortViewTab( const vector<TabViewInfo>& vViewList, const QRect& viewRect );

	void	DrawObject();
	
protected slots:
	void	FilenameClicked();
	void	SizeClicked();
	void	IconClicked();
	void	AttrClicked();
	void	DateClicked();
	void	TimeClicked();
	void	OwnerClicked();
	void	GroupClicked();

private:
	MLS::File*					_pFile;
	bool						_bFocus;
	bool						_bMouseFocus;
	int							_nDrawType;
	int							_nNumber;
	bool						_bShowFileOwner;
	bool						_bTitle;
	int							_nDefFontSize;
	bool						_bExtBind;
	QRect						_viewRect;
	QVector<TitleButtonItem*>	_vTitleButtonList;
	bool						_bTitleButton;

	QColor						_focusColor;
	QColor						_fontColor;
	QColor						_backColor;
	QColor						_lineColor;
};

#endif // __QT_PANELFILEBOX_H__
