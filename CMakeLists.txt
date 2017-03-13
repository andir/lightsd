cmake_minimum_required(VERSION 3.5)
project(lightsd)

include(cotire)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O0 -ggdb -march=native ")

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(Boost COMPONENTS system)
#find_package(yaml-Cpp)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(mqtt_client_cpp/include)


set(SOURCE_FILES main.cpp FrameScheduler.cpp algorithm.cpp WebsocketOutput.h
        rgb.h hsv.cpp Buffer.h algorithm.h SharedMemoryBuffer.cpp SharedMemoryBuffer.h
        utils/MemFD.cpp utils/MemFD.h utils/MemMap.cpp utils/MemMap.h Output.cpp Output.h
        VariableStore/ValueType.cpp
        VariableStore/VariableStore.cpp
        VariableStore/InvalidVariableTypeException.cpp
        MqttVarStore.cpp
        Operation.cpp
        UDPOutputWrapper.cpp
        UDPOutput.cpp UDPOutput.h
        WebsocketOutputWrapper.cpp
        BellOperation.cpp BellOperation.h
        config.cpp
        utils/util.cpp utils/util.h
        RainbowOperation.cpp
        RotateOperation.cpp
        WorkerThread.cpp
        SharedMemoryOutput.cpp SharedMemoryOutput.h
        RaindropOperation.cpp RaindropOperation.h SharedMemoryOutput.cpp SharedMemoryOutput.h Operation.cpp BellOperation.h BellOperation.cpp SplashdropOperation.h SplashdropOperation.cpp HSVUDPSink.h
        HSVUDPSink.cpp HSVUDPSink.h
        RGBUDPSink.cpp RGBUDPSink.h
        HSVUDPOutput.h HSVUDPOutput.cpp
        HSVUDPInputOperation.h HSVUDPInputOperation.cpp)

add_executable(lightsd ${SOURCE_FILES})

target_link_libraries(lightsd m rt)
target_link_libraries(lightsd pthread)
target_link_libraries(lightsd crypto)
target_link_libraries(lightsd ssl)
target_link_libraries(lightsd ${Boost_LIBRARIES})
#target_link_libraries(lightsd /usr/lib64/libboost_system.a)
target_link_libraries(lightsd yaml-cpp)

cotire(lightsd)

