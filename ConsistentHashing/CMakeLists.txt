cmake_minimum_required(VERSION 3.21)
project(ConsistentHash)

set(CMAKE_CXX_STANDARD 14)

include_directories(./)

add_executable(ConsistentHash main.cpp md5.cc)
