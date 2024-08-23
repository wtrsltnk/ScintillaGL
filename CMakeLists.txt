cmake_minimum_required(VERSION 3.12)

include(cmake/CPM.cmake)

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

project(ScintillaGL VERSION "0.1.0")

add_library(ScintillaGL INTERFACE
    README.md
    TODO.md
)

add_subdirectory(scintilla)
add_subdirectory(Demo)
