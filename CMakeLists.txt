cmake_minimum_required(VERSION 3.16)
SET(CMAKE_BUILD_TYPE Debug)
project(main)

# # Dependencies
include(FetchContent)

find_package(Torch REQUIRED)

FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

# # Misc flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 20)

include_directories("include/")

file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)
file(GLOB SCRIPTS RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

add_library(GammonCore ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(GammonCore PUBLIC "${TORCH_LIBRARIES}")

foreach(scriptfile ${SCRIPTS})
        string(REPLACE ".cpp" "" scriptname ${scriptfile})
        add_executable(${scriptname} ${scriptfile})

        target_link_libraries(${scriptname} GammonCore argparse)
endforeach(scriptfile ${SCRIPTS})
