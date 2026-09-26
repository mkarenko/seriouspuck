cmake_minimum_required(VERSION 3.16)
project(seriouspuck_core LANGUAGES CXX)

add_library(seriouspuck_core
    firmware/common/src/protocol.cpp
    firmware/rp2040/src/mapper.cpp)
target_include_directories(seriouspuck_core PUBLIC firmware/common/include firmware/rp2040/include)
target_compile_features(seriouspuck_core PUBLIC cxx_std_17)
target_compile_options(seriouspuck_core PRIVATE -Wall -Wextra -Wpedantic)
