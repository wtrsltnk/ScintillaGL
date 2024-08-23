cmake_minimum_required(VERSION 3.12)

project(Demo VERSION "0.1.0")

add_executable(Demo WIN32)

target_sources(Demo
    PRIVATE
        "Demo.cxx"
        "../external/glad.c"
        "DefaultFontData.cpp"
        "ScPlatform.cxx"
        "ShaderEditOverlay.cxx"
        "DefaultFontData.h"
        "ShaderEditOverlay.h"
        "EditorEx.cpp"
        "EditorEx.hpp"
        "stb_truetype.h"
        "stbtt_font.hpp"
        "MenuLayer.cpp"
        "MenuLayer.hpp"
        "iscreenlayer.hpp"
        "screen-utils.hpp"
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
