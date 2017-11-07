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
#include <exception.h>

#include "define.h"
#include "strutil.h"
#include "ncurses_editor.h"
#include "dialog_replace.h"
#include "strlinetoken.h"
#include "mlscfgload.h"
#include "syntexcfgload.h"

using namespace MLSUTIL;
using namespace MLS;

NCurses_Editor::~NCurses_Editor()
{
#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
	if ( _pFormatterManager ) delete _pFormatterManager;
	if ( _pLangDefManager ) delete _pLangDefManager;
	if ( _pHighlighter ) delete _pHighlighter;
#endif // __LINM_SOURCE_HIGHLIGHT_USE__
}

void	NCurses_Editor::Init()
{
	_tColorEdit = g_tColorCfg.GetColorEntry("Edit");
	_tColorStat = g_tColorCfg.GetColorEntry("Stat");
	_tColorEditInfo = g_tColorCfg.GetColorEntry("EditInfo");
	_tColorEditInfoA = g_tColorCfg.GetColorEntry("EditInfoA");
	_tLine = g_tColorCfg.GetColorEntry("Line");

	_sDefStyle = g_tSyntaxExtCfg.GetValue("HighLight", "DefaultStyle", "default.style" );
	string strName = g_tSyntaxExtCfg.GetValue("HighLight", "ConfigPath",
						"/usr/local/share/source-highlight;/usr/share/source-highlight" );

	LOG( "HIGHLIGHT :: [%s] [%s]", _sDefStyle.c_str(), strName.c_str() );
	
	StringToken st(strName, ";");
	while(st.Next())
	{
		string sCheckStyleFile;
		string sPath = st.Get();

		if ( sPath.substr( sPath.length()-1, sPath.length() ) == "/" ) 
			sCheckStyleFile = sPath + _sDefStyle;	
		else
			sCheckStyleFile = sPath + "/" + _sDefStyle;

		LOG( "HIGHLIGHT :: [%s]", sCheckStyleFile.c_str() );

		if ( access( sCheckStyleFile.c_str(), R_OK ) == 0 )
		{
			_sDefHighLightPath = sPath;
			break;
		}
	}

	LOG( "HIGHLIGHT :: PATH [%s]", _sDefHighLightPath.c_str() );
#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
	_pLangDefManager = 0; 
	_pHighlighter = 0;
    _pFormatterManager = 0;
#endif // __LINM_SOURCE_HIGHLIGHT_USE__
}

#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
bool NCurses_Editor::GetLineSyntaxData( const wstring& wLine, std::vector<SyntaxData>& vSyntaxData, srchilite::HighlightStatePtr& statePtr )
{
    string	sTab;
	sTab.append(1, (char)TABCONVCHAR);

    vSyntaxData.clear();
    
	if ( !_pHighlighter ) return false;
	    
	string sViewStr = MLSUTIL::Replace(wstrtostr(wLine), sTab.c_str(), " "); // Tab -> Space
	if ( sViewStr.size() == 0 ) return false;
	
	_tFactory.clearSyntaxList();
	_tFormatParams.start = 0;

	try 
	{
	    //LOG( "GetLineSyntaxData [%s]", sViewStr.c_str() );
		_pHighlighter->highlightParagraph( sViewStr );

		statePtr = _pHighlighter->getCurrentState();
	} catch (const srchilite::HighlightBuilderException &e) {
        LOG( "%s", e.what() );
		_tFactory.clearSyntaxList();
		return false;
    } catch (const srchilite::ParserException &e) {
        LOG( "%s", e.what() );
		_tFactory.clearSyntaxList();
		return false;
    } catch (const srchilite::IOException &e ) {
        LOG( "%s", e.what() );
		_tFactory.clearSyntaxList();
		return false;
    } catch (const std::exception &e) {
        LOG( "%s", e.what() );
		_tFactory.clearSyntaxList();
		return false;
    }

	vSyntaxData = _tFactory.getSyntaxList();

	for( SyntaxData& tData : vSyntaxData ) {
		string::size_type type = sViewStr.find(tData.strString, tData.nStart);
		if ( type != string::npos ) {
			int nScrLen = scrstrlen(sViewStr.substr(0, type));
			if ( nScrLen != tData.nStart ) {
				LOG( "SYNTAX SYNC - [%s] -> [%d] : [%d]", sViewStr.c_str(), tData.nStart, nScrLen );
				tData.nScreenPos = nScrLen;
			} else {
				tData.nScreenPos = tData.nStart;
			}
		}
	}
    return true;
}
#endif // __LINM_SOURCE_HIGHLIGHT_USE__

void	NCurses_Editor::PostLoad()
{
    LOG( "NCurses_Editor::PostLoad()" );
    _bSyntaxOn = false;

#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
	if ( _tFile.Ext().length() > 0 )
	{
        string  strSyntaxFileName;
        if ( g_tSyntaxExtCfg.getExtSyntax( _tFile.Ext(), strSyntaxFileName ) )
        {
            if ( SyntaxInit( strSyntaxFileName ) ) {
                _bSyntaxOn = true;
            }
        }
	}
#endif
    if ( !_bSyntaxOn )
    {
        string  sName =_tFile.sName;
        string::size_type p = sName.rfind(".");
		if (p != string::npos && p !=0 )
		    sName = sName.substr(0, p+1);

        string  strSyntaxFileName;
        if ( g_tSyntaxExtCfg.getNameSyntax( sName, strSyntaxFileName ) ) {
            if ( SyntaxInit(strSyntaxFileName) ) {
                _bSyntaxOn = true;
            }
        }
    }
    LOG( "NCurses_Editor::PostLoad() :: _bSyntaxOn : %s", _bSyntaxOn ? "true" : "false" );

#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
    if ( _bSyntaxOn )
    {
		_pHighlighter->clearStateStack();

        for ( int n = 0; n < _vText.size(); n++ )
            GetLineSyntaxData( _vText[n].wLine, _vText[n].vSyntaxData, _vText[n].statePtr );
    }
#endif
}

void	NCurses_Editor::PostUpdateLines( int nLineNum, int nHeight )
{
#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
    LOG( "PostUpdateLines !!! - %d", nLineNum );
	for ( int n = 0; n < 20; n++ )
    {
		if ( n == 0 && _vText[nLineNum + n].statePtr != nullptr ) {
			_pHighlighter->setCurrentState( _vText[nLineNum + n].statePtr );
		}
        if ( nLineNum+n < _vText.size() ) {
			GetLineSyntaxData(_vText[nLineNum + n].wLine,
							  _vText[nLineNum + n].vSyntaxData,
							  _vText[nLineNum + n].statePtr);
		}
    }
#endif // __LINM_SOURCE_HIGHLIGHT_USE__
}

#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
bool	NCurses_Editor::SyntaxInit( const std::string& strSyntaxLang )
{
	bool bFail = false;

	try
	{
		if ( !_pLangDefManager )
		{
			_pLangDefManager = new srchilite::LangDefManager( &_tRuleFactory );

			// we highlight C++ code for simplicity
			_pHighlighter = new srchilite::SourceHighlighter(
					_pLangDefManager->getHighlightState(_sDefHighLightPath, strSyntaxLang) );

			// the background color for the document (not used here)
			std::string strDocBgColor;

			// let's parse the default.style and create our formatters
			srchilite::StyleFileParser::parseStyleFile(_sDefHighLightPath, _sDefStyle, 
														&_tFactory, strDocBgColor);

			// now we need to fill up the formatter manager with our formatters
			_pFormatterManager = new srchilite::FormatterManager(StyleFormatterPtr( new StyleFormatter ));

			int n = 0;
			for (StyleFormatterMap::const_iterator it = _tFactory.formatterMap.begin(); 
					it != _tFactory.formatterMap.end(); ++it) 
			{
			    _pFormatterManager->addFormatter(it->first, it->second);
			}
			_pHighlighter->setFormatterManager(_pFormatterManager);
			_pHighlighter->setFormatterParams(&_tFormatParams);
		}
		else
		{
			if ( _pHighlighter ) delete _pHighlighter;

			_pHighlighter = new srchilite::SourceHighlighter(
				_pLangDefManager->getHighlightState(_sDefHighLightPath, strSyntaxLang) );
			_pHighlighter->setFormatterManager(_pFormatterManager);
			_pHighlighter->setFormatterParams(&_tFormatParams);
		}
	} 
    catch (const srchilite::HighlightBuilderException &e) {
        LOG( "%s", e.what() );
		bFail = true;
    } catch (const srchilite::ParserException &e) {
        LOG( "%s", e.what() );
		bFail = true;
	} catch (const srchilite::IOException &e ) {
        LOG( "%s", e.what() );
		bFail = true;
    } catch (const std::exception &e) {
        LOG( "%s", e.what() );
		bFail = true;
    }

	if ( bFail )
	{
		if ( _pLangDefManager ) delete _pLangDefManager; _pLangDefManager = 0;
		if ( _pHighlighter ) delete _pHighlighter; _pHighlighter = 0;
		return false;
	}
	_strSyntaxLang = strSyntaxLang;
    LOG( "SyntaxInit :: %s :: true", (const char*)strSyntaxLang.c_str() );
    return true;
}
#endif // __LINM_SOURCE_HIGHLIGHT_USE__

void	NCurses_Editor::BackDrawEtc()
{
	_bNoOutRefresh = true; // when refresh the screen, not doupdate() call.
	_bNoViewUpdate = true; // Did not update.
	_bNotDrawBox = true; // Did not draw the box.

	wbkgd(_pWin, COLOR(_tColorEdit.font, _tColorEdit.back));
	
	setcol(_tColorEdit, _pWin);
	
	for (int n = 0; n < height; n++)
	{
		wmove(_pWin, n, 0);
		whline(_pWin, ' ', width);
	}

	if ( !_bFullScreen )
	{
		setcol(_tLine, _pWin);
		mvwaddch (_pWin, 0, 0, ULCORNER);
		mvwaddch (_pWin, 0, width-1, URCORNER);
		mvwaddch (_pWin, height-1, 0, LLCORNER);
		mvwaddch (_pWin, height-1, width-1, LRCORNER);
		mvwhline(_pWin, 0, 1, HLINE, width-2);
		mvwvline(_pWin, 1, 0, VLINE, height-2);
		mvwhline(_pWin, height-1, 1, HLINE, width-2);
		mvwvline(_pWin, 1, width-1, VLINE, height-2);
	}
	// title output
	/*
	setcol(_tColorStat, _pWin);
	wmove(_pWin, 1, 0);
	whline(_pWin, ' ', width);
	mvwprintw(_pWin, 1, (width - krstrlen(_title))/2 , "%s", _title.c_str());
	*/
}

void NCurses_Editor::DrawSyntaxText(const wstring& str,
                                    std::vector<SyntaxData>& vSyntaxData,
                                    int nY, int nN, bool bRevertColor )
{
	string	sTab;
	sTab.append(1, (char)TABCONVCHAR);

#ifndef __LINM_SOURCE_HIGHLIGHT_USE__
	bool	bSyntaxOn = false;
#else
	bool	bSyntaxOn = _bSyntaxOn;
#endif
	
	if ( !bSyntaxOn )
	{
		string sViewStr = wstrtostr(str);
		string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
		if ( !bRevertColor )
    		setcol(_tColorEdit, _pWin);
    	else
    	    setrcol(_tColorEdit, _pWin);
		mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
	}
	else
	{
#ifdef __LINM_SOURCE_HIGHLIGHT_USE__
		string sViewStr = MLSUTIL::Replace(wstrtostr(str), sTab.c_str(), " "); // Tab -> Space
		if ( sViewStr.size() == 0 ) return;
	
		setcol(_tColorEdit, _pWin);
		mvwprintw(_pWin, nY, nN, "%s", sViewStr.c_str());

		int nX = nN;
		for ( unsigned long n = 0; n < vSyntaxData.size(); n++ ) {
			ColorEntry	tColorEdit = _tColorEdit;
			tColorEdit.font = vSyntaxData[n].nColor;
			//tColorEdit.back = vSyntaxData[n].nBgColor;
	
			if ( !bRevertColor )
				setcol(tColorEdit, _pWin);
			else
				setrcol(tColorEdit, _pWin);

            mvwprintw(_pWin, nY, nX + vSyntaxData[n].nScreenPos, "%s", vSyntaxData[n].strString.c_str());
			//LOG( "%03d [%s] [%d] [%d] Color [%d]", n, vSyntaxData[n].strString.c_str(),
			//                      nY, nX + vSyntaxData[n].nStart, tColorEdit.font );
		}
#endif
	}
}

void NCurses_Editor::SelectionDraw(const wstring& sViewWStr, 
                                   std::vector<SyntaxData>& vSyntaxData,
                                   int nY, int nN, int x1, int x2)
{
	string	sViewStr, sViewStr2, sViewStr3;
	wstring sWStr1, sWStr2, sWStr3;

	string	sTab;
	sTab.append(1, (char)TABCONVCHAR);

	if (sViewWStr.size() == 0) return;

	if (x1 >= sViewWStr.size()) x1 = sViewWStr.size();
	if (x2 >= sViewWStr.size()) x2 = sViewWStr.size();
	
	sWStr1 = sViewWStr.substr(0, x1);
	sWStr2 = sViewWStr.substr(x1, x2-x1);
	//sWStr3 = sViewWStr.substr(x2, sViewWStr.size());

	DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );

	if (sWStr2.size() != 0)
	{
		sViewStr2 = wstrtostr(sWStr2);
		sViewStr2 = MLSUTIL::Replace(sViewStr2, sTab.c_str(), " "); // Tab -> Space
	
		setrcol(_tColorEdit, _pWin);// reverse
		mvwprintw(_pWin, nY, nN+wstrlen(sWStr1), "%s", sViewStr2.c_str());
	}
}

void NCurses_Editor::LineDraw(	const wstring&	sViewWStr,
                                std::vector<SyntaxData>& vSyntaxData,
						        int nY, 
						        int nN, 
						        int nTextNum,
						        int nNum)
{
	setcol(_tColorEdit, _pWin);
	wmove(_pWin, nY, nN);

	int 	nBoxLine = 0;

	if ( _bFullScreen )
		nBoxLine = 0;
	else
		nBoxLine = 1;
	
	whline(_pWin, ' ', width-(nBoxLine*2));
	
	if (_bLineNumView == true)
	{
		setcol(_tColorEditInfo, _pWin);
		mvwprintw(_pWin, nY, nN+1, "%*d", _nLineWidth, nTextNum+1);
		setcol(_tColorEditInfo, _pWin);
		mvwhline(_pWin, nY, nN+_nLineWidth+1, VLINE, 1);
		nN = 3+_nLineWidth+nN;
	}
	
	string	sTab;
	sTab.append(1, (char)TABCONVCHAR);

	if (_EditMode == EDIT)
	{
		DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );
	}
	else if (_EditMode == SELECT || _EditMode == SHIFT_SELECT)
	{
		EditSelect	tEditSelect = _EditSelect;
		SelectSort(&tEditSelect);

		int x1 = tEditSelect.x1;
		int x2 = tEditSelect.x2;

		wstring 		sLineWStr = _vText[nTextNum].wLine;
		wstring			sWStr;
		StrLineToken	tStrLineToken;
		tStrLineToken.SetWString(sLineWStr, _nCulumn);
		
		int				nCurSize = 0;
		for (int n = 0; n<tStrLineToken.LineSize(); n++)
		{
			sWStr = tStrLineToken.GetLineStr(n);
			if (n == nNum) break;
			nCurSize=nCurSize+sWStr.size();
		}

		int	nCurEndSize = nCurSize+sViewWStr.size();
		
		if (tEditSelect.x1 > nCurSize)	x1 = tEditSelect.x1 - nCurSize;
		if (tEditSelect.x2 > nCurSize)	x2 = tEditSelect.x2 - nCurSize;
		
		if (tEditSelect.y1 == nTextNum && tEditSelect.y2 == nTextNum)
		{
			if (tEditSelect.x1 >= nCurSize && tEditSelect.x2 <= nCurEndSize)
				SelectionDraw(sViewWStr, vSyntaxData, nY, nN, x1, x2);
			else if (tEditSelect.x1 >= nCurSize && tEditSelect.x2 > nCurEndSize)
				SelectionDraw(sViewWStr, vSyntaxData, nY, nN, x1, sViewWStr.size());
			else if (tEditSelect.x1 < nCurSize && tEditSelect.x2 > nCurSize)
				SelectionDraw(sViewWStr, vSyntaxData, nY, nN, 0, x2);
			else if (tEditSelect.x1 < nCurSize && tEditSelect.x2 > nCurEndSize)
				DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, true );
			else
				DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );
		}
		else if (tEditSelect.y1 == nTextNum)
		{
			if (tEditSelect.x1 >= nCurSize)
			{
				SelectionDraw(sViewWStr, vSyntaxData, nY, nN, x1, sViewWStr.size());
				
				string sViewStr = wstrtostr(sViewWStr);
				string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
				wmove(_pWin, nY, nN+scrstrlen(sViewStr2));
				whline(_pWin, ' ', width-nN-scrstrlen(sViewStr2)-nBoxLine);
			}
			else if (tEditSelect.x1 < nCurSize)
				DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );
			else
				DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );
		}
		else if (tEditSelect.y2 == nTextNum)
		{
			if (tEditSelect.x2 > nCurSize)
				SelectionDraw(sViewWStr, vSyntaxData, nY, nN, 0, x2);
			else if (tEditSelect.x2 > nCurEndSize)
				DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, true );
			else
				DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );
		}
		else if (tEditSelect.y1 < nTextNum && tEditSelect.y2 > nTextNum)
		{
			string sViewStr = wstrtostr(sViewWStr);
			string sViewStr2 = MLSUTIL::Replace(sViewStr, sTab.c_str(), " "); // Tab -> Space
			setrcol(_tColorEdit, _pWin); // reverse
			wmove(_pWin, nY, nN);
			whline(_pWin, ' ', width-nBoxLine);
			mvwprintw(_pWin, nY, nN, "%s", sViewStr2.c_str());
		}
		else
		{
			DrawSyntaxText( sViewWStr, vSyntaxData, nY, nN, false );
		}
	}
}

void NCurses_Editor::CurserDraw(int nY, int nN)
{
	LineInfo			tLineInfo;
	vector<LineInfo>	vLineInfo;

	for (uint n = 0; n < _vViewString.size(); n++)
	{
		tLineInfo = _vViewString[n];
		if (tLineInfo.nTextLine == _nCurLine) 
			vLineInfo.push_back(tLineInfo);
	}

	int		cur = _nCurCulumn;
	int		length = 0;
	int 	line1 = 0;
	
	for (uint n = 0; n < vLineInfo.size(); n++)
	{
		tLineInfo = vLineInfo[n];
		length = length + tLineInfo.sWString.size();
		if (length >= _nCurCulumn)
		{
			line1 = tLineInfo.nViewLine;
			break;
		}
		cur = cur - tLineInfo.sWString.size();
	}
	cur = wstrlen((tLineInfo.sWString.substr(0, cur)));

	touchwin(_pWin);
	wmove(_pWin, nY+line1, nN+cur);
	//setsyx(y+nY+line1, x+nN+cur);
	
	wnoutrefresh(_pWin);
	LOG("CURSOR :: [%d] [%d] [%d] [%d]", line1, cur, _nCurCulumn, vLineInfo.size());
}

void NCurses_Editor::Draw()
{
	_pWin = GetWin();

	if (!_pWin)
	{
		LOG("NCurses_Editor pWin NULL");
		return;
	}

	uint 		nViewLine = 0;
	wstring 	sLineStr, sViewStr;
	string		sSpaceTab;
	string		sInfo1, sInfo2, sInfo3;
	char		sLineFormat[50];

	int nViewWidth = 0, nKrStrSize = 0, nBoxLine = 0;

	LineInfo	        tLineInfo;
	vector<SyntaxData>  vSyntaxInfo;

	if ( _bFullScreen ) 
		nBoxLine = 0;
	else
		nBoxLine = 1;

	_nLine = height - nBoxLine - 2;
	_nLineWidth = itoa(_vText.size()).size();
	if (_bLineNumView == true)
	{
		_nCulumn = width - (_nLineWidth+5+(nBoxLine*2));
	}
	else
		_nCulumn = width - (nBoxLine*2);

	ScreenMemSave(_nLine, _nCulumn);

	werase(_pWin);

	BackDrawEtc();

	for (int t=0; t< _nLine; t++)
	{
		if (_vViewString.size() > t)
		{
			tLineInfo = _vViewString[t];
			vSyntaxInfo = _vViewString[t].vSyntaxData;

			LineDraw(tLineInfo.sWString, vSyntaxInfo, nBoxLine+t, nBoxLine, tLineInfo.nTextLine, tLineInfo.nNextLineNum);
		}
		else
		{
			setcol(_tColorEdit, _pWin);
			wmove(_pWin, nBoxLine+t, nBoxLine);
			whline(_pWin, ' ', _nCulumn);

			if (_bLineNumView == true)
			{
				setcol(COLOR_BLACK, _tColorEdit.back, _pWin);
				mvwprintw(_pWin, nBoxLine+t, nBoxLine+3+_nLineWidth, "~");
			}
			else
			{
				setcol(COLOR_BLACK, _tColorEdit.back, _pWin);
				mvwprintw(_pWin, nBoxLine+t, nBoxLine, "~");
			}
		}
		nViewLine++;
	}

	setcol(_tColorEdit, _pWin);
	wmove(_pWin, height-(nBoxLine*2), nBoxLine);
	whline(_pWin, ' ', width-(nBoxLine*2));

	if (_bInsert)
		sInfo1 = _("[Ins]");
	else
		sInfo1 = _("[Ovr]");
		
	if (_EditMode == SELECT)
		sInfo1 = _("[Select]");

	if (_bDosMode)	sInfo2 = "[DOS]";

	if (_eEncode == KO_EUCKR)		sInfo3 = "[EUC-KR]";
	else if (_eEncode == KO_UTF8)	sInfo3 = "[UTF-8]";
	else							sInfo3 = "[US]";

	setcol(_tColorEditInfoA, _pWin);
	mvwprintw(_pWin, height-nBoxLine-1, width-scrstrlen(sInfo1)-nBoxLine, (char*)sInfo1.c_str());
	if (_bDosMode)
	{
		setcol(_tColorEditInfoA, _pWin);
		mvwprintw(_pWin, height-nBoxLine-1, width-scrstrlen(sInfo2)-scrstrlen(sInfo1)-nBoxLine, (char*)sInfo2.c_str());
	}
	setcol(_tColorEditInfo, _pWin);
	mvwprintw(_pWin,height-nBoxLine-1, width-scrstrlen(sInfo3)-scrstrlen(sInfo2)-scrstrlen(sInfo1)-nBoxLine, (char*)sInfo3.c_str());

	setcol(_tColorEditInfo, _pWin);
	snprintf(sLineFormat, sizeof(sLineFormat), "Line: %d Col: %d (%d%%) ",
				_nCurLine+1, _nCurCulumn, (int)((_nCurLine+1)*100 / _vText.size()) );
	mvwprintw(_pWin, height-nBoxLine-1,
			  width-scrstrlen(sLineFormat)-scrstrlen(sInfo3)-scrstrlen(sInfo2)-scrstrlen(sInfo1)-nBoxLine,
			  sLineFormat);
	
	if (_bReadOnly == true)
	{
		setcol(_tColorEditInfo, _pWin);
		mvwprintw(_pWin, height-nBoxLine-1, 10, _("[Read Only]"));
	}
	else
	{
		if (_nConvInfo != _vDoInfo.size())
		{
			setcol(_tColorEditInfoA, _pWin);
			mvwprintw(_pWin, height-nBoxLine-1, 10, _("[Change]"));
		}
	}
	
	//  change the cursor position.
	if (_bLineNumView == true)
		CurserDraw(nBoxLine, nBoxLine+3+_nLineWidth);
	else
		CurserDraw(nBoxLine, nBoxLine);
}

void NCurses_Editor::Replace()
{
    string  sFind = wstrtostr(_sFindStr);
    string  sReplace = wstrtostr(_sReplaceStr);

    {
        Dialog_Replace      tReplace(_("Replace"), sFind, sReplace);
    
        tReplace.Size(9, 50);
        tReplace.Move(MIDDLE, MIDDLE);
        
        tReplace.Do();
        tReplace.Close();

        if ( !tReplace.GetOk() ) return;
        
        sFind = tReplace.GetFind();
        sReplace = tReplace.GetReplace();
    }

    if (sFind.size() == 0) return;
    if (sReplace.size() == 0) return;

    _sFindStr = strtowstr(sFind);
    _sReplaceStr = strtowstr(sReplace);

    _nFindPosX = 0;
    _nFindPosY = _nCurLine;
    
    wstring         str;
    wstring         sLine, sLine1, sLine2, sLine3;
    wstring::size_type idx;
    bool            bReplaceAll = false;

    for(;;)
    {
        for (int n = _nFindPosY; n<(int)_vText.size(); n++)
        {
            do
            {
                idx = _vText[n].wLine.find(_sFindStr, _nFindPosX);
                if (idx != wstring::npos)
                {
                    _nFindPosX = idx+_sFindStr.size();
                    _nFindPosY = n;
                    _EditMode = SHIFT_SELECT;
                    _EditSelect.x1 = idx;
                    _EditSelect.y1 = n;
                    _EditSelect.x2 = idx+_sFindStr.size();
                    _EditSelect.y2 = n;
                    _nCurCulumn = idx+_sFindStr.size();
                    _nCurLine = n;
                    _nCurCulumn_Max = _nCurCulumn;
                    _nFirstLine = _nCurLine - 10;

                    Draw();

                    bool bSkip = false;

                    if ( !bReplaceAll )
                    {
                        vector<string> q;
                        q.push_back(_("Replace"));
                        q.push_back(_("Replace All"));
                        q.push_back(_("Skip"));
                        q.push_back(_("Cancel"));
        
                        int     nSelect = 0;
                        nSelect = SelectBox(_("Replace"), q, 0);
    
                        switch( nSelect )
                        {
                            case 0: 
                                break;
                            case 1:     
                                bReplaceAll = true;
                                break;
                            case 2:
                                bSkip = true;
                                break;
                            case 3:
                                return;
                                break;
                            default:
                                return;
                        }     
                    }

                    if ( !bSkip )
                    {   
                        sLine = _vText[_nCurLine].wLine;
    
                        // Data save for undo.
                        _vDoInfo.push_back(new DoInfo(_nCurLine, _nCurCulumn, 1, sLine));

                        _nFindPosX = idx+_sReplaceStr.size();
                        _nFindPosY = n;
                        _EditMode = SHIFT_SELECT;
                        _EditSelect.x1 = idx;
                        _EditSelect.y1 = n;
                        _EditSelect.x2 = idx+_sReplaceStr.size();
                        _EditSelect.y2 = n;
                        _nCurCulumn = idx+_sReplaceStr.size();
                        _nCurLine = n;
                        _nCurCulumn_Max = _nCurCulumn;
                        _nFirstLine = _nCurLine - 10;
    
                        sLine2 = sLine.substr(0, idx) + 
                                _sReplaceStr + 
                                sLine.substr( idx+_sFindStr.size() );
    
                        _vText[_nCurLine] = sLine2;
                    }   
                }
            }
            while( idx != wstring::npos );
            _nFindPosX = 0;
        }

        _nFindPosY= 0;
        Draw();
        if (YNBox(  _("Replace Next"), 
                    _("End of document reached. Continue from the begining?"), 
                    true) == false)
            break;
    }
}


