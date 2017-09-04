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

namespace MLS
{

chtype HLINE=ACS_HLINE;		///< horizone char. '-'
chtype VLINE=ACS_VLINE;		///< vertical char. '|'
chtype ULCORNER=ACS_ULCORNER;	///< left up corner char.
chtype LLCORNER=ACS_LLCORNER;	///< left bottom corner char.
chtype URCORNER=ACS_URCORNER;	///< right up corner char.
chtype LRCORNER=ACS_LRCORNER;	///< right down corner char.
chtype LTEE=ACS_LTEE;			///< left tree char.
chtype RTEE=ACS_RTEE;			///< right tree char.
chtype BTEE=ACS_BTEE;			///< Botton tree char.
chtype TTEE=ACS_TTEE;			///< Bottom tree char.

LINECODE	e_nBoxLineCode = AUTOLINE;	///< BOX LINE MODE

static Curses_Dialog*		s_pDialog = NULL;
static Curses_Progress*		s_pProgressBox = NULL;

/// @brief	set the graphic mode or ascii box line.
void Set_BoxLine(
		LINECODE nCode			///< graphic char mode, ascii mode
	)
{
	if (nCode == AUTOLINE)
	{
		string	sTerm = getenv("TERM");	
		if (sTerm == "cygwin")
			nCode = CHARLINE;
		else
			nCode = ACSLINE;	
	}	

	switch(nCode)
	{
		case CHARLINE:
			HLINE='-';
			VLINE='|';
			ULCORNER='+';
			LLCORNER='+';
			URCORNER='+';
			LRCORNER='+';
			LTEE='+';
			RTEE='+';
			BTEE='+';
			TTEE='+';
			e_nBoxLineCode = CHARLINE;
			break;

		case ACSLINE:
			HLINE=ACS_HLINE;
			VLINE=ACS_VLINE;
			ULCORNER=ACS_ULCORNER;
			LLCORNER=ACS_LLCORNER;
			URCORNER=ACS_URCORNER;
			LRCORNER=ACS_LRCORNER;
			LTEE=ACS_LTEE;
			RTEE=ACS_RTEE;
			BTEE=ACS_BTEE;
			TTEE=ACS_TTEE;
			e_nBoxLineCode = ACSLINE;

		default:
			break;
	}
}

static	bool	s_bnCursesInit = false;

void setcol(int font, int back, WINDOW *win)
{
	wattroff(win, A_BOLD | A_BLINK);

	if ( font >= 8 ) wattron(win, A_BOLD);
	//if ( back >= 8 ) wattron(win, A_BLINK);

	wattron( win, COLOR_PAIR( ((font%8) << 3) + (back%8)) );
}

/// @brief		consol initialize.
void CursesInit( bool  bTransparency )
{
	initscr();     // start NCurses.
	start_color(); // use color

	//nonl();        // enter
	raw();		   // use for copy, paste(Ctrl+C). set ignore signal.
	//cbreak();      // nCurses : incontinently input.

	noecho();	   // nCurses : noecho
	keypad(stdscr, TRUE);  // use keypad

	curs_set(0); // unview cursor.

	use_default_colors();
	assume_default_colors(-1, -1);
	
	// colors initialize. 0 ~ 64 values.
	for (int t=0; t<8; t++)
		for (int t2=0; t2<8; t2++)
	{
		if ( t2 == 0 && bTransparency ) // Transparency console support
			init_pair( t*8+t2, t, -1 );
		else
			init_pair( t*8+t2, t, t2 );
	}

	// line setting.
	Set_BoxLine(e_nBoxLineCode);

	// The first time, need the clear(), refresh().
	clear();
	refresh();

	ESCDELAY = 10;  // reduce ESC delay.

	if ( !s_pDialog )
		s_pDialog = new Curses_Dialog;

	if ( !s_pProgressBox )
		s_pProgressBox = new Curses_Progress;

	// Dialog, Progress class are change to ncurses.
	MLSUTIL::SetDialogProgress( (MlsDialog*)s_pDialog , (MlsProgress*)s_pProgressBox );
	s_bnCursesInit = true;
}

void CursesDestroy()
{
	clear();
	refresh();
	
	// unuse keypad 
	keypad(stdscr, TRUE);  // set input the line unit.
	
	nocbreak();
	echo();
	nl();
	curs_set(1); // cursor view.
	noraw();
	while( endwin() != OK ) // end NCurses
		sleep(1);
	
	echo();
	s_bnCursesInit = false;
}

void	CursesRestart( bool bTransparency )
{
	CursesDestroy();
	CursesInit( bTransparency );
}

void	MouseInit()
{
	// curses mouse setting.
	
	mousemask( 	BUTTON1_CLICKED |
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

void	MouseDestroy()
{
	// curses mouse setting.
	mousemask(0, NULL);
}

void	ScreenClear()
{
	clear();
	refresh();
}

/// @brief signal action function.
/// @param sig signal number
void signal_action(int sig)
{
	switch(sig)
	{
		case SIGINT:
			// use ctrl+c for copy and paste.
			ungetch(3);
			return;
		case SIGPIPE:
			return;
		case SIGCHLD:
			g_SubShell.SigchldHandler();
			return;
		case SIGWINCH:
			if ( !s_bnCursesInit )
				g_SubShell.LowLevel_Resize();
			return;
	}
	CursesDestroy();

# if (__GNUC__ * 1000 + __GNUC_MINOR__) >= 3000
	LOG_WRITE( "Signal [%d] [%s]", sig, strsignal(sig));
	printf("Signal [%d] [%s]\n", sig, strsignal(sig));
# else
	printf("Signal [%d]\n", sig);
# endif
	exit(0);
}

/// @brief signal blocking function
/// @return if succcess, SUCCESS is return.
int Signal_Blocking()
{
	struct sigaction act;

	// if recieved signal, signal_action() call.
	act.sa_handler = signal_action;

	// if not use under line comment, ignore signal.
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
static 	void*	s_OldAct = NULL;

void	Signal_ResizeBlocking()
{
	struct sigaction act;

	if ( !s_OldAct )
		s_OldAct = new (struct sigaction);

	act.sa_handler = signal_action;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	/* 28 Window size change (4.3 BSD, Sun).  */
	sigaction(SIGWINCH, &act, (struct sigaction*)s_OldAct);
}

void	Signal_ResizeUnblocking()
{
	// Go to previous setting of SIGWINCH.
	sigaction(SIGWINCH, (struct sigaction*)s_OldAct, 0);
}

};
