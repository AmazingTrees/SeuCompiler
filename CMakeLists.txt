cmake_minimum_required(VERSION 2.8)

project(SeuCompiler)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -DDEBUG")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Werror -DDEBUG")

