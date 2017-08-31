add_subdirectory(common)
add_subdirectory(vfs)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/panel ${CMAKE_SOURCE_DIR}/panel/common ${CMAKE_SOURCE_DIR}/panel/vfs )


########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#SUBDIRS = common vfs
#INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/panel \
#	-I$(top_srcdir)/panel/common -I$(top_srcdir)/panel/vfs
