#ifndef  __DIALOGBOX_H__
#define  __DIALOGBOX_H__

#include "define.h"
#include "strutil.h"
#include "dialog.h"
#include "colorcfgload.h"
#include "mlsmutex.h"

using namespace MLSUTIL;

namespace MLS {

    class Curses_MsgBox : public Form {
    public:
        Curses_MsgBox(const string &sTitle, const string &sMsg);

        Curses_MsgBox();

        void SetMsg(const string &str);

    protected:
        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    private:
        Button _tButton;
        Label _tLabel;
    };

    class Curses_YNBox : public Form {
    public:
        Curses_YNBox(const string &sTitle, const string &sMsg, bool bYes = false);

        bool GetData() { return _bYes; }

    protected:
        void Resize();

        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    private:
        Button _tButtonOk;
        Button _tButtonCancel;
        Label _tLabel;
        bool _bYes;
    };

    class Curses_SelectBox : public Form {
    public:
        Curses_SelectBox(const string &sTitle, int nDef = 0);

        ~Curses_SelectBox() {
            Clear();
        }

        void Clear();

        void SetSelData(vector<string> vData, bool bShowNumber = false);

        int GetNum() { return _nCur; }

    protected:
        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    protected:
        vector<string> _vData;
        int _nCur;

    private:
        vector<Button *> _vButtonList;
        int _nStrMaxLength;
        bool _bHoriz;
        bool _bShowNumber;
    };

    class Curses_InputBox : public Form {
    public:
        Curses_InputBox(const string &sTitle, const string &sStr, bool bPasswd = false);

        ~Curses_InputBox() {}

        bool GetStat() { return _bYes; }

        string GetStr() {
            return _tInputBox.GetStr();
        }

    protected:
        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    private:
        Button _tButtonOk;
        Button _tButtonCancel;
        Input _tInputBox;
        bool _bStat;
        bool _bYes;
    };

    class Curses_ProgressBox : public Form {
    public:
        Curses_ProgressBox();

        virtual ~Curses_ProgressBox() {}

        void init() {
            _sTitle = _sMsg = "";
            _bDouble = false;
            _bExit = false;
            setLeftStr("");
            setLeftStr2("");
            setRightStr("");
            setRightStr2("");
        }

        void setTitle(const string &sTitle) { _sTitle = sTitle; }

        void setMsg(const string &sMsg) { _sMsg = sMsg; }

        void setDouble(const bool bDouble) { _bDouble = bDouble; }

        bool isExit() { return _bExit; }

        void setExitInit() { _bExit = false; }

        void Do();

        void setLeftStr(const string &p) { _tLabelUpLeft.SetMsg(p); }

        void setRightStr(const string &p) {
            _tLabelUpRight.SetMsg(p);
            _sRightStr = p;
        }

        void setLeftStr2(const string &p) { _tLabelDnLeft.SetMsg(p); }

        void setRightStr2(const string &p) {
            _tLabelDnRight.SetMsg(p);
            _sRightStr2 = p;
        }

        void setOneValue(int nCnt);

        void setTwoValue(int nCnt);

    protected:
        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    private:
        Button _tButtonCancel;
        Label _tLabel;
        Label _tLabelUpLeft, _tLabelUpRight;
        Label _tLabelDnLeft, _tLabelDnRight;

        string _sRightStr;
        string _sRightStr2;

        Progress _tProgress1, _tProgress2;

        bool _bDouble;        ///< Progress가 위아래 두개면 true
        string _sTitle, _sMsg;

        int _nCnt;        ///< 카운터 기준 (% 단위)
        int _nCnt2;        ///< 카운터 기준 (% 단위)
    };

    class Curses_TextBox : public Form {
    public:
        Curses_TextBox(const string &sTitle);

        virtual ~Curses_TextBox() {}

        void setTitle(const string &sTitle) { _sTitle = sTitle; }

        void setText(vector<string> &vText);

        bool isExit() { return _bExit; }

        void setCurShow(bool bCursor = false);

        int GetCur() {
            if (!_bESC)
                return _tTextBox.GetCur();
            else
                return -1;
        }

    protected:
        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    private:
        Button _tButton;
        TextBox _tTextBox;
        string _sTitle;
        bool _bESC;
    };

    class Curses_CheckBox : public Form {
    public:
        Curses_CheckBox(const string &sTitle);

        ~Curses_CheckBox() {
            Clear();
        }

        void Clear();

        void SetChkData(vector<string> &vData, vector<bool> &vChkList);

        vector<bool> GetChkList() { return _vChkList; }

        bool IsYes() { return _bYes; }

    protected:
        void Draw();

        bool MouseEvent(int Y, int X, mmask_t bstate);

        void Execute(KeyInfo &tKeyInfo);

    protected:
        vector<string> _vData;
        vector<bool> _vChkList;
        int _nCur;

    private:
        vector<CheckRadioButton *> _vChkButtonList;
        Button _tButtonOk;
        Button _tButtonCancel;
        bool _bYes;
        bool _bCheckList;
        int _nStrMaxLength;
    };

};

#endif
