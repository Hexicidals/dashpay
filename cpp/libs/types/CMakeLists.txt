add_library(dashpay-types STATIC)

target_sources(dashpay-types PRIVATE
    src/util.cpp
)

target_include_directories(dashpay-types PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/types/include
)

target_link_libraries(dashpay-types PUBLIC)

install(TARGETS dashpay-types)
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
