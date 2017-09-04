/******************************************************************************
 *   Copyright (C) 2010 by la9527                                             *
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

#include "define.h"
#include "src_highlight.h"
#include "dialog.h"

#include "srchilite/langdefmanager.h"
#include "srchilite/regexrulefactory.h"
#include "srchilite/sourcehighlighter.h"
#include "srchilite/formattermanager.h"
#include "srchilite/stylefileparser.h" // for parsing style files

using namespace MLSUTIL;
using namespace MLS;

StyleFormatter::StyleFormatter(const std::string &elem_ ) :
        elem(elem_), bold(false), italic(false), underline(false),
                fixed(false), not_fixed(false)
{
}

int	StyleFormatter::getColorNumber( const string& strColorName )
{
	/* Console Color
	# 0:Black     1:Red     2:Green            3:Brown       
	# 4:Blue      5:Violet  6:Bluish green     7:White  
	# 8:Gray      9:Orange  10:Yellowish green 11:Yellow  
	# 12:Sky Blue 13:Pink   14:Light Blue      15:Bright White
	*/
	if ( strColorName == "black" ) 			return 7; // 15
	if ( strColorName == "darkred" ) 		return 7;
	if ( strColorName == "red" ) 			return 1; // 1
	if ( strColorName == "darkgreen" ) 		return 2;
	if ( strColorName == "green" ) 			return 2;
	if ( strColorName == "brown" )			return 3;
	if ( strColorName == "darkblue" ) 		return 4;
	if ( strColorName == "purple" ) 		return 12;
	if ( strColorName == "cyan" ) 			return 6;
	if ( strColorName == "normal" ) 		return 7;
	if ( strColorName == "gray" ) 			return 8;
	if ( strColorName == "orange" ) 		return 9;
	if ( strColorName == "brightorange" ) 	return 10;
	if ( strColorName == "yellow" ) 		return 11;
	if ( strColorName == "brightgreen" ) 	return 12;
	if ( strColorName == "pink" ) 			return 13;
	if ( strColorName == "teal" ) 			return 14;
	if ( strColorName == "blue" ) 			return 11;
	return 7; // normal color value.
}

void StyleFormatter::setSyntexDataList( std::vector<SyntexData>* pListSyntex )
{
	_pListSyntex = pListSyntex;
}

void StyleFormatter::format(const std::string &str,
            const srchilite::FormatterParams *params)
{
	int	nColorNum = 7, nBgColorNum = -1;
	if ( color.size() )
		nColorNum = getColorNumber( color );
	
	if ( bgColor.size() )
		nBgColorNum = getColorNumber( bgColor );
	
	if ( str.size() && nColorNum != 7 )
	{
		SyntexData		tSyntexData( str, nColorNum, nBgColorNum );
		tSyntexData.bold = bold;
		tSyntexData.italic = italic;
		tSyntexData.underline = underline;
		tSyntexData.fixed = fixed;
		tSyntexData.not_fixed = not_fixed;
		if ( params )
			tSyntexData.nStart = params->start;
/* Debug code.
		string strTmp;
		if ( tSyntexData.bold )			strTmp =+ "B";
		if ( tSyntexData.italic )		strTmp =+ "I";
		if ( tSyntexData.underline )	strTmp =+ "U";
		if ( tSyntexData.fixed )		strTmp =+ "F";
		if ( tSyntexData.fixed )		strTmp =+ "N";
		LOG_WRITE( "pushed [%s] [%d] [%d] [%s] [%s]", tSyntexData.strString.c_str(), tSyntexData.nStart, tSyntexData.nColor, color.c_str(), strTmp.c_str()  );
*/	
		_pListSyntex->push_back( tSyntexData );
	}
}

void StyleFormatterFactory::clearSyntexList()
{
	_vListSyntex.clear();
}

const vector<SyntexData>& 	StyleFormatterFactory::getSyntexList()
{
	return _vListSyntex;
}

bool StyleFormatterFactory::createFormatter(const string &key,
											const string &color,
            								const string &bgcolor,
            								srchilite::StyleConstantsPtr styleconstants)
{
	if (hasFormatter(key))
		return false;
	
	StyleFormatter *formatter = new StyleFormatter(key);
	formatter->setSyntexDataList( &_vListSyntex );
	
	formatterMap[key] = StyleFormatterPtr(formatter);

	if (styleconstants.get())
	{
		for (srchilite::StyleConstantsIterator it = styleconstants->begin();
			it != styleconstants->end(); ++it)
		{
			switch (*it) 
			{
				case srchilite::ISBOLD:
					formatter->bold = true;
                	break;
				case srchilite::ISITALIC:
					formatter->italic = true;
					break;
				case srchilite::ISUNDERLINE:
					formatter->underline = true;
					break;
				case srchilite::ISFIXED:
					formatter->fixed = true;
					break;
				case srchilite::ISNOTFIXED:
					formatter->not_fixed = true;
					break;
				case srchilite::ISNOREF: // ignore references here
					break;
			}
		}
	}

	formatter->color = color;
	formatter->bgColor = bgcolor;
	return true;
}

