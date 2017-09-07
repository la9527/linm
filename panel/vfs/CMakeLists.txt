include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive
        ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
        ${CMAKE_SOURCE_DIR}/panel/vfs/libssh2
        ${CMAKE_SOURCE_DIR}/panel/vfs/samba )

add_subdirectory(ftp)
add_subdirectory(archive)
if (NOT DISABLE_SFTP)
    add_subdirectory(libssh2)
endif()
if (NOT DISABLE_SAMBA)
    add_subdirectory(samba)
endif()

########### next target ###############

SET(vfsplugin_STAT_HEADERS dirreader.h vfsplugin.h)
SET(vfsplugin_STAT_SRCS dirreader.cpp vfsplugin.cpp)

add_library(vfsplugin STATIC ${vfsplugin_STAT_SRCS})

########### install files ###############






#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#SUBDIRS = ftp archive libssh2 samba
#
#noinst_HEADERS = dirreader.h vfsplugin.h
#libvfsplugin_a_SOURCES = dirreader.cpp vfsplugin.cpp
#noinst_LIBRARIES = libvfsplugin.a
#
#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel/common \
#	-I$(top_srcdir)/panel/vfs -I$(top_srcdir)/panel/vfs/archive -I$(top_srcdir)/panel/vfs/ftp \
#	-I$(top_srcdir)/panel/vfs/libssh2 -I$(top_srcdir)/panel/vfs/samba
#
