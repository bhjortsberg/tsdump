cmake_minimum_required(VERSION 3.0)
project(tsparse)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp TransportStream.cpp TransportStream.h TSPacket.cpp TSPacket.h AdaptionField.cpp AdaptionField.h PESHeader.cpp PESHeader.h)
add_executable(tsparse ${SOURCE_FILES})
