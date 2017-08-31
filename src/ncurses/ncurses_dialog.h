#ifndef __NCURESES_DIALOG_H__
#define __NCURESES_DIALOG_H__

#include "dialogbox.h"
#include "mlsdialog.h"
#include "mlsthread.h"

using namespace MLSUTIL;

namespace MLS {

    class Curses_Dialog : public MlsDialog {
    protected:
        Curses_MsgBox s_tMsgBox;

    public:
        Curses_Dialog() {
            s_tMsgBox = Curses_MsgBox("", "");
        }

        void MsgBox(const string &sTitle, const string &sMsg);

        bool YNBox(const string &sTitle, const string &sMsg, bool bYes = false);

        void *MsgWaitBox(const string &sTitle, const string &sMsg);

        void MsgWaitEnd(void *pWait);

        int InputBox(const string &sTitle, string &sInputStr, bool bPasswd = false);

        int SelectBox(const string &sTitle, vector<string> &vMsgStr, int n = 0);

        int GetChar(bool bNoDelay = false);

        int SetKeyBreakUse(bool bBreak = false);

        int TextBox(const string &sTitle, vector<string> &vTextStr, bool bCurShow = false, int width = 50);
    };

///	@brief	 상태출력 박스
    class Curses_Progress : public CommonProgress, public MlsThread {
    public:
        Curses_Progress();

        virtual ~Curses_Progress();

    protected:
        bool _bStarted;
        MlsMutex *_pMutex;
        void *_pVoidTmp;        ///< void Tmp 형태. 형변환 해서 사용 바람.

        string _sTitle;
        string _sMsg;
        string _sLeftStr, _sLeftStr2;
        string _sRightStr, _sRightStr2;

        int _nCnt, _nCnt2;
        bool _bDouble;

    public:
        void init();

        void setTitle(const string &str) { _sTitle = str; }

        void setMsg(const string &str) { _sMsg = str; }

        void setDouble(bool bDouble) { _bDouble = bDouble; }

        void setLeftStr(const string &p) {
            MlsMutexSelf tMutex(_pMutex);
            _sLeftStr = p;
        }

        void setRightStr(const string &p) {
            MlsMutexSelf tMutex(_pMutex);
            _sRightStr = p;
        }

        void setLeftStr2(const string &p) {
            MlsMutexSelf tMutex(_pMutex);
            _sLeftStr2 = p;
        }

        void setRightStr2(const string &p) {
            MlsMutexSelf tMutex(_pMutex);
            _sRightStr2 = p;
        }

        bool isExit();

        void show();

        void redraw();

        void setOneValue(int nCnt) {
            MlsMutexSelf tMutex(_pMutex);
            _nCnt = nCnt;
        }

        void setTwoValue(int nCnt2) {
            MlsMutexSelf tMutex(_pMutex);
            _nCnt = nCnt2;
        }

        void setExitInit();

        void showProgBar();

        void hideProgBar();

        void *Execute(void *pArg);
    };

};

#endif

