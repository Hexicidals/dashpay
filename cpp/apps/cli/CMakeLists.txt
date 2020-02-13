cmake_minimum_required(VERSION 3.20)
find_package(PkgConfig REQUIRED Threads REQUIRED fmt REQUIRED range-v3 REQUIRED)

add_executable(dashpay-cli)

target_sources(dashpay-cli PRIVATE
    src/main.cpp
)

target_include_directories(dashpay-cli PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/cli/include
    ${CMAKE_SOURCE_DIR}/libs/core/include
    ${CMAKE_SOURCE_DIR}/libs/keystore/include
)

target_link_libraries(dashpay-cli PRIVATE
    Threads::Threads
    fmt::fmt
)

if(WIN32)
    target_compile_definitions(dashpay-cli PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    target_link_libraries(dashpay-cli PRIVATE
        ws2_32
    )
endif()

if(APPLE)
    target_link_libraries(dashpay-cli PRIVATE
        "-framework Foundation"
        "-framework Security"
    )
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS dashpay-cli DESTINATION ${CMAKE_INSTALL_BINDIR})
