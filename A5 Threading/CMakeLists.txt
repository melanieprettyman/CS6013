cmake_minimum_required(VERSION 3.27)
project(A5_Threading)

set(CMAKE_CXX_STANDARD 17)

add_executable(A5_Threading
        SerialQueue.hpp
        SerialQueueTests.cpp
        ConcurrentQueue.hpp
        ConcurrentQueueTests.cpp)
