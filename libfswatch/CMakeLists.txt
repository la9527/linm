#
# Copyright (c) 2014-2016 Enrico M. Crisostomo
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#

add_definitions(-DHAVE_CONFIG_H)

set(LIB_SOURCE_FILES
        ./c/cevent.cpp
        ./c/cevent.h
        ./c/cfilter.h
        ./c/cmonitor.h
        ./c/error.h
        ./c/libfswatch.cpp
        ./c/libfswatch.h
        ./c/libfswatch_log.cpp
        ./c/libfswatch_log.h
        ./c/libfswatch_types.h
        ./c++/event.cpp
        ./c++/event.hpp
        ./c++/filter.hpp
        ./c++/filter.cpp
        ./c++/libfswatch_exception.cpp
        ./c++/libfswatch_exception.hpp
        ./c++/libfswatch_map.hpp
        ./c++/libfswatch_set.hpp
        ./c++/monitor.cpp
        ./c++/monitor.hpp
        ./c++/path_utils.cpp
        ./c++/path_utils.hpp
        ./c++/poll_monitor.cpp
        ./c++/poll_monitor.hpp
        ./c++/string/string_utils.cpp
        ./c++/string/string_utils.hpp)

INCLUDE(CheckIncludeFiles)

CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)

if (HAVE_SYS_INOTIFY_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            ./c++/inotify_monitor.cpp
            ./c++/inotify_monitor.hpp)
endif (HAVE_SYS_INOTIFY_H)

CHECK_INCLUDE_FILES(sys/event.h HAVE_SYS_EVENT_H)

if (HAVE_SYS_EVENT_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            ./c++/kqueue_monitor.cpp
            ./c++/kqueue_monitor.hpp)
endif (HAVE_SYS_EVENT_H)

CHECK_INCLUDE_FILES(port.h HAVE_PORT_H)

if (HAVE_PORT_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            ./c++/fen_monitor.cpp
            ./c++/fen_monitor.hpp)
endif (HAVE_PORT_H)

if (WIN32)
    CHECK_INCLUDE_FILES(sys/cygwin.h HAVE_CYGWIN)

    if (HAVE_CYGWIN)
        set(LIB_SOURCE_FILES
                ${LIB_SOURCE_FILES}
                ./c++/windows/win_directory_change_event.cpp
                ./c++/windows/win_directory_change_event.hpp
                ./c++/windows/win_error_message.cpp
                ./c++/windows/win_error_message.hpp
                ./c++/windows/win_handle.cpp
                ./c++/windows/win_handle.hpp
                ./c++/windows/win_paths.cpp
                ./c++/windows/win_paths.hpp
                ./c++/windows/win_strings.cpp
                ./c++/windows/win_strings.hpp
                ./c++/windows_monitor.cpp
                ./c++/windows_monitor.hpp)
        add_definitions(-DHAVE_WINDOWS)
    endif (HAVE_CYGWIN)
endif (WIN32)

if (APPLE)
    CHECK_INCLUDE_FILES(CoreServices/CoreServices.h HAVE_FSEVENTS_FILE_EVENTS)
    message( STATUS "HAVE_FSEVENTS_FILE_EVENTS: ${HAVE_FSEVENTS_FILE_EVENTS}")
    if (HAVE_FSEVENTS_FILE_EVENTS)
        find_library(CORESERVICES_LIBRARY CoreServices)
        set(EXTRA_LIBS ${CORESERVICES_LIBRARY})

        set(LIB_SOURCE_FILES
                ${LIB_SOURCE_FILES}
                ./c++/fsevents_monitor.cpp
                ./c++/fsevents_monitor.hpp)

    endif (HAVE_FSEVENTS_FILE_EVENTS)
endif (APPLE)

CHECK_STRUCT_HAS_MEMBER("struct stat" "st_mtime" "sys/stat.h" HAVE_STRUCT_STAT_ST_MTIME)
CHECK_STRUCT_HAS_MEMBER("struct stat" "st_mtimespec" "sys/stat.h" HAVE_STRUCT_STAT_ST_MTIMESPEC)

check_function_exists (atexit HAVE_ATEXIT)
check_function_exists (CFLocaleCopyCurrent HAVE_CFLOCALECOPYCURRENT)
check_function_exists (CFPreferencesCopyAppValue HAVE_CFPREFERENCESCOPYAPPVALUE)

CHECK_INCLUDE_FILE (CoreServices/CoreServices.h HAVE_CORESERVICES_CORESERVICES_H)

CHECK_INCLUDE_FILE_CXX (atomic HAVE_CXX_ATOMIC)
CHECK_INCLUDE_FILE_CXX (mutex HAVE_CXX_MUTEX)

add_library(fswatch STATIC ${LIB_SOURCE_FILES})

include_directories( ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/lib )
#target_link_libraries(fswatch ${CORESERVICES_LIBRARY}  ${LIBICONV} ${LIBINTL})


