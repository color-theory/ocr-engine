cmake_minimum_required(VERSION 3.10)

project(OCRProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SRC_DIR src)
set(BUILD_DIR ${CMAKE_BINARY_DIR})

file(GLOB SRCS "${SRC_DIR}/*.cpp")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(ocr ${SRCS})

target_link_libraries(ocr ${OpenCV_LIBS})

set_target_properties(ocr PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
)

