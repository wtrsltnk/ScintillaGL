cmake_minimum_required(VERSION 3.12)

project(Demo VERSION "0.1.0")

add_executable(Demo WIN32)

target_sources(Demo
    PRIVATE
        "../external/glad.c"
        "DefaultFontData.cpp"
        "DefaultFontData.h"
        "Demo.cxx"
        "EditorEx.cpp"
        "EditorEx.hpp"
        "MenuLayer.cpp"
        "MenuLayer.hpp"
        "ScPlatform.cxx"
        "ShaderEditOverlay.cxx"
        "ShaderEditOverlay.h"
        "iscreenlayer.hpp"
        "screen-utils.hpp"
        "scrollbarlayer.cpp"
        "scrollbarlayer.hpp"
        "stb_truetype.h"
        "stbtt_font.hpp"
)

target_compile_features(Demo
    PRIVATE
        cxx_nullptr
        cxx_std_17
)

target_include_directories(Demo
    PRIVATE
        "include"
        "../external/include"
        "${PROJECT_BINARY_DIR}"
        "../scintilla"
        "../scintilla/lexlib"
)

target_link_libraries(Demo
    PRIVATE
        glm
        SDL2-static
        scintilla
        winmm.lib
        opengl32.lib
        imm32.lib
)
