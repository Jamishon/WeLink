cmake_minimum_required(VERSION 2.6)

project (WELINK_SERVER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
)

#find_library(MYSQL_LIB libmysqlclient.so /usr/lib64/mysql/)

set(server_srcs
main.cc
Log.cc
ConfigFile.cc
)

add_executable(server ${server_srcs})