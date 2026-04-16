cmake_minimum_required(VERSION 3.20)
find_package(PkgConfig REQUIRED jsoncpp)
find_package(PkgConfig REQUIRED nlohmann_json)

add_library(dashpay-mcp STATIC)

target_sources(dashpay-mcp PRIVATE
    src/server.cpp
)

target_include_directories(dashpay-mcp PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/mcp/include
    ${JSONCPP_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIRS}
)

target_link_libraries(dashpay-mcp PRIVATE
    ${JSONCPP_LIBRARIES}
    ${NLOHMANN_JSON_LIBRARIES}
)

# Build executable for testing
add_executable(dashpay-mcp-test tests/test_mcp.cpp)
target_link_libraries(dashpay-mcp-test PRIVATE dashpay-mcp)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS dashpay-mcp)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}

# Expose to CLI for linking
install(TARGETS dashpay-mcp)
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
