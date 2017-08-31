#include "dialog_menu.h"

using namespace MLS;

void MenuItem::Draw() {
    WINDOW *pWin = _pForm->GetWin();

    if (_sName.empty()) {
        // 빈 문자열은 줄긋기
        setcol(0, _tMenuColor.back, pWin);
        mvwhline(pWin, y, x + 1, HLINE, width - 2);
    } else {
        if (_bCur == false)
            setcol(_tMenuColor, pWin);
        else
            setcol(7, 0, pWin);

        mvwhline(pWin, y, x + 1, ' ', width - 2);
        if (_bEnable == false) setcol(8, _tMenuColor.back, pWin);
        mvwprintw(pWin, y, x + 2, "%s", _sName.c_str());

        if (!_sCtlKey.empty()) {
            // Ctrl, Alt 정보를 넣는다
            if (_bCur == false) {
                if (_bEnable == false)
                    setcol(8, _tMenuColor.back, pWin);
                else
                    setcol(_tMenuAColor, pWin);
            } else
                setcol(7, 0, pWin);
            mvwprintw(pWin, y, x + width - 2 - scrstrlen(_sCtlKey), "%s", _sCtlKey.c_str());
        }
    }
    //wnoutrefresh(pWin);
}

void MenuCategory::AddItem(const string &n,
                           const string &c,
                           bool b) {
    MenuItem tMenuItem(n, c, b);
    LOG_WRITE("Menu AddItem Cmd [%s] Key [%s] [%d]", c.c_str(), g_tKeyCfg.CmdToKeyName(c, _eViewType).c_str(),
              _eViewType);
    tMenuItem.SetCtlKey(g_tKeyCfg.CmdToKeyName(c, _eViewType));
    tMenuItem.x = 0;
    tMenuItem.y = _vItem.size() + 1;
    tMenuItem.height = 1;
    tMenuItem.width = width;
    _vItem.push_back(tMenuItem);
}

void MenuCategory::SetDisable(const std::string &sStr) {
    for (int c = 0; c < (int) _vItem.size(); c++) {
        if (_vItem[c].GetCmd() == sStr) _vItem[c].SetEnable(false);
    }
}

void MenuCategory::SetCursor(const int nIdx) {
    if (nIdx < 0 || (int) _vItem.size() <= nIdx) return;
    if (_vItem[nIdx].GetName().empty()) return;
    _nIndex = nIdx;
}

void MenuCategory::DrawFirst() {
    werase(_pWin);
    wbkgd(_pWin, COLOR(_tMenuColor.font, _tMenuColor.back));

    setcol(15, _tMenuColor.back, _pWin);
    mvwaddch (_pWin, 0, 0, ULCORNER);
    mvwaddch (_pWin, 0, width - 1, URCORNER);
    mvwaddch (_pWin, height - 1, 0, LLCORNER);
    mvwaddch (_pWin, height - 1, width - 1, LRCORNER);
    mvwhline(_pWin, 0, 1, HLINE, width - 2);
    mvwvline(_pWin, 1, 0, VLINE, height - 2);
    mvwhline(_pWin, height - 1, 1, HLINE, width - 2);
    mvwvline(_pWin, 1, width - 1, VLINE, height - 2);
}

void MenuCategory::Draw() {
    for (int n = 0; n < (int) _vItem.size(); n++) {
        MenuItem tMenuItem = _vItem[n];
        tMenuItem.SetForm((Form *) this);
        tMenuItem.Color(_tMenuColor, _tMenuAColor);
        if (_nIndex == n)
            tMenuItem.SetCur(true);
        else
            tMenuItem.SetCur(false);
        tMenuItem.Show();
    }
}

void MenuCategory::CursorUp() {
    if (_vItem.empty()) return;

    _nIndex = (_nIndex + _vItem.size() - 1) % _vItem.size();

    // 빈 스트링은 건너뛴다
    // 메뉴전체가 빈 스트링일 경우 무한 루프 빠질위험 있음
    if (_vItem[_nIndex].GetName().empty()) CursorUp();
    if (_vItem[_nIndex].GetEnable() == false) CursorUp();
}

void MenuCategory::CursorDown() {
    if (_vItem.empty()) return;

    _nIndex = (_nIndex + 1) % _vItem.size();

    // 빈 스트링은 건너뛴다
    // 메뉴전체가 빈 스트링일 경우 무한 루프 빠질위험 있음
    if (_vItem[_nIndex].GetName().empty()) CursorDown();
    if (_vItem[_nIndex].GetEnable() == false) CursorDown();
}

bool MenuCategory::MouseEvent(int Y, int X, mmask_t bstate) {
    int nY = Y - y;
    int nX = X - x;
    for (int n = 0; n < (int) _vItem.size(); n++) {
        if (_vItem[n].AreaChk(nY, nX)) {
            _nIndex = n;
            return true;
        }
    }
    return false;
}

bool Menu::MouseEvent(int Y, int X, mmask_t bstate) {
    if (Y == 0) {
        for (int t = 0; t < (int) _vItem.size(); t++) {
            int width = scrstrlen(_vItem[t].GetName());
            if (_vItem[t].x <= X && X <= _vItem[t].x + width) {
                _nNum = t;
                MenuRefresh();
                return true;
            }
        }
    } else {
        if (_vItem[_nNum].MouseEvent(Y, X, bstate) == false) {
            // 선택이외의 것이 존재하면 ESC와 같은 효
            _bESC_Exit = true;
            _bExit = true;
        } else {
            if (bstate & BUTTON1_DOUBLE_CLICKED) {
                _bExit = true;
            }
        }
    }
    return false;
}

Menu::Menu() {
    _nNum = 0;
    //_bNoOutRefresh = true; // Refresh 할때 doupdate 하지 않는다.
    //_bNoViewUpdate = true; // 업데이트 하지 않는다.
    _bNotDrawBox = true; // 박스를 그리지 않는다.
    _bESC_Exit = false;
    _bExit = false;
    x = 0;
    y = 0;
    height = 1;
    width = COLS - 1;
}

///	\brief	menuCategory를 추가한다.
///	\param	p	추가할 MenuCategory
void Menu::AddItem(MenuCategory &p) {
    // . 아무런 메뉴(예. File 메뉴)가 없으면 기위치(x)를 4로한다.
    if (_vItem.empty())
        p.x = 4;
        // . 메뉴가 있다면 전 메뉴가 위치한 자리와 글자수를 참고하여 위치를 설정
    else {
        p.x = _vItem.back().x + scrstrlen(_vItem.back().GetName()) + 2;
    }

    p.y = 1;
    p.width = 26;
    p.height = p.GetItemSize() + 2;

    p.MenuColor(_tColorMenu, _tColorMenuA);

    // . 메뉴 뭉치 추가
    _vItem.push_back(p);
}

string Menu::GetCurCmd() {
    if (_bESC_Exit == false) { return _vItem[_nNum].GetCmd(); }
    return "";
}

void Menu::Create() {
    _vItem.clear();

    MenuColorSetting();
    MenuAddCatecory();

    // Disable 된 데이터를 바꾼다.
    for (int c = 0; c < (int) _vCmdDisable.size(); c++)
        for (int b = 0; b < (int) _vItem.size(); b++)
            _vItem[b].SetDisable(_vCmdDisable[c]);

    _bESC_Exit = false;
    _bExit = false;
}

void Menu::DrawFirst() {
    x = 0;
    y = 0;
    height = 1;
    width = COLS - 1;

    // 주 메뉴를 그린다.
    // 예 "Mls  Run  File  Directory  Archive  Option" 메뉴
    setcol(_tColorMenu, _pWin);
    wmove(_pWin, y, x);
    whline(_pWin, ' ', width);

    for (int t = 0; t < (int) _vItem.size(); t++) {
        mvwprintw(_pWin, y, _vItem[t].x, " %s ", _vItem[t].GetName().c_str());
        LOG_WRITE("MENU :: %-11s", _vItem[t].GetName().c_str());
    }

    // 주 메뉴에서 첫글자를 노란글자로 쓴다.(구분)
    setcol(_tColorMenuA, _pWin);
    for (int t = 0; t < (int) _vItem.size(); t++) {
        mvwprintw(_pWin, y, _vItem[t].x + 1,
                  (char *) scrstrncpy(_vItem[t].GetName(), 0, 1).c_str());
    }

    // 선택 된 메뉴에 검은색바탕에 글자를 씀
    setcol(7, 0, _pWin);
    mvwprintw(_pWin, y, _vItem[_nNum].x, " %s ", _vItem[_nNum].GetName().c_str());

    // 선택 된 메뉴에 첫번째 글자를 노란색으로 쓴다.
    setcol(_tColorMenuA.font, 0, _pWin);
    mvwprintw(_pWin, y, _vItem[_nNum].x + 1,
              (char *) scrstrncpy(_vItem[_nNum].GetName(), 0, 1).c_str());

    wnoutrefresh(_pWin);
}

void Menu::Draw() {
    if (_vItem[_nNum].GetEnable() == false) _vItem[_nNum].CursorDown();
    _vItem[_nNum].Show();
}

void Menu::Refresh(bool bNoOutRefresh) {
    werase(_pWin);
    _bNoOutRefresh = bNoOutRefresh;
    MenuRefresh();
    Show();
}

#include "mlsdialog.h"

void Menu::Execute(KeyInfo &tKeyInfo) {
    bool bRefesh = false;

    switch ((int) tKeyInfo) {
        case KEY_DOWN: {
            _vItem[_nNum].CursorDown();
            break;
        }
        case KEY_UP: {
            _vItem[_nNum].CursorUp();
            break;
        }
            // .. 왼쪽 화살표를 누르면 왼쪽에 있는 메뉴로 이동한다.
        case KEY_LEFT: {
            _vItem[_nNum].Close(); // 전에 사용한 윈도우는 Close 한다.
            _nNum = (_nNum + _vItem.size() - 1) % _vItem.size();
            if (_vItem[_nNum].GetEnable() == false) _vItem[_nNum].CursorDown();
            bRefesh = true;
            break;
        }

            // .. 오른쪽 화살표를 누르면 오른쪽에 있는 메뉴로 이동한다.
        case KEY_RIGHT: {
            _vItem[_nNum].Close(); // 전에 사용한 윈도우는 Close 한다.
            _nNum = (_nNum + 1) % _vItem.size();
            if (_vItem[_nNum].GetEnable() == false) _vItem[_nNum].CursorDown();
            bRefesh = true;
            break;
        }

        case KEY_ENTER: {
            _bExit = true;
            break;
        }

        case KEY_ESC: {
            _bESC_Exit = true;
            _bExit = true;
            break;
        }
    }

    if (bRefesh) {
        MenuRefresh();
        curs_set(0);
    }
}
