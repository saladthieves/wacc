cmake_minimum_required(VERSION 4.0.0)

include(cmake/init.cmake)
include(cmake/modules.cmake)

project(
    wacc
    VERSION 0.0.1
    LANGUAGES CXX
)

add_subdirectory(source/util)
add_subdirectory(source/frontend)
add_subdirectory(source/backend)
add_subdirectory(source/driver)
add_subdirectory(source)

add_subdirectory(test)