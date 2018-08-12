cmake_minimum_required(VERSION 3.1)

set(CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

add_executable(get_version get_version.cpp)
add_executable(tag_sectors tag_sectors.cpp random.h)
