#ifndef __NCurses_Editor_h__
#define __NCurses_Editor_h__

#include "define.h"
#include "editor.h"
#include "dialog.h"

#ifdef __LINM_SOURCE_HIGHLIGHT_USE__

#include "srchilite/langdefmanager.h"
#include "srchilite/regexrulefactory.h"
#include "srchilite/sourcehighlighter.h"
#include "srchilite/formattermanager.h"
#include "srchilite/formatterfactory.h"
#include "srchilite/stylefileparser.h" // for parsing style files
#include "srchilite/highlightbuilderexception.h"
#include "srchilite/parserexception.h"

#include "src_highlight.h"

#endif // __LINM_SOURCE_HIGHLIGHT_USE__


using namespace std;

namespace MLS
{

class NCurses_Editor:public Editor, public Form
{
	WINDOW*		_pWin;

	ColorEntry	_tLine;
	ColorEntry	_tColorEdit;
	ColorEntry	_tColorStat;
	ColorEntry	_tColorEditInfo;
	ColorEntry	_tColorEditInfoA;

    // replace
    wstring         _sReplaceStr;

	// Highlight section.
	string 			_sDefStyle;
	string 			_sDefHighLightPath;
	string          _strSyntexLang;
#ifdef __LINM_SOURCE_HIGHLIGHT_USE__    
	srchilite::RegexRuleFactory 	_tRuleFactory;
    srchilite::LangDefManager* 		_pLangDefManager;
	srchilite::SourceHighlighter*  	_pHighlighter;
    StyleFormatterFactory 			_tFactory;
	StyleFormatter*					_tStyleFormatter;
    srchilite::FormatterManager* 	_pFormatterManager;
	srchilite::FormatterParams 		_tFormatParams;
#endif // __LINM_SOURCE_HIGHLIGHT_USE__
	bool                            _bSyntexOn;

protected:
	bool	SyntexInit( const std::string& strSyntexLang = "cpp.lang");
    bool    GetLineSyntexData( const wstring& wLine, std::vector<SyntexData>& vSyntexData );
    
	void	PostLoad();
	void    PostUpdateLines( int nLineNum, int nHeight = 1 );
	
	void	DrawSyntexText( const wstring& str, 
	                        const std::vector<SyntexData>& vSyntexData, 
	                        int nY, int nN, bool bRevertColor );
	void	SelectionDraw(  const wstring& sViewWStr, 
	                        const std::vector<SyntexData>& vSyntexData, 
	                        int nY, int nN, int x1, int x2);
	void 	LineDraw(	const wstring&	sViewStr,
	                    const std::vector<SyntexData>& vSyntexData, 
						int nY, 
						int nN, 
						int nLineNum,   // current line offset
						int nNum);      // line
	void	CurserDraw(int nY, int nN);
	void	BackDrawEtc();
	void	Draw();

public:
	bool		_bFocus;
	bool		_bMouseMode;
	bool		_bFullScreen;
	bool		_bConfChg;

	NCurses_Editor(): Editor()
	{
		_bFocus = false;
		_bNoOutRefresh = true; // When Refresh() call, not call doupdate().
		_bNoViewUpdate = true; // Does not update.
		_bNotDrawBox = true; // Does not draw the box.
		_bMouseMode = true;  // Not use the mouse mode. (vclip copy in terminal )
		_bFullScreen = false; // FullScreen
		_bConfChg = false;		// When configure file is editing, this is true.
		
		Init();
	}
	
	~NCurses_Editor();

	void	Init();

	void	CurserDraw()
	{
		//  Change to cursor position
		if (_bLineNumView == true)
			CurserDraw(1, 3+_nLineWidth);
		else
			CurserDraw(1, 0);
	}

    void    Replace();
};

};

#endif

