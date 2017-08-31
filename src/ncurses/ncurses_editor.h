#ifndef __NCurses_Editor_h__
#define __NCurses_Editor_h__

#include "define.h"
#include "editor.h"
#include "dialog.h"

using namespace std;

namespace MLS {

    class NCurses_Editor : public Editor, public Form {
        WINDOW *_pWin;

        ColorEntry _tLine;
        ColorEntry _tColorEdit;
        ColorEntry _tColorStat;
        ColorEntry _tColorEditInfo;
        ColorEntry _tColorEditInfoA;

        // replace 관련
        wstring _sReplaceStr;

    protected:
        void SelectionDraw(const wstring &sViewWStr, int nY, int nN, int x1, int x2);

        void LineDraw(const wstring &sViewStr,
                      int nY,
                      int nN,
                      int nLineNum,   // 현재 라인 위치
                      int nNum);      // 줄바뀜 번호
        void CurserDraw(int nY, int nN);

        void BackDrawEtc();

        void Draw();

    public:
        bool _bFocus;
        bool _bMouseMode;
        bool _bFullScreen;
        bool _bConfChg;

        NCurses_Editor() : Editor() {
            _bFocus = false;
            _bNoOutRefresh = true; // Refresh 할때 doupdate 하지 않는다.
            _bNoViewUpdate = true; // 업데이트 하지 않는다.
            _bNotDrawBox = true; // 박스를 그리지 않는다.
            _bMouseMode = true;  // 마우스모드를 사용하지 않는다.( ternminal에서 Clip 복사 사용)
            _bFullScreen = false; // FullScreen
            _bConfChg = false;        // Configure 파일 편집시 true.

            Init();
        }

        void Init();

        void CurserDraw() {
            //  커서 위치를 옮긴다.
            if (_bLineNumView == true)
                CurserDraw(1, 3 + _nLineWidth);
            else
                CurserDraw(1, 0);
        }

        void Replace();
    };

};

#endif

