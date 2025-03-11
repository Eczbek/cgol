cmake_minimum_required(VERSION 3.25)

project(gt)
include(FetchContent)

add_executable(${PROJECT_NAME} main2.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion)

FetchContent_Declare(xieite GIT_REPOSITORY "https://github.com/Eczbek/xieite" GIT_TAG "5231b78638ab5c9b7c8869a22acfb5f8ea4cca61")
FetchContent_MakeAvailable(xieite)
target_link_libraries(${PROJECT_NAME} PRIVATE xieite)
