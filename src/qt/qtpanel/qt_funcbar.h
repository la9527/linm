#ifndef QT_FUNCBAR_H
#define QT_FUNCBAR_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QLabel>

#include "define.h"

class QHBoxLayout;
class PanelCmd;

/**
	@author Byoungyoung, La <la9527@yahoo.co.kr>
*/
class FuncBar : public QWidget
{
Q_OBJECT
public:
    FuncBar( QWidget *parent = 0, PanelCmd* pCmdList = 0 );
    ~FuncBar();

	void	init( ViewType tType = PANEL );

private:
	QVector<QPushButton*>		_vButtons;
	PanelCmd*					_pCmdList;
	QHBoxLayout*				_pHBox;
};

#endif
