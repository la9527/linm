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
    message( STATUS "Unsupport SAMBA" )
endif()

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel/common 	-I$(top_srcdir)/panel/vfs -I$(top_srcdir)/panel/vfs/archive -I$(top_srcdir)/panel/vfs/ftp 	-I$(top_srcdir)/panel/vfs/libssh2
#METASOURCES = AUTO
#
#if !LINM_UNUSE_SAMBA
#
#lib_LTLIBRARIES = liblinmpluginsmb.la
#liblinmpluginsmb_la_SOURCES = SMBReader.cpp
#
#endif
#
#noinst_HEADERS = SMBReader.h
#
