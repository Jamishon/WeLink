cmake_minimum_required(VERSION 3.3)

project (WELINK_SERVER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -g -Wall -O0 -Wno-unused-variable -pthread")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
)

#find_library(MYSQL_LIB libmysqlclient.so /usr/lib64/mysql/)
find_library(LOG4CPLUS_LIB liblog4cplus.so ${PROJECT_SOURCE_DIR}/lib/)
if(NOT LOG4CPLUS_LIB)
    MESSAGE(FATAL_ERROR "liblog4cplus.so not found")
endif(NOT LOG4CPLUS_LIB)

set(server_srcs
src/main.cc
src/log/LogWrapper.cc
src/ConfigFile.cc
src/channel.cc
src/epoller.cc
src/socket.cc
src/tcpserver.cc
src/channel_connect.cc
src/channel_listen.cc
src/connection.cc
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)
add_executable(server ${server_srcs})
TARGET_LINK_LIBRARIES(server log4cplus)