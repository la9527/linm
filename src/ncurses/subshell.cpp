#include <signal.h>		/* sigaction(), sigprocmask(), etc.   */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "subshell.h"
#include "drawset.h"

using namespace std;
using namespace MLS;
using namespace MLSUTIL;

enum {READ=0, WRITE=1};

#ifndef STDIN_FILENO
#    define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#    define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#    define STDERR_FILENO 2
#endif

#define KEY_M_CTRL  0x4000
#define XCTRL(x) (KEY_M_CTRL | ((x) & 31))

#define	FORK_FAILURE	63

SubShell &SubShell::GetInstance()
{
	static SubShell instance;
	return instance;
}

SubShell::SubShell()
{
	_nSubShellPty = 0;
	_eSubShellState = ACTIVE;
	_bSubShellReady = false;
	_bSubShellStopped = false;
	_bSubShellAlive = false;
	_nSubShellPid = -1;
	_nCtrlKey = XCTRL('o') & 255;

	char	cHostName[100];
	memset(&cHostName, 0, sizeof(cHostName));
	if ( gethostname((char*)&cHostName, sizeof(cHostName)) != -1 )
	{
		_sHostName = cHostName;
		if (_sHostName.find(".") != string::npos)
			_sHostName = _sHostName.substr(0, _sHostName.find("."));
	}
}

SubShell::~SubShell()
{
}

ssize_t		SubShell::Write(int fd, const void *buf, size_t count)
{
	ssize_t ret = 0;
	ssize_t written = 0;
	while (count > 0) 
	{
		ret = write (fd, (const char*)buf+ret, count);
		if (ret < 0) 
		{
			if (errno == EINTR) 
				continue;
			else 
				return written > 0 ? written : ret;
		}
		count -= ret;
		written += ret;
	}
    return written;
}

#if defined(HAVE_GRANTPT) && (__GNUC__ * 1000 + __GNUC_MINOR__) >= 3000

/* System V version of pty_open_master */

int	SubShell::PtyOpenMaster(string& sPtyName)
{
    char*	cSlaveName;
    int 	nPtyMaster;

#ifdef HAVE_POSIX_OPENPT
    nPtyMaster = posix_openpt(O_RDWR);
#elif HAVE_GETPT
    /* getpt () is a GNU extension (glibc 2.1.x) */
    nPtyMaster = getpt ();
#elif IS_AIX
    sPtyName = "/dev/ptc";
    nPtyMaster = open (sPtyName.c_str(), O_RDWR);
#else
	sPtyName = "/dev/ptmx";
    nPtyMaster = open (sPtyName.c_str(), O_RDWR);
#endif

    if ( nPtyMaster == -1)
    	return ERROR;

	if (	grantpt (nPtyMaster) == -1        /* Grant access to slave */
	    || 	unlockpt (nPtyMaster) == -1        /* Clear slave's lock flag */
	    || 	!(cSlaveName = ptsname (nPtyMaster)))  /* Get slave's name */
    {
	    close (nPtyMaster);
	    return -1;
    }

	sPtyName = cSlaveName;
    return nPtyMaster;
}

/* System V version of pty_open_slave */
int SubShell::PtyOpenSlave(const string& sPtyName)
{
    int nPtySlave = open (sPtyName.c_str(), O_RDWR);

    if (nPtySlave == -1) {
	    LOG( "open (%s, O_RDWR): %s", sPtyName.c_str(), strerror(errno));
	    return ERROR;
    }
#if !defined(__osf__) && !defined(__linux__)
#if defined (I_FIND) && defined (I_PUSH)
    if (!ioctl (nPtySlave, I_FIND, "ptem"))
    if (ioctl (nPtySlave, I_PUSH, "ptem") == -1) {
    	LOG( "ioctl (%d, I_PUSH, \"ptem\") failed: %s",, sPtyName.c_str(), strerror(errno));
        close (nPtySlave);
        return ERROR;
    }

    if (!ioctl (nPtySlave, I_FIND, "ldterm"))
	    if (ioctl (nPtySlave, I_PUSH, "ldterm") == -1) 
	    {
	    	LOG("ioctl (%d, I_PUSH, \"ldterm\") failed: %s", nPtySlave, strerror(errno));
	        close (nPtySlave);
	        return ERROR;
	    }
#if !defined(sgi) && !defined(__sgi)
    if (!ioctl (nPtySlave, I_FIND, "ttcompat"))
	    if (ioctl (nPtySlave, I_PUSH, "ttcompat") == -1) 
	    {
	    	LOG("ioctl (%d, I_PUSH, \"ttcompat\") failed: %s", nPtySlave, strerror(errno));
	        close (nPtySlave);
	        return ERROR;
	    }
#endif              /* sgi || __sgi */
#endif              /* I_FIND && I_PUSH */
#endif              /* __osf__ || __linux__ */
    return nPtySlave;
}

#else // HAVE_GRANTPT

int	SubShell::PtyOpenMaster(string& sPtyName)
{
	int		nPtyMaster;
	char	*cPtr1, *cPtr2;
	
	sPtyName = "/dev/ptyXX";
	
    for (cPtr1 = "pqrstuvwxyzPQRST"; *cPtr1; ++cPtr1)
    {
		sPtyName[8] = *cPtr1;
		for (cPtr2 = "0123456789abcdef"; *cPtr2; ++cPtr2)
		{
		    sPtyName[9] = *cPtr2;
	
		    /* Try to open master */
		    if ((nPtyMaster = open (sPtyName.c_str(), O_RDWR)) == -1) 
		    {
				if (errno == ENOENT)  /* Different from EIO */
				    return -1;	      /* Out of pty devices */
				else
				    continue;	      /* Try next pty device */
		    }
		    sPtyName[5] = 't';	      /* Change "pty" to "tty" */
		    if (access (sPtyName.c_str(), 6))
		    {
				close (nPtyMaster);
				sPtyName[5] = 'p';
				continue;
			}
	    	return nPtyMaster;
	    }
	}
    return -1;  /* Ran out of pty devices */
}

int SubShell::PtyOpenSlave(const string& sPtyName)
{
	int nPtySlave;
	
	if ((nPtySlave = open (sPtyName.c_str(), O_RDWR)) == -1)
		fprintf (stderr, "open (sPtyName.c_str(), O_RDWR): %s\r\n", sPtyName.c_str());
    return nPtySlave;
}

#endif // HAVE_GRANTPT

void	SubShell::LowLevel_Resize()
{
#if defined TIOCSWINSZ
    struct winsize winsz;

    winsz.ws_col = winsz.ws_row = 0;
    /* Ioctl on the STDIN_FILENO */
    ioctl (0, TIOCGWINSZ, &winsz);
    if (winsz.ws_col && winsz.ws_row)
	{
        COLS = winsz.ws_col;
        LINES = winsz.ws_row;
    }

	if (_nSubShellPty != 0)
		Resize_tty( _nSubShellPty );
#endif
}

int SubShell::Resize_tty (int fd)
{
#if defined TIOCSWINSZ
    struct winsize tty_size;

    tty_size.ws_row = LINES;
    tty_size.ws_col = COLS;
    tty_size.ws_xpixel = tty_size.ws_ypixel = 0;

    return ioctl (fd, TIOCSWINSZ, &tty_size);
#else
    return 0;
#endif
}

void	SubShell::InitSubShellChild(const string& sPtyName, const string& sDir)
{
	int		nPtySlave;
	string	sInitFile;
	
	pid_t	sidLinM;
	
	for ( int n = 0; n < 10; n++ )
		cout << "[InitSubShellChild]---\n";
	
	setsid();
	
	nPtySlave = PtyOpenSlave(sPtyName);
	close (_nSubShellPty);

	#ifdef TIOCSCTTY
	    ioctl (nPtySlave, TIOCSCTTY, 0);
	#endif

/*  The follow lines is ncurses bug. Will need review.
	if ( tcgetattr (STDOUT_FILENO, &_ShellMode) )
	{
		LOG("Cannot get terminal settings: %s\r\n", strerror(errno));
	}
	_ShellMode.c_lflag |= TOSTOP;
	if (tcsetattr (nPtySlave, TCSANOW, &_ShellMode))
    {
		fprintf (stderr, "Cannot set pty terminal modes: %s\r\n", strerror(errno));
		_exit (-1);
    }
*/
	Resize_tty (nPtySlave);
	
    chdir (sDir.c_str());
	#if (__GNUC__ * 1000 + __GNUC_MINOR__) >= 3000
	sidLinM = getsid (0);
	#else
	sidLinM = -1;
	#endif
    
    if (sidLinM != -1) 
    {
		String	strSid;
		strSid.Printf("%ld", (long)sidLinM);
		LOG("InitSubShellChild [%s]", strSid.c_str());
		setenv ("LINM_SID", strSid.c_str(), 1 );
		setenv ("MC_SID", strSid.c_str(), 1 );	// Avoid conflict with MC(Midnight Commander).
    }
    
	sInitFile = "~/.linm/bashrc";
	if (access (sInitFile.c_str(), R_OK) == -1)
	    sInitFile = "~/.bashrc";
	
	for ( int n = 0; n < 10; n++ )
		cout << "[InitSubShellChild]---\n";

	dup2 (nPtySlave, STDIN_FILENO);
    dup2 (nPtySlave, STDOUT_FILENO);
    dup2 (nPtySlave, STDERR_FILENO);
	
    close (_nSubshellPipe[READ]);
    close (nPtySlave);

	string sPS1 = "\\u@\\h:\\w\\$ ";
	setenv ("PS1", sPS1.c_str(), sPS1.size() );
	
	execl ("/bin/bash", "bash", "-rcfile", sInitFile.c_str(), (char *) NULL);
    _exit (FORK_FAILURE);
}

int		SubShell::CheckSid (void)
{
	pid_t 		mySid, oldSid;
	const char 	*sSID;

	sSID = getenv ("LINM_SID" );

	if (!sSID) 
	{
		sSID = getenv ("MC_SID" ); // Avoid conflict with MC(Midnight Commander).
		if ( !sSID ) return SUCCESS;
	}

	oldSid = (pid_t) strtol( sSID, NULL, 0);
	if (!oldSid)	
		return SUCCESS;

	#if (__GNUC__ * 1000 + __GNUC_MINOR__) >= 3000
	mySid = getsid (0);
	#else
	mySid = -1;
	#endif
	if (mySid == -1)
		return SUCCESS;

	/* The parent LINM is in a different session, it's OK */
	LOG("CheckSid [%d] [%d]", (int)mySid, (int)oldSid);
	//if (oldSid != mySid)	return ERROR;
	
	//printf(_("LinM is already runnning on this terminal. Subshell support will be disabled.\n"));
	return ERROR;
}

static struct termios raw_mode;

void	SubShell::InitRowMode()
{
	static int initialized = 0;

	/* LINM calls reset_shell_mode() in pre_exec() to set the real tty to its */
	/* original settings.  However, here we need to make this tty very raw, */
	/* so that all keyboard signals, XON/XOFF, etc. will get through to the */
	/* pty.  So, instead of changing the code for execute(), pre_exec(),    */
	/* etc, we just set up the modes we need here, before each command.     */

	if (initialized == 0)  /* First time: initialise `raw_mode' */
	{
		tcgetattr (STDOUT_FILENO, &raw_mode);		
		raw_mode.c_lflag &= ~ICANON;  /* Disable line-editing chars, etc.   */
		raw_mode.c_lflag &= ~ISIG;    /* Disable intr, quit & suspend chars */
		raw_mode.c_lflag &= ~ECHO;    /* Disable input echoing		    */
		raw_mode.c_iflag &= ~IXON;    /* Pass ^S/^Q to subshell undisturbed */
		raw_mode.c_iflag &= ~ICRNL;   /* Don't translate CRs into LFs	    */
		raw_mode.c_oflag &= ~OPOST;   /* Don't postprocess output	    */
		raw_mode.c_cc[VTIME] = 0;     /* IE: wait forever, and return as    */
		raw_mode.c_cc[VMIN] = 1;      /* soon as a character is available   */
		initialized = 1;
	}
}

int		SubShell::InitSubShell(const string& sDir)
{
	if (CheckSid () == ERROR) return ERROR;

	InitRowMode();

	string		sPtyName;
	int			nPtySlave = 0;
	
    if (_nSubShellPty == 0)
	{
		// SubShell Initialize
		_nSubShellPty = PtyOpenMaster(sPtyName);
		if ( _nSubShellPty == ERROR)
		{
			LOG("Cannot open master side of pty: %s \n", strerror( errno ));
			return ERROR;
		}
			
		nPtySlave = PtyOpenSlave(sPtyName);
		if ( nPtySlave == ERROR )
		{
			LOG("Cannot open slave side of pty %s: %s\n", strerror( errno ));
			return ERROR;
		}
			
		if (pipe (_nSubshellPipe)) 
		{
		    LOG("couldn't create pipe");
		    return ERROR;
		}
	}

	_bSubShellAlive = true;
    _bSubShellStopped = false;
	
	_nSubShellPid = fork();
	
	if ( _nSubShellPid == -1 )
	{
		LOG("Error.. fork failure !!!");
		return ERROR;
	}
	
	if (_nSubShellPid == 0) {	/* We are in the child process */		
		InitSubShellChild(sPtyName, sDir);
    }

    /* pty_slave is only opened when called the first time */
    if (nPtySlave != -1) 
		close (nPtySlave);
	
	LOG("InitSubShell Fork [%d]", (int)_nSubShellPid);

    /* Wait until the subshell has started up and processed the command */
    _eSubShellState = RUNNING_COMMAND;

	return SUCCESS;
}

void	SubShell::SetDir_SubShell(const string& sDir)
{
	if (_nSubShellPty != 0)
	{
		string	sCmd;
		if ( !sDir.empty() )
			sCmd = "cd " + sDir + "\n";
		
		Write (_nSubShellPty, " \n", 2);
		Write (_nSubShellPty, sCmd.c_str(), sCmd.size());
		FeedSubShell(QUIETLY, true );
		Write (_nSubShellPty, " \b", 2);
	}
}

int	SubShell::FeedSubShell(int nHow, bool bOnError)
{
	CheckSid();
	
	tcsetattr (STDOUT_FILENO, TCSANOW, &raw_mode);
	
	fd_set	Read_Set;
	int		nMaxFdp, nBytes, i, nRt;
	
	for (;;)
    {
		struct	timeval		tvTime, *pTimePtr;
	
		tvTime.tv_sec = 10;
		tvTime.tv_usec = 0;
		pTimePtr = bOnError ? &tvTime : NULL;

		if (!_bSubShellAlive)
		    return ERROR;

		/* Prepare the file-descriptor set and call `select' */
		FD_ZERO (&Read_Set);
		FD_SET (_nSubShellPty, &Read_Set);
		FD_SET (_nSubshellPipe[READ], &Read_Set);
		nMaxFdp = max (_nSubShellPty, _nSubshellPipe[READ]);
		if (nHow == VISIBLY)
		{
		    FD_SET (STDIN_FILENO, &Read_Set);
		    nMaxFdp = max (nMaxFdp, STDIN_FILENO);
		}
		
		nRt =  select (nMaxFdp + 1, &Read_Set, NULL, NULL, pTimePtr);
		
		if (nRt == -1) 
		{
		    /* Despite using SA_RESTART, we still have to check for this */
		    if (errno == EINTR) continue;	/* try all over again */
		    LOG("select (FD_SETSIZE, &read_set...): %s\r\n", strerror( errno ));
		    return ERROR;
		}
		
		//LOG("FeedSubShell select !!!");
		
		if (FD_ISSET (_nSubShellPty, &Read_Set))
	    /* Read from the subshell, Write to stdout */

	    /* This loop improves performance by reducing context switches
	       by a factor of 20 or so... unfortunately, it also hangs MC
	       randomly, because of an apparent Linux bug.  Investigate. */
		{
			char	cPtyBuffer[1024];
			memset(cPtyBuffer, 0, sizeof(cPtyBuffer));
		    nBytes = read (_nSubShellPty, cPtyBuffer, sizeof(cPtyBuffer));
	
		    /* The subshell has died */
		    if (nBytes == -1 && errno == EIO && !_bSubShellAlive)
				return ERROR;

			cPtyBuffer[nBytes] = 0;	/* Squash the final '\n' */
	
		    if (nBytes <= 0) 
		    {
				LOG("read (subshell_pty...): %s\n", strerror(errno));
				return ERROR;
		    }

			if ( cPtyBuffer[0] )
			{
				// get prompt
				StringToken		tToken( cPtyBuffer, "\r\n" );
				while( tToken.Next() )
				{
					string sTmp = tToken.Get();
					if (getbetween(sTmp, '@', ':') == _sHostName )
					{
						_sPrompt = sTmp;
						break;
					}
				}
			}
			
			//LOG("FeedSubShell WRITE [%s] !!!\n", cPtyBuffer);
			if (nHow == VISIBLY)
				Write (STDOUT_FILENO, cPtyBuffer, nBytes);
			else
				break;
		}
		else if (FD_ISSET (_nSubshellPipe[READ], &Read_Set))
	    /* Read the subshell's CWD and capture its prompt */
		{
			char	cSubShellCwd[1024];
			nBytes = read (_nSubshellPipe[READ], cSubShellCwd, sizeof(cSubShellCwd));
		    if (nBytes <= 0) 
		    {
				LOG("read (subshell_pipe[READ]...): %s\r\n", strerror (errno));
				return ERROR;
		    }	
		    cSubShellCwd[nBytes] = 0;
			//LOG("FeedSubShell _nSubshellPipe1[READ] [%s] !!!", cSubShellCwd);
		    Synchronize ();
	
			_bSubShellReady = true;
		    if (_eSubShellState == RUNNING_COMMAND) 
			{
				_eSubShellState = INACTIVE;
				return SUCCESS;
		    }
		}
		else if (FD_ISSET (STDIN_FILENO, &Read_Set))
	    /* Read from stdin, Write to the subshell */
		{
			char	cPtyBuffer[1024];
			nBytes = read (STDIN_FILENO, cPtyBuffer, sizeof(cPtyBuffer));
		    if (nBytes <= 0)
		    {
				LOG("read (STDIN_FILENO, pty_buffer...): %s\r\n", strerror(errno));
				return ERROR;
		    }

			cPtyBuffer[nBytes] = 0;
		    
		    for (i = 0; i < nBytes; ++i)
		    {
				if (cPtyBuffer[i] == _nCtrlKey)
				{
				    Write (_nSubShellPty, cPtyBuffer, i);
				    if (_bSubShellReady)
						_eSubShellState = INACTIVE;
				    return SUCCESS;
				}
			}

			Write (_nSubShellPty, cPtyBuffer, nBytes);
		    _bSubShellReady = false;
		} 
		else 
		{
			LOG("select timeout !!!");
		    return ERROR;
		}
	}
	return SUCCESS;
}

void		SubShell::Synchronize()
{
	sigset_t tSigChildMask, tOldMast;

	sigemptyset (&tSigChildMask);

	sigaddset (&tSigChildMask, SIGCHLD);
	sigprocmask (SIG_BLOCK, &tSigChildMask, &tOldMast);
	
    /*
     * SIGCHLD should not be blocked, but we unblock it just in case.
     * This is known to be useful for cygwin 1.3.12 and older.
     */
    sigdelset (&tOldMast, SIGCHLD);

	/* Wait until the subshell has stopped */
    while (_bSubShellAlive && !_bSubShellStopped)
	{
		sigsuspend (&tOldMast);
	}

	/* Discard all remaining data from stdin to the subshell */
	if (_eSubShellState != ACTIVE) {
        /* Discard all remaining data from stdin to the subshell */
        tcflush (_nSubShellPty, TCOFLUSH);
    }
    
	_bSubShellStopped = false;
    kill (_nSubShellPid, SIGCONT);

	_nSubShellPty = 0;
	_nSubShellPid = -1;

    sigprocmask (SIG_SETMASK, &tOldMast, NULL);
    /* We can't do any better without modifying the shell(s) */
}

// Main ( Signal Child )
void	SubShell::SigchldHandler()
{
    int 	nStatus = 0;
    pid_t 	pidWaitSubShell = 0;

	pidWaitSubShell = waitpid (_nSubShellPid, &nStatus, WUNTRACED | WNOHANG);

	if (pidWaitSubShell == _nSubShellPid)
    {
		// Figure out what has happened to the subshell 
		if (WIFSTOPPED (nStatus))
		{
		    if (WSTOPSIG (nStatus) == SIGSTOP)
		    {
				// The subshell has received a SIGSTOP signal 
				_bSubShellStopped = true;
		    } 
		    else 
		    {
				// The user has suspended the subshell.  Revive it 
				kill (_nSubShellPid, SIGCONT);
		    }
		} 
		else 
		{
		    // The subshell has either exited normally or been killed 
		    _bSubShellAlive = false;
		    if (WIFEXITED (nStatus) && WEXITSTATUS (nStatus) != FORK_FAILURE)
			{
				//QUIT
				//quit |= SUBSHELL_EXIT;	// Exited normally.
			}
		}
		_nSubShellPty = 0;
		_nSubShellPid = -1;
    }
}

const string& SubShell::GetPrompt_SubShell()
{
	if (_nSubShellPty != 0)
	{
		Write (_nSubShellPty, " \n", 2);
		FeedSubShell(QUIETLY, true );
		Write (_nSubShellPty, " \b", 2);
	}
	return _sPrompt;
}
