if ( OPENSSL_FOUND AND LIBSSH2_LIBRARIES )
include_directories(
        ${CMAKE_SOURCE_DIR}/libfswatch/c++
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs )

SET(ssh_test_SRCS ssh2_sample.cpp)

add_executable(ssh_test ${ssh_test_SRCS} ${linm_INCLUDES})
target_link_libraries(ssh_test mlscommon vfsplugin dialog editor panelcommon linmpluginftp ${LIBICONV} ${LIBINTL} ${NCURSES_LIB})

add_dependencies(ssh_test linmpluginsftp)
target_link_libraries(ssh_test linmpluginsftp ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} ${LIBSSH2}  )

    if ( SOURCEHIGHLIGHT )
        target_link_libraries(ssh_test ${SOURCEHIGHLIGHT} )
    endif ( SOURCEHIGHLIGHT )
endif ()

