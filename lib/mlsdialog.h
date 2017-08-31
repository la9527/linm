#ifndef __MLSDIALOG_H__
#define __MLSDIALOG_H__

#include <stdarg.h>
#include "define.h"
#include "mlsthread.h"
#include "mlsmutex.h"

namespace MLSUTIL {

    class MlsDialog {
    public:
        MlsDialog() {}

        virtual ~MlsDialog() {}

        virtual void MsgBox(const string &sTitle, const string &sMsg) {
            cout << "MsgBox :: " << sTitle << "Msg :: " << sMsg << endl;
        }

        virtual bool YNBox(const string &sTitle, const string &sMsg, bool /*bYes*/ = false) {
            cout << "YNBox :: " << sTitle << "Msg :: " << sMsg << endl;
            return false;
        }

        virtual void *MsgWaitBox(const string &sTitle, const string &sMsg) {
            cout << "MsgWaitBox :: " << sTitle << "Msg :: " << sMsg << endl;
            return (void *) false;
        }

        virtual void MsgWaitEnd(void *) {}

        virtual int InputBox(const string &sTitle, string &sInputStr, bool /*bPasswd*/ = false) {
            cout << "InputBox :: " << sTitle << endl;
            cin >> sInputStr;
            return 0;
        }

        virtual int SelectBox(const string & /*sTitle*/, vector<string> & /*vMsgStr*/, int /*n*/ = 0) { return 0; }

        virtual int GetChar(bool /*bNoDelay*/ = false) { return 0; }

        virtual int SetKeyBreakUse(bool /*bBreak*/ = false) { return 0; }

        virtual int TextBox(const string & /*sTitle*/, vector<string> & /*vTextStr*/, bool /*bCurShow*/ = false,
                            int /*width*/ = 40) { return 0; }
    };

    class CommonProgress {
    public:
        CommonProgress() {}

        virtual ~CommonProgress() {}

        virtual void init() = 0;

        virtual void hideProgBar() = 0;

        virtual void showProgBar() = 0;

        virtual void setTitle(const string &) = 0;

        virtual void setLeftStr(const string &) = 0;

        virtual void setRightStr(const string &) = 0;

        virtual void setLeftStr2(const string &) = 0;

        virtual void setRightStr2(const string &) = 0;

        virtual void setOneValue(int) = 0;

        virtual void setTwoValue(int) = 0;

        virtual bool isExit() = 0;

        virtual void setExitInit() = 0;

        virtual void redraw() = 0;
    };

/*
class CommonProgress
{
protected:
	string			_sTitle;
	string			_sMsg;
	bool			_bDouble;
	MlsProgress*	_pProgress;
	bool			_bStarted;

public:
	CommonProgress(const string& sTitle, const string& sMsg, bool bDouble = false);
	virtual ~CommonProgress();

	void SetProgress( MlsProgress* p);
	
	void setTitle( const string& title );
	
	void setLeftStr(const string& p);
	void setRightStr(const string& p);
	void setLeftStr2(const string& p);
	void setRightStr2(const string& p);
	
	bool isExit();
	void show();
	
	void redraw();
	void setCount(int nCnt, int nCnt2 = -1);
	void Start(void* pArg = NULL);
	void End();
};
*/

/// @brief	SetDialog
    void SetDialogProgress(MlsDialog *pDialog, CommonProgress *pProgress);

///	@brief	 메시지 박스
    void MsgBox(const string &sTitle, const string &sMsg);

///	@brief	 예, 아니오 박스
    bool YNBox(const string &sTitle, const string &sMsg, bool bYes = false);

///	@brief	 메시지 박스 시작. 대기화면에 사용.
    void *MsgWaitBox(const string &sTitle, const string &sMsg);

///	@brief	 메시지 박스 끝. 대기화면에 사용.
    void MsgWaitEnd(void *p);

///	@brief	 입력창
/// @param	sTitle		제목
/// @param	sInputStr	입력된 Str
/// @param	bPasswd		패스워드 형식 (영문만 입력 가능하게)
/// @return	성공 SUCCESS
    int InputBox(const string &sTitle, string &sInputStr, bool bPasswd = false);

///	@brief	 선택 박스
/// @param	sTitle		제목
/// @param	vMsgStr		선택할 데이터들
/// @param	n			선택번호
/// @return	선택된 번호
    int SelectBox(const string &sTitle,
                  vector<string> &vMsgStr,
                  int n = 0);

///	@brief	 키 읽기.
    int GetChar(bool bNoDelay = false);

/// @brief	ctrl+c 사용 가능하게 사용.
    int SetKeyBreakUse(bool bBreak = false);

/// @brief	text box
    int TextListBox(const string &sTitle, vector<string> &vTextStr, bool bCurShow = false, int width = 40);

};

#endif
