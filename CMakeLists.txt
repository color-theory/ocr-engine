cmake_minimum_required(VERSION 3.10)

project(OCRProject)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")

set(SRC_DIR src)
set(INCLUDE_DIR include)
set(BUILD_DIR ${CMAKE_BINARY_DIR})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libs)

add_library(Preprocessor src/Preprocessor.cpp)
add_library(Segmenter src/Segmenter.cpp)
add_library(OCRImageProcessor SHARED src/OCRImageProcessor.cpp src/OCRExport.cpp)

target_link_libraries(Preprocessor ${OpenCV_LIBS})
target_link_libraries(Segmenter ${OpenCV_LIBS})
target_link_libraries(OCRImageProcessor Preprocessor Segmenter ${OpenCV_LIBS})

