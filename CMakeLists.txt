cmake_minimum_required(VERSION 3.10)
project(pumpkin-api-c C)

set(CMAKE_C_STANDARD 11)

# Add the generated bindings
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/gen)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(pumpkin-api STATIC
    src/gen/plugin.c
    src/pumpkin_api.c
)
