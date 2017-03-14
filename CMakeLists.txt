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


set(SOURCE_FILES main.cpp FrameScheduler.cpp algorithm.cpp 
        rgb.h hsv.cpp Buffer.h algorithm.h SharedMemoryBuffer.cpp SharedMemoryBuffer.h
        utils/MemFD.cpp utils/MemFD.h utils/MemMap.cpp utils/MemMap.h
        VariableStore/ValueType.cpp
        VariableStore/VariableStore.cpp
        VariableStore/InvalidVariableTypeException.cpp
        net/MqttConnection.cpp net/MqttConnection.h
        
        config.cpp

        utils/util.cpp utils/util.h
        operations/Operation.cpp operations/Operation.h
        operations/BellOperation.cpp operations/BellOperation.h
        operations/RainbowOperation.cpp operations/RainbowOperation.h
        operations/RotateOperation.cpp operations/RotateOperation.h
        operations/RaindropOperation.cpp operations/RaindropOperation.h
        operations/BellOperation.h operations/BellOperation.cpp
        operations/SplashdropOperation.h operations/SplashdropOperation.cpp
        operations/HSVUDPInputOperation.h operations/HSVUDPInputOperation.cpp
        operations/ShadeOperation.h
        
        WorkerThread.cpp

        outputs/Output.cpp outputs/Output.h
        outputs/UDPOutput.cpp outputs/UDPOutput.h
        outputs/UDPOutputWrapper.cpp outputs/UDPOutputWrapper.h
        outputs/SharedMemoryOutput.cpp outputs/SharedMemoryOutput.h
        outputs/HSVUDPOutput.h outputs/HSVUDPOutput.cpp
        outputs/WebsocketOutputWrapper.cpp outputs/WebsocketOutputWrapper.h
        outputs/WebsocketOutput.h 
        
        net/HSVUDPSink.cpp net/HSVUDPSink.h
        net/RGBUDPSink.cpp net/RGBUDPSink.h
        )

add_executable(lightsd ${SOURCE_FILES})

target_link_libraries(lightsd m rt)
target_link_libraries(lightsd pthread)
target_link_libraries(lightsd crypto)
target_link_libraries(lightsd ssl)
target_link_libraries(lightsd ${Boost_LIBRARIES})
#target_link_libraries(lightsd /usr/lib64/libboost_system.a)
target_link_libraries(lightsd yaml-cpp)

cotire(lightsd)

