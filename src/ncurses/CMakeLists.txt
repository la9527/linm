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

SET(
    linm_INCLUDES
        ncurses_panel.h ncurses_mcd.h mainframe_view.h mainframe.h command.h mlsmenu.h
        ncurses_editor.h cmd_editor_imp.h cmd_mcd_imp.h cmd_panel_imp.h ncurses_dialog.h
        cmd_common_imp.h ncurses_qcd.h subshell.h FSWatchDetect.h
)

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
   FSWatchDetect.cpp)

if ( SOURCEHIGHLIGHT_FOUND )
    SET( linm_SRCS ${linm_SRCS} src_highlight.cpp )
endif()

add_executable(linm ${linm_SRCS} ${linm_INCLUDES})
target_link_libraries(linm libfswatch mlscommon vfsplugin dialog editor panelcommon linmpluginarc linmpluginftp ${LIBICONV} ${LIBINTL} ${NCURSES_LIB})

if ( OPENSSL_FOUND )
    add_dependencies(linm linmpluginsftp)
    target_link_libraries(linm linmpluginsftp ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${LIBSSH2}  )
endif()

if ( SOURCEHIGHLIGHT_FOUND )
    target_link_libraries(linm ${SOURCEHIGHLIGHT} )
endif()

install(TARGETS linm DESTINATION bin)
