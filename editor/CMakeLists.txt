include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/panel/common )


########### next target ###############

SET(editor_STAT_SRCS
   editor.cpp
   editorexecute.cpp
)

add_library(editor STATIC ${editor_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel/common
#METASOURCES = AUTO
#noinst_LIBRARIES = libeditor.a
#libeditor_a_SOURCES = editor.cpp editorexecute.cpp
#noinst_HEADERS = editor.h editorclipboard.h
