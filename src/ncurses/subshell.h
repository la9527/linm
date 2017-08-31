#ifndef __SUBSHELL_H__
#define __SUBSHELL_H__

#include <termios.h>	/* tcgetattr(), struct termios, etc.  */

#include "define.h"
#include "drawset.h"
#include "mlsdialog.h"

namespace MLS {

    enum ENUM_SUBSHELL_STATE {
        INACTIVE, ACTIVE, RUNNING_COMMAND
    };
    enum {
        QUIETLY, VISIBLY
    };

    class SubShell {
    public:
        SubShell();

        ~SubShell();

        static SubShell &GetInstance();

        void SigchldHandler();

        int InitSubShell(const string &sDir);

        int FeedSubShell(int nHow, bool bOnError);

        void SubShellGetConsoleAttr();

        bool IsAlive() { return _bSubShellAlive; }

        int CheckSid();

        void LowLevel_Resize();

        const string &GetPrompt_SubShell();

        void SetDir_SubShell(const string &sDir);

    protected:
        void InitRowMode();

        int PtyOpenMaster(string &sPtyName);

        int PtyOpenSlave(const string &sPtyName);

        void InitSubShellChild(const string &sPtyName, const string &sDir);

        void Synchronize();

        ssize_t Write(int fd, const void *buf, size_t count);

        int Resize_tty(int fd);

    private:
        int _nSubShellPty;
        string _sHomeDir;
        string _sHostName;
        string _sPrompt;

        struct termios _ShellMode;
        int _nSubshellPipe[2];

        ENUM_SUBSHELL_STATE _eSubShellState;
        bool _bSubShellReady;
        bool _bSubShellStopped;
        bool _bSubShellAlive;
        int _nSubShellPid;
        int _nCtrlKey;
    };

#define    g_SubShell    SubShell::GetInstance()

};

#endif
