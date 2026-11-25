
cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable fmt consteval for compatibility
set(GEODE_DISABLE_FMT_CONSTEVAL ON CACHE BOOL "" FORCE)

project(OffscreenCuller VERSION 1.0.0)

# Mod source
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

# Check Geode SDK
if(NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR
        "Unable to find Geode SDK! Please define GEODE_SDK environment variable."
    )
else()
    message(STATUS "Found Geode SDK: $ENV{GEODE_SDK}")
endif()

# Load Geode SDK
add_subdirectory(
    "$ENV{GEODE_SDK}"
    "${CMAKE_CURRENT_BINARY_DIR}/geode"
)

# Configure Geode mod
setup_geode_mod(${PROJECT_NAME})
