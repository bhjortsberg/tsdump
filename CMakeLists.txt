cmake_minimum_required(VERSION 3.0)
project(tsdump)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(SOURCE_FILES main.cpp TransportStream.cpp TransportStream.h TSPacket.cpp TSPacket.h AdaptationField.cpp AdaptationField.h PESHeader.cpp PESHeader.h TSReport.cpp TSReport.h PacketFilter.cpp PacketFilter.h IFilter.h IPacket.h OutputOptions.cpp OutputOptions.h PMTPacket.cpp PMTPacket.h TSSource.h FileSource.cpp FileSource.h MulticastSource.cpp MulticastSource.h SourceFactory.cpp SourceFactory.h TSSourceAbstract.cpp TSSourceAbstract.h)
add_executable(tsdump ${SOURCE_FILES})
target_link_libraries(tsdump pthread)
