cmake_minimum_required(VERSION 3.5)
project(lightsd)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "andreas@rammhold.de")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(cotire)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O3 -march=native -Wall")

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(Boost COMPONENTS system thread)
#find_package(yaml-Cpp)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(mqtt_client_cpp/include)
include_directories(src)

file(GLOB_RECURSE SOURCE_FILES src/**.cpp src/**.h src/**.hpp)

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
