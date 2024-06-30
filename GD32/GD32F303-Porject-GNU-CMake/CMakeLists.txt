set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)

cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

set(CMAKE_PROJECT_NAME GD32F303-Project)

include("cmake/gcc-arm-none-eabi.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

enable_language(C ASM)

project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

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
