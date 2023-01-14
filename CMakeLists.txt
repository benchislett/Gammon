cmake_minimum_required(VERSION 3.16)
SET(CMAKE_BUILD_TYPE Debug)
project(main)

# Include project src files
include_directories("include/")

# Add your project files
file(GLOB SRC_FILES *.cpp src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES} main.cpp)

# target_compile_options(${PROJECT_NAME} PUBLIC "-fno-omit-frame-pointer" "-g")
