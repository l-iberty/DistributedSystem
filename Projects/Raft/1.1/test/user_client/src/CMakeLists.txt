cmake_minimum_required(VERSION 3.9)
project(user_client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp TCP.cpp TCP.h TCPClient.cpp TCPClient.h font_properties.h)
add_executable(user_client ${SOURCE_FILES})
