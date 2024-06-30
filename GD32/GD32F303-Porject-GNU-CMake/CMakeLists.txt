set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("cmake/gcc-arm-none-eabi.cmake")

project(GD32F303-Project C CXX ASM)
set(CMAKE_PROJECT_NAME ${PROJECT_NAME})

add_executable(${CMAKE_PROJECT_NAME})

add_subdirectory(lib)
add_subdirectory(src)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
        Proj_Src
        Proj_Lib
)

add_custom_command(
        POST_BUILD
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf
        COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
        VERBATIM
)
