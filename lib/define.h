///	\file	define.h
///	\brief	define.h

#ifndef __DEFINE_H__
#define __DEFINE_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#include <signal.h>

#include <map>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#ifdef HAVE_GETTEXT
#include <libintl.h>
#include <wchar.h>
#include <wctype.h>

#define _(Text)	gettext(Text)
#else
#define _(Text)    Text
#endif

#ifdef __CYGWIN_C__
#define wstring string
#endif

#if (__GNUC__ * 1000 + __GNUC_MINOR__) < 3000
#include <wchar.h>
#include <wctype.h>

typedef basic_string<wchar_t> wstring;
#endif

#include "mlslog.h"

using namespace std;

typedef unsigned int uint;    ///< unsigned int
typedef long long ullong;    ///< use the file size

#define        ERROR    -1        ///< ERROR
#define        SUCCESS 0        ///< SUCCESS

enum McdExeMode {
    MCD_EXEMODE_NONE,
    ARCHIVE_COPY,
    EXECUTE_QCD
};

enum ViewType {
    COMMON,
    PANEL,
    MCD,
    EDITOR
};

enum ClipState {
    CLIP_NONE,
    CLIP_COPY,
    CLIP_CUT
};

#endif // _DEFINE_H_
