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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <iomanip>

#include "define.h"
#include "drawset.h"
#include "colorcfgload.h"
#include "mlscfgload.h"
#include "mlslocale.h"
#include "reader.h"
#include "mainframe.h"
#include "mlsdialog.h"
#include "subshell.h"

#include "ncurses_dialog.h"

using namespace MLSUTIL;
using namespace MLS;

namespace { // 처음 기본 세팅

    bool _bMcdExe = false;        ///< MCD실행 여부
    ENCODING _nLangSet = AUTO;       ///< 언어 설정
    LINECODE _nLineSet = ACSLINE;    ///< LINE MODE
    bool _bTitleChange = true;

    vector<string> _vCfgFile, _vColFile, _vKeyFile, _vEditorKeyFile;

}

/// @brief	mls 시작시 초기화 함수
///
///  config준비. 설정파일 읽기, 컬러셋 읽기, history읽기를 한다.
/// @return	성공여부 0일때 성공
bool Initialize() {
    int t;
    const char *succMsg = "[\033[01;36m SUCCESS \033[0m]";
    const char *failMsg = "[\033[01;31m  FAIL   \033[0m]";
    const char *errMsg = "[\033[01;31m  ERROR  \033[0m]";

    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        String sMsg;
        sMsg.appendBlank(60, "%s", strerror(errno));
        cout << sMsg.c_str();
        cout << errMsg << endl;
        return false;
    }
    if (access(cwd, R_OK | X_OK) == -1) {
        String sMsg;
        sMsg.appendBlank(60, "%s", strerror(errno));
        cout << sMsg.c_str();
        cout << errMsg << endl;
        return false;
    }
    free(cwd);

    Set_Locale(_nLangSet);    // Locale  설정

    string sCfgDefaultPath, sCfgColorPath;

    sCfgDefaultPath = sCfgDefaultPath + __LINM_CFGPATH__ + "/default.cfg";
    sCfgColorPath = sCfgColorPath + __LINM_CFGPATH__ + "/colorset.cfg";

    { // Config 준비..
        struct passwd *pw = getpwuid(getuid());
        {
            std::string home = pw->pw_dir;
            home += '/';

            g_tCfg.setStaticValue("Home", home);
        }

        // . config dir 지정
        g_tCfg.setStaticValue("CfgHome", g_tCfg.getValue("Static", "Home") + ".linm/");
        g_tCfg.setStaticValue("TmpDir", g_tCfg.getValue("Static", "Home") + ".linm/linm_tmpdir/");
        g_tCfg.setStaticValue("TmpCopyDir", g_tCfg.getValue("Static", "Home") + ".linm/linm_copydir/");
        g_tColorCfg.Init();

        // 홈에 .linm를 만든다. mcd treeinfo를 저장하기 위해서도 필요
        mkdir((g_tCfg.getValue("Static", "Home") + ".linm").c_str(), 0755);
        // 파일 View 필요한 tmp 디렉토리.
        mkdir((g_tCfg.getValue("Static", "TmpDir")).c_str(), 0777);
        // 파일 복사에 필요한 tmp 디렉토리.
        mkdir((g_tCfg.getValue("Static", "TmpCopyDir")).c_str(), 0777);
    }

    { // 설정 파일 읽기
        _vCfgFile.push_back(g_tCfg.getValue("Static", "CfgHome") + "default.cfg");
        _vCfgFile.push_back(sCfgDefaultPath);

        for (t = 0; t < (int) _vCfgFile.size(); t++) {
            string cfgfile = _vCfgFile[t];

            if (g_tCfg.Load(cfgfile.c_str())) {
                g_tCfg.setStaticValue("CfgFile", cfgfile);
#ifdef __DEBUGMODE__
                String sMsg;
                sMsg.AppendBlank(60, "Load configuration %s", cfgfile.c_str());
                cout << sMsg.c_str();
                cout << succMsg << endl;
#endif
                if (cfgfile == sCfgDefaultPath) {
                    string sCmd = "cp " + sCfgDefaultPath + " " + g_tCfg.getValue("Static", "Home") + ".linm";
                    system(sCmd.c_str());
                }
                break;
            } else {
                String sMsg;
                sMsg.appendBlank(60, "Load configuration %s", cfgfile.c_str());
                cout << sMsg.c_str();
                cout << failMsg << endl;
            }
        }

        if (t == (int) _vCfgFile.size()) {
            String sMsg;
            sMsg.appendBlank(60, "Load configuration ");
            cout << sMsg.c_str();
            cout << failMsg << endl;
            return false;
        }
    }

    { // 컬러셋 읽기
        if (g_tCfg.getValue("Default", "ColorSetFile") != "")
            _vColFile.push_back(g_tCfg.getValue("Static", "CfgHome") + g_tCfg.getValue("Default", "ColorSetFile"));
        _vColFile.push_back(sCfgColorPath);

        for (t = 0; t < (int) _vColFile.size(); t++) {
            string colfile = _vColFile[t];
            if (g_tColorCfg.Load(colfile.c_str())) {
                g_tCfg.setStaticValue("ColFile", colfile);
#ifdef __DEBUGMODE__
                String sMsg;
                sMsg.AppendBlank(60, "Load colorset %s", colfile.c_str());
                cout << sMsg.c_str();
                cout << succMsg << endl;
#endif
                if (colfile == sCfgColorPath) {
                    string sCmd = "cp " + sCfgColorPath + " " + g_tCfg.getValue("Static", "Home") + ".linm";
                    system(sCmd.c_str());
                }
                break;
            } else {
                String sMsg;
                sMsg.appendBlank(60, "Load colorset %s", colfile.c_str());
                cout << failMsg << endl;
            }
        }

        if (t == (int) _vColFile.size()) {
            String sMsg;
            sMsg.appendBlank(60, "Load colorset");
            cout << sMsg.c_str();
            cout << failMsg << endl;
            return false;
        }
    }
    return true;
}

/// @brief  Mls keybind파일, mls editor keybind 파일을 읽는다.
///
/// MainFrame 안에 KeyBind가 있기 때문에 따로 만듬.
/// @return keybind 파일 읽기 성공여부.
bool Load_KeyFile() {
    int t;
    const char *succMsg = "[\033[01;36m SUCCESS \033[0m]";
    const char *failMsg = "[\033[01;31m  FAIL   \033[0m]";
    string sKeyCfgPath;
    sKeyCfgPath = sKeyCfgPath + __LINM_CFGPATH__ + "/keyset.cfg";

    {// Key Binding file을 읽는다.
        if (g_tCfg.getValue("Default", "KeySetFile") != "")
            _vKeyFile.push_back(g_tCfg.getValue("Static", "CfgHome") + g_tCfg.getValue("Default", "KeySetFile"));
        _vKeyFile.push_back(sKeyCfgPath);

        for (t = 0; t < (int) _vKeyFile.size(); t++) {
            string keyfile = _vKeyFile[t];
            if (g_tKeyCfg.Load(keyfile)) {
                g_tCfg.setStaticValue("KeyFile", keyfile);
#ifdef __DEBUGMODE__
                String sMsg;
                sMsg.AppendBlank(60, "Load key settings... %s", keyfile.c_str());
                cout << sMsg.c_str();
                cout << succMsg << endl;
#endif
                if (keyfile == sKeyCfgPath) {
                    string sCmd = "cp " + sKeyCfgPath + " " + g_tCfg.getValue("Static", "Home") + ".linm";
                    system(sCmd.c_str());
                }
                break;
            } else {
                String sMsg;
                sMsg.appendBlank(60, "Load key settings... %s", keyfile.c_str());
                cout << sMsg.c_str();
                cout << failMsg << endl;
            }
        }

        if (t == (int) _vKeyFile.size()) {
            String sMsg;
            sMsg.appendBlank(60, "Load key settings... ");
            cout << sMsg.c_str();
            cout << failMsg << endl;
            return false;
        }
    }

#ifdef __DEBUGMODE__
    {
        String sMsg;
        sMsg.AppendBlank(60, "Mouse Wheel... ");
        cout << sMsg.c_str();

#if NCURSES_MOUSE_VERSION > 1
            cout << succMsg << endl;
#else
            cout << failMsg << endl;
#endif
    }
#endif
    return true;
}

/// @brief	print help
void PrintHelp(void) {
    const char *sStr_Ko =
            "LinM 는 도스용 파일관리 툴 Mdir의 리눅스 클론입니다.\n"
                    "프로그램의 기능 버그, 추가할 사항, 기타 문의는 "
                    "프로젝트 홈페이지나, 개발자 이메일을 통해서 연락주십시오.\n\n"
                    "  * 프로젝트 홈페이지 : https://github.com/la9527/linm\n"
                    "  * 옵션 설명\n"
                    "\t --help       : 도움말\n"
                    "\t --lang=CODE  : 출력언어 설정, 사용가능한 CODE는 \n"
                    "\t              : us(영어), ko(한글, utf-8), ko_euckr(한글, euc-kr)입니다.\n"
#ifdef __DEBUGMODE__
                    "\t --debug=FILE : 디버그 메시지를 지정된 파일로 출력\n"
#endif
                    "\t --cfg=FILE   : 설정파일 지정\n"
                    "\t --col=FILE   : 컬러셋 파일 지정\n"
                    "\t --key=FILE   : 키 파일 지정\n"
                    "\t --noline     : 선형태를 -,|,+ 로 바꿈\n"
                    "\t --mcd        : 바로 MCD 실행 \n";

    const char *sStr_En =
                    "LinM is a clone of Mdir, the famous file manager from the MS-DOS age. \n"
                    "LinM is rich full-screen text mode shell application that assist you copy, move, delete"
                    "files and directories, search for files and run commands in the subshell.\n\n"
                    " * Project homepage : https://github.com/la9527/linm\n"
                    " * Option\n"
                    "\t --help       : print this page\n"
                    "\t --lang=CODE  : set language, following codes are available\n"
                    "\t              : us(english), ko(korean, utf-8), ko_euckr(korean, euc-kr)\n"
#ifdef __DEBUGMODE__
                    "\t --debug=FILE : redirect debug message to FILE.\n"
#endif
                    "\t --cfg=FILE   : config file\n"
                    "\t --col=FILE   : colorset file\n"
                    "\t --key=FILE   : keybind file\n"
                    "\t --noline     : change box code to ascii character(-,|,+)\n"
                    "\t --mcd        : execute Mcd \n";

    cout << ChgEngKor(sStr_En, sStr_Ko) << endl;
}

/// @brief	Option 처리함수
/// @param	argc	프로그램 시작인자 개수
/// @param	argv	프로그램 시작 인자
void OptionProc(int argc, char *const argv[]) {
    int opt = -1;
    struct option longopts[] = {
            {"help",   no_argument,       NULL, 'h'},
            {"noline", no_argument,       NULL, 'n'},
            {"mcd",    no_argument,       NULL, 'm'},
            {"lang",   required_argument, NULL, 'l'},
            {"debug",  required_argument, NULL, 'd'},
            {"cfg",    required_argument, NULL, 'c'},
            {"col",    required_argument, NULL, 's'},
            {"key",    required_argument, NULL, 'k'},
            {NULL, 0,                     NULL, 0}
    };

    if (getenv("TERM")) {
        string sTerm = getenv("TERM");

        // 일반 콘솔 화면에서는 한글 처리가 미흡하기 때문에
        // TERM 이 linux 이면 영문으로 나오게 한다.
        // 옵션으로 한글을 바꿀수 있다.

        if (sTerm == "linux") {
            _nLangSet = US;
            _bTitleChange = false;
            //e_nBoxLineCode = CHARLINE; // 검토..
        }

        if (sTerm == "cygwin") {
            _nLangSet = KO_EUCKR;
            e_nBoxLineCode = CHARLINE;
        }
    }

    string sLogFile = "/dev/null";
    cout << "LinM " << VERSION << ", user-friendly graphic shell, 2017" << endl << endl;

    while ((opt = getopt_long(argc, argv, "hnmldcsk:", longopts, NULL)) != -1) {
        switch (opt) {
            case 'n':        // noline
                e_nBoxLineCode = CHARLINE;
                break;

            case 'l':        // lang
                if (!optarg) break;
                if (!strcmp(optarg, "us"))
                    _nLangSet = US;
                else if (!strcmp(optarg, "ko_euckr"))
                    _nLangSet = KO_EUCKR;
                else if (!strcmp(optarg, "ko"))
                    _nLangSet = KO_UTF8;
                break;

#ifdef __DEBUGMODE__
            case 'd':       // debug
                if (optarg)
                    sLogFile = optarg;
                break;
#endif

            case 'm':        // mcd
                _bMcdExe = true;
                break;

            case 'c':       // configure file
                if (optarg) _vCfgFile.push_back(optarg);
                break;

            case 's':       // color set file
                if (optarg) _vColFile.push_back(optarg);
                break;

            case 'k':       // keybind file
                if (optarg) _vKeyFile.push_back(optarg);
                break;

            case '?':       // 오류
            case 'h':        // help
            default:
                Set_Locale(_nLangSet);
                PrintHelp();
                exit(0);
        }
    }

    int fd = 0;

#ifdef __DEBUGMODE__
    if (sLogFile != "/dev/null")
        g_Log.SetFile(sLogFile);

    if ((fd = open(sLogFile.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
    {
        printf("Log file open error : %s ", sLogFile.c_str());
        exit(0);
    }
    else
    {
        //dup2(fd, 2); // stderr에 리다이렉션한다.
        close(fd);
    }
#endif

    if ((fd = open("/dev/null", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR)) >= 0) {
        dup2(2, fd); // stderr에 리다이렉션한다.
        close(fd);
    }
}

void CopyConfFiles() {
    if (g_tCfg.getVersion() != VERSION) {
        bool bYN = YNBox(_("Error"),
                         _("configuration files are not compatible. configuration files copy ?"),
                         true);

        if (bYN == true) {
            system("mkdir ~/.linm/back 2> /dev/null > /dev/null; "
                           "cp ~/linm/* ~/.linm/back 2> /dev/null > /dev/null; "
                           "cp " __LINM_CFGPATH__ "/* ~/.linm 2> /dev/null > /dev/null");

            g_tCfg.Load((g_tCfg.getValue("Static", "CfgHome") + "default.cfg").c_str());
            g_tColorCfg.Load((g_tCfg.getValue("Static", "CfgHome") + "colorset.cfg").c_str());
            g_tKeyCfg.Load((g_tCfg.getValue("Static", "CfgHome") + "keyset.cfg").c_str());
        }
    }
}

int main(int argc, char *argv[]) {
    OptionProc(argc, argv);

    if (Initialize() == false) return SUCCESS;
    if (Load_KeyFile() == false) return SUCCESS;

    Signal_Blocking();

    // terminal title change.
    printf("%c]0;LinM %s%c", '\033', VERSION, '\007');

    CursesInit(g_tCfg.getBool("Default", "Transparency", false));

    // Dialog, Progress를 ncurses로 변경
    MLSUTIL::SetDialogProgress(new Curses_Dialog(), new Curses_Progress());

    MouseInit();

    if (g_SubShell.CheckSid() == ERROR) {
        MsgBox(_("Error"), _("LinM is already running on this terminal. LinM is return key [ Ctrl+O ]"));
        CursesDestroy();
        return 0;
    }

    try {
        CopyConfFiles();

        g_tMainFrame.SetTitleChange(_bTitleChange);

        if (_bMcdExe)
            g_tMainFrame.DoMcd();

        g_tMainFrame.Do();
        CursesDestroy();
    }
    catch (Exception &ex) {
        CursesDestroy();
        cout << "Exception Error !!! -" << ex << endl;
    }
#ifndef __DEBUGMODE__
    catch (...) {
        CursesDestroy();
        cout << "Exception Error !!! -" << endl;
    }
#endif
    LOG_WRITE("Main Success Exit ...");

    if (g_tCfg.getValue("Static", "TmpDir") != "") {
        string sTmpDel = "rm -rf " + g_tCfg.getValue("Static", "TmpDir") + "*";
        system(sTmpDel.c_str());
    }

    if (g_tCfg.getValue("Static", "TmpCopyDir") != "") {
        string sTmpDel = "rm -rf " + g_tCfg.getValue("Static", "TmpCopyDir") + "*";
        system(sTmpDel.c_str());
    }

    if (_bTitleChange) {
        // terminal title change.
        printf("%c]0;%c", '\033', '\007');
    }
    return SUCCESS;
}
