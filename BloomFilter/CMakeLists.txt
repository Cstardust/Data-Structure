cmake_minimum_required(VERSION 3.21)
project(BloomFilter)

set(CMAKE_CXX_STANDARD 14)

add_executable(BloomFilter BloomFilter.cpp stringHash.h)
