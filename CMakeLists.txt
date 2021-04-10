cmake_minimum_required(VERSION 3.16)
project(savtocsv)

set(CMAKE_C_STANDARD 11)

ADD_DEFINITIONS(-Wall -Wextra -Wconversion -pedantic -c -g -O0 -std=c99 -lm)

add_executable(
        savtocsv savtocsv.c savtocsvlib.c savtocsvcommon.c common.h)

target_link_libraries(savtocsv m)