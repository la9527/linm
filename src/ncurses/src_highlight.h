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

#ifndef __SRC_HIGHLIGHT_H__
#define __SRC_HIGHLIGHT_H__

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "editor.h"

#include "srchilite/formatter.h"
#include "srchilite/formatterfactory.h"
#include "srchilite/formatterparams.h"

#include "srchilite/ioexception.h"
#include "srchilite/docgenerator.h"
#include "srchilite/parserexception.h"

class StyleFormatter: public srchilite::Formatter
{
protected:
	std::vector<MLS::SyntexData>*	_pListSyntex;
	
	int getColorNumber( const std::string& strColorData );
	
public:
    /// the language element represented by this formatter
    std::string elem;
    bool bold, italic, underline, fixed, not_fixed;
    std::string color;
    std::string bgColor;

public:
    StyleFormatter(const std::string &elem_ = "normal");

	void setSyntexDataList( std::vector<MLS::SyntexData>* pListSyntex );
	
    virtual void format(const std::string &s,
            const srchilite::FormatterParams *params = 0);
};

/// shared pointer for StyleFormatter
typedef boost::shared_ptr<StyleFormatter> StyleFormatterPtr;
typedef std::map<std::string, StyleFormatterPtr> StyleFormatterMap;

class StyleFormatterFactory: public srchilite::FormatterFactory 
{
protected:
	std::vector<MLS::SyntexData>		_vListSyntex;

public:
    StyleFormatterMap 			formatterMap;
	
	void	clearSyntexList();
    const 	std::vector<MLS::SyntexData>& getSyntexList();

    bool hasFormatter(const string &key) const 
    {
        return formatterMap.find(key) != formatterMap.end();
    }

    bool createFormatter(const string &key, const string &color,
						const string &bgcolor,
						srchilite::StyleConstantsPtr styleconstants);
};

#endif /* __SRC_HIGHLIGHT_H__ */

