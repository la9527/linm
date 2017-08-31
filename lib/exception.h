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

/// @file	exception.h
/// @brief	exception 클래스 헤더 파일
#ifndef    __Exception_h__
#define    __Exception_h__

#include "define.h"
#include "strutil.h"
#include <stdarg.h>

namespace MLSUTIL {
    /// @brief	예외처리를 위한 Exception 클래스
    class Exception {
    public:
        Exception() { _nErrNo = 0; }

        Exception(string pMsg);

        Exception(const char *fmt, ...);

        Exception(int nNum) : _nErrNo(nNum) {}

        Exception(int nErrNo, char *pMsg = NULL);

        Exception(int nErrNo, string pMsg);

        Exception(int nErrNo, const char *fmt, ...);

        char *getInfo(void);

        int getErrNo() { return _nErrNo; }

        /// @brief	char*로 에러문장을 돌려준다.
        operator char *() { return (char *) _strError.c_str(); }

        /// @brief	string 으로 에러문장을 돌려준다.
        operator string() { return _strError; }

        /// @brief	cout을 위한 operator ex) cout << ExceptionClass << endl;
        friend ostream &operator>>(ostream &output, Exception &tEx);

    protected:
        string _strError;        ///<	string형태의 에러내용
        string _sExcepInfo;    ///<	string형태의 에러내용
        int _nErrNo;
    };
}

#endif
