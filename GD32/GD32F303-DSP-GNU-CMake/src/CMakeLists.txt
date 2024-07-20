cmake_minimum_required(VERSION 3.22)

project(Proj_Src)
add_library(${PROJECT_NAME} INTERFACE)

enable_language(C CXX ASM)

target_compile_definitions(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE app bsp core dbg)

file(GLOB_RECURSE APP_FILE "app/*.c")
file(GLOB_RECURSE BSP_FILE "bsp/*.c")
file(GLOB_RECURSE CORE_FILE "core/*.c")
file(GLOB_RECURSE DBG_FILE "dbg/*.c")

target_sources(${PROJECT_NAME} INTERFACE ${APP_FILE} ${BSP_FILE} ${CORE_FILE} ${DBG_FILE})

target_link_directories(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE)

# Validate that STM32CubeMX code is compatible with C standard
if (CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif ()


