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

#include "ncurses_mcd.h"
#include "colorcfgload.h"

using namespace MLSUTIL;
using namespace MLS;

int ColLocation[13]={0, 4, 18, 32, 46, 60, 74, 88, 102, 116, 130, 144, 158 };	///< screen draw tree depth(x position)

void	McdDirButton::Draw()
{
	WINDOW*	pWin = _pForm->GetWin();
	if (_pNode == NULL) return;

	setcol(_tMCDColor, pWin);
	wmove(pWin, y, x);
	whline(pWin, ' ', width);
	
	if (_pNode->tFile.sFullName == "/")
	{
		if (_bSelect)
			setrcol(_tMCDColor, pWin);
		else
			setcol(_tMCDColor, pWin);
		wprintw(pWin, "/");
	}
	else
	{
		string sName = _pNode->tFile.sName;
#ifdef __APPLE__
		sName = MLSUTIL::CodeConvert( sName, "UTF-8", "UTF-8-MAC" );
#endif

		if (_bSelect)
		{
			setrcol(_tMCDColor, pWin);
			LOG("Selected [%s]", sName.c_str());
			if (scrstrlen(_pNode->tFile.sName) <= 12)
				wprintw(pWin, "%-12s", sName.c_str());
			else
				wprintw(pWin, "%-11.11s~", sName.c_str());
		}
		else
		{
			LOG("Mcd Data [%s]", sName.c_str());

			if (scrstrlen(sName) <= 12)
			{
				setcol(_tMCDColor, pWin);
				wprintw(pWin, "%s ", sName.c_str());
				
				if (!_pNode->vNode.empty())
				{
					setcol(_tLineColor, pWin);
					whline(pWin, HLINE, 12-sName.size());
				}
			}
			else 
			{
				setcol(_tMCDColor, pWin);
				wprintw(pWin, "%-11.11s~", sName.c_str());
			}
		}
	}

	wnoutrefresh(pWin);
}

void	NCurses_Mcd::Init()
{
	_tMCDColor = g_tColorCfg.GetColorEntry("MCD");
	_tMCDLine = g_tColorCfg.GetColorEntry("Line");
	_tMCDHighLight = g_tColorCfg.GetColorEntry("MCDHighLight");
}

void	NCurses_Mcd::DrawInit()
{
	werase(_pWin);
	//wbkgd(_pWin, COLOR(tMCD.font, tMCD.back));

	setcol(_tMCDColor, _pWin);
	for (int j=0;j< height; j++)
	{
		wmove(_pWin, j, 0);
		whline(_pWin, ' ', width);
	}
	
	setcol(_tMCDLine, _pWin);
	mvwaddch (_pWin, 0, 0, ULCORNER);
	mvwaddch (_pWin, 0, width-1, URCORNER);
	mvwaddch (_pWin, height-1, 0, LLCORNER);
	mvwaddch (_pWin, height-1, width-1, LRCORNER);
	mvwhline(_pWin, 0, 1, HLINE, width-2);
	mvwvline(_pWin, 1, 0, VLINE, height-2);
	mvwhline(_pWin, height-1, 1, HLINE, width-2);
	mvwvline(_pWin, 1, width-1, VLINE, height-2);
	
	_nRowSize = 0;
	
	// set width
	for (int i = 0; i < 13; i++)
	{
		if (width - 30 < ColLocation[i])
		{
			_nRowSize = i;
			break;
		}
		_nRowSize = 12;
	}

	//int nNewSize = (height-2 * _nRowSize) * 2;
	int nNewSize = _pOrder.size();
	if (_nBefMemSize != nNewSize)
	{
		_tMemPoolDirButton.Clear();
		_vDirButtonList.clear();
		
		for (int n = 0; n < nNewSize; n++)
		{
			McdDirButton& tDirButton = _tMemPoolDirButton.Get();
			tDirButton = McdDirButton();
			_vDirButtonList.push_back(&tDirButton);
		}
	}
	_nBefMemSize = _vDirButtonList.size();
}

///	\brief	Mcd Draw
void	NCurses_Mcd::Draw()
{
	WINDOW*	pWin = GetWin();

	vector<chtype> vLineChType;
	
    int nRow = 0, nCol = 0, nODep=0;
	int j = 0;

	DrawInit();

	Dir *pCur = (*_pCur);
	
	if (pCur->nRow - _nSRow > height-3) _nSRow = pCur->nRow-height+3;
	if (pCur->nDepth - _nSCol > _nRowSize) _nSCol = pCur->nDepth - _nRowSize;
	if (pCur->nRow - _nSRow < 0) _nSRow = pCur->nRow;
	if (pCur->nDepth - _nSCol < 1)
	{
		 _nSCol = pCur->nDepth-1;
		 if (_nSCol == -1) _nSCol=0;
	}

	// clean
	if (!_sStrSearch.empty())
	{
		setcol(_tMCDColor, pWin);
		mvwprintw(pWin, 0, width-25, "Search : [%-10s]", _sStrSearch.c_str());
	}

	setcol(_tMCDColor, pWin);
	mvwprintw(pWin, height-1, 2, "Path [ %s ]", isKorCode(pCur->Path()).c_str() );

	int nButtonCount = 0;

	Dir *pNode = NULL;
	
	for (pDirIterator i=_pOrder.begin(); i!=_pOrder.end(); i++)
	{
		pNode = *i;

		if (nODep < pNode->nDepth)
			vLineChType.push_back(VLINE);
		else
			while(pNode->nDepth < nODep)
			{
				nODep--;
				vLineChType.pop_back();
			}
		
		nODep = pNode->nDepth;
		nCol  = pNode->nDepth;
		nRow  = pNode->nRow;

		// for exit the section.
		if (pNode->nDepth != 0 && pNode->pParentDir->vNode.back()==pNode)
			vLineChType[nCol-1] = ' ';

		//LOG("nODep [%d] nCol [%d] nRow [%d] _nSRow [%d] _nSCol [%d]", nODep, nCol, nRow, _nSRow, _nSCol);
		//LOG("NCurses::Draw pNode->nDepth [%d]", pNode->nDepth);
			
		if (nRow - _nSRow > height-3) break;
		if (nRow - _nSRow < 0) continue;

		if (pNode != _pRoot && pNode->pParentDir->vNode.front()!= pNode)
		{
			for (int t = _nSCol; t < nCol && t < _nSCol+_nRowSize; t++)
			{
				setcol(_tMCDLine, pWin);
				mvwaddch(pWin, nRow-_nSRow+1, ColLocation[t-_nSCol+1], vLineChType[t]);
			}
		}
	
		if (nCol - _nSCol > _nRowSize ) continue;
		if (_nSCol != 0 && nCol - _nSCol < 1 ) continue;
		
		if (pNode->nDepth==0)
		{// if root directory.
			McdDirButton* pDirButton = _vDirButtonList[nButtonCount];

			pDirButton->SetForm((Form*)this);
			pDirButton->height = 1;
			pDirButton->width = 12;
			pDirButton->x = nRow-_nSRow+1;
			pDirButton->y = 1;

			// line draw more and name output.
			if (pNode == pCur)
				pDirButton->SetDir(pNode, _bFocus);
			else
				pDirButton->SetDir(pNode, false);

			pDirButton->Show();
			nButtonCount++;
		}		
		else
		{
			if (pNode->pParentDir->vNode.size() > 1)
			{	
				// if not one directory,
				if (pNode->pParentDir->vNode.front()==pNode)
				{	
					// first node
					setcol(_tMCDLine, pWin);
					wmove(pWin, nRow-_nSRow+1, ColLocation[nCol-_nSCol]);
					waddch(pWin, TTEE);
				}
				else
				{
					// another directory
					wmove(pWin, nRow-_nSRow+1, ColLocation[nCol-_nSCol]);

					setcol(_tMCDLine, pWin);
					if (pNode->pParentDir->vNode.back()==pNode)
						waddch(pWin, LLCORNER);
					else 
						waddch(pWin, LTEE); 
				}	
			}
			else
			{
				wmove(pWin, nRow-_nSRow+1, ColLocation[nCol-_nSCol]);
				setcol(_tMCDLine, pWin);
				waddch(pWin, HLINE); 
			}
			
			setcol(_tMCDHighLight, pWin);
			if (pNode->bCheck == false)
			{
				waddch(pWin, '+');
			}
			else
			{
				waddch(pWin, HLINE);
			}

			//LOG("NCurses_Mcd::Draw nButtonCount [%d] [%d]", nButtonCount, _vDirButtonList.size());

			McdDirButton* pDirButton = _vDirButtonList[nButtonCount];

			pDirButton->SetForm((Form*)this);
			pDirButton->height = 1;
			pDirButton->width = 12;
			pDirButton->x = ColLocation[nCol-_nSCol]+2;
			pDirButton->y = nRow-_nSRow+1;

			// line draw more and name output
			if (pNode == pCur)
				pDirButton->SetDir(pNode, _bFocus);
			else
				pDirButton->SetDir(pNode, false);

			pDirButton->Show();
			nButtonCount++;
		}
	}
	wnoutrefresh(_pWin);
}


bool	NCurses_Mcd::MouseEvent(int Y, int X, mmask_t bstate)
{
	McdDirButton* pDirButton = NULL;

	int nY = Y - y; int nX = X - x;
	for (int n = 0; n<_vDirButtonList.size(); n++)
	{
		pDirButton = _vDirButtonList[n];
		if (pDirButton->GetShowCheck() == true)
		{
			if (pDirButton->AreaChk(nY, nX))
			{
				LOG("MouseEvent Y [%d] X [%d]", Y, X);
				if (bstate & BUTTON1_CLICKED) 
				{
					SetCur(pDirButton->GetNode()->tFile.sFullName);
				}
				else if (bstate & BUTTON1_DOUBLE_CLICKED) 
				{
					SetCur(pDirButton->GetNode()->tFile.sFullName);
					return true;
				}
				break;
			}
		
			if (pDirButton->y == nY && pDirButton->x == nX+1)
			{
				if (bstate & BUTTON1_CLICKED) 
				{
					if (pDirButton->GetNode()->bCheck == false)
					{
						SetCur(pDirButton->GetNode()->tFile.sFullName);
						SubDirOneSearch();
					}
					else
					{
						SetCur(pDirButton->GetNode()->tFile.sFullName);
						SubDirClear();
					}
				}
				break;
			}
		}
	}
	return false;
}

void	NCurses_Mcd::Key_PageDown()
{
	int nDepth = (*_pCur)->nDepth;
	int nFindRow = (*_pCur)->nRow + height - 3;
	Dir* pNode = GetDir_RowArea(nFindRow, nDepth);
	
	if (pNode)
	{
		LOG("pNode [%d]", pNode->nRow);
		LOG("pNode [%d]", pNode->tFile.sFullName.c_str());
		SetCur(pNode->tFile.sFullName);
	}
}

void	NCurses_Mcd::Key_PageUp()
{
	int nDepth = (*_pCur)->nDepth;
	int nFindRow = (*_pCur)->nRow - height + 3;
	Dir* pNode = GetDir_RowArea(nFindRow, nDepth);
	
	if (pNode)
	{
		LOG("pNode [%d]", pNode->nRow);
		LOG("pNode [%d]", pNode->tFile.sFullName.c_str());
		SetCur(pNode->tFile.sFullName);
	}
}
