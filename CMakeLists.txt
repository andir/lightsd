cmake_minimum_required(VERSION 3.6)
project(lightsd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O2 ")

find_package(Boost COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})


set(SOURCE_FILES main.cpp algorithm.cpp WebsocketOutput.h rgb.h hsv.cpp buffer.h algorithm.h SharedMemoryBuffer.cpp SharedMemoryBuffer.h MemFD.cpp MemFD.h MemMap.cpp MemMap.h Output.cpp Output.h)
add_executable(lightsd ${SOURCE_FILES})

target_link_libraries(lightsd pthread)
target_link_libraries(lightsd ${Boost_LIBRARIES})
