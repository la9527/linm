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

#ifndef __MENU_H__
#define __MENU_H__

#include "define.h"
#include "dialog.h"
#include "keycfgload.h"

namespace MLS {

///	\brief	MenuItem class
    class MenuItem : public Position {
        string _sName;        ///< 화면에 출력될 MenuItem 이름
        string _sCmd;        ///< 실행될 MenuItem 명령어
        string _sCtlKey;

        bool _bEnable;
        bool _bCur;        /// 선택 여부

        ColorEntry _tMenuColor;
        ColorEntry _tMenuAColor;

    public:

        ///	\brief	생성자
        ///	\param	n		MenuItem 이름
        ///	\param	c		MenuItem 명령어
        MenuItem(const string &n,
                 const string &c,
                 bool bEn = true,
                 bool bCur = false)
                : _sName(n), _sCmd(c), _bEnable(bEn), _bCur(bCur) {}

        // setter	===================================
        ///	\brief	이름 설정함수
        ///	\param	n		설정할 화면에 출력될 메뉴 Item
        void SetName(const string &n) { _sName = n; }

        ///	\brief	명령 설정 함수
        ///	\param	n		  설정할 명령어
        void SetCmd(const string &n) { _sCmd = n; }
        // ============================================

        // getter	===================================
        ///	\brief	화면에 출력될 이름 얻기 함수
        ///	\return	화면에 출력될 이름
        const string &GetName() { return _sName; }

        ///	\brief	명령어 얻기 함수
        ///	\return	명령어
        const string &GetCmd() { return _sCmd; }
        // ============================================

        void SetCtlKey(const string &sCtlKey) { _sCtlKey = sCtlKey; }

        const string &GetCtlKey() { return _sCtlKey; }

        ///	\brief	명령 활성화
        ///	\param	bEn		Enable이면 true
        void SetEnable(bool bEn) { _bEnable = bEn; }

        ///	\brief	활성화 여부를 돌려준다.
        ///	\return	Enable
        bool GetEnable() { return _bEnable; }

        void SetCur(bool bCur) { _bCur = bCur; }

        bool GetCur() { return _bCur; }

        void Color(const ColorEntry &tMenuColor, const ColorEntry &tMenuAColor) {
            _tMenuColor = tMenuColor;
            _tMenuAColor = tMenuAColor;
        }

        void Draw();
    };

// 메뉴의 분류
///	\brief	MenuCategory class[메뉴의 분류]
    class MenuCategory : public Form {
        string _sName;        ///< 메뉴 이름

        int _nIndex;            ///< 메뉴 Category 내에서 현재 커서 위치

        vector<MenuItem> _vItem; ///< 서브메뉴
        ColorEntry _tMenuColor;
        ColorEntry _tMenuAColor;
        ViewType _eViewType;

    public:
        ///	\brief	생성자
        ///	\param	p		MenuCategory 이름
        ///	\param	l		MenuCategory location
        MenuCategory(const string &p, const ViewType eViewType)
                : _sName(p), _nIndex(0) {
            _bNoOutRefresh = true; // Refresh 할때 doupdate 하지 않는다.
            _bNoViewUpdate = true; // 업데이트 하지 않는다.
            _bNotDrawBox = true; // 박스를 그리지 않는다.
            _eViewType = eViewType;
        }

        ~MenuCategory() {
        }

        void MenuColor(const ColorEntry &tMenuColor, const ColorEntry &tMenuAColor) {
            _tMenuColor = tMenuColor;
            _tMenuAColor = tMenuAColor;
        }

        ///	\brief	이름  얻기 함수
        ///	\return	이름
        const string &GetName() const { return _sName; }

        ///	\brief	MenuItem 추가 함수
        ///	\param	n		MenuItem 이름
        ///	\param	c		MenuItem Command
        ///	\param	k		MenuItem KeyInfo
        ///	\param	b		MenuItem Enable 여부 (기본 Enable)
        void AddItem(const string &n,
                     const string &c,
                     bool b = true);

        ///	\brief	MenuItem Command 얻기함수
        ///	\return	MenuItem Command
        string GetCmd() { return _vItem[_nIndex].GetCmd(); }

        ///	\brief	MenuItem Command 얻기함수
        ///	\return	MenuItem Command
        bool GetEnable() { return _vItem[_nIndex].GetEnable(); }

        int GetItemSize() { return _vItem.size(); }

        void SetDisable(const std::string &sStr);

        void SetCursor(const int nIdx);

        void SetTypeName(const ViewType e) { _eViewType = e; }

        void DrawFirst();

        void Draw();

        void CursorUp();

        void CursorDown();

        bool MouseEvent(int Y, int X, mmask_t bstate);
    };

///	\brief	Menu class
    class Menu : public Form {
    protected:
        int _nNum;                                    ///< 현재 선택되어 있는 item index
        bool _bESC_Exit;        ///< ESC Exit
        std::vector<MenuCategory> _vItem;        ///< 메뉴 뭉치들(예.윈도우에서 File 메뉴)
        std::vector<std::string> _vCmdDisable;    /// Disable될 Cmd list
        ColorEntry _tColorMenu;
        ColorEntry _tColorMenuA;

        bool MouseEvent(int Y, int X, mmask_t bstate);

        // _tColorMenu, _tColorMenuA 의 컬러를 세팅한다.
        virtual void MenuColorSetting() = 0;

        // Reflash 할때 기타 화면을 Reflesh 꼭 넣어야 제대로 화면 나옴.
        virtual void MenuRefresh() {}

        // 각 메뉴를 넣는 가상함수
        virtual void MenuAddCatecory() = 0;

    public:
        Menu();

        void AddItem(MenuCategory &p);

        string GetCurCmd();

        ///	\brief	Disable 될 명령 list 를 추가한다.
        ///	\param	vCmd	disable 할 메뉴 List (command)
        void SetDefault(void) { _vCmdDisable.clear(); }

        void SetDisable(std::vector<std::string> vCmd) { _vCmdDisable = vCmd; }

        void Create();

        void DrawFirst();

        void Draw();

        void Refresh(bool bNoOutRefresh = false);

        void Execute(KeyInfo &tKeyInfo);
    };

};

#endif
