#include "mlsdialog.h"

namespace MLSUTIL {

    static MlsDialog *g_pDialog = NULL;
    static CommonProgress *g_pProgress = NULL;

    void SetDialogProgress(MlsDialog *pDialog, CommonProgress *pProgress) {
        LOG_WRITE("SetDialogProgress Dialog [%p] Progress [%p]",
                  pDialog, pProgress);
        LOG_WRITE("SetDialogProgress g_pDialog [%p] g_pProgress [%p]",
                  g_pDialog, g_pProgress);
        g_pDialog = pDialog;
        g_pProgress = pProgress;
    }

///	@brief	 메시지 박스
    void MsgBox(const string &sTitle, const string &sMsg) {
        if (g_pDialog)
            g_pDialog->MsgBox(sTitle, sMsg);
    }

///	@brief	 예, 아니오 박스
    bool YNBox(const string &sTitle, const string &sMsg, bool bYes) {
        if (g_pDialog)
            return g_pDialog->YNBox(sTitle, sMsg, bYes);
        else
            return false;
    }

///	@brief	 메시지 박스 시작. 대기화면에 사용.
    void *MsgWaitBox(const string &sTitle, const string &sMsg) {
        if (!g_pDialog) return NULL;
        return g_pDialog->MsgWaitBox(sTitle, sMsg);
    }

///	@brief	 메시지 박스 끝. 대기화면에 사용.
    void MsgWaitEnd(void *p) {
        if (g_pDialog)
            g_pDialog->MsgWaitEnd(p);
    }

///	@brief	 입력창
/// @param	sTitle		제목
/// @param	sInputStr	입력된 Str
/// @param	bPasswd		패스워드 형식 (영문만 입력 가능하게)
/// @return	성공 SUCCESS
    int InputBox(const string &sTitle, string &sInputStr, bool bPasswd) {
        return g_pDialog->InputBox(sTitle, sInputStr, bPasswd);
    }

///	@brief	 선택 박스
/// @param	sTitle		제목
/// @param	vMsgStr		선택할 데이터들
/// @param	n			선택번호
/// @return	선택된 번호
    int SelectBox(const string &sTitle,
                  vector<string> &vMsgStr,
                  int n) {
        return g_pDialog->SelectBox(sTitle, vMsgStr, n);
    }

///	@brief	 키 읽기.
    int GetChar(bool bNoDelay) {
        return g_pDialog->GetChar(bNoDelay);
    }

    int SetKeyBreakUse(bool bBreak) {
        return g_pDialog->SetKeyBreakUse(bBreak);
    }

/// @brief	text box
    int TextListBox(const string &sTitle, vector<string> &vTextStr, bool bCurShow, int width) {
        return g_pDialog->TextBox(sTitle, vTextStr, bCurShow, width);
    }


/* COMMON PROGRESS REMOVE
CommonProgress::CommonProgress(const string& sTitle, const string& sMsg, bool bDouble)
{
	_sTitle = sTitle; _sMsg = sMsg; _bDouble = bDouble;
	_bStarted = false;
	_pProgress = g_pProgress;
}

CommonProgress::~CommonProgress()
{
	if (_bStarted) _pProgress->End();
	_bStarted = false;
}

void CommonProgress::SetProgress( MlsProgress* p) 
{
	_pProgress = p;
}

void CommonProgress::setLeftStr(const string& p)
{ 
	if (_pProgress)
		_pProgress->setLeftStr( p ); 
}

void CommonProgress::setRightStr(const string& p)
{
	if (_pProgress)
		_pProgress->setRightStr( p ); 
}

void CommonProgress::setLeftStr2(const string& p)
{
	if (_pProgress)
		_pProgress->setLeftStr2( p ); 
}

void CommonProgress::setRightStr2(const string& p)
{
	if (_pProgress)
		_pProgress->setRightStr2( p ); 
}

bool CommonProgress::isExit()
{
	if (!_pProgress)
		return false;
	return _pProgress->isExit();
}

void CommonProgress::show()
{
	_pProgress->_sTitle = _sTitle;
	_pProgress->_sMsg = _sMsg;
	_pProgress->_bDouble = _bDouble;

	if (_pProgress)
		_pProgress->Show();
}

void CommonProgress::redraw()
{
	_pProgress->_sTitle = _sTitle;
	_pProgress->_sMsg = _sMsg;
	_pProgress->_bDouble = _bDouble;

	_pProgress->Redraw();
}

void CommonProgress::setCount(int nCnt, int nCnt2)
{
	_pProgress->setCount(nCnt, nCnt2);
}

void CommonProgress::Start(void* pArg)
{
	try
	{
		_bStarted = true;
		_pProgress->_bStarted = true;
		show();
		LOG_WRITE("CommonProgress::Start");
		_pProgress->Start(pArg, JOIN);
	}
	catch(Exception& ex)
	{
	}
}

void CommonProgress::End()
{
	try
	{
		_pProgress->_bStarted = false;
		//_pProgress->Cancel(); // 오류 발생.
		_pProgress->End();
		LOG_WRITE("CommonProgress End ...");
	}
	catch(Exception& ex)
	{
	}
}
*/

};
