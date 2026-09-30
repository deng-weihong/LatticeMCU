cmake_minimum_required(VERSION 3.16)

project(LatticeMCU LANGUAGES C)

add_library(lmcu STATIC
    src/kernel/lmcu_scheduler.c
)

target_include_directories(lmcu
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(lmcu PUBLIC c_std_99)

if(MSVC)
    target_compile_options(lmcu PRIVATE /W4)
else()
    target_compile_options(lmcu PRIVATE -Wall -Wextra -Wpedantic)
endif()
