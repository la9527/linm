#include "dialogbox.h"
#include "ncurses_dialog.h"
#include "mlsmutex.h"

using namespace MLSUTIL;
using namespace MLS;

void Curses_Dialog::MsgBox(const string &sTitle, const string &sMsg) {
    Curses_MsgBox tMsgBox(sTitle, sMsg);
    int size = scrstrlen(sMsg) > 20 ? scrstrlen(sMsg) : 20;
    if (size > COLS - 10) size = COLS - 10;
    tMsgBox.Size(7, size + 6);
    tMsgBox.Move(MIDDLE, MIDDLE);
    tMsgBox.Do();
}

bool Curses_Dialog::YNBox(const string &sTitle, const string &sMsg, bool bYes) {
    Curses_YNBox tYNBox(sTitle, sMsg, bYes);
    int size = scrstrlen(sMsg) > 20 ? scrstrlen(sMsg) : 20;
    if (size > COLS - 10) size = COLS - 10;
    tYNBox.Size(7, size + 4);
    tYNBox.Move(MIDDLE, MIDDLE);
    tYNBox.Do();
    return tYNBox.GetData();
}

void *Curses_Dialog::MsgWaitBox(const string &sTitle, const string &sMsg) {
    s_tMsgBox.SetTitle(sTitle);
    s_tMsgBox.SetMsg(sMsg);
    int size = scrstrlen(sMsg) > 20 ? scrstrlen(sMsg) : 20;
    if (size > COLS - 10) size = COLS - 10;
    s_tMsgBox.Size(7, size + 6);
    s_tMsgBox.Move(MIDDLE, MIDDLE);
    s_tMsgBox.Show();
    return (void *) &s_tMsgBox;
}

void Curses_Dialog::MsgWaitEnd(void *pWait) {
    s_tMsgBox.Close();
}

int Curses_Dialog::InputBox(const string &sTitle, string &sInputStr, bool bPasswd) {
    Curses_InputBox tInputBox(sTitle, sInputStr, bPasswd);
    int size = scrstrlen(sTitle) > 40 ? scrstrlen(sTitle) : 40;
    if (size > COLS - 10) size = COLS - 10;
    tInputBox.Size(7, size + 6);
    tInputBox.Move(MIDDLE, MIDDLE);
    tInputBox.Do();
    if (tInputBox.GetStat() == false) return ERROR;
    sInputStr = tInputBox.GetStr();
    return SUCCESS;
}

int Curses_Dialog::SelectBox(const string &sTitle,
                             vector<string> &vMsgStr,
                             int n) {
    Curses_SelectBox tSelectBox(sTitle, n);
    tSelectBox.Size(5, 30);
    tSelectBox.SetSelData(vMsgStr);
    tSelectBox.Move(MIDDLE, MIDDLE);
    tSelectBox.Do();
    return tSelectBox.GetNum();
}

int Curses_Dialog::GetChar(bool bNoDelay) {
    // delay 처리
    if (bNoDelay) nodelay(stdscr, TRUE);
    int n = getch();
    if (bNoDelay) nodelay(stdscr, FALSE);
    return n;
}

int Curses_Dialog::SetKeyBreakUse(bool bBreak) {
    if (bBreak == true) {
        cbreak();
        noraw();
    } else {
        nocbreak();
        raw();
    }
    return SUCCESS;
}

int Curses_Dialog::TextBox(const string &sTitle, vector<string> &vTextStr, bool bCurShow, int width) {
    Curses_TextBox tTextBox(sTitle);

    int nHeightSize = vTextStr.size();

    if (nHeightSize > LINES - 6 && LINES - 6 > 0)
        nHeightSize = LINES - 6;
    else
        nHeightSize = vTextStr.size() + 6;

    int nMaxWidth = width - 4;
    for (uint n = 0; n < vTextStr.size(); n++) {
        int nSize = scrstrlen(vTextStr[n]);
        if (nSize > nMaxWidth) nMaxWidth = nSize;
    }

    if (nMaxWidth + 4 > COLS - 8) nMaxWidth = COLS - 8 - 4;
    tTextBox.setCurShow(bCurShow);
    tTextBox.Size(nHeightSize, nMaxWidth + 4);
    tTextBox.setText(vTextStr);
    tTextBox.Move(MIDDLE, MIDDLE);
    tTextBox.Do();
    return 0;
}

// ProgressBox
Curses_Progress::Curses_Progress() : CommonProgress(), MlsThread(true) {
    _bStarted = false;
    _bDouble = false;

    _nCnt = 0;
    _nCnt2 = 0;

    _pVoidTmp = (void *) new Curses_ProgressBox();
    _pMutex = new MlsMutex;
}

Curses_Progress::~Curses_Progress() {
    if (_pVoidTmp) {
        Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;
        pProgress->Close();
        delete pProgress;
        delete _pMutex;
        _pMutex = NULL;
        _pVoidTmp = NULL;
    }
}

void Curses_Progress::show() {
    Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;

    if (!pProgress) return;

    MlsMutexSelf tMutex(_pMutex);

    int size = scrstrlen(_sTitle) > COLS - 20 ? scrstrlen(_sTitle) : COLS - 20;

    if (size > 130) size = 130;

    if (!_bDouble)
        pProgress->Size(9, size);
    else
        pProgress->Size(11, size);
    pProgress->Move(MIDDLE, MIDDLE);

    if (_sLeftStr.size()) pProgress->setLeftStr(_sLeftStr);
    if (_sRightStr.size()) pProgress->setRightStr(_sRightStr);
    if (_sLeftStr2.size()) pProgress->setLeftStr2(_sLeftStr2);
    if (_sRightStr2.size()) pProgress->setRightStr2(_sRightStr2);

    pProgress->setOneValue(_nCnt);
    pProgress->setTwoValue(_nCnt2);
    pProgress->Show();
}

void Curses_Progress::redraw() {
    show();
}

bool Curses_Progress::isExit() {
    Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;
    if (pProgress) {
        return pProgress->isExit();
    } else {
        // Curses_ProgressBox* 생성을 늦게 하기 때문에 true 로 해놓으면 에러발생.
        return false;
    }
}

void Curses_Progress::showProgBar() {
    try {
        _bStarted = true;
        show();
        MlsThread::Start(NULL, JOIN);
    }
    catch (Exception &ex) {}
}

void Curses_Progress::hideProgBar() {
    try {
        _bStarted = false;
        //_pProgress->Cancel(); // 오류 발생.
        MlsThread::End();
    }
    catch (Exception &ex) {
    }
}

void *Curses_Progress::Execute(void *pArg) {
    try {
        for (;;) {
            show();
            CancelPoint();
            Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;

            if (pProgress) {
                pProgress->Do();

                CancelPoint();
                if (pProgress->isExit()) break;
                if (_bStarted == false) break;
                Thread_uSleep(1000);
            }
        }
    }
    catch (Exception &ex) {
    }
    catch (...) {
    }

    if (_pVoidTmp) {
        Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;

        pProgress->Close();
        delete pProgress;
        _pVoidTmp = NULL;
    }
    return (void *) this;
}

void Curses_Progress::init() {
    MlsMutexSelf tMutex(_pMutex);

    Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;

    if (pProgress)
        pProgress->init();
}

void Curses_Progress::setExitInit() {
    MlsMutexSelf tMutex(_pMutex);

    Curses_ProgressBox *pProgress = (Curses_ProgressBox *) _pVoidTmp;

    if (pProgress)
        pProgress->setExitInit();
}


