cmake_minimum_required(VERSION 3.12)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME SDL2
    VERSION 2.0.14
    URL https://www.libsdl.org/release/SDL2-2.0.14.zip
    OPTIONS
        "SDL_SHARED Off"
)

project(ScintillaGL VERSION "0.1.0")

add_subdirectory(scintilla)
add_subdirectory(Demo)
