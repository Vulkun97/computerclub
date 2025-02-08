cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED True)

project(club_log VERSION 1.0)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(club_log ${SRC_FILES})
add_compile_options(-std==c++20)
target_compile_features(club_log PRIVATE cxx_std_20)
target_include_directories(club_log PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)