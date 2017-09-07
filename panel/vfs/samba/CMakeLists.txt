include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/panel/common ${CMAKE_SOURCE_DIR}/panel/vfs ${CMAKE_SOURCE_DIR}/panel/vfs/archive ${CMAKE_SOURCE_DIR}/panel/vfs/ftp ${CMAKE_SOURCE_DIR}/panel/vfs/libssh2 )

########### next target ###############

if( NOT DISABLE_SAMBA AND SMBCLIENT_FOUND )
    SET(linmpluginsmb_LIB_SRCS
       SMBReader.cpp
    )

    add_library(linmpluginsmb STATIC ${linmpluginsmb_LIB_SRCS})

    #target_link_libraries(linmpluginsmb)
    #set_target_properties(linmpluginsmb PROPERTIES VERSION 4.2.0 SOVERSION 4)
    #install(TARGETS linmpluginsmb DESTINATION lib)
else()
    message( WARNING "Unsupported smb connection" )
endif()

