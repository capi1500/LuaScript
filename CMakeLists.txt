cmake_minimum_required(VERSION 3.15)
project(LuaScript)

set(CMAKE_CXX_COMPILER_VERSION GCC7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${PROJECT_SOURCE_DIR}" include)

add_library(LuaScript STATIC Script.cpp Script.hpp)

target_link_libraries(LuaScript ${CMAKE_SOURCE_DIR}/lib/liblua53.so)