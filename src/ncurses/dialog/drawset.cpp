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

#include "drawset.h"
#include "ncurses_dialog.h"
#include "subshell.h"

using namespace MLSUTIL;

namespace MLS {

    chtype HLINE = ACS_HLINE;        ///< 가로 그래픽 문자 '-'
    chtype VLINE = ACS_VLINE;        ///< 세로 그래픽 문자 '|'
    chtype ULCORNER = ACS_ULCORNER;    ///< 왼쪽 위 사각 귀뚱이 그래픽 문자
    chtype LLCORNER = ACS_LLCORNER;    ///< 왼쪽 아래 사각 귀뚱이 그래픽 문자
    chtype URCORNER = ACS_URCORNER;    ///< 오른쪽 위 사각 귀뚱이 그래픽 문자
    chtype LRCORNER = ACS_LRCORNER;    ///< 오른쪽 아래 사각 귀뚱이 그래픽 문자
    chtype LTEE = ACS_LTEE;            ///< ?????
    chtype RTEE = ACS_RTEE;            ///< ?????
    chtype BTEE = ACS_BTEE;            ///< 'ㅗ'그래픽 문자
    chtype TTEE = ACS_TTEE;            ///< 'ㅜ'그래픽 문자

    LINECODE e_nBoxLineCode = AUTOLINE;    ///< BOX LINE MODE

/// @brief	그래픽모드 라인이나 ASCII모드 라인을 설정한다.
    void Set_BoxLine(
            LINECODE nCode            ///< 그래픽모드, ASCII모드
    ) {
        if (nCode == AUTOLINE) {
            string sTerm = getenv("TERM");
            if (sTerm == "cygwin")
                nCode = CHARLINE;
            else
                nCode = ACSLINE;
        }

        if (nCode == CHARLINE) {
            HLINE = '-';
            VLINE = '|';
            ULCORNER = '+';
            LLCORNER = '+';
            URCORNER = '+';
            LRCORNER = '+';
            LTEE = '+';
            RTEE = '+';
            BTEE = '+';
            TTEE = '+';
            e_nBoxLineCode = CHARLINE;
        }

        if (nCode == ACSLINE) {
            HLINE = ACS_HLINE;
            VLINE = ACS_VLINE;
            ULCORNER = ACS_ULCORNER;
            LLCORNER = ACS_LLCORNER;
            URCORNER = ACS_URCORNER;
            LRCORNER = ACS_LRCORNER;
            LTEE = ACS_LTEE;
            RTEE = ACS_RTEE;
            BTEE = ACS_BTEE;
            TTEE = ACS_TTEE;
            e_nBoxLineCode = ACSLINE;
        }
    }

    static bool s_bnCursesInit = false;

    void setcol(int font, int back, WINDOW *win) {
        wattroff(win, A_BOLD | A_BLINK);

        if (font >= 8) wattron(win, A_BOLD);
        //if ( back >= 8 ) wattron(win, A_BLINK);

        wattron(win, COLOR_PAIR(((font % 8) << 3) + (back % 8)));
    }

/// @brief		consol 초기화 작업.
    void CursesInit(bool bTransparency) {
        initscr();     // nCurses 시작
        start_color(); // 컬러 가능하게

        nonl();        // 엔터 처리?
        raw();           // Ctrl+C : Signal무시 가능하게
        //cbreak();      // nCurses : 즉시 입력이 가능하도록 설정

        noecho();       // nCurses : echo모드 해제
        keypad(stdscr, TRUE);  // keypad 활성화

        curs_set(0); // 커서를 보이지 않게 한다.

        use_default_colors();
        assume_default_colors(-1, -1);

        // 색깔 쌍을 초기화 한다. 디폴트 포함 0 ~ 64개 까지
        for (int t = 0; t < 8; t++)
            for (int t2 = 0; t2 < 8; t2++) {
                if (t2 == 0 && bTransparency) // Transparency console support
                    init_pair(t * 8 + t2, t, -1);
                else
                    init_pair(t * 8 + t2, t, t2);
            }

        // line 설정
        Set_BoxLine(e_nBoxLineCode);

        // 처음 시작할 경우 clear, refresh 해줘야 한다.
        clear();
        refresh();

        ESCDELAY = 10;  // ESC 딜레이 줄이기.

        s_bnCursesInit = true;
    }

    void CursesDestroy() {
        clear();
        refresh();

        // . keypad 비활성화
        keypad(stdscr, FALSE);  // . 라인단위로 입력이 이루어지도록 설정
        //nocbreak();
        noraw();
        curs_set(1); // 커서를 다시 보이게 한다.
        echo();
        nl();
        endwin();   // . nCurses 해제

        s_bnCursesInit = false;
    }

    void CursesRestart(bool bTransparency) {
        CursesDestroy();
        CursesInit(bTransparency);
    }

    void MouseInit() {
        // curses 마우스 설정

        mousemask(BUTTON1_CLICKED |
                  BUTTON2_CLICKED |
                  BUTTON3_CLICKED |
                  #if NCURSES_MOUSE_VERSION > 1
                  BUTTON4_TRIPLE_CLICKED |
					BUTTON5_TRIPLE_CLICKED |
                  #endif
                  BUTTON_SHIFT |
                  BUTTON_CTRL, NULL);

        //mousemask(ALL_MOUSE_EVENTS, NULL);
        mouseinterval(10);
    }

    void MouseDestroy() {
        // curses 마우스 설정
        mousemask(0, NULL);
    }

    void ScreenClear() {
        clear();
        refresh();
    }

/// @brief 시그널 처리를 하는 함수
/// @param sig 시그널 번호
    void signal_action(int sig) {
        switch (sig) {
            case SIGINT:
                // Copy & paste를 위해서 Ctrl-C를 사용합니다.
                ungetch(3);
                return;
            case SIGPIPE:
                return;
            case SIGCHLD:
                g_SubShell.SigchldHandler();
                return;
            case SIGWINCH:
                if (!s_bnCursesInit)
                    g_SubShell.LowLevel_Resize();
                return;
        }
        CursesDestroy();

# if (__GNUC__ * 1000 + __GNUC_MINOR__) >= 3000
        LOG_WRITE("Signal [%d] [%s]", sig, strsignal(sig));
        printf("Signal [%d] [%s]\n", sig, strsignal(sig));
# else
        printf("Signal [%d]\n", sig);
# endif
        exit(0);
    }

/// @brief 시그널블럭 처리를 한다.
/// @return 성공할 경우 SUCCESS
    int Signal_Blocking() {
        struct sigaction act;

        // 다음은 시그널이 들어왔을 때 실행 함수
        act.sa_handler = signal_action;

        // 다음 주석을 해제 하면 시그널 무시
        //act.sa_handler = SIG_IGN;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        sigaction(SIGHUP, &act, 0);     /* 1 Hangup (POSIX).  */
        sigaction(SIGINT, &act, 0);     /* 2 Interrupt (ANSI).  */
        sigaction(SIGQUIT, &act, 0);    /* 3 Quit (POSIX).  */
        sigaction(SIGPIPE, &act, 0);    /* 13 Broken pipe (POSIX).  */
        sigaction(SIGCHLD, &act, 0);    /* 17 Child status has changed (POSIX).  */
//	sigaction(SIGWINCH, &act, 0);   /* 28 Window size change (4.3 BSD, Sun).  */

//#ifdef __DEBUGMODE__
//	sigaction(SIGILL, &act, 0);     /* 4 Illegal instruction (ANSI).  */
//	sigaction(SIGTRAP, &act, 0);    /* 5 Trace trap (POSIX).  */
//	sigaction(SIGABRT, &act, 0);    /* 6 Abort (ANSI).  */
//	sigaction(SIGBUS, &act, 0);     /* 7 BUS error (4.2 BSD).  */
//	sigaction(SIGFPE, &act, 0);     /* 8 Floating-point exception (ANSI).  */
//  sigaction(SIGKILL, &act, 0);    /* 9 Kill, unblockable (POSIX).  */
//	sigaction(SIGUSR1, &act, 0);    /* 10 User-defined signal 1 (POSIX).  */
//	sigaction(SIGSEGV, &act, 0);    /* 11 Segmentation violation (ANSI).  */
//	sigaction(SIGUSR2, &act, 0);    /* 12 User-defined signal 2 (POSIX).  */
//	sigaction(SIGALRM, &act, 0);    /* 14 Alarm clock (POSIX).  */
//	sigaction(SIGTERM, &act, 0);    /* 15 Termination (ANSI).  */
//  sigaction(SIGCHLD, &act, 0);    /* 17 Child status has changed (POSIX).  */
//	sigaction(SIGCONT, &act, 0);    /* 18 Continue (POSIX).  */
//	sigaction(SIGSTOP, &act, 0);    /* 19 Stop, unblockable (POSIX).  */
//	sigaction(SIGTSTP, &act, 0);    /* 20 Keyboard stop (POSIX).  */
//	sigaction(SIGTTIN, &act, 0);    /* 21 Background read from tty (POSIX).  */
//	sigaction(SIGTTOU, &act, 0);    /* 22 Background write to tty (POSIX).  */
//	sigaction(SIGURG, &act, 0);     /* 23 Urgent condition on socket (4.2 BSD).  */
//	sigaction(SIGXFSZ, &act, 0);    /* 24 CPU limit exceeded (4.2 BSD).  */
//	sigaction(SIGXCPU, &act, 0);    /* 25 File size limit exceeded (4.2 BSD).  */
//	sigaction(SIGVTALRM, &act, 0);  /* 26 File size limit exceeded (4.2 BSD).  */
//	sigaction(SIGPROF, &act, 0);    /* 27 Profiling alarm clock (4.2 BSD).  */
//	sigaction(SIGWINCH, &act, 0);   /* 28 Window size change (4.3 BSD, Sun).  */
//	sigaction(SIGIO, &act, 0);      /* 29 I/O now possible (4.2 BSD). Pollable event occurred (System V).*/

//	sigaction(SIGSYS, &act, 0);     /* 31 Bad system call. (Unused) */

//	#ifdef LINUX
//	sigaction(SIGSTKFLT, &act, 0);   /* 16 Stack fault.  */
//	sigaction(SIGPWR, &act, 0);      /* 30 Power failure restart (System V).  */
//	#endif
//#endif
        return 0;
    }

// SubShell, Teminal Size Signal setting.
// gcc 2.9 compile failure fix.
    static void *s_OldAct = NULL;

    void Signal_ResizeBlocking() {
        struct sigaction act;

        if (!s_OldAct)
            s_OldAct = new (struct sigaction);

        act.sa_handler = signal_action;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        /* 28 Window size change (4.3 BSD, Sun).  */
        sigaction(SIGWINCH, &act, (struct sigaction *) s_OldAct);
    }

    void Signal_ResizeUnblocking() {
        // ncurses SIGWINCH 의 이전 설정으로 돌린다.
        sigaction(SIGWINCH, (struct sigaction *) s_OldAct, 0);
    }

};
