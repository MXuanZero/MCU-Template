cmake_minimum_required(VERSION 3.22)

project(Proj_Lib)
add_library(${PROJECT_NAME} INTERFACE)

set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")

add_subdirectory(gd32)
add_subdirectory(threadx)

target_link_libraries(${PROJECT_NAME} INTERFACE GD32_Lib threadx)

