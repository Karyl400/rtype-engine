cmake_minimum_required(VERSION 3.25)
project(rtype_engine VERSION 0.1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")
include(Hardening)
include(cmake/backends/SelectBackend.cmake)

set(RTYPE_INCLUDE_DIRS
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)

add_subdirectory(src/engine)
