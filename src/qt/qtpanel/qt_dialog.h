/******************************************************************************
 *   Copyright (C) 2005 by la9527                                             *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.*
 ******************************************************************************/

#ifndef __QT_DIALOG_H__
#define __QT_DIALOG_H__

#include <QObject>
#include <QPixmap>

#include "define.h"
#include "mlsdialog.h"
#include "file.h"

class QObject;
class QWidget;
class QMessageBox;
class QTimer;
class QProgressDialog;
class QInputDialog;
class QIcon;

class Qt_DialogMoc:public QObject
{
Q_OBJECT

public:
	Qt_DialogMoc(QObject * parent = 0 ):
		QObject ( parent ) { }
	
protected slots:
	virtual void	ProgressBreak() = 0;
	virtual void	ProgressTimerPerform() = 0;
};

class Qt_Dialog:public Qt_DialogMoc, public MLSUTIL::MlsDialog
{
	friend class Qt_DialogMoc; // QT 의 Q_OBJECT 떄메 ㅠㅠ

protected:
	void	ProgressBreak();
	void	ProgressTimerPerform();

public:
	Qt_Dialog( QWidget* pWidget  = 0):
		Qt_DialogMoc(), MlsDialog(), _pWidget( pWidget ), _pProgDig( 0 )
	{
		if ( !pWidget )
			throw MLSUTIL::Exception( "Qt_Dialog : pWidget = NULL !!!" );
	}

	~Qt_Dialog() {}

	void	MsgBox(const string& sTitle, const string& sMsg);
	bool	YNBox(const string& sTitle, const string& sMsg, bool bYes = false);
	void*	MsgWaitBox(const string& sTitle, const string& sMsg);
	void	MsgWaitEnd(void* p);
	int		GetChar(bool bNoDelay = false);
	int		InputBox(const string& sTitle, string& sInputStr, bool bPasswd = false);
	int		SelectBox(	const string& sTitle, vector<string>& vMsgStr, int n = 0);
	
	int		TextBox( const string& sTitle, vector<string>& vTextStr, bool bCurShow = false, int width = 40 );

private:
	QWidget*			_pWidget;
	QProgressDialog*	_pProgDig;
	QTimer*				_pTimer;
	bool				_bProgBreak;
	int					_nSteps;
};

class Qt_ProgressMoc:public QObject
{
Q_OBJECT

public:
	Qt_ProgressMoc(QObject * parent = 0 )
		: QObject ( parent ) { }
};

///	@brief	 상태출력 박스
class Qt_Progress:public Qt_ProgressMoc, public MLSUTIL::MlsProgress
{
	friend class Qt_ProgressMoc; // QT 의 Q_OBJECT 떄메 ㅠㅠ

protected:
	void*		_pVoidTmp;		///< void Tmp 형태. 형변환 해서 사용 바람.
	string 		_sLeftStr, _sRightStr;
	string 		_sLeftStr2, _sRightStr2;

	int 		_nCnt;  		///< 카운터 기준 (% 단위)
	int 		_nCnt2;  		///< 카운터 기준 (% 단위)

public:
	Qt_Progress(): Qt_ProgressMoc(0), MLSUTIL::MlsProgress() {}
	~Qt_Progress() {}
	
	void 	SetTitle(const string& str)		{ _sTitle = str; }
	void	SetMsg(const string& str)		{ _sMsg = str; }
	void	SetDouble(bool bDouble) 		{ _bDouble = bDouble; }

	void setLeftStr(const string& p)
	{ 
		//MLSUTIL::MlsMutexSelf	tMutex(&_pMutex);
		_sLeftStr=p; 	
	}

	void setRightStr(const string& p) 
	{ 
		//MLSUTIL::MlsMutexSelf	tMutex(&_pMutex);
		_sRightStr=p; 	
	}
	void setLeftStr2(const string& p) 
	{
		//MLSUTIL::MlsMutexSelf	tMutex(&_pMutex);
		_sLeftStr2=p;	
	}

	void setRightStr2(const string& p) 	
	{
		//MLSUTIL::MlsMutexSelf	tMutex(&_pMutex);
		_sRightStr2=p; 
	}
	
	bool isExit()
	{
		return false;
	}

	void show()
	{
	}

	void redraw()
	{
	}

	void setCount(int nCnt, int nCnt2 = -1)
	{
		//MLSUTIL::MlsMutexSelf	tMutex(&_tMutex);
		_nCnt = nCnt;
		if (nCnt2 != -1) _nCnt2 = nCnt2;
	}

	void*	Execute(void* /*pArg*/)
	{
		return (void*)this;
	}
};

class	LinMGlobal
{
	public:
		static QPixmap 	GetSmallIcon( MLS::File* pFile );
		static QPixmap 	GetSmallIcon( const QString& strIconName );
		static QIcon	GetIconSet( const QString& strIconName );
		static QColor	GetColor( int nNum );
};

#endif // __QT_DIALOG_H__
