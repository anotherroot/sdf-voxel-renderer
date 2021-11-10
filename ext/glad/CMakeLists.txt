cmake_minimum_required(VERSION 3.17)

add_library(glad STATIC)
target_sources(glad
        PUBLIC
        src/glad.c
        )
target_include_directories(glad PUBLIC include)
