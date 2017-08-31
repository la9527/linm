include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/panel/common ${CMAKE_SOURCE_DIR}/panel/vfs/ftp )


########### next target ###############

SET(linmpluginftp_LIB_SRCS
   ftplib.c ftpreader.cpp
)

link_directories()

add_library(linmpluginftp STATIC ${linmpluginftp_LIB_SRCS})
#set_target_properties(linmpluginftp PROPERTIES VERSION 4.2.0 SOVERSION 4)

#target_link_libraries(linmpluginftp)
install(TARGETS linmpluginftp DESTINATION lib)


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel/common \
#	-I$(top_srcdir)/panel/vfs/ftp
#METASOURCES = AUTO
#noinst_HEADERS = ftplib.h ftpreader.h
#lib_LTLIBRARIES = liblinmpluginftp.la
#liblinmpluginftp_la_SOURCES = ftpreader.cpp ftplib.c
#
