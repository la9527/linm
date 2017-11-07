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

namespace MLSUTIL
{
	ENCODING	e_nCurLang     = AUTO;		///< language setting
	
#ifndef ICONV_CONST
	#define ICONV_CONST 
#endif
	
	/// @brief	Locale setting function
	///
	/// LinM locale setting.
	///
	/// @param nEncode	LOCALE
	void Set_Locale(ENCODING nEncode)
	{
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
#endif // HAVE_GETTEXT
	}
	
	/// @brief	Code convert function : utf-8 -> euckr function.
	///
	/// Code convert. This return data is need to call free().
	///
	/// @param str			source
	/// @param to_codeset	To codeset
	/// @param from_codeset	From codeset
	/// @return Changed string.
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
		outbuf_size = len + 1;        /* + 1 for null in case len == 1 */
	
		outbytes_remaining = outbuf_size - 1; /* -1 for null */
		outp = dest = (char*)malloc (outbuf_size);
	
		again:
	
		err = iconv (cd, (ICONV_CONST char **) &p, &inbytes_remaining,	&outp, &outbytes_remaining);
	
		if (err == (size_t) - 1)
		{
			switch (errno)
			{
				case EINVAL:
					/* Incomplete text, do not report an error */
					LOG("code_convert error EINVAL");
					have_error = 1;
					break;
	
					case E2BIG:
					{
						size_t used = outp - dest;
						outbuf_size *= 2;
						dest = (char*)realloc (dest, outbuf_size);
	
						outp = dest + used;
						outbytes_remaining = outbuf_size - used - 1;        /* -1 for null */
	
						goto again;
					}
					break;
	
				case EILSEQ:
					have_error = 1;
					LOG("code_convert error EILSEQ");
					break;
	
				default:
					LOG("code_convert error default");
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
	
	/// @brief	CodeConvert : the function is change from utf-8 to euckr. (C++ Function)
	///
	/// If not other special reason, this function must be used/
	///
	/// @param sSource		source
	/// @param to_codeset	orignal codeset
	/// @param from_codeset	change coddset
	/// @return	Chagned the string data.
	string CodeConvert(const string& sSource, const char *to_codeset, const char *from_codeset)
	{
		char*	cStr = NULL;
		string	sCodeConvertStr;
	
		#ifdef HAVE_ICONV
		cStr = code_convert((char*)sSource.c_str(), to_codeset, from_codeset);
		if (cStr != NULL)
		{
			sCodeConvertStr = cStr;
			free(cStr);
		}
        return sCodeConvertStr;
        #else
        return sSource;
        #endif
	}
	
	/// @brief	This function is change from english to korean.
	/// @param	sEnglish	english message
	/// @param	sKorean		korean message. ( symmetric message of english message ) 
	/// @return	Depending on the language setting will return a message.
	string ChgEngKor(const string& sEnglish, const string& sKorean)
	{
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
	string ChgCurLocale(const string& sStr)
	{
		#ifndef HAVE_ICONV
			return sStr;
		#endif

		#ifndef 	__CYGWIN_C__
		bool bUS = true;
		wstring	wstr = strtowstr(sStr);
		if (wstr.size() != 0)
		{
			for (int n = 0; n < (int)wstr.size(); n++)
			{
				if (wstr[n] > 0x80) {
					bUS = false;
					break;
				}
			}
			if ( bUS ) return sStr;
		}
		#endif

		return KorCodeChg( sStr, e_nCurLang );
	}
	
	/// @brief	change to use the current code. if current used code is US, change to utf-8 return.
	/// @param	sSrc	string
	/// @param	eEncode	set current encode.
	/// @return	 change the string
	string	isKorCode(const string& sSrc, ENCODING* eEncode)
	{
		string	sCodeConvertStr;
		string	strData1, strData2;
		if (eEncode != NULL) *eEncode = US;
		if (sSrc == "") return "";
		bool 	bUS = true;
		
		#ifndef 	__CYGWIN_C__
		
		wstring	wstr = strtowstr(sSrc);
		if (wstr.size() != 0)
		{
			for (int n = 0; n < (int)wstr.size(); n++)
			{
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

		if (e_nCurLang == KO_EUCKR)
		{
			#ifndef HAVE_ICONV
				sCodeConvertStr = sSrc;
				if (eEncode != NULL) *eEncode = KO_EUCKR;
				return sCodeConvertStr;
			#endif

			// EUCKR
			strData1 = CodeConvert(sSrc, "UTF-8", "EUC-KR"); 			// change to UTF8.	
	
			if ( strData1 == "" )
			{
				// Change from UTF8 to EUC-KR
				strData1 = CodeConvert(sSrc, "EUC-KR", "UTF-8"); 		// change to EUC-KR.
				strData2 = CodeConvert(strData1, "UTF-8", "EUC-KR");	// change to UTF8.
				
				if ( strData1 != "" && sSrc == strData2 )
				{
					if (eEncode != NULL) *eEncode = KO_UTF8;
					return strData1;
				}
			}
			else
			{
				strData2 = CodeConvert(strData1, "EUC-KR", "UTF-8"); 		// change to EUCKR.	
				if ( sSrc == strData2 )
				{
					if (eEncode != NULL) *eEncode = KO_EUCKR;
					return sSrc; // UTF8
				}
			}	
		}
		else // another things are change to utf-8.
		{
			#ifndef HAVE_ICONV
				sCodeConvertStr = sSrc;
				if (eEncode != NULL) *eEncode = KO_UTF8;
				return sCodeConvertStr;
			#endif
			
			// Change from UTF8 to EUC-KR.
			strData1 = CodeConvert(sSrc, "EUC-KR", "UTF-8"); 			
			
			if ( strData1 == "" )
			{
				// EUCKR
				strData1 = CodeConvert(sSrc, "UTF-8", "EUC-KR"); 		// change to UTF8.
				strData2 = CodeConvert(strData1, "EUC-KR", "UTF-8"); 	// change to EUCKR.
				
				if ( strData1 != "" && sSrc == strData2 )
				{
					if (eEncode != NULL) *eEncode = KO_EUCKR;
					return strData1;
				}
			}
			else
			{	
				strData2 = CodeConvert(strData1, "UTF-8", "EUC-KR"); 		// change to UTF8.
				
				if ( sSrc == strData2 )
				{
					if (eEncode != NULL) *eEncode = KO_UTF8;
					return sSrc; // UTF8
				}
			}	
		}
		return sSrc;
	}

	string	KorCodeChg(const string& sSrc, ENCODING eEncode)
	{
		#ifndef HAVE_ICONV
			return sSrc;
		#endif
		
		ENCODING	tSrcEncode;
		string sChgCode = isKorCode(sSrc, &tSrcEncode);

		if ( tSrcEncode == US ) return sSrc;

		if ( eEncode == KO_EUCKR )
		{
			if (tSrcEncode == KO_UTF8 && e_nCurLang == KO_UTF8 )
			{
				return CodeConvert(sChgCode, "EUC-KR", "UTF-8");
			}
			if (tSrcEncode == KO_EUCKR) return sSrc;
			return sChgCode;
		}
		else if ( eEncode == KO_UTF8 )
		{
			if (tSrcEncode == KO_EUCKR && e_nCurLang == KO_EUCKR )
				return CodeConvert(sChgCode, "UTF-8", "EUC-KR");
			if (tSrcEncode == KO_UTF8) return sSrc;
			return sChgCode;
		}
		return sSrc;
	}
};

