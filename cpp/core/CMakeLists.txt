add_library(dashpay-core STATIC)

target_sources(dashpay-core PRIVATE
    src/error.cpp
    src/config.cpp
    src/url.cpp
    src/encoding.cpp
)

target_include_directories(dashpay-core PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/core/include
)

target_link_libraries(dashpay-core PUBLIC
    OpenSSL::SSL
    OpenSSL::Crypto
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(WITH_SERVER)
    target_sources(dashpay-core PUBLIC
        src/accounts.cpp
        src/api.cpp
        src/persistence.cpp
        src/crypto.cpp
        src/mpp.cpp
        src/x402.cpp
        src/session.cpp
        src/rpc.cpp
    )

    target_link_libraries(dashpay-core PUBLIC
        Threads::Threads
    )

    add_subdirectory(server)
endif()

install(TARGETS dashpay-core)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
