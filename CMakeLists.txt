cmake_minimum_required(VERSION 3.16)
project(savtocsv C)

set(CMAKE_C_STANDARD 11)

include_directories(head)

add_executable(
        savtocsvcommon.h
        savtocsvlib.h
        savtocsvcommon.c
        savtocsvlib.c
        savtocsv.c common.h)
