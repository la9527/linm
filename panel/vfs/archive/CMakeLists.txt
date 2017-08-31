include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive )


########### next target ###############

SET(linmpluginarc_LIB_SRCS
   archive.cpp
   arcreader.cpp
)

add_library(linmpluginarc STATIC ${linmpluginarc_LIB_SRCS})

# target_link_libraries(linmpluginarc)
# set_target_properties(linmpluginarc PROPERTIES VERSION 4.2.0 SOVERSION 4)
# install(TARGETS linmpluginarc DESTINATION lib)

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel/common \
#	-I$(top_srcdir)/panel/vfs/archive
#METASOURCES = AUTO
#
#lib_LTLIBRARIES = liblinmpluginarc.la
#liblinmpluginarc_la_SOURCES = archive.cpp arcreader.cpp
#
#noinst_HEADERS = archive.h arcreader.h
#
