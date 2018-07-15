cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(lcc_cv)
include_directories(
  ${CMAKE_SOURCE_DIR}
)

#add_subdirectory(common)
#add_subdirectory(filter)
#add_subdirectory(edge)
#add_subdirectory(segmentation)
#add_subdirectory(fitting)
add_subdirectory(test)
