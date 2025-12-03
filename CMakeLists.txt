cmake_minimum_required(VERSION 4.0.0)

include(cmake/init.cmake)
include(cmake/modules.cmake)

project(
    wacc
    VERSION 0.0.1
    LANGUAGES CXX
)

add_subdirectory(source)