project(test_main)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(OpenCV_Root /home/lichongchong/my_soft/opencv249/opencv249)
set(OpenCV_INCLUDE_DIRS ${OpenCV_Root}/include)
set(OpenCV_LIBRARY_DIRS ${OpenCV_Root}/lib)
file(GLOB OpenCV_LIBS ${OpenCV_Root}/lib/*.so)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${OpenCV_INCLUDE_DIRS}
)
Message (status "Opecv include dir: " ${OpenCV_INCLUDE_DIRS})
link_directories(
  ${OpenCV_LIBRARY_DIRS}
)
add_executable(test_main test_main.cc)
target_link_libraries(test_main
  ${OpenCV_LIBS}
)
