cmake_minimum_required(VERSION 3.12)

project(burg)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.c)

add_executable(burg ${SRC_FILES})
