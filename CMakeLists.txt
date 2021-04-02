cmake_minimum_required(VERSION 3.16)
project(savtocsv)

set(CMAKE_C_STANDARD 11)

ADD_DEFINITIONS(-Wall -Wextra -Wconversion -pedantic -c -g -O3 -std=gnu99)

add_executable(
        savtocsv savtocsv.c savtocsvlib.c savtocsvcommon.c common.h)
