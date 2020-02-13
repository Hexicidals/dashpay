cmake_minimum_required(VERSION 3.16)
find_package(PkgConfig REQUIRED libsecret-1)

add_library(dashpay-keystore STATIC)

target_sources(dashpay-keystore PRIVATE
    src/apple_keychain.cpp
    src/windows_hello.cpp
    src/gnome_keyring.cpp
    src/onepassword.cpp
)

target_sources(dashpay-keystore PRIVATE APPLE_KEYCHAIN
    src/apple_keychain.cpp
)

target_sources(dashpay-keystore PRIVATE WINDOWS_HELLO
    src/windows_hello.cpp
)

target_sources(dashpay-keystore PRIVATE LINUX_GNOME_KEYRING
    src/gnome_keyring.cpp
)

target_include_directories(dashpay-keystore PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/keystore/include
    ${SECRET-1_INCLUDE_DIRS}
)

target_link_libraries(dashpay-keystore PRIVATE
    ${SECRET-1_LIBRARIES}
)

if(NOT TARGET_NAME)
    add_subdirectory(src)
endif()

install(TARGETS dashpay-keystore
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
