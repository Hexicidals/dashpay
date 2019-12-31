add_library(dashpay-core
    src/accounts.cpp
    src/config.cpp
    src/error.cpp
    src/instructions.cpp
    src/client/balance.cpp
    src/client/fetch.cpp
    src/client/mpp.cpp
    src/client/runner.cpp
    src/client/send.cpp
    src/client/x402.cpp
    src/client/session.cpp
    src/client/sandbox.cpp
)

target_include_directories(dashpay-core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/core/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/external/include
)

target_link_libraries(dashpay-core
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
)

if(BUILD_SERVER)
    target_sources(dashpay-core PRIVATE
        src/server/mod.cpp
        src/server/proxy.cpp
        src/server/payment.cpp
        src/server/session.cpp
        src/server/metering.cpp
        src/server/accounting.cpp
        src/server/telemetry.cpp
        src/server/openapi.cpp
    )
endif()
