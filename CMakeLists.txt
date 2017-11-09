cmake_minimum_required(VERSION 3.5)
project(lightsd)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT "andreas@rammhold.de")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

#include(cotire)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -march=native -Wall")

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBRARIES OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(PkgConfig)
pkg_search_module(luajit REQUIRED luajit)
pkg_search_module(yaml-cpp REQUIRED yaml-cpp)
find_package(Boost COMPONENTS system thread)
find_package(websocketpp)

file(GLOB_RECURSE SOURCE_FILES src/**.cpp src/**.h src/**.hpp)

add_executable(lightsd ${SOURCE_FILES})
install(TARGETS lightsd DESTINATION bin)

target_include_directories(lightsd PRIVATE ${luajit_INCLUDE_DIRS})
target_include_directories(lightsd PRIVATE ${yaml-cpp_INCLUDE_DIRS})
target_include_directories(lightsd PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(lightsd PRIVATE mqtt_client_cpp/include)
target_include_directories(lightsd PRIVATE ${luajit_INCLUDE_DIRS})
target_include_directories(lightsd PRIVATE src)

target_link_libraries(lightsd m rt)
target_link_libraries(lightsd pthread)
target_link_libraries(lightsd crypto)
target_link_libraries(lightsd ssl)
target_link_libraries(lightsd ${luajit_LIBRARIES})
target_link_libraries(lightsd ${Boost_LIBRARIES})
target_link_libraries(lightsd yaml-cpp)

#cotire(lightsd)

include(CPack)

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
