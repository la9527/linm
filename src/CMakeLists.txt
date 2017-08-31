add_subdirectory(ncurses)

include_directories(${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs
        ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/ncurses )


########### install files ###############

#original Makefile.am contents follow:

#
#
## set the include path found by configure
#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel \
#	-I$(top_srcdir)/panel/common -I$(top_srcdir)/panel/vfs -I$(top_srcdir)/panel/vfs/ftp \
#	-I$(top_srcdir)/src -I$(top_srcdir)/src/ncurses $(all_includes)
#
## the library search path.
#
#
#SUBDIRS = ncurses
#
#
