cmake_minimum_required(VERSION 3.0)
project(tsparse)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp TransportStream.cpp TransportStream.h TSPacket.cpp TSPacket.h AdaptionField.cpp AdaptionField.h PESHeader.cpp PESHeader.h TSReport.cpp TSReport.h PacketFilter.cpp PacketFilter.h IFilter.h IPacket.h OutputOptions.cpp OutputOptions.h PMTPacket.cpp PMTPacket.h TSSource.h FileSource.cpp FileSource.h)
add_executable(tsparse ${SOURCE_FILES})
target_link_libraries(tsparse pthread)