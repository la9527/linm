if (UNIX)
    add_custom_command(OUTPUT linm.sh
            COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm.sh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm.sh )
    add_custom_command(OUTPUT linm.csh
            COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm.csh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm.csh )
    add_custom_command(OUTPUT linm_alias.sh
            COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm_alias.sh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.sh )
    add_custom_command(OUTPUT linm_alias.csh
            COMMAND sed "s%@bindir@%${CMAKE_INSTALL_PREFIX}/bin%" ${CMAKE_CURRENT_SOURCE_DIR}/linm_alias.csh.in > ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.csh )

    add_custom_target(linmScript DEPENDS linm.sh linm.csh linm_alias.csh linm_alias.sh)

    install(FILES colorset.cfg default.cfg keyset.cfg syntexset.cfg DESTINATION etc )

    install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/linm.sh
                ${CMAKE_CURRENT_BINARY_DIR}/linm.csh
                ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.sh
                ${CMAKE_CURRENT_BINARY_DIR}/linm_alias.csh
            DESTINATION bin )
endif (UNIX)

add_dependencies(linm linmScript)
