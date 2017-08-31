#ifndef __editor_h__
#define __editor_h__

#include "define.h"
#include "mlsdialog.h"
#include "mlslocale.h"
#include "editorclipboard.h"
#include "strutil.h"
#include "file.h"

using namespace std;

using namespace MLSUTIL;

namespace MLS {

    struct LineInfo {
        int nTextLine;        /// Text Position
        int nViewLine;        /// screen view position
        int nNextLineNum;    /// 한 라인을 넘어 서는 경우에 라인 위치
        bool bNext;            /// 한 라인을 넘어섰는가?
        wstring sWString;        /// wstring
    };

    struct EditSelect {
        int x1;
        int y1; /// select first position(x,y)
        int x2;
        int y2; /// select last position (x,y)
    };

    enum EditMode {
        EDIT,            /// Edit Mode
        SELECT,            /// Select Mode
        BLOCK,            /// Block Select Mode
        SHIFT_SELECT    /// Shift Mode
    };

#define        TABCONVCHAR        '\t'

    class Editor {
    protected:
        int _nLine;                /// text screen line size
        int _nCulumn;            /// text screen culumn size
        int _nCurCulumn_Max;    /// max culumn position

        int _nFirstLine;        /// screen line first position
        int _nLastLine;            /// screen line last postion
        int _nViewCul;        /// view culumn size
        int _nViewLine;        /// view line size

        int _nCurLine;        /// current line position
        int _nCurCulumn;        /// current culumn position

        bool _bLineNumView;    /// Line Number View
        bool _bInsert;        /// Insert mode
        bool _bIndentMode;    /// Indentation mode (들여쓰기 모드)

        EditMode _EditMode;        /// edit mode
        EditSelect _EditSelect;    /// Select X,Y value

        int _nLineWidth;    /// line number size
        int _nTabSize;        /// tab size

        bool _bReadOnly;        /// ReadOnly value
        bool _bDosMode;        /// DosMode
        bool _bKeyCfgLoad;    /// key file load

        string _title;            /// title
        string _sFile;            /// file
        bool _bBackup;        /// backup 여부
        ENCODING _eEncode;        /// current load file encoding code value
        int _nConvInfo;        /// Save 지점 변경 위치 저장 (_vDoInfo 사이즈 참조)

        File _tFile;            /// File 정보

        // find 관련
        wstring _sFindStr;
        int _nFindPosX;
        int _nFindPosY;

        vector<LineInfo> _vViewString;
        vector<wstring> _vText;            /// wstring vector
        vector<DoInfo *> _vDoInfo;        /// vector for Undo

        /// Function List
        void SelectSort(EditSelect *pEditSelect);

        void Selected_Del();

        void ScreenMemSave(int nLine, int nCulumn);

    public:
        Editor();

        ~Editor();

        void Destroy();

        void SetViewTitle(const string &sStr = "") { _title = sStr; }

        void SetEditor(int nTabSize = 8, bool bBackup = false, bool bLineNumView = false);

        string GetViewTitle() const { return _title; }

        void New(const string &sFile);

        bool Load(const string &sFile, bool bReadOnly = false);

        bool Load(File *pFile, bool bReadOnly = false);

        bool Save(const string &sFile, ENCODING Encode, bool bDosMode = false, bool bBackup = false);

        bool Save(File *pFile, ENCODING Encode, bool bDosMode = false, bool bBackup = false);

        string TabToEdit(const string &sStr, char cTabChar, const int nTabSize);

        string EditToTab(const string &sStr, char cTabChar, const int nTabSize);

        File *GetFile() { return &_tFile; }

        string GetFileName() const { return _sFile; }

        /********************************************/
        /* 명령어 */
        void LineNumberView() {
            _bLineNumView = !_bLineNumView;
        }

        void Key_Left();

        void Key_Right();

        void Key_Up();

        void Key_Down();

        void Key_ShiftLeft();

        void Key_ShiftRight();

        void Key_ShiftUp();

        void Key_ShiftDown();

        void Key_Ins() { _bInsert = !_bInsert; }

        void Key_Del();

        void Key_BS();

        void Key_Tab();

        void Key_Untab();

        void IndentMode() { _bIndentMode = !_bIndentMode; }

        bool GetIndentMode() { return _bIndentMode; }

        void InputData(const string &sKrStr);

        void Key_Home();

        void Key_End();

        void Key_PgUp();

        void Key_PgDn();

        void Key_Enter();

        void Key_Mouse();

        void GotoLine();

        void GotoFirst();

        void GotoEnd();

        void Copy();

        void Cut();

        void Paste();

        void Undo();

        void Key_ESC();

        void Select();

        void SelectAll();

        void BlockSelect();

        void FileNew();

        bool FileSave();

        bool FileSaveAs();

        void Find();

        void FindNext();

        void FindPrevious();

        bool Quit();

        bool IsEditMode() { return _EditMode == EDIT; }
    };

};

#endif

