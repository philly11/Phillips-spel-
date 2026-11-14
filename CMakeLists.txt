
cmake_minimum_required(VERSION 3.24)

project(phillipSpel LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Raylib version
set(RAYLIB_VERSION 6.0)

# Download raylib automatically
FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
)

FetchContent_MakeAvailable(raylib)

# Game executable
add_executable(${PROJECT_NAME})

add_subdirectory(src)

# Output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
