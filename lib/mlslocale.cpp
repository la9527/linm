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

/// @file	mlslocale.cpp
///	@brief	mlslocale.cpp

#include <cctype>
#include "mlslocale.h"

namespace MLSUTIL {
    ENCODING e_nCurLang = AUTO;        ///< langeuage seting

#ifndef ICONV_CONST
#define ICONV_CONST
#endif

    /// @brief	Locale을 설정하는 함수
    ///
    /// locale에 따라 그래픽 문자들을 초기화 한다.
    ///
    /// @param nEncode	LOCALE
    void Set_Locale(ENCODING nEncode) {
#ifdef HAVE_GETTEXT
        if (nEncode == AUTO)
        {
            string sLang;

            if ( getenv("LANG") )
            {
                sLang = getenv("LANG");
                sLang = Tolower(sLang);
            }

            setlocale(LC_ALL, getenv("LANG"));
            setlocale(LC_CTYPE, getenv("LANG"));
            bindtextdomain(PACKAGE, LOCALEDIR);
            textdomain(PACKAGE);

            if  ( 	sLang.substr(0, 12)  == "ko_kr.euc-kr" ||
                    sLang.substr(0, 11)  == "ko_kr.euckr" ||
                    sLang == "ko_kr" )
            {
                bind_textdomain_codeset(PACKAGE, "EUC-KR");
                e_nCurLang = KO_EUCKR;
            }
            else if (	sLang.substr(0, 11) == "ko_kr.utf-8" || sLang.substr(0, 10) == "ko_kr.utf8" )
            {
                e_nCurLang = KO_UTF8;
                bind_textdomain_codeset(PACKAGE, "UTF-8");
            }
            else
                e_nCurLang = US;
        }
        else
        {
            e_nCurLang = nEncode;

            if (nEncode == KO_EUCKR)
            {
                if ( setlocale(LC_MESSAGES, "ko_KR.euckr") )
                {
                    setlocale(LC_CTYPE, "ko_KR.euckr");
                }
                else
                {
                    setlocale(LC_MESSAGES, "ko_KR.EUC-KR");
                    setlocale(LC_CTYPE, "ko_KR.EUC-KR");
                }

                bindtextdomain(PACKAGE, LOCALEDIR);
                bind_textdomain_codeset(PACKAGE, "EUC-KR");
                textdomain(PACKAGE);
            }
            else if (nEncode == KO_UTF8)
            {
                setlocale(LC_MESSAGES, "ko_KR.utf8");
                setlocale(LC_CTYPE, "ko_KR.utf8");
                bindtextdomain(PACKAGE, LOCALEDIR);
                bind_textdomain_codeset(PACKAGE, "UTF-8");
                textdomain(PACKAGE);
            }
            else
            {
                setlocale(LC_MESSAGES, "C");
                setlocale(LC_CTYPE, getenv("LANG"));
                bindtextdomain(PACKAGE, LOCALEDIR);
                bind_textdomain_codeset(PACKAGE, "C");
            }
        }
#endif
    }

    /// @brief	CodeConvert 함수 : utf-8 -> euckr로 바꿀때 유용 반대 가능 c함수
    ///
    /// code 형식을 바꿀 경우 사용한다. 이것을 끝낼때는 리턴된 char*를 free 해야 한다.
    ///
    /// @param str			source
    /// @param to_codeset	원시 codeset
    /// @param from_codeset	바꿀 coddset
    /// @return	바뀐 char*
#ifdef HAVE_ICONV
    char* code_convert(const char *str,
                            const char *to_codeset,
                            const char *from_codeset)
    {
        iconv_t cd;
        char *dest;
        char *outp;
        char *p, *startp;
        size_t inbytes_remaining;
        size_t outbytes_remaining;
        size_t err;
        size_t outbuf_size;
        int have_error = 0;
        int len;

        if (strcmp (to_codeset, from_codeset) == 0)
        {
            char *p;
            p = (char*)malloc (sizeof(char) * (strlen (str) + 1));
            if (!p)
                return NULL;
                strcpy (p, str);
            return p;
        }

        cd = iconv_open (to_codeset, from_codeset);

        if (cd == (iconv_t) - 1)
            return NULL;

        p = (char*)malloc (sizeof(char) * (strlen (str) + 1));
        strcpy (p, str);

        if (p == NULL) return NULL;

        len = strlen (p);
        startp = p;
        inbytes_remaining = len;
        outbuf_size = len + 1;        /* + 1 for nul in case len == 1 */

        outbytes_remaining = outbuf_size - 1; /* -1 for nul */
        outp = dest = (char*)malloc (outbuf_size);

        again:

        err = iconv (cd, (ICONV_CONST char **) &p, &inbytes_remaining,	&outp, &outbytes_remaining);

        if (err == (size_t) - 1)
        {
            switch (errno)
            {
                case EINVAL:
                    /* Incomplete text, do not report an error */
                    LOG_WRITE("code_convert error EINVAL");
                    have_error = 1;
                    break;

                    case E2BIG:
                    {
                        size_t used = outp - dest;
                        outbuf_size *= 2;
                        dest = (char*)realloc (dest, outbuf_size);

                        outp = dest + used;
                        outbytes_remaining = outbuf_size - used - 1;        /* -1 for nul */

                        goto again;
                    }
                    break;

                case EILSEQ:
                    have_error = 1;
                    LOG_WRITE("code_convert error EILSEQ");
                    break;

                default:
                    LOG_WRITE("code_convert error default");
                    have_error = 1;
                    break;
            }
        }

        *outp = '\0';

        if ((p - startp) != len)
            have_error = 1;

        free (startp);

        iconv_close (cd);

        if (have_error)
        {
            free (dest);
            dest = NULL;
        }

        return dest;
    }
#endif

    /// @brief	CodeConvert 함수 : 예를 들어 utf-8 -> euckr로 바꿀때 유용 C++ 함수
    ///
    /// code 형식을 바꿀 경우 사용한다.
    /// 다른 이유가 없으면 이것을 이용해서 사용한다.
    ///
    /// @param sSource		source
    /// @param to_codeset	원시 codeset
    /// @param from_codeset	바꿀 coddset
    /// @return	바뀐 string 데이터
    string CodeConvert(const string &sSource, const char *to_codeset, const char *from_codeset) {
        char *cStr = NULL;
        string sCodeConvertStr;

#ifdef HAVE_ICONV
        cStr = code_convert((char*)sSource.c_str(), to_codeset, from_codeset);
#endif

        if (cStr != NULL) {
            sCodeConvertStr = cStr;
            free(cStr);
        }
        return sCodeConvertStr;
    }

    /// @brief	영문과 한글을 자동으로 보여주기 위한 함수.
    /// @param	sEnglish	영문
    /// @param	sKorean		영문에 대응한 한글 내용
    /// @return	언어 설정에 따라 영어 아니면 한글 메시지를 반환한다.
    string ChgEngKor(const string &sEnglish, const string &sKorean) {
#ifndef HAVE_ICONV
        return sEnglish;
#endif

        if (e_nCurLang == KO_EUCKR)
            return CodeConvert(sKorean, "EUC-KR", "UTF-8");
        else if (e_nCurLang == KO_UTF8)
            return sKorean;
        else
            return sEnglish;
    }

    /// @brief	current locale change.
    /// @param	sSrc	source string (utf8)
    /// @return	 change string
    string ChgCurLocale(const string &sStr) {
#ifndef HAVE_ICONV
        return sStr;
#endif

#ifndef    __CYGWIN_C__
        bool bUS = true;
        wstring wstr = strtowstr(sStr);
        if (wstr.size() != 0) {
            for (int n = 0; n < (int) wstr.size(); n++) {
                if (wstr[n] > 0x80) {
                    bUS = false;
                    break;
                }
            }
            if (bUS) return sStr;
        }
#endif

        return KorCodeChg(sStr, e_nCurLang);
    }

    /// @brief	현재 사용하는 코드로 바꿔준다. US 면 utf-8로 바꾼다.
    /// @param	sSrc	문자열
    /// @param	eEncode	리턴 현재 인코드
    /// @return	 바꾼 문자열
    string isKorCode(const string &sSrc, ENCODING *eEncode) {
        string sCodeConvertStr;
        string strData1, strData2;
        if (eEncode != NULL) *eEncode = US;
        if (sSrc == "") return "";
        bool bUS = true;

#ifndef    __CYGWIN_C__

        wstring wstr = strtowstr(sSrc);
        if (wstr.size() != 0) {
            for (int n = 0; n < (int) wstr.size(); n++) {
                if (wstr[n] > 0x80) {
                    bUS = false;
                    break;
                }
            }
            if (bUS == true) {
                if (eEncode != NULL) *eEncode = US;
                return sSrc;
            }
        }

#else

        unsigned char cData;

        if (sSrc.size() != 0)
        {
            for (int n = 0; n < (int)sSrc.size(); n++)
            {
                cData = sSrc[n];
                if (cData > 0x0080) {
                    bUS = false;
                    break;
                }
            }
            if (bUS == true) {
                if (eEncode != NULL) *eEncode = US;
                return sSrc;
            }
        }
#endif

        if (e_nCurLang == KO_EUCKR) {
#ifndef HAVE_ICONV
            sCodeConvertStr = sSrc;
            if (eEncode != NULL) *eEncode = KO_EUCKR;
            return sCodeConvertStr;
#endif

            // EUCKR 로 본다.
            strData1 = CodeConvert(sSrc, "UTF-8", "EUC-KR");            // UTF8 로 바꿔봄.

            if (strData1 == "") {
                // 현재 : UTF8
                strData1 = CodeConvert(sSrc, "EUC-KR", "UTF-8");        // EUCKR 로 바꿔봄.
                strData2 = CodeConvert(strData1, "UTF-8", "EUC-KR");    // UTF8 로 바꿔봄.

                if (strData1 != "" && sSrc == strData2) {
                    if (eEncode != NULL) *eEncode = KO_UTF8;
                    return strData1;
                }
            } else {
                strData2 = CodeConvert(strData1, "EUC-KR", "UTF-8");        // EUCKR 로 바꿔봄.
                if (sSrc == strData2) {
                    if (eEncode != NULL) *eEncode = KO_EUCKR;
                    return sSrc; // UTF8
                }
            }
        } else // 나머지는 utf-8로 바꿈.
        {
#ifndef HAVE_ICONV
            sCodeConvertStr = sSrc;
            if (eEncode != NULL) *eEncode = KO_UTF8;
            return sCodeConvertStr;
#endif

            // 현재 : UTF8
            strData1 = CodeConvert(sSrc, "EUC-KR", "UTF-8");            // EUCKR 로 바꿔봄.

            if (strData1 == "") {
                // EUCKR 로 본다.
                strData1 = CodeConvert(sSrc, "UTF-8", "EUC-KR");        // UTF8 로 바꿔봄.
                strData2 = CodeConvert(strData1, "EUC-KR", "UTF-8");    // EUCKR 로 바꿔봄.

                if (strData1 != "" && sSrc == strData2) {
                    if (eEncode != NULL) *eEncode = KO_EUCKR;
                    return strData1;
                }
            } else {
                strData2 = CodeConvert(strData1, "UTF-8", "EUC-KR");        // UTF8 로 바꿔봄.

                if (sSrc == strData2) {
                    if (eEncode != NULL) *eEncode = KO_UTF8;
                    return sSrc; // UTF8
                }
            }
        }
        return sSrc;
    }

    string KorCodeChg(const string &sSrc, ENCODING eEncode) {
#ifndef HAVE_ICONV
        return sSrc;
#endif

        ENCODING tSrcEncode;
        string sChgCode = isKorCode(sSrc, &tSrcEncode);

        if (tSrcEncode == US) return sSrc;

        if (eEncode == KO_EUCKR) {
            if (tSrcEncode == KO_UTF8 && e_nCurLang == KO_UTF8) {
                return CodeConvert(sChgCode, "EUC-KR", "UTF-8");
            }
            if (tSrcEncode == KO_EUCKR) return sSrc;
            return sChgCode;
        } else if (eEncode == KO_UTF8) {
            if (tSrcEncode == KO_EUCKR && e_nCurLang == KO_EUCKR)
                return CodeConvert(sChgCode, "UTF-8", "EUC-KR");
            if (tSrcEncode == KO_UTF8) return sSrc;
            return sChgCode;
        }
        return sSrc;
    }
};

