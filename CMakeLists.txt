cmake_minimum_required(VERSION 3.15)
project(oskp)

set(CMAKE_CXX_STANDARD 14)

add_executable(oskp main.cpp N2Allocator.h ListAllocator.h ListAllocator.cpp N2Allocator.cpp profile.h)