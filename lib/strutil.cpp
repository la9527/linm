/******************************************************************************
 *   Copyright (C) 2005 by Byoungyoung, La                                    *
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

#include <ctype.h>
#include "strutil.h"
#include "exception.h"

namespace	MLSUTIL
{
	/// @brief	p문자열에서 위치 f, r 사이의 문자열을 반환한다.
	string getbetween(
		const string &p,			///< 문자열
		const char f,				///< 반환할 문자의 시작 위치
		const char r				///< 반환할 문자의 끝위치
		)
	{
		string::size_type s = p.find_first_of(f);
		if (s == string::npos) return "";
		string::size_type s2 = p.find_first_of(r, s+1);
		if (s2 == string::npos) return "";	
		
		return p.substr(s+1 , s2-s-1);	
	}

	///	@brief	해당되는 글자를 다른 글자로 바꾼다.
	///	@return	바꾸어진 글
	string Replace(
		const string &p,		///< 글자를 바꿀 대상 전체
		const char*find,		///< 바꿀 대상 문자
		const char*replace		///< 바꿀 문자
		)
	{
		//  01234567890123456789012
		// "the great great bassist" "great" "brother"	
		string arg;
		
		string::size_type t = 0;
		string::size_type t2= p.find(find);
		
		string::size_type len = strlen(find);
		
		while(t2 != string::npos)
		{
			arg += p.substr(t, t2-t);
			arg += replace;
			
			t = t2 + len;
			t2 = p.find(find, t);
		}
		
		arg += p.substr(t);
		
		return arg;
	}

	///	@brief 스트링을 대문자나 소문자로 바꿔준다.
	/// @param str     문자열
	/// @param bUpper  true 면 대문자
	///	@return	바꾸어진 글
	string	to_lowerupper(const string& str, bool bUpper)
	{
		int			nChar;
		string 		sRt = "";
		
		for (int n = 0; n < (int)str.size(); n++)
		{
			nChar = (int)str[n];
			if (bUpper)
				nChar = toupper (nChar);
			else
				nChar = tolower (nChar);
			sRt.append(1, (char)nChar);
		}
		return sRt;
	}

	///	@brief 스트링을 대문자로 바꿔준다.
	string	Toupper(const string& str)
	{
		return to_lowerupper(str, true);
	}
	///	@brief 스트링을 소문자로 바꿔준다.
	string	Tolower(const string& str)
	{
		return to_lowerupper(str, false);
	}

	///	@brief	숫자를 문자로 바꾸어준다.
	///	@return	문자화 된 숫자
	string 	toregular(
		ullong number		///< number	문자로 바꿀 숫자
		)
	{
		ullong 		n=1, p=1;
		string 		str;	
		str.reserve(10);
				
		while(number/10 >= p) {p*=10; n+=1;}
		
		while (n)
		{
			str += '0' + number / p;
			
			number %= p;
			p /= 10;
			n--;
			
			if (n!=0 && n%3==0) str += ',';	
		}
		
		return str;
	}

	/// @brief	필요 없는 문자를 제거한다.
	///	@return	공백, 엔터등을 제거한 문자열 알맹이
	string chop(
		const string &p			///< 문자열 
		)
	{
		string::size_type f = p.find_first_not_of(" \n\r");
		string::size_type r = p.find_last_not_of(" \n\r");
		
		if (f == string::npos || r== string::npos) return "";
		
		return p.substr(f, r-f+1);
	}

	/// @brief	string을 int로.
	/// @param	str  스트링
	/// @return int로
	int	strtoint(const string& str)
	{
		string	Str = str;
		if (Str.size() == 0) return 0;
		for (int n=0; n< (int)Str.size(); n++)
		{
			if (n == 0 && Str[0] == '-') continue;
			if (Str[n] == ' ') continue;
			if (isdigit((int)Str[n]) == 0)
				throw Exception("str is not digit [%s]", Str.c_str());
		}

		int nNum = atoi(Str.c_str());
		return nNum;
	}

	/// @brief	string을 long으로.
	/// @param	str  스트링
	/// @return long 으로
	long	strtolong(const string& str)
	{
		string	Str = str;
		if (Str.size() == 0) return 0;
		for (int n=0; n< (int)Str.size(); n++)
		{
			if (n == 0 && Str[0] == '-') continue;
			if (Str[n] == ' ') continue;
			if (isdigit((int)Str[n]) == 0)
				throw Exception("str is not digit [%s]", Str.c_str());
		}

		long lNum = atol(Str.c_str());
		return lNum;
	}

	/// @brief	string에 slash를 추가하는 함수
	///
	/// 파일이름중에서 특수문자가(예 공백이나 () & *등..을 '\ ' '\ ('로 바꾸어
	/// 주는함수
	///
	/// @return 	shlash가 추가된 글.
	string addslash(
		const string &p			///< 문자열.
		)
	{
		//  t
		// 01234
		// jones
		string arg;
		
		string::size_type t  = 0,
						t2 = p.find_first_of(" ()&*?$'\"\\\0");
		
		while(t2 != string::npos)
		{
			arg.append(p, t, t2-t);
			arg += '\\';
			arg += p[t2];		
			t = t2 + 1;
			t2 = p.find_first_of(" ()&*?$'\"\\\0", t);
		}	
		arg.append(p, t, p.length()-t);
		
		return arg;			
	}

	string itoa(int number)
	{
		char buf[20];
		snprintf(buf, 20, "%d", number);
		return buf;
	}

	string ltoa(long number)
	{
		char buf[20];
		snprintf(buf, sizeof(buf), "%ld",  number);
		return buf;
	}

	struct interval {
		int first;
		int last;
	};

	#ifndef __CYGWIN_C__
	/* auxiliary function for binary search in interval table */
	static int bisearch(wchar_t ucs, const struct interval *table, int max)
	{
		int min = 0;
		int mid;
	
		if (ucs < table[0].first || ucs > table[max].last)
			return 0;
		while (max >= min) {
			mid = (min + max) / 2;
			if (ucs > table[mid].last)
			min = mid + 1;
			else if (ucs < table[mid].first)
			max = mid - 1;
			else
			return 1;
		}
	
		return 0;
	}

	int mk_wcwidth(wchar_t ucs)
	{
		/* sorted list of non-overlapping intervals of non-spacing characters */
		/* generated by "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c" */
		static const struct interval combining[] = {
			{ 0x0300, 0x0357 }, { 0x035D, 0x036F }, { 0x0483, 0x0486 },
			{ 0x0488, 0x0489 }, { 0x0591, 0x05A1 }, { 0x05A3, 0x05B9 },
			{ 0x05BB, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
			{ 0x05C4, 0x05C4 }, { 0x0600, 0x0603 }, { 0x0610, 0x0615 },
			{ 0x064B, 0x0658 }, { 0x0670, 0x0670 }, { 0x06D6, 0x06E4 },
			{ 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED }, { 0x070F, 0x070F },
			{ 0x0711, 0x0711 }, { 0x0730, 0x074A }, { 0x07A6, 0x07B0 },
			{ 0x0901, 0x0902 }, { 0x093C, 0x093C }, { 0x0941, 0x0948 },
			{ 0x094D, 0x094D }, { 0x0951, 0x0954 }, { 0x0962, 0x0963 },
			{ 0x0981, 0x0981 }, { 0x09BC, 0x09BC }, { 0x09C1, 0x09C4 },
			{ 0x09CD, 0x09CD }, { 0x09E2, 0x09E3 }, { 0x0A01, 0x0A02 },
			{ 0x0A3C, 0x0A3C }, { 0x0A41, 0x0A42 }, { 0x0A47, 0x0A48 },
			{ 0x0A4B, 0x0A4D }, { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A82 },
			{ 0x0ABC, 0x0ABC }, { 0x0AC1, 0x0AC5 }, { 0x0AC7, 0x0AC8 },
			{ 0x0ACD, 0x0ACD }, { 0x0AE2, 0x0AE3 }, { 0x0B01, 0x0B01 },
			{ 0x0B3C, 0x0B3C }, { 0x0B3F, 0x0B3F }, { 0x0B41, 0x0B43 },
			{ 0x0B4D, 0x0B4D }, { 0x0B56, 0x0B56 }, { 0x0B82, 0x0B82 },
			{ 0x0BC0, 0x0BC0 }, { 0x0BCD, 0x0BCD }, { 0x0C3E, 0x0C40 },
			{ 0x0C46, 0x0C48 }, { 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 },
			{ 0x0CBC, 0x0CBC }, { 0x0CBF, 0x0CBF }, { 0x0CC6, 0x0CC6 },
			{ 0x0CCC, 0x0CCD }, { 0x0D41, 0x0D43 }, { 0x0D4D, 0x0D4D },
			{ 0x0DCA, 0x0DCA }, { 0x0DD2, 0x0DD4 }, { 0x0DD6, 0x0DD6 },
			{ 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A }, { 0x0E47, 0x0E4E },
			{ 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 }, { 0x0EBB, 0x0EBC },
			{ 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 }, { 0x0F35, 0x0F35 },
			{ 0x0F37, 0x0F37 }, { 0x0F39, 0x0F39 }, { 0x0F71, 0x0F7E },
			{ 0x0F80, 0x0F84 }, { 0x0F86, 0x0F87 }, { 0x0F90, 0x0F97 },
			{ 0x0F99, 0x0FBC }, { 0x0FC6, 0x0FC6 }, { 0x102D, 0x1030 },
			{ 0x1032, 0x1032 }, { 0x1036, 0x1037 }, { 0x1039, 0x1039 },
			{ 0x1058, 0x1059 }, { 0x1160, 0x11FF }, { 0x1712, 0x1714 },
			{ 0x1732, 0x1734 }, { 0x1752, 0x1753 }, { 0x1772, 0x1773 },
			{ 0x17B4, 0x17B5 }, { 0x17B7, 0x17BD }, { 0x17C6, 0x17C6 },
			{ 0x17C9, 0x17D3 }, { 0x17DD, 0x17DD }, { 0x180B, 0x180D },
			{ 0x18A9, 0x18A9 }, { 0x1920, 0x1922 }, { 0x1927, 0x1928 },
			{ 0x1932, 0x1932 }, { 0x1939, 0x193B }, { 0x200B, 0x200F },
			{ 0x202A, 0x202E }, { 0x2060, 0x2063 }, { 0x206A, 0x206F },
			{ 0x20D0, 0x20EA }, { 0x302A, 0x302F }, { 0x3099, 0x309A },
			{ 0xFB1E, 0xFB1E }, { 0xFE00, 0xFE0F }, { 0xFE20, 0xFE23 },
			{ 0xFEFF, 0xFEFF }, { 0xFFF9, 0xFFFB }, { 0x1D167, 0x1D169 },
			{ 0x1D173, 0x1D182 }, { 0x1D185, 0x1D18B }, { 0x1D1AA, 0x1D1AD },
			{ 0xE0001, 0xE0001 }, { 0xE0020, 0xE007F }, { 0xE0100, 0xE01EF }
		};
		
		/* test for 8-bit control characters */
		if (ucs == 0)
			return 0;
		if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0))
			return -1;
		
		/* binary search in table of non-spacing characters */
		if (bisearch(ucs, combining,
				sizeof(combining) / sizeof(struct interval) - 1))
			return 0;
		
		/* if we arrive here, ucs is not a combining or C0/C1 control character */
		
		return 1 +
			(ucs >= 0x1100 &&
			(ucs <= 0x115f ||                    /* Hangul Jamo init. consonants */
			ucs == 0x2329 || ucs == 0x232a ||
			(ucs >= 0x2e80 && ucs <= 0xa4cf &&
			ucs != 0x303f) ||                  /* CJK ... Yi */
			(ucs >= 0xac00 && ucs <= 0xd7a3) || /* Hangul Syllables */
			(ucs >= 0xf900 && ucs <= 0xfaff) || /* CJK Compatibility Ideographs */
			(ucs >= 0xfe30 && ucs <= 0xfe6f) || /* CJK Compatibility Forms */
			(ucs >= 0xff00 && ucs <= 0xff60) || /* Fullwidth Forms */
			(ucs >= 0xffe0 && ucs <= 0xffe6) ||
			(ucs >= 0x20000 && ucs <= 0x2fffd) ||
			(ucs >= 0x30000 && ucs <= 0x3fffd)));
	}
	
	int mk_wcswidth(const wchar_t *pwcs, size_t n)
	{
		int w, width = 0;
		
		for (;*pwcs && n-- > 0; pwcs++)
			if ((w = mk_wcwidth(*pwcs)) < 0)
			return -1;
			else
			width += w;
		
		return width;
	}
	#endif

	/// @brief  char 를 wchar_t 형태로 바꾼다. setlocale(LC_TYPE, locale)을 사용해야 함.
	wstring strtowstr(const string& str)
	{
		#ifdef __CYGWIN_C__
			return str;
		#else
			string 	s = str;
			if (s.size() == 0 && s.size() > 10000000) return L"";
			wstring		wstr;
			
			wchar_t*	w = new wchar_t[s.size()+1];
			memset(w, 0, (s.size()+1)*sizeof(wchar_t));
			
			if (mbstowcs(w, s.c_str(), s.size()) == (size_t)(-1))
			{
				LOG_WRITE("mbstowcs func err [%s] [%d] [%s]",
							s.c_str(), errno, strerror(errno));
				delete []w;
				return L"";
			}
			wstr = w;
			delete []w;
			return wstr;
		#endif
	}
	
	/// @brief  wchar_t를 char 형태로 바꾼다. setlocale(LC_TYPE, locale)을 사용해야 함.
	string	wstrtostr(const wstring& wstr)
	{
		#ifdef __CYGWIN_C__
			return wstr;
		#else
			wstring	s = wstr;
			if (s.size() == 0) return "";
			string  str;
			char	mbchar[10];
				
			for (uint n = 0; n < s.size(); n++)
			{
				memset(&mbchar, 0, sizeof(mbchar));
				if (wctomb((char*)&mbchar, s[n]) == -1)
				{
					LOG_WRITE("wcstombs func err [%d] num[%d] [%d] [%d] [%s]",
						(int)s[n], n, errno, strerror(errno));
				}
				str.append(mbchar);
			}
			return str;
		#endif
	}
	
	/// @brief  화면에 맞게 스트링사이즈를 구한다. setlocale(LC_TYPE, locale)을 사용해야 함.
	int scrstrlen(
		const string& sSrc
		)
	{
		#ifdef __CYGWIN_C__
			return sSrc.size();
		#else
			if (sSrc.size() == 0) return 0;
			
			int		nLength = 0, nWidth = 0;
			string	s = sSrc;
			wstring wc = strtowstr(s);
		
			if (wc.size() == 0) return 0;
		
			for (uint n = 0; n < wc.size(); n++)
			{
				if (wc[n] >= (wchar_t)0x100)
				{
					if ((nWidth = mk_wcwidth(wc[n])) == -1)
					{
						LOG_WRITE("krstrlen :: wcwidth func err [%d] ['%s'::%d] [%d] [%d] [%s]",
							n, sSrc.c_str(), sSrc.size(), errno, strerror(errno));
						nWidth = 0;
						continue;
					}
					nLength = nLength + nWidth;
				}
				else
				{
					nLength = nLength + 1;
				}
			}
			return nLength;
		#endif
	}
	
	/// @brief  화면에 맞는 스트링사이즈를 구한다. setlocale(LC_TYPE, locale)을 사용해야 함.
	int	wstrlen(const wstring& w)
	{
		#ifdef __CYGWIN_C__
			return w.size();
		#else
			int	nWidth = 0, nLength = 0;
			wstring wstr = w;
			if (wstr.size() == 0) return 0;
			
			for (uint n = 0; n < wstr.size(); n++)
			{
				if (wstr[n] >= (wchar_t)0x100)
				{
					if ((nWidth = mk_wcwidth(wstr[n])) == -1) {
						LOG_WRITE("wstrlen err [%d] [%d]", (int)wstr[n], n);
						nWidth = 0;
						continue;
					}
					nLength = nLength + nWidth;
				}
				else
				{
					nLength = nLength + 1;
				}
				
			}
			return nLength;
		#endif
	}
	
	/// @brief  문자 길이에 맞게 자를때 씀. setlocale(LC_TYPE, locale)을 사용해야 함.
	string scrstrncpy(
		const string &sSrc,			///< 한글이나 영문자.
		int nPoint,					///< 위치
		int nLength					///< 문자열 길이.
		)
	{
		#ifdef __CYGWIN_C__
			return sSrc.substr(nPoint, nLength);
		#else
			string	sRt = "";
			if (sSrc.size() == 0) return sRt;
			string	s = sSrc;
			wstring wsrc = strtowstr(s);
		
			if (wsrc.size() == 0) return sRt;
		
			char	mbchar[10];
			int nPos = 0, nLen = 0, nWidth = 0;
		
			for (int n = 0; n < (int)wsrc.size(); n++)
			{
				if (wsrc[n] >= (wchar_t)0x100)
				{
					if ((nWidth = mk_wcwidth(wsrc[n])) == -1)
					{
						LOG_WRITE("krstrncpy wcwidth func err [%d] [%s] [%d] [%d] [%s]",
							n, sSrc.c_str(), errno, strerror(errno));
						nWidth = 1;
						continue;
					}
				}
				else
				{
					nWidth = 1;
				}
		
				if (nLength <= nLen) break;
				
				if (nPos >= nPoint)
				{
					memset(&mbchar, 0, sizeof(mbchar));
					if (wctomb((char*)&mbchar, wsrc[n]) == -1)
					{
						LOG_WRITE("krstrncpy wcstombs func err [%d] num[%d] [%d] [%d] [%s]",
							(int)wsrc[n], n, errno, strerror(errno));
					}
					else
					{
						sRt.append(mbchar);
					}
					nLen = nLen + nWidth;
				}
		
				nPos = nPos + nWidth;
			}
			return sRt;
		#endif
	}
	
	/// @brief nLength 사이즈에 맞게 문자가 중간에 위치하게 만든다.
	string strmid(int nLength, const string &sSrc)
	{
		string sConvert;
		
		if (nLength == 0) return "";
		if (sSrc.size() == 0) return "";
		int nSrcStrLength = scrstrlen(sSrc);
		if (nSrcStrLength >= nLength) return sSrc;	
		int nPos = (nLength - nSrcStrLength) / 2;
		sConvert.append(nPos, ' ');
		sConvert.append(sSrc);
		sConvert.append(nLength-(nPos+nSrcStrLength), ' ');
		return sConvert;
	}

	String::String(const char* fmt, ...)
	{
		va_list argList;
		va_start( argList, fmt );
		PrintfV( fmt, argList );
		va_end( argList );
	}

	void String::PrintfV(const char* fmt, va_list argList )
	{
		char buf_data[256];
		char* data = buf_data;
		int data_size = sizeof(buf_data);
		int ret = 0;
	
		memset(&buf_data, 0, sizeof(buf_data));
		ret = vsnprintf(data, data_size, fmt, argList);
		++ret;	
	
		if(ret > (int)sizeof(buf_data))
		{
			data = (char*)malloc(ret);
			if (data == NULL)
			{
				cerr << "Buffer over flow" << endl;
				return ;
			}
			memset(data, 0, sizeof(char)*ret);
			data_size = ret;
			vsnprintf(data, data_size, fmt, argList);
		}
	
		m_str = data;
		if(data != buf_data)
		{
			if (data != NULL) free(data);
		}
	}

	void	String::Append(const char* fmt, ...)
	{
		va_list argptr;
		char buf_data[256];
		char* data = buf_data;
		int data_size = sizeof(buf_data);
		int ret = 0;
	
		memset(&buf_data, 0, sizeof(buf_data));
		va_start(argptr, fmt);
		ret = vsnprintf(data, data_size, fmt, argptr);
		++ret;
		va_end(argptr);
	
		if(ret > (int)sizeof(buf_data))
		{
			data = (char*)malloc(ret);
			if (data == NULL)
			{
				cerr << "Buffer over flow" << endl;
				return ;
			}
			memset(data, 0, sizeof(char)*ret);
			data_size = ret;
			va_start(argptr, fmt);
			vsnprintf(data, data_size, fmt, argptr);
			va_end(argptr);
		}
	
		m_str = m_str + data;
		if(data != buf_data)
		{
			if (data != NULL) free(data);
		}
	}

	void	String::Printf(const char* fmt, ...)
	{
		va_list argList;
		va_start( argList, fmt );
		PrintfV( fmt, argList );
		va_end( argList );
	}
	
	void	String::AppendBlank(const string& str, int nSize)
	{
		char cData[nSize+1];
		memset(cData, 0, nSize+1);
		memset(cData, ' ', nSize);
		if ((int)str.size() < nSize)
			strncpy(cData, str.c_str(), str.size());
		else
			strncpy(cData, str.c_str(), nSize);
		m_str.append(cData);
	}
	
	void	String::AppendBlank(int nSize, const char* fmt, ...)
	{
		va_list argptr;
		char buf_data[256];
		char* data = buf_data;
		int data_size = sizeof(buf_data);
		int ret;
	
		memset(&buf_data, 0, sizeof(buf_data));
		va_start(argptr, fmt);
		ret = vsnprintf(data, data_size, fmt, argptr);
		++ret;
		va_end(argptr);
	
		if(ret > (int)sizeof(buf_data))
		{
			data = (char*)malloc(ret);
			if (data == NULL)
			{
				cerr << "Buffer over flow" << endl;
				return ;
			}
			memset(data, 0, sizeof(char)*ret);
			data_size = ret;
			va_start(argptr, fmt);
			vsnprintf(data, data_size, fmt, argptr);
			va_end(argptr);
		}
	
		char cData[nSize+1];
		memset(cData, 0, nSize+1);
		memset(cData, ' ', nSize);
		if ((int)strlen(data) < nSize)
			strncpy(cData, data, strlen(data));
		else
			strncpy(cData, data, nSize);
			
		if(data != buf_data)
		{
			if (data != NULL) free(data);
		}
		m_str = m_str + cData;
	}
	
	///	\brief	생성자
	StringToken::StringToken(): _delim("\r\n "), _cur(0)
	{
	}
	
	///	\brief	생성자
	///	\param	p		분쇄 대상 문자열
	///	\param	d		분쇄할 때 기준이 되는 문자(예. /etc/passwd 를 분쇄할때 ':')
	StringToken::StringToken(const string &p, const string &d)
		: _str(p), _delim(d), _cur(0)
	{
	}
	
	///	\brief	소멸자
	StringToken::~StringToken()
	{
	}
	
	///	\brief	분쇄된 다음 문자열로 이동한다.
	///	\return	성공여부
	bool StringToken::Next()
	{
		// _delim = "a";
		//         0123456789
		// _str = "laneapple";
		//         ss2
	
		// . 범위가 벗어나면 끝냄
		if (_cur == string::npos) return false;
	
		// . 기준문자('a') 위치다음를 찾음
		string::size_type s = _str.find_first_not_of(_delim, _cur);
	
		// . 기준문자('a')가 없으면 끝냄
		if (s == string::npos) return false;
	
		// . 다음 문자열 위치를 구함
		_cur = _str.find_first_of(_delim, s+1);
	
		// . 다음 문자열이 없다면 문자끝까지 분쇄문자열로 정함
		if (_cur == string::npos)
			_token = _str.substr(s);
		// . 다음 문자열이 있다면 다음 문자열 전까지 분쇄함
		else
			_token = _str.substr(s, _cur-s);
		
		return true;	
	}
};
