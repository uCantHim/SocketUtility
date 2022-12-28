cmake_minimum_required(VERSION 3.13)
project(SUC VERSION 0.1)

if (UNIX AND NOT APPLE)
    set (LINUX TRUE)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(suc)

add_subdirectory(src)
add_subdirectory(test)

target_include_directories(
    suc
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include/
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include/suc/
)
target_compile_features(suc PUBLIC cxx_std_20)
target_compile_options(
    suc
    PRIVATE
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
)
