cmake_minimum_required(VERSION 3.27.3)
project(lab_03 LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    Boost
    REQUIRED
)
include_directories( ${Boost_INCLUDE_DIR} )

add_executable(app main.cpp)

option(BUILD_TESTS "Build Tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(lib)

target_link_libraries(app lib ${Boost_LIBRARIES})
