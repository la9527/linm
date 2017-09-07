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
if (ENABLE_SFTP)
    add_subdirectory(libssh2)
endif()
if (NOT DISABLE_SAMBA)
    add_subdirectory(samba)
endif()

SET(vfsplugin_STAT_HEADERS dirreader.h vfsplugin.h)
SET(vfsplugin_STAT_SRCS dirreader.cpp vfsplugin.cpp)

add_library(vfsplugin STATIC ${vfsplugin_STAT_SRCS})
