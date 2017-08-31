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

#ifndef __FILESORT_H__

#include "define.h"
#include "file.h"
#include "colorcfgload.h"

namespace MLS {

    template<class sort_comparator>
    class sort_ascend {
        bool _bSortAscend;

    public:
        /// @brief	sort_ascend 생성자 정의
        ///
        /// sort_comparator로 비교한 결과를 뒤집어 반대로 정렬되게한다.
        ///
        /// @param	sort_comparator	정렬을 위한 비교자
        /// @param	bSortAscend	false면 정렬을 뒤집음
        sort_ascend(bool bSortAscend = true) {
            _bSortAscend = bSortAscend;
        }

        /// @brief	operator () 정의
        ///
        /// 두개의 filename을 sort_comparator로 비교한다
        ///
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            sort_comparator comparator;
            return _bSortAscend == comparator(a, b);
        }
    };

    template<class DirSort, class FileSort>
    class sort_dir_adaptor {
        sort_ascend<DirSort> _ds;
        sort_ascend<FileSort> _fs;

    public:
        sort_dir_adaptor(bool bDirSortAscend = true, bool bFileSortAscend = true)
                : _ds(bDirSortAscend), _fs(bFileSortAscend) {
        }

        bool operator()(File *a, File *b) {
            // dir 이면 제일 앞으로...
            // .으로 시작하는 디렉토리는 항상 앞에..
            // ".."이 제일 앞에 오도록..

            if (a->bDir && b->bDir) {
                if (a->sName == "..") return true;
                if (b->sName == "..") return false;

                return _ds(a, b);
            } else if (a->bDir) return true;
            else if (b->bDir) return false;
            else {
                return _fs(a, b);
            }

        }
    };

///	@brief	file color를 비교하는 class
    class sort_color {
    public:
        /// @brief	operator () 정의
        ///
        /// file color로 sort시 필요한 함수로써 두개의 file color을 비교한다
        ///
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            if (a->tColor == b->tColor) return a->sName < b->sName;

            if (a->tColor == g_tColorCfg._DefaultColor) return false;
            if (b->tColor == g_tColorCfg._DefaultColor) return true;

            return a->tColor < b->tColor;
        }

    };

/// @brief	file name으로 비교하는 class
    class sort_name {
    public:

        /// @brief	operator () 정의
        ///
        /// filename으로 sort시 필요한 함수로써 두개의 filename을 비교한다
        ///
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            return a->sName < b->sName;
        }
    };


///	@brief	파일 확장자로 파일을 비교하는 class
    class sort_ext {
    public:
        /// @brief	operator () 정의
        ///
        /// 확장자 sort시 필요한 함수로써 두개의 파일확장자를 비교한다
        ///
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            if (a->Ext() == b->Ext()) return a->sName < b->sName;
            return a->Ext() < b->Ext();
        }
    };

///	@brief	file size로 비교하는  class
    class sort_size {
    public:
        /// @brief	operator () 정의
        ///
        /// file size로 sort시 필요한 함수로써 두개의 file size를 비교한다
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            if (a->uSize == b->uSize) return a->sName < b->sName;
            return a->uSize < b->uSize;
        }
    };

///	@brief	파일을 최근 수정 시간을 비교하는 class
    class sort_time {
    public:
        /// @brief	operator () 정의
        ///
        /// file mtime(time of last modification)으로 sort시 필요한 함수로써 두개의
        /// file mtime를 비교한다
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            if (a->tCreateTime == b->tCreateTime) return a->sName < b->sName;
            return a->tCreateTime < b->tCreateTime;
        }
    };

    class sort_fullname_length {
    public:
        /// @brief	operator () 정의
        ///
        /// file mtime(time of last modification)으로 sort시 필요한 함수로써 두개의
        /// file mtime를 비교한다
        /// @param	*a	첫번째 파일
        /// @param	*b	두번째 파일
        /// @return	true/false
        bool operator()(File *a, File *b) {
            return a->sFullName.length() > b->sFullName.length();
        }
    };

};

#endif
