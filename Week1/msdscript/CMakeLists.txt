cmake_minimum_required(VERSION 3.27)
project(msdscript)

set(CMAKE_CXX_STANDARD 17)

add_executable(msdscript main.cpp
        cmdline.h
        cmdline.cpp)
