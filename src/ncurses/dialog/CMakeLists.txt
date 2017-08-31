include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/panel ${CMAKE_SOURCE_DIR}/panel/common ${CMAKE_SOURCE_DIR}/src/ncurses ${CMAKE_SOURCE_DIR}/src/ncurses/dialog ${CMAKE_BINARY_DIR} )


########### next target ###############

SET(dialog_STAT_SRCS
   dialog.cpp
   dialog_button.cpp
   dialog_chkradio.cpp
   dialog_form.cpp
   dialog_input.cpp
   dialog_label.cpp
   dialog_menu.cpp
   dialog_remoteconnprops.cpp
   dialog_replace.cpp
   dialog_text.cpp
   dialogbox.cpp
   drawset.cpp
   keyreader.cpp
   dialog_config.cpp
)

add_library(dialog STATIC ${dialog_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel -I$(top_srcdir)/panel/common -I$(top_srcdir)/src/ncurses
#
#noinst_LIBRARIES = libdialog.a
#libdialog_a_SOURCES = dialog.cpp dialog_button.cpp dialog_chkradio.cpp \
#	dialog_form.cpp dialog_input.cpp dialog_label.cpp dialog_menu.cpp \
#	dialog_remoteconnprops.cpp dialog_replace.cpp dialog_text.cpp dialogbox.cpp drawset.cpp \
#	keyreader.cpp dialog_config.cpp
#
#noinst_HEADERS = dialog.h drawset.h keyreader.h dialogbox.h dialog_menu.h dialog_remoteconnprops.h dialog_config.h dialog_replace.h
#
