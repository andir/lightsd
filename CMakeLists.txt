cmake_minimum_required(VERSION 3.5)
project(lightsd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O0 ")

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(Boost COMPONENTS system)
#find_package(yaml-Cpp)
include_directories(${Boost_INCLUDE_DIRS})


set(SOURCE_FILES main.cpp FrameScheduler.cpp test.cpp algorithm.cpp WebsocketOutput.h rgb.h hsv.cpp buffer.h algorithm.h SharedMemoryBuffer.cpp SharedMemoryBuffer.h MemFD.cpp MemFD.h MemMap.cpp MemMap.h Output.cpp Output.h)
add_executable(lightsd ${SOURCE_FILES})

target_link_libraries(lightsd pthread)
target_link_libraries(lightsd ${Boost_LIBRARIES})
#target_link_libraries(lightsd /usr/lib64/libboost_system.a)
target_link_libraries(lightsd yaml-cpp)
