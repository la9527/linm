cmake_minimum_required(VERSION 3.0)
project (linm)

set(PACKAGE "linm")
set(VERSION "0.9.1")
set(CMAKE_CXX_STANDARD 11)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

message( STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message( STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

# disallow in-source build
include(MacroEnsureOutOfSourceBuild)
macro_ensure_out_of_source_build("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# header or library check.

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckTypeSize)
include(CheckStructHasMember)
include(CheckIncludeFileCXX)

# Boost library find
# set(Boost_USE_STATIC_LIBS        ON) # only find static libs
# set(Boost_USE_MULTITHREADED      ON)
# find_package(Boost 1.50.0 COMPONENTS program_options signals system)

# if (NOT Boost_FOUND)
#    message(FATAL_ERROR "'boost' library is not found. please install 'boost' library.(http://www.boost.org/)")
#else()
#    include_directories(${Boost_INCLUDE_DIRS})
#    message( STATUS "BOOST INCLUDE : ${Boost_INCLUDE_DIRS}")
#    message( STATUS "BOOST LIBS : ${Boost_LIBRARIES}")
#endif()

CHECK_INCLUDE_FILES(errno.h HAVE_ERRNO_H)
CHECK_INCLUDE_FILES(fcntl.h HAVE_FCNTL_H)

check_function_exists (alloca HAVE_ALLOCA)
check_function_exists (asprintf HAVE_ASPRINTF)

CHECK_INCLUDE_FILES(alloca.h HAVE_ALLOCA_H)
CHECK_INCLUDE_FILES(argz.h HAVE_ARGZ_H)

check_function_exists (CFLocaleCopyCurrent HAVE_CFLOCALECOPYCURRENT)
check_function_exists (CFPreferencesCopyAppValue HAVE_CFPREFERENCESCOPYAPPVALUE)

CHECK_INCLUDE_FILES (curses.h HAVE_CURSES_H)

check_function_exists (getopt getopt_long HAVE_DECL_GETOPT)

check_function_exists (_snprintf HAVE_DECL__SNPRINTF)
check_function_exists (_snwprintf HAVE_DECL__SNWPRINTF)

CHECK_INCLUDE_FILES(argz.h HAVE_ARGZ_H)

check_function_exists (vprintf HAVE_DOPRNT)
check_function_exists (fwprintf HAVE_FWPRINTF)
check_function_exists (getegid HAVE_GETEGID)
check_function_exists (geteuid HAVE_GETEUID)
check_function_exists (getgid HAVE_GETGID)

check_function_exists (getopt HAVE_GETOPT)
check_function_exists (getopt_long HAVE_GETOPT_LONG)

check_function_exists (getpt HAVE_GETPT)
check_function_exists (gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists (getuid HAVE_GETUID)

check_function_exists (grantpt HAVE_GRANTPT)
check_function_exists (intmax_t HAVE_INTMAX_T)
CHECK_INCLUDE_FILES (inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES (inttypes.h HAVE_INTTYPES_H_WITH_UINTMAX)
CHECK_INCLUDE_FILES (langinfo.h HAVE_LANGINFO_CODESET)

CHECK_LIBRARY_EXISTS (pt "" "" "HAVE_LIBPT")
CHECK_INCLUDE_FILES (pthread.h PTHREAD_ENABLE)
CHECK_INCLUDE_FILES (limits.h HAVE_LIMITS_H)

CHECK_INCLUDE_FILES (locale.h libintl.h HAVE_LC_MESSAGES)
CHECK_INCLUDE_FILES (locale.h HAVE_LOCALE_H)

CHECK_TYPE_SIZE("long double" HAVE_LONG_DOUBLE)
CHECK_TYPE_SIZE("long long" HAVE_LONG_LONG)
CHECK_TYPE_SIZE("lstat" HAVE_LSTAT_EMPTY_STRING_BUG)

check_function_exists (malloc HAVE_MALLOC)
CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES (memory.h HAVE_MEMORY_H)
check_function_exists (mempcpy HAVE_MEMPCPY)
check_function_exists (memset HAVE_MEMSET)
check_function_exists (mkdir HAVE_MKDIR)
check_function_exists (mmap HAVE_MMAP)

check_function_exists (munmap HAVE_MUNMAP)
CHECK_INCLUDE_FILES (ncurses.h HAVE_NCURSES_H)
CHECK_INCLUDE_FILES (ndir.h HAVE_NDIR_H)
CHECK_INCLUDE_FILES (nl_types.h HAVE_NL_TYPES_H)

check_function_exists (poll HAVE_POLL)
check_function_exists (posix_openpt HAVE_POSIX_OPENPT)
check_function_exists (printf HAVE_POSIX_PRINTF)

check_function_exists (putenv HAVE_PUTENV)
check_function_exists (select HAVE_SELECT)
check_function_exists (setenv HAVE_SETENV)
check_function_exists (setlocale HAVE_SETLOCALE)
check_function_exists (snprintf HAVE_SNPRINTF)
check_function_exists (stat HAVE_STAT_EMPTY_STRING_BUG)

CHECK_INCLUDE_FILES (stdbool.h HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES (stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES (stdint.h sys/types.h HAVE_STDINT_H_WITH_UINTMAX)

CHECK_INCLUDE_FILES (stdio.h HAVE_STDIO_H)
CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)

check_function_exists (stpcpy HAVE_STPCPY)
check_function_exists (strcasecmp HAVE_STRCASECMP)
check_function_exists (strdup HAVE_STRDUP)

CHECK_INCLUDE_FILES (string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES (strings.h HAVE_STRINGS_H)

check_function_exists (strtol HAVE_STRTOL)
check_function_exists (strtoul HAVE_STRTOUL)

CHECK_INCLUDE_FILES (sys/dir.h HAVE_SYS_DIR_H)
CHECK_INCLUDE_FILES (sys/param.h HAVE_SYS_PARAM_H)

CHECK_INCLUDE_FILES (sys/select.h HAVE_SYS_SELECT_H)
CHECK_INCLUDE_FILES (sys/socket.h HAVE_SYS_SOCKET_H)

CHECK_INCLUDE_FILES (sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES (sys/types.h HAVE_SYS_TYPES_H)

CHECK_INCLUDE_FILES (sys/uio.h HAVE_SYS_UIO_H)
CHECK_INCLUDE_FILES (sys/wait.h HAVE_SYS_WAIT_H)

CHECK_TYPE_SIZE ("uintmax_t" HAVE_UINTMAX_T)
CHECK_INCLUDE_FILES (unistd.h HAVE_UNISTD_H)

CHECK_TYPE_SIZE("unsigned long long" HAVE_UNSIGNED_LONG_LONG)
check_function_exists (vprintf HAVE_VPRINTF)

CHECK_TYPE_SIZE("wchar_t" HAVE_WCHAR_T)

check_function_exists (wcslen HAVE_WCSLEN)

set(_FILE_OFFSET_BITS 64)
set(_LARGEFILE_SOURCE TRUE)

include(GNUInstallDirs)

add_definitions("-DHAVE_CONFIG_H")
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/intl ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} )

########### find iconv library

find_library(LIBICONV iconv)
find_path(ICONV_H iconv.h PATHS /usr/local/opt/libiconv/include )

if (NOT LIBICONV OR NOT ICONV_H)
    message("Please install 'iconv' library. (https://ftp.gnu.org/pub/gnu/libiconv/)")
    message(FATAL_ERROR "'iconv' library is not found.")
else()
    set(HAVE_ICONV "TRUE")
    set(ICONV_CONST "TRUE")
    message( STATUS "iconv: ${LIBICONV}")
endif()

########### find intl(gettext) library

find_library(LIBINTL intl)
if (NOT LIBINTL)
    message(FATAL_ERROR "'intl' library is not found. please install 'gettext' library.(https://www.gnu.org/software/gettext)")
else()
    find_path(PATH_LIBINTL_H libintl.h PATHS /usr/local/include )
    include_directories( ${PATH_LIBINTL_H} )
    set(HAVE_GETTEXT "TRUE")
    add_definitions( -DLOCALEDIR=\"${CMAKE_INSTALL_LOCALEDIR}\" )
    message( STATUS "intl: ${LIBINTL}")
endif()

########### set configure file

option(LINM_CFGPATH "set configure file install path. (default: '${CMAKE_INSTALL_PREFIX}/etc/linm')")

if(LINM_CFGPATH)
    set(__LINM_CFGPATH__ "${LINM_CFGPATH}")
else()
    set(__LINM_CFGPATH__ "${CMAKE_INSTALL_PREFIX}/etc/linm")
endif(LINM_CFGPATH)

message( STATUS "Option - configure path : '${__LINM_CFGPATH__}'" )

########### set debug mode

option(DEBUG_MODE "Build debugging mode")
if( DEBUG_MODE )
    set(__DEBUGMODE__ "ON" )
endif(DEBUG_MODE)

message( STATUS "Option - debug mode : ${DEBUG_MODE}" )

########### setting tar name
option(TAR_FILE_NAME "set 'tar' filename. (default: 'tar')")

if (TAR_FILE_NAME)
    set(TARNAME ${TAR_FILE_NAME})
else()
    find_program(GTAR gtar)
    find_program(TAR tar)

    if (GTAR)
        set(TARGNAME "gtar")
    else()
        set(TARGNAME "tar")
    endif(GTAR)
endif(TAR_FILE_NAME)

message(  STATUS  "Option - tar or gtar program name : '${TARGNAME}'" )

########### find ncurses library

find_library(CURSES_LIB curses)
find_library(NCURSES_LIB ncurses)
find_library(NCURSESW_LIB ncursesw)

if(NOT NCURSESW_LIB AND NOT NCURSES_LIB AND NOT NCURSESW_LIB)
    message("")
    message("Need to the ncurces library.")
    message("")
    message(" debian or ubuntu : ")
    message("    apt-get install libncursesw5-dev libncurses5-dev")
    message(" redhat or fedora : ")
    message("    yum install ncurses-devel")
    message(" other  : ")
    message("    Please refer to http://www.gnu.org/software/ncurses")
    message("")
    message(FATAL_ERROR "'curses' or 'ncurses' library is not found.")
else()
    message(STATUS "find CURSES_LIB: ${CURSES_LIB}")
    message(STATUS "find NCURSES_LIB: ${NCURSES_LIB}")
    message(STATUS "find NCURSESW_LIB: ${NCURSESW_LIB}")
endif()

########### find openssl library
option(ENABLE_SFTP "Enable support for SFTP protocol. (default: OFF)" OFF)
if( ENABLE_SFTP )
    if (EXISTS "/usr/local/opt/openssl")
        set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
    endif()
    if (EXISTS "/usr/local/openssl")
        set(OPENSSL_ROOT_DIR /usr/local/openssl)
    endif()

    include(FindOpenSSL)

    find_library(SOCKET socket)
    find_library(MLIB ceil)

    include(FindPkgConfig)
    if ( PKG_CONFIG_EXECUTABLE )
        pkg_check_modules(LIBSSH2 REQUIRED libssh2)
        find_library(LIBSSH2 ssh2 PATHS ${LIBSSH2_LIBRARY_DIRS})
    else ( PKG_CONFIG_EXECUTABLE )
        find_library(LIBSSH2_LIBRARIES ssh2)
        #find_library(LIBSSH2_LIBRARY_INCLUDE_DIRS ssh2)
    endif( PKG_CONFIG_EXECUTABLE )

    message(STATUS "Option - SFTP protocol : ON")

    if ( OPENSSL_FOUND AND LIBSSH2_LIBRARIES )
        set(__LINM_SFTP_USE__ ON)
    else()
        message("")
        message("Unable to find openssl, libssh2 libraries.")
        message("")
        message(WARNING "Not support sftp protocol.")
    endif()
else()
    message(STATUS "Option - SFTP protocol : OFF")
endif()

option(ENABLE_SAMBA "Enable support for Microsoft SMB protocol. (default: OFF)")
if( ENABLE_SAMBA )
    message(STATUS "Option - SMB protocol : ON")
    find_library(SMBCLIENT_FOUND smbclient)
    if ( SMBCLIENT_FOUND )
        set(__LINM_SAMBA_USE__ ON)
    endif( SMBCLIENT_FOUND )
else()
    message(STATUS "Option - SMB protocol : OFF")
endif()

option(ENABLE_SOURCE_HIGHLIGHT "Enable support for source highlight. (default: ON)" ON)
if( ENABLE_SOURCE_HIGHLIGHT )
    message(STATUS "Option - SOURCE HIGHLIGHT : ON")
    find_library(SOURCEHIGHLIGHT source-highlight)

    find_path(PATH_HIGHLIGHT_H srchilite/highlightstate.h PATHS /usr/local/include )
    include_directories( ${PATH_HIGHLIGHT_H} )
    if ( SOURCEHIGHLIGHT )
        set(__LINM_SOURCE_HIGHLIGHT_USE__ ON)
    endif( SOURCEHIGHLIGHT )
else()
    message(STATUS "Option - SOURCE HIGHLIGHT : OFF")
endif()

# subdirectory

#add_subdirectory(po)

add_subdirectory(libfswatch)
add_subdirectory(lib)
add_subdirectory(panel)
add_subdirectory(editor)
add_subdirectory(src)
add_subdirectory(sh)

if ( DEBUG_MODE )
    add_subdirectory(test)
endif( DEBUG_MODE )

# make config.h
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/config.h)

message("============================================")
message("              Build Status")
message("")
if ( DEBUG_MODE )
    message("- Debug mode : ${DEBUG_MODE}" )
endif()
if ( NOT OPENSSL_FOUND AND NOT LIBSSH2_LIBRARIES )
    message("- Unsupport sftp protocol.")
endif()
if( NOT ENABLE_SAMBA )
    message("- Unsupport samba protocol.")
endif()
if( NOT ENABLE_SOURCE_HIGHLIGHT )
    message("- Unsupport source highlight.")
endif()
message("============================================")
########### install files ###############

# install(FILES README.md README.ko DESTINATION share )
