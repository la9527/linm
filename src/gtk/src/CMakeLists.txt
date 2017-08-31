
########### next target ###############

SET(linmgtkpanel_SRCS
   main.c
   support.c
   support.h
   interface.c
   interface.h
   callbacks.c
   callbacks.h
)

add_executable(linmgtkpanel ${linmgtkpanel_SRCS})

target_link_libraries(linmgtkpanel)

install(TARGETS linmgtkpanel DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

### Process this file with automake to produce Makefile.in
#
#INCLUDES = \
#	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
#	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
#	@PACKAGE_CFLAGS@
#
#bin_PROGRAMS = linmgtkpanel
#
#linmgtkpanel_SOURCES = \
#	main.c \
#	support.c support.h \
#	interface.c interface.h \
#	callbacks.c callbacks.h
#
#linmgtkpanel_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)
#
