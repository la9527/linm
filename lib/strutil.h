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

///	@file	strutil.h
///	@brief	strutil.h
#ifndef STRUTIL_H__
#define STRUTIL_H__

#include "define.h"

///	@brief	string utility
namespace MLSUTIL
{
	string Tolower(const string &p);
	string Toupper(const string &p);
	string chop(const string &p);
	string getbetween(const string &p, const char f, const char r);
	string Replace(const string &p, const char*find, const char*replace);
	string toregular(ullong		lNumber);
	string addslash(const string &p);
	string strmid(int nLength, const string &sStr);

	string	scrstrncpy(	const string &sSrc, int nPoint, int nLength);
	int		scrstrlen(const string &sStr);
	wstring strtowstr(const string& s);
	string	wstrtostr(const wstring& s);
	int		wstrlen(const wstring& s);
	int    strtoint(const string& str);
	long   strtolong(const string& str);
	
	string itoa(int number);
	string ltoa(long number);

	class	String
	{
		protected:
			string	m_str;
			
		public:
			String() {}
			String(const String& str) { m_str = (char*)str; }
			String(const string& str) { m_str = str; }
			String(const char* fmt, ...);

			void	SetEmpty()	{ m_str = ""; }
			void	clear() 	{ m_str = ""; }
			int		size() 		{ return m_str.size(); }

			void	PrintfV(const char* fmt, va_list argList );
			void	Printf(const char* fmt, ...);
			void	Append(const char* fmt, ...);
			
			void	Append(int /*n*/, const char nChar) 	{ m_str.append(1, nChar); }
			void	Append(const string& 	str)		{ m_str.append(str); }
			void	Append(String 	str)				{ m_str.append(str.c_str()); }
			
			void	AppendBlank(const string& str, int nSize);
			void	AppendBlank(const String& str, int nSize)	{ AppendBlank(str.Get(), nSize); }
			void	AppendBlank(int nSize, const char* fmt, ...);

			string	Read(int nPos, int nSize) const	{
				string	tString;
				if (nPos <= (int)m_str.size())
					tString =  m_str.substr(nPos, nSize);
				return tString;
			}

			void	Write(int nPos, const string& str,int nSize)
			{
				if (nPos <= (int)m_str.size())
				{
					for (int n = 0; n < nSize;n++)
					{
						if (n+nPos >= (int)m_str.size() || n >= (int)str.size()) break;
						m_str[n+nPos] = str[n];
					}
				}
			}
	
			void	Write(int nPos, const String& str, int nSize)
			{
				if (nPos <= (int)m_str.size())
				{
					string sTmp = str.Get();
					for (int n = 0; n < nSize;n++)
					{
						if (n+nPos >= (int)m_str.size() || n >= (int)sTmp.size()) break;
						m_str[n+nPos] = sTmp[n];
					}
				}
			}
	
			const string&		Get() const	{ return m_str; }
			const char*	c_str() const { return m_str.c_str(); }

			/// @brief	(char*)
			operator	char*()	const		{ return (char*)m_str.c_str();	}
			/// @brief	(const char*)
			operator	const char*() const	{ return m_str.c_str();	}
			/// @brief	(const string)
			operator	string() const 	{ return m_str; }
			operator	string() 		{ return m_str; }
		
			///	\brief	[] operator redefine.
			///	\param	t	index
			///	\return command name
			char operator [](unsigned int t)
			{
				if (m_str.size() > t) return m_str[t];
				return 0;
			}
	
			/// @brief	operator ex) Text3 = Text + Text2
			String&	operator= (char* tData) 		{	clear(); Append(tData); return *this;}
			String&	operator= (const char* tData) 	{	clear(); Append(tData); return *this; }
			String&	operator= (string tString) 		{	clear(); Append(tString.c_str()); return *this; }
			String&	operator= (String tString) 		{	clear(); Append(tString); return *this; }
			String&	operator= (const char nChar)	{   clear(); m_str.append(1, nChar); return *this; }
	
			/// @brief	= 형태의 operator ex) Text3 = Text + Text2
			String	operator+ (const char* tData) {
				String	tString; tString.Append(tData); return tString;
			}
			String	operator+ (char* tData)	{
				String	tString; tString.Append(tData); return tString;
			}
			String	operator+ (string  tString2) {
				String	tString; tString.Append(tString2); return tString;
			}
			String	operator+ (String&  tString2) {
				String	tString; tString.Append(tString2); return tString;
			}

			friend ostream& operator>>(ostream& output, String& tString)
			{
				output << tString.c_str();
				return output;
			}
	};

	///	\brief	string 분쇄기
	class StringToken
	{
	protected:
		std::string _str;				///< 분쇄할 문자열
		std::string _delim;				///< 기준 문자. 예(';')
		std::string _token;				///< 분쇄된 문자열
		std::string::size_type _cur;	///< 문자열 중 위치 index
	
	public:
		StringToken();
		StringToken(const string &p, const string &d = "\r\n ");
		~StringToken();
	
		///	\brief	분쇄할 문자열을 설정한다.
		///	\param	p	분쇄할 문자열
		void SetString(const string &p) {_str = p; _cur = 0; }
		void SetString(String &p)		{_str = p.c_str(); _cur = 0; }
	
		///	\brief	기준문자를 설정한다.
		///	\param	p	기준문자
		void SetDelim (const string &p) {_delim = p;}
		void SetDelim (String &p) 		{_delim = p.c_str();}
	
		bool Next();
	
		///	\brief	분쇄된 하나의 문자열을 얻는다.
		///	\return	분쇄된 하나의 문자열
		const string &Get() {return _token;}
	
		///	\brief	문자열중 분쇄할 필요가 문자열을 구함
		///	\return	문자열중 분쇄할 필요가 문자열
		const string Rest() {return _str.substr(_cur);}
	};
}

#endif//STRUTIL_H__
