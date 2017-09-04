include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs
        ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive )

########### next target ###############

if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

SET(panelcommon_STAT_HEADERS colorcfgload.h colorentry.h file.h mcd.h history.h mlscfgload.h mpool.h panel.h passencrypt.h reader.h readerctl.h remotecfgload.h selection.h sortfile.h syntexcfgload.h)

SET(panelcommon_STAT_SRCS
   colorcfgload.cpp
   history.cpp
   keycfgload.cpp
   mcd.cpp
   mlscfgload.cpp
   panel.cpp
   passencrypt.cpp
   remotecfgload.cpp
   selection.cpp
   syntexcfgload.cpp
)

add_library(panelcommon STATIC ${panelcommon_STAT_SRCS} ${panelcommon_STAT_HEADERS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel \
#	-I$(top_srcdir)/panel/common -I$(top_srcdir)/panel/vfs -I$(top_srcdir)/panel/vfs/ftp \
#	-I$(top_srcdir)/panel/vfs/archive
#METASOURCES = AUTO
#noinst_LIBRARIES = libpanelcommon.a
#libpanelcommon_a_SOURCES = colorcfgload.cpp history.cpp keycfgload.cpp mcd.cpp \
#	mlscfgload.cpp panel.cpp passencrypt.cpp reader.cpp remotecfgload.cpp selection.cpp
#noinst_HEADERS = colorcfgload.h colorentry.h keycfgload.h mlscfgload.h file.h \
#			sortfile.h reader.h readerctl.h panel.h mcd.h mpool.h selection.h history.h \
#	remotecfgload.h passencrypt.h
#
