########### next target ###############

SET(mlscommon_STAT_HEADERS define.h exception.h mlslocale.h strutil.h configure.h mlslog.h mlsmutex.h cmdshell.h mlsthread.h strlinetoken.h mlsdialog.h)

SET(mlscommon_STAT_SRCS
   exception.cpp
   mlslocale.cpp
   configure.cpp
   strutil.cpp
   mlslog.cpp
   mlsmutex.cpp
   mlsthread.cpp
   mlsdialog.cpp
)

add_library(mlscommon STATIC ${mlscommon_STAT_SRCS} ${mlscommon_STAT_HEADERS})

########### install files ###############

#original Makefile.am contents follow:

#INCLUDES = -I. -I.. -I../intl -I$(top_srcdir)/intl
#METASOURCES = AUTO
#noinst_HEADERS = define.h exception.h mlslocale.h strutil.h configure.h \
#	mlslog.h mlsmutex.h cmdshell.h mlsthread.h strlinetoken.h mlsdialog.h
#noinst_LIBRARIES = libmlscommon.a
#libmlscommon_a_SOURCES = exception.cpp mlslocale.cpp configure.cpp strutil.cpp \
#		mlslog.cpp mlsmutex.cpp mlsthread.cpp mlsdialog.cpp
#
#localedir = $(datadir)/locale
#DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
