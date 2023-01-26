cmake_minimum_required(VERSION 3.16)
SET(CMAKE_BUILD_TYPE Release)
project(main)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 20)

include_directories("include/")

file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)
file(GLOB SCRIPTS RELATIVE ${PROJECT_SOURCE_DIR} *.cpp)

add_library(GammonCore ${SRC_FILES} ${HEADER_FILES})
target_link_libraries(GammonCore PUBLIC "${TORCH_LIBRARIES}")

# set_property(TARGET GammonCore PROPERTY CXX_STANDARD 17)
message(STATUS "Scripts: ${SCRIPTS}")

foreach(scriptfile ${SCRIPTS})
        string(REPLACE ".cpp" "" scriptname ${scriptfile})
        add_executable(${scriptname} ${scriptfile})

        target_link_libraries(${scriptname} GammonCore)
endforeach(scriptfile ${SCRIPTS})
