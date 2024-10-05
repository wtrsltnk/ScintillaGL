cmake_minimum_required(VERSION 3.12)

include(cmake/CPM.cmake)

CPMAddPackage("gh:nlohmann/json@3.10.5")

CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 0.9.9.8
)

CPMAddPackage(
    NAME SDL2
    VERSION 2.0.22
    URL https://www.libsdl.org/release/SDL2-2.0.22.zip
    OPTIONS
        "SDL_SHARED Off"
)

CPMAddPackage(
    NAME mbedtls
    GITHUB_REPOSITORY Mbed-TLS/mbedtls
    GIT_TAG v3.6.1
)

CPMAddPackage(
    NAME tinycc
    GITHUB_REPOSITORY TinyCC/tinycc
    GIT_TAG release_0_9_27
    DOWNLOAD_ONLY YES
)

if (tinycc_ADDED)
    file(WRITE "${tinycc_BINARY_DIR}/config.h"
        "#define TCC_VERSION \"0.9.27\"\n"
    )

    add_library(tinycc
        "${tinycc_SOURCE_DIR}/libtcc.c"
        "${tinycc_SOURCE_DIR}/libtcc.h"
    )

    target_include_directories(tinycc
        PUBLIC
            "${tinycc_BINARY_DIR}"
            # "${tinycc_SOURCE_DIR}/include"
    )

endif()

project(ScintillaGL VERSION "0.1.0")

add_library(ScintillaGL INTERFACE
    README.md
    TODO.md
)

add_subdirectory(httpclient)
add_subdirectory(scintilla)
add_subdirectory(Demo)
