include_directories(
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/panel/common
        ${CMAKE_SOURCE_DIR}/panel/vfs/archive )

SET(linmpluginarc_LIB_SRCS
   archive.cpp
   arcreader.cpp
)

add_library(linmpluginarc STATIC ${linmpluginarc_LIB_SRCS})
