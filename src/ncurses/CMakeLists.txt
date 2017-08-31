add_subdirectory(dialog)

include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive
        ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
        ${CMAKE_SOURCE_DIR}/panel/vfs/libssh2
        ${CMAKE_SOURCE_DIR}/editor
        ${CMAKE_SOURCE_DIR}/src/ncurses/dialog
        ${CMAKE_SOURCE_DIR}/src/ncurses )


########### next target ###############

SET(linm_SRCS
   cmd_common_imp.cpp
   cmd_editor_imp.cpp
   cmd_mcd_imp.cpp
   cmd_panel_chown.cpp
   cmd_panel_find.cpp
   cmd_panel_imp.cpp
   cmd_settingchg.cpp
   command.cpp
   main_ncurces.cpp
   mainframe.cpp
   mainframe_view.cpp
   mlsmenu.cpp
   ncurses_dialog.cpp
   ncurses_editor.cpp
   ncurses_mcd.cpp
   ncurses_panel.cpp
   ncurses_qcd.cpp
   subshell.cpp
)

add_executable(linm ${linm_SRCS})

target_link_libraries(linm mlscommon vfsplugin dialog editor panelcommon linmpluginarc linmpluginftp "${NCURSES_LIB}")
if ( OPENSSL_FOUND )
    target_link_libraries(linm ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})
endif()

install(TARGETS linm DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel \
#	-I$(top_srcdir)/panel/common -I$(top_srcdir)/panel/vfs -I$(top_srcdir)/panel/vfs/archive \
#	-I$(top_srcdir)/panel/vfs/ftp -I$(top_srcdir)/panel/vfs/libssh2 -I$(top_srcdir)/editor \
#	-I$(top_srcdir)/src/ncurses/dialog \
#	-I$(top_srcdir)/src/ncurses $(all_includes)
#
#METASOURCES = AUTO
#
#SUBDIRS = dialog
#
#bin_PROGRAMS = linm
#
#noinst_HEADERS = ncurses_panel.h ncurses_mcd.h mainframe_view.h mainframe.h command.h mlsmenu.h \
#        ncurses_editor.h cmd_editor_imp.h cmd_mcd_imp.h cmd_panel_imp.h ncurses_dialog.h \
#        cmd_common_imp.h ncurses_qcd.h subshell.h
#
#linm_SOURCES = cmd_common_imp.cpp cmd_editor_imp.cpp cmd_mcd_imp.cpp \
#	cmd_panel_chown.cpp cmd_panel_find.cpp cmd_panel_imp.cpp cmd_settingchg.cpp command.cpp \
#	main_ncurces.cpp mainframe.cpp mainframe_view.cpp mlsmenu.cpp ncurses_dialog.cpp \
#	ncurses_editor.cpp ncurses_mcd.cpp ncurses_panel.cpp ncurses_qcd.cpp subshell.cpp
#
#linm_LDADD = $(top_builddir)/lib/libmlscommon.a \
#	$(top_builddir)/panel/vfs/libvfsplugin.a \
#	$(top_builddir)/src/ncurses/dialog/libdialog.a \
#	$(top_builddir)/editor/libeditor.a \
#	$(top_builddir)/panel/common/libpanelcommon.a \
#	$(top_builddir)/panel/vfs/archive/liblinmpluginarc.la \
#	$(top_builddir)/panel/vfs/ftp/liblinmpluginftp.la \
#	@LIBINTL@
#
#if !LINM_UNUSE_SFTP
#
#linm_LDADD += $(top_builddir)/panel/vfs/libssh2/liblinmpluginsftp.la 
#
#endif
#
#if !LINM_UNUSE_SAMBA
#
#linm_LDADD += $(top_builddir)/panel/vfs/samba/liblinmpluginsmb.la 
#
#endif
#
