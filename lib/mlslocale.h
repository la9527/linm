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

/// @file	mlslocale.h
/// @brief	locale 설정 관련 소스
/// @author 

#ifndef __MLSLOCALE_H__
#define __MLSLOCALE_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#ifdef HAVE_LC_MESSAGES
#include <locale.h>
#endif

#ifdef HAVE_GETTEXT
#include <libintl.h>
#endif

#include "define.h"
#include "strutil.h"

namespace MLSUTIL {
    /// @brief	locale language enum
    enum ENCODING {
        KO_EUCKR,    ///< korean Language - EUC-KR
        KO_UTF8,    ///< korean Language - UTF-8
        US,            ///< USA Language
        AUTO        ///< AUTO 자동으로 바꿔준다.
    };

    extern ENCODING e_nCurLang; // 현재 시스템의 language 를 세팅한다.
    extern ENCODING e_nBefLang;

    void Set_Locale(ENCODING e);

#ifdef HAVE_ICONV
    char* code_convert(const char *str,
                        const char *to_codeset,
                        const char *from_codeset);
#endif

    string CodeConvert(const string &sSource, const char *to_codeset, const char *from_codeset);

    string ChgCurLocale(const string &sStr);

    string ChgEngKor(const string &sEnglish, const string &sKorean);

    string isKorCode(const string &sSrc, ENCODING *eEncode = NULL);

    string KorCodeChg(const string &sSrc, ENCODING eEncode);
};

#endif //__MLSLOCALE_H__
