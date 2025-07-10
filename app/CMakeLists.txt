cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(slam-module)

project(app LANGUAGES C)

file(GLOB SLAM_MODULE_SOURCES "src/*.c")

target_sources(app PRIVATE ${SLAM_MODULE_SOURCES})
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link Fusion library (headers are automatically included)
target_link_libraries(app PRIVATE Fusion)

# Add Xtensa-specific compiler flags to Fusion library to handle call range issues
if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_SYSTEM_PROCESSOR MATCHES "xtensa")
    target_compile_options(Fusion PRIVATE 
        -mtext-section-literals
        -mlongcalls
    )
endif()
