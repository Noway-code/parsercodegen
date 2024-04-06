cmake_minimum_required(VERSION 3.27)
project(hw3/parsercodegen C)

set(CMAKE_C_STANDARD 11)

add_executable(hw3/parsercodegen
        hw3/parsercodegen.c
        hw4/hw4compiler.c)
