
########### install files ###############

add_custom_command(OUTPUT linm.sh
        COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm.sh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm.sh )
add_custom_command(OUTPUT linm.csh
        COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm.csh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm.csh )
add_custom_command(OUTPUT linm_alias.sh
        COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm_alias.sh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.sh )
add_custom_command(OUTPUT linm_alias.csh
        COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm_alias.csh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.csh )

add_custom_target(linmScript DEPENDS linm.sh linm.csh linm_alias.csh linm_alias.sh)

install(FILES colorset.cfg default.cfg keyset.cfg DESTINATION etc )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/linm.sh
        ${CMAKE_CURRENT_BINARY_DIR}/linm.csh
        ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.sh
        ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.csh DESTINATION bin )

add_dependencies(linm linmScript)

#original Makefile.am contents follow:

#suppbindir = $(bindir)
#
#noinst_DATA = colorset.cfg default.cfg keyset.cfg linm.csh.in linm.sh.in \
#	linm_alias.csh.in linm_alias.sh.in qtkeyset.cfg
#					
#noinst_SCRIPTS = linm_alias.sh linm_alias.csh
#suppbin_SCRIPTS = linm.sh linm.csh 
#
#CLEANFILES = linm.sh linm_alias.sh linm.csh linm_alias.csh
#
#EXTRA_DIST = $(noinst_DATA)              
#
#linm.sh: $(srcdir)/linm.sh.in
#	sed "s%@""bindir@%$(bindir)%" $(srcdir)/linm.sh.in > linm.sh
#	
#linm.csh: $(srcdir)/linm.csh.in
#	sed "s%@""bindir@%$(bindir)%" $(srcdir)/linm.csh.in > linm.csh
#		
#linm_alias.sh: $(srcdir)/linm_alias.sh.in
#	sed "s%@""bindir@%$(bindir)%" $(srcdir)/linm_alias.sh.in > linm_alias.sh
#	
#linm_alias.csh: $(srcdir)/linm_alias.csh.in
#	sed "s%@""bindir@%$(bindir)%" $(srcdir)/linm_alias.csh.in > linm_alias.csh
