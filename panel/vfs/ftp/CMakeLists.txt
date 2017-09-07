include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/panel/common ${CMAKE_SOURCE_DIR}/panel/vfs/ftp )

SET(linmpluginftp_LIB_INCLUDES
    ftplib.h ftpreader.h
)

SET(linmpluginftp_LIB_SRCS
   ftplib.c ftpreader.cpp
)

add_library(linmpluginftp STATIC ${linmpluginftp_LIB_SRCS} ${linmpluginftp_LIB_INCLUDES})
