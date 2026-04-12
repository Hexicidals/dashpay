cmake_minimum_required(VERSION 3.20)
project(dashpay VERSION 0.16.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Find dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Options
option(BUILD_SERVER "Build payment server components" ON)
option(BUILD_TESTS "Build tests" ON)

# Core library
add_subdirectory(libs/core)

# Keystore library
add_subdirectory(libs/keystore)

# Payment types library
add_subdirectory(libs/types)

# MCP server
add_subdirectory(libs/mcp)

# PDB (Payment Debugger)
if(BUILD_SERVER)
    add_subdirectory(libs/pdb)
endif()

# CLI executable
add_subdirectory(apps/cli)

# Enable testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS dashpay-cli DESTINATION bin)
