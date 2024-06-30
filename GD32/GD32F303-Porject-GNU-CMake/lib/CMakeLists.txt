cmake_minimum_required(VERSION 3.22)

project(Proj_Lib)
add_library(${PROJECT_NAME} INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(${PROJECT_NAME} INTERFACE
		GD32F30X_HD
)

target_include_directories(${PROJECT_NAME} INTERFACE
        gd32/CMSIS
        gd32/CMSIS/GD/GD32F30x/Include
        gd32/GD32F30x_standard_peripheral/Include
)

file(GLOB_RECURSE SOURCES
		"gd32/CMSIS/*.c"
		"gd32/GD32F30x_standard_peripheral/*.c"
		"gd32/CMSIS/GD/GD32F30x/Source/GNU/startup_gd32f30x_hd.s"
)

target_sources(${PROJECT_NAME} INTERFACE
		${SOURCES}
)

target_link_directories(${PROJECT_NAME} INTERFACE

)

target_link_libraries(${PROJECT_NAME} INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


