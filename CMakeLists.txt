cmake_minimum_required(VERSION 3.7)
project(tempserver_rmt_cpp)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp OneWireSensor.cpp OneWireSensor.h TalkToServer.cpp TalkToServer.h structs.h)
add_executable(tempserver_rmt_cpp ${SOURCE_FILES})