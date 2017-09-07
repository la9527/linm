include_directories(
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive
        ${CMAKE_SOURCE_DIR}/panel/vfs/ftp
        ${CMAKE_SOURCE_DIR}/panel/vfs/libssh2 )

if (NOT DISABLE_SFTP AND OPENSSL_FOUND)
    message( STATUS "openssl include directory : ${OPENSSL_INCLUDE_DIR} ${LIBSSH_LIBRARY_INCLUDE_DIRS}" )
    include_directories(${OPENSSL_INCLUDE_DIR} ${LIBSSH_LIBRARY_INCLUDE_DIRS})

    SET(linmpluginsftp_LIB_INCLUDES
            SFtpReader.H
            )

    SET(linmpluginsftp_LIB_SRCS
       SFtpReader.cpp
    )

    add_library(linmpluginsftp STATIC ${linmpluginsftp_LIB_SRCS} ${linmpluginsftp_LIB_INCLUDES})
else()
    message( WARNING "Unsupported sftp connect" )
endif()
