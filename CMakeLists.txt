cmake_minimum_required(VERSION 3.5)
project(lightsd)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "andreas@rammhold.de")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(cotire)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O3 -ggdb -march=native -Wall")

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(Boost COMPONENTS system)
#find_package(yaml-Cpp)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(mqtt_client_cpp/include)


set(SOURCE_FILES main.cpp FrameScheduler.cpp algorithm.cpp 
        rgb.h rgb.cpp
        hsv.h hsv.cpp
        Buffer.h
        algorithm.h
        SharedMemoryBuffer.cpp SharedMemoryBuffer.h
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
        operations/lua/LuaOperation.cpp
        operations/lua/LuaOperation.h
        operations/lua/LuaHSVBuffer.cpp operations/lua/LuaHSVBuffer.h
        operations/GameOfLifeOperation.cpp operations/GameOfLifeOperation.h

        net/HSVUDPSink.cpp net/HSVUDPSink.h
        net/RGBUDPSink.cpp net/RGBUDPSink.h

        utils/random.h
        utils/luna.h

        outputs/spi/SPI.cpp outputs/spi/SPI.h outputs/spi/SPIOutput.cpp outputs/spi/SPIOutput.h
)

add_executable(lightsd ${SOURCE_FILES})
install(TARGETS lightsd DESTINATION bin)

target_link_libraries(lightsd m rt)
target_link_libraries(lightsd pthread)
target_link_libraries(lightsd crypto)
target_link_libraries(lightsd ssl)
target_link_libraries(lightsd luajit-5.1)
target_link_libraries(lightsd ${Boost_LIBRARIES})
#target_link_libraries(lightsd /usr/lib64/libboost_system.a)
target_link_libraries(lightsd yaml-cpp)

cotire(lightsd)

include(CPack)
