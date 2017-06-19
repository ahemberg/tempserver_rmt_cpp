cmake_minimum_required(VERSION 3.7)
project(tempserver_rmt_cpp)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp lib/OneWireSensor.cpp lib/OneWireSensor.h lib/TalkToServer.cpp lib/TalkToServer.h lib/structs.h lib/pch.h lib/dbFunctions.cpp lib/dbFunctions.h lib/cl_parser.cpp lib/cl_parser.h)
add_executable(tempserver_rmt_cpp ${SOURCE_FILES})